#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "variable_functions.h"

#define EEPROM_SIZE 32      // Size of the variable to be stored
#define MAX_STRING_LENGTH 20  // Maximum length of the string to be stored
#define EEPROM_ADDRESS 0    // Starting EEPROM address for storage
#define MAX_PASSWORD_LENGTH 20  // Maximum length of the password to be stored
#define ARRAY_SIZE 96

bool shouldPerformConfig = true;
//void connectToWiFi();

//const int numberOfMinsInDay = 96;

bool connectToWiFi(char* storedSSID, char* storedPassword, int arraySize, bool (&waterTimer)[], bool (&lightTimer)[]) {
  // Read the stored hashed password from EEPROM
  const int ssidMaxSize = 32;    // Maximum size of SSID (including null terminator)
  const int passwordMaxSize = 64;    // Maximum size of password (including null terminator)

  const int totalArrays = 2;
  const int eepromArraySizes = arraySize * totalArrays * sizeof(bool);
  const int eepromStartAddress = 0;
  unsigned long startTime = millis();
//  unsigned long timeoutDuration = 60000; // 60 seconds
  unsigned long timeoutDuration = 1000; // 60 seconds


  
  readAppDataFromEEPROM(storedSSID, storedPassword, waterTimer, lightTimer);

  for (int i =0; i < ARRAY_SIZE; i++) {
    Serial.print("water - ");
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(waterTimer[i]);
  }

  for (int i =0; i < ARRAY_SIZE; i++) {
    Serial.print("light - ");
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(lightTimer[i]);
  }
  
  Serial.println(storedSSID);
  Serial.println(storedPassword);


  WiFi.mode(WIFI_STA);
  // Connect to the wireless network using the stored password
  WiFi.begin(storedSSID, storedPassword);  // Replace "SSID" with the actual network SSID

  // Wait for the connection to be established
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (millis() - startTime >= timeoutDuration) {
      // Connection timeout
      Serial.println("Connection timeout!");
      // Handle timeout condition here, such as entering into a fallback mode

      const char* ssid = "PLANTER";
      const char* password = "";

      // Set ESP8266 to AP mode
      WiFi.mode(WIFI_AP);
    
      // Configure access point settings
//      WiFi.softAP(ssid, password);
      WiFi.softAP(storedSSID, storedPassword);
    
      // Print the ESP8266 IP address
      Serial.print("AP IP address: ");
      Serial.println(WiFi.softAPIP());

      return false;
    }
    Serial.println(storedSSID);
    Serial.println(storedPassword);
    Serial.println("Connecting to WiFi...");
    
  }

  // Connected to WiFi
  Serial.println("Connected to WiFi");
  return true;
}

void handleAPRedirect(ESP8266WebServer& server) {
  // Perform any necessary operations before redirection

  server.sendHeader("Location", "/init");  // Set the desired endpoint
  server.send(302, "text/plain", "");  // Send a redirect response
}

void handleAP(ESP8266WebServer& server) {
   // Serve the captive portal landing page
    String html = "";
    html = "<html><body>";
    html += "<h2>Wi-Fi Configuration</h2>";
    html += "</body></html>";
    
  server.send(200, "text/html", html);
}

void configModeCallback(WiFiManager *myWiFiManager, ESP8266WebServer* server) {
  // Redirect to a specific endpoint after successful connection
    Serial.println("Connected to WiFi");
}


void handleSendArray(ESP8266WebServer& server, bool (&myArray)[]) {
  // Calculate the size of the array
  const int arraySize = 96;

  const int capacity = JSON_ARRAY_SIZE(arraySize);
  // Create a JSON document
  StaticJsonDocument<capacity> doc;

  // Create an array in the JSON document
  JsonArray jsonArr = doc.to<JsonArray>();

  // Copy the boolean values from the array to the JSON array
  for (int i = 0; i < arraySize; i++) {
    jsonArr.add(myArray[i]);
  }

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(doc, jsonString);

  // Set the Content-Type header to application/json
  server.sendHeader("Content-Type", "application/json");

  // Send the JSON string as the response body
  server.send(200, "application/json", jsonString);
}

