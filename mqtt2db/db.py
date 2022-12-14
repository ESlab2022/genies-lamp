import time

import psycopg2

from env import env
from config import config

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
    except psycopg2.OperationalError:
        # wait 5 seconds before retrying
        print("Connection to database failed, retrying in 5 seconds...")
        time.sleep(5)
        print(f"Attempt {i+1} of 5")
    finally:
        print("Connection to database successful")
        break


def init_db():
    cur = conn.cursor()
    for table, datatype in config["tables"].items():
        cur.execute(
            f"""
            CREATE TABLE IF NOT EXISTS {table} (
                id SERIAL PRIMARY KEY,
                deviceid INTEGER NOT NULL,
                timestamp TIMESTAMP NOT NULL
            );
            """
        )
        cur.execute(
            f"""
            ALTER TABLE {table}
            ADD COLUMN IF NOT EXISTS value {datatype} NOT NULL;
            """
        )
    conn.commit()

def get_timestamp():
    return time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime())

def write_to_db(table, payload):
    cur = conn.cursor()
    keys = ", ".join(payload.keys())
    # wrap values in single quotes if they are strings
    values = ", ".join(
        [f"'{value}'" if isinstance(value, str) else str(value) for value in payload.values()]
    )
    cur.execute(
        f"""
        INSERT INTO {table} ({keys}) VALUES ({values});        
        """
    )
    print(f"Writing to {table} successful", flush=True)
    conn.commit()
    cur.close()
    