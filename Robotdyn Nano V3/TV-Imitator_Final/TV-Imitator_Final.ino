#include <Wire.h>  
#include <TimeLib.h>
#include <DS3232RTC.h>

tmElements_t tm;

bool trigger = 0;

unsigned long currentMillis; // Current time, to which previous times are compared

unsigned long previousMillis_red = 0; // Counter for checking intervals of each led color
unsigned long previousMillis_blu = 0; // 
unsigned long previousMillis_grn = 0; //
unsigned long previousMillis_wht = 0; //
unsigned long previousMillis_alm = 0; // Counter for checking intervals of alarms and night light
unsigned long previousMillis[] = {previousMillis_red, previousMillis_blu, previousMillis_grn, previousMillis_wht, previousMillis_alm};

byte red = 0; // Index of array of each led color
byte grn = 1;
byte blu = 2;
byte wht = 3;
byte alm = 4;

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

short interval_red = 1; //Initial interval of changing lighting level (larger value means lightning changes slower)
short interval_grn = 2;
short interval_blu = 3;
short interval_wht = 4;
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

Alarm alm1(ALM1_MATCH_HOURS, 0, 30, 20, 0);
Alarm alm2(ALM2_MATCH_HOURS, 0, 45, 23, 0);
Alarm alm_morning(ALM2_MATCH_HOURS, 0, 0, 8, 0);

short t_gap, current_t;

const short nl_number = 4;

short tn[nl_number];
short t_hours[nl_number];
short t_minutes[nl_number];
short t_duration[nl_number];
short t_h_stop[nl_number];
short t_m_stop[nl_number];


void setup()  {
	Serial.begin(9600);

	pinMode(red, OUTPUT);           
	pinMode(grn, OUTPUT);        
	pinMode(blu, OUTPUT); 
	pinMode(wht, OUTPUT); 

	RTC.setAlarm(alm1.Type, alm1.Seconds, alm1.Minutes, alm1.Hours, alm1.Date);
	RTC.setAlarm(alm2.Type, alm2.Seconds, alm2.Minutes, alm2.Hours, alm2.Date);
	
	RTC.read(tm);
	
	if (((tm.Hour*60+tm.Minute)>=(alm1.Hours*60+alm1.Minutes))&& 
		((tm.Hour*60+tm.Minute)<(alm2.Hours*60+alm2.Minutes))) { // Check if time of first alarm has already passed
		trigger = true;
		
	}
	
	
	nightLightGeneratorTask();
	
	Serial.println("Started at:");
	TimeRead();
}
 
void loop()
{
	currentMillis = millis();
	
	if (currentMillis-previousMillis[alm]>30000) { // Checks for alarms every 30 secs
		
		if ( RTC.alarm(ALARM_1) ) {     // has Alarm1 triggered?
			trigger = 1;
			
			TimeRead();
		} else if ( RTC.alarm(ALARM_2) ) {     // has Alarm2 triggered?
			trigger = 0;

			ledOff(red);
			ledOff(grn);
			ledOff(blu);
			ledOff(wht);
			
			nightLightGeneratorTask();

			TimeRead();
		}
		
		RTC.read(tm);
		current_t = tm.Hour*60 + tm.Minute;
		
		for (byte i = 0; i < sizeof(tn)/sizeof(short); i++) { // Checking all night light times
		
			if (current_t >= t_h_stop[i]*60 + t_m_stop[i]) {
				ledOff(wht);
			} else if (current_t >= t_hours[i]*60 + t_minutes[i]) {
				ledOn(wht);
			}
		}
		
		previousMillis[alm] = currentMillis;
	}
	
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

void ledOn(byte pin) {
	analogWrite(ledpins[pin], 255);
}
 
void ledSmooth(byte color, byte interval, )
{
	
	if(currentMillis - previousMillis[color] > intervals[color]) {
		// сохраняем время последнего переключения
			
		if (pwmfreezes[color] == prevpwms[color]) { // if current pwm "prevpwm" is equal to pwm that must be freezed "prevpwm"
			if (currentMillis-previousMillis[color] > intervalfreezes[color]) { // if freezed pwm level stayed for specified time interval "intervalfreeze", we generate new interval of freezing and new pwm level of freezing. 
				intervalfreezes[color] = random(1000, 10001);
				pwmfreezes[color] = random(0, 240);
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
				intervals[color] = random(1,5);
				prevpwms[color] = newpwms[color];
				newpwms[color] = random(0, 256);
			}
			
		}
	}
}

// Algorithm for genrating random times for switching night-light

void nightLightGeneratorTask() {
	
	randomSeed(analogRead(0));
	
	t_gap = (alm2.Hours*60+alm2.Minutes);
	
	if (t_gap > (alm_morning.Hours*60 + alm_morning.Minutes)) {
		t_gap = 24*60 - t_gap + (alm_morning.Hours*60 + alm_morning.Minutes);
	} else {
		t_gap = (alm_morning.Hours*60 + alm_morning.Minutes) - t_gap;
	}
	
	t_gap = t_gap/(sizeof(tn)/sizeof(short));
	
	for (byte i = 0; i < sizeof(tn)/sizeof(short); i++) {
		
		tn[i] = random(15, t_gap-15) + i*t_gap; // Times (in minutes) of starting night lights, counting from alm2

		t_hours[i] = tn[i] / 60;
		t_hours[i] += alm2.Hours;
		t_minutes[i] = tn[i] % 60;
		t_minutes[i] += alm2.Minutes;
		
		if (t_minutes[i] >= 60) {
			t_hours[i]++;
			t_minutes[i] -= 60;
		}
		if (t_hours[i] >= 24) t_hours[i] -= 24;
		
		t_duration[i] = random(10, 15);
		t_m_stop[i] = t_minutes[i] + t_duration[i];
		t_h_stop[i] = t_hours[i];
		
		if (t_m_stop[i] >= 60) {
			t_h_stop[i]++;
			t_m_stop[i] -= 60;
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
		Serial.println();
		Serial.println();
		
	}
}
