# Plant Irrigator Configuration

The Plant Irrigator uses a JSON configuration file stored in the device's flash memory (SPIFFS). This allows you to change WiFi credentials, MQTT settings, and Plant definitions without modifying the source code.

## Setup

1.  **Copy the Example**: Copy the example configuration file to the `data` directory.
    ```bash
    cp config.example.json data/config.json
    ```
    *Note: `data/config.json` is ignored by git, so your private credentials won't be committed.*

2.  **Edit Configuration**: Open `data/config.json` and update it with your specific settings (WiFi SSID, Password, MQTT Broker, etc.).

## Configuration File Location

The active configuration file is located at:
`src/plant-irrigator/data/config.json`

## Configuration Structure

The `config.json` file has three main sections: `wifi`, `mqtt`, and `plants`.

### Example `config.json`

```json
{
    "wifi": {
        "ssid": "YOUR_WIFI_SSID",
        "password": "YOUR_WIFI_PASSWORD"
    },
    "mqtt": {
        "broker_ip": "192.168.1.100",
        "port": 1883,
        "topic": "sensors/plant_irrigator",
        "client_id": "esp32_client",
        "device_id": "plant_irrigator_01"
    },
    "plants": [
        {
            "id": "plant_1",
            "sensor_power_pin": 25,
            "sensor_adc_channel": 6,
            "valve_pin": 26,
            "dry_threshold": 2500,
            "release_duration_ms": 2500
        }
    ]
}
```

## Fields Description

### WiFi Settings
*   **ssid**: The name of your WiFi network.
*   **password**: The password for your WiFi network.

### MQTT Settings
*   **broker_ip**: The IP address of your MQTT broker (e.g., Raspberry Pi running Mosquitto).
*   **port**: The MQTT broker port (default is usually 1883).
*   **topic**: The MQTT topic where sensor data and events will be published.
*   **client_id**: A unique ID for the MQTT client connection.
*   **device_id**: A unique ID for the physical device, included in the JSON payload.

### Plant Settings
You can define multiple plants in the `plants` array. Each plant object requires:
*   **id**: A unique string identifier for the plant (e.g., "basil", "fern").
*   **sensor_power_pin**: The GPIO pin connected to the soil moisture sensor's VCC (for powering it only during reading).
*   **sensor_adc_channel**: The ADC channel corresponding to the sensor's analog output pin.
    *   *Note: Check ESP32 documentation to map GPIO pins to ADC channels (e.g., GPIO 34 is ADC1_CH6).*
*   **valve_pin**: The GPIO pin connected to the relay/transistor controlling the water pump/valve.
*   **dry_threshold**: The analog value (0-4095) above which the soil is considered "dry". Higher values usually mean drier soil (depending on sensor).
*   **release_duration_ms**: How long (in milliseconds) to open the valve when watering is triggered.

## Applying Changes

1.  **Edit the File**: Open `src/plant-irrigator/data/config.json` in your editor and make your changes.
2.  **Flash the Device**: Run the flash command. The build system is configured to automatically pack the `data` folder into a filesystem image and flash it along with the application.

    ```bash
    idf.py flash
    ```

    If you only want to update the configuration without recompiling the application code, you can try flashing just the storage partition (command may vary based on ESP-IDF version, but usually involves `partition_table` targets or `spiffs` targets if defined, otherwise `idf.py flash` is the safest bet).
