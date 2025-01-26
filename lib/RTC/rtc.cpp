#include <time.h>
#include "RTC.h"
#include "main.h"

// Inicializa o RTC com sincronização NTP
void RTC::begin(const char* ntpServer, long gmtOffsetSec, int daylightOffsetSec) {
    configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);
    LOG("RTC", "Sincronizando com o servidor NTP...");
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo)) {
        LOG("RTC", "Falha ao obter a hora.");
        return;
    }
    LOG("RTC", "Sincronizacao concluida.");
}

// Obtém a data e hora atual no formato "YYYY-MM-DD HH:MM:SS"
String RTC::getDateTime() {
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo)) {
        LOG("RTC", "Hora indisponível");
        return "";
    }
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
    String dateTime = String(buffer);
    LOG("RTC", "Data e Hora atual: %s", dateTime.c_str());
    return dateTime;
}

// Obtém apenas a data atual no formato "YYYY-MM-DD"
String RTC::getDate() {
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo)) {
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
String RTC::getTime() {
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo)) {
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
unsigned long RTC::getEpochTime() {
    unsigned long epochTime = time(nullptr);
    LOG("RTC", "Timestamp atual (epoch time): %lu", epochTime);
    return epochTime;
}
