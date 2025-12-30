#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "<WIFI_SSID_HERE>"
#define WIFI_PASSWORD "<WIFI_PASSWORD_HERE>"
#define MQTT_BROKER_IP "192.168.1.215"
#define MQTT_BROKER_PORT 1888
#define MQTT_PUBLISH_TOPIC "home/livingroom/irrigation"
#define CLIENT_ID "firebeetle"
#define DEVICE_ID "plant_waterer_1"

// TODO I want to be able to add compile-time git sha here, like __DATE__
// String VERSION = "unknown";

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, MQTT_BROKER_IP, MQTT_BROKER_PORT, "", "");

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  MQTT_connect();


    char payload[512];
    sprintf(payload, "{\"device_id\":\"%s\",\"client_id\":\"%s\"}",
      DEVICE_ID, CLIENT_ID);

  mqtt.publish(MQTT_PUBLISH_TOPIC, payload);
}

void loop() {
  /*
  if (!mqtt.connected()) {
    MQTT_connect();
  }
  */

  

}

void MQTT_connect() {
  int8_t ret;

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
