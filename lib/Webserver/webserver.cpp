#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
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

    // Inicia o servidor
    server.begin();
    LOG("WEBSERVER", "Servidor iniciado.");
}
