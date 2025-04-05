import json
import time
import paho.mqtt.client as mqtt
from os import path
import csv
from datetime import datetime

id = "2d85d648-2b06-40f5-a556-9e12aca7329f"
client_telemetry_topic = f"{id}/telemetry"
server_command_topic = f"{id}/commands"
client_name = f"{id}_temperature_logger"

temperature_file_name = 'temperature.csv'
fieldnames = ['date', 'tempC', 'tempF']

if not path.exists(temperature_file_name):
    with open(temperature_file_name, mode='w') as csv_file:
        writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
        writer.writeheader()

def handle_telemetry(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode())
        tempC = payload.get('tempC')
        tempF = payload.get('tempF')

        if tempC is not None and tempF is not None:
            with open(temperature_file_name, mode='a') as temperature_file:
                writer = csv.DictWriter(temperature_file, fieldnames=fieldnames)
                writer.writerow({
                    'date': datetime.now().astimezone().replace(microsecond=0).isoformat(),
                    'tempC': tempC,
                    'tempF': tempF
                })
            print("✅ Data saved:", tempC, tempF)
        else:
            print("⚠️ Incomplete temperature data.")
    except json.JSONDecodeError:
        print("❌ Invalid JSON payload.")

mqtt_client = mqtt.Client(client_name)
mqtt_client.on_message = handle_telemetry

print("🔗 Connecting to MQTT broker...")
mqtt_client.connect('test.mosquitto.org')
mqtt_client.subscribe(client_telemetry_topic, qos=1)

print(f"📡 Listening on topic: {client_telemetry_topic}")
mqtt_client.loop_forever()
