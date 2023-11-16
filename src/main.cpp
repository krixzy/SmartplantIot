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
              client.println("<form method='post' action='/'>");
              client.println("SSID: <input type='text' name='ssid'><br>");
              client.println("Password: <input type='password' name='pass'><br>");
              client.println("<input type='submit' value='Connect'>");
              client.println("</form>");
              client.println("</html>");
              break;
            } else {
              currentLine = "";
            }
          } else if (c == '\r') {
            // if the current line is the Content-Length line, get the value
            if (currentLine.startsWith("Content-Length: ")) {
              int contentLength = currentLine.substring(16).toInt();
              // read the next (blank) line
              while (client.read() != '\n');
              // now read the POST data
              for (int i = 0; i < contentLength; i++) {
                postData += (char)client.read();
              }
            }
          }
        }
      }
      // close the connection:
      client.stop();
      delay(1000);

      // print the POST data to the console
      Serial.println(postData);

    }
  }
}




void connectWifi() {
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    delay(4000);
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