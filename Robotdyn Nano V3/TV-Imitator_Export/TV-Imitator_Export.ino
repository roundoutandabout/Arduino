#include <Wire.h>  
#include <TimeLib.h>
#include <DS3232RTC.h>

tmElements_t tm;

// Essential vars

bool trigger = 0; // trigger for activating or deactivatinf TV imitator on alarms
byte mode = 0; // Modes: TV imitator, Off, White light level1; White light level2; Smooth LED flashing; Red light; Green light; Blue light.
bool flag=0; // Protection from bouncing
bool mode_flag = 1; // For activating each mode only one time

// END essential vars

// LEDSmooth vars

unsigned long currentMillis; // Current time, to which previous times are compared

unsigned long previousMillis_red = 0; // Counter for checking intervals of each led color
unsigned long previousMillis_blu = 0; // 
unsigned long previousMillis_grn = 0; //
unsigned long previousMillis_wht = 0; //
unsigned long previousMillis_alm = 0; // Counter for checking intervals of alarms and night-light
unsigned long previousMillis[] = {previousMillis_red, previousMillis_blu, previousMillis_grn, previousMillis_wht, previousMillis_alm};

byte red = 0; // Index of array of each led color
byte grn = 1;
byte blu = 2;
byte wht = 3;
byte alm = 4;

char* colours[] = {"red", "green", "blue", "white"};
byte* ledpins[] = {11, 10, 9, 6}; // {red, grn, blu, wht};

short prevpwm_red = 0; // PWM level that is gradually changed to newpwm
short prevpwm_grn = 0;
short prevpwm_blu = 0;
short prevpwm_wht = 0;
short prevpwms[] = {prevpwm_red, prevpwm_grn, prevpwm_blu, prevpwm_wht};

short newpwm_red = 0; // PWM level to which prevpwm is changed
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


// END LEDSmooth vars

class Alarm {
	
	public:
	
	ALARM_TYPES_t Type;
	byte Date;
	short Hours;
	short Minutes;
	byte Seconds;
	
	short getTimeInMinutes() {
		return Hours*60 + Minutes;
	}
	
	Alarm (ALARM_TYPES_t type, byte seconds, short minutes, short hours, byte date, bool randomize)
	{
		
		if (randomize) {
			short time;
			
			time = hours*60 + minutes - 40 + random(0, 80); // Shifts alarm time for random value from -40 min to +40 min
			
			if (time < 0) time += 24*60;
			
			hours = time / 60;
			minutes = time % 60;

			if (minutes >= 60) {
				hours++;
				minutes -= 60;
			}
			
			if (hours >= 24) hours -= 24;
		}
		
		Type = type;
		
		Date = date;

		Hours = hours;

		Minutes = minutes; 
		Seconds = seconds;
	}
 
};

	// Set TV Alarm times
	
	Alarm alm1(ALM1_MATCH_HOURS, 0, 0, 0, 0, 0);
	
	Alarm alm2(ALM2_MATCH_HOURS, 0, 0, 0, 0, 0);

	Alarm alm_morning(ALM2_MATCH_HOURS, 0, 0, 0, 0, 0);


	// END Set TV Alarm times

// Night-light vars

short t_gap, current_t;

const short nl_number = 4;

short tn[nl_number];
short t_hours[nl_number];
short t_minutes[nl_number];
short t_duration[nl_number];
short t_h_stop[nl_number];
short t_m_stop[nl_number];

bool flag_nl[nl_number] = {1, 1, 1, 1};

// END Night-light vars


void setup()  {
	Serial.begin(9600);
	randomSeed(analogRead(2));

	pinMode(red, OUTPUT);           
	pinMode(grn, OUTPUT);        
	pinMode(blu, OUTPUT); 
	pinMode(wht, OUTPUT);
	
	alm1 = Alarm(ALM1_MATCH_HOURS, 0, 30, 19, 0, 1);
	
	alm2 = Alarm(ALM2_MATCH_HOURS, 0, 0, 0, 0, 1);

	alm_morning = Alarm(ALM2_MATCH_HOURS, 0, 0, 7, 0, 0);

	RTC.setAlarm(alm1.Type, alm1.Seconds, alm1.Minutes, alm1.Hours, alm1.Date);
	RTC.setAlarm(alm2.Type, alm2.Seconds, alm2.Minutes, alm2.Hours, alm2.Date);
	
	RTC.read(tm);
	current_t = tm.Hour*60 + tm.Minute;
	
	if (alm1.getTimeInMinutes()>=alm2.getTimeInMinutes()) {
		
		if ((current_t>=alm1.getTimeInMinutes())|| 
		(current_t<alm2.getTimeInMinutes())) { // Check if time of first alarm has already passed
			trigger = true;
		
		}
	} else {
		if ((current_t>=alm1.getTimeInMinutes())&&
		(current_t<alm2.getTimeInMinutes())) { // Check if time of first alarm has already passed
			trigger = true;
		
		}
	}
	
	nightLightGeneratorTask();
	
	/*Serial.println("Started at:");
	TimeRead();
	Serial.println();
	
	Serial.println("Alarm 1 at:");
	Serial.print(alm1.Hours);
	Serial.print(":");
	Serial.println(alm1.Minutes);
	
	Serial.println("Alarm 2 at:");
	Serial.print(alm2.Hours);
	Serial.print(":");
	Serial.println(alm2.Minutes);*/
	
}
 
