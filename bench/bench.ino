// The Sensor Piece: Nano, HC-SR04 or US-025A, OLED, and three LEDs.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int TRIG = 9;
const int ECHO = 10;
const int JAXSON_LED = 3;
const int DIEGO_LED = 4;
const int BRENNAN_LED = 5;
const int BUZZER = 6;

const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const float GAP_CM = 10.0;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String state = "nobody";
String candidate = "nobody";
unsigned long candidateSince = 0;
float shownCm = 400.0;
unsigned long lastPrint = 0;

String classify(float cm) {
  if (cm <= 30.0) return "Jaxson";
  if (cm <= 120.0) return "Diego";
  if (cm <= 200.0) return "Brennan";
  return "nobody";
}

unsigned long holdFor(const String& name) {
  if (name == "Jaxson") return 1000;
  if (name == "Diego") return 2000;
  if (name == "Brennan") return 2000;
  return 3000;
}

void setResponse(const String& name) {
  digitalWrite(JAXSON_LED, name == "Jaxson" ? HIGH : LOW);
  digitalWrite(DIEGO_LED, name == "Diego" ? HIGH : LOW);
  digitalWrite(BRENNAN_LED, name == "Brennan" ? HIGH : LOW);

  drawDisplay(name);

  if (name == "Jaxson") {
    tone(BUZZER, 660, 100);
  } else if (name == "Diego") {
    tone(BUZZER, 440, 100);
  } else if (name == "Brennan") {
    tone(BUZZER, 330, 100);
  } else {
    noTone(BUZZER);
  }
}

void drawDisplay(const String& name) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(name == "nobody" ? 2 : 3);
  display.setCursor(0, 0);
  display.print(name == "nobody" ? "NOBODY" : name);
  display.setTextSize(2);
  display.setCursor(0, 38);
  display.print("cm ");
  display.print(shownCm, 1);
  display.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(JAXSON_LED, OUTPUT);
  pinMode(DIEGO_LED, OUTPUT);
  pinMode(BRENNAN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  setResponse("nobody");
}

void loop() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  unsigned long us = pulseIn(ECHO, HIGH, 30000);
  float reading = us ? us / 58.0 : 400.0;
  if (reading < 2.0) reading = 400.0;
  if (abs(reading - shownCm) > GAP_CM) {
    shownCm = reading;
  }

  String next = classify(shownCm);
  unsigned long now = millis();
  if (next != candidate) {
    candidate = next;
    candidateSince = now;
  }
  if (next != state && now - candidateSince >= holdFor(next)) {
    state = next;
    setResponse(state);
  }

  if (now - lastPrint >= 50) {
    Serial.print("cm ");
    Serial.println(shownCm, 1);
    drawDisplay(state);
    lastPrint = now;
  }

  delay(50);
}
