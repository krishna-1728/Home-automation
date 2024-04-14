#include <ESP32Servo.h>
#include "UbidotsEsp32Mqtt.h"
#define IR_SENSOR_POWER_PIN 5
#define BUZZER_PIN 4
#define SERVO_PIN 13// Assuming you're using pin D12 for the servo

const int irSensorPin = 23;
const int thresholdValue = HIGH;
int irSensorValue;
bool sensorPowered = false;
int value;

const char *UBIDOTS_TOKEN = "BBUS-3zPhXSmPS8b7z0brAphfq1Dw1Zttz0";            // Put here your Ubidots TOKEN
const char *WIFI_SSID = "vivo 1814";                // Put here your Wi-Fi SSID
const char *WIFI_PASS = "12345678";    

Ubidots ubidots(UBIDOTS_TOKEN);
Servo servo;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == '1') {
    digitalWrite(IR_SENSOR_POWER_PIN, HIGH);
    sensorPowered = true;
    // Set servo to 0 degrees when IR sensor power is high
    servo.write(0);
   
  }
 
  else {
    digitalWrite(IR_SENSOR_POWER_PIN, LOW);
    sensorPowered = false;
    // Set servo to 90 degrees when IR sensor power is low
    servo.write(90);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  ubidots.setup();
  ubidots.setDebug(true);  
 ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
   ubidots.reconnect();
  pinMode(IR_SENSOR_POWER_PIN, OUTPUT);
  pinMode(irSensorPin, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  servo.attach(SERVO_PIN); // Attach the servo to its pin
 ubidots.subscribeLastValue("irsensorcontrol66", "irsensor55"); 
}

void loop() {
  if (!ubidots.connected())
  {
    ubidots.reconnect();
    ubidots.subscribeLastValue("irsensorcontrol66", "irsensor55"); // Insert the device and variable's Labels, respectively
  }

  if (sensorPowered) {
    value = digitalRead(irSensorPin);
    irSensorValue = !value;

    Serial.print("IR Sensor Value: ");
    Serial.println(irSensorValue);
 
    if (irSensorValue == thresholdValue) {
      Serial.println("Object detected!");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(3000);
      digitalWrite(BUZZER_PIN, LOW);
    }
 
     ubidots.add("irsensorval", irSensorValue);  // Insert your variable Labels and the value to be sent
    ubidots.publish("ir_sensor");
  }
 
 ubidots.loop();
  delay(1000);
}