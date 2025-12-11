#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "wifi_app.h"
#include "mqtt_app.h"
#include "plant.h"

static const char *TAG = "main";

#define CHECK_INTERVAL_MS 1800000 // 30 minutes

// Configuration for 3 plants
static const plant_config_t plants[] = {
    {
        .plant_id = "plant_1",
        .sensor_power_pin = GPIO_NUM_25,
        .sensor_adc_channel = ADC_CHANNEL_6, // GPIO 34
        .pump_relay_pin = GPIO_NUM_26,
        .moisture_dry_threshold = 780,
        .pump_duration_ms = 2500
    },
    {
        .plant_id = "plant_2",
        .sensor_power_pin = GPIO_NUM_27,
        .sensor_adc_channel = ADC_CHANNEL_7, // GPIO 35
        .pump_relay_pin = GPIO_NUM_14,
        .moisture_dry_threshold = 780,
        .pump_duration_ms = 2500
    },
    {
        .plant_id = "plant_3",
        .sensor_power_pin = GPIO_NUM_12,
        .sensor_adc_channel = ADC_CHANNEL_4, // GPIO 32
        .pump_relay_pin = GPIO_NUM_13,
        .moisture_dry_threshold = 780,
        .pump_duration_ms = 2500
    }
};

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Start WiFi
    wifi_app_start();

    // Start MQTT
    mqtt_app_start();

    // Initialize Plant System
    adc_oneshot_unit_handle_t adc_handle;
    plant_system_init(&adc_handle);

    // Initialize Plants
    int num_plants = sizeof(plants) / sizeof(plants[0]);
    for (int i = 0; i < num_plants; i++) {
        plant_gpio_init(&plants[i], adc_handle);
    }

    ESP_LOGI(TAG, "System initialized. Starting loop...");

    while (1) {
        for (int i = 0; i < num_plants; i++) {
            plant_process(&plants[i], adc_handle);
            vTaskDelay(pdMS_TO_TICKS(1000)); // Small delay between plants
        }
        
        ESP_LOGI(TAG, "Sleeping for %d ms", CHECK_INTERVAL_MS);
        vTaskDelay(pdMS_TO_TICKS(CHECK_INTERVAL_MS));
    }
}
