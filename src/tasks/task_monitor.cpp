#include <Arduino.h>
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
        LOG("MONITOR", "Task Name\tExecution Time (ms)\tExecution Count");
        LOG("MONITOR", "--------------------------------------------------------------------------------------------------");
        for (int i = 0; i < MONITOR_COUNT - 1; i++) { // Ajuste para evitar acessar MONITOR_COUNT
            if (taskTimes[taskInfoArray[i].task].executionTime == 0 && taskTimes[taskInfoArray[i].task].executionCount == 0) {
                continue; // Não imprime tasks que nunca foram iniciadas
            }
            uint32_t executionTimeMs = taskTimes[taskInfoArray[i].task].executionTime * portTICK_PERIOD_MS;
            LOG("MONITOR", "%s\t\t%u\t\t\t%u", taskInfoArray[i].name, executionTimeMs, taskTimes[taskInfoArray[i].task].executionCount);

            // Verifica se a task está travada (tempo atual - tempo de início > 10 minutos)
            if ((xTaskGetTickCount() - taskTimes[taskInfoArray[i].task].startTime) > (10 * 60 * 1000 / portTICK_PERIOD_MS)) { // 10 minutos
                LOG("MONITOR", "Reiniciando dispositivo devido a task travada: %s", taskInfoArray[i].name);
                ESP.restart();
            }
        }
        vTaskDelay(30000 / portTICK_PERIOD_MS); // 10 segundos
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
