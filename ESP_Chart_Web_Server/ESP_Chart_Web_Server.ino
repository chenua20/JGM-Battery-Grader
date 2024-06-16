// Import required libraries
#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

// Replace with your network credentials
const char* ssid = "Seans iPhone";
const char* password = "BakedLays";

// Global variable initialization
float c = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readINA219Voltage() {
  float temp = ina219.getBusVoltage_V() + ina219.getShuntVoltage_mV()/1000;
  float v = 0;
  if (temp>1 && temp<10){
    v = temp+0.42;
  }
  if (isnan(temp)) {    
    Serial.println("Failed to read from INA219 sensor!");
    return "";
  }
  else {
    Serial.println(v);
    return String(v);
  }
}

String readINA219Current() {
  float temp = ina219.getCurrent_mA();
  float i = 0;
  if (temp < -1){
    i = -(temp + 400);
  }
  if (isnan(temp)) {
    Serial.println("Failed to read from INA219 sensor!");
    return "";
  }
  else {
    Serial.println(i);
    return String(i);
  }
}

String readINA219Power() {
  float temp = ina219.getPower_mW();
  float p = 0;
  if (temp < 10000){
    p = temp;
  }
  if (isnan(temp)) {
    Serial.println("Failed to read from INA219 sensor!");
    return "";
  }
  else {
    Serial.println(p);
    return String(p);
  }
}


String readCapacity() {
  float i = ina219.getCurrent_mA();
  float temp = 0;
  if (i < -1){
    temp = c + -(i + 400)/3600;
    c = temp;
  }
  if (isnan(i)) {
    Serial.println("Failed to read from INA219 sensor!");
    return "";
  }
  else {
    Serial.println(c);
    return String(c);
  }
}

void setup(){

  // Serial port for debugging purposes
  Serial.begin(115200);

  // Serial port for debugging purposes
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
