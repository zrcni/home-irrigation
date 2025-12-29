# ESP32-C3 Super Mini Connection Guide

This document outlines the recommended GPIO connections for the Plant Irrigator system when using the **ESP32-C3 Super Mini** board.

## Pinout Considerations
The ESP32-C3 Super Mini has a limited number of exposed pins. The following pins are generally safe to use, avoiding strapping pins (GPIO 2, 8, 9) which can affect boot modes.

## Plant Connections

| Component | Function | ESP32-C3 Pin | ADC Channel | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **Plant 1** | | | | |
| Moisture Sensor Data | Analog Input | **GPIO 0** | `ADC1_CH0` | |
| Moisture Sensor Power | Digital Output | **GPIO 5** | - | |
| Water Valve Relay | Digital Output | **GPIO 6** | - | Active Low/High depends on relay |
| **Plant 2** | | | | |
| Moisture Sensor Data | Analog Input | **GPIO 1** | `ADC1_CH1` | |
| Moisture Sensor Power | Digital Output | **GPIO 7** | - | |
| Water Valve Relay | Digital Output | **GPIO 10** | - | |
| **Plant 3** | | | | |
| Moisture Sensor Data | Analog Input | **GPIO 3** | `ADC1_CH3` | |
| Moisture Sensor Power | Digital Output | **GPIO 4** | - | |
| Water Valve Relay | Digital Output | **GPIO 21** | - | |

## Strapping Pins (Avoid if possible)
*   **GPIO 8**: Internal Pull-up, Strapping Pin (High at boot). Connected to onboard LED.
*   **GPIO 9**: Boot Button (Low for download mode).
*   **GPIO 2**: Strapping Pin (High at boot).

## ADC Notes
*   The ESP32-C3 uses `ADC_UNIT_1` for these channels.
*   Ensure the ADC attenuation settings in code match the voltage range of your sensors (usually 0-3.3V).

## Relay Module & Valve Wiring
The system uses a 4-Channel Relay Module to control the water valves.

### Relay Channel Assignment
*   **Plant 1**: Relay Channel 1
*   **Plant 2**: Relay Channel 2
*   **Plant 3**: Relay Channel 3

### Wiring Connections
For each plant's valve, wire the relay channel as follows:

1.  **COM (Common) Port**: Connect to the **+5V Power Source**.
2.  **NO (Normally Open) Port**: Connect to the **Valve's Positive (+) Wire**.
3.  **Valve's Negative (-) Wire**: Connect to **Ground (GND)**.

### Operation Logic
*   **Idle State**: The relay is open (NC connected to COM, NO disconnected). The valve is unpowered and closed.
*   **Watering State**: The ESP32 activates the relay channel. The relay switches, connecting **COM (+5V)** to **NO**. This powers the valve, opening it to let water flow.

## Power Supply Requirements
*   **Voltage**: 5V DC.
*   **Current**: Minimum **1A** (1000mA), recommended **2A** (2000mA).
*   **Cable**: Use a high-quality **USB Charging Cable** (24AWG or thicker power wires).
    *   **Avoid**: Mouse/Keyboard cables or thin LED power cables. These often have thin wires (28AWG) that cause voltage drops, leading to ESP32 reboots when valves open.
*   **Protection**:
    *   **Flyback Diode (Required)**: Connect a diode (e.g., 1N4007) across each valve's terminals (Cathode/Stripe to Positive) to prevent voltage spikes from destroying the ESP32.
    *   **Capacitor (Recommended)**: Add a capacitor (e.g., 470µF) across the 5V/GND rails to stabilize voltage during valve activation.
        *   *Tip*: If you only have smaller capacitors (e.g., 100µF), you can connect multiple in **parallel** (Positive to Positive, Negative to Negative) to add up their capacity (e.g., 3 x 100µF = 300µF).
        *   *Note*: A larger capacitor (e.g., **2200µF**) is excellent and provides even better stability.
        *   *Placement*: Connect the capacitor **as close to the ESP32 as possible** (in parallel with the power wires) for maximum effectiveness.

