# Home Assistant & ESPHome Irrigation System

This folder contains the configuration for running Home Assistant and ESPHome for the irrigation system.

## Prerequisites

- Docker and Docker Compose installed.
- A Raspberry Pi (recommended) or a Linux machine.

## Setup

1.  **Secrets**:
    Copy `esphome/secrets.yaml.example` to `esphome/secrets.yaml`:
    ```bash
    cp esphome/secrets.yaml.example esphome/secrets.yaml
    ```
    Then edit `esphome/secrets.yaml` and set your Wi-Fi credentials and generate keys/passwords.

2.  **Start Services**:
    Run the following command in this directory:
    ```bash
    docker compose up -d
    ```

3.  **Access Home Assistant**:
    Open `http://<your-ip>:8123` (or `http://localhost:8123` if local).
    Follow the onboarding process.

4.  **Access ESPHome Dashboard**:
    Open `http://<your-ip>:6052` (or `http://localhost:6052` if local).
    - You should see the `irrigation-controller` device.
    - Click **Install** to flash your ESP32-C3.

## How it Works (Deep Sleep & Automation)

The system is designed to be energy efficient using Deep Sleep.

1.  **Wake Up**: The ESP32 wakes up every **15 minutes**.
2.  **Report**: It immediately reads the moisture sensors and sends the data to Home Assistant.
3.  **Trigger**: Home Assistant detects the new sensor data.
    *   If a plant is dry (below threshold), the `Trigger Sequential Watering Script` automation fires.
    *   This script turns on the valves sequentially.
    *   **Crucially**, opening a valve tells the ESP32 to **stay awake** (`deep_sleep.prevent`).
4.  **Sleep**:
    *   If no watering is needed, the ESP32 goes back to sleep after **60 seconds**.
    *   If watering occurs, the ESP32 sleeps immediately after the last valve closes.

### Performing Updates (OTA)
Because the device sleeps most of the time, you cannot simply push an update.
1.  Wait for the device to wake up (check Home Assistant).
2.  Toggle the **"Prevent Deep Sleep"** switch in the Dashboard.
3.  The device will now stay awake indefinitely.
4.  Perform your update via ESPHome.
5.  Toggle the switch **OFF** to resume the sleep schedule.

## Configuration

-   **Home Assistant Config**: Located in `config/`.
    -   `configuration.yaml`: Main config, defines inputs and locks.
    -   `scripts.yaml`: Contains the sequential watering logic.
    -   `automations.yaml`: Triggers the watering script when data arrives.
-   **ESPHome Config**: Located in `esphome/`.
    -   `irrigation.yaml`: Configuration for the ESP32-C3 controller.

## Notes on Networking

The `docker-compose.yml` is configured with `network_mode: host`. This is the recommended way to run Home Assistant on Linux/Raspberry Pi to ensure device discovery (mDNS) works correctly.

If you are running this on macOS or Windows, `network_mode: host` does not work as expected. You will need to edit `docker-compose.yml` to uncomment the `ports` sections and comment out `network_mode: host`.
