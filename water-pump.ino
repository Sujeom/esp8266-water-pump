#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

unsigned long previousMillis = 0;
const unsigned long interval = 1 * 60 * 60 * 1000; //  1 hour
// const unsigned long interval = 1 * 60 * 60 * 1000; //  30 minutes
// const unsigned long interval = 10000; // 3 seconds
const int switchGpioPin = 16;
const int waterGpioPin = 5;
const int waterGpioPinSwitchV = 14;
const int waterGpioPinSwitchG = 13;

bool needsWater = 0;


int waterCounter = 0;
int lightCounter = 0;

ESP8266WebServer server(80);
//AsyncWebServer server(80);

void setup() {

  Serial.begin(115200); 

  Serial.println("starting");
  
  pinMode(waterGpioPin, INPUT);
  pinMode(waterGpioPinSwitchV, OUTPUT);
  pinMode(waterGpioPinSwitchG, OUTPUT);
  pinMode(switchGpioPin, OUTPUT);
 

}

void loop() {
  server.handleClient();

  // Check if the specified interval has elapsed

  // unsigned long currentMillis = millis();
  // if (currentMillis - previousMillis >= interval) {
    Serial.println("Turning on water sensor");
    digitalWrite(waterGpioPinSwitchV, HIGH);
    digitalWrite(waterGpioPinSwitchG, HIGH);
    needsWater = digitalRead(waterGpioPin);
    Serial.println(needsWater);

    Serial.println("checking if full");
    if(needsWater != false) {
      Serial.println("needs to be filled");
      digitalWrite(switchGpioPin, HIGH);
      while (needsWater != false) {
        delay(1000);
        needsWater = digitalRead(waterGpioPin);
        Serial.println("filling");
        Serial.print("needsWater: ");
        Serial.println(needsWater);
      }
      digitalWrite(switchGpioPin, LOW);
    }
      Serial.println("it is full");
      Serial.println("Turning off water sensor");
      digitalWrite(waterGpioPinSwitchV, LOW);
      digitalWrite(waterGpioPinSwitchG, LOW);
    // Reset the timer
  //   previousMillis = currentMillis;
  // }
  delay(interval);
}
