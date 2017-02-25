
/*

An Arduino "Fake TV"
This is an Arduino simulation of the product called "Fake TV" 
It simulates the action of a TV being viewd by a person at night.
The purpose is to fool burglars into thinking someone is at home watching a TV at night.
Place it near a window behind blinds or curtains so as to not directly show the LEDs.
You only want the aggragate effect of all the LEDs to be seen.

I used 2 green, 2 red, one bright white and one bright blue.
I neede more red/green because the apparent brightness of the white and blue is greater than the red/greens
Use a diffuser over the board such as vellum papar to help scene quality.
Of course all these parameters can be changed to suit your needs.
Check at night near a window and adjust to suit your preferences.
Free to modify, the possible scene algorithms are endless.

*/

int blu = 11;
int red = 10;
int grn = 9;
int wht = 6;
int randpwm = 0;
int currentpwm = 0;
/*int randpwm2= 0;
int randpwm3 = 0;
int randpwm4 = 0;*/
int currentpwm1 = 0;
int currentpwm2 = 0;
int currentpwm3 = 0;
int currentpwm4 = 0;

void setup() 
{   
  Serial.begin(9600);
  pinMode(red, OUTPUT);           
  pinMode(grn, OUTPUT);        
  pinMode(blu, OUTPUT); 
  pinMode(wht, OUTPUT); 
}

void loop() 
{
  /*for(int i=0;i<10;i++)  //play scene 1 multiple times
  {
    scene1();
  }
  if (random(2) == 1)  //Possibly call scene 2
  {
    scene2();
  }
  if (random(2) == 1)  //Possibly call scene 3
  {
    scene3();
  }
  if (random(2) == 1)  //Possibly call scene 4
  {
    scene4();
  }
  if (random(4) == 1)  //Possibly do a commercial break
  {
    commercial();
  }*/

  currentpwm1 = analogSmoothWrite(grn, random(20,255), currentpwm1);
  //randpwm1 = random(20,255); 
  currentpwm2 = analogSmoothWrite(red, random(20,255), currentpwm2);
  //randpwm2 = random(20,255); 
  currentpwm3 = analogSmoothWrite(blu, random(20,255), currentpwm3);
  //randpwm3 = random(20,255); 
  currentpwm4 = analogSmoothWrite(wht, random(20,255), currentpwm4);
  //randpwm4 = random(20,255);  
}

int analogSmoothWrite(int color, int randomPWM, int lastPWM) {
    //Serial.println(lastPWM);
    //Serial.println(randomPWM);
    if (lastPWM < randomPWM){
      for (int i = lastPWM; i < randomPWM; i++) {
        analogWrite(color, i);
        delay(random(3));
      }
    } else {
      for (int i = lastPWM; i >= randomPWM; i--) {
        analogWrite(color, i);
        delay(random(3));
      }
    }
   return randomPWM;
  }


/*
These are the main scene algorithms
*/



void scene1()
// Changes random light levels and linger-times 
// of all colors to simulate "normal" TV action
{
  randpwm = random(20,255);
  analogWrite(red,randpwm);  
  randpwm = random(20,255);
  analogWrite(grn,randpwm); 
  analogWrite(blu,random(10,225)); 
  analogWrite(wht,random(10,175));
  delay(random(500,2000));
}

void scene2()
// Increases intensity of wht,blu (fade-in)
{
  delay(1000);
  for(int i=2;i<255;i++)
  {
    analogWrite(blu,i); 
    analogWrite(wht,i);
    delay(20);
  }
}

void scene3()
// Flickers wht,blu for a flickeriing scene effect
{
  boolean sw = HIGH;
  for(int i=0;i<30;i++)
  {
    digitalWrite(wht,sw);
    digitalWrite(blu,sw);
    sw = !sw;
    delay(random(50,300));
  }
}

void scene4()
// Changes red/grn light levels only
// wht/blu are off
{
  //don't use wht/blu
  digitalWrite(wht,LOW);
  digitalWrite(blu,LOW);
  for(int i=0;i<12;i++)
  {
    randpwm = random(20,255);
    analogWrite(red,randpwm); 
    randpwm = random(20,255);
    analogWrite(grn,randpwm);
    delay(random(200,2000));
  }
}

void commercial()
// Simulates a switch to or from a commercial break 
{
  analogWrite(red,2); 
  analogWrite(grn,2);
  analogWrite(blu,0);
  analogWrite(wht,0);
  delay(random(1000,2500));
}
