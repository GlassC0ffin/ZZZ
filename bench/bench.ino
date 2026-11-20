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
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String state = "nobody";
float shownCm = 400.0;
unsigned long lastPrint = 0;
const int SAMPLE_COUNT = 3;
float samples[SAMPLE_COUNT];
int sampleIndex = 0;
bool samplesFilled = false;

float medianReading(float reading) {
  samples[sampleIndex++] = reading;
  if (sampleIndex >= SAMPLE_COUNT) {
    sampleIndex = 0;
    samplesFilled = true;
  }

  int count = samplesFilled ? SAMPLE_COUNT : sampleIndex;
  if (count == 0) return reading;

  float sorted[SAMPLE_COUNT];
  for (int i = 0; i < count; i++) sorted[i] = samples[i];
  for (int i = 1; i < count; i++) {
    float value = sorted[i];
    int j = i - 1;
    while (j >= 0 && sorted[j] > value) {
      sorted[j + 1] = sorted[j];
      j--;
    }
    sorted[j + 1] = value;
  }
  return sorted[count / 2];
}

String classify(float cm) {
  if (cm <= 30.0) return "Jaxson";
  if (cm <= 120.0) return "Diego";
  if (cm <= 200.0) return "Bremer";
  return "nobody";
}

void setResponse(const String& name) {
  digitalWrite(JAXSON_LED, name == "Jaxson" ? HIGH : LOW);
  digitalWrite(DIEGO_LED, name == "Diego" ? HIGH : LOW);
  digitalWrite(BRENNAN_LED, name == "Bremer" ? HIGH : LOW);

  drawDisplay(name);

  noTone(BUZZER);
  if (name == "Jaxson") {
    tone(BUZZER, 880, 180);
  } else if (name == "Diego") {
    tone(BUZZER, 660, 180);
  } else if (name == "Bremer") {
    tone(BUZZER, 440, 180);
  } else {
    tone(BUZZER, 220, 180);
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
  shownCm = medianReading(reading);

  String next = classify(shownCm);
  if (next != state) {
    state = next;
    setResponse(state);
  }

  unsigned long now = millis();
  if (now - lastPrint >= 50) {
    Serial.print("cm ");
    Serial.println(shownCm, 1);
    drawDisplay(state);
    lastPrint = now;
  }

}
