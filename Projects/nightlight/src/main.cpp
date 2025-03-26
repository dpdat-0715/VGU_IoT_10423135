#include <Arduino.h>

// Number of the pin the led is connected to
int led_pin = 5;

void setup() {
  Serial.begin(9600);
  // Set LED to OUTPUT 
  pinMode(led_pin, OUTPUT);
}

void loop() {
  digitalWrite(led_pin, HIGH);
  delay(1000);
  digitalWrite(led_pin, LOW);
  delay(1000);
}

