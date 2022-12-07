from flask import Flask, request
from .mqtt import publish

app = Flask("app")

# handle post request on /turnOn
@app.route("/turnOn", methods=["POST"])
def turnOn():
    data = request.json
    # single number
    lampID = data.get("lampID")
    if lampID == None:
        return "ERROR"
    print(f"turning on lamp {lampID}")
    publish(topic=f"/lamp{lampID}/brightness", payload=100)
    return "OK"

# handle post request on /turnOff
@app.route("/turnOff", methods=["POST"])
def turnOff():
    data = request.json
    # single number
    lampID = data.get("lampID")
    if lampID == None:
        return "ERROR"
    print(f"turning off lamp {lampID}")
    publish(topic=f"/lamp{lampID}/brightness", payload=0)
    return "OK"

# handle post request on /setBrightness
@app.route("/setBrightness", methods=["POST"])
def setBrightness():
    data = request.json
    # single number
    lampID = data.get("lampID")
    # 0 - 100
    brightness = data.get("brightness")
    if lampID == None or brightness == None:
        return "ERROR"
    print(f"setting brightness of lamp {lampID} to {brightness}")
    publish(topic=f"/lamp{lampID}/brightness", payload=brightness)
    return "OK"
