#include <Arduino.h>
#include <ArduinoJSON.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <SPI.h>
#include "config.h"

#define LED_PIN 5 
#define SENSOR_PIN 34

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

void reconnectMQTTClient()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(CLIENT_NAME.c_str()))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("Retying in 5 seconds - failed, rc=");
            Serial.println(client.state());
            delay(5000);
        }
    }
    client.subscribe(SERVER_COMMAND_TOPIC.c_str());
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
    deserializeJson(doc, buff);
    JsonObject obj = doc.as<JsonObject>();
    bool led_on = obj["led_on"];

    if (led_on)
        digitalWrite(LED_PIN, HIGH);
    else
        digitalWrite(LED_PIN, LOW);
}

void createMQTTClient()
{
    client.setServer(BROKER.c_str(), 1883);
    reconnectMQTTClient();
    client.setCallback(clientCallback);
}

unsigned long lastTelemetryTime = 0;
const long telemetryInterval = 2000;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);
    connectWiFi();
    createMQTTClient();
}

void loop()
{
    reconnectMQTTClient();
    client.loop();
    
    if (millis() - lastTelemetryTime >= telemetryInterval) {
        lastTelemetryTime = millis();
        int light = esp_random() % 1024;
        DynamicJsonDocument doc(1024);
        doc["light"] = light;
        String telemetry;
        serializeJson(doc, telemetry);
        Serial.print("Sending telemetry ");
        Serial.println(telemetry.c_str());
        client.publish(CLIENT_TELEMETRY_TOPIC.c_str(), telemetry.c_str());
    }
}
