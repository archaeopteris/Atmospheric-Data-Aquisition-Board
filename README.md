# Atmospheric-Data-Aquisition-Board
This board reads data from on board sensors stores them on SD card and also transmits them through RF432 on board transmitter. It is designed for use on UAV Drones as Autonomous Atmospheric Data Aquisition Unit. It reads humidity, temperature, atm. pressure, electrostatic field strength and ionization radiation levels.

## Description:
Current program Reads data from BMP180 and DHT2302 Sensors.
Sensors are giving Humidity, Temperature, Atmospheric
Pressure, Altitude. It also reads data from Electrostatic
Field Strength meter and Ionizating Radiation meter.
Microcontroller writes that data on SD Card and also
transmittes it through RF433 transmitter to Ground Station. 

###### NOTE: Current program reads data every one second.

Reading temperature or humidity takes about 250ms!
Sensor readings may also be up to 2 seconds 'old'.

To calculate the Altitude you need to have current
temperature as well as Sea Level Pressure for your region.
You can find current SLP value by looking at weather
websites or from environmental information centers near any 
major airport. For example, for Paris, France:
http:bit.ly/16Au8ol
Otherwise use 'generic' value of 1013.25 hPa defined as
SENSORS_PRESSURE_SEALEVELHPA in sensors.h. This isn't ideal
and will give variable results from day to day.
___________________________________________________________ 

## Connect Sensors as follows:
###### NOTE: This is for connecting to Teensy 3.1 board.
For Arduino boards connections are different.


#### Barometric Sensor Connections
Connect SCL to Pin 19
Connect SDA to Pin 18
Connect VDD to 3V DC
Connect GROUND to common ground


#### Humidity Sensor Connections
Connect pin 1 (on the left) to 3.3V
Connect pin 2 to DHTPIN
Connect pin 4 (on the right) to GROUND
Connect a 10K resistor from pin 2 (data) to pin 1 (power)


#### SD Card Connections (Attached to SPI bus)
MOSI - pin 11
MISO - pin 12
CLK - pin 13
CS - pin 10


#### Bluetooth Transceiver's connections:
Hardware Serial2 is used. RX - 9; Tx - 10


#### Geyger counter's connecctions:
Data - A10


#### EField Meter's connections:
Bluetooth receive on 9


