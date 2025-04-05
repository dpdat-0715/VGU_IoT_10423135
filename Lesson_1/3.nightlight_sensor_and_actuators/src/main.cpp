#include <Arduino.h>
const int LIGHT_SENSOR_PIN = 36;
const int LED_PIN = 2;

void setup() {
  Serial.begin(9600);

  while (!Serial);
  delay(1000);

  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int light = analogRead(LIGHT_SENSOR_PIN); 
  Serial.print("Light Value: ");
  Serial.println(light);

  if (light < 300) {
      digitalWrite(LED_PIN, HIGH);
  } else {
      digitalWrite(LED_PIN, LOW); 
  }

  delay(1000);
}