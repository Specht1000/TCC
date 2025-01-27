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

    // Rota para configurar a data e hora do RTC
    server.on("/setRTC", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("year", true) && request->hasParam("month", true) &&
            request->hasParam("day", true) && request->hasParam("hour", true) &&
            request->hasParam("minute", true) && request->hasParam("second", true)) {

            int year = request->getParam("year", true)->value().toInt();
            int month = request->getParam("month", true)->value().toInt();
            int day = request->getParam("day", true)->value().toInt();
            int hour = request->getParam("hour", true)->value().toInt();
            int minute = request->getParam("minute", true)->value().toInt();
            int second = request->getParam("second", true)->value().toInt();

            rtc.setRTC(year, month, day, hour, minute, second);
            LOG("RTC", "Data e hora ajustadas: %s", rtc.getDateTime().c_str());

            request->send(200, "text/html", "<h2>Data e hora ajustadas com sucesso!</h2>");
        } else {
            request->send(400, "text/html", "<h2>Erro: Campos inválidos.</h2>");
        }
    });

    // Rota para obter a data e hora atuais
    server.on("/getRTC", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", rtc.getDateTime());
    });
    
    // Inicia o servidor
    server.begin();
    LOG("WEBSERVER", "Servidor iniciado.");
}
