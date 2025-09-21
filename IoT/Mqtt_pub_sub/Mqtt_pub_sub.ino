/*************************************************************
  Project : ESP32-S3 MQTT Publisher with DHT11 (Unified API)
  Description : Publishes temperature and humidity separately
                to two MQTT topics every 1 second using
                Adafruit Unified Sensor API for DHT11.
  Hardware : ESP32-S3-WROOM-1, DHT11 Sensor on GPIO 10
  Author : Age of Robotics (Modified with Unified Comments)
*************************************************************/

#include <WiFi.h>             // WiFi library for ESP32 (by Espressif)
#include <PubSubClient.h>     // MQTT client library (by Nick O'Leary)

/************** WiFi Network Credentials ****************/
const char* ssid = "Yo";              // <-- Replace with your WiFi SSID
const char* password = "1234566495";  // <-- Replace with your WiFi Password

/************** MQTT Broker Settings *******************/
const char* mqtt_server_ip = "10.44.188.224";  // <-- Replace with your MQTT Broker IP
const int mqtt_port = 1883;                    // Default MQTT port

/************** MQTT Topics ***************************/
const char* sensor_topic = "IoT/sensor";  // Topic to publish sensor

/************** MQTT Client Declarations ***************/
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/************** INTITIAL ***************/
bool state_cmd;
int rand_number;
unsigned long previousMillis = 0;
const long interval = 1000;

/************** Connect to WiFi Network ***************/
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);  // Start WiFi connection

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

/************** Reconnect to MQTT Broker ***************/
void reconnectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT Client...");
    if (mqttClient.connect("ESP32_S3_Client")) {
      Serial.println("connected to MQTT broker!");
      mqttClient.subscribe("IoT/cmd");  // Subscribe to topic after successful connection
    } else {
      Serial.print("failed with state ");
      Serial.print(mqttClient.state());
      Serial.println(". Retrying in 5 seconds...");
      delay(5000);  // Wait before retrying
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String incomingMessage = "";

  // Convert incoming byte array to String
  for (unsigned int i = 0; i < length; i++) {
    incomingMessage += (char)payload[i];
  }

  incomingMessage.trim();  // Remove trailing whitespace or newline

  // Act only if topic is "ledState"
  if (String(topic) == "IoT/cmd") {
    if (incomingMessage == "start") {
      state_cmd = true;
      Serial.println("true");
    } else if (incomingMessage == "stop") {
      state_cmd = false;
      Serial.println("false");
    }
    // No action needed if state hasn't changed
  }
}

void MqttConnect() {
  if (!mqttClient.connected()) {
    reconnectToMQTT();  // Ensure MQTT connection
  }
  mqttClient.loop();  // Keep MQTT client alive
}

void BlinkLed() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

/************** Arduino Setup Function ****************/
void setup() {
  Serial.begin(115200);  // Initialize Serial Monitor

  pinMode(LED_BUILTIN, OUTPUT);

  connectToWiFi();                                  // Connect to WiFi
  mqttClient.setServer(mqtt_server_ip, mqtt_port);  // Set MQTT server
  mqttClient.setCallback(mqttCallback);             // Set the callback function
}

/************** Arduino Main Loop *********************/
void loop() {
  MqttConnect();

  unsigned long currentMillis = millis();

  if (state_cmd == true) {
    rand_number = random(100);
    char msg[10];
    sprintf(msg, "%d", rand_number);
    
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; 
      mqttClient.publish(sensor_topic, msg);
      BlinkLed();
    }
  } else {
    digitalWrite(LED_BUILTIN, 0);
  }
}