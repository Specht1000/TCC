#include <Arduino.h>
#include <SPIFFS.h>
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
        // Informações sobre a RAM (Heap)
        uint32_t freeHeap = xPortGetFreeHeapSize();             // Memória livre em bytes
        uint32_t minFreeHeap = xPortGetMinimumEverFreeHeapSize(); // Memória mínima já registrada
        uint32_t totalHeap = ESP.getHeapSize();                 // Memória total (somente ESP32)
        uint32_t usedHeap = totalHeap - freeHeap;               // Memória usada
        float heapUsagePercentage = ((float)usedHeap / totalHeap) * 100.0; // Porcentagem usada

        // Informações sobre o Flash (SPIFFS)
        uint32_t totalFlash = SPIFFS.totalBytes(); // Total de bytes no sistema de arquivos SPIFFS
        uint32_t usedFlash = SPIFFS.usedBytes();   // Bytes usados no SPIFFS
        uint32_t freeFlash = totalFlash - usedFlash; // Bytes livres no SPIFFS
        float flashUsagePercentage = ((float)usedFlash / totalFlash) * 100.0; // Porcentagem usada

        // Exibindo a tabela de memória alinhada
        LOG("MONITOR", "--------------------------------------------------------------------------------------------------");
        LOG("MONITOR", "| Memory Type      | Total (bytes) | Used (bytes) | Free (bytes) | Usage (%%) |");
        LOG("MONITOR", "--------------------------------------------------------------------------------------------------");
        LOG("MONITOR", "| RAM (Heap)       | %13u | %12u | %12u | %9.2f%% |", totalHeap, usedHeap, freeHeap, heapUsagePercentage);
        LOG("MONITOR", "| Flash (SPIFFS)   | %13u | %12u | %12u | %9.2f%% |", totalFlash, usedFlash, freeFlash, flashUsagePercentage);
        LOG("MONITOR", "--------------------------------------------------------------------------------------------------");
        LOG("MONITOR", "| RAM Min. Livre Registrada: %u bytes", minFreeHeap);
        LOG("MONITOR", "--------------------------------------------------------------------------------------------------");

        vTaskDelay(pdMS_TO_TICKS(120000));
    }
}