// -*- Mode: C++ -*-
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Secrets.hpp"


ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PWD);
  WiFi.config(IP, WiFi.gatewayIP(), WiFi.subnetMask());
  // Wait until WiFi is connected
  Serial.println("");
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Setup WebServer Handlers
  server.on("/", [](){
    String b = String(analogRead(17));
    server.send(200, "text/plain", b);
  });

  server.begin();
}

void loop() {
    server.handleClient();
}