void loop()
{
	currentMillis = millis();
	
	if((analogRead(1)>500)&&(flag==0)) {
		flag=1;
		mode_flag = 1;
	}

	if((analogRead(1)<500)&&(flag==1)) {  
		flag=0;
		mode_flag = 1;
		mode++;
		if(mode>7) mode=0;
	}
	
	// Main menu
	
	switch (mode) {
		case 0: if (mode_flag) {
			
			ledOff(red);
			ledOff(grn);
			ledOff(blu);
			ledOff(wht);
			
			RTC.read(tm);
			current_t = tm.Hour*60 + tm.Minute;
	
			if (alm1.getTimeInMinutes()>=alm2.getTimeInMinutes()) {
				
				if ((current_t>=alm1.getTimeInMinutes())|| 
				(current_t<alm2.getTimeInMinutes())) { // Check if time of first alarm has already passed
					trigger = true;
				
				}
			} else {
				if ((current_t>=alm1.getTimeInMinutes())&&
				(current_t<alm2.getTimeInMinutes())) { // Check if time of first alarm has already passed
					trigger = true;
				}
			}
			
			mode_flag = 0;
			
		} 
			
			tvImitatorMode();
		break;
		case 1: if (mode_flag) {
			
			ledOff(red);
			ledOff(grn);
			ledOff(blu);
			ledOff(wht);
			
			mode_flag = 0;
			
		}
		break;
		case 2: if (mode_flag) {
			
			ledOn(wht);
			
			ledOff(red);
			ledOff(grn);
			ledOff(blu);
			
			mode_flag = 0;
		}
		break;
		case 3: if (mode_flag) {
			
			ledOn(wht);
			ledOn(red);
			ledOn(grn);
			ledOn(blu);
			
			mode_flag = 0;
		}
		break;
		case 4: 
			ledSmooth(red, 1, 4, 0, 255, 1000, 10001, 0, 230);
			ledSmooth(grn, 1, 4, 0, 255, 1000, 10001, 0, 230);
			ledSmooth(blu, 1, 4, 0, 255, 1000, 10001, 0, 230);
			ledSmooth(wht, 1, 4, 0, 255, 1000, 10001, 0, 230);
		break;
		case 5: if (mode_flag) {
			
			ledOn(red);
			
			ledOff(grn);
			ledOff(blu);
			ledOff(wht);
			
			mode_flag = 0;
		}
		break;
		case 6: if (mode_flag) {
			
			ledOn(grn);
			
			ledOff(red);
			ledOff(blu);
			ledOff(wht);
			
			mode_flag = 0;
		}
		break;
		case 7: if (mode_flag) {
			
			ledOn(blu);
			
			ledOff(red);
			ledOff(grn);
			ledOff(wht);
			
			mode_flag = 0;
		}
		break;
	}
	
	// END Main menu
}

void TimeRead() { // Reads and prints current time on RTC
      RTC.read(tm);
      Serial.print(tm.Hour, DEC);
      Serial.print(':');
      Serial.print(tm.Minute,DEC);
      Serial.print(':');
      Serial.println(tm.Second,DEC);
}

