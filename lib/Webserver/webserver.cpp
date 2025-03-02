#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <float.h> 
#include "main.h"
#include "webpage.h"
#include "rtc.h"
#include "webserver.h"
#include "filesystem.h"

AsyncWebServer server(80);

void initWebServer() {
    // Configura ponto de acesso Wi-Fi
    WiFi.softAP("ESP32-Config", "12345678");
    LOG("WEBSERVER", "Ponto de acesso criado.");
    LOG("WEBSERVER", "Conecte-se a rede ESP32-Config");
    IPAddress IP = WiFi.softAPIP();
    LOG("WEBSERVER", "AP IP: %u.%u.%u.%u", IP[0], IP[1], IP[2], IP[3]);

    // Rota principal para exibir a pagina HTML
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", webpageHTML);
    });

    // Rota para obter a data e hora atuais
    server.on("/getRTC", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", rtc.getDateTime());
    });

    server.on("/getNetworkInfo", HTTP_GET, [](AsyncWebServerRequest *request) {
        String ssid = (WiFi.getMode() == WIFI_STA) ? WiFi.SSID() : WiFi.softAPSSID();
    
        String json = "{";
        json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
        json += "\"gateway\":\"" + WiFi.gatewayIP().toString() + "\",";
        json += "\"subnet\":\"" + WiFi.subnetMask().toString() + "\",";
        json += "\"dns\":\"" + WiFi.dnsIP().toString() + "\",";
        json += "\"mac\":\"" + WiFi.macAddress() + "\",";
        json += "\"ssid\":\"" + ssid + "\"";
        json += "}";
    
        request->send(200, "application/json", json);
    });

    // Rota para obter o serial number
    server.on("/getSerialNumber", HTTP_GET, [](AsyncWebServerRequest *request) {
        uint32_t serialNumber = getSerialNumber();
        String snStr = String(serialNumber);
        request->send(200, "text/plain", snStr);
        LOG("WEBSERVER", "Serial Number enviado: %s", snStr.c_str());
    });    

    // Rota para definir os limites dos sensores
    server.on("/setLimits", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("tempMin", true) && request->hasParam("tempMax", true) &&
            request->hasParam("humMin", true) && request->hasParam("humMax", true)) {

            tempMin = request->getParam("tempMin", true)->value().toFloat();
            tempMax = request->getParam("tempMax", true)->value().toFloat();
            humMin = request->getParam("humMin", true)->value().toFloat();
            humMax = request->getParam("humMax", true)->value().toFloat();

            LOG("WEBSERVER", "Novos limites: TempMin=%.1f, TempMax=%.1f, HumMin=%.1f, HumMax=%.1f",
                tempMin, tempMax, humMin, humMax);

            request->send(200, "application/json", "{\"status\":\"ok\", \"message\":\"Limites atualizados!\"}");
        } else {
            request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Parâmetros ausentes!\"}");
            LOG("ERRO", "Parâmetros ausentes na requisicao.");
        }
    });

    // Rota para obter os limites atuais
    server.on("/getLimits", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = "{";
        json += "\"tempMin\":" + String(tempMin, 1) + ",";
        json += "\"tempMax\":" + String(tempMax, 1) + ",";
        json += "\"humMin\":" + String(humMin, 1) + ",";
        json += "\"humMax\":" + String(humMax, 1);
        json += "}";

        LOG("WEBSERVER", "Enviando limites: %s", json.c_str());
        request->send(200, "application/json", json);
    });

    // Inicia o servidor
    server.begin();
    LOG("WEBSERVER", "Servidor iniciado.");
}

void checkAlerts() {
    char message[100];

    if (temperature < tempMin || temperature > tempMax) {
        snprintf(message, sizeof(message),
                 "Temperatura %.2f Celsius fora do limite (%.2f - %.2f)!",
                 temperature, tempMin, tempMax);
        LOG("ALERTA", "%s", message);
    } else {
        LOG("ALERTA", "Temperatura dentro dos limites.");
    }

    if (humidity < humMin || humidity > humMax) {
        snprintf(message, sizeof(message),
                 "Umidade %.2f%% fora do limite (%.2f - %.2f)!",
                 humidity, humMin, humMax);
        LOG("ALERTA", "%s", message);
    } else {
        LOG("ALERTA", "Umidade dentro dos limites.");
    }
}