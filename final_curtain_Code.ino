#include <ESP32Servo.h>
#include "UbidotsEsp32Mqtt.h"

#define SERVO_PIN 13

const char *UBIDOTS_TOKEN = "BBUS-3zPhXSmPS8b7z0brAphfq1Dw1Zttz0";
const char *WIFI_SSID = "Krishna1";
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
  Serial.println();

  // Extract input value from payload
  int inputValue = atoi((char*)payload);

  // Map input value to servo angle range (0 to 4 -> 0 to 180)
  int angle = map(inputValue, 0, 4, 0,120);

  // Set servo angle according to the mapped value
  servo.write(angle);

  // Print the angle of the servo
  Serial.print("Servo angle set to: ");
  Serial.println(angle);
}

void setup() {
  Serial.begin(115200);
  ubidots.setup();
  ubidots.setDebug(true);
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.reconnect();
  servo.attach(SERVO_PIN);
  ubidots.subscribeLastValue("curtain", "sliderr");
}

void loop() {
  if (!ubidots.connected())
  {
    ubidots.reconnect();
    ubidots.subscribeLastValue("curtain", "sliderr"); // Insert the device and variable's Labels, respectively
  }

  ubidots.loop();
  delay(1000);
}
