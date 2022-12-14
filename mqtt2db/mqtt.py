import time

import paho.mqtt.client as mqtt

from env import env
from db import write_to_db, get_timestamp
from config import config

__all__ = ["init_client", "start_client"]

def on_message(client, obj, msg):
    print(f"TOPIC:{msg.topic}, VALUE:{msg.payload}")
    # write data to database
    topic = msg.topic.split("/")[1:]
    deviceID = int(topic[0].replace("lamp", ""))
    table = topic[1]

    if table in config["tables"].keys():
        dataType = config["tables"][topic[1]]
        if dataType == "FLOAT":
            value = float(msg.payload.decode("utf-8"))
        elif dataType == "INT":
            value = int(msg.payload.decode("utf-8"))
        elif dataType == "BOOL":
            value = int(msg.payload.decode("utf-8"))
            value = "true" if value == 1 else "false"
        
        timestamp = get_timestamp()
        payload = {
            "deviceID": deviceID,
            "value": value,
            "timestamp": timestamp
        }
        write_to_db(table, payload)


client = mqtt.Client()

def init_client():
    client.on_message = on_message
    client.connect(host=env["MQTT_HOST"], port=int(env["MQTT_PORT"]))
    # subscribe to all topics
    for i in range(config["devices"]):
        for topic in config["subscriptions"]:
            client.subscribe(f"/lamp{i}/{topic}")

def start_client():
    try:
        client.loop_forever()
    except KeyboardInterrupt as e:
        pass
