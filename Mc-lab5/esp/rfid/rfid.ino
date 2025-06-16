#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN     5  // D1
#define SS_PIN      4  // D2
#define SERVO_PIN   2  // D4
#define BUZZER_PIN  0  // D3

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myServo;

const char* ssid = "Готель Шкло";
const char* password = "12345678";
const char* serverUrl = "http://172.20.10.14:3000/access-check";

bool isUnlocked = false;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Підключення до Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Підключено до Wi-Fi");

  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(BUZZER_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(0);

  Serial.println("Скануй картку...");
}

String getUID() {
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  return uid;
}

void positiveBeep() {
  tone(BUZZER_PIN, 1000, 150); delay(200);
  tone(BUZZER_PIN, 1500, 150); delay(200);
}

void negativeBeep() {
  tone(BUZZER_PIN, 400, 250); delay(300);
  tone(BUZZER_PIN, 400, 250); delay(300);
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  String uid = getUID();
  Serial.print("Зчитано UID: ");
  Serial.println(uid);

  // Надсилання UID на сервер
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"uid\": \"" + uid + "\"}";

    int code = http.POST(payload);
    String response = http.getString();

    Serial.print("HTTP code: "); Serial.println(code);
    Serial.print("Відповідь сервера: "); Serial.println(response);

    if (response.indexOf("true") >= 0) {
      positiveBeep();
      if (!isUnlocked) {
        myServo.attach(SERVO_PIN);
        myServo.write(90);
        delay(500);
        myServo.detach();
        isUnlocked = true;
      } else {
        myServo.attach(SERVO_PIN);
        myServo.write(0);
        delay(500);
        myServo.detach();
        isUnlocked = false;
      }
    } else {
      negativeBeep();
    }

    http.end();
  } else {
    Serial.println("❌ Wi-Fi з'єднання втрачено");
  }

  mfrc522.PICC_HaltA();
  delay(1000);
}
