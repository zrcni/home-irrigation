# ESP32-C3 Super Mini Connection Guide

This document outlines the recommended GPIO connections for the Plant Irrigator system when using the **ESP32-C3 Super Mini** board.

## Power Requirements

*   **Power Source**: The entire system (ESP32, Relay Module, and Valves) is powered by a single **5V 2A USB Charger**.
*   **Current Capacity**: It is critical to use a power supply capable of delivering at least **2 Amps**.
    *   **Reason**: The ESP32 itself consumes power, but the solenoid valves and the relay coils draw significant current when active. A standard 500mA or 1A USB port/charger is **insufficient** and may cause the ESP32 to brownout or reset when a valve opens.
*   **Sensor Power**: Soil moisture sensors are powered directly from the ESP32's GPIO pins (configured as outputs) to enable intermittent powering and prevent sensor corrosion.

### Estimated Current Draw
To understand why a 2A power supply is necessary, consider the following estimates:

*   **ESP32 (WiFi Active)**: ~150mA - 250mA
*   **Relay Module (per channel)**: ~70mA - 90mA
*   **Solenoid Valve (5V)**: ~300mA - 500mA (per valve)

**Scenario: Watering one plant**
*   Total: ~150mA (ESP) + 80mA (Relay) + 500mA (Valve) ≈ **730mA**

**Scenario: Watering three plants simultaneously**
*   Total: ~150mA (ESP) + 240mA (Relays) + 1500mA (Valves) ≈ **1.9A**

*Note: These are estimates. Actual current depends on the specific valve and relay models used.*

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
| Moisture Sensor Data | Analog Input | **GPIO 4** | `ADC1_CH4` | |
| Moisture Sensor Power | Digital Output | **GPIO 3** | - | |
| Water Valve Relay | Digital Output | **GPIO 21** | - | |

## Strapping Pins (Avoid if possible)
*   **GPIO 8**: Internal Pull-up, Strapping Pin (High at boot). Connected to onboard LED.
*   **GPIO 9**: Boot Button (Low for download mode).
*   **GPIO 2**: Strapping Pin (High at boot).

## ADC Notes
*   The ESP32-C3 uses `ADC_UNIT_1` for these channels.
*   Ensure the ADC attenuation settings in code match the voltage range of your sensors (usually 0-3.3V).

## Optional Components

### OLED Display (SSD1306/SH1106)
You can add a 0.96" or 1.3" I2C OLED display to view status information.

| OLED Pin | ESP32-C3 Pin | Notes |
| :--- | :--- | :--- |
| **GND** | GND | |
| **VCC** | 3.3V | 3.3V is recommended. |
| **SCL** | **GPIO 8** | Recommended (Check for conflicts with LED/Strapping) |
| **SDA** | **GPIO 9** | Recommended (Check for conflicts with Boot Button) |
*   *Note*: On the C3 Super Mini, GPIO 8 and 9 are often used for internal functions. If you encounter issues, you can reassign I2C to any other free pins (e.g., GPIO 2, 3) in the software.

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

### Flyback Diode Wiring (Crucial)
Connect a diode across the terminals of **each** valve to protect the system.

*   **Stripe (Cathode)**: Connect to the **Positive (+)** valve terminal (connected to Relay NO).
*   **No Stripe (Anode)**: Connect to the **Negative (-)** valve terminal (connected to GND).

```text
       +5V Power
           |
      [Relay NO Port]
           |
           |  <-- Wire to Valve Positive
           |
      +----o----+  <-- Connection Point A (Valve Positive)
      |         |
      |         |
    [VALVE]  [DIODE]  (Stripe/Cathode pointing UP towards Positive)
      |         |
      |         |
      +----o----+  <-- Connection Point B (Valve Negative)
           |
           |  <-- Wire to Valve Negative
           |
      Ground (GND)
```

### What happens?
1.  **Watering (Relay ON)**: +5V comes down. It sees the Valve and the Diode.
    *   It flows through the **Valve** (making it open).
    *   It is **blocked** by the Diode (because the stripe is facing the pressure), so no current flows through the diode.
2.  **Stop Watering (Relay OFF)**: The relay cuts the power.
    *   The magnetic coil inside the valve is "charged" and doesn't want to stop. It tries to force electricity to keep moving, creating a massive reverse voltage spike (like a kickback).
    *   Instead of shooting back up the wire to your ESP32, this "kickback" electricity takes the easy path: it loops through the **Diode** and back into the valve coil.
    *   The energy spins around in this little "Valve-Diode loop" for a split second until it dies out, protecting everything else.

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

