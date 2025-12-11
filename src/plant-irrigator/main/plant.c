#include "plant.h"
#include "mqtt_app.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>
#include <stdio.h>
#include "sdkconfig.h"

static const char *TAG = "plant";

void plant_system_init(adc_oneshot_unit_handle_t *adc_handle)
{
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, adc_handle));
}

void plant_gpio_init(const plant_config_t* plant, adc_oneshot_unit_handle_t adc_handle)
{
    // Init Sensor Power Pin
    gpio_reset_pin(plant->sensor_power_pin);
    gpio_set_direction(plant->sensor_power_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(plant->sensor_power_pin, 0);

    // Init Pump Relay Pin
    gpio_reset_pin(plant->pump_relay_pin);
    gpio_set_direction(plant->pump_relay_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(plant->pump_relay_pin, 0); // Assume 0 is OFF (Open)

    // Init ADC Channel
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, plant->sensor_adc_channel, &config));
}

void plant_process(const plant_config_t* plant, adc_oneshot_unit_handle_t adc_handle)
{
    // 1. Turn on Sensor
    gpio_set_level(plant->sensor_power_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    // 2. Read Sensor
    int adc_raw;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, plant->sensor_adc_channel, &adc_raw));
    ESP_LOGI(TAG, "Plant %s: Moisture %d", plant->plant_id, adc_raw);

    // 3. Turn off Sensor
    gpio_set_level(plant->sensor_power_pin, 0);

    // 4. Publish Sensor Data
    char payload[256];
    time_t now;
    time(&now);
    
    snprintf(payload, sizeof(payload), 
        "{\"timestamp\":%d,\"moisture\":%d,\"water_duration\":%d,\"device_id\":\"%s\",\"client_id\":\"%s\",\"plant_id\":\"%s\"}",
        (int)now, adc_raw, 0, CONFIG_MQTT_DEVICE_ID, CONFIG_MQTT_CLIENT_ID, plant->plant_id);
    
    mqtt_app_publish(CONFIG_MQTT_PUBLISH_TOPIC, payload);

    // 5. Check Threshold
    if (adc_raw > plant->moisture_dry_threshold) {
        ESP_LOGI(TAG, "Plant %s is DRY. Watering...", plant->plant_id);
        
        // Turn on Pump
        gpio_set_level(plant->pump_relay_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(plant->pump_duration_ms));
        gpio_set_level(plant->pump_relay_pin, 0);

        // Publish Irrigation Event
        snprintf(payload, sizeof(payload), 
            "{\"timestamp\":%d,\"moisture\":%d,\"water_duration\":%d,\"device_id\":\"%s\",\"client_id\":\"%s\",\"plant_id\":\"%s\"}",
            (int)now, adc_raw, plant->pump_duration_ms, CONFIG_MQTT_DEVICE_ID, CONFIG_MQTT_CLIENT_ID, plant->plant_id);
        
        mqtt_app_publish(CONFIG_MQTT_PUBLISH_TOPIC, payload);
    } else {
        ESP_LOGI(TAG, "Plant %s is WET.", plant->plant_id);
    }
}
