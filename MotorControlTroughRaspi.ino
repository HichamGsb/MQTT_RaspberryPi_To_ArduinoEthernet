// This code connect to a raspberry pi using ethernet and the mqtt protocol
// Raspberry Pi IP Address : 192.168.1.2 
// Arduino Ethernet IP Address : 192.168.1.3
// The mqtt server run on the raspi using the port 1883

#include "Arduino.h"
#include "PubSubClient.h"
#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include <SPI.h>
#include <Ethernet.h>               // Tested version 2.0.1, should work above

#define CMD (byte)0x00              // Registre principal qui doit être appellé avant tous les autres Specifique à la commande "Serial1"
#define Set_Speed1          0x31    // Registre pour la Vitesse du moteur1
#define Set_Speed2_Turn     0x32    // Registre pour la Vitesse du moteur2 ou valeur du Turn selon Mode
#define Set_Acceleration    0x33    // Registre pour l'accélération
#define Reset_Encoders      0x35    // Registre pour effectuer le Reset des encodeurs
#define Set_Mode            0x34    // Registre des Modes (0 défaut, 1,2 et 3) 
#define Get_Encoder1        0x23    // Registre de Lecture de l'encodeur_1.
#define Get_Encoder2        0x24    // Registre de Lecture de l'encodeur_2.
#define Get_Encoders        0x25    // Registre de Lecture des 2 encodeurs.
#define Enable_Regulation   0x36    
#define Disable_Regulation  0x37    
#define Enable_TimeOut      0x3     

uint8_t mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x72, 0x8E }; // Arduino MAC Address (Writen on a label to the back)

IPAddress ip(192, 168, 1, 3);           // Arduino IP Address (Raspi IP Address+1)
IPAddress dns(192, 168, 1, 2);          // Pi address
IPAddress gateway(192, 168, 1, 2);      // Pi address
IPAddress subnet(255, 255, 255, 0);
IPAddress mqtt_server(192, 168, 1, 2);  // Pi address
const int mqtt_port = 1883;

EthernetClient ethClient;
PubSubClient mqttClient;

bool objectIsClose = false;
// Since we don't have a direct serial port on the arduino, we use pin 6 and 7 to communicate with the MD25
SoftwareSerial md25Serial(6, 7); 

void setup(void) {
  Serial.begin(115200);

  md25Serial.begin(38400);
  
  Ethernet.begin(mac, ip);              // Starting ethernet interface
  
  mqttClient.setClient(ethClient);
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);     // Set the function that's gonna be called for each message recieved
}

void loop() {
  // Reconnect if connnection was lost
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  if (!objectIsClose) {                 // If no object is near the robot we can go
    md25Serial.write(CMD);
    md25Serial.write(Set_Speed1);
    md25Serial.write(160);
  } else {                              // If an object is detected, we stop the motor
    md25Serial.write(CMD);
    md25Serial.write(Set_Speed1);
    md25Serial.write(128);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    // Attempt to connect
    Serial.println("Attempting to connect to MQTT Broker");
    if (mqttClient.connect("arduino-client")) {
      // Subscribe to MQTT topic
      Serial.println("Subscribing to topic 'minDistance'");
      mqttClient.subscribe("minDistance");
      Serial.println("Subscribed !");
    } else {
      // Wait 1 second before retrying
      Serial.println("Could not connect to MQTT Broker. Retrying in 1s...");
      Serial.println(mqttClient.state());
      delay(1000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  objectIsClose = message.toFloat() < 150.0;    // If the distance given to us by the lidar is less than 15cm there is an object close

  // Print received message
  Serial.print("Received message: ");
  Serial.println(message);
}