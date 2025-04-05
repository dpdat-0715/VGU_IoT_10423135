#include <Arduino.h>

void setup() {
  Serial.begin(9600);

  while (!Serial);
  delay(1000);
}

void loop() {
  Serial.println("Hello World");
  delay(5000);
}
