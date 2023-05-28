bool connectToWiFi(char* storedSSID, char* storedPassword, int arraySize, bool (&waterTimer)[], bool (&lightTimer)[]);
void dashboard(ESP8266WebServer& server, const char* storedSSID, const char* storedPassword, bool (&waterTimer)[], bool (&lightTimer)[]);
void handleAP(ESP8266WebServer& server);
void handleAPRedirect(ESP8266WebServer& server);
void setupAP(ESP8266WebServer& server, bool (&waterTimer)[], bool (&lightTimer)[]);
void configModeCallback(WiFiManager *myWiFiManager, ESP8266WebServer* server);
void handleSendArray(ESP8266WebServer& server, bool (&myArray)[]);
