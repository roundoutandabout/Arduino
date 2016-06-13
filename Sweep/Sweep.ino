/* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 28 May 2015
 by Michael C. Miller
 modified 8 Nov 2013
 by Scott Fitzgerald

 http://arduino.cc/en/Tutorial/Sweep
*/ 

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards

Servo myservo1;

const int pinPhoto = A0;

const int data2 = 2;

int raw;
int angle;
int pos;

void setup() 
{ 
  myservo.attach(data2);  // attaches the servo on GIO2 to the servo object 
  //myservo1.attach(D3);
  Serial.begin(115200);
} 
 
void loop() 
{ 

  raw = analogRead( pinPhoto );
  angle = map(raw, 1, 1024, 0, 120);
  
  //Serial.println(raw);
  Serial.println(angle);
  
  myservo.write(angle);

  /* for(pos = 0; pos <= 120; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    //myservo1.write(pos);
    delay(10);                       // waits 15ms for the servo to reach the position 
  } 
  delay(100);
  for(pos = 120; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    //myservo1.write(pos); 
    delay(10);                       // waits 15ms for the servo to reach the position 
  }*/
} 

