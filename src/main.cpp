#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include <secrets.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include "functions.h"


MKRIoTCarrier carrier;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
WiFiServer server(80);
unsigned long previousMillis = 0;
const long interval = 1000 * 60 * 60 * 3;
int dry;
int wet;

const char user[] = MQTTUSER;
const char password[] = MQTTPASS;
const char broker[] = MQTT_SERVER;
int        port     = MQTT_PORT;


WiFiSSLClient wifiSSLClient;
MqttClient mqttClient(wifiSSLClient);


void setup() {
  Serial.begin(9600);
  while (!Serial);
  carrier.withCase();
  carrier.begin();
  carrier.display.setTextSize(2);
  delay(10000);
  Serial.println(carrier.Light.colorAvailable()); 
  // calibratedMoistureSensor();
  // readLight();
  // connectWifi();

}

void loop() {
  if(millis() - previousMillis >= interval){
    previousMillis = millis();
    connectWifi();
  }

  delay(10000);
  // connectWifi();
}

void connectAp() {
  server.begin();
  WiFi.beginAP("ArduinoWiFi", "12345678");
  bool apOpen = true;
  while (apOpen) {
    WiFiClient client = server.available();
    if (client) {
      String currentLine = "";
      String postData = "";
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          currentLine += c;
          if (c == '\n') {
            // if the line is empty, this is the end of the client's request
            if (currentLine.length() == 2) {
              // send a standard http response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              client.println();
              client.println("<!DOCTYPE HTML>");
              client.println("<html>");
              client.println("<head>");
              client.println("<style>");
              client.println("body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f0f0f0; }");
              client.println("form { max-width: 350px; margin: 50px auto; padding: 20px; background-color: #ffffff; border-radius: 8px; }");
              client.println("input[type='text'], input[type='password'] { width: 90%; padding: 10px; margin-bottom: 10px; border-radius: 4px; border: 1px solid #ccc; }");
              client.println("input[type='submit'] { width: 96%; padding: 10px; border-radius: 4px; border: none; color: #ffffff; background-color: #007BFF; cursor: pointer; }");
              client.println("input[type='submit']:hover { background-color: #0056b3; }");
              client.println("</style>");
              client.println("</head>");
              client.println("<body>");
              client.println("<form method='post' action='/'>");
              client.println("SSID: <input type='text' name='ssid'><br>");
              client.println("Password: <input type='password' name='pass'><br>");
              client.println("<input type='hidden' name='hiddenField' value='test'>");
              client.println("<input type='submit' value='Connect'>");
              client.println("</form>");
              client.println("</body>");
              client.println("</html>");
              break;
            } else {
              currentLine = "";
            }
          } else if (c == '\r') {
            // // if the current line is the Content-Length line, get the value
            if (currentLine.startsWith("Content-Length: ")) {
              while (client.read() != '\n');
              
             
                  postData = client.readStringUntil('\n');

                  if (postData.startsWith("ssid="))
                  {
                    Serial.println(postData);
                    int ssidStart = postData.indexOf("ssid=") + 5;
                    int ssidEnd = postData.indexOf("&", ssidStart);
                    String newSsid = postData.substring(ssidStart, ssidEnd);
                    int passStart = postData.indexOf("pass=") + 5;
                    int passEnd = postData.indexOf("&", passStart);
                    String newPassword = postData.substring(passStart, passEnd);
                    ssid[newSsid.length() + 1];
                    newSsid.toCharArray(ssid, newSsid.length() + 1);

                    pass[newPassword.length() + 1];
                    newPassword.toCharArray(pass, newPassword.length() + 1);
                    #define SECRET_SSID newSsid
                    #define SECRET_PASS newPassword
                    Serial.println(ssid);
                    Serial.println(pass);
                    client.stop();
                    apOpen = false;
                    connectWifi();

                  }

            }

          }

        }

      }
      // close the connection:
      

    }
  }
      Serial.println("client disconnected");

}




