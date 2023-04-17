#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <MQTT.h>

// MQTT Credentials
#define MQTT_USERNAME "slnp_device_1"
#define MQTT_PASSWORD "slnp_device_1"

#define PUB_TOPIC "sub/slnp/test"     // "sub/1119/tester2/tesingdev2/v3/pub"

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

// WIFI credentials SSID/Password combination
const char* ssid = "MenaMega";
const char* password = "Shobana01";


const char* mqtt_server = "broker.hivemq.com";
const uint16_t mqtt_port = 1883;

WiFiClient espClient;
MQTTClient client;

String clientId = "";
char setmsg[50];

const int trigPin = 5;
const int echoPin = 18;

long duration;
float distanceCm;
float distanceInch;

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  setup_wifi();
  client.begin(mqtt_server, mqtt_port, espClient);
//  client.setCallback(callback);
}

void loop() {
  if (client.connected()) {
    client.loop();
  }else{
    Serial.println(F("Connecting MQTT..."));
    ConnectToMQTT();
    delay(100);
  }
  delay(1000);
  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);
  //client.publish("slnp/distance", distanceCm);
  publishEvent(distanceCm);
  
  delay(1000);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

boolean ConnectToMQTT(){
  randomSeed(analogRead(34)); //analog pin 34 used to genarate random number
  clientId = "TEST-" + String(millis()) + String(random(0, 100000000), HEX);
    Serial.println(clientId);
  if (!client.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD)){
    Serial.println(F("MQTT fail"));
    return false;
  }
//  Serial.println(SUB_TOPIC);
  //mqtt.subscribe(SUB_TOPIC);
  return client.connected();
}

void publishEvent(float distance){
  sprintf(setmsg,"{\"distance\": %.2f}",distance);
  if (!client.publish(PUB_TOPIC, setmsg)){
    Serial.println(F("Failed."));
  }else{
    Serial.println(setmsg);
    Serial.println(F("SEND:OK"));
  }
}
