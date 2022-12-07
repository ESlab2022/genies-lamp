from .mqtt import init_client, start_client
from .db import init_db

def main():
    init_db()
    init_client()
    start_client()

if __name__ == "__main__":
    main()
