#include <Wire.h>
#include <DS1307.h>

DS1307 clock;
RTCDateTime dt;

void setup()
{
  Serial.begin(9600);

  // Initialize DS1307
  Serial.println("Initialize DS1307");

  clock.begin();

  // If date not set
  if (!clock.isReady())
  {
    // Set sketch compiling time
    clock.setDateTime(__DATE__, __TIME__);
  }


  // set custom date
  // clock.setDateTime(22, 10, 27, 20, 55, 30);
}

void loop()
{
  dt = clock.getDateTime();

  // For leading zero look to DS1307_dateformat example

  Serial.print("Raw data: ");
  Serial.print(dt.year);
  Serial.print("-");
  Serial.print(dt.month);
  Serial.print("-");
  Serial.print(dt.day);
  Serial.print(" ");
  Serial.print(dt.hour);
  Serial.print(":");
  Serial.print(dt.minute);
  Serial.print(":");
  Serial.print(dt.second);
  Serial.println("");

  delay(1000);
}
