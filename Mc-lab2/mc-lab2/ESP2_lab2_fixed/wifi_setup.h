#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <ESP8266WiFi.h>

const char* ssid = "Готель Шкло";     
const char* password = "11111111";

void setupWiFi() {
    Serial.print("Підключення до WiFi");
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi підключено!");
    Serial.print("IP адреса: ");
    Serial.println(WiFi.localIP());
}

#endif // WIFI_SETUP_H
