
Crowtail capacitive moisture sensor v2.0 (red)

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
