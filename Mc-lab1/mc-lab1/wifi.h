#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>

#define WIFI_SSID "Готель Шкло"
#define WIFI_PASS "12345678"

void connectToWiFi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

#endif