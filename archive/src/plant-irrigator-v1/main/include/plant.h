#pragma once

#include <stdint.h>
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

typedef struct {
    const char* plant_id;
    gpio_num_t sensor_power_pin;
    adc_channel_t sensor_adc_channel;
    gpio_num_t valve_gpio_pin;
    int moisture_dry_threshold;
    int release_duration_ms;
} plant_config_t;

// Initialize the plant system (ADC, etc)
void plant_system_init(adc_oneshot_unit_handle_t *adc_handle);

// Initialize a specific plant's GPIOs and ADC channel
void plant_gpio_init(const plant_config_t* plant, adc_oneshot_unit_handle_t adc_handle);

// Process a single plant (read sensor, check threshold, water if needed)
void plant_process(const plant_config_t* plant, adc_oneshot_unit_handle_t adc_handle);
