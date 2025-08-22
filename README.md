# Environmental Monitor

This project is for an IoT environmental monitor. 

### Equipment

- Pi Pico 2W 
    - Uses the Wifi module for self configuration and connecting to a Wifi network.
- BME688
    - Sensor with Temperature, Humidity, Pressure, and Gas sensing
    - This project doesn't use the Gas sensing since it's a bit more involved to actually get anything useful from that sensor.
- Buttons
    - Reset Button
        - Reboots the board
    - Config Reset Button
        - Resets the internal config from Wifi and MQTT settings

### Features

- Device configuration through AP and webserver
- Persistent config storage using onboard flash
- Watchdog to reset the device if something gets stuck
- Async sensor and network workers so the processor can sleep when not needed
- Docker based server with visualization and alerts through Grafana and persistent storage using InfluxDB

# Setup

Since there are submodules in this repo you'll need to clone it with:

``` shell
git clone --recursive <repo_url>
```

# Build environment setup

Install the Pico SDK

This project was setup using the Pi Pico VSCode extension, so I'd recommend just using that for easy setup. It is just a CMake project though, so you can just use CMake if you want assuming your paths line up to what the extension expects.


# Server setup

This uses the MING stack so there's no need to write a backend for data visualization.

## Docker

This setup uses docker compose so you'll need docker installed first.

## Create directories
``` shell
mkdir -p ./server/log/mosquitto \
mkdir -p ./server/data/{mosquitto,influxdb,node-red,grafana}
```

## Launch the server
``` shell
# In the directory with the docker-compose.yml
docker compose up -d
```

## Setup InfluxDB

Create the database

``` shell
docker exec -it influxdb influx -execute 'CREATE DATABASE env_monitor'
```

## Setup Node Red

- Install the InfluxDB plugin
- Create an MQTT in node and an InfluxDB out
- Configure the nodes to point at corresponding services
- Connect the two nodes together


## Setup Grafana

Add InfluxDB as a data source and create the corresponding panels (You probably need some data there first so maybe set this up after getting the device up and running). You can also add alerts etc. for anything else you may want in Grafana.


# Device Setup

On initial boot, there will be no Wifi or MQTT config set so the device will start up in Access Point mode.

Join the network `env_monitor` with the password `password` and navigate to the device's webserver at `192.168.4.1` and set all the fields in the form, then reboot the device.

Once the device reboots it should connect to the Wifi network and then connect to the MQTT server. The LED on the board will light up once connected to the MQTT server.