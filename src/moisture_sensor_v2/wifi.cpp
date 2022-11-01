#include <ESP8266WiFi.h>

#define WIFI_SSID "<WIFI SSID HERE>"
#define WIFI_PASSWORD "<WIFI PASSWORD HERE>"

WiFiClient wifi_client;

void connectWiFi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
