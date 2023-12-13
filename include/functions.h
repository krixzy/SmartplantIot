#include <ArduinoJson.h>
#include <Arduino_MKRIoTCarrier.h>


DynamicJsonDocument updateValues();
void showQrCode();
void updateUserInterface( bool wifiConnected);
void sendToApi(String data);
void arduinoApInstruction(MKRIoTCarrier carrier);
void arduinoApConnectionText(MKRIoTCarrier carrier);
void printNertworkData();
void connectWifi();
void connectAp();
void calibratedMoistureSensor();
void sendMqttMessage(String topic, String value);
DynamicJsonDocument readMoistureSensor();
DynamicJsonDocument readTemperature();
DynamicJsonDocument readLight();
DynamicJsonDocument readHumidity();
