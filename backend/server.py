from flask import Flask, request, jsonify
from flask_cors import CORS
from mqtt import publish
from db import connect_to_db, get_emergency
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
    rgb = [int(color[i:i+2], 16) for i in (0, 2, 4)]
    # cast from 0-255 to 0-100
    rgb = [int(x/255*100) for x in rgb]
    # interpolate brightness
    rgb = [int(x*brightness/100) for x in rgb]
    rgb = ",".join([str(x) for x in rgb])

    print(f"setting lamp {deviceID} to color {color} with brightness {brightness}", flush=True)

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
    
    # convert emergency to json and return
    return jsonify(emergency)
