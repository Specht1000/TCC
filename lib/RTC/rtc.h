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
    void setRTC(int year, int month, int day, int hour, int minute, int second);
};

#endif // RTC_H