void tvImitatorMode () {
	if (currentMillis-previousMillis[alm]>30000) { // Check for alarms every 30 secs
		
		if ( RTC.alarm(ALARM_1) ) {     // has Alarm1 triggered?
			trigger = 1;
			
			//TimeRead();
		} else if ( RTC.alarm(ALARM_2) ) {     // has Alarm2 triggered?
			trigger = 0;

			ledOff(red);
			ledOff(grn);
			ledOff(blu);
			ledOff(wht);
			
			nightLightGeneratorTask();

			//TimeRead();
		}
		
		RTC.read(tm);
		current_t = tm.Hour*60 + tm.Minute;
		
		for (byte i = 0; i < sizeof(tn)/sizeof(short); i++) { // Checking all night-light times
		
			if (t_h_stop[i] < t_hours[i]) { // When night-light starts before midnight and stops after midnight
				
				if ((current_t >= t_hours[i]*60 + t_minutes[i]) || (current_t <= t_h_stop[i]*60 + t_m_stop[i])) {
					
					if (flag_nl[i]) {
						ledOn(wht);
						flag_nl[i] = 0; // Flag allows not to put LED to max value every time
						/*Serial.print(i);
						Serial.println(" Bug1");*/
					}
					
					
				} else {
					
					if (!flag_nl[i]) {
						ledOff(wht);
						flag_nl[i] = 1;
						/*Serial.print(i);
						Serial.println(" Bug2");*/
					}
					
				}
				
			} else {
		
				if ((current_t >= t_hours[i]*60 + t_minutes[i])&&(current_t <= t_h_stop[i]*60 + t_m_stop[i])) {
					
					if (flag_nl[i]) {
						ledOn(wht);
						flag_nl[i] = 0; // Flag allows not to put LED level to max value every time
						/*Serial.print(i);
						Serial.println(" Bug3");*/
					}
					
					
				} else {
					
					if (!flag_nl[i]) {
						ledOff(wht);
						flag_nl[i] = 1;
						/*Serial.print(i);
						Serial.println(" Bug4");*/
					}
					
				}
				
			}
		}
		
		previousMillis[alm] = currentMillis;
	}
	
	tvImitatorTask();
}

void tvImitatorTask() {
	if (trigger) {
		ledSmooth(red, 1, 4, 0, 255, 1000, 10001, 0, 230);
		ledSmooth(grn, 1, 4, 0, 255, 1000, 10001, 0, 230);
		ledSmooth(blu, 1, 4, 0, 255, 1000, 10001, 0, 230);
		ledSmooth(wht, 1, 4, 0, 255, 1000, 10001, 0, 230);
	}
}

void ledOff(byte pin) {
	analogWrite(ledpins[pin], 0);
}

void ledOn(byte pin) {
	analogWrite(ledpins[pin], 255);
}
 
void ledSmooth(byte color, byte minInterval, byte maxInterval, byte minPWM, byte maxPWM, short minFreezeInterval, short maxFreezeInterval, byte minFreezePWM, byte maxFreezePWM)
{
	
	if(currentMillis - previousMillis[color] > intervals[color]) {
		
		// сохраняем время последнего переключения
		
		if (pwmfreezes[color] == prevpwms[color]) { // if current pwm "prevpwm" is equal to pwm that must be freezed "prevpwm"
			if (currentMillis-previousMillis[color] > intervalfreezes[color]) { // if freezed pwm level stayed for specified time interval "intervalfreeze", we generate new interval of freezing and new pwm level of freezing. 
				intervalfreezes[color] = random(minFreezeInterval, maxFreezeInterval);
				pwmfreezes[color] = random(minFreezePWM, maxFreezePWM);
				//Serial.print("Match ");
				//Serial.print(colours[color]);
				//Serial.print(": ");
				//Serial.println(pwmfreezes[color]);
			}
		} else { // if current pwm is not equal to pwm that must be freezed, we cuntinue to change pwm level, until it will be equal to pwmfreeze again
			
			previousMillis[color] = currentMillis;
			
			if (newpwms[color]>prevpwms[color]) {
				analogWrite(ledpins[color], prevpwms[color]++);
			} else if (newpwms[color]<prevpwms[color]) {
				analogWrite(ledpins[color], prevpwms[color]--);
			} else {
				intervals[color] = random(minInterval, maxInterval);
				prevpwms[color] = newpwms[color];
				newpwms[color] = random(minPWM, maxPWM);
			}
			
		}
	}
}

// Algorithm for genrating random times for switching night-light

void nightLightGeneratorTask() {  // Defines times for switching on night-light. Night-light turns on for i times, in time range fron alm2 to alm_morning. Works for all values of alarm2 and alarm_morning	
	
	randomSeed(analogRead(2));
	
	t_gap = (alm2.Hours*60+alm2.Minutes);
	
	if (t_gap > (alm_morning.Hours*60 + alm_morning.Minutes)) {
		t_gap = 24*60 - t_gap + (alm_morning.Hours*60 + alm_morning.Minutes);
	} else {
		t_gap = (alm_morning.Hours*60 + alm_morning.Minutes) - t_gap;
	}
	
	t_gap = t_gap/(sizeof(tn)/sizeof(short));
	
	for (byte i = 0; i < sizeof(tn)/sizeof(short); i++) {
		
		tn[i] = random(15, t_gap-15) + i*t_gap; // Times (in minutes) of starting night-lights, counting from alm2

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
			t_m_stop[i] -= 60;
		}
		if (t_h_stop[i] >= 24) t_h_stop[i] -= 24;
		
		/*Serial.print("night-light ");
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
		Serial.println();*/
		
	}
}