void connectWifi() {
    int counter = 10;
    WiFi.begin(ssid, pass);
    Serial.print("Connecting to ");
  while (WiFi.status() != WL_CONNECTED && counter-- > 0) {
    delay(4000);
    WiFi.begin(ssid, pass);
    Serial.print(counter);
  }
  if (WiFi.status() == WL_CONNECTED) {
    printNertworkData();
    DynamicJsonDocument doc(1024);
    doc["name"] = "Arduino MKR1010";
    doc["plant"] = "MKR1010";
    doc["temperature"][0] = readTemperature(); // Assign value to the first element of the array
    doc["humidity"][0] = readHumidity();
    doc["light"][0] = readLight();
    doc["soilmoisture"][0] = readMoistureSensor();
    doc["device_id"] = "MKR1010";
    
    String jsonString;
    // serializeJson(doc, Serial);
    // sendMqttMessage("device", jsonString);
    Serial.println(jsonString);
  } else {
    connectAp();
  
  }
  delay(8000);
  WiFi.disconnect();

}



void printNertworkData(){
  Serial.println("You're connected to the network");

  Serial.println("----------------------------------------");

  Serial.println("Board Information:");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
 
  Serial.println();
  Serial.println("Network Information:");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
}

 void calibratedMoistureSensor(){

  bool dryCalab = false;
  bool wetCalab = false;
  carrier.display.setCursor(25, 100);
  carrier.display.print("Place your sensor");
  carrier.display.setCursor(20, 120);
  carrier.display.print("on a dry surface.");
  carrier.display.setCursor(20, 140);
  carrier.display.print("and press the button");
  carrier.leds.setPixelColor(0, 0xF800);
  carrier.leds.show();
  
  while (!dryCalab)
  {
    carrier.Buttons.update();

    if(carrier.Buttons.onTouchDown(TOUCH0)){
      dry = analogRead(A6); 
      dryCalab = true;
      carrier.leds.clear();


    }
  }
  carrier.display.fillScreen(0);
  carrier.display.setCursor(25, 100);
  carrier.display.print("Place your sensor");
  carrier.display.setCursor(20, 120);
  carrier.display.print("in a glass of water.");
  carrier.display.setCursor(20, 140);
  carrier.display.print("and press the button");
  carrier.leds.setPixelColor(4, 0xF800);
  carrier.leds.show();
  while (!wetCalab)
  {
    carrier.Buttons.update();

    if(carrier.Buttons.onTouchDown(TOUCH4)){
      wet = analogRead(A6);
      wetCalab = true;
      carrier.leds.clear();
      carrier.leds.show();
      

    }
  }
  
  carrier.display.fillScreen(0);
  



 }



 DynamicJsonDocument readMoistureSensor(){
  int moisture = analogRead(A6);
  int moisturePercent = map(moisture, wet - 30, dry, 100, 0);
  DynamicJsonDocument moistJson(1024);
  moistJson["soilMoisturePercentage"] = (moisturePercent);
  return moistJson;
 }


 DynamicJsonDocument readTemperature(){
  int temp = carrier.Env.readTemperature();
  DynamicJsonDocument tempJson(1024);
  tempJson["temperatureDegrees"] = (temp -8);
  return tempJson;
 }

DynamicJsonDocument readLight(){
  int A, B, C, D;
  Serial.println(carrier.Light.colorAvailable());  
  if(carrier.Light.colorAvailable()){
  Serial.println("test123");

    carrier.Light.readColor(A, B, C, D);
    
    if (D > 120)
    {
      D = 120;
    }
    
    int lightProcent = map(D, 0, 120, 0, 100);
    DynamicJsonDocument lightJson(1024);
    lightJson["lightPercentage"] = (lightProcent);
    serializeJson(lightJson, Serial);
    // return lightJson;
  }
}
DynamicJsonDocument readHumidity(){
  int hum = carrier.Env.readHumidity();
  DynamicJsonDocument humJson(1024);
  humJson["humidityPercentage"] = (hum);
  Serial.println("test");
  return humJson;
}



void sendMqttMessage(String topic, String value){
  mqttClient.setUsernamePassword(user, password);
  if(!mqttClient.connect(broker, port)){
    Serial.println("MQTT connection failed");
    return;
  }
  if(WiFi.status() == WL_CONNECTED){
  Serial.println("MQTT connection success");
  mqttClient.beginMessage(topic);
  mqttClient.print(value);
  mqttClient.endMessage();


  }

}