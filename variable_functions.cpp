#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define EEPROM_SIZE 32      // Size of the variable to be stored
#define MAX_STRING_LENGTH 20  // Maximum length of the string to be stored
#define EEPROM_ADDRESS 0    // Starting EEPROM address for storage
#define PASSWORD_ADDRESS 0    // Starting EEPROM address for storage
#define SSID_ADDRESS 0
#define MAX_PASSWORD_LENGTH 64  // Maximum length of the password to be stored
#define MAX_SSID_LENGTH 32  // Maximum length of the SSID to be stored
#define ARRAY_SIZE 96


const int ssidMaxSize = 32;    // Maximum size of SSID (including null terminator)
const int passwordMaxSize = 64;    // Maximum size of password (including null terminator)
const int arraySize = 96;
const int eepromArraySize = 96 * 2 * sizeof(bool);
const int ssidAddress = 0;    // EEPROM address for storing SSID
const int passwordAddress = ssidAddress + MAX_SSID_LENGTH;    // EEPROM address for storing password
const int appdataAddress = 0;
//const int eepromWaterAddress = credentialAddress + sizeof(Credentials);  // Start address in EEPROM
//const int eepromLightAddress = eepromWaterAddress + arraySize;

struct AppData {
  char ssid[MAX_SSID_LENGTH];
  char password[MAX_PASSWORD_LENGTH];
  bool waterTimer[ARRAY_SIZE];
  bool lightTimer[ARRAY_SIZE];
};

void readAppDataFromEEPROM(char* ssid, char* password, bool (&waterTimer)[], bool (&lightTimer)[]) {
  
  AppData appdata;
  EEPROM.get(0, appdata);
  
  strncpy(ssid, appdata.ssid, MAX_SSID_LENGTH);
  strncpy(password, appdata.password, MAX_PASSWORD_LENGTH);

  for (int i = 0; i < arraySize; i++ ) {
    const byte water = appdata.waterTimer[i];
    const byte light = appdata.lightTimer[i];
    waterTimer[i] = water;
    lightTimer[i] = light;
  }
}

void saveAppDataToEEPROM(const char* ssid, const char* password, bool (&waterTimer)[], bool (&lightTimer)[]) {

  Serial.println(ssid);
  Serial.println(password);
  
  AppData appdata;
  strncpy(appdata.ssid, ssid, MAX_SSID_LENGTH);
  strncpy(appdata.password, password, MAX_PASSWORD_LENGTH);

  for (int i = 0; i < arraySize; i++ ) {
    const byte water = waterTimer[i];
    const byte light = lightTimer[i];
    appdata.waterTimer[i] = water;
    appdata.lightTimer[i] = light;
  }
  
  EEPROM.put(0, appdata);

  delay(10);
  
  EEPROM.commit();
}

void clearEEPROM(int size) {
  for (int i = 0; i < size; i++) {
    EEPROM.write(i, 0);
  }

  EEPROM.commit();

  Serial.println("EEPROM Cleared");
}
