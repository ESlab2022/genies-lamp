import time

import psycopg2

from env import env

def connect_to_db():
    # retry connection if it fails
    for i in range(5):
        try:
            conn = psycopg2.connect(
                host=env["POSTGRES_HOST"],
                port=env["POSTGRES_PORT"],
                database=env["POSTGRES_DB"],
                user=env["POSTGRES_USER"],
                password=env["POSTGRES_PASS"],
            )
            print("Connection to database successful")
            return conn
        except psycopg2.OperationalError:
            # wait 5 seconds before retrying
            print("Connection to database failed, retrying in 3 seconds...")
            time.sleep(3)
            print(f"Attempt {i+1} of 5")
            continue


def get_emergency(cur):
    # select the latest emergency for each device
    cur.execute(
        """
        SELECT deviceid, value, timestamp FROM emergency WHERE timestamp = (
            SELECT MAX(timestamp) FROM emergency WHERE deviceid = emergency.deviceid
        );
        """
    )
    return cur.fetchall()
