// Thermistor Example #3 from the Adafruit Learning System guide on Thermistors 
// https://learn.adafruit.com/thermistor/overview by Limor Fried, Adafruit Industries
// MIT License - please keep attribution and consider buying parts from Adafruit
#include <Arduino.h>
// which analog pin to connect
int AN_In1 = 36;
#define THERMISTORPIN AN_In1         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 876      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 71   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 255
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT -440.18
// the value of the 'other' resistor
#define SERIESRESISTOR 10000    

// 


int samples[NUMSAMPLES];

void setup(void) {
  Serial.begin(115200);
}

void tempread(void){
 uint8_t i;
  float average;

  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
  
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;

  Serial.print("Average analog reading "); 
  Serial.println(average);
  
  // convert the value to resistance
  average = 4095 / average - 1;
  average = SERIESRESISTOR / average;
  Serial.print("Thermistor resistance "); 
  Serial.println(average);
  
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert absolute temp to C
  
  Serial.print("Temperature "); 
  Serial.print(steinhart);
  Serial.println(" *C");
  
  delay(1000);
}


void loop(void) {
 tempread();
}