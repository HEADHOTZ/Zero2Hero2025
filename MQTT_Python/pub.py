# pip install paho-mqtt
from paho.mqtt import client as mqtt
import time
import random  

BROKER = "test.mosquitto.org"
PORT   = 1883
TOPIC  = "67015080/sensor"   # Change to your own topic

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id="basic-pub-67015000")
client.connect(BROKER, PORT, 60)

try:
    while True:
        value = random.randint(1, 100)  
        msg = str(value)
        client.publish(TOPIC, payload=msg, qos=1, retain=False)
        print("[PUB]", msg)
        time.sleep(1) 
except KeyboardInterrupt:
    print("Stopped by user")
finally:
    client.disconnect()
