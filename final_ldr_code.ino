#include "UbidotsEsp32Mqtt.h"
#include <ESP32Servo.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define SERVO_PIN 13
#define DHTPIN 18     // Digital pin connected to the DHT sensor
#define DHTTYPE  DHT11     // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);

const int motorPin = 32;
const char *UBIDOTS_TOKEN = "BBUS-3zPhXSmPS8b7z0brAphfq1Dw1Zttz0";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "Krishna1";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "12345678";      // Put here your Wi-Fi password  // Put here your Device label to which data  will be published
const int ldrPin = 32; // LDR connected to ADC1_CHANNEL_6 (GPIO 32)
const int ledPin = 4;  // LED connected to GPIO 2
const int irSensorPin = 5; // IR sensor connected to GPIO 5
int bright; 


Ubidots ubidots(UBIDOTS_TOKEN);
Servo servo;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  int inputValue = atoi((char*)payload);

  int angle = map(inputValue, 0, 4, 0,120);

  servo.write(angle);

  Serial.print("Servo angle set to: ");
  Serial.println(angle);
}

void setup() {
  Serial.begin(115200);
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(irSensorPin, INPUT);
  pinMode(motorPin, OUTPUT);
  servo.attach(SERVO_PIN);
  ubidots.subscribeLastValue("curtain", "sliderr");
  // No need to initialize ADC for default channel (ADC1_CHANNEL_6 on GPIO 32)
}

void loop() {
  if (!ubidots.connected())
  {
    ubidots.reconnect();
    ubidots.subscribeLastValue("curtain", "sliderr");
  }

  // Check if the IR sensor output is low to start the process
  if (digitalRead(irSensorPin) == LOW) {
    // Read analog value from LDR
    int ldrValue = analogRead(ldrPin);

    // Map the analog value from the LDR (0-4095) to LED brightness (0-255)
    int brightness = map(ldrValue, 500,3700, 150,0);
      bright = map(ldrValue,500,3700,0,100);
    Serial.print(bright);
    analogWrite(ledPin, brightness);

    // Print LDR value and LED brightness to Serial Monitor
    Serial.print("LDR Value: ");
    Serial.print(ldrValue);
    Serial.print(" | LED Brightness: ",B);
    Serial.println(brightness);
    ubidots.add("light sensitivity",bright ); // Insert your variable Labels and the value to be sent
    ubidots.publish("ldr");
    ubidots.loop();
    int ACValuee;
  int motorspeedu;// Declare ACValuee here
  float tempu;
  // Read temperature and humidity
  sensors_event_t event;
 
 
  dht.temperature().getEvent(&event);
  if (!isnan(event.temperature)) {
    
    int ACValue = map(event.temperature, 22, 30, 23, 18);
    ACValuee = ACValue; 

    int motorSpeed = map(event.temperature, 20, 38, 0, 255);
    analogWrite(motorPin, motorSpeed);
    float analog_voltage = (motorSpeed / 255.0) * 3.3; 

    Serial.print("Analog Voltage: ");
    Serial.print(analog_voltage);
    Serial.println("V");

    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    tempu = event.temperature;
    Serial.println("°C");

    Serial.print("Motor Speed: ");
    motorspeedu=motorSpeed;
    Serial.println(motorSpeed);
  
  } else {
    Serial.println("Error reading temperature!");
  }
  int hum;
  dht.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity)) {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    hum = event.relative_humidity;
    Serial.println("%");
  } else {
    Serial.println("Error reading humidity!");
  }
  ubidots.add("ac temperature", ACValuee);
  ubidots.add("humidity", hum);
  ubidots.add("temperature", tempu);
  ubidots.add("fan speed", motorspeedu);
  ubidots.publish("dht22");

  ubidots.loop();
  } else {
    // If the IR sensor output is high, print "Process terminated"
    Serial.println("Process terminated.");
    digitalWrite(ledPin,LOW);
  }
  ubidots.loop();

  delay(1000); // Adjust delay as needed
}
