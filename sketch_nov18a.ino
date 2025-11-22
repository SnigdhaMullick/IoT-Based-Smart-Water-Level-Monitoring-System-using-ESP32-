/************************************************************
        Smart Water Level Monitoring System
        Hardware: ESP32, HC-SR04, Blynk App
        Features:
        ✔ Sends water level to Blynk
        ✔ Sends alerts when tank is FULL or EMPTY
        ✔ Shows % and cm level
        ✔ Easy to add Motor Control
************************************************************/

#define BLYNK_TEMPLATE_ID "TMPL3AacZ9Fkd"
#define BLYNK_TEMPLATE_NAME "Water Level Monitor"
#define BLYNK_AUTH_TOKEN "EaLSUWPIX4PVQS8excClQd5ZgqshlkyM"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32_SSL.h>

// Wi-Fi Credentials
char ssid[] = "snigdha";
char pass[] = "Mullick0317";

// Ultrasonic Sensor Pins
#define TRIG_PIN 5
#define ECHO_PIN 18

// Tank height in cm
const int TANK_DEPTH = 100;

long duration;
float distance, waterLevel, waterPercent;

BlynkTimer timer;

// Alert flags (to avoid repeated notifications)
bool fullAlertSent = false;
bool emptyAlertSent = false;

/***************** Measure Water Level *****************/
void measureWaterLevel() {

  // Ultrasonic trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  distance = duration * 0.034 / 2;    // Distance from sensor
  waterLevel = TANK_DEPTH - distance; // Water available in cm
  waterPercent = (waterLevel / TANK_DEPTH) * 100.0;

  if (waterPercent < 0) waterPercent = 0;
  if (waterPercent > 100) waterPercent = 100;

  // Send values to Blynk
  Blynk.virtualWrite(V0, waterPercent);
  Blynk.virtualWrite(V1, waterLevel);

  // Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Water Level: ");
  Serial.print(waterLevel);
  Serial.print(" cm | Tank Filled: ");
  Serial.print(waterPercent);
  Serial.println(" %");

  /**************** ALERT LOGIC ****************/

  // Tank FULL alert
  if (waterPercent >= 90 && !fullAlertSent) {
    Blynk.logEvent("tank_full", "⚠ Tank is FULL! Water Level above 90%");
    fullAlertSent = true;
    emptyAlertSent = false;   // Reset empty alert
  }

  // Tank EMPTY alert
  if (waterPercent <= 10 && !emptyAlertSent) {
    Blynk.logEvent("tank_empty", "⚠ Tank is EMPTY! Water Level below 10%");
    emptyAlertSent = true;
    fullAlertSent = false;    // Reset full alert
  }
}

/***************** Setup *****************/
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);

  // Call every 1 second
  timer.setInterval(1000L, measureWaterLevel);
}

/***************** Loop *****************/
void loop() {
  Blynk.run();
  timer.run();
}
