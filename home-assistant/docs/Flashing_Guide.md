# Flashing Guide for ESP32-C3 Devices

To flash the ESP32-C3 devices with the configurations we created, you will use the **ESPHome Dashboard**. Since you are running ESPHome via Docker with `network_mode: host`, the dashboard is accessible on your network.

Here is the step-by-step process for the **first-time flash** (which must be done via USB cable).

## Prerequisites

1.  **Update Secrets**: Ensure you have updated `home-assistant/esphome/secrets.yaml` with your actual Wi-Fi SSID, Password, and generated keys. If you flash with the placeholders, the device won't connect to your network!
2.  **USB Cable**: You need a high-quality USB data cable (some cables are power-only and won't work).
3.  **Browser**: Use **Google Chrome** or **Microsoft Edge** on your computer. These browsers support *WebSerial*, which allows the website to talk directly to your USB device.

## Flashing Steps

1.  **Open the Dashboard**:
    *   Go to `http://localhost:6052` (if you are on the same machine) or `http://<IP-ADDRESS-OF-PI>:6052`.
    *   You should see three cards: `plant_1_irrigation`, `plant_2_irrigation`, and `plant_3_irrigation`.

2.  **Connect the Device**:
    *   Plug your **first ESP32-C3** into the USB port of the computer **where you are running the browser**.

3.  **Start Installation**:
    *   On the dashboard card for **Plant 1**, click **Install**.
    *   Select **Plug into this computer**.
    *   *Note: If this option is missing, ensure you are viewing the dashboard via `http` (not https) or that your browser supports WebSerial.*

4.  **Select Port**:
    *   A browser popup will appear asking you to select a serial port.
    *   Select the one that says "USB JTAG/serial debug unit" or similar (often `ttyUSB0` or `COMx`).
    *   Click **Connect**.

5.  **Flash**:
    *   The dashboard will compile the firmware (this might take a minute or two the first time).
    *   Once compiled, it will erase and flash the device.
    *   **Troubleshooting**: If it fails to initialize, you might need to put the ESP32-C3 into **Bootloader Mode**:
        1.  Hold down the **BOOT** button on the ESP32-C3.
        2.  Press and release the **RESET** (EN) button.
        3.  Release the **BOOT** button.
        4.  Try connecting again.

6.  **Repeat**:
    *   Unplug the first device.
    *   Plug in the second device.
    *   Repeat the steps for **Plant 2** and **Plant 3**.

## Future Updates (OTA)

Once the devices are flashed and successfully connect to your Wi-Fi, you **no longer need the USB cable**.

*   The devices will show as "Online" in the dashboard.
*   To update them (e.g., if you change the sleep duration), just click **Install** -> **Wirelessly**. ESPHome will upload the new firmware over Wi-Fi.

### Important Note on Deep Sleep

Since your devices are configured with `deep_sleep`, they will be offline most of the time.

*   **To update a sleeping device wirelessly**: You usually have to wait for it to wake up, or manually wake it up (press the RESET button) right before you click "Install" in the dashboard.
