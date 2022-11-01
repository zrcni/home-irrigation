void connectMQTT();

void disconnectMQTT();

void publishMessage(char *payload);

char* buildMessagePayload(uint32_t timestamp, uint16_t moisture_value, uint16_t water_release_duration);
