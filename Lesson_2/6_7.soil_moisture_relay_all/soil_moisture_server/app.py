import json
import time
import paho.mqtt.client as mqtt
import threading

DRYNESS_THRESHOLD = 15  # Percent

id = "192a986b-602f-4092-be87-89f3bc80e9b0"
client_telemetry_topic = id + '/telemetry'
server_command_topic = id + '/commands'
client_name = id + 'soil_moisture_server'

water_time = 5  # Open relay for 5 seconds
wait_time = 20  # Wait 20 seconds for water to soak

def send_relay_command(client, state):
    command = {'relay_on': state}
    print(f"📡 Sending command: {command}")
    client.publish(server_command_topic, json.dumps(command))

def control_relay(client):
    print("Unsubscribing from telemetry")
    mqtt_client.unsubscribe(client_telemetry_topic)

    send_relay_command(client, True)
    time.sleep(water_time)
    send_relay_command(client, False)

    time.sleep(wait_time)
    print("Subscribing to telemetry")
    mqtt_client.subscribe(client_telemetry_topic)

def handle_telemetry(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode())
        print(f"📩 Message received: {payload}")

        soil_moisture = payload.get('soil_moisture', 999)

        if soil_moisture != 999 and soil_moisture < DRYNESS_THRESHOLD:
            threading.Thread(target=control_relay, args=(client,)).start()

    except json.JSONDecodeError:
        print("❌ Invalid JSON payload.")

mqtt_client = mqtt.Client(client_name)
mqtt_client.on_message = handle_telemetry

print("🔗 Connecting to MQTT broker...")
mqtt_client.connect('test.mosquitto.org')

mqtt_client.subscribe(client_telemetry_topic, qos=1)

print(f"📡 Listening for messages on: {client_telemetry_topic}")
mqtt_client.loop_forever()
