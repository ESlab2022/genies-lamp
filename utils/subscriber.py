import os
import paho.mqtt.client as mqtt
from dotenv import dotenv_values

ENV_PATH = os.path.join(os.path.dirname(__file__), "..", ".env")

env = dotenv_values(ENV_PATH)


def on_message(client, obj, msg):
    print(f"TOPIC:{msg.topic}, VALUE:{msg.payload}")


def main(env, topics):
    # Establish connection to mqtt broker
    client = mqtt.Client()
    client.on_message = on_message
    client.connect(host=env["MQTT_HOST"], port=int(env["MQTT_PORT"]))

    device_list = [0, 1]
    for i in device_list:
        for t in topics:
            client.subscribe(f"/lamp{i}/{t}", 0)

    try:
        client.loop_forever()
    except KeyboardInterrupt as e:
        pass


if __name__ == "__main__":
    # get arguments
    topics = [
        "brightness",
        # "temperature",
        # "pressure",
        # "humidity",
        "emergency",
    ]
    main(env, topics)
