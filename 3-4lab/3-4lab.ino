#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Піни
#define BUTTON_PIN 0
#define BUZZER_PIN 2

// WiFi та MQTT
const char* ssid = "Queendom";
const char* password = "godsmenu8";
const char* mqtt_server = "192.168.0.128";

WiFiClient espClient;
PubSubClient client(espClient);

// Слот символи
String symbols[] = {"7", "$"};
const int numSymbols = sizeof(symbols) / sizeof(symbols[0]);

bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(115200);

  // OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while(true);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.println("SLOT READY");
  display.display();

  // WiFi
  setup_wifi();

  // MQTT
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int reading = digitalRead(BUTTON_PIN);
  if (reading == LOW && (millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();
    playSlot();
  }
}

void playSlot() {
  display.clearDisplay();
  playSpinSound();

  int r1 = random(numSymbols);
  int r2 = random(numSymbols);
  int r3 = random(numSymbols);

  display.setTextSize(2);
  display.setCursor(10, 20);
  display.print(symbols[r1]);
  display.setCursor(50, 20);
  display.print(symbols[r2]);
  display.setCursor(90, 20);
  display.print(symbols[r3]);
  display.display();

  delay(1000);
  display.setTextSize(1);
  display.setCursor(25, 50);

  String result;
  if (r1 == r2 && r2 == r3) {
    display.print("JACKPOT!");
    playWinSound();
    result = "JACKPOT: " + symbols[r1] + symbols[r2] + symbols[r3];
  } else {
    display.print("Try again");
    playLoseSound();
    result = "TRY AGAIN: " + symbols[r1] + symbols[r2] + symbols[r3];
  }
  display.display();

  // Надсилання в MQTT
  client.publish("slot/result", result.c_str());
}

// Звук
void playSpinSound() {
  tone(BUZZER_PIN, 1000, 100);
  delay(120);
}

void playWinSound() {
  tone(BUZZER_PIN, 1500, 200); delay(200);
  tone(BUZZER_PIN, 2000, 200); delay(200);
  noTone(BUZZER_PIN);
}

void playLoseSound() {
  tone(BUZZER_PIN, 300, 400); delay(400);
  noTone(BUZZER_PIN);
}

// WiFi підключення
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

// Підключення до MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("SlotMachineClient")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}
