/*
  Adapted from WriteSingleField Example from ThingSpeak Library (Mathworks)
  
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-thingspeak-publish-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

const int AirValue = 875;   //you need to replace this value with Value_1
const int WaterValue = 400;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent=0;

const char* ssid = "WG_Fit_GmbH_EXT";   // your network SSID (name) 
const char* password = "Liebdochwenduwillst18";   // your network password

WiFiClient  client;

unsigned long myChannelNumber = 2209609;
const char * myWriteAPIKey = "BXMFL87C1GZNYENO";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000*60*10;

// Variable to hold temperature readings
float temperatureC;
float hum;
float pressure;
//uncomment if you want to get temperature in Fahrenheit
//float temperatureF;

// Create a sensor object
Adafruit_BME280 bme; //BME280 connect to ESP32 I2C (GPIO 21 = SDA, GPIO 22 = SCL)

void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void setup() {
  Serial.begin(115200);  //Initialize serial
  initBME();
  
  WiFi.mode(WIFI_STA);   
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }
    soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
    Serial.println(soilMoistureValue);
    soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

    // Get a new temperature reading
    temperatureC = bme.readTemperature();
    hum = bme.readHumidity();
    pressure = bme.readPressure();
    Serial.print("Temperature (ºC): ");
    Serial.println(temperatureC);
    Serial.print("Temperature (ºC): ");
    Serial.println(hum);
    Serial.print("Temperature (ºC): ");
    Serial.println(pressure);

    ThingSpeak.setField(1, temperatureC);
    ThingSpeak.setField(2, hum);
    ThingSpeak.setField(3, pressure);
    ThingSpeak.setField(4, soilmoisturepercent);
    
    //uncomment if you want to get temperature in Fahrenheit
    /*temperatureF = 1.8 * bme.readTemperature() + 32;
    Serial.print("Temperature (ºC): ");
    Serial.println(temperatureF);*/
    
    
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
 
    //uncomment if you want to get temperature in Fahrenheit
    //int x = ThingSpeak.writeField(myChannelNumber, 1, temperatureF, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
}