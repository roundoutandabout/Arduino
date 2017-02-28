#include <Wire.h>  
#include <TimeLib.h>
#include <DS3232RTC.h>

tmElements_t tm;

class Alarm {
	
	public:
	
	ALARM_TYPES_t Type;
	byte Date;
	byte Hours;
	byte Minutes;
	byte Seconds;
	
	Alarm (ALARM_TYPES_t type, byte seconds, byte minutes, byte hours, byte date)
	{
		Type = type;    

		Date = date;
		Hours = hours;

		Minutes = minutes; 
		Seconds = seconds;
	}
 
};

Alarm alm1(ALM1_MATCH_HOURS, 0, 40, 20, 0);
Alarm alm2(ALM2_MATCH_HOURS, 0, 20, 23, 0);
Alarm alm_morning(ALM2_MATCH_HOURS, 0, 30, 8, 0);

short t_gap;

short tn[3];
short t_hours[3];
short t_minutes[3];
short t_duration[3];
short t_h_stop[3];
short t_m_stop[3];


void setup()  {
	Serial.begin(9600);
	
	RTC.read(tm);
	
	//***********************
	
	randomSeed(analogRead(0));
	
	t_gap = 24*60-(alm2.Hours*60+alm2.Minutes);
	t_gap = t_gap + (alm_morning.Hours*60+alm_morning.Minutes);
	
	t_gap = t_gap/3;
	
	Serial.println(sizeof(tn)/sizeof(short));
	
	for (byte i = 0; i < sizeof(tn)/sizeof(short); i++) {
		
		tn[i] = random(15, t_gap-15) + i*t_gap;
	
		Serial.print("t");
		Serial.print(i);
		Serial.print(": ");
		Serial.println(tn[i]);

		t_hours[i] = tn[i] / 60;
		t_hours[i] += alm2.Hours;
		t_minutes[i] = tn[i] % 60;
		t_minutes[i] += alm2.Minutes;
		
		if (t_minutes[i] >= 60) {
			t_hours[i]++;
			t_minutes[i] -= 60;
		}
		if (t_hours[i] >= 24) t_hours[i] -= 24;
		
		t_duration[i] = random(5, 15);
		t_m_stop[i] = t_minutes[i] + t_duration[i];
		t_h_stop[i] = t_hours[i];
		if (t_m_stop[i] >= 60) {
			t_h_stop[i]++;
			t_m_stop[i] = t_m_stop[i] - 60;
		}
		
		Serial.print("night-light ");
		Serial.print(i);
		Serial.println(" starts at:");
		Serial.print(t_hours[i]);
		Serial.print(":");
		Serial.print(t_minutes[i]);
		Serial.println("");
		Serial.println("and stops at: ");
		Serial.print(t_h_stop[i]);
		Serial.print(":");
		Serial.println(t_m_stop[i]);
		Serial.println();
		Serial.println();
		
	}
	
	//Alarm alm_t1(ALM2_MATCH_HOURS, 0, t1_minutes, t1_hours, 0);
	
	//*************************
	
	
	TimeRead();
	
	Serial.println();
	Serial.println();
	
	t_m_stop[0] = 0
	t_h_stop[0] = 0
	t_hours[0] = 
	
	
	if (((current_t >= t_hours[0]*60 + t_hours[0]*60) && (current_t <= 24*60 + t_h_stop[0]*60 + t_m_stop[0]))||((current_t + 24*60 >= t_hours[0]*60 + t_hours[0])&&(current_t <= t_h_stop[0]*60 + t_m_stop[0]))) { // Too Hard to explain
					
					if (flag_nl) {
						ledOn(wht);
						flag_nl = 0; // Flag allows not to put LED to max value every time
						Serial.println("Bug1");
					}
					
					
				} else {
					
					if (!flag_nl) {
						ledOff(wht);
						flag_nl = 0;
						Serial.println("Bug2");
					}
					
				}
				
			} else {
		
				if ((current_t >= t_hours[0]*60 + t_hours[0])&&(current_t <= t_h_stop[0]*60 + t_m_stop[0])) {
					
					if (flag_nl) {
						ledOn(wht);
						flag_nl = 0; // Flag allows not to put LED to max value every time
						Serial.println("Bug3");
					}
					
					
				} else {
					
					if (!flag_nl) {
						ledOff(wht);
						flag_nl = 0;
						Serial.println("Bug4");
					}
					
				}
}
 
void loop()
{	
}

void TimeRead() { // Reads and prints current time on RTC
      RTC.read(tm);
      Serial.print(tm.Hour, DEC);
      Serial.print(':');
      Serial.print(tm.Minute,DEC);
      Serial.print(':');
      Serial.println(tm.Second,DEC);
}