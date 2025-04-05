#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"

// DHT sensor settings
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// WiFi & MQTT
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastReconnectAttempt = 0;
const long reconnectInterval = 5000;

unsigned long lastTelemetryTime = 0;
const long telemetryInterval = 10000;

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.disconnect(true);
  delay(1000);
  WiFi.begin(SSID, PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    Serial.print(".");
    delay(1000);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected.");
    // Serial.println(WiFi.localIP()); // Optional IP display
  } else {
    Serial.println("\nWiFi connection failed.");
  }
}

void reconnectMQTTClient() {
  if (client.connected()) return;

  if (millis() - lastReconnectAttempt >= reconnectInterval) {
    lastReconnectAttempt = millis();

    Serial.print("Connecting to MQTT...");
    if (client.connect(CLIENT_NAME.c_str())) {
      Serial.println("connected.");
      client.subscribe(SERVER_COMMAND_TOPIC.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
    }
  }
}

void createMQTTClient() {
  client.setServer(BROKER.c_str(), 1883);
  reconnectMQTTClient();
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  connectWiFi();
  createMQTTClient();
}

void loop() {
  if (!client.connected()) {
    reconnectMQTTClient();
  }
  client.loop();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Skipping telemetry.");
    delay(1000);
    return;
  }

  if (millis() - lastTelemetryTime >= telemetryInterval) {
    lastTelemetryTime = millis();

    float tempC = dht.readTemperature();
    float tempF = dht.readTemperature(true);

    if (isnan(tempC) || isnan(tempF)) {
      Serial.println("Failed to read from DHT sensor.");
      return;
    }

    DynamicJsonDocument doc(1024);
    doc["tempC"] = tempC;
    doc["tempF"] = tempF;

    String payload;
    serializeJson(doc, payload);

    Serial.print("Publishing: ");
    Serial.println(payload);

    if (!client.publish(CLIENT_TELEMETRY_TOPIC.c_str(), payload.c_str(), 1)) {
      Serial.println("MQTT publish failed.");
    }
  }
}
