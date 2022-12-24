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

## MQTT topic naming regulation

```
/lamp{deviceID}/temperature:	float
/lamp{deviceID}/humidity:		float
/lamp{deviceID}/pressure:		float
/lamp{deviceID}/emergency:	bool 0, 1

/lamp{deviceID}/brightness:	int 0 - 100
```
