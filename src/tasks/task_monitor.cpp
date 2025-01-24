#include <Arduino.h>
#include <SPIFFS.h>
#include <esp_spiffs.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "task_monitor.h"
#include "main.h"

// Array para armazenar os tempos de execução das tasks
TaskExecutionTime taskTimes[MONITOR_COUNT];

// Task para monitorar os tempos de execução das tasks e reiniciar o dispositivo se necessário
void taskMonitorTasks(void *pvParameters) {
    delay(5000);
    while (true) {
        LOG("MONITOR", "--------------------------------------------------------------------------------------------------");
        LOG("MONITOR", "Task Name\tExecution Time (ms)\tExecution Count");
        LOG("MONITOR", "--------------------------------------------------------------------------------------------------");
        for (int i = 0; i < MONITOR_COUNT - 1; i++) { // Ajuste para evitar acessar MONITOR_COUNT
            if (taskTimes[taskInfoArray[i].task].executionTime == 0 && taskTimes[taskInfoArray[i].task].executionCount == 0) {
                continue; // Não imprime tasks que nunca foram iniciadas
            }
            uint32_t executionTimeMs = taskTimes[taskInfoArray[i].task].executionTime * portTICK_PERIOD_MS;
            LOG("MONITOR", "%s\t\t%u\t\t\t%u", taskInfoArray[i].name, executionTimeMs, taskTimes[taskInfoArray[i].task].executionCount);

            // Verifica se a task está travada (tempo atual - tempo de início > 10 minutos)
            if ((xTaskGetTickCount() - taskTimes[taskInfoArray[i].task].startTime) > (10 * 60 * 1000 / portTICK_PERIOD_MS)) 
            {
                LOG("MONITOR", "Reiniciando dispositivo devido a task travada: %s", taskInfoArray[i].name);
                ESP.restart();
            }
        }
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
}

// Função para registrar o tempo de início de uma task
void startTaskTimer(TASKS_TIMER task) {
    taskTimes[task].startTime = xTaskGetTickCount();
}
// Função para registrar o tempo de término de uma task
void endTaskTimer(TASKS_TIMER task) {
    taskTimes[task].endTime = xTaskGetTickCount();
    taskTimes[task].executionTime = taskTimes[task].endTime - taskTimes[task].startTime;
    taskTimes[task].executionCount++;
}

void taskMonitorMemory(void *parameter)
{
    delay(5000);
    while (true)
    {
        // Informações da RAM (Heap)
        size_t totalBytesHeap = ESP.getHeapSize();
        size_t freeBytesHeap = ESP.getFreeHeap();
        size_t usedBytesHeap = totalBytesHeap - freeBytesHeap;
        float heapUsagePercentage = ((float)usedBytesHeap / totalBytesHeap) * 100.0;

        // Informações do Flash (SPIFFS)
        size_t totalBytesFlash = 0, usedBytesFlash = 0;
        esp_err_t ret = esp_spiffs_info(NULL, &totalBytesFlash, &usedBytesFlash);
        if (ret != ESP_OK)
        {
            LOG("MONITOR", "Erro ao obter informações do SPIFFS: %s", esp_err_to_name(ret));
        }
        size_t freeBytesFlash = totalBytesFlash - usedBytesFlash;
        float flashUsagePercentage = ((float)usedBytesFlash / totalBytesFlash) * 100.0;

        // Exibindo a tabela de memória alinhada
        LOG("MONITOR", "--------------------------------------------------------------------------------------------------");
        LOG("MONITOR", "| Memory Type      | Total (bytes) | Used (bytes) | Free (bytes) | Usage (%%) |");
        LOG("MONITOR", "--------------------------------------------------------------------------------------------------");
        LOG("MONITOR", "| RAM (Heap)       | %13zu | %12zu | %12zu | %9.2f%% |", totalBytesHeap, usedBytesHeap, freeBytesHeap, heapUsagePercentage);
        LOG("MONITOR", "| Flash (SPIFFS)   | %13zu | %12zu | %12zu | %9.2f%% |", totalBytesFlash, usedBytesFlash, freeBytesFlash, flashUsagePercentage);
        LOG("MONITOR", "--------------------------------------------------------------------------------------------------");

        // Atraso de 2 minutos
        vTaskDelay(pdMS_TO_TICKS(120000));
    }
}
