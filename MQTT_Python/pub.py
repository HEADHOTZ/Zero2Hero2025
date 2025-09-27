# pip install paho-mqtt
from paho.mqtt import client as mqtt
import time
import random  

BROKER = "test.mosquitto.org" # Change to your own broker
PORT   = 1883                  # Change to your own port
TOPIC  = "67015080/sensor"   # Change to your own topic
CLIENT_ID = "Bunyakorn"      # Change to your own client ID

INTERVAL = 1000  # 1000 ms = 1 วินาที

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=CLIENT_ID)
client.connect(BROKER, PORT, 60)

# Start the loop
client.loop_start()

previousMillis = int(time.time() * 1000)

try:
    while True:
        currentMillis = int(time.time() * 1000)
        
        if currentMillis - previousMillis >= INTERVAL:
            previousMillis = currentMillis
            
            # Simulate a sensor value
            value = random.randint(1, 100)  
            msg = str(value)
            client.publish(TOPIC, payload=msg, qos=1, retain=False)
            print("[PUB]", msg)

# Ctrl + C to stop
except KeyboardInterrupt:
    print("Stopped by user")
finally:
    # Stop the loop and disconnect
    client.loop_stop()
    client.disconnect()
