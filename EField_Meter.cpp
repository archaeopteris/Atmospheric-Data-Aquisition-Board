//////////////////////////////////////////////////////////////////////////////
//	Author: Hayk Voskanyan													//
//	Version: 3.2															//
//	Last Modified: 10.Jun.2016												//
//																			//
//  Description: This program runs onboard of the electric field meter.		//
//				The electric field is of cylindric type and doesn't require	//
//				grounding for proper operation.								//
//				It reads analog data from operational amplifier,			//
//				digitizes it and transmites through bluetooth to the		//
//				main board.													//
//				Designed as part of Atmospheric Data Aquisition Board.		//
//																			//
//////////////////////////////////////////////////////////////////////////////

#include <ADC.h>

#define HWSERIAL Serial2

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = A11;  // Analog input pin that the opAmp is attached to
const int phTransistorPin = A10;
// ADC Object;
ADC *adc = new ADC();

void setup()
{
    // Serial Connection with Bluetooth HC-06
    HWSERIAL.begin(38400);
    
    //pin 23 single ended
    pinMode(analogInPin, INPUT);
    pinMode(phTransistorPin, INPUT);
         
    ///// ADC_0 ////
    // reference can be ADC_REF_3V3, ADC_REF_1V2 (not for Teensy LC) or ADC_REF_EXT.
    //adc->setReference(ADC_REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->setAveraging(4); // set number of averages. Can be 0, 4, 8, 16, 32
    adc->setResolution(16); // set bits of resolution

    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    // see the documentation for more information
    adc->setConversionSpeed(ADC_HIGH_SPEED); // change the conversion speed
    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_HIGH_SPEED); // change the sampling speed

//    adc->startContinuous(analogInPin);

    ///// ADC_1
    adc->setAveraging(4, ADC_1); // set number of averages. Can be 0, 4, 8, 16, 32
    adc->setResolution(10, ADC_1); // set bits of resolution

    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    // see the documentation for more information
    adc->setConversionSpeed(ADC_HIGH_SPEED, ADC_1); // change the conversion speed
    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_HIGH_SPEED, ADC_1); // change the sampling speed


    delay(500);
}

// Continuous Measurement with continuous mode ADC conversion
void loop()
{   
    int phTransistor = adc->analogRead(A10, ADC_1);
    
    // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
    float OpAmp = (uint16_t)adc->analogRead(analogInPin, ADC_0);

//    float temp = ((OpAmp * 3.3) / 65535) / 0.01655733121779;

//    float temp = ((OpAmp * 3.3) / 65535);

    float EField = (0.0000226685 * pow(OpAmp, 2.0)) - (8.11988 * OpAmp) + 241249;

    
    
    if(phTransistor < 420)
    {
        HWSERIAL.println(EField);
//        HWSERIAL.print("\tPeak");
//        HWSERIAL.print(phTransistor);
    }

//	HWSERIAL.print("\n");

    if(adc->adc0->fail_flag)
	{
        Serial.print("ADC0 error flags: 0x");
        Serial.println(adc->adc0->fail_flag, HEX);
        if(adc->adc0->fail_flag == ADC_ERROR_COMPARISON)
        {
            adc->adc0->fail_flag &= ~ADC_ERROR_COMPARISON; // clear that error
            Serial.println("Comparison error in ADC0");
        }
    }

    delay(1);
}

// Measures once per rotation with single-mode ADC conversion
/*
void loop()
{   
    int phTransistor = analogRead(A10);
    
    if(phTransistor < 10000)
    {
        // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
        float OpAmp = float(adc->analogRead(analogInPin));
        
        HWSERIAL.print(((OpAmp * 3.3) / 65535) / 0.01655733121779);
//        HWSERIAL.print(OpAmp);
        HWSERIAL.print("\n");

        if(adc->adc0->fail_flag)
        {
            Serial.print("ADC0 error flags: 0x");
            Serial.println(adc->adc0->fail_flag, HEX);
            if(adc->adc0->fail_flag == ADC_ERROR_COMPARISON)
            {
                adc->adc0->fail_flag &= ~ADC_ERROR_COMPARISON; // clear that error
                Serial.println("Comparison error in ADC0");
            }
        }
    }
    
    delay(1);
}
*/
