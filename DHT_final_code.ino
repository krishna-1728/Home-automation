#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 18     // Digital pin connected to the DHT sensor

#define DHTTYPE    DHT11     // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);

const int motorPin = 32;
#include "UbidotsEsp32Mqtt.h"

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBUS-3zPhXSmPS8b7z0brAphfq1Dw1Zttz0";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "vivo 1814";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "12345678";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "dht22";   // Put here your Device label to which data  will be published
 // Put here your Variable label to which data  will be published


Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  dht.begin();
  pinMode(motorPin, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
 
  int ACValuee;
  int motorspeedu;// Declare ACValuee here
  float tempu;
  // Read temperature and humidity
  sensors_event_t event;
 
  // Read temperature
  dht.temperature().getEvent(&event);
  if (!isnan(event.temperature)) {
    // Map temperature to PWM range
    int ACValue = map(event.temperature, 22, 30, 23, 18);
    ACValuee = ACValue; // Assign value to ACValuee

    // Calculate motor speed based on temperature
    int motorSpeed = map(event.temperature, 20, 38, 0, 255);
    analogWrite(motorPin, motorSpeed);
    float analog_voltage = (motorSpeed / 255.0) * 3.3; // Assuming you're using a 3.3V Arduino

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
    Serial.print("AC Value: ");
    Serial.println(ACValue);
  } else {
    Serial.println("Error reading temperature!");
  }

  // Read humidity
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

  // Publish sensor data to Ubidots
  ubidots.add("ac temperature", ACValuee);
  ubidots.publish("dht22");

  ubidots.add("humidity", hum);
  ubidots.publish("dht22");

  ubidots.add("temperature", tempu);
  ubidots.publish("dht22");

  ubidots.add("fan speed", motorspeedu);
  ubidots.publish("dht22");

  ubidots.loop();
  delay(2000);
}