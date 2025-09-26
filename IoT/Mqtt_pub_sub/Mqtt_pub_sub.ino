#include <WiFi.h>             // WiFi library for ESP32 (by Espressif)
#include <PubSubClient.h>     // MQTT client library (by Nick O'Leary)

/************** WiFi Network Credentials ****************/
const char* ssid = "Yo";              // <-- Replace with your WiFi SSID
const char* password = "1234566495";  // <-- Replace with your WiFi Password

/************** MQTT Broker Settings *******************/
const char* mqtt_server = "test.mosquitto.org";  // <-- Replace with your MQTT Broker IP
const int mqtt_port = 1883;                    // Default MQTT port

/************** MQTT Topics ***************************/
const char* pub_topic = "67015080/sensor";  // Topic to publish sensor

const char* sub_topic = "67015080/command";   // Topic to subscirbe for Node-RED

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
      mqttClient.subscribe(sub_topic);  // Subscribe to topic after successful connection
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
  if (String(topic) == sub_topic) {
    if (incomingMessage == "on") {
      state_cmd = true;
      Serial.println("true");
    } else if (incomingMessage == "off") {
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
  mqttClient.setServer(mqtt_server, mqtt_port);  // Set MQTT server
  mqttClient.setCallback(mqttCallback);             // Set the callback function
}

/************** Arduino Main Loop *********************/
void loop() {
  MqttConnect();

  unsigned long currentMillis = millis();

  if (state_cmd == true) {
    rand_number = random(100);
    
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; 
      mqttClient.publish(pub_topic, String(rand_number).c_str());
      BlinkLed();
    }
  } else {
    digitalWrite(LED_BUILTIN, 0);
  }
}