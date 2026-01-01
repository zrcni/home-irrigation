# Home Assistant & ESPHome Irrigation System

This folder contains the configuration for running Home Assistant and ESPHome for the irrigation system.

## Prerequisites

- Docker and Docker Compose installed.
- A Raspberry Pi (recommended) or a Linux machine.

## Setup

1.  **Secrets**:
    Edit `esphome/secrets.yaml` and set your Wi-Fi credentials and generate keys/passwords.

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

## Configuration

-   **Home Assistant Config**: Located in `config/`.
    -   `configuration.yaml`: Main config, defines inputs and locks.
    -   `scripts.yaml`: Contains the sequential watering logic.
    -   `automations.yaml`: Triggers the watering script.
-   **ESPHome Config**: Located in `esphome/`.
    -   `plant_a.yaml`: Configuration for the first plant device.

## Notes on Networking

The `docker-compose.yml` is configured with `network_mode: host`. This is the recommended way to run Home Assistant on Linux/Raspberry Pi to ensure device discovery (mDNS) works correctly.

If you are running this on macOS or Windows, `network_mode: host` does not work as expected. You will need to edit `docker-compose.yml` to uncomment the `ports` sections and comment out `network_mode: host`.
