// -*- Mode: C++ -*-
#include <stdio.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include "Pins.hpp"
#include "Secrets.hpp"

ESP8266WebServer server(80);

static void ticker_power() {
  static bool toggle = false;
  toggle = !toggle;
  digitalWrite(POWER_LED_PIN, toggle ? HIGH: LOW);
}

static Ticker ticker;

void setup() {
  Serial.begin(115200);
  pinMode(SWITCH_PIN, OUTPUT);
  pinMode(POWER_LED_PIN, OUTPUT);
  ticker.attach_ms(500, ticker_power); // Blink power led until connected to Wi-Fi.

  WiFi.begin(WIFI_SSID, WIFI_PWD);
  WiFi.config(IP, WiFi.gatewayIP(), WiFi.subnetMask());
  // Wait until WiFi is connected
  while(WiFi.status() != WL_CONNECTED){
    delay(100);
  }
  ticker.detach();
  digitalWrite(POWER_LED_PIN, HIGH);

  // Setup WebServer Handlers
  server.on("/", [](){
    if (server.method() == HTTP_POST) {
      String val = server.arg("value");
      if (val == "1") {
        digitalWrite(SWITCH_PIN, HIGH);
      } else {
        digitalWrite(SWITCH_PIN, LOW);
      }
      server.send(200, "text/plain", val);
    } else {
      String val = digitalRead(SWITCH_PIN) == HIGH ? "1" : "0";
      server.send(200, "text/plain", val);
    }
  });

  server.begin();
}

void loop() {
  static bool wait_high = false;
  int manual = digitalRead(MANUAL_PIN);
  if (wait_high) {
    if (manual == HIGH) {
      wait_high = false;
      delay(30); // prevent chattering
    }
  } else {
    if (manual == LOW) {
      digitalWrite(SWITCH_PIN, digitalRead(SWITCH_PIN) == HIGH ? LOW : HIGH);
      delay(30); // prevent chattering
      wait_high = true;
    }
  }
  server.handleClient();
}
