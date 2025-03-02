#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <PubSubClient.h>

extern PubSubClient mqttClient;

void initWebServer();
void checkAlerts();

#endif
