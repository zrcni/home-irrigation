# Raspberry Pi Setup Guide

This guide explains how to set up a Raspberry Pi (specifically older models like RPi 2 running Raspbian Buster) to run the Home Assistant and ESPHome stack using Docker.

## 1. OS Preparation (Raspbian Buster / Legacy)

If you are running an older OS like Raspbian Buster (Debian 10), you may need to fix your `apt` sources because the release is End-of-Life.

1.  **Edit Sources List**:
    ```bash
    sudo nano /etc/apt/sources.list
    ```
    Change the main repository line to point to the legacy archive:
    ```text
    deb http://legacy.raspbian.org/raspbian/ buster main contrib non-free rpi
    ```

2.  **Update System**:
    ```bash
    sudo apt-get update
    sudo apt-get upgrade
    ```

## 2. Install Docker & Docker Compose

1.  **Install Docker**:
    ```bash
    sudo apt-get install docker.io
    ```

2.  **Install Docker Compose**:
    ```bash
    sudo apt-get install docker-compose
    ```

3.  **Configure Permissions**:
    Add your user (usually `pi`) to the docker group so you don't need `sudo` for every command.
    ```bash
    sudo usermod -aG docker $USER
    ```
    **Important**: You must **log out and log back in** (or reboot) for this to take effect.

4.  **Verify Installation**:
    ```bash
    docker --version
    docker-compose --version
    docker ps  # Should run without 'permission denied'
    ```

## 3. Setup Home Irrigation Repository

1.  **Clone the Repository**:
    ```bash
    git clone https://github.com/zrcni/home-irrigation.git
    cd home-irrigation/home-assistant
    ```

2.  **Configure Secrets**:
    Copy the example secrets file and edit it with your actual Wi-Fi credentials.
    ```bash
    cp esphome/secrets.yaml.example esphome/secrets.yaml
    nano esphome/secrets.yaml
    ```
    *   Set `wifi_ssid` and `wifi_password`.
    *   The encryption keys are pre-generated, but you can regenerate them if you wish.

3.  **Start the Services**:
    ```bash
    docker-compose up -d
    ```
    *Note: On older systems, use `docker-compose` (with hyphen). On newer systems, use `docker compose` (space).*

## 4. Accessing the System

Give the Pi some time to start the containers (especially on RPi 2).

*   **Home Assistant**: `http://<your-pi-ip>:8123`
*   **ESPHome Dashboard**: `http://<your-pi-ip>:6052`

## 5. Maintenance

*   **Update Containers**:
    ```bash
    cd ~/home-irrigation/home-assistant
    docker-compose pull
    docker-compose up -d
    ```
*   **View Logs**:
    ```bash
    docker-compose logs -f
    ```
