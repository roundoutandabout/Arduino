/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO 
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
  
  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  modified 2 Sep 2016
  by Arturo Guadalupi
  
  modified 8 Sep 2016
  by Colby Newman
*/

int blu = 11;
int red = 10;
int grn = 9;
int randpwm = 0;
int lastPWMblu = 0;
int lastPWMred = 0;
int lastPWMgrn = 0;


void scene1()
// Changes random light levels and linger-times 
// of all colors to simulate "normal" TV action
{
  lastPWMred=analogSmoothWrite(red,random(20,255), lastPWMred);  
  lastPWMgrn=analogSmoothWrite(grn,random(20,255), lastPWMgrn); 
  lastPWMblu=analogSmoothWrite(blu,random(10,225), lastPWMblu); 
  delay(random(500,2000));
}

int analogSmoothWrite(int color, int randomPWM, int lastPWM) {
    Serial.println(lastPWM);
    Serial.println(randomPWM);
    if (lastPWM < randomPWM){
      for (int i = lastPWM; i < randomPWM; i++) {
        analogWrite(color, i);
        delay(random(2));
      }
    } else {
      for (int i = lastPWM; i >= randomPWM; i--) {
        analogWrite(color, i);
        delay(random(2));
      }
    }
   return randomPWM;
  }

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(blu, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(grn, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  /*for (int i = 0; i < 100; i++) {
    analogWrite(blu, i);
    analogWrite(red, i);
    analogWrite(grn, i);
    delay(50);                       // wait for a second
  }
  delay(100);

  for (int i = 100; i >= 0; i--) {
    analogWrite(blu, i);
    analogWrite(red, i);
    analogWrite(grn, i);
    delay(50);                       // wait for a second
  }
  delay(1000);*/
  scene1();
  //lastPWM = analogSmoothWrite(grn, random(20,255), lastPWM);
}
