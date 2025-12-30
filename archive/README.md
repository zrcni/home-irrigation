
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

- power source's ground connects directly to pump's ground
- power source's positive goes through the relay
  - power source to COM, NO to pump

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

https://www.microbot.it/en/product/150/Real-Time-Clock-module-with-DS1307.html
https://www.microbot.it/sketches/DS1307.zip
https://www.microbot.it/documents/ds1307.pdf

https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html

https://wiki.dfrobot.com/FireBeetle_ESP8266_IOT_Microcontroller_SKU__DFR0489

# issues

including Wire.h in a sketch causes ESP8266 to crash?

This was a part of the error: `ETS JAN 8 2013,RST CAUSE:2, BOOT MODE:(3,6)`
