# ESP-WROOM-32 Wiring Diagram

This diagram illustrates the complete wiring for the 3-plant irrigation system, including power distribution, relay connections, flyback diode protection, and sensor connections.

```mermaid
graph TD
    %% --- Power Supply ---
    subgraph Power_Supply [Power Source]
        direction TB
        USB[USB Power Source 5V 2A]
        USB_VCC((+5V Red Wire))
        USB_GND((GND Black Wire))
        CAP[Capacitor 1000uF]

        USB --> USB_VCC
        USB --> USB_GND
        USB_VCC -- Parallel --> CAP
        USB_GND -- Parallel --> CAP
    end

    %% --- Main Controller ---
    subgraph Control_Unit [Control Unit]
        direction TB
        ESP[ESP32 Board]
        RELAY[4-Channel Relay Module]
        
        %% Powering Controller
        USB_VCC -->|VIN / 5V| ESP
        USB_GND -->|GND| ESP
        USB_VCC -->|VCC| RELAY
        USB_GND -->|GND| RELAY
        
        %% Control Signals
        ESP -->|GPIO 26| R1_IN[Relay IN1]
        ESP -->|GPIO 14| R2_IN[Relay IN2]
        ESP -->|GPIO 13| R3_IN[Relay IN3]
    end

    %% --- PLANT 1 ---
    subgraph Plant_1 [Plant 1]
        direction TB
        R1_COM[Relay 1 COM]
        R1_NO[Relay 1 NO]
        VALVE1[Valve 1]
        DIODE1[Diode 1]
        SENS1[Moisture Sensor 1]
        
        %% Watering Circuit
        USB_VCC --> R1_COM
        R1_IN -.-> R1_COM
        R1_COM -. Switch .- R1_NO
        R1_NO -->|Positive| VALVE1
        R1_NO -->|Cathode Stripe| DIODE1
        VALVE1 -->|Negative| USB_GND
        DIODE1 -->|Anode| USB_GND

        %% Sensing Circuit
        ESP -->|GPIO 25 Power| SENS1
        SENS1 -->|GPIO 34 Data| ESP
        SENS1 -->|GND| USB_GND
    end

    %% --- PLANT 2 ---
    subgraph Plant_2 [Plant 2]
        direction TB
        R2_COM[Relay 2 COM]
        R2_NO[Relay 2 NO]
        VALVE2[Valve 2]
        DIODE2[Diode 2]
        SENS2[Moisture Sensor 2]
        
        %% Watering Circuit
        USB_VCC --> R2_COM
        R2_IN -.-> R2_COM
        R2_COM -. Switch .- R2_NO
        R2_NO -->|Positive| VALVE2
        R2_NO -->|Cathode Stripe| DIODE2
        VALVE2 -->|Negative| USB_GND
        DIODE2 -->|Anode| USB_GND

        %% Sensing Circuit
        ESP -->|GPIO 27 Power| SENS2
        SENS2 -->|GPIO 35 Data| ESP
        SENS2 -->|GND| USB_GND
    end

    %% --- PLANT 3 ---
    subgraph Plant_3 [Plant 3]
        direction TB
        R3_COM[Relay 3 COM]
        R3_NO[Relay 3 NO]
        VALVE3[Valve 3]
        DIODE3[Diode 3]
        SENS3[Moisture Sensor 3]
        
        %% Watering Circuit
        USB_VCC --> R3_COM
        R3_IN -.-> R3_COM
        R3_COM -. Switch .- R3_NO
        R3_NO -->|Positive| VALVE3
        R3_NO -->|Cathode Stripe| DIODE3
        VALVE3 -->|Negative| USB_GND
        DIODE3 -->|Anode| USB_GND

        %% Sensing Circuit
        ESP -->|GPIO 12 Power| SENS3
        SENS3 -->|GPIO 32 Data| ESP
        SENS3 -->|GND| USB_GND
    end
```
