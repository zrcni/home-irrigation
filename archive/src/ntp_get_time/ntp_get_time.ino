#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

const char *ssid = "<WIFI SSID>";
const char *password = "<WIFI PASSWORD>";

WiFiUDP ntpUDP;

// Set update interval to 0. That is handled by TimeLib.
NTPClient timeClient(ntpUDP, "fi.pool.ntp.org", 0, 0);

void setup()  {
  Serial.begin(115200);
  while (!Serial);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  
  timeClient.begin();

  setSyncProvider(getNTPTime);

  if(timeStatus()!= timeSet) 
     Serial.println("Unable to sync with NTP");
  else
     Serial.println("System time has been set based on NTP response");      
}

void loop()
{
  if (timeStatus() == timeSet) {
    digitalClockDisplay();
  } else {
    Serial.println("The time has not been set.  Please run the Time");
    Serial.println("TimeRTCSet example, or DS1307RTC SetTime example.");
    Serial.println();
    delay(4000);
  }
  delay(1000);
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
  Serial.println(now());
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


time_t getNTPTime() {
  timeClient.update();
  int seconds = timeClient.getEpochTime();
  return (time_t)seconds;
}
