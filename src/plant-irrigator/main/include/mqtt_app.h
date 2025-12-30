#pragma once

typedef struct {
    char broker_ip[64];
    int port;
    char topic[64];
    char client_id[64];
    char device_id[64];
} mqtt_config_t;

void mqtt_app_start(const mqtt_config_t *config);
void mqtt_app_publish(const char *topic, const char *data);

