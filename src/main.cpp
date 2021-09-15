/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <ESPmDNS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Implement this!
// #include <WiFiManager.h>  

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "north-america.pool.ntp.org", -14400, 600000);

// BBQ probe
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
float steinhart;

int timenow = 0;

// Replace with your network credentials
const char* ssid = "";
const char* password = "";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


float probe1temp(){
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
  
  // convert the value to resistance
  average = 4095 / average - 1;
  average = SERIESRESISTOR / average;
  
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert absolute temp to C
  return float(steinhart);

}

String probe1text() {
  // Read temperature as Celsius (the default)
  float t1 = probe1temp();
  // Convert temperature to Fahrenheit
  //t = 1.8 * t + 32;
  if (isnan(t1)) {    
    Serial.println("Failed to get Probe 1 Temp!");
    return "";
  }
  else {
    Serial.println(t1);
    return String(t1);
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  bool status; 
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  timeClient.begin();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", probe1text().c_str());
  });
  server.on("/temps", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/temps.csv");
  });
  // Start server
  server.begin();
}



void loop(){
  if ((timenow + 1000) < millis()) {
    timenow = millis();
    timeClient.update();
    Serial.println(timeClient.getFormattedTime());
  }
}