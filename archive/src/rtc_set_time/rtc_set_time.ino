#include <ds1307.h>
#include <Wire.h>

ds1307 DS1307;

uint8_t buffer[7]; // receiving buffer

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    Serial.println("Real Time Clock module with DS1307 test sketch");

    DS1307.resume();   // turn on the internal clock

    DS1307.setYear(22);
    DS1307.setMonth(10);
    DS1307.setDate(29);
    DS1307.setDow(6);
    DS1307.setHours(14);
    DS1307.setMinutes(48);
    DS1307.setSeconds(20);
}

void loop()
{
    DS1307.getDate(buffer);

    Serial.print("Date(dd:mm:yy) ");
    Serial.print(buffer[4], DEC);    // day
    Serial.print(':');
    Serial.print(buffer[5], DEC);    // month
    Serial.print(':');
    Serial.print(buffer[6], DEC);    // year
    Serial.print(':');
    Serial.print(" Time(hh:mm:ss) ");
    Serial.print(buffer[2], DEC);    // hours
    Serial.print(':');
    Serial.print(buffer[1], DEC);    // minutes
    Serial.print(':');
    Serial.println(buffer[0], DEC);  // seconds

    delay(1000);
}