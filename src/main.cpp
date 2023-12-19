#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include <secrets.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include <HttpClient.h>
#include "functions.h"
#include "frontEnd.cpp"



FlashStorage(flashmemo, WifiData);
int latestMouister = 0;
String ssid = "";
String pass = "";
WiFiServer server(80);
unsigned long previousMillis = 0;
const long interval = 1000 * 60 * 60 * 3;
int dry;
int wet;

bool wifiConnected = false;
bool updateUi = false;



WiFiSSLClient wifiSSLClient;
MqttClient mqttClient(wifiSSLClient);


// returnes alle the values from the sensors in a json object
DynamicJsonDocument updateValues(){
  DynamicJsonDocument valueJson(1024);

  valueJson["soilMoisture"] = readMoistureSensor(false);
  valueJson["temperature"] = readTemperature();
  valueJson["humidity"] = readHumidity();
  valueJson["light"] = readLight();
  return valueJson;
};



// check if you allready have some saved wifi id and password in flash and go to default if not
void checkFlash(){
  if (strcmp(flashmemo.read().ssid, "") == 0)
  {
    Serial.println("No ssid");
    ssid = SECRET_SSID;
    pass = SECRET_PASS; 
  }else{
    Serial.println("ssid");
    ssid = flashmemo.read().ssid;
    pass = flashmemo.read().password;

  }
  
};


// runs once when the arduino startup
void setup() {


  Serial.begin(9600);
  while (!Serial);



  checkFlash();
  carrier.noCase();
  carrier.begin();
  onStartUp(wifiConnected);
  // updateUserInterface(wifiConnected);
  // showQrCode();

 
  // carrier.display.drawBitmap(40, 40, smartplantQR, 140, 140, 0xffff);
    
  // calibratedMoistureSensor();
  // connectWifi(false);
  // connectWifi(false);




  
}
// runs continuously in a loop after setup() have run once
void loop() {


  if(millis() - previousMillis >= interval){
    previousMillis = millis();
    connectWifi(false);
    updateUi = true;
  }
  if (updateUi)
  {
      updateUserInterface(wifiConnected);
      updateUi = false;

  }
  carrier.Buttons.update();
  if (carrier.Buttons.onTouchDown(TOUCH2))
  {
    showOptions();
  }
  if (carrier.Buttons.onTouchDown(TOUCH3))
  {
    showData();
  }
  if (carrier.Buttons.onTouchDown(TOUCH1))
  {
    showQrCode();
  }

  // connectWifi(false);

}
// set up an acccess point for the user to connect to
void connectAp() {
  arduinoApConnectionText();
  server.begin();
  WiFi.beginAP("ArduinoWiFi", "12345678");
  bool apOpen = true;
  bool apConnectedFirst = true;
  while (apOpen) {
    WiFiClient client = server.available();
    if (client) {
      if(apConnectedFirst){
        arduinoApInstruction();

        apConnectedFirst = false;
      }
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
              client.println("Wifi navn: <input type='text' name='ssid'><br>");
              client.println("Wifi kode: <input type='password' name='pass'><br>");
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
                    ssid = newSsid;
                    pass = newPassword;
                    WifiData data;
                    strncpy(data.ssid, newSsid.c_str(), sizeof(data.ssid));
                    // data.ssid[sizeof(data.ssid) - 1] = '\0'; 
                    strncpy(data.password, newPassword.c_str(), sizeof(data.password));
                    // data.password[sizeof(data.password) - 1] = '\0'; 
                    flashmemo.write(data);
                    Serial.println(ssid);
                    Serial.println(pass);
                    client.stop();
                    apOpen = false;
                    carrier.display.fillScreen(0);
                    connectWifi(false);
 
                  }

            }

          }

        }


      }

      // close the connection:
      

    }
  }

}

// connect to the wifi network, sending the data to mongo and disconate again it is a bool to check if the plant has been watered
void connectWifi(bool watthering) {
    int counter = 20;
    carrier.display.fillScreen(0);
    carrier.display.setTextSize(2);
      carrier.display.setCursor(80, 117);
      carrier.display.print("Loading");
      WiFi.begin(ssid.c_str(), pass.c_str());
      Serial.print("Connecting to ");
    while (WiFi.status() != WL_CONNECTED && counter-- > 0) {
      delay(4000);
      WiFi.begin(ssid.c_str(), pass.c_str());
      Serial.print(counter);
    }
  
  if (WiFi.status() == WL_CONNECTED) {
    printNertworkData();
    DynamicJsonDocument doc(2048);

    doc["temperature"] = readTemperature(); 
    doc["humidity"] = readHumidity();
    doc["light"] = readLight();
    doc["soilMoisture"] = readMoistureSensor(watthering);
    doc["deviceId"] = device_id;

    String jsonString;
    serializeJson(doc, jsonString);
    if (wifiConnected)
    {
      sendMqttMessage("device", jsonString);
      // sendToApi(jsonString);
    }else{
      wifiConnected = true;
      updateUi = true;
    }


    


  } else {
    wifiConnected = false;
    updateUi = true;
  }
  delay(2000);
  WiFi.disconnect();

}
// send data to an api it takes a json string as a parameter for the data it sends
void sendToApi(String data) {
      WiFiSSLClient wifiSSLClient;
      HttpClient httpClient(wifiSSLClient, "smartplant.vercel.app", 443);
      httpClient.beginRequest();
      httpClient.post("/api/smartplant");
      httpClient.sendHeader("Content-Type", "application/json");
      httpClient.sendHeader("Content-Length", String(data.length()));
      Serial.println("Sending data to API");
      httpClient.beginBody();
      httpClient.print(data);
      httpClient.endRequest();

      int statusCode = httpClient.responseStatusCode();
      String response = httpClient.responseBody();

      Serial.print("HTTP Status Code: ");
      Serial.println(statusCode);
      Serial.print("Response: ");
      Serial.println(response);
    }


