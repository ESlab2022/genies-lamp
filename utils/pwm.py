import time
import argparse

import paho.mqtt.client as mqtt


def main(args):
    # Establish connection to mqtt broker
    client = mqtt.Client()
    client.connect(host=args["ip"], port=args["port"])
    client.loop_start()

    # Intervally send topic message
    try:
        duty_cycle = 0
        while True:
            # Fill the payload
            duty_cycle = (duty_cycle + 10) % 100
            # Publish the message to topic
            client.publish(topic=f'lamp{args["device"]}/PWM', payload=duty_cycle)
            time.sleep(1)
    except KeyboardInterrupt as e:
        client.loop_stop()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", default="localhost", help="service ip of MQTT broker")
    parser.add_argument(
        "--port", default=1883, type=int, help="service port of MQTT broker"
    )
    parser.add_argument("--device", default=0, type=int, help="lamp id")
    args = vars(parser.parse_args())
    main(args)
