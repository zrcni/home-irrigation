#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

WiFiUDP ntpUDP;

// Set update interval to 0. That is handled by TimeLib.
NTPClient timeClient(ntpUDP, "fi.pool.ntp.org", 0, 0);

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.print(" ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println(" UTC"); 
}

time_t getNTPTime() {
  timeClient.update();
  int seconds = timeClient.getEpochTime();
  return (time_t)seconds;
}

void setupTimeSync()  {  
  timeClient.begin();

  setSyncProvider(getNTPTime);

  while (timeStatus() != timeSet) {
     Serial.println("Unable to sync with NTP. Trying again in 5 seconds...");
     delay(5000);
  } 
  
  Serial.print("System time has been set: ");      
  digitalClockDisplay();
}
