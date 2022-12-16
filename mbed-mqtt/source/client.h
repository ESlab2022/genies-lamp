#include "mbed.h"
// #include <cstdio>
// #include <stdlib.h>
// #include <string.h>

// To connect to internet with WiFi
#include "stm32_hal_legacy.h"
#include "wifi_helper.h"

// For MQTT client
#include <MQTTClientMbedOs.h>

#define LAMP_ID 0
#define PERIOD 0.01f

extern PwmOut led;
extern InterruptIn button;

extern void messageArrived(MQTT::MessageData &md);
extern void pwmArrived(MQTT::MessageData &md);

class Client {
    static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 10;
    static constexpr size_t REMOTE_PORT = 1883;  // standard MQTT port

   public:
    Client() : _net(NetworkInterface::get_default_instance()) {}

    ~Client() {
        if (_net) {
            _net->disconnect();
        }
        if (_mqtt_client) {
            _mqtt_client->disconnect();
        }
    }

    void init_WiFi() {
        if (!_net) {
            printf("Error! No network interface found.\r\n");
            return;
        }

        /* if we're using a wifi interface run a quick scan */
        if (_net->wifiInterface()) {
            /* the scan is not required to connect and only serves to show visible access points */
            wifi_scan();

            /* in this example we use credentials configured at compile time which are used by
             * NetworkInterface::connect() but it's possible to do this at runtime by using the
             * WiFiInterface::connect() which takes these parameters as arguments */
        }

        /* connect will perform the action appropriate to the interface type to connect to the
         * network */

        printf("Connecting to the network...\r\n");

        nsapi_size_or_error_t result = _net->connect();
        if (result != 0) {
            printf("Error! _net->connect() returned: %d\r\n", result);
            return;
        }

        print_network_info();
    }

    void init_MQTT() {
        _mqtt_client = new MQTTClient(&_socket);

        SocketAddress address;

        if (!resolve_hostname(address)) {
            printf("Failed to resolve hostname!\n");
            return;
        }

        address.set_port(REMOTE_PORT);

        _socket.open(_net);
        _socket.connect(address);

        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        data.MQTTVersion = 3;

        char *clientID = new char[10];
        sprintf(clientID, "lamp%d", LAMP_ID);
        data.clientID.cstring = clientID;

        _mqtt_client->connect(data);
        return;
    }

    void reconnect() {
        free(_mqtt_client);
        _socket.close();

        nsapi_connection_status_t net_status = _net->get_connection_status();
        nsapi_connection_status_t wifi_status = _wifi->get_connection_status();
        if (net_status < 2 || wifi_status < 2) {
            init_WiFi();
        }

        init_MQTT();
        printf("Reconnected\n");
    }

    void publish(const char *topic, const char *payload_buf) {
        MQTT::Message message;

        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void *)payload_buf;
        message.payloadlen = strlen(payload_buf);

        char buf[100];
        sprintf(buf, "/lamp%d/%s", LAMP_ID, topic);
        nsapi_error_t result = _mqtt_client->publish(buf, message);
        // printf("%s: %s\n", buf, message.payload);

        if (result != 0) {
            printf("Error! client.publish returned: %d\r\n", result);
            reconnect();
            return;
        }

        // printf("Message sent to %s:%d\r\n", MBED_CONF_APP_HOSTNAME ,REMOTE_PORT);
        return;
    }

    void subscribe(const char *topic) {
        printf("Subscribe to %s\n", topic);
        _mqtt_client->subscribe(topic, MQTT::QOS0, messageArrived);
    }

    void subscribe_pwm() {
        char *buf = new char[100];
        sprintf(buf, "/lamp%d/brightness", LAMP_ID);
        printf("Subscribe to pwm %s\n", buf);
        _mqtt_client->subscribe(buf, MQTT::QOS0, pwmArrived);
    }

    void unscbscribe(const char *topic) { _mqtt_client->unsubscribe(topic); }

    void yield(const float yeild_time) {
        nsapi_error_t result = _mqtt_client->yield(yeild_time); 
        if (result != 0) {
            printf("Error! client.yield returned: %d\r\n", result);
            reconnect();
            return;
        }
    }

    bool isConnected() { return _mqtt_client->isConnected(); }

   private:
    void wifi_scan() {
        _wifi = _net->wifiInterface();

        WiFiAccessPoint ap[MAX_NUMBER_OF_ACCESS_POINTS];

        /* scan call returns number of access points found */
        int result = _wifi->scan(ap, MAX_NUMBER_OF_ACCESS_POINTS);

        if (result <= 0) {
            printf("WiFiInterface::scan() failed with return value: %d\r\n", result);
            return;
        }

        printf("%d networks available:\r\n", result);

        for (int i = 0; i < result; i++) {
            printf(
                "Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: "
                "%hhd\r\n",
                ap[i].get_ssid(), get_security_string(ap[i].get_security()), ap[i].get_bssid()[0],
                ap[i].get_bssid()[1], ap[i].get_bssid()[2], ap[i].get_bssid()[3],
                ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
        }
        printf("\r\n");
    }

    void print_network_info() {
        /* print the network info */
        SocketAddress a;
        _net->get_ip_address(&a);
        printf("IP address: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_netmask(&a);
        printf("Netmask: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_gateway(&a);
        printf("Gateway: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    }

    bool resolve_hostname(SocketAddress &address) {
        const char hostname[] = MBED_CONF_APP_HOSTNAME;

        /* get the host address */
        printf("\nResolve hostname %s\r\n", hostname);
        nsapi_size_or_error_t result = _net->gethostbyname(hostname, &address);
        if (result != 0) {
            printf("Error! gethostbyname(%s) returned: %d\r\n", hostname, result);
            return false;
        }

        printf("%s address is %s\r\n", hostname,
               (address.get_ip_address() ? address.get_ip_address() : "None"));

        return true;
    }

   private:
    NetworkInterface *_net;
    WiFiInterface* _wifi;
    TCPSocket _socket;
    MQTTClient *_mqtt_client;
};
