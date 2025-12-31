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
#include "config_loader.h"

static const char *TAG = "main";

#define CHECK_INTERVAL_MS 1800000 // 30 minutes

static app_config_t app_config;

static esp_err_t init_spiffs(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .max_files = 5,
        .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ESP_OK;
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize SPIFFS
    ESP_ERROR_CHECK(init_spiffs());

    // Load Config
    if (!config_load(&app_config))
    {
        ESP_LOGE(TAG, "Failed to load configuration. Halting.");
        while (1)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    // Start Services
    wifi_app_start(app_config.wifi.ssid, app_config.wifi.password);
    mqtt_app_start(&app_config.mqtt);

    // Publish Initialization Message
    char payload[256];
    snprintf(payload, sizeof(payload),
             "{\"event\":\"system_ready\",\"device_id\":\"%s\",\"client_id\":\"%s\"}",
             app_config.mqtt.device_id, app_config.mqtt.client_id);
    mqtt_app_publish(app_config.mqtt.topic, payload);

    // Initialize Plant System
    adc_oneshot_unit_handle_t adc_handle;
    plant_system_init(&adc_handle);

    // Initialize Plants
    for (int i = 0; i < app_config.num_plants; i++)
    {
        plant_gpio_init(&app_config.plants[i], adc_handle);
    }

    ESP_LOGI(TAG, "System initialized. Starting loop...");

    // Publish Initialization Message
    char payload[256];
    snprintf(payload, sizeof(payload),
        "{\"event\":\"system_initialized\",\"device_id\":\"%s\",\"client_id\":\"%s\"}",
        app_config.mqtt.device_id, app_config.mqtt.client_id);
    mqtt_app_publish(app_config.mqtt.topic, payload);

    while (1)
    {
        for (int i = 0; i < app_config.num_plants; i++)
        {
            plant_process(&app_config.plants[i], adc_handle, app_config.mqtt.topic, app_config.mqtt.device_id, app_config.mqtt.client_id);
            vTaskDelay(pdMS_TO_TICKS(1000)); // Small delay between plants
        }

        ESP_LOGI(TAG, "Sleeping for %d ms", CHECK_INTERVAL_MS);
        vTaskDelay(pdMS_TO_TICKS(CHECK_INTERVAL_MS));
    }
}
