#include <time.h>
#include "RTC.h"
#include "main.h"
#include "esp_task_wdt.h"

// Inicializa o RTC com sincronização NTP
void RTC::begin(const char* ntpServer, long gmtOffsetSec, int daylightOffsetSec) 
{
    configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);
    LOG("RTC", "Sincronizando com o servidor NTP...");
    delay(2000); // Aguarde a sincronização do NTP

    struct tm timeInfo;
    if (getLocalTime(&timeInfo)) 
    {
        LOG("RTC", "Sincronizacao concluida: %s", getDateTime().c_str());
    } 
    else 
    {
        LOG("RTC", "Falha ao sincronizar com o servidor NTP.");
    }
}

// Obtém a data e hora atual no formato "YYYY-MM-DD HH:MM:SS"
String RTC::getDateTime() 
{
    struct tm timeInfo;
    if (getLocalTime(&timeInfo)) 
    {
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
        return String(buffer);
    } 
    else 
    {
        LOG("RTC", "Hora indisponível.");
        return "Indisponível";
    }
}

// Obtém apenas a data atual no formato "YYYY-MM-DD"
String RTC::getDate()
{
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo))
    {
        LOG("RTC", "Data indisponível");
        return "";
    }
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeInfo);
    String date = String(buffer);
    LOG("RTC", "Data atual: %s", date.c_str());
    return date;
}

// Obtém apenas a hora atual no formato "HH:MM:SS"
String RTC::getTime() 
{
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo)) 
    {
        LOG("RTC", "Hora indisponivel");
        return "";
    }
    char buffer[9];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeInfo);
    String time = String(buffer);
    LOG("RTC", "Hora atual: %s", time.c_str());
    return time;
}

// Retorna o timestamp atual (epoch time)
unsigned long RTC::getEpochTime() 
{
    unsigned long epochTime = time(nullptr);
    LOG("RTC", "Timestamp atual (epoch time): %lu", epochTime);
    return epochTime;
}

void RTC::setRTC(int year, int month, int day, int hour, int minute, int second) 
{
    struct tm timeInfo = {};
    timeInfo.tm_year = year - 1900;
    timeInfo.tm_mon = month - 1;
    timeInfo.tm_mday = day;
    timeInfo.tm_hour = hour;
    timeInfo.tm_min = minute;
    timeInfo.tm_sec = second;

    time_t t = mktime(&timeInfo);
    struct timeval now = { .tv_sec = t, .tv_usec = 0 };
    settimeofday(&now, nullptr);

    LOG("RTC", "Data e hora ajustadas para: %s", getDateTime().c_str());
}