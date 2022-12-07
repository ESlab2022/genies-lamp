from flask import Flask, request
from .mqtt import publish

app = Flask("app")

# handle post request on /turnOn
@app.route("/turnOn", methods=["POST"])
def turnOn():
    data = request.json
    # single number
    deviceID = data.get("deviceID")
    if deviceID == None:
        return "ERROR"
    print(f"turning on lamp {deviceID}")
    publish(topic=f"/lamp{deviceID}/brightness", payload=100)
    return "OK"

# handle post request on /turnOff
@app.route("/turnOff", methods=["POST"])
def turnOff():
    data = request.json
    # single number
    deviceID = data.get("deviceID")
    if deviceID == None:
        return "ERROR"
    print(f"turning off lamp {deviceID}")
    publish(topic=f"/lamp{deviceID}/brightness", payload=0)
    return "OK"

# handle post request on /setBrightness
@app.route("/setBrightness", methods=["POST"])
def setBrightness():
    data = request.json
    # single number
    deviceID = data.get("deviceID")
    # 0 - 100
    brightness = data.get("brightness")
    if deviceID == None or brightness == None:
        return "ERROR"
    print(f"setting brightness of lamp {deviceID} to {brightness}")
    publish(topic=f"/lamp{deviceID}/brightness", payload=brightness)
    return "OK"
