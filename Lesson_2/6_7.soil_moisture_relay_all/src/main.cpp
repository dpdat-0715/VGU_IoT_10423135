#include <Arduino.h>
#include <ArduinoJSON.h>
#include <math.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <SPI.h>

#include "config.h"

#define SOIL_MOISTURE_PIN 34  // GPIO34 for soil moisture sensor
#define PIN_WIRE_SCL 26 // GPIO26 for relay control
#define ABSOLUTE_DRYNESS 4095

int calculate_soil_moisture_percentage(int soil_moisture)
{
    return map(soil_moisture, ABSOLUTE_DRYNESS, 0, 0, 100);
}

void connectWiFi()
{
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
        Serial.println("\nConnected to WiFi!");
    } else {
        Serial.println("\nFailed to connect. Check WiFi settings.");
    }
}

WiFiClient espClient;         
PubSubClient client(espClient); 

unsigned long lastReconnectAttempt = 0;  
const long reconnectInterval = 5000;     

void reconnectMQTTClient()
{
  if (client.connected()) return;  
  
  if (millis() - lastReconnectAttempt >= reconnectInterval) {
    lastReconnectAttempt = millis();  

    Serial.print("Attempting MQTT connection...");
    if (client.connect(CLIENT_NAME.c_str())) {
        Serial.println("connected");
        client.subscribe(SERVER_COMMAND_TOPIC.c_str());
        Serial.println("Subscribed to command topic.");
    } else {
        Serial.print("Failed, rc=");
        Serial.println(client.state());
    }
  }
}

void clientCallback(char *topic, uint8_t *payload, unsigned int length)
{
    char buff[length + 1];
    for (int i = 0; i < length; i++)
    {
        buff[i] = (char)payload[i];
    }
    buff[length] = '\0';

    Serial.print("Message received:");
    Serial.println(buff);

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, buff);
    if (error) {
      Serial.println("⚠️ JSON parsing failed!");
      return;
    }
    
    JsonObject obj = doc.as<JsonObject>();
    bool relay_on = obj["relay_on"];

    digitalWrite(PIN_WIRE_SCL, relay_on ? HIGH : LOW);
}

void createMQTTClient()
{
    client.setServer(BROKER.c_str(), 1883);
    reconnectMQTTClient();
    client.setCallback(clientCallback);
}

unsigned long lastTelemetryTime = 0;  
const long telemetryInterval = 10000;  

void setup() {
    Serial.begin(9600); 
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    pinMode(PIN_WIRE_SCL, OUTPUT);

    connectWiFi();
    createMQTTClient();
}

void loop() {
    reconnectMQTTClient();
    client.loop();

    if (millis() - lastTelemetryTime >= telemetryInterval)
    {
      lastTelemetryTime = millis();  

      int soil_moisture = calculate_soil_moisture_percentage(analogRead(SOIL_MOISTURE_PIN));

      DynamicJsonDocument doc(1024);
      doc["soil_moisture"] = soil_moisture;
      
      String telemetry;
      serializeJson(doc, telemetry);

      Serial.print("Sending telemetry: ");
      Serial.println(telemetry.c_str());

      client.publish(CLIENT_TELEMETRY_TOPIC.c_str(), telemetry.c_str(), 1);
    }
}
