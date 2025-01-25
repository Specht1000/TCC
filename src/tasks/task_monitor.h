#ifndef TASKS_MONITOR_H
#define TASKS_MONITOR_H

#include <Arduino.h>

// Enumerado para identificar as tasks
enum TASKS_TIMER {
    MONITOR_LOOP = 1,
    MONITOR_DHT,
    MONITOR_BH1750,
    MONITOR_MQTT,
    MONITOR_COUNT
};

// Estrutura para associar o valor do enumerado e o nome da task
typedef struct {
    TASKS_TIMER task;
    const char* name;
} TaskInfo;

// Array de TaskInfo para associar os valores do enumerado aos nomes das tasks
const TaskInfo taskInfoArray[MONITOR_COUNT] = {
    {MONITOR_LOOP,      "LOOP  "},
    {MONITOR_DHT,       "DHT   "},
    {MONITOR_BH1750,    "BH1750"},
    {MONITOR_MQTT,      "MQTT  "}
};

// Estrutura para armazenar o tempo de execução e o contador de execuções de cada task
typedef struct {
    TickType_t startTime;
    TickType_t endTime;
    TickType_t executionTime;
    uint32_t executionCount;
} TaskExecutionTime;

void taskMonitorTasks(void *pvParameters);
void startTaskTimer(TASKS_TIMER task);
void endTaskTimer(TASKS_TIMER task);
void taskMonitorMemory(void *pvParameters);

#endif // TASKS_MONITOR_H