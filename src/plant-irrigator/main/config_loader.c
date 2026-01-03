#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "cJSON.h"
#include "config_loader.h"

static const char *TAG = "config_loader";

bool config_load(app_config_t *config)
{
    if (config == NULL) {
        return false;
    }
    
    // Initialize defaults / clear structure
    memset(config, 0, sizeof(app_config_t));

    ESP_LOGI(TAG, "Reading config file");
    FILE* f = fopen("/spiffs/config.json", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open config file");
        return false;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    if (string == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for config file");
        fclose(f);
        return false;
    }

    fread(string, 1, fsize, f);
    fclose(f);
    string[fsize] = 0;

    cJSON *json = cJSON_Parse(string);
    free(string);

    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            ESP_LOGE(TAG, "Error parsing JSON before: %s", error_ptr);
        }
        return false;
    }

    bool success = true;

    // WiFi
    cJSON *wifi = cJSON_GetObjectItemCaseSensitive(json, "wifi");
    cJSON *ssid = cJSON_GetObjectItemCaseSensitive(wifi, "ssid");
    cJSON *password = cJSON_GetObjectItemCaseSensitive(wifi, "password");

    if (!cJSON_IsString(ssid) || (ssid->valuestring == NULL) ||
        !cJSON_IsString(password) || (password->valuestring == NULL)) {
        ESP_LOGE(TAG, "Invalid WiFi config");
        success = false;
    } else {
        strlcpy(config->wifi.ssid, ssid->valuestring, sizeof(config->wifi.ssid));
        strlcpy(config->wifi.password, password->valuestring, sizeof(config->wifi.password));
    }

    // MQTT
    if (success) {
        cJSON *mqtt = cJSON_GetObjectItemCaseSensitive(json, "mqtt");
        cJSON *broker_ip = cJSON_GetObjectItemCaseSensitive(mqtt, "broker_ip");
        cJSON *port = cJSON_GetObjectItemCaseSensitive(mqtt, "port");
        cJSON *topic = cJSON_GetObjectItemCaseSensitive(mqtt, "topic");
        cJSON *debug_topic = cJSON_GetObjectItemCaseSensitive(mqtt, "debug_topic");
        cJSON *client_id = cJSON_GetObjectItemCaseSensitive(mqtt, "client_id");
        cJSON *device_id = cJSON_GetObjectItemCaseSensitive(mqtt, "device_id");

        if (!cJSON_IsString(broker_ip) || !cJSON_IsNumber(port) || !cJSON_IsString(topic)) {
            ESP_LOGE(TAG, "Invalid MQTT config");
            success = false;
        } else {
            strlcpy(config->mqtt.broker_ip, broker_ip->valuestring, sizeof(config->mqtt.broker_ip));
            config->mqtt.port = port->valueint;
            strlcpy(config->mqtt.topic, topic->valuestring, sizeof(config->mqtt.topic));
            
            if (cJSON_IsString(debug_topic)) strlcpy(config->mqtt.debug_topic, debug_topic->valuestring, sizeof(config->mqtt.debug_topic));
            if (cJSON_IsString(client_id)) strlcpy(config->mqtt.client_id, client_id->valuestring, sizeof(config->mqtt.client_id));
            if (cJSON_IsString(device_id)) strlcpy(config->mqtt.device_id, device_id->valuestring, sizeof(config->mqtt.device_id));
        }
    }

    // Plants
    if (success) {
        cJSON *plants_json = cJSON_GetObjectItemCaseSensitive(json, "plants");
        int num_plants = cJSON_GetArraySize(plants_json);
        
        if (num_plants > 0) {
            config->plants = malloc(num_plants * sizeof(plant_config_t));
            if (config->plants == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for plants");
                success = false;
            } else {
                config->num_plants = num_plants;
                int i = 0;
                cJSON *plant_item = NULL;
                cJSON_ArrayForEach(plant_item, plants_json) {
                    memset(&config->plants[i], 0, sizeof(plant_config_t));

                    cJSON *id = cJSON_GetObjectItemCaseSensitive(plant_item, "id");
                    cJSON *name = cJSON_GetObjectItemCaseSensitive(plant_item, "name");
                    cJSON *sensor_power_pin = cJSON_GetObjectItemCaseSensitive(plant_item, "sensor_power_pin");
                    cJSON *sensor_adc_channel = cJSON_GetObjectItemCaseSensitive(plant_item, "sensor_adc_channel");
                    cJSON *valve_pin = cJSON_GetObjectItemCaseSensitive(plant_item, "valve_pin");
                    cJSON *dry_threshold = cJSON_GetObjectItemCaseSensitive(plant_item, "dry_threshold");
                    cJSON *release_duration_ms = cJSON_GetObjectItemCaseSensitive(plant_item, "release_duration_ms");

                    if (cJSON_IsString(id)) config->plants[i].plant_id = strdup(id->valuestring);
                    if (cJSON_IsString(name)) config->plants[i].plant_name = strdup(name->valuestring);
                    if (cJSON_IsNumber(sensor_power_pin)) config->plants[i].sensor_power_pin = (gpio_num_t)sensor_power_pin->valueint;
                    if (cJSON_IsNumber(sensor_adc_channel)) config->plants[i].sensor_adc_channel = (adc_channel_t)sensor_adc_channel->valueint;
                    if (cJSON_IsNumber(valve_pin)) config->plants[i].valve_gpio_pin = (gpio_num_t)valve_pin->valueint;
                    if (cJSON_IsNumber(dry_threshold)) config->plants[i].moisture_dry_threshold = dry_threshold->valueint;
                    if (cJSON_IsNumber(release_duration_ms)) config->plants[i].release_duration_ms = release_duration_ms->valueint;
                    
                    i++;
                }
            }
        }
    }

    // System Settings
    if (success) {
        cJSON *deep_sleep = cJSON_GetObjectItemCaseSensitive(json, "deep_sleep_enabled");
        cJSON *sleep_duration = cJSON_GetObjectItemCaseSensitive(json, "sleep_duration_ms");

        if (cJSON_IsBool(deep_sleep)) {
            config->deep_sleep_enabled = cJSON_IsTrue(deep_sleep);
        } else {
            config->deep_sleep_enabled = true; // Default to true
        }

        if (cJSON_IsNumber(sleep_duration)) {
            config->sleep_duration_ms = sleep_duration->valueint;
        } else {
            config->sleep_duration_ms = 1800000; // Default 30 mins
        }
    }

    cJSON_Delete(json);
    
    if (!success) {
        config_free(config);
    }
    
    return success;
}

void config_free(app_config_t *config)
{
    if (config == NULL) return;

    if (config->plants != NULL) {
        for (int i = 0; i < config->num_plants; i++) {
            // Cast away const to free
            free((void*)config->plants[i].plant_id);
            free((void*)config->plants[i].plant_name);
        }
        free(config->plants);
        config->plants = NULL;
    }
    config->num_plants = 0;
}
