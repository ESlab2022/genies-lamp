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
            duty_cycle += 1
            
            # Publish the message to topic
            for i in range(5):
                payload = "50,100,100" if duty_cycle % 2 == 0 else "0,0,0"
                client.publish(topic=f'/lamp{i}/brightness', payload=payload)
            time.sleep(1)
            
    except KeyboardInterrupt as e:
        client.loop_stop()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", default="localhost", help="service ip of MQTT broker")
    parser.add_argument(
        "--port", default=1883, type=int, help="service port of MQTT broker"
    )
    # parser.add_argument("--device", default=0, type=int, help="lamp id")
    args = vars(parser.parse_args())
    main(args)
