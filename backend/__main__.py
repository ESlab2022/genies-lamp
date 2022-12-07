from .server import app

# start the server
if __name__ == "__main__":
    app.run(host="127.0.0.1", port=5000, debug=False)
