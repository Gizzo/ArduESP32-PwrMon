// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"             // Include Emon Library

#define VOLT_CAL1 110.68    //voltage calibration
#define VOLT_CAL2 163.92
#define CURRENT_CAL1 56.1 //sensor 1 calibration
#define CURRENT_CAL2 60.9 //sensor 2 calibration

EnergyMonitor emon1;             // Create an instance
EnergyMonitor emon2;             // Create an instance

const int currentPin1 = 1;
const int currentPin2 = 3;
const int voltagePin1 = 2;
const int voltagePin2 = 0;

int serUpdate = 1000;            // Serial update time in millis
unsigned long time_now = 0;

void setup()
{  
  Serial.begin(115200);
 // Serial1.begin(115200);
  
  emon1.voltage(voltagePin1, VOLT_CAL1, 1.7);       // Voltage: input pin, calibration, phase_shift
  emon1.current(currentPin1, CURRENT_CAL1);       // Current: input pin, calibration.

  emon2.voltage(voltagePin2, VOLT_CAL2, 1.64);       // Voltage: input pin, calibration, phase_shift
  emon2.current(currentPin2, CURRENT_CAL2);       // Current: input pin, calibration.
}

void loop()
{
  //Serial.print("* ");
  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  emon2.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out

  //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)  
  //emon2.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  
  float rPower1       = emon1.realPower;        //extract Real Power into variable
  float rPower2       = emon2.realPower;        //extract Real Power into variable
  float aPower1   = emon1.apparentPower;    //extract Apparent Power into variable
  float aPower2   = emon2.apparentPower;    //extract Apparent Power into variable
  float pFactor1     = emon1.powerFactor;      //extract Power Factor into Variable
  float pFactor2     = emon2.powerFactor;      //extract Power Factor into Variable
  float Irms1            = emon1.Irms;             //extract Irms into Variable
  float Irms2            = emon2.Irms;             //extract Irms into Variable
  float supplyVoltage1   = emon1.Vrms;             //extract Vrms into Variable
  float supplyVoltage2   = emon2.Vrms;             //extract Vrms into Variable

  if( millis() > time_now + serUpdate ) {
    time_now = millis();

    Serial.print("<CT1,");
    Serial.print(rPower1);
    Serial.print(",");
    Serial.print(aPower1);
    Serial.print(",");
    Serial.print(pFactor1);
    Serial.print(",");
    Serial.print(supplyVoltage1);
    Serial.print(",");
    Serial.print(Irms1);
    Serial.println(">");

    Serial.print("<CT2,");
    Serial.print(rPower2);
    Serial.print(",");
    Serial.print(aPower2);
    Serial.print(",");
    Serial.print(pFactor2);
    Serial.print(",");
    Serial.print(supplyVoltage2);
    Serial.print(",");
    Serial.print(Irms2);
    Serial.println(">");

    //emon2.serialprint();
  }
}
