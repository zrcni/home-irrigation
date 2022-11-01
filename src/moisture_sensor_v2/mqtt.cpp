#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "wifi.h"

#define MQTT_BROKER_IP "192.168.1.215"
#define MQTT_BROKER_PORT 1888
#define MQTT_PUBLISH_TOPIC "home/livingroom/irrigation"
#define MQTT_CLIENT_ID "firebeetle"
#define MQTT_DEVICE_ID "plant_waterer_1"

Adafruit_MQTT_Client mqtt(&wifi_client, MQTT_BROKER_IP, MQTT_BROKER_PORT);

void connectMQTT() {
  int8_t ret;

  Serial.println("Connecting to MQTT...");

  uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }

  Serial.println("MQTT Connected!");
}

void disconnectMQTT() {
  mqtt.disconnect();
  Serial.println("MQTT disconnected");
}

void publishMessage(char *payload) {
  if (!mqtt.connected()) {
    Serial.println("MQTT client must be connected to publish a message!");
    return;
  }

  mqtt.publish(MQTT_PUBLISH_TOPIC, payload);

  Serial.print("Published a message to topic ");
  Serial.println(MQTT_PUBLISH_TOPIC);
}

char* buildMessagePayload(uint32_t timestamp, uint16_t moisture_value, uint16_t water_release_duration) {
  char payload[512];

  sprintf(payload, "{\"timestamp\":%d,\"moisture\":%d,\"water_duration\":%d,\"device_id\":\"%s\",\"client_id\":\"%s\"}",
          timestamp, moisture_value, water_release_duration, MQTT_DEVICE_ID, MQTT_CLIENT_ID);

  return payload;
}
