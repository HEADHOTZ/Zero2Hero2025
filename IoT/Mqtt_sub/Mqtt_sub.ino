#include <WiFi.h>           // WiFi library for ESP32
#include <PubSubClient.h>   // MQTT library

/************** WiFi Network Credentials ****************/
const char* ssid = "Yo1";           // <-- Replace with your WiFi SSID
const char* password = "1234566495";  // <-- Replace with your WiFi Password

/************** MQTT Broker Settings *******************/
const char* mqtt_server = "test.mosquitto.org";  // <-- Replace with your Broker IP address
const int mqtt_port = 1883;                     // Default MQTT port
const char* client_id = "Bunyakorn";  // <-- Replace with your client_id

/************** MQTT Topics ***************************/
const char* sub_topic = "67015080/command";  // Topic to subscribe sensor

/************** Object Declarations ********************/
WiFiClient wifiClient;              // Create a WiFi client
PubSubClient mqttClient(wifiClient); // Create an MQTT client using WiFi

/************** Global Variables ***********************/
const int ledPin = LED_BUILTIN;             // LED connected to GPIO 13

/************** Function to Connect to WiFi *************/
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  // Wait until WiFi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

/************** Function to Handle Incoming MQTT Messages *************/
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String incomingMessage = "";

  // Convert incoming byte array to String
  for (unsigned int i = 0; i < length; i++) {
    incomingMessage += (char)payload[i];
  }

  incomingMessage.trim(); // Remove trailing whitespace or newline

  // Act only if topic is sub_topic
  if (String(topic) == sub_topic) {
    if (incomingMessage == "on") {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED turned ON");
    }
    else if (incomingMessage == "off") {
      digitalWrite(ledPin, LOW);
      Serial.println("LED turned OFF");
    }
    // No action needed if state hasn't changed
  }
}

/************** Function to Reconnect to MQTT Broker *************/
void reconnectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT Client...");

    // Attempt to connect with a client ID
    if (mqttClient.connect("client_id")) {
      Serial.println("connected to MQTT broker!");
      mqttClient.subscribe(sub_topic); // Subscribe to topic after successful connection
    }
    else {
      Serial.print("failed with state ");
      Serial.print(mqttClient.state());
      Serial.println(". Retrying in 5 seconds...");
      delay(5000); // Wait before retrying
    }
  }
}

/************** Arduino Setup Function *************/
void setup() {
  Serial.begin(115200); // Start Serial Monitor
  delay(100);           // Short delay for stability

  pinMode(ledPin, OUTPUT);        // Set LED pin as output
  digitalWrite(ledPin, LOW);      // Ensure LED is initially OFF

  connectToWiFi();                          // Connect to WiFi
  mqttClient.setServer(mqtt_server, mqtt_port); // Set MQTT Broker
  mqttClient.setCallback(mqttCallback);     // Set the callback function
}

/************** Arduino Loop Function *************/
void loop() {
  // Ensure connection to MQTT server
  if (!mqttClient.connected()) {
    reconnectToMQTT();
  }
  
  mqttClient.loop(); // Process incoming MQTT messages
}