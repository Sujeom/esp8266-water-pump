void readAppDataFromEEPROM(char* ssid, char* password, bool (&waterTimer)[], bool (&lightTimer)[]);
void saveAppDataToEEPROM(const char* ssid, const char* password, bool (&waterTimer)[], bool (&lightTimer)[]);
void saveArraysToEEPROM(int index, bool waterTimer[], bool lightTimer[]);
void readArraysFromEEPROM(bool waterTimer[], bool lightTimer[]);
void clearEEPROM(int size);
