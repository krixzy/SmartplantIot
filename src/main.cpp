#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include <secrets.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>


MKRIoTCarrier carrier;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
void printNertworkData();
void connectWifi();
void connectAp();
WiFiServer server(80);


void setup() {
  Serial.begin(9600);
  // connectWifi();
  server.begin();
  connectAp();

}

void loop() {

}

void connectAp() {
  WiFi.beginAP("ArduinoWiFi", "12345678");
  while (true) {
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
                    Serial.println(ssid);
                    Serial.println(pass);
                    connectWifi();
                  }


            }

          }
        }
      }
      // close the connection:
      client.stop();
      delay(1000);

      

    }
  }
}




void connectWifi() {
    WiFi.begin(ssid, pass);
    Serial.print("Connecting to ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(4000);
    WiFi.begin(ssid, pass);
    Serial.print(".");
  }
  printNertworkData();
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