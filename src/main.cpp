/* Bibliotecas externas */
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <BH1750.h>
#include <WiFi.h>
#include <PubSubClient.h>

/* Bibliotecas internas */
#include "main.h"
#include "filesystem.h"
#include "webserver.h"
#include "rtc.h"
#include "tasks/task_monitor.h"

/* Pinagem do projeto */
#define PIN_DHT 26
#define PIN_SDA 21
#define PIN_SCL 22

//const char* ssid = "GMS 2.4G";
//const char* password = "GMS271931@";
const char* ssid = "igoal_24G";
const char* password = "igoal@2021";

/* MQTT */
WiFiClient espClient;
PubSubClient mqttClient(espClient);
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

/* Sensores e atuadores*/
RTC rtc;
DHT dht(PIN_DHT, DHT22);
BH1750 lightMeter;

/* Variáveis globais para sensores */
float temperature = 0.0;
float humidity = 0.0;
float lux = 0.0;

/* Protótipo das funções */
void initSensors();
void initMQTT();
void reconnectMQTT();
void initRTC();

/* Protótipos das tasks e handlers*/
void taskDHT(void *parameter);
void taskBH1750(void *parameter);
void taskMQTT(void *parameter);
TaskHandle_t taskMQTTHandle = NULL;

void setup() 
{
  Serial.begin(115200);
  Wire.begin(PIN_SDA, PIN_SCL);

  Serial.print("/******************************************************************************\n");
  Serial.print(" * Projeto: Dispositivo de Telemetria para Agricultura de Precisao e\n" );
  Serial.print(" *          Monitoramento Ambiental\n");
  Serial.print(" * Autor: Guilherme Martins Specht\n");
  Serial.print(" * Orientador: Julio Cesar Marques de Lima\n");
  Serial.print(" * Data: \n");
  Serial.print(" * Descricao: Firmware de coleta e analise de dados\n");
  Serial.print("/******************************************************************************\n");

  initSensors();
  initWebServer();
  initFileSystem();
  
  // Tasks dos sensores
  xTaskCreate(taskDHT, "taskDHT", 2048, NULL, 1, NULL);
  //xTaskCreate(taskBH1750, "taskBH1750", 2048, NULL, 1, NULL);

  // Tasks monitoras
  xTaskCreate(taskMonitorTasks, "taskMonitorTasks", 2048, NULL, 1, NULL); // Task para monitorar execução
  xTaskCreate(taskMonitorMemory, "taskMonitorMemory", 2048, NULL, 1, NULL);
}

void loop() 
{
  static bool connected = false;

  // Tenta conectar ao WiFi até ter sucesso
  if (!connected)
  {
      LOG("WIFI", "Tentando conectar ao Wi-Fi: %s", ssid);
      WiFi.begin(ssid, password);

      int retries = 0;
      const int maxRetries = 10;

      while (WiFi.status() != WL_CONNECTED && retries < maxRetries)
      {
          LOG("WIFI", "Tentativa %d de %d", retries + 1, maxRetries);
          delay(5000);
          retries++;
      }

      if (WiFi.status() == WL_CONNECTED)
      {
          LOG("WIFI", "Conectado com sucesso!");
          LOG("WIFI", "SSID: %s", ssid);
          LOG("WIFI", "Senha: %s", password);
          LOG("WIFI", "Endereco IP: %s", WiFi.localIP().toString().c_str());
          connected = true;
          initRTC();
          initMQTT();
      }
      else
      {
          LOG("WIFI", "Falha ao conectar. Tentando novamente...");
      }
      delay(1000);
  }
  
  // Tenta conectar no broker até ter sucesso
  if (!mqttClient.connected())
  {
    reconnectMQTT();
  }
    mqttClient.loop(); // Mantém a conexão MQTT ativa
}

void initSensors()
{
    dht.begin();
    delay(1000);
    if (isnan(dht.readTemperature()) || isnan(dht.readHumidity())) {
        LOG("DHT22", "Falha ao inicializar o sensor DHT22.");
    } 
    else
    {
        LOG("DHT22", "Sensor DHT22 inicializado com sucesso.");
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

void initMQTT()
{
    mqttClient.setServer(mqtt_server, mqtt_port);
    reconnectMQTT();
}

void reconnectMQTT()
{
    while (!mqttClient.connected())
    {
        LOG("MQTT", "Tentando conectar ao broker...");
        if (mqttClient.connect("ESP32_Client"))
        {
            LOG("MQTT", "Conectado ao broker com sucesso!");

            // Se a task não estiver rodando, crie-a novamente
            if (taskMQTTHandle == NULL)
            {
                xTaskCreate(taskMQTT, "taskMQTT", 4096, NULL, 1, &taskMQTTHandle);
                LOG("MQTT", "Task MQTT criada.");
            }
        }
        else
        {
            LOG("MQTT", "Falha na conexao ao broker. Tentando novamente...");
            
            // Se a conexão foi perdida, exclua a task MQTT
            if (taskMQTTHandle != NULL)
            {
                LOG("MQTT", "Desativando Task MQTT...");
                vTaskDelete(taskMQTTHandle);
                taskMQTTHandle = NULL; // Resetar o handle da task
            }

            delay(5000); // Aguarde antes de tentar reconectar
        }
    }
}

void initRTC() 
{
    const char* ntpServer = "pool.ntp.org";
    long gmtOffsetSec = -10800;
    int daylightOffsetSec = 3600;

    rtc.begin(ntpServer, gmtOffsetSec, daylightOffsetSec);
    LOG("RTC", "Hora atual após sincronizacao: %s", rtc.getDateTime().c_str());
}

void taskDHT(void *parameter)
{
  LOG("DHT22", "Task DHT inicializada");
  while (true)
  {
      startTaskTimer(MONITOR_DHT);
      temperature = dht.readTemperature();
      humidity = dht.readHumidity();

      if (isnan(temperature) || isnan(humidity))
      {
          LOG("DHT22", "Falha ao ler do sensor DHT22");
      }
      else
      {
          LOG("DHT22", "Temperatura: %.2f Celsius, Umidade: %.2f %%", temperature, humidity);
      }
      vTaskDelay(pdMS_TO_TICKS(15000));
      endTaskTimer(MONITOR_DHT);
  }
  LOG("DHT22", "Task DHT finalizada");
}

void taskBH1750(void *parameter)
{
    while (true)
    {
        startTaskTimer(MONITOR_BH1750);

        // Leitura da luminosidade
        lux = lightMeter.readLightLevel();

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

void taskMQTT(void *parameter)
{
    LOG("MQTT", "Task MQTT inicializada");
    while (true)
    {
        startTaskTimer(MONITOR_MQTT);
        // Publica a temperatura
        char tempPayload[20];
        snprintf(tempPayload, sizeof(tempPayload), "%.2f", temperature);
        if (mqttClient.publish("tcc/sensores/temperatura", tempPayload))
        {
            LOG("MQTT", "Temperatura publicada com sucesso!");
        }
        else
        {
            LOG("MQTT", "Falha ao publicar temperatura!");
        }

        // Publica a umidade
        char humPayload[20];
        snprintf(humPayload, sizeof(humPayload), "%.2f", humidity);
        if (mqttClient.publish("tcc/sensores/umidade_do_ar", humPayload))
        {
            LOG("MQTT", "Umidade do ar publicada com sucesso!");
        }
        else
        {
            LOG("MQTT", "Falha ao publicar umidade do ar!");
        }

        vTaskDelay(pdMS_TO_TICKS(15000)); // Publica a cada 15 segundos
        endTaskTimer(MONITOR_MQTT);
    }
}