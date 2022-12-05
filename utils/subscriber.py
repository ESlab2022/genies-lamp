import time
import argparse

import psutil
import paho.mqtt.client as mqtt

def on_message(client, obj, msg):
    print(f"TOPIC:{msg.topic}, VALUE:{msg.payload}")

def main(args):
    # Establish connection to mqtt broker
    client = mqtt.Client()
    client.on_message = on_message
    client.connect(host=args['ip'], port=args['port'])
    client.subscribe('cpu', 0)
    client.subscribe('mem', 0)
    client.subscribe('TEMPERATURE', 0)
    client.subscribe('PRESSURE', 0)
    client.subscribe('HUMIDITY', 0)

    try:
        client.loop_forever()
    except KeyboardInterrupt as e:
        pass

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip",
                        default="localhost",
                        help="service ip of MQTT broker")
    parser.add_argument("--port",
                        default=1883,
                        type=int,
                        help="service port of MQTT broker")
    args = vars(parser.parse_args())
    main(args)