void dashboard(ESP8266WebServer& server, const char* storedSSID, const char* storedPassword, bool (&waterTimer)[], bool (&lightTimer)[]) {
  // Serve the captive portal landing page
    
  server.on("/dashboard", HTTP_GET, [&]() {
      String html = "";
      html += "<!DOCTYPE html>\n";
      html += "<html>\n";
      html += "    <head>\n";
      html += "        <style>\n";
      html += "            .loading-indicator {\n";
      html += "                position: fixed;\n";
      html += "                top: 50%;\n";
      html += "                left: 50%;\n";
      html += "                z-index: 2;\n";
      html += "                display: none;\n";
      html += "                margin: 20px;\n";
      html += "                width: 50px;\n";
      html += "                height: 50px;\n";
      html += "                border: 3px solid #ccc;\n";
      html += "                border-top-color: #333;\n";
      html += "                border-radius: 50%;\n";
      html += "                animation: spin 1s linear infinite;\n";
      html += "            }\n";
      html += "\n";
      html += "            @keyframes spin {\n";
      html += "                0% {\n";
      html += "                    transform: rotate(0deg);\n";
      html += "                }\n";
      html += "                100% {\n";
      html += "                    transform: rotate(360deg);\n";
      html += "                }\n";
      html += "            }\n";
      html += "            .rectangle {\n";
      html += "                width: 60px;\n";
      html += "                height: 50px;\n";
      html += "                background-color: gray;\n";
      html += "                display: inline-block;\n";
      html += "                margin: 5px;\n";
      html += "                position: relative;\n";
      html += "                cursor: pointer;\n";
      html += "            }\n";
      html += "            \n";
      html += "            .text {\n";
      html += "                position: absolute;\n";
      html += "                bottom: 0;\n";
      html += "                left: 0;\n";
      html += "                width: 100%;\n";
      html += "                height: 20px;\n";
      html += "                line-height: 20px;\n";
      html += "                text-align: center;\n";
      html += "                font-size: 12px;\n";
      html += "            }\n";
      html += "            \n";
      html += "            .container {\n";
      html += "                display: flex;\n";
      html += "                flex-wrap: wrap;\n";
      html += "                justify-content: center;\n";
      html += "            }\n";
      html += "            \n";
      html += "            .container2 {\n";
      html += "                display: flex;\n";
      html += "                flex-wrap: wrap;\n";
      html += "                justify-content: center;\n";
      html += "                margin-top: 100px;\n";
      html += "            }\n";
      html += "            \n";
      html += "            .confirm-button {\n";
      html += "                margin-top: 20px;\n";
      html += "                text-align: center;\n";
      html += "            }\n";
      html += "            \n";
      html += "            .confirm-button button {\n";
      html += "                padding: 10px 20px;\n";
      html += "                font-size: 16px;\n";
      html += "                cursor: pointer;\n";
      html += "            }\n";
      html += "        </style>\n";
      html += "    </head>\n";
      html += "    <body>\n";
      html += "        <div>\n";
      html += "            <div class='loading-indicator' id='loading-indicator'></div>\n";
      html += "            <div id='loading-background' style='display: none;height: 100%;width: 100%;position: fixed;top: 0%;left: 0%;background: rgba(0, 0, 0, 0.5);z-index: 1;' ></div>\n";
      html += "        </div>\n";
      html += "        <h3>Water Timer</h3>\n";
      html += "        <div class='container' id='buttonContainer1' name='buttonContainer1' ></div>\n";
      html += "        <h3>Light Timer</h3>\n";
      html += "        <div class='container2' id='buttonContainer2' name='buttonContainer2'></div>\n";
      html += "        <div class='confirm-button'>\n";
      html += "            <button id='confirmButton' >Confirm</button>\n";
      html += "        </div>\n";
      html += "        <script>\n";
      html += "            var recievedWaterTimer = [];\n";
      html += "            var recievedLightTimer = [];\n";
      html += "            const getWaterPromise = fetch('/getWaterTimer')\n";
      html += "                .then(response => response.json())\n";
      html += "                .then((data) => {\n";
      html += "                    console.log(data);\n";
      html += "                    return data;\n";
      html += "                })\n";
      html += "                .catch(error => {\n";
      html += "                    console.error('Error:', error);\n";
      html += "                });\n";
      html += "\n";
      html += "            const getLightPromise = fetch('/getLightTimer')\n";
      html += "                .then(response => response.json())\n";
      html += "                .then((data) => {\n";
      html += "                    console.log(data);\n";
      html += "                    return data;\n";
      html += "                })\n";
      html += "                .catch(error => {\n";
      html += "                    console.error('Error:', error);\n";
      html += "                });\n";
      html += "\n";
      html += "            Promise.all([getWaterPromise, getLightPromise])\n";
      html += "                .then(([recievedWaterTimer, recievedLightTimer]) => {\n";
      html += "                    var buttonContainer1 = document.getElementById('buttonContainer1');\n";
      html += "                    var buttonContainer2 = document.getElementById('buttonContainer2');\n";
      html += "                    var rowCount = 8; \n";
      html += "                    var columnCount = 12; \n";
      html += "                    var startTime = new Date();\n";
      html += "                    startTime.setHours(0, 0, 0, 0); \n";
      html += "\n";
      html += "                    for (var i = 0; i < rowCount * columnCount; i++) {\n";
      html += "                        var button1 = document.createElement('button');\n";
      html += "                        button1.className = 'rectangle';\n";
      html += "                        var button2 = document.createElement('button');\n";
      html += "                        button2.className = 'rectangle';\n";
      html += "                        \n";
      html += "                        button1.value = false;\n";
      html += "                        button2.value = false;\n";
      html += "                        if (recievedWaterTimer[i] === true) {\n";
      html += "                            button1.style.backgroundColor = 'green';\n";
      html += "                            button1.value = true;\n";
      html += "                        }\n";
      html += "                        if (recievedLightTimer[i] === true) {\n";
      html += "                            button2.style.backgroundColor = 'green';\n";
      html += "                            button2.value = true;\n";
      html += "                        }\n";
      html += "                        button1.name = 'button1-' + (i);\n";
      html += "                        button2.name = 'button2-' + (i);\n";
      html += "                        button1.type = 'button';\n";
      html += "                        button2.type = 'button';\n";
      html += "                        var time = new Date(startTime.getTime() + i * 15 * 60 * 1000); \n";
      html += "                        var hours = time.getHours();\n";
      html += "                        var ampm = hours >= 12 ? 'PM' : 'AM';\n";
      html += "                        hours = hours % 12 || 12; \n";
      html += "                        var minutes = time.getMinutes().toString().padStart(2, '0');\n";
      html += "                        var title = hours + ':' + minutes + ' ' + ampm;\n";
      html += "                        button1.title = title;\n";
      html += "                        button2.title = title;\n";
      html += "                        \n";
      html += "                        var text1 = document.createElement('div');\n";
      html += "                        text1.className = 'text';\n";
      html += "                        text1.innerHTML = title;\n";
      html += "                        button1.appendChild(text1);\n";
      html += "                        \n";
      html += "                        var text2 = document.createElement('div');\n";
      html += "                        text2.className = 'text';\n";
      html += "                        text2.innerHTML = title;\n";
      html += "                        button2.appendChild(text2);\n";
      html += "                        \n";
      html += "                        buttonContainer1.appendChild(button1);\n";
      html += "                        buttonContainer2.appendChild(button2);\n";
      html += "                        button1.addEventListener('click', function() {\n";
      html += "                        if (this.style.backgroundColor === 'green') {\n";
      html += "                            this.style.backgroundColor = 'gray';\n";
      html += "                            this.value = false;\n";
      html += "                        } else {\n";
      html += "                            this.style.backgroundColor = 'green';\n";
      html += "                            this.value = true;\n";
      html += "                        }\n";
      html += "                        });\n";
      html += "                        \n";
      html += "                        button2.addEventListener('click', function() {\n";
      html += "                        if (this.style.backgroundColor === 'green') {\n";
      html += "                            this.style.backgroundColor = 'gray';\n";
      html += "                            this.value = false;\n";
      html += "                        } else {\n";
      html += "                            this.style.backgroundColor = 'green';\n";
      html += "                            this.value = true;\n";
      html += "                        }\n";
      html += "                        });\n";
      html += "                    }                \n";
      html += "            })\n";
      html += "            .catch(error => {\n";
      html += "                \n";
      html += "                console.error('Error:', error);\n";
      html += "            });\n";
      html += "        \n";
      html += "            var confirmButton = document.getElementById('confirmButton');\n";
      html += "            confirmButton.addEventListener('click', function() {\n";
      html += "                var buttonElements1 = buttonContainer1.getElementsByClassName('rectangle');\n";
      html += "                var buttonElements2 = buttonContainer2.getElementsByClassName('rectangle');\n";
      html += "                var buttonStates1 = getButtonStates(buttonElements1);\n";
      html += "                var buttonStates2 = getButtonStates(buttonElements2);\n";
      html += "                var buttonStates = buttonStates1.concat(buttonStates2);\n";
      html += "                var xhttp = new XMLHttpRequest();\n";
      html += "                var loadingIndicator = document.getElementById('loading-indicator');\n";
      html += "                var loadingBackground = document.getElementById('loading-background');\n";
      html += "\n";
      html += "                xhttp.onreadystatechange = function() {\n";
      html += "                    if (this.readyState === 4 && this.status === 200) {\n";
      html += "                        console.log('value sent successfully!');\n";
      html += "                        sendNextValue();\n";
      html += "                    }\n";
      html += "                };\n";
      html += "                function sendNextValue(index) {\n";
      html += "                    if (buttonStates.length > 0 ) {\n";
      html += "                        var i = buttonStates.length - 1;\n";
      html += "                        var value = buttonStates.pop();\n";
      html += "                         xhttp.open('POST', '/buttonstates', true);\n";
      html += "                         xhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');\n";
      html += "                        xhttp.send('index=' + encodeURIComponent(i) +'&value=' + encodeURIComponent(value));\n";
      html += "                        console.log( 'save - ' + (i) + ' - ' + value);\n";
//      html += "                         }\n";
      html += "                        showLoadingIndicator();\n";

      html += "                    } else {\n";
      html += "                        hideLoadingIndicator();\n";
      html += "                        console.log('All values sent!');\n";
      html += "                        xhttp.open('GET', '/commit-array', true);\n";
      html += "                        xhttp.send();\n";
      html += "                    }\n";
      html += "                }\n";
      html += "                function showLoadingIndicator() {\n";
      html += "                    loadingIndicator.style.display = 'block';\n";
      html += "                    loadingBackground.style.display = 'block';\n";
      html += "                }\n";
      html += "\n";
      html += "                function hideLoadingIndicator() {\n";
      html += "                    loadingIndicator.style.display = 'none';\n";
      html += "                    loadingBackground.style.display = 'none';\n";
      html += "                }\n";
      html += "                sendNextValue();\n";
      html += "                \n";
      html += "            });\n";
      html += "        \n";
      html += "            function getButtonStates(buttonElements) {\n";
      html += "                var buttonStates = [];\n";
      html += "                for (var i = 0; i < buttonElements.length; i++) {\n";
      html += "                    var button = buttonElements[i];\n";
      html += "                    var isGreen = button.style.backgroundColor === 'green';\n";
      html += "                    buttonStates.push(isGreen);\n";
      html += "                }\n";
      html += "                return buttonStates;\n";
      html += "            }\n";
      html += "        </script>\n";
      html += "    </body>\n";
      html += "</html>\n";
      html += "\n";

      server.send(200, "text/html", html);
    });

    server.on("/buttonstates", [&, storedSSID, storedPassword, &waterTimer, &lightTimer]() {
      if (server.method() == HTTP_POST) {
        
        String receivedIndex = server.arg("index");
        String receivedValue = server.arg("value");

          int index = receivedIndex.toInt();
          bool value = receivedValue == "true"? true : false;
          
          if(index < 96) {
            waterTimer[index] = value;
          } else {
            lightTimer[index%ARRAY_SIZE] = value;
          }
          
        
          saveAppDataToEEPROM(storedSSID, storedPassword, waterTimer, lightTimer);
        
        // Send a response back to the client
        server.send(200, "text/plain", "Successful");
  } else {
    // Invalid method
    server.send(405, "text/plain", "Method Not Allowed");
  }
  });

  server.on("/commit-array", [&, &waterTimer, &lightTimer]() {
        
        EEPROM.commit();
        Serial.println("Saved to eeprom");
        // Send a response back to the client
  });

  server.on("/getWaterTimer", [&, &waterTimer]() {handleSendArray(server, waterTimer);});
  server.on("/getLightTimer", [&, &lightTimer]() {handleSendArray(server, lightTimer);});
}

void setupAP(ESP8266WebServer& server, bool (&waterTimer)[], bool (&lightTimer)[]) {

  // Serve the captive portal landing page

    Serial.println("HTTP server started");

    server.on("/", HTTP_GET, [&]() {
      String html = "";
      html = "<html><body>";
      html += "<h2>Wi-Fi Configuration</h2>";
      html += "<form method='POST' action='/save'>";
      html += "SSID: <input type='text' name='ssid'><br>";
      html += "Password: <input type='password' name='password'><br>";
      html += "<input type='submit' value='Save'>";
      html += "</form>";
      html += "</body></html>";
      server.send(200, "text/html", html);
    });

    server.on("/save", HTTP_POST, [&]() {
      String ssid = server.arg("ssid");
      String password = server.arg("password");
      
      saveAppDataToEEPROM(ssid.c_str(), password.c_str(), waterTimer, lightTimer);
      
      String response = "Credentials saved: SSID = " + ssid + ", Password = " + password;
      server.send(200, "text/plain", response);
      ESP.restart();
  });

      server.onNotFound([&]() {
        server.send(302, "text/plain", "Redirecting...");
        server.sendHeader("Location", "/");
        server.sendContent("");
      });
  
  server.begin();
}
