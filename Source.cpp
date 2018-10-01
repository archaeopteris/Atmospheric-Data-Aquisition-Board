//////////////////////////////////////////////////////////////////
//																//
//	Author: Haik Voskanyan										//
//	Version: 2.0 for Teensy 3.1									//
//																//
//	Description:												//
//	Current program Reads data from BMP180 and DHT2302 Sensors.	//
//	Sensors are giving Humidity, Temperature, Atmospheric		//
//	Pressure, Altitude. It also reads data from Electrostatic	//
//	Field Strength meter and Ionizating Radiation meter.		//
//	Microcontroller writes that data on SD Card and also		//
//	transmittes it through RF433 transmitter to Ground Station. //
//																//
//	NOTES: Current program reads data every one second.			//
//																//
//	Reading temperature or humidity takes about 250ms!			//
//	Sensor readings may also be up to 2 seconds 'old'.			//
//																//
//	To calculate the Altitude you need to have current			//
//	temperature as well as Sea Level Pressure for your region.	//
//	You can find current SLP value by looking at weather		//
//	websites or from environmental information centers near any //
//	major airport. For example, for Paris, France:				//
//	http://bit.ly/16Au8ol										//
//	Otherwise use 'generic' value of 1013.25 hPa defined as		//
//	SENSORS_PRESSURE_SEALEVELHPA in sensors.h. This isn't ideal	//
//	and will give variable results from day to day.				//
//	___________________________________________________________ //
//																//
//	Connect Sensors as follows:									//
//	NOTE: connections for Arduino boards are different.			//
//		See appropriate programs.								//
//																//
//	Barometric Sesnor Connections								//
//	===========													//
//	Connect SCL to Pin 19										//
//	Connect SDA to Pin 18										//
//	Connect VDD to 3V DC										//
//	Connect GROUND to common ground								//
//																//
//																//
//	Humidity Sensor Connections									//
//	===========													//
//	Connect pin 1 (on the left) to 3.3V							//
//	Connect pin 2 to DHTPIN										//
//	Connect pin 4 (on the right) to GROUND						//
//	Connect a 10K resistor from pin 2 (data) to pin 1 (power)	//
//																//
//	SD Card Connections (Attached to SPI bus)					//
//	============												//
//	MOSI - pin 11												//
//	MISO - pin 12												//
//	CLK - pin 13												//
//	CS - pin 10													//
//																//
//	Bluetooth Transceiver's connections:						//
//	============												//
//	Hardware Serial2 is used. RX - 9; Tx - 10					//
//																//
//	Geyger counter's connecctions:								//
//	============												//
//	Data - A10													//
//																//
//	EField Meter's connections:									//
//	============												//
//	Bluetooth receive on 9										//
//																//
//////////////////////////////////////////////////////////////////

#include "DHT.h"
#include <Adafruit_Sensor.h>
#include <BMP085_U.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <VirtualWire.h>

// Define Type of Sensor that we are using.
#define DHTTYPE DHT22   // DHT 22  (AM2302)

// Define Pin number DHT sensor is attached
#define DHTPIN 9

// Define Serial2 for Bluetooth communication
#define HWSERIAL Serial2

// Declaration of Global Variables
// double seaLevelPressure = 1016; // Sea level pressure for current sensor location
double seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;  //Average Sea Level Pressure
double Humid;			// Variable for Humidity;
double Tempr_DHT;		// Variable for Temperature from DHT Sensor;
float Tempr_BMP;
double AtmPressure;		 // Variable for Atmospheric Pressure;
double Altitude;		 // Variable for Altitude;
int IonRadCount = 0;
float EField = 0.0;	   // Strength of Electric Field

// Define ID for Transmission. NOTE: should be a PWM Pin
int TX_ID = 3; // Transmitter ID address

// Initialize DHT sensor. Data pin is DHTPIN.
DHT dht(DHTPIN, DHTTYPE);

// Initialize Barometric Sensor
Adafruit_BMP085_Unified BMP180 = Adafruit_BMP085_Unified(10085);


// Initialize SD Card for Datalogging;
//File fileToWrite;


// Initialize a storage place for Data to be sent;
typedef struct roverRemoteData// Data Structure 
{
	int TX_ID;	  // Initialize a storage place for the outgoing TX ID
	
	double tx_Humid;
	double tx_Tempr_DHT;
	double tx_Tempr_BMP;
	double tx_Pressure;
	double tx_Altitude;
	double tx_IonRadCount;
	float tx_EField;
} roverRemoteData;


