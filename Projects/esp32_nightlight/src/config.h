#pragma once

#include <string>
using namespace std;


const char* SSID = "Chicken Nugget";
const char* PASSWORD = "tumotdentam";

const string ID = "32eb34b1-ffef-4507-a768-772e8005ba06";
const string BROKER = "test.mosquitto.org";
const string CLIENT_NAME = ID + "nightlight_client";


const string CLIENT_TELEMETRY_TOPIC = ID + "/telemetry";

const string SERVER_COMMAND_TOPIC = ID + "/commands";