import time

import psutil
import paho.mqtt.client as mqtt
from dotenv import dotenv_values


def main(env):
    # Establish connection to mqtt broker
    client = mqtt.Client()
    client.connect(host=env["HOST_IP"], port=int(env["HOST_PORT"]))
    client.loop_start()

    try:
        while True:
            # Fill the payload
            payload = psutil.cpu_percent()
            # Publish the message to topic
            client.publish(topic="cpu", payload=payload)
            time.sleep(1)
    except KeyboardInterrupt as e:
        client.loop_stop()


if __name__ == "__main__":
    env = dotenv_values("../.env")
    main(env)
