# genies-lamp

## Presquique

### Hardware

1. DISCO_L475VG_IOT01
2. Anode RGB LED
3. micro USB cable

### Software

1. Docker
2. Python
3. Mbed OS

## Installation

```shell
python3 -m venv venv
source venv/bin/activate
pip3 install -r utils/requirements.txt
```

### Environment Configuration

```shell
cp .env.default .env
```

Please modify .env to according to the comment insied.

### Server Side

Start the MQTT server, database, dashboard and Control Panel.

```shell
docker compose up -d --build
```

### Mbed OS

1. Open Mbed OS
2. Choose the cloned repository folder and open worker space
3. Modify wifi name, password and hostname of the server in `mbed-mqtt/mbed_app.json`.
4. Modify clientID in `mbed-mqtt/source/client.h`
5. Press build and run

## Usage

1. open your browser and open `hostname:3000` (Dashboard)
2. open your browser and open `hostname:3001` (Control Panel)

## Demo videos



https://user-images.githubusercontent.com/55420821/209436531-7b835418-ea20-4811-a5da-27b588f9689f.mp4


https://user-images.githubusercontent.com/55420821/209436524-54da717d-f55a-455d-bae4-5ac3371e20f8.mp4


https://user-images.githubusercontent.com/55420821/209436498-577ec972-1a37-4c13-8246-f392bc78cfd4.mp4

## MQTT topic naming regulation

```




/lamp{deviceID}/temperature:	float
/lamp{deviceID}/humidity:		float
/lamp{deviceID}/pressure:		float
/lamp{deviceID}/emergency:	bool 0, 1

/lamp{deviceID}/brightness:	int 0 - 100
```
