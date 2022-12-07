import time

import psycopg2
import paho.mqtt.client as mqtt
from dotenv import dotenv_values

env = dotenv_values("../.env")

conn = psycopg2.connect(
    host=env["POSTGRES_HOST"],
    port=env["POSTGRES_PORT"],
    database=env["POSTGRES_DB"],
    user=env["POSTGRES_USER"],
    password=env["POSTGRES_PASS"],
)


def on_message(client, obj, msg):
    print(f"TOPIC:{msg.topic}, VALUE:{msg.payload}")
    # write data to database
    if msg.topic == "cpu":
        payload = float(msg.payload.decode("utf-8"))
        # get timestamp in sql format in utc timezone
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime())
        cur = conn.cursor()
        cur.execute(
            """
            INSERT INTO cpu (value, timestamp) VALUES (%s, %s);
            """,
            (payload, timestamp),
        )
        conn.commit()


def main(env):
    # Establish connection to mqtt broker
    client = mqtt.Client()
    client.on_message = on_message
    client.connect(host=env["HOST_IP"], port=int(env["HOST_PORT"]))
    client.subscribe("cpu", 0)
    client.subscribe("mem", 0)

    device_list = [0, 1]
    for i in device_list:
        client.subscribe(f"/lamp{i}/brightness", 0)
        client.subscribe(f"/lamp{i}/temperature", 0)
        client.subscribe(f"/lamp{i}/pressure", 0)
        client.subscribe(f"/lamp{i}/humidity", 0)
        client.subscribe(f"/lamp{i}/emergency", 0)

    try:
        client.loop_forever()
    except KeyboardInterrupt as e:
        pass


if __name__ == "__main__":
    # create a table cpu if not exists
    cur = conn.cursor()
    cur.execute(
        """
        CREATE TABLE IF NOT EXISTS cpu (
            id SERIAL PRIMARY KEY,
            value FLOAT NOT NULL,
            timestamp TIMESTAMP NOT NULL
        );
        """
    )
    conn.commit()
    main(env)
    conn.close()
