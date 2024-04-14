 This example works for both Industrial and STEM users.
 *
 * Developed by Jose Garcia, https://github.com/jotathebest/
 *
 * ****************************************/

/****************************************
 * Include Libraries
 ****************************************/
#include "UbidotsEsp32Mqtt.h"

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBUS-fhbrAs8g3nc5spnllgZ2qf3Od8A1Vo";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "Redmii";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "Sri@12345678";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "smoke";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "smokeey"; // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds

const int mq135_pin = 34; // Example GPIO pin supporting analog input on ESP32 DevKit

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
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }

  int sensor_value = analogRead(mq135_pin);

  // Convert analog reading to air quality measurement
  // Example conversion formula (may vary based on the sensor and calibration):
  // You need to calibrate the sensor and adjust this formula according to the datasheet.
  float ppm = (float)sensor_value / 1024.0 * 10000; // Example conversion assuming linear output

  // Print air quality measurement to serial monitor
  Serial.print("Air Quality (PPM): ");
  Serial.println(ppm);

  ubidots.add(VARIABLE_LABEL, ppm); // Insert your variable Labels and the value to be sent
  ubidots.publish(DEVICE_LABEL);

  delay(1000); // Wait for the specified interval before publishing again
  ubidots.loop();
}