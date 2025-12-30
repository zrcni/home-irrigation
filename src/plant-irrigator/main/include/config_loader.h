#pragma once

#include <stdbool.h>
#include "mqtt_app.h"
#include "plant.h"

typedef struct {
    char ssid[33];
    char password[65];
} app_wifi_config_t;

typedef struct {
    app_wifi_config_t wifi;
    mqtt_config_t mqtt;
    plant_config_t *plants;
    int num_plants;
} app_config_t;

/**
 * @brief Load configuration from /spiffs/config.json
 * 
 * @param config Pointer to app_config_t structure to populate
 * @return true if configuration was loaded and validated successfully
 * @return false if configuration failed to load or validate
 */
bool config_load(app_config_t *config);

/**
 * @brief Free resources allocated during config loading (e.g. plant array)
 * 
 * @param config Pointer to app_config_t structure
 */
void config_free(app_config_t *config);
