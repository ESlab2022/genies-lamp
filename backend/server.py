from flask import Flask, request
from mqtt import publish

app = Flask(__name__)

@app.route("/turnOn", methods=["GET"])
def turnOn():
    data = request.args
    # single number
    deviceID = data.get("deviceID")
    if deviceID == None:
        return "ERROR"
    print(f"turning on lamp {deviceID}")
    publish(topic=f"/lamp{deviceID}/brightness", payload=100)
    return "OK"

@app.route("/turnOff", methods=["GET"])
def turnOff():
    data = request.args
    # single number
    deviceID = data.get("deviceID")
    if deviceID == None:
        return "ERROR"
    print(f"turning off lamp {deviceID}")
    publish(topic=f"/lamp{deviceID}/brightness", payload=0)
    return "OK"

@app.route("/setBrightness", methods=["GET"])
def setBrightness():
    data = request.args
    # single number
    deviceID = data.get("deviceID")
    # 0 - 100
    brightness = data.get("brightness")
    if deviceID == None or brightness == None:
        return "ERROR"
    print(f"setting brightness of lamp {deviceID} to {brightness}")
    publish(topic=f"/lamp{deviceID}/brightness", payload=brightness)
    return "OK"
