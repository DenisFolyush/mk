#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "wifi_setup.h"
#include "web_server.h"

#define LED1 14  
#define LED2 0   
#define LED3 2   
#define BUTTON 4 

bool reverseOrder = false;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 50;

void setup() {
    Serial.begin(115200);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);

    setupWiFi();     
    setupWebServer();
}

void loop() {
    server.handleClient();

    bool buttonState = digitalRead(BUTTON);
    if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime > debounceDelay)) {
        reverseOrder = !reverseOrder;
        Serial.print("Напрямок змінився: ");
        Serial.println(reverseOrder ? "зворотній" : "прямий");
        lastDebounceTime = millis();
    }
    lastButtonState = buttonState;

    if (!reverseOrder) {
        blinkLeds(LED1, LED2, LED3);
    } else {
        blinkLeds(LED3, LED2, LED1);
    }
}

void blinkLeds(int led1, int led2, int led3) {
    digitalWrite(led1, HIGH);
    delay(300);
    digitalWrite(led1, LOW);

    digitalWrite(led2, HIGH);
    delay(300);
    digitalWrite(led2, LOW);

    digitalWrite(led3, HIGH);
    delay(300);
    digitalWrite(led3, LOW);
}
