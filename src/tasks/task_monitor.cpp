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
    delay(15000); // Delay inicial para permitir que outras tasks sejam iniciadas
    while (true) {
        LOG("MONITOR", "Task Name\tExecution Time (ms)\tExecution Count\t\tStuck Status");
        LOG("MONITOR", "------------------------------------------------------------------------------------------------");
        for (int i = 0; i < MONITOR_COUNT; i++) { // Verifica todas as tasks
            if (taskTimes[taskInfoArray[i].task].executionTime == 0 && taskTimes[taskInfoArray[i].task].executionCount == 0) {
                continue; // Não imprime tasks que nunca foram iniciadas
            }

            // Calcula o tempo de execução em milissegundo
            uint32_t executionTimeMs = pdTICKS_TO_MS(xTaskGetTickCount() - taskTimes[taskInfoArray[i].task].startTime);

            if(taskTimes[taskInfoArray[i].task].executionCount != taskTimes[taskInfoArray[i].task].executionCountCompare)
            {
                executionTimeMs = pdTICKS_TO_MS(taskTimes[taskInfoArray[i].task].executionTime);
            }

            LOG("MONITOR", "%s\t\t%u\t\t\t%u\t\t\t%s",
                      taskInfoArray[i].name,
                      executionTimeMs,
                      taskTimes[taskInfoArray[i].task].executionCount,
                      taskTimes[taskInfoArray[i].task].isStuck ? "STUCK" : "OK");

            // Verifica se a task está travada (tempo atual - tempo de início > 10 minutos)
            if (executionTimeMs > (60 * 1000)) { // 10 minutos
                if (!taskTimes[taskInfoArray[i].task].isStuck) {
                    // Marca a task como travada
                    taskTimes[taskInfoArray[i].task].isStuck = true;
                }
                if(executionTimeMs > (10 * 60 * 1000))
                {
                    delay(2000);
                    // Reinicia o dispositivo após 10 minutos de task travada
                    LOG("MONITOR", "Reiniciando dispositivo devido a task travada: %s", taskInfoArray[i].name);
                    ESP.restart();
                }
            }
            taskTimes[taskInfoArray[i].task].executionCountCompare = taskTimes[taskInfoArray[i].task].executionCount;
        }
        // Chama a função para logar o uso de memória
        logMemoryUsage();
        vTaskDelay(30000 / portTICK_PERIOD_MS); // Verifica a cada 1 segundo
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

void logMemoryUsage() {
    // RAM do microcontrolador (heap)
    size_t totalBytesHeap = ESP.getHeapSize();
    size_t freeBytesHeap = ESP.getFreeHeap();
    size_t usedBytesHeap = totalBytesHeap - freeBytesHeap;
    float usedPercentageHeap = ((float)usedBytesHeap* 100) / (float)totalBytesHeap ;

    // Flash SPI
    size_t totalBytes = SPIFFS.totalBytes();
    size_t usedBytes = SPIFFS.usedBytes();
    float usedPercentageFlash = ((float)usedBytes * 100) / (float)totalBytes;

    // Log de uso de memória formatado em tabela
    LOG("MONITOR", "------------------------------------------------------------------------------------------------");
    LOG("MONITOR", "Memoria\t\t\tLivre(b)\t\tUsada(b)\t\tPorcentagem de Uso");
    LOG("MONITOR", "------------------------------------------------------------------------------------------------");
    LOG("MONITOR", "Heap (RAM interna)\t%u\t\t%u\t\t\t%.2f%%", freeBytesHeap, usedBytesHeap, usedPercentageHeap);
    LOG("MONITOR", "SPIFFS (Flash)\t\t%u\t%u\t\t\t%.2f%%", totalBytes - usedBytes, usedBytes, usedPercentageFlash);

    vTaskDelay(30000 / portTICK_PERIOD_MS);
}