void setup()
{
	Serial.begin(9600);
	Serial.println("Hello");
	
//	Serial.println("Reading of Atmospheric Data...\n");

	//  Initialise Barometric sensor
	if(!BMP180.begin())
	{
		Serial.print("Ooops, no BMP180 detected ... Check your wiring!");
	}

	Serial.print("Humidity (%)\tTempr. (*C)\tAtm. Pres. (hPa)\tAltitude (m)\tIR Level (Imp/50ms)\tEField (V/m)\n");


	// On the Ethernet Shield, CS is pin 4. It's set as an output by default.
	// Note that even if it's not used as the CS pin, the hardware SS pin
	// (10 on most Arduino boards, 53 on the Mega) must be left as an output
	// or the SD library functions will not work.
	pinMode(4, OUTPUT);

	delay(1);
	
	if (!SD.begin(10))
	{
		Serial.println("SD card initialization failed!");
		return;
	}
	Serial.println("SD Card initialization done.");

//	fileToWrite = SD.open("data.txt", FILE_WRITE);


	// Setup and Begin communication over the radios(MIN Speed is 1000bps, MAX 4000bps);
	vw_setup(2000);
	vw_set_tx_pin(10);// Set Tx Pin to 6;

	// Serial Connection with Bluetooth HC-05
	HWSERIAL.begin(38400);

}

void loop()
{
	// Load data that needs to be sent.
	struct roverRemoteData payload;
	int RadsignalLevel = analogRead(A10);
	

	// Reading from HC-05 and store the value, otherwise write 0.
	// Store the message in a temp string then convert to Double.
	if(HWSERIAL.available())
	{
		String BMessage = HWSERIAL.readString();
		EField = BMessage.toFloat();
	}
	else
	{
		EField = 0;
	}

	fileToWrite = SD.open("data.txt", FILE_WRITE);
	delay(1);
	
	if(RadsignalLevel <= 1000)
	{
		IonRadCount++;
	}
		
	// Make Measurements every 1 second;
	if((millis() % 1000) <= 100)
	{
		// Reading Humidity data from DHT (0-100% RH)
		Humid = dht.readHumidity();
		delay(1);
	
		// Read temperature from DHT as Celsius (the default)
		Tempr_DHT = dht.readTemperature();
		delay(1);

		// Check if any reads from DHT Sensor failed and exit early (to try again).
		if (isnan(Humid) || isnan(Tempr_DHT))
		{
			Serial.println("Failed to read from DHT sensor!");
			return;
		}

		/* Get a new sensor event from BMP */ 
		sensors_event_t event;
		BMP180.getEvent(&event);
		delay(1);

		// Get current Temperature from BMP180 Sensor;
		BMP180.getTemperature(&Tempr_BMP);
		delay(1);
		
		// Read Atmospheric Pressure Value in hPa from BMP Sensor
		AtmPressure = event.pressure;
		delay(1);
		
		// Convert the atmospheric pressure, and SLP to altitude in meters
		Altitude = BMP180.pressureToAltitude(seaLevelPressure, AtmPressure);
		delay(1);

		// Display current Humidity, Temperature, Atm. Pressure and Altitude
		Serial.print(Humid);
		Serial.print(" ");
		Serial.print(Tempr_DHT);
		Serial.print(" ");
		Serial.print(Tempr_BMP);
		Serial.print(" ");
		Serial.print(AtmPressure);
		Serial.print(" ");
		Serial.print(Altitude);
		Serial.print(" ");
		Serial.print(IonRadCount);
		Serial.print(" ");
		Serial.print(EField);
		Serial.print(" \n");


		// Get the Sensor Values and send throught Radio;
		payload.TX_ID = TX_ID; // Set the Radio Address 
		payload.tx_Humid = Humid;
		payload.tx_Tempr_DHT = Tempr_DHT;
		payload.tx_Tempr_BMP = Tempr_BMP;
		payload.tx_Pressure = AtmPressure;
		payload.tx_Altitude = Altitude;
		payload.tx_IonRadCount = IonRadCount;
		payload.tx_EField = EField;

		// Send the data and wait for all data to be sent;
		vw_send((uint8_t *)&payload, sizeof(payload));
		vw_wait_tx();


		// Print Data to SD Card;
		if (fileToWrite)
		{
			fileToWrite.print(Humid);
			fileToWrite.print("\t");
			fileToWrite.print(Tempr_DHT);
			fileToWrite.print("\t");
			fileToWrite.print(Tempr_BMP);
			fileToWrite.print("\t");
			fileToWrite.print(AtmPressure);
			fileToWrite.print("\t");
			fileToWrite.print(Altitude);
			fileToWrite.print("\t");
			fileToWrite.print(IonRadCount);
			fileToWrite.print("\t");
			fileToWrite.print(EField);
			fileToWrite.print("\n");
		}
		else
		{
			// if the file didn't open, print an error:
			Serial.println("error opening AtmData.txt");
		}
		delay(1);

		IonRadCount = 0;
	}

	fileToWrite.close();
	
	delay(10);
}
