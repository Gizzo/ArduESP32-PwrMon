#include <EmonLib.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
 
Adafruit_ADS1115 ads(0x48);

//adsGain_t getGain(void);
 
void setup(void)
{
Serial.begin(115200);
Serial.println("Hello!");
 
Serial.println("Getting single-ended readings from AIN0..3");
Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

Wire.begin(5,4, 40000);
 
ads.begin();

//ads.setGain(GAIN_FOUR);
adsGain_t gainNow = ads.getGain();

Serial.print("Current gain - ");
Serial.println(gainNow);
}
 
void loop(void)
{
int16_t adc0, adc1, adc2, adc3;
 
adc0 = ads.readADC_SingleEnded(0);
adc1 = ads.readADC_SingleEnded(1);
adc2 = ads.readADC_SingleEnded(2);
adc3 = ads.readADC_SingleEnded(3);

Serial.print(adc0);
Serial.print(", ");
Serial.print(adc1);
Serial.print(", ");
Serial.print(adc2);
Serial.print(", ");
Serial.print(adc3);
Serial.println("");
 
delay(500);
}
