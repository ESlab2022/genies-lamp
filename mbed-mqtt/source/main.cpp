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

#include "InterruptIn.h"
#include "PinNames.h"
#include "mbed.h"
#include <cstdio>
#include <stdlib.h>
#include <string.h>

// For sensors
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

#include "client.h"
#include "scanner.h"

events::EventQueue event_queue;

PwmOut led(PA_7);
InterruptIn button(BUTTON1);

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

    int duty_cycle = atoi((char *)message.payload);
    float pulsewidth = PERIOD * (duty_cycle / 100.0f);
    led.pulsewidth(pulsewidth);
}

volatile int button_status;

void button_pressed() {
    button_status = 1;
}

void button_released() {
    button_status = 0;
}

void schedule_ble_events(BLE::OnEventsToProcessCallbackContext* context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

int main() {

    float temp_value = 0;
    float humid_value = 0;
    float pressure_value = 0;
    int last_button_status = 0;
    int last_on_time;

    char buf[100];

    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();

    BLE& ble = BLE::Instance();
    ble.onEventsToProcess(schedule_ble_events);
    Scanner scanner(ble, event_queue);
    scanner.run();

    led.period(PERIOD);
    button.fall(&button_pressed);
    button.rise(&button_released);

    Client *client = new Client();

    client->init_WiFi();
    client->init_MQTT();

    if (client->isConnected())
        printf("Successfully connected to broker!\n");
    else
        printf("Error! mqtt client is not connected to broken\n");

    // client->subscribe("mem");

    client->subscribe_pwm();

    while(1) {
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
        
        // printf("%d %d\n", last_button_status, button_status);

        if (last_button_status == 0 && button_status == 1) {
            printf("pressed\n");
            sprintf(buf, "%d", button_status);
            client->publish("emergency", buf);
            last_button_status = 1;
        } else {
            last_button_status = button_status;
        }

        client->yield(100);
        // scanner.scan_phone();
        event_queue.dispatch_for(1000ms);

        if (scanner.phone_near) {
            // printf("near\n");
            led.pulsewidth(PERIOD);
            last_on_time = time(NULL);
        } else {
            if (time(NULL) - last_on_time > 1) {
                led.pulsewidth(0);
            }
        }
        // printf("\n");
    }

    return 0;
}