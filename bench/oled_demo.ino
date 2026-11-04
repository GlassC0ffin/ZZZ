/*
  ARDUINO NANO + ULTRASONIC SENSOR (HC-SR04) + 4-PIN I2C OLED (SSD1306)
  ---------------------------------------------------------------------
  WIRING

  Ultrasonic sensor (HC-SR04):
    VCC  -> 5V
    GND  -> GND
    TRIG -> D9   (change TRIG_PIN below if you use another pin)
    ECHO -> D10  (change ECHO_PIN below if you use another pin)

  OLED (4 pins, I2C):
    VCC -> 5V (or 3.3V, most boards accept both)
    GND -> GND
    SDA -> A4   (fixed on Nano, cannot be changed)
    SCL -> A5   (fixed on Nano, cannot be changed)

  LIBRARIES TO INSTALL (Sketch -> Include Library -> Manage Libraries):
    1. "Adafruit SSD1306"
    2. "Adafruit GFX Library"
*/

#include <Wire.h>              // I2C communication (built in)
#include <Adafruit_GFX.h>      // Graphics library (text, shapes)
#include <Adafruit_SSD1306.h>  // OLED driver

// ===================== SETTINGS YOU CAN CHANGE =====================

// --- Ultrasonic sensor pins ---
const int TRIG_PIN = 9;        // Pin connected to TRIG
const int ECHO_PIN = 10;       // Pin connected to ECHO

// --- OLED settings ---
const int SCREEN_WIDTH  = 128; // OLED width in pixels (most are 128)
const int SCREEN_HEIGHT = 64;  // 64 for 128x64 screens, 32 for 128x32 screens
const int OLED_ADDRESS  = 0x3C; // I2C address. Most are 0x3C. If screen stays blank, try 0x3D

// --- Measurement settings ---
const int  MAX_DISTANCE_CM = 200;   // Farther than this shows "No object"
                                    // (HC-SR04 works up to about 400 cm)
const int  UPDATE_DELAY_MS = 200;   // Time between measurements in milliseconds
const int  WARNING_CM      = 20;    // Show "CLOSE!" when closer than this (cm)

// ===================================================================

// Create the OLED object (-1 means the OLED has no reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- Function: measure distance in cm ----------
// Returns the distance in centimeters, or -1 if nothing was detected
float readDistanceCm() {
  // 1) Make sure TRIG starts LOW
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // 2) Send a 10 microsecond pulse to start a measurement
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // 3) Measure how long ECHO stays HIGH (in microseconds)
  //    The 30000 is a timeout (30 ms) so the code never gets stuck
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // 4) If timeout happened, nothing was detected
  if (duration == 0) {
    return -1;
  }

  // 5) Convert time to distance.
  //    Sound travels about 0.0343 cm per microsecond.
  //    Divide by 2 because the sound goes to the object AND back.
  float distance = duration * 0.0343 / 2;

  return distance;
}

// ---------- Runs once when the Arduino starts ----------
void setup() {
  Serial.begin(9600);          // Serial Monitor speed (set it to 9600 baud)

  pinMode(TRIG_PIN, OUTPUT);   // TRIG sends the pulse -> OUTPUT
  pinMode(ECHO_PIN, INPUT);    // ECHO receives the pulse -> INPUT

  // Start the OLED. If it fails, the code stops here.
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED not found! Check wiring or change OLED_ADDRESS.");
    while (true);              // Stop forever
  }

  // Show a short startup message
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);               // 1 = small, 2 = medium, 3 = large
  display.setCursor(0, 0);              // Position (x, y) in pixels. (0,0) = top-left
  display.println("Distance meter");
  display.println("Starting...");
  display.display();                    // IMPORTANT: nothing shows until you call display()
  delay(1000);
}

// ---------- Runs over and over ----------
void loop() {
  float distance = readDistanceCm();   // Take a measurement

  display.clearDisplay();              // Erase the previous screen

  // ----- Title (top line, small text) -----
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Distance:");

  // ----- Distance value (big text) -----
  display.setTextSize(3);              // Make the value bigger/smaller here
  display.setCursor(0, 18);            // Move this to change where the value appears

  if (distance < 0 || distance > MAX_DISTANCE_CM) {
    // Nothing detected or too far
    display.setTextSize(2);
    display.print("No object");
    Serial.println("Out of range");
  } else {
    // Show the distance with 1 decimal. Use 0 for no decimals, 2 for two, etc.
    display.print(distance, 1);
    display.setTextSize(1);
    display.print(" cm");

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // ----- Bar graph at the bottom: closer object = longer bar -----
    int barLength = map((int)distance, 0, MAX_DISTANCE_CM, SCREEN_WIDTH, 0);
    barLength = constrain(barLength, 0, SCREEN_WIDTH);
    display.fillRect(0, SCREEN_HEIGHT - 8, barLength, 6, SSD1306_WHITE);  // x, y, width, height

    // ----- Warning text if the object is very close -----
    if (distance < WARNING_CM) {
      display.setTextSize(1);
      display.setCursor(80, 0);        // Top-right area
      display.print("CLOSE!");
    }
  }

  display.display();                   // Send everything to the screen
  delay(UPDATE_DELAY_MS);              // Wait before the next measurement
}