# Home plant irrigation system

This system automates watering for multiple plants using an **ESP32** microcontroller. It connects via **Wi-Fi** to publish status updates to a local **MQTT** broker.

Water is gravity-fed from a container placed high on a shelf through a thin hose.

Each plant is equipped with a **moisture sensor**, a normally-closed (NC) **solenoid valve** to prevent water flow when idle. T-connectors split the main water line to serve each plant.

When a plant requires watering, the system activates the corresponding **relay** to open the solenoid valve, which allows water to flow to the plant.

# Home Assistant

home-assistant folder has an alternative setup for the irrigation system that uses ESPHome to integrate it with Home Assistant, where irrigation logic is implemented by using HA automations.
