/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries

#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_INA219.h>

/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

Adafruit_INA219 ina219; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

// Replace with your network credentials
const char* ssid = "Justin Phone";
const char* password = "Test123123";
float c = 0;
bool charging = false;
bool discharging = false;
bool complete = false;


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readINA219Voltage() {
  float v = ina219.getBusVoltage_V() + ina219.getShuntVoltage_mV()/1000;
  if (isnan(v)) {    
    Serial.println("Failed to read from INA219 sensor!");
    return "";
  }
  else {
    Serial.println(v);
    return String(v);
  }
}

String readINA219Current() {
  float i = ina219.getCurrent_mA();
  if (isnan(i)) {
    Serial.println("Failed to read from INA219 sensor!");
    return "";
  }
  else {
    Serial.println(i);
    return String(i);
  }
}

String readINA219Power() {
  float p = ina219.getPower_mW();
  if (isnan(p)) {
    Serial.println("Failed to read from INA219 sensor!");
    return "";
  }
  else {
    Serial.println(p);
    return String(p);
  }
}


String readCapacity() {
  float n = c + ina219.getCurrent_mA()/3600;
  c = n;
  if (isnan(c)) {
    Serial.println("Failed to read from INA219 sensor!");
    return "";
  }
  else {
    Serial.println(n);
    return String(n);
  }
}

void setup(){

  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(33,OUTPUT);
  pinMode(32,OUTPUT);

  digitalWrite(33, HIGH);
  digitalWrite(32, LOW);
  Serial.println("PIN SET");


  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1);
  }

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/voltage", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readINA219Voltage().c_str());
  });
  server.on("/current", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readINA219Current().c_str());
  });
  server.on("/power", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readINA219Power().c_str());
  });
  server.on("/capacity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readCapacity().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}
