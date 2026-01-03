# Project Context & Instructions

## Environment
- The user is using the ESP-IDF VS Code extension.
- `idf.py` is NOT available in the terminal path.
- Do not generate terminal commands using `idf.py`.
- Use VS Code commands or explain how to use the extension for building, flashing, and monitoring.

## Project Structure
- ESP-IDF v5.5.1 project.
- Uses SPIFFS for configuration (`config.json`).
- Custom partition table `custom_partitions.csv`.

## Configuration Management
- `config.example.json` is the template for the actual configuration file `data/config.json`.
- When modifying the configuration structure or adding new fields to `data/config.json`, ALWAYS update `config.example.json` to reflect these changes.

