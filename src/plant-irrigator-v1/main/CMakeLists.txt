# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES nvs_flash esp_wifi esp_event mqtt driver esp_adc lwip)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "plant.c" "mqtt_app.c" "wifi_app.c" "main.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
