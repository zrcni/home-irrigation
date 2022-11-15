#include <TimeLib.h>
#include "time.h"
#include "wifi.h"
#include "mqtt.h"

const uint8_t sensor_output_pin = 3;
// analog pin
const uint8_t sensor_input_pin = A0;

const uint8_t relay_output_pin = 10;
const uint16_t moisture_limit = 780;
const uint16_t water_release_duration = 2500;
// 30 minutes
const uint32_t check_interval = 1800000UL;
// const uint32_t check_interval = 5000L;
uint16_t moisture_value = 0;


void setup() {
  Serial.begin(115200);
  while (!Serial) {};

  connectWiFi();
  setupTimeSync();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(sensor_output_pin, OUTPUT);
  pinMode(sensor_input_pin, INPUT);

  /*
    pinMode(relay_output_pin, OUTPUT);
  */


}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);

  moisture_value = readMoisture();

  if (isDry(moisture_value)) {
    handleDry(moisture_value);
  } else {
    handleWet(moisture_value);
  }

  digitalWrite(LED_BUILTIN, LOW);

  delay(check_interval);
}

void handleDry(uint16_t moisture_value) {
  time_t timestamp = now();

  Serial.println(moisture_value);
  Serial.println("dry :(");

  releaseWater(water_release_duration);

  connectMQTT();

  char* payload = buildMessagePayload(timestamp, moisture_value, water_release_duration);
  publishMessage(payload);

  disconnectMQTT();
}

void handleWet(uint16_t moisture_value) {
  Serial.println(moisture_value);
  Serial.println("wet :)");
}

// release water for a duration
void releaseWater(uint16_t ms) {
  Serial.println("Pretending to release water...");
  delay(ms);
  /*
    analogWrite(relay_output_pin, 255);
    delay(ms);
    analogWrite(relay_output_pin, 0);
  */
}

uint16_t readMoisture() {
  return 810;
  // power up the sensor
  digitalWrite(sensor_output_pin, 255);

  // Wait a bit for the sensor to get
  // set up after powering it up.
  delay(100);

  moisture_value = analogRead(sensor_input_pin);

  digitalWrite(sensor_output_pin, 0);

  return moisture_value;
}

bool isDry(uint16_t value) {
  return value > moisture_limit;
}
