#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "btn.h"
#include "wifi.h"
#include "indexHtml.h"

ESP8266WebServer server(80);

#define LED1 0   // GPIO0 (D3)
#define LED2 12  // GPIO12 (D6)
#define LED3 13  // GPIO13 (D7)

int leds[] = {LED1, LED2, LED3};
int ledCount = sizeof(leds) / sizeof(leds[0]);
int currentLED = -1;

void toggleNextLED() {
    if (currentLED != -1) {
        digitalWrite(leds[currentLED], LOW);
    }
    currentLED = (currentLED + 1) % ledCount;
    digitalWrite(leds[currentLED], HIGH);
    Serial.print("LED ON: ");
    Serial.println(currentLED);
}

void handleRoot() {
    server.send_P(200, "text/html", MAIN_page);
}

void handleToggle() {
    if (server.hasArg("led") && server.arg("led") == "next") {
        toggleNextLED();
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    for (int i = 0; i < ledCount; i++) {
        pinMode(leds[i], OUTPUT);
        digitalWrite(leds[i], LOW);
    }

    connectToWiFi();

    server.on("/", handleRoot);
    server.on("/toggle", handleToggle);
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
    if (  checkButton()) {
        toggleNextLED();
    }
}
