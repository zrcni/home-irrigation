# Project Context & Instructions

## Firmware (`src/plant-irrigator`)
- **Scope**: Custom C firmware.
- **Environment**:
  - The user is using the ESP-IDF VS Code extension.
  - `idf.py` is NOT available in the terminal path.
  - Do not generate terminal commands using `idf.py`.
  - Use VS Code commands or explain how to use the extension for building, flashing, and monitoring.
- **Structure**:
  - ESP-IDF v5.5.1 project.
  - Uses SPIFFS for configuration (`config.json`).
  - Custom partition table (`custom_partitions.csv`).

## Home Assistant (`home-assistant/`)
- **Verification**: After making changes to the configuration, always verify that the entire configuration is up to date and consistent across all related YAML files (including main config, automations, scripts, dashboards, and ESPHome definitions).
