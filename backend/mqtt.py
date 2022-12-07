import paho.mqtt.client as mqtt
from .env import env

__all__ = ["publish", "stop"]

publisher = mqtt.Client()
publisher.connect(host=env["HOST_IP"], port=int(env["HOST_PORT"]))
publisher.loop_start()

def publish(topic, payload):
    publisher.publish(topic=topic, payload=payload)

def stop():
    publisher.loop_stop()