// print Network data to the serial monitor
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



//calibrate the moisture sensor
 void calibratedMoistureSensor(){
  carrier.display.fillScreen(0);
  carrier.display.setTextSize(2);

  bool dryCalab = false;
  bool wetCalab = false;
  carrier.display.setCursor(25, 100);
  carrier.display.print("Placer din sensor");
  carrier.display.setCursor(40, 120);
  carrier.display.print("paa en toer");
  carrier.display.setCursor(20, 140);
  carrier.display.print("overflade og");
  carrier.display.setCursor(20, 160);
  carrier.display.print("tryk paa knappen");
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
  carrier.display.print("Placer din sensor");
  carrier.display.setCursor(20, 120);
  carrier.display.print("i et glas vand");
  carrier.display.setCursor(20, 140);
  carrier.display.print("og tryk paa");
  carrier.display.setCursor(20, 160);
  carrier.display.print("knappen");
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
  
  updateUi = true;


 }


// read the moisture sensor and return a json object it takes a bool to check if the plant has been watered
 DynamicJsonDocument readMoistureSensor(bool watthering){
  int moisture = analogRead(A6);
  int moisturePercent = map(moisture, wet - 30, dry, 100, 0);
  if(moisturePercent < 0){
    moisturePercent = 0;
  }
if (watthering)
{
  if (moisturePercent <= latestMouister + 20)
  {
    moisturePercent = latestMouister + 20;
  }
  
}else
{
  latestMouister = moisturePercent;

}
  DynamicJsonDocument moistJson(1024);
  moistJson["value"] = (moisturePercent);
  return moistJson;
 }

// read the temperature sensor and return a json object
 DynamicJsonDocument readTemperature(){
  int temp = carrier.Env.readTemperature();
  DynamicJsonDocument tempJson(1024);
  tempJson["value"] = (temp -8);
  return tempJson;
 }
// read the light sensor and return a json object
DynamicJsonDocument readLight(){
  int A, B, C, D;
  int counter = 0;  
  while (!carrier.Light.colorAvailable() || counter++ < 20)
  {
    delay(100);
  }

  carrier.Light.readColor(A, B, C, D);

  if (D > 200)
  {
    D = 200;
  }

  int lightProcent = map(D, 0, 200, 0, 100);
    DynamicJsonDocument lightJson(1024);
    lightJson["value"] = (lightProcent);
    return lightJson;
  
}

// read the humidity sensor and return a json object
DynamicJsonDocument readHumidity(){
  int hum = carrier.Env.readHumidity();
  DynamicJsonDocument humJson(1024);
  humJson["value"] = (hum);
  return humJson;
}


// send data to the mqtt server it takes a topic that i send to and a value that is the data
void sendMqttMessage(String topic, String value){
const char user[] = MQTTUSER;
const char password[] = MQTTPASS;
const char broker[] = MQTT_SERVER;
int        port     = MQTT_PORT;
  mqttClient.setUsernamePassword(user, password);
  if(!mqttClient.connect(broker, port)){
    Serial.println("MQTT connection failed");
    return;
  }
  if(WiFi.status() == WL_CONNECTED){
  Serial.println("MQTT connection success");
  Serial.println(value);
  mqttClient.beginMessage(topic);
  mqttClient.print(value);
  mqttClient.endMessage();


  }

}


// show a qr code on the screen
void showQrCode()
{
  carrier.leds.clear();
  carrier.leds.show();
  carrier.display.fillScreen(0);
  carrier.display.drawBitmap(50, 50, smartplantQR, 140, 140, 0xffff);
  carrier.display.setTextSize(1, 2);
  carrier.display.setCursor(80, 30);
  carrier.display.print("Scan QR koden");
  carrier.display.setCursor(110, 195);
  carrier.display.print("Vider");
  carrier.display.drawBitmap(93, 190, arrow_down, 61, 61, 0xffff);
  if (ledOn)
  {
    carrier.leds.setBrightness(25);
    carrier.leds.setPixelColor(2, 0, 0, 255);
    carrier.leds.show();
  }
  
  bool next = true;

  while (next)
  {
    carrier.Buttons.update();
    if (carrier.Buttons.onTouchDown(TOUCH2))
    {
      next = false;
      updateUi = true;
 
    }
  }
  
}