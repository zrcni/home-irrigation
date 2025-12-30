#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "cJSON.h"

#include "wifi_app.h"
#include "mqtt_app.h"
#include "plant.h"

static const char *TAG = "main";

#define CHECK_INTERVAL_MS 1800000 // 30 minutes

static plant_config_t *plants = NULL;
static int num_plants = 0;
static mqtt_config_t mqtt_cfg = {0};

static esp_err_t init_spiffs(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = "storage",
      .max_files = 5,
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ESP_OK;
}

static void load_config(void)
{
    ESP_LOGI(TAG, "Reading config file");
    FILE* f = fopen("/spiffs/config.json", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open config file");
        return;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);
    string[fsize] = 0;

    cJSON *json = cJSON_Parse(string);
    free(string);

    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            ESP_LOGE(TAG, "Error before: %s", error_ptr);
        }
        return;
    }

    // WiFi
    cJSON *wifi = cJSON_GetObjectItemCaseSensitive(json, "wifi");
    cJSON *ssid = cJSON_GetObjectItemCaseSensitive(wifi, "ssid");
    cJSON *password = cJSON_GetObjectItemCaseSensitive(wifi, "password");

    if (cJSON_IsString(ssid) && (ssid->valuestring != NULL) &&
        cJSON_IsString(password) && (password->valuestring != NULL)) {
        wifi_app_start(ssid->valuestring, password->valuestring);
    }

    // MQTT
    cJSON *mqtt = cJSON_GetObjectItemCaseSensitive(json, "mqtt");
    cJSON *broker_ip = cJSON_GetObjectItemCaseSensitive(mqtt, "broker_ip");
    cJSON *port = cJSON_GetObjectItemCaseSensitive(mqtt, "port");
    cJSON *topic = cJSON_GetObjectItemCaseSensitive(mqtt, "topic");
    cJSON *client_id = cJSON_GetObjectItemCaseSensitive(mqtt, "client_id");
    cJSON *device_id = cJSON_GetObjectItemCaseSensitive(mqtt, "device_id");

    if (cJSON_IsString(broker_ip)) strlcpy(mqtt_cfg.broker_ip, broker_ip->valuestring, sizeof(mqtt_cfg.broker_ip));
    if (cJSON_IsNumber(port)) mqtt_cfg.port = port->valueint;
    if (cJSON_IsString(topic)) strlcpy(mqtt_cfg.topic, topic->valuestring, sizeof(mqtt_cfg.topic));
    if (cJSON_IsString(client_id)) strlcpy(mqtt_cfg.client_id, client_id->valuestring, sizeof(mqtt_cfg.client_id));
    if (cJSON_IsString(device_id)) strlcpy(mqtt_cfg.device_id, device_id->valuestring, sizeof(mqtt_cfg.device_id));

    mqtt_app_start(&mqtt_cfg);

    // Plants
    cJSON *plants_json = cJSON_GetObjectItemCaseSensitive(json, "plants");
    num_plants = cJSON_GetArraySize(plants_json);
    if (num_plants > 0) {
        plants = malloc(num_plants * sizeof(plant_config_t));
        int i = 0;
        cJSON *plant_item = NULL;
        cJSON_ArrayForEach(plant_item, plants_json) {
            cJSON *id = cJSON_GetObjectItemCaseSensitive(plant_item, "id");
            cJSON *sensor_power_pin = cJSON_GetObjectItemCaseSensitive(plant_item, "sensor_power_pin");
            cJSON *sensor_adc_channel = cJSON_GetObjectItemCaseSensitive(plant_item, "sensor_adc_channel");
            cJSON *valve_pin = cJSON_GetObjectItemCaseSensitive(plant_item, "valve_pin");
            cJSON *dry_threshold = cJSON_GetObjectItemCaseSensitive(plant_item, "dry_threshold");
            cJSON *release_duration_ms = cJSON_GetObjectItemCaseSensitive(plant_item, "release_duration_ms");

            if (cJSON_IsString(id)) plants[i].plant_id = strdup(id->valuestring);
            if (cJSON_IsNumber(sensor_power_pin)) plants[i].sensor_power_pin = (gpio_num_t)sensor_power_pin->valueint;
            if (cJSON_IsNumber(sensor_adc_channel)) plants[i].sensor_adc_channel = (adc_channel_t)sensor_adc_channel->valueint;
            if (cJSON_IsNumber(valve_pin)) plants[i].valve_gpio_pin = (gpio_num_t)valve_pin->valueint;
            if (cJSON_IsNumber(dry_threshold)) plants[i].moisture_dry_threshold = dry_threshold->valueint;
            if (cJSON_IsNumber(release_duration_ms)) plants[i].release_duration_ms = release_duration_ms->valueint;
            
            i++;
        }
    }

    cJSON_Delete(json);
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize SPIFFS
    ESP_ERROR_CHECK(init_spiffs());

    // Load Config and Start Services
    load_config();

    // Initialize Plant System
    adc_oneshot_unit_handle_t adc_handle;
    plant_system_init(&adc_handle);

    // Initialize Plants
    for (int i = 0; i < num_plants; i++) {
        plant_gpio_init(&plants[i], adc_handle);
    }

    ESP_LOGI(TAG, "System initialized. Starting loop...");

    while (1) {
        for (int i = 0; i < num_plants; i++) {
            plant_process(&plants[i], adc_handle, mqtt_cfg.topic, mqtt_cfg.device_id, mqtt_cfg.client_id);
            vTaskDelay(pdMS_TO_TICKS(1000)); // Small delay between plants
        }
        
        ESP_LOGI(TAG, "Sleeping for %d ms", CHECK_INTERVAL_MS);
        vTaskDelay(pdMS_TO_TICKS(CHECK_INTERVAL_MS));
    }
}
