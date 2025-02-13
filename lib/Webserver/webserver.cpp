#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "main.h"
#include "webpage.h"
#include "rtc.h"

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

    // Rota para obter as informações da rede
    server.on("/getNetworkInfo", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = "{";
        json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
        json += "\"gateway\":\"" + WiFi.gatewayIP().toString() + "\",";
        json += "\"subnet\":\"" + WiFi.subnetMask().toString() + "\",";
        json += "\"dns\":\"" + WiFi.dnsIP().toString() + "\",";
        json += "\"mac\":\"" + WiFi.macAddress() + "\"";
        json += "}";
        request->send(200, "application/json", json);
    });
    
    // Inicia o servidor
    server.begin();
    LOG("WEBSERVER", "Servidor iniciado.");
}
