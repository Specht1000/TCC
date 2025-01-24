#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "main.h"
#include "webserver.h"
#include "webpage.h"

// Instância do servidor
AsyncWebServer server(80);

void initWebServer() {
    // Configura ponto de acesso Wi-Fi
    WiFi.softAP("ESP32_AP", "12345678");
    LOG("WEBSERVER", "AP criado. Conecte-se ao Wi-Fi 'ESP32_AP'.");
    IPAddress IP = WiFi.softAPIP();
    LOG("WEBSERVER", "AP IP: %u.%u.%u.%u", IP[0], IP[1], IP[2], IP[3]);

    // Rota principal para exibir a página HTML
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", webpageHTML);
    });

    // Rota para configurar o Wi-Fi
    server.on("/setWiFi", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
            String ssid = request->getParam("ssid", true)->value();
            String password = request->getParam("password", true)->value();

            request->send(200, "text/html", "<h2>Conectando...</h2>");

            // Conectando ao Wi-Fi
            WiFi.begin(ssid.c_str(), password.c_str());
        } else {
            request->send(400, "text/html", "<h2>Erro: Campos vazios.</h2>");
        }
    });

    // Inicia o servidor
    server.begin();
    LOG("WEBSERVER", "Servidor Web iniciado.");
}
