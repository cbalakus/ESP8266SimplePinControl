#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <string.h>
#include <FS.h>

bool IsServer = false;

const char* ssid = "Can";
const char* password = "1048576?CaN";

ESP8266WebServer server(80);

//GPIO pins
const int inputs[8] = { 16, 14, 12, 13, 15, 3, 0, 4 };

void setup(void) {
  for (int i = 0; i < 8; i++) {
    pinMode(inputs[i], OUTPUT);
    digitalWrite(inputs[i], LOW);
  }

  Serial.begin(115200);
  if (IsServer) {
    Serial.println("Wifi Server activated.");
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  }
  else {
    Serial.println("Wifi Client activated.");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    IPAddress ip(192,168,1,10);   
    IPAddress gateway(192,168,1,1);   
    IPAddress subnet(255,255,255,0);   
    WiFi.config(ip, gateway, subnet);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  SPIFFS.begin();
  Serial.println("Spiffs started");
  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/style.css", SPIFFS, "/style.css");
  server.serveStatic("/bulbOn.png", SPIFFS, "/bulbOn.png");
  server.serveStatic("/bulbOff.png", SPIFFS, "/bulbOff.png");
  server.on("/d", []() {
    const String inputIndex = server.arg("idx");
    const String inputVal = server.arg("val");
    digitalWrite(inputs[inputIndex.toInt()], inputVal.toInt());
    server.send(200, "text/plain", "OK");
    Serial.print("DO");
    Serial.print(inputIndex);
    Serial.print(" : ");
    Serial.println(inputVal);
  });
  server.on("/s", []() {
    String retVal = "";
    for (int i = 0; i < 8; i++)
      retVal += String(digitalRead(inputs[i]));
    server.send(200, "text/plain", retVal);
  });
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
