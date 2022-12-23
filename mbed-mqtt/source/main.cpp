#include <stdlib.h>
#include <string.h>

#include <cstdio>

#include "DigitalIn.h"
#include "InterruptIn.h"
#include "PinNames.h"
#include "mbed.h"

// For sensors
#include "client.h"
#include "light.h"
#include "scanner.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_tsensor.h"

events::EventQueue event_queue;

Light light;
// InterruptIn button(PA_5);
DigitalIn button(PA_4);

void messageArrived(MQTT::MessageData &md) {
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos,
           message.retained, message.dup, message.id);
    printf("Payload %.*s\r\n", message.payloadlen, (char *)message.payload);
}

void pwmArrived(MQTT::MessageData &md) {
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos,
           message.retained, message.dup, message.id);
    printf("PWM %.*s\r\n", message.payloadlen, (char *)message.payload);

    light.set_server_color((char *)message.payload);

    // int duty_cycle = atoi((char *)message.payload);
    // float pulsewidth = PERIOD * (duty_cycle / 100.0f);
    // led.pulsewidth(pulsewidth);
}

volatile int button_status = -1;

void button_pressed() { button_status = 1; }

void button_released() { button_status = -1; }

void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

int main() {
    float temp_value = 0;
    float humid_value = 0;
    float pressure_value = 0;
    int last_button_status = 0;
    int last_on_time;

    char buf[100];

    light.set_color(0, 0, 0);

    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();

    BLE &ble = BLE::Instance();
    ble.onEventsToProcess(schedule_ble_events);
    Scanner scanner(ble, event_queue);
    scanner.run();

    // led.period(PERIOD);
    // button.rise(&button_pressed);
    // button.fall(&button_released);
    button.mode(PullDown);

    Client *client = new Client();

    client->init_WiFi();
    client->init_MQTT();

    if (client->isConnected())
        printf("Successfully connected to broker!\n");
    else
        printf("Error! mqtt client is not connected to broken\n");

    // client->subscribe("mem");

    client->subscribe_pwm();

    while (1) {
        scanner.phone_near = false;

        temp_value = BSP_TSENSOR_ReadTemp();
        sprintf(buf, "%.2f", temp_value);
        // printf("\nTEMPERATURE = %.2f degC\n", temp_value);
        client->publish("temperature", buf);

        humid_value = BSP_HSENSOR_ReadHumidity();
        sprintf(buf, "%.2f", humid_value);
        // printf("HUMIDITY = %.2f %%\n", humid_value);
        client->publish("humidity", buf);

        pressure_value = BSP_PSENSOR_ReadPressure();
        sprintf(buf, "%.2f", pressure_value);
        // printf("PRESSURE = %.2f mBar\n", pressure_value);
        client->publish("pressure", buf);

        button_status = button.read();
        printf("%d\n", button_status);
        if (last_button_status != button_status) {
            if (button_status == 1) {
                printf("pressed\n");
                client->publish("emergency", "1");
            } else {
                printf("released\n");
                client->publish("emergency", "0");
            }
        }
        last_button_status = button_status;

        client->yield(100);
        // scanner.scan_phone();
        event_queue.dispatch_for(900ms);

        bool phone_near;
        if (scanner.phone_near) {
            printf("near\n");
            // led.pulsewidth(PERIOD);
            phone_near = true;
            client->publish("proximity", "1");
            last_on_time = time(NULL);
        } else {
            if (time(NULL) - last_on_time > 5) {
                phone_near = false;
                client->publish("proximity", "0");
                // led.pulsewidth(0);
            }
        }
        // printf("\n");

        light_state state;
        if (button_status == 1) {
            state = light_state::EMERGENCY;
            // scanner.stop_scan();
        } else if (light.server_is_on()) {
            state = light_state::SERVER;
            // scanner.stop_scan();
        } else if (phone_near) {
            state = light_state::PROXIMITY;
            // scanner.start_scan();
        } else {
            state = light_state::OFF;
            // scanner.start_scan();
        }
        // printf("%d\n", state);
        light.set_state(state);
    }

    return 0;
}