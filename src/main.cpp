/* Bibliotecas externas */
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <BH1750.h>
#include <WiFi.h>

/* Bibliotecas internas */
#include "main.h"
#include "tasks/task_monitor.h"

/* Pinagem do projeto */
#define PIN_DHT 26

const char* ssid = "GMS 2.4G";
const char* password = "GMS271031@";

/* Sensores */
DHT dht(PIN_DHT, DHT22);
BH1750 lightMeter;

/* Protótipo das funções */
void init_sensors();

/* Protótipos das tasks */
void taskDHT(void *parameter);
void taskBH1750(void *parameter);

void setup() 
{
  Serial.begin(115200);

  init_sensors();
  
  xTaskCreate(taskDHT, "taskDHT", 2048, NULL, 1, NULL);
  xTaskCreate(taskBH1750, "taskBH1750", 2048, NULL, 1, NULL);
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

void init_sensors()
{
    if (isnan(dht.readTemperature()) || isnan(dht.readHumidity())) {
        LOG("DHT", "Falha ao inicializar o sensor DHT22.");
    } 
    else
    {
        LOG("DHT", "Sensor DHT22 inicializado com sucesso.");
    }
    
    if (lightMeter.begin())
    {
        LOG("BH1750", "Sensor BH1750 inicializado com sucesso.");
    }
    else
    {
        LOG("BH1750", "Falha ao inicializar o sensor BH1750.");
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

void taskBH1750(void *parameter)
{
    while (true)
    {
        startTaskTimer(MONITOR_BH1750);

        // Leitura da luminosidade
        float lux = lightMeter.readLightLevel();

        // Verifica se a leitura é válida
        if (lux == -1)
        {
            LOG("BH1750", "Falha ao ler do sensor BH1750");
        }
        else
        {
            LOG("BH1750", "Luminosidade: %.2f lx", lux);
        }

        // Delay para a próxima leitura (2 segundos)
        vTaskDelay(pdMS_TO_TICKS(2000));
        endTaskTimer(MONITOR_BH1750);
    }
}
