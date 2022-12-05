/* Sockets Example
 * Copyright (c) 2016-2020 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include <cstdio>

// To connect to internet with WiFi
#include "stm32_hal_legacy.h"
#include "wifi_helper.h"

// For sensors
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

// For MQTT client
#include <MQTTClientMbedOs.h>


void messageArrived(MQTT::MessageData &md)
{
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\r\n", message.payloadlen, (char *)message.payload);
}


class Client {
    static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 10;
    static constexpr size_t REMOTE_PORT = 1883; // standard MQTT port

public:
    Client() : _net(NetworkInterface::get_default_instance())
    {
    }

    ~Client()
    {
        if (_net) {
            _net->disconnect();
        }
        if (_mqtt_client) {
            _mqtt_client->disconnect();
        }
    }

    void init_WiFi()
    {
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

        /* connect will perform the action appropriate to the interface type to connect to the network */

        printf("Connecting to the network...\r\n");

        nsapi_size_or_error_t result = _net->connect();
        if (result != 0) {
            printf("Error! _net->connect() returned: %d\r\n", result);
            return;
        }

        print_network_info();
    }

    void init_MQTT()
    {
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
        data.clientID.cstring = (char *)"MQTT_CONNECT";

        _mqtt_client->connect(data);    
        return;
    }

    void publish(const char * topic, const char * payload_buf)
    {
        MQTT::Message message;

        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void*)payload_buf;
        message.payloadlen = strlen(payload_buf);

        nsapi_error_t result = _mqtt_client->publish(topic, message);

        if (result != 0) {
            printf("Error! client.publish returned: %d\r\n", result);
            return;
        }
        
        // printf("Message sent to %s:%d\r\n", MBED_CONF_APP_HOSTNAME ,REMOTE_PORT);
        return;
    }

    void subscribe(const char * topic)
    {
        _mqtt_client->subscribe(topic, MQTT::QOS0, messageArrived);
    }

    void unscbscribe(const char * topic)
    {
        _mqtt_client->unsubscribe(topic);
    }


    void yield(const float yeild_time)
    {
        _mqtt_client->yield(yeild_time);
    }

    bool isConnected()
    {
        return _mqtt_client->isConnected();
    }


private:
    void wifi_scan()
    {
        WiFiInterface *wifi = _net->wifiInterface();

        WiFiAccessPoint ap[MAX_NUMBER_OF_ACCESS_POINTS];

        /* scan call returns number of access points found */
        int result = wifi->scan(ap, MAX_NUMBER_OF_ACCESS_POINTS);

        if (result <= 0) {
            printf("WiFiInterface::scan() failed with return value: %d\r\n", result);
            return;
        }

        printf("%d networks available:\r\n", result);

        for (int i = 0; i < result; i++) {
            printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\r\n",
                   ap[i].get_ssid(), get_security_string(ap[i].get_security()),
                   ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
                   ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5],
                   ap[i].get_rssi(), ap[i].get_channel());
        }
        printf("\r\n");
    }

    void print_network_info()
    {
        /* print the network info */
        SocketAddress a;
        _net->get_ip_address(&a);
        printf("IP address: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_netmask(&a);
        printf("Netmask: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_gateway(&a);
        printf("Gateway: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    }
        
    bool resolve_hostname(SocketAddress &address)
    {
        const char hostname[] = MBED_CONF_APP_HOSTNAME;

        /* get the host address */
        printf("\nResolve hostname %s\r\n", hostname);
        nsapi_size_or_error_t result = _net->gethostbyname(hostname, &address);
        if (result != 0) {
            printf("Error! gethostbyname(%s) returned: %d\r\n", hostname, result);
            return false;
        }

        printf("%s address is %s\r\n", hostname, (address.get_ip_address() ? address.get_ip_address() : "None") );

        return true;
    }

    
private:
    NetworkInterface *_net;
    TCPSocket _socket;
    MQTTClient* _mqtt_client;
};

int main() {

    float temp_value = 0;
    float humid_value = 0;
    float pressure_value = 0;

    char buf[100];

    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();


    Client *client = new Client();

    client->init_WiFi();
    client->init_MQTT();

    if (client->isConnected())
        printf("Successfully connected to broker!");
    else
        printf("Error! mqtt client is not connected to broken");

    client->subscribe("mem");

    while(1) {
        temp_value = BSP_TSENSOR_ReadTemp();
        sprintf(buf, "%.2f", temp_value);
        // printf("\nTEMPERATURE = %.2f degC\n", temp_value);
        client->publish("TEMPERATURE", buf);

        humid_value = BSP_HSENSOR_ReadHumidity();
        sprintf(buf, "%.2f", humid_value);
        // printf("HUMIDITY = %.2f %%\n", humid_value);
        client->publish("HUMIDITY", buf);

        pressure_value = BSP_PSENSOR_ReadPressure();
        sprintf(buf, "%.2f", pressure_value);
        // printf("PRESSURE = %.2f mBar\n", pressure_value);
        client->publish("PRESSURE", buf);
        
        client->yield(500);
        // printf("\n");
    }

    return 0;
}