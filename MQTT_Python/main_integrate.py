# pip install paho-mqtt
from paho.mqtt import client as mqtt
import time, random

BROKER = "test.mosquitto.org"
PORT   = 1883
CLIENT_ID = "Bunyakorn"
TOPIC_CMD = "67015080/command"   
TOPIC_OUT = "67015080/sensor"    

state_send = False              
interval   = 1000   # หน่วงเวลา (ms) เหมือน millis()
last_pub   = 0      # เก็บเวลา publish ล่าสุด (ms)

def millis():
    """คืนค่าเวลาปัจจุบันเป็น ms"""
    return int(time.time() * 1000)

def on_connect(client, userdata, flags, reason_code, properties=None):
    print("Connected:", reason_code)
    client.subscribe(TOPIC_CMD, qos=1)
    print(f"Subscribed: {TOPIC_CMD}")
    print(f"ส่งคำสั่งที่ topic '{TOPIC_CMD}' เป็น 'on' หรือ 'off' เพื่อเริ่ม/หยุดการส่งค่า")
    
def on_message(client, userdata, msg):
    global state_send
    cmd = msg.payload.decode("utf-8", "ignore").strip().lower()
    print(f"[CMD] {msg.topic} -> {cmd}")
    if cmd == "on":
        state_send = True
        print("START sending mock data")
    elif cmd == "off":
        state_send = False
        print("STOP sending mock data")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=CLIENT_ID)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT, 60)
client.loop_start()  

try:
    while True:
        now = millis()
        if state_send and (now - last_pub >= interval):
            value = random.randint(1, 100)   
            msg = str(value)
            client.publish(TOPIC_OUT, payload=msg, qos=1, retain=False)
            print(f"[PUB] {TOPIC_OUT} -> {msg}")
            last_pub = now

        time.sleep(0.01)  # ป้องกัน while ใช้ CPU เต็ม
except KeyboardInterrupt:
    pass
finally:
    client.loop_stop()
    client.disconnect()
