int a=0;
int flag=0; 
bool val = 0;
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
        regim++;
        //СЌС‚Рѕ РЅСѓР¶РЅРѕ РґР»СЏ С‚РѕРіРѕ С‡С‚Рѕ Р±С‹ СЃ РєР°Р¶РґС‹Рј РЅР°Р¶Р°С‚РёРµРј РєРЅРѕРїРєРё 
        //РїСЂРѕРёСЃС…РѕРґРёР»Рѕ С‚РѕР»СЊРєРѕ РѕРґРЅРѕ РґРµР№СЃС‚РІРёРµ 
        // РїР»СЋСЃ Р·Р°С‰РёС‚Р° РѕС‚ "РґСЂРµР±РµР·РіР°"  100% 
          
        if(regim>6)//РѕРіСЂР°РЅРёС‡РёРј РєРѕР»РёС‡РµСЃС‚РІРѕ СЂРµР¶РёРјРѕРІ 
        { 
          regim=1;//С‚Р°Рє РєР°Рє РјС‹ РёСЃРїРѕР»СЊР·СѓРµРј С‚РѕР»СЊРєРѕ РѕРґРЅСѓ РєРЅРѕРїРєСѓ, 
                    // С‚Рѕ РїРµСЂРµРєР»СЋС‡Р°С‚СЊ СЂРµР¶РёРјС‹ Р±СѓРґРµРј С†РёРєР»РёС‡РЅРѕ 
        } 
     } 
       
      if((analogRead(1)<500)&&(flag==1))//РµСЃР»Рё РєРЅРѕРїРєР° РќР• РЅР°Р¶Р°С‚Р° 
     //Рё РїРµСЂРµРјРµРЅРЅР°СЏ flag СЂР°РІРЅР° - 1 ,С‚Рѕ ... 
     {  
        //if ((millis() - lastmillis)>500) regim++;
        Serial.println(digitalRead(14)<500);
        Serial.println(flag);
        flag=0;//РѕР±РЅСѓР»СЏРµРј РїРµСЂРµРјРµРЅРЅСѓСЋ "knopka" 
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

