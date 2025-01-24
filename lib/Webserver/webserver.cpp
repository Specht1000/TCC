#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "main.h"
#include "webpage.h"

// Instância do servidor
AsyncWebServer server(80);

void initWebServer() {
    // Configura ponto de acesso Wi-Fi
    WiFi.softAP("ESP32-Config", "12345678");
    LOG("WEBSERVER", "Ponto de acesso criado.");
    LOG("WEBSERVER", "Conecte-se a rede 'ESP32-Config'");
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

            WiFi.begin(ssid.c_str(), password.c_str());
            Serial.println("Tentando conectar ao Wi-Fi...");

            // Aguarda a conexão
            unsigned long startTime = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
                delay(500);
                Serial.print(".");
            }

            // Verifica se a conexão foi bem-sucedida
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("\nConexão bem-sucedida!");
                Serial.print("IP do dispositivo: ");
                Serial.println(WiFi.localIP());
                request->send(200, "text/html", "<h2>Conexão bem-sucedida!</h2><p>O dispositivo está conectado à rede Wi-Fi.</p>");
            } else {
                Serial.println("\nErro ao conectar.");
                request->send(200, "text/html", "<h2>Erro ao conectar.</h2><p>Verifique o SSID e a senha e tente novamente.</p>");
            }
        } else {
            request->send(400, "text/html", "<h2>Erro: Campos vazios.</h2>");
        }
    });

    // Inicia o servidor
    server.begin();
    LOG("WEBSERVER", "Servidor iniciado.");
}
