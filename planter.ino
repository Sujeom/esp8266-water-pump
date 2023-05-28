#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include "connection.h"
#include "variable_functions.h"

#define EEPROM_SIZE 32      // Size of the variable to be stored
#define MAX_STRING_LENGTH 20  // Maximum length of the string to be stored
#define EEPROM_ADDRESS 0    // Starting EEPROM address for storage
#define ARRAY_SIZE 96

unsigned long previousMillis = 0;
const unsigned long interval = 15 * 60 * 1000; // 15 minutes

const int ssidMaxSize = 32;    // Maximum size of SSID (including null terminator)
const int passwordMaxSize = 64;    // Maximum size of password (including null terminator)
const int waterGpioPin = 16;
const int lightGpioPin = 14;

bool waterTimer[ARRAY_SIZE];
bool lightTimer[ARRAY_SIZE];

char storedSSID[ssidMaxSize];
char storedPassword[passwordMaxSize];

int waterCounter = 0;
int lightCounter = 0;

ESP8266WebServer server(80);
//AsyncWebServer server(80);

void setup() {
  // initialize the eeprom
  EEPROM.begin(512 * 1024);

  Serial.begin(115200); 

  
  pinMode(waterGpioPin, OUTPUT);
  pinMode(lightGpioPin, OUTPUT);
 
  // attempt to connect to wifi
  bool didConnect = connectToWiFi(storedSSID, storedPassword, ARRAY_SIZE, waterTimer, lightTimer);
  
  // Start the web server
  server.begin();

  if(!didConnect) {
    setupAP(server, waterTimer, lightTimer);
  }

  dashboard(server, storedSSID, storedPassword, waterTimer, lightTimer);

  previousMillis = millis();
}

void loop() {
  server.handleClient();
  // Check if the specified interval has elapsed
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    waterCounter = (waterCounter + 1) % ARRAY_SIZE;
    lightCounter = (lightCounter + 1) % ARRAY_SIZE;
    // Reset the timer
    previousMillis = currentMillis;
  }

  if(waterTimer[waterCounter] == true) {
    digitalWrite(waterGpioPin, HIGH);
  } else {
    digitalWrite(waterGpioPin, LOW);
  }

  if(lightTimer[lightCounter] == true) {
    digitalWrite(lightGpioPin, HIGH);
  } else {
    digitalWrite(lightGpioPin, LOW);
  }
  // Other code in the loop
  // ...
}
