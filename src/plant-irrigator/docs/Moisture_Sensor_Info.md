# Moisture Sensor Information

## Overview
This document provides information about the moisture sensors used in the project, including interpreting ADC values, setting thresholds, and calibration.

## ADC Values & Thresholds (12-bit)
In 2026, for a 12-bit ADC system (where values range from 0 to 4095), a good AD value to trigger watering is typically between **3200 and 3800**, depending on your specific plant's needs and soil type.

Because high values indicate "dry" on most capacitive sensors, you should set your watering threshold just below the "bone-dry" air value of ~4000.

### Recommended Thresholds
| Soil Condition | AD Value Range | Action Needed |
| :--- | :--- | :--- |
| **Saturated / In Water** | 1000 – 1500 | Do not water; check for drainage. |
| **Adequately Moist** | 1600 – 2800 | Ideal for most plants. |
| **Drying Out** | 2900 – 3300 | Monitor closely; good for drought-tolerant plants. |
| **Dry / Needs Water** | 3400 – 3800 | Begin watering. |
| **Bone Dry / Air** | 3900 – 4095 | Sensor is likely not in soil or is completely dry. |

## Calibration: How to Find Your Exact "Water Me" Value
Since every sensor and soil type is different, use these steps to calibrate your specific setup:

1.  **Air Reading**: Hold the sensor in the air. This is your "maximum dry" value (e.g., 4050).
2.  **Saturation Reading**: Place the sensor in a cup of water (up to the white line). This is your "maximum wet" value (e.g., 1200).
3.  **Soil Test**: Stick the sensor into soil that you feel just needs watering. Record that number. For most vegetables, this is often around 80–85% of your total dry-to-wet range.
4.  **Set the Threshold**: Use that recorded "Soil Test" number as your software's trigger to turn on a pump or send a notification.

## Factors That Change These Numbers
*   **Soil Type**: Clay holds onto water more tightly than sand; you may need to water clay at a "wetter" AD value (e.g., 3200) to ensure the plant can actually extract it.
*   **Plant Type**: Succulents may not need water until the value hits 3800, while leafy tropicals might need it as soon as the value reaches 3000.
*   **Supply Voltage**: Ensure your sensor is powered by a stable 3.3V or 5V source. If the voltage fluctuates, your "dry" threshold of 3800 will drift, leading to over or under-watering.

## Firmware Implementation Details
The `plant-irrigator` firmware implements specific logic for handling moisture sensors:

*   **Sensor Power Control**: To prevent corrosion, the sensor is not powered continuously. The firmware powers on the sensor (via `sensor_power_pin`), waits 100ms, takes a reading, and then powers it off.
*   **Safety Checks**: The firmware currently treats readings **> 3800** as "Invalid" or "Air".
    *   If the reading is > 3800, the system logs a warning ("Invalid sensor reading") and **skips watering**. This prevents the pump from running if the sensor is pulled out of the soil.
*   **Configuration**: Thresholds are configured per plant in `config.json` using the `moisture_dry_threshold` parameter.

## Configuration Note
The `config.example.json` file may use a default `dry_threshold` (e.g., 2500) that differs from the "Dry / Needs Water" range (3400-3800) suggested above. 
*   **2500**: Keeps the soil "Adequately Moist".
*   **3400+**: Allows the soil to dry out more before watering.

Adjust the `dry_threshold` in your `config.json` according to the specific needs of your plants and the calibration of your sensors.

## ESPHome Configuration & Calibration (2026)

For the ESPHome-based firmware (`irrigation-controller.yaml`), we use a linear calibration to convert the raw voltage output of the sensor into a percentage (0-100%).

### Calibration Values (Jan 3, 2026)
These values were measured using the specific Capacitive Soil Moisture Sensors v2.0 installed in the system.

*   **Dry (Air)**: `2.34 V` -> **0%**
*   **Wet (Submerged)**: `0.96 V` -> **100%**

### YAML Configuration
The following filter configuration is used in `irrigation-controller.yaml` to map these values:

```yaml
filters:
  - calibrate_linear:
      - 2.34 -> 0.0   # Dry (Air)
      - 0.96 -> 100.0 # Wet (Water)
  # Clamp values to 0-100% range to avoid negative numbers or >100%
  - lambda: "return max(0.0f, min(100.0f, x));"
```

### Hardware Note
*   **Power**: Sensors are powered via GPIO pins (Active High) only during reading to prevent corrosion.
*   **Voltage Range**: The sensors operate on 3.3V logic but output a max of ~3.0V (Air) down to ~1.0V (Water) depending on the specific unit and cable length.

