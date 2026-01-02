#pragma once

#include <stdbool.h>

typedef struct {
    char broker_ip[64];
    int port;
    char topic[64];
    char debug_topic[64];
    char client_id[64];
    char device_id[64];
} mqtt_config_t;

bool mqtt_app_start(const mqtt_config_t *config);
void mqtt_app_publish(const char *topic, const char *data);
void mqtt_app_publish_debug(const char *message);
bool mqtt_app_wait_all_published(int timeout_ms);

