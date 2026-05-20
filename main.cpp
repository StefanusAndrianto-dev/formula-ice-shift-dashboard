#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define RPM_PIN 34

int ledPins[10] = {13, 12, 14, 27, 26, 25, 33, 32, 4, 16};

void setup() {

  Serial.begin(115200);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  for (int i = 0; i < 10; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.display();
}

void loop() {

  int potValue = analogRead(RPM_PIN);

  int rpm = map(potValue, 0, 4095, 1000, 9000);

  int ledLevel = map(rpm, 1000, 9000, 0, 10);

  ledLevel = constrain(ledLevel, 0, 10);

  for (int i = 0; i < 10; i++) {

    if (i < ledLevel) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }

  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("SHIFT DASHBOARD");

  display.setCursor(0, 15);
  display.print("RPM : ");
  display.println(rpm);

  display.setCursor(0, 30);

  if (rpm < 3000) {
    display.println("STATUS : LOW RPM");
  }
  else if (rpm < 6000) {
    display.println("STATUS : POWER BAND");
  }
  else if (rpm < 8000) {
    display.println("STATUS : SHIFT NOW");
  }
  else {
    display.println("STATUS : OVER REV");
  }

  display.setCursor(0, 45);
  display.print("SHIFT LEVEL : ");
  display.print(ledLevel);
  display.print("/10");

  display.display();

  Serial.print("RPM : ");
  Serial.println(rpm);

  delay(100);
}