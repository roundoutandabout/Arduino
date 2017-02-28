
int flag=0; 
int regim=1;



void setup() {
  // put your setup code here, to run once:
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode (A1, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  if((analogRead(1)>500)&&(flag==0))//РµСЃР»Рё РєРЅРѕРїРєР° РЅР°Р¶Р°С‚Р°   
     // Рё РїРµСЂРµРјРµРЅРЅСЏ flag СЂР°РІРЅР° 0 , С‚Рѕ ... 
     {   
      Serial.println(analogRead (1));
      Serial.println(flag);
        flag=1; 

     } 
       
      if((analogRead(1)<500)&&(flag==1))//РµСЃР»Рё РєРЅРѕРїРєР° РќР• РЅР°Р¶Р°С‚Р° 
     //Рё РїРµСЂРµРјРµРЅРЅР°СЏ flag СЂР°РІРЅР° - 1 ,С‚Рѕ ... 
     {  
	 
        flag=0;//РѕР±РЅСѓР»СЏРµРј РїРµСЂРµРјРµРЅРЅСѓСЋ "knopka"
		regim++;
		if(regim>6)
        { 
          regim=1;
		  
        } 
     } 
      
       
       
    if(regim==1)//РїРµСЂРІС‹Р№ СЂРµР¶РёРј 
    { 
      digitalWrite(6, 0);
      digitalWrite(9, 0);
      digitalWrite(10, 0);
      digitalWrite(11, 0);  
        
      //Р·РґРµСЃСЊ РјРѕР¶РµС‚ Р±С‹С‚СЊ Р»СЋР±РѕРµ РІР°С€Рµ РґРµР№СЃС‚РІРёРµ 
    } 
    if(regim==2)//РІС‚РѕСЂРѕР№ СЂРµР¶РёРј 
    { 
      digitalWrite(9, 255);
      digitalWrite(6, 0);
      digitalWrite(10, 0);
      digitalWrite(11, 0);
        
      //Р·РґРµСЃСЊ РјРѕР¶РµС‚ Р±С‹С‚СЊ Р»СЋР±РѕРµ РІР°С€Рµ РґРµР№СЃС‚РІРёРµ 
    } 
      
    if(regim==3)//С‚СЂРµС‚РёР№ СЂРµР¶РёРј 
    { 
        
      digitalWrite(10, 255); 
      digitalWrite(6, 0);
      digitalWrite(9, 0);
      digitalWrite(11, 0);  
        
      //Р·РґРµСЃСЊ РјРѕР¶РµС‚ Р±С‹С‚СЊ Р»СЋР±РѕРµ РІР°С€Рµ РґРµР№СЃС‚РІРёРµ 
    } 
      
    if(regim==4)//С‡РµС‚РІРµСЂС‚С‹Р№ СЂРµР¶РёРј 
    { 
        
      digitalWrite(11, 255);
      digitalWrite(6, 0);
      digitalWrite(9, 0);
      digitalWrite(10, 0); 
      //Р·РґРµСЃСЊ РјРѕР¶РµС‚ Р±С‹С‚СЊ Р»СЋР±РѕРµ РІР°С€Рµ РґРµР№СЃС‚РІРёРµ 
    }

    if(regim==5)
    { 
        
      digitalWrite(9, 255); 
      digitalWrite(10, 255);
      digitalWrite(11, 255);
      digitalWrite(6, 0);
      //Р·РґРµСЃСЊ РјРѕР¶РµС‚ Р±С‹С‚СЊ Р»СЋР±РѕРµ РІР°С€Рµ РґРµР№СЃС‚РІРёРµ 
    } 

    if(regim==6)
    { 
      digitalWrite(6, 255);   
      digitalWrite(9, 255); 
      digitalWrite(10, 255);
      digitalWrite(11, 255);
      //Р·РґРµСЃСЊ РјРѕР¶РµС‚ Р±С‹С‚СЊ Р»СЋР±РѕРµ РІР°С€Рµ РґРµР№СЃС‚РІРёРµ 
    } 
}

