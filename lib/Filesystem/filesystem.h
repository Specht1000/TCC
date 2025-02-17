#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <SPIFFS.h>
#include "main.h"

void initFileSystem();
int32_t initNVS();
bool saveSerialNumber(uint32_t serialNumber);
uint32_t getSerialNumber();
uint32_t generateSerialNumber();
void ensureSerialNumber();

#endif // FILESYSTEM_H
