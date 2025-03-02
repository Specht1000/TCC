#include "filesystem.h"
#include <nvs_flash.h>
#include <nvs.h>
#include <ArduinoJson.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

// Caminho do arquivo de configuração na SPIFFS
const char *configFilePath = "/config.json";

// Definição das variáveis globais
float tempMin = 15.0;
float tempMax = 35.0;
float humMin = 30.0;
float humMax = 80.0;

void initFileSystem() 
{
    if (!SPIFFS.begin(true)) 
    {
        LOG("SPIFFS", "Erro ao montar o sistema de arquivos SPIFFS!");
    } 
    else 
    {
        LOG("SPIFFS", "Sistema de arquivos SPIFFS inicializado com sucesso!");
    }
    
    initNVS();
}

int32_t initNVS()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    return err;
}

bool saveSerialNumber(uint32_t serialNumber)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return false;

    err = nvs_set_u32(my_handle, "serialNumber", serialNumber);
    if (err != ESP_OK) {
        nvs_close(my_handle);
        return false;
    }

    err = nvs_commit(my_handle);
    nvs_close(my_handle);
    return (err == ESP_OK);
}

uint32_t getSerialNumber()
{
    uint32_t serialNumber = 0;
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err == ESP_OK)
    {
        nvs_get_u32(my_handle, "serialNumber", &serialNumber);
        nvs_close(my_handle);
    }
    return serialNumber;
}

uint32_t generateSerialNumber()
{
    srand(time(NULL) ^ (uint32_t)esp_random());
    return rand() % 100000000;
}

void ensureSerialNumber() 
{
    uint32_t serialNumber = getSerialNumber();
    if (serialNumber == 0) 
    {
        // Gera um número serial aleatório baseado no MAC Address
        uint32_t mac = 0;
        uint8_t macAddr[6];
        esp_read_mac(macAddr, ESP_MAC_WIFI_STA);
        for (int i = 0; i < 4; i++) 
        {
            mac = (mac << 8) | macAddr[i];
        }
        serialNumber = mac & 0xFFFFFF; // Garante um número único a partir do MAC
        saveSerialNumber(serialNumber);
    } 
}