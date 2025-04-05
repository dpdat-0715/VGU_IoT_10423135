#pragma once

#include <string>

using namespace std;

// WiFi credentials
const char* SSID = "Chicken Nugget";
const char* PASSWORD = "tumotdentam";

// MQTT settings
const std::string ID = "998db37c-acf1-442d-a495-5d6579778676";  // GUID string, without the angle brackets
const std::string BROKER = "test.mosquitto.org";
const std::string CLIENT_NAME = ID + "_nightlight_client";  // Concatenate at runtime, not compile-time

const string CLIENT_TELEMETRY_TOPIC = ID + "/telemetry";

const string SERVER_COMMAND_TOPIC = ID + "/commands";