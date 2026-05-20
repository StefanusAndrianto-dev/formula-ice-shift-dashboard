#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <ESP32Servo.h>

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= DHT =================
#define DHTPIN 15
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// ================= SERVO =================
Servo coolingServo;

// ================= PIN =================
#define SERVO_PIN 18
#define BUZZER_PIN 19
#define BUTTON_PIN 17

// ================= LED BAR =================
int ledPins[10] = {13, 12, 14, 27, 26, 25, 33, 32, 4, 16};

bool alarmReset = false;

void setup() {

  Serial.begin(115200);

  // DHT
  dht.begin();

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  // Servo
  coolingServo.attach(SERVO_PIN);
  coolingServo.write(0);

  // LED Bar
  for (int i = 0; i < 10; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Pushbutton
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // OLED Initial
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
}

void loop() {

  // ================= READ TEMP =================
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("Failed to read DHT!");
    return;
  }

  // ================= BUTTON RESET =================
  if (digitalRead(BUTTON_PIN) == LOW) {
    alarmReset = true;
    noTone(BUZZER_PIN);
  }

  // ================= LED BAR CONTROL =================
  int ledLevel = map((int)temp, 20, 50, 0, 10);

  ledLevel = constrain(ledLevel, 0, 10);

  for (int i = 0; i < 10; i++) {

    if (i < ledLevel) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }

  // ================= SERVO CONTROL =================
  int servoAngle = map((int)temp, 20, 50, 0, 180);

  servoAngle = constrain(servoAngle, 0, 180);

  coolingServo.write(servoAngle);

  // ================= BUZZER CONTROL =================
  if (temp > 40 && !alarmReset) {
    tone(BUZZER_PIN, 1000);
  } else {
    noTone(BUZZER_PIN);
  }

  if (temp < 35) {
    alarmReset = false;
  }

  // ================= OLED DISPLAY =================
  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("FORMULA STUDENT");

  display.setCursor(0, 15);
  display.print("TEMP : ");
  display.print(temp);
  display.println(" C");

  display.setCursor(0, 30);

  if (temp < 30) {
    display.println("STATUS : NORMAL");
  }
  else if (temp < 40) {
    display.println("STATUS : WARNING");
  }
  else {
    display.println("STATUS : OVERHEAT");
  }

  display.setCursor(0, 45);
  display.print("COOLING : ");
  display.print(servoAngle);
  display.println(" DEG");

  display.display();

  delay(500);
}