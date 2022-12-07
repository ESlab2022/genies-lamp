import os
from dotenv import dotenv_values

ENV_PATH = os.path.join(os.path.dirname(__file__), "..", ".env")

env = dotenv_values(ENV_PATH)
