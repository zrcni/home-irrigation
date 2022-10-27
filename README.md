
sensor values:
air: ~850
water: ~530
wet soil: ~700

completely dry soil was pretty much same as air, duh

---

# connections

**pump relay**

COM -> power
NO -> pump 

**relay to arduino**

+ -> VCC
- -> GND
IN1 -> 10

**moisture sensor to arduino**

SIG (yellow) -> A0
NC (white) -> unconnected
VCC (red) -> 3
GND (black) -> GND

# links

https://github.com/jarzebski/Arduino-DS1307/blob/master/DS1307.cpp

https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html

https://wiki.dfrobot.com/FireBeetle_ESP8266_IOT_Microcontroller_SKU__DFR0489
