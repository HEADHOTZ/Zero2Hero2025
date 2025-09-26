# pip install paho-mqtt
from paho.mqtt import client as mqtt

BROKER = "test.mosquitto.org"
PORT   = 1883
TOPIC  = "67015080/command" # Change to your own topic

def on_connect(client, userdata, flags, reason_code, properties=None):
    print("Connected:", reason_code)
    client.subscribe(TOPIC, qos=1)
    print("Subscribed:", TOPIC)

def on_message(client, userdata, msg):
    print(f"[SUB] {msg.topic} -> {msg.payload.decode('utf-8', 'ignore')}")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="basic-sub-67015080")
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT, 60)

try:
    client.loop_forever()
except KeyboardInterrupt:
    client.disconnect()