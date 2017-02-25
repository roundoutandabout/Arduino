#include <Wire.h>  
#include <TimeLib.h>
#include <DS3232RTC.h>

tmElements_t tm;

bool trigger = 0;

unsigned long currentMillis; // Current time, to which previous times are compared

unsigned long previousMillis_red = 0; // Counter for checking intervals of each led color
unsigned long previousMillis_blu = 0;
unsigned long previousMillis_grn = 0;
unsigned long previousMillis_wht = 0;
unsigned long previousMillis[] = {previousMillis_red, previousMillis_blu, previousMillis_grn, previousMillis_wht};

byte red = 0; // Index of array of each led color
byte grn = 1;
byte blu = 2;
byte wht = 3;

char* colours[] = {"red", "green", "blue", "white"};
byte* ledpins[] = {11, 10, 9, 6}; // {red, grn, blu, wht};

short prevpwm_red = 0; // PWM level that is gradually changing to newpwm
short prevpwm_grn = 0;
short prevpwm_blu = 0;
short prevpwm_wht = 0;
short prevpwms[] = {prevpwm_red, prevpwm_grn, prevpwm_blu, prevpwm_wht};

short newpwm_red = 0; // PWM level that is gradually changing to newpwm
short newpwm_grn = 0;
short newpwm_blu = 0;
short newpwm_wht = 0;
short newpwms[] = {newpwm_red, newpwm_grn, newpwm_blu, newpwm_wht};

short interval_red = 3; // Interval of changing lighting level (larger value means lightning changes slower)
short interval_grn = 3;
short interval_blu = 3;
short interval_wht = 3;
short intervals[] = {interval_red, interval_grn, interval_blu, interval_wht};

short intervalfreeze_red = 50; // Interval of freezing on particular PWM level
short intervalfreeze_grn = 50;
short intervalfreeze_blu = 50;
short intervalfreeze_wht = 50;
short intervalfreezes[] = {intervalfreeze_red, intervalfreeze_grn, intervalfreeze_blu, intervalfreeze_wht};

short pwmfreeze_red = 0; // PWM level which freezes
short pwmfreeze_grn = 0;
short pwmfreeze_blu = 0;
short pwmfreeze_wht = 0;
short pwmfreezes[] = {pwmfreeze_red, pwmfreeze_grn, pwmfreeze_blu, pwmfreeze_wht};

/*class LedColor {
	
	public:
	
	short color;
	short interval;
	short prevpwm;
	short newpwm;
	short pwmfreeze;
	short intervalfreeze;
	unsigned long previousMillis;
	
	Alarm (ALARM_TYPES_t type, byte seconds, byte minutes, byte hours, byte date)
	{
		Type = type;    

		Date = date;
		Hours = hours;

		Minutes = minutes; 
		Seconds = seconds;
	}
 
};*/

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

Alarm alm1(ALM1_MATCH_HOURS, 0, 40, 13, 0);
Alarm alm2(ALM2_MATCH_HOURS, 0, 45, 13, 0);

void setup()  {
	Serial.begin(9600);

	pinMode(red, OUTPUT);           
	pinMode(grn, OUTPUT);        
	pinMode(blu, OUTPUT); 
	pinMode(wht, OUTPUT); 

	RTC.setAlarm(alm1.Type, alm1.Seconds, alm1.Minutes, alm1.Hours, alm1.Date);
	RTC.setAlarm(alm2.Type, alm2.Seconds, alm2.Minutes, alm2.Hours, alm2.Date);
	
	RTC.read(tm);
	
	if (((tm.Hour*60+tm.Minute)>=(alm2.Hours*60+alm1.Minutes))&& 
		((tm.Hour*60+tm.Minute)<(alm2.Hours*60+alm2.Minutes))) { // Check if time of first alarm has already passed
		trigger = true;
		
	}
	
	TimeRead();
}
 
void loop()
{
	
	if ( RTC.alarm(ALARM_1) ) {     //has Alarm1 triggered?
		trigger = true;
		
		TimeRead();
	}

	if ( RTC.alarm(ALARM_2) ) {     //has Alarm1 triggered?
		trigger = false;

		ledOff(red);
		ledOff(grn);
		ledOff(blu);
		ledOff(wht);

		TimeRead();
	}
	
	currentMillis = millis();
	
	tvImitatorTask();
}

void TimeRead() { // Reads and prints current time on RTC
      RTC.read(tm);
      Serial.print(tm.Hour, DEC);
      Serial.print(':');
      Serial.print(tm.Minute,DEC);
      Serial.print(':');
      Serial.println(tm.Second,DEC);
}

void tvImitatorTask() {
	if (trigger) {
		ledSmooth(red);
		ledSmooth(grn);
		ledSmooth(blu);
		ledSmooth(wht);
	}
}

void ledOff(byte pin) {
	analogWrite(ledpins[pin], 0);
}
 
void ledSmooth(byte color) //byte color, short interval, short prevpwm, short newpwm, short pwmfreeze, short intervalfreeze, unsigned long previousMillis)
{
	
	if(currentMillis - previousMillis[color] > intervals[color]) {
		// сохраняем время последнего переключения
			
		if (pwmfreezes[color] == prevpwms[color]) { // if current pwm "prevpwm" is equal to pwm that must be freezed "prevpwm"
			if (currentMillis-previousMillis[color] > intervalfreezes[color]) { // if freezed pwm level stayed for specified time interval "intervalfreeze", we generate new interval of freezing and new pwm level of freezing. 
				intervalfreezes[color] = random(1000, 10001);
				pwmfreezes[color] = random(0, 251);
				//Serial.print("Match ");
				//Serial.print(colours[color]);
				//Serial.print(": ");
				//Serial.println(pwmfreezes[color]);
			}
		} else { // if current pwm is not equal to pwm that must be freezed, we cuntinue to change pwm level, until it will be equal to pwmfreeze again
			
			previousMillis[color] = currentMillis;
			
			if (newpwms[color]>prevpwms[color]) {
				analogWrite(ledpins[color], prevpwms[color]++);
				//Serial.println(prevpwm);
			} else if (newpwms[color]<prevpwms[color]) {
				analogWrite(ledpins[color], prevpwms[color]--);
				//Serial.println(prevpwm);
			} else {
				intervals[color] = random(1,3);
				prevpwms[color] = newpwms[color];
				newpwms[color] = random(0, 256);
			}
			
		}
	}
}