#include <DS3232RTC.h>
#include <TimeLib.h>
#include <Wire.h>  

tmElements_t tm;

void setup()  {
  Serial.begin(9600);
  RTC.setAlarm(ALM1_MATCH_HOURS, 0, 45, 7, 0);
  RTC.setAlarm(ALM2_MATCH_HOURS, 0, 0, 16, 0);
  TimeRead();
}
 
void loop()
{
  if ( RTC.alarm(ALARM_1) ) {     //has Alarm1 triggered?
      Serial.println("Wake Up!");
      TimeRead();
  }

  if ( RTC.alarm(ALARM_2) ) {     //has Alarm2 triggered?
      Serial.println("Get High!");
      TimeRead();
  }
}

void TimeRead() {
      RTC.read(tm);
      Serial.print(tm.Hour, DEC);
      Serial.print(':');
      Serial.print(tm.Minute,DEC);
      Serial.print(':');
      Serial.println(tm.Second,DEC);
}
 
