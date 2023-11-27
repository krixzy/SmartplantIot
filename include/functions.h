#include <ArduinoJson.h>



void printNertworkData();
void connectWifi();
void connectAp();
void calibratedMoistureSensor();
void sendMqttMessage(String topic, String value);
DynamicJsonDocument readMoistureSensor();
DynamicJsonDocument readTemperature();
DynamicJsonDocument readLight();
DynamicJsonDocument readHumidity();
