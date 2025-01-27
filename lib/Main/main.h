#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "rtc.h"

extern RTC rtc;

#define ENABLE_MAIN_DEBUG

#ifdef ENABLE_MAIN_DEBUG
#define LOG(tag, fmt, ...) printf("[" tag "] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG(fmt, ...) // Nada
#endif

#endif // MAIN_H