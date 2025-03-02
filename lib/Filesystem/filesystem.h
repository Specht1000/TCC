#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <SPIFFS.h>
#include "main.h"

extern float tempMin;
extern float tempMax;
extern float humMin;
extern float humMax;

extern const char* ssid;
extern const char* password;

extern const char* mqtt_server;
extern const int mqtt_port;

void initFileSystem();
int32_t initNVS();
bool saveSerialNumber(uint32_t serialNumber);
uint32_t getSerialNumber();
uint32_t generateSerialNumber();
void ensureSerialNumber();

#endif // FILESYSTEM_H
