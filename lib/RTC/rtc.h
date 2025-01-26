#ifndef RTC_H
#define RTC_H

#include <Arduino.h>

class RTC {
public:
    void begin(const char* ntpServer, long gmtOffsetSec, int daylightOffsetSec);
    String getDateTime();
    String getDate();
    String getTime();
    unsigned long getEpochTime();
};

#endif // RTC_H
