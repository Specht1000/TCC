/* Bibliotecas externas */
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <WiFi.h>

/* Bibliotecas internas */
#include "main.h"
#include "tasks/task_monitor.h"

/* Pinagem do projeto */
#define PIN_DHT 26

const char* ssid = "GMS 2.4G";
const char* password = "GMS271031@";

DHT dht(PIN_DHT, DHT22);

/* Protótipos das tasks */
void taskDHT(void *parameter);

void setup() 
{
  Serial.begin(115200);
  
  dht.begin();
  
  xTaskCreate(taskDHT, "taskDHT", 2048, NULL, 1, NULL);
}

void loop() 
{
  static bool connected = false;

  // Tenta conectar ao Wi-Fi até ter sucesso
  if (!connected)
  {
      LOG("WIFI", "Tentando conectar ao Wi-Fi: %s", ssid);
      WiFi.begin(ssid, password);

      int retries = 0;
      const int maxRetries = 10;

      while (WiFi.status() != WL_CONNECTED && retries < maxRetries)
      {
          LOG("WIFI", "Tentativa %d de %d", retries + 1, maxRetries);
          delay(1000);
          retries++;
      }

      if (WiFi.status() == WL_CONNECTED)
      {
          LOG("WIFI", "Conectado com sucesso!");
          LOG("WIFI", "SSID: %s", ssid);
          LOG("WIFI", "Senha: %s", password);
          LOG("WIFI", "Endereço IP: %s", WiFi.localIP().toString().c_str());
          connected = true;
      }
      else
      {
          LOG("WIFI", "Falha ao conectar. Tentando novamente...");
      }
      delay(1000);
  }
}

void taskDHT(void *parameter)
{
  LOG("DHT", "Task DHT inicializada");
  while (true)
  {
      startTaskTimer(MONITOR_DHT);
      float temperature = dht.readTemperature();
      float humidity = dht.readHumidity();

      if (isnan(temperature) || isnan(humidity))
      {
          LOG("DHT", "Falha ao ler do sensor DHT22");
      }
      else
      {
          LOG("DHT", "Temperatura: %.2f °C, Umidade: %.2f %%", temperature, humidity);
      }
      vTaskDelay(pdMS_TO_TICKS(2000));
      endTaskTimer(MONITOR_DHT);
  }
  LOG("DHT", "Task DHT finalizada");
}
