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

// Змінна для відстеження стану алгоритму
bool algorithmMode = true; // true - один режим, false - інший режим

void toggleNextLED() {
    if (currentLED != -1) {
        digitalWrite(leds[currentLED], LOW);  // Вимикаємо попередній
    }
    currentLED = (currentLED + 1) % ledCount;
    digitalWrite(leds[currentLED], HIGH);     // Вмикаємо новий
    Serial.print("LED ON: ");
    Serial.println(currentLED);
}

void toggleAlgorithmMode() {
    algorithmMode = !algorithmMode;  // Перемикаємо режим алгоритму
    if (algorithmMode) {
        Serial.println("Algorithm Mode: Forward");
    } else {
        Serial.println("Algorithm Mode: Reverse");
    }
}

void handleRoot() {
    server.send_P(200, "text/html", MAIN_page);  // Надсилаємо HTML
}

void handleToggle() {
    if (server.hasArg("led") && server.arg("led") == "next") {
        toggleNextLED();  // Реакція на веб-кнопку
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void handleToggleDevice1() {
    toggleAlgorithmMode();  // Перемикаємо режим алгоритму
    server.send(200, "text/plain", "Algorithm Mode Toggled");
}

void setup() {
    Serial.begin(115200);  // UART зв'язок

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    for (int i = 0; i < ledCount; i++) {
        pinMode(leds[i], OUTPUT);
        digitalWrite(leds[i], LOW);
    }

    connectToWiFi();

    server.on("/", handleRoot);
    server.on("/toggle", handleToggle);
    server.on("/toggleDevice1", handleToggleDevice1);  // Обробка маршруту для Toggle Device 1
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();

    if (checkButton()) {
        toggleNextLED();  // Реакція на кнопку
    }

    if (Serial.available()) {
        char command = Serial.read();
        if (command == 'J') {
            toggleNextLED();  // Реакція на UART
        }
    }

    // Залежно від алгоритму виконувати різний режим
    if (algorithmMode) {
        // Режим "forward"
        if (millis() % 500 < 250) {  // Це просто приклад, можна змінити на реальний алгоритм
            toggleNextLED();
        }
    } else {
        // Режим "reverse" або інший алгоритм
        if (millis() % 1000 < 500) {  // Це просто приклад
            toggleNextLED();
        }
    }
}
