// The Sensor Piece: Nano, HC-SR04 or US-025A, OLED, and three LEDs.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int TRIG = 9;
const int ECHO = 10;
const int RED_LED = 3;
const int GREEN_LED = 5;
const int BLUE_LED = 11;
const int JAXSON_BUZZER = 6;
const int DIEGO_BUZZER = 7;
const int BOI_BUZZER = 13;

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

struct Note {
  uint16_t frequency;
  uint16_t duration;
};

const Note JAXSON_TUNE[] = {
  {659, 220}, {659, 220}, {659, 440}, {659, 220}, {659, 220}, {659, 440},
  {659, 220}, {784, 220}, {523, 220}, {587, 220}, {659, 600}
};

const Note DIEGO_TUNE[] = {
  {392, 260}, {523, 260}, {523, 260}, {587, 260}, {523, 260}, {494, 260}, {440, 520},
  {440, 260}, {587, 260}, {587, 260}, {659, 260}, {587, 260}, {523, 260}, {494, 520}
};

const Note BOI_TUNE[] = {
  {440, 180}, {392, 180}, {370, 180}, {330, 180}, {294, 180}, {330, 180},
  {370, 180}, {294, 180}, {330, 180}, {370, 180}, {392, 180}, {440, 180},
  {440, 180}, {392, 180}, {370, 180}, {330, 180}, {294, 520}
};

const Note* activeTune = nullptr;
uint8_t activeTuneLength = 0;
uint8_t nextNoteIndex = 0;
int activeBuzzer = -1;
unsigned long nextNoteAt = 0;

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
  if (cm <= 60.0) return "#BOI";
  if (cm <= 200.0) return "Diego";
  return "nobody";
}

void stopTunes() {
  noTone(JAXSON_BUZZER);
  noTone(DIEGO_BUZZER);
  noTone(BOI_BUZZER);
  activeTune = nullptr;
  activeTuneLength = 0;
  activeBuzzer = -1;
}

void updateTune(unsigned long now) {
  if (activeTune == nullptr || now < nextNoteAt) return;

  if (nextNoteIndex >= activeTuneLength) {
    noTone(activeBuzzer);
    activeTune = nullptr;
    activeBuzzer = -1;
    return;
  }

  const Note note = activeTune[nextNoteIndex++];
  if (note.frequency == 0) {
    noTone(activeBuzzer);
  } else {
    tone(activeBuzzer, note.frequency, note.duration);
  }
  nextNoteAt = now + note.duration + 35;
}

void startTune(const String& name, unsigned long now) {
  stopTunes();
  nextNoteIndex = 0;
  nextNoteAt = now;

  if (name == "Jaxson") {
    activeTune = JAXSON_TUNE;
    activeTuneLength = sizeof(JAXSON_TUNE) / sizeof(JAXSON_TUNE[0]);
    activeBuzzer = JAXSON_BUZZER;
  } else if (name == "Diego") {
    activeTune = DIEGO_TUNE;
    activeTuneLength = sizeof(DIEGO_TUNE) / sizeof(DIEGO_TUNE[0]);
    activeBuzzer = DIEGO_BUZZER;
  } else if (name == "#BOI") {
    activeTune = BOI_TUNE;
    activeTuneLength = sizeof(BOI_TUNE) / sizeof(BOI_TUNE[0]);
    activeBuzzer = BOI_BUZZER;
  }
  updateTune(now);
}

void setResponse(const String& name) {
  digitalWrite(RED_LED, name == "Jaxson" ? HIGH : LOW);
  digitalWrite(GREEN_LED, name == "Diego" ? HIGH : LOW);
  digitalWrite(BLUE_LED, name == "#BOI" ? HIGH : LOW);
  startTune(name, millis());
  drawDisplay(name);
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
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(JAXSON_BUZZER, OUTPUT);
  pinMode(DIEGO_BUZZER, OUTPUT);
  pinMode(BOI_BUZZER, OUTPUT);

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
  updateTune(millis());

  unsigned long now = millis();
  if (now - lastPrint >= 50) {
    Serial.print("cm ");
    Serial.println(shownCm, 1);
    drawDisplay(state);
    lastPrint = now;
  }
}
