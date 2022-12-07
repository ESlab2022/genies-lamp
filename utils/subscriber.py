import time

import psycopg2
import paho.mqtt.client as mqtt
from dotenv import dotenv_values

conn = psycopg2.connect(
    host="localhost",
    database="postgres",
    user="postgres",
    password="postgres",
    port="5432",
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

    env = dotenv_values("../.env")
    main(env)

    conn.close()
