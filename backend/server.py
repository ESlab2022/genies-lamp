from flask import Flask, request, jsonify
from flask_cors import CORS
from mqtt import publish
from db import connect_to_db, get_emergency, get_proximity
from env import env

app = Flask(__name__)
CORS(app)


@app.route("/turnOn", methods=["GET"])
def turnOn():
    data = request.args
    # single number
    deviceID = data.get("deviceID")
    if deviceID == None:
        return "ERROR"
    print(f"turning on lamp {deviceID}", flush=True)
    publish(topic=f"/lamp{deviceID}/brightness", payload="100,100,100")
    return "OK"


@app.route("/turnOff", methods=["GET"])
def turnOff():
    data = request.args
    # single number
    deviceID = data.get("deviceID")
    if deviceID == None:
        return "ERROR"
    print(f"turning off lamp {deviceID}")
    publish(topic=f"/lamp{deviceID}/brightness", payload="0,0,0")
    return "OK"


@app.route("/setColor", methods=["GET"])
def setColor():
    data = request.args
    # single number
    deviceID = data.get("deviceID")
    color = data.get("color")
    brightness = data.get("brightness")
    if deviceID == None or color == None or brightness == None:
        return "ERROR"

    brightness = int(brightness)

    # turn color into rgb
    rgb = [int(color[i : i + 2], 16) for i in (0, 2, 4)]
    # cast from 0-255 to 0-100
    rgb = [int(x / 255 * 100) for x in rgb]
    # interpolate brightness
    rgb = [int(x * brightness / 100) for x in rgb]
    rgb = ",".join([str(x) for x in rgb])

    print(
        f"setting lamp {deviceID} to color {color} with brightness {brightness}",
        flush=True,
    )

    publish(topic=f"/lamp{deviceID}/brightness", payload=rgb)

    return "OK"


@app.route("/getEmergency", methods=["GET"])
def getEmergency():
    conn = connect_to_db()
    cur = conn.cursor()
    emergency = get_emergency(cur)
    cur.close()
    conn.close()
    # return an object with deviceID as key and emergency as value
    emergency = {x[0]: x[1] for x in emergency}

    temp = emergency.copy()

    # compare to last emergency
    global last_emergency

    # remove all devices that did not change
    for deviceID in last_emergency:
        if deviceID in emergency:
            if emergency[deviceID] == last_emergency[deviceID]:
                del emergency[deviceID]

    # update last emergency
    last_emergency = temp

    # convert emergency to json and return
    return jsonify(emergency)


@app.route("/getState", methods=["GET"])
def getState():
    conn = connect_to_db()
    cur = conn.cursor()
    emergency = get_emergency(cur)
    proximity = get_proximity(cur)
    cur.close()
    conn.close()
    # return an object with deviceID as key and emergency as value
    emergency = {x[0]: x[1] for x in emergency}
    proximity = {x[0]: x[1] for x in proximity}

    print(emergency, flush=True)
    print(proximity, flush=True)

    # merge emergency and proximity
    data = {}

    for deviceID in emergency:
        data[deviceID] = {"emergency": emergency[deviceID], "proximity": False}
    for deviceID in proximity:
        if deviceID in data:
            data[deviceID]["proximity"] = proximity[deviceID]
        else:
            data[deviceID] = {"emergency": False, "proximity": proximity[deviceID]}

    return jsonify(data)
