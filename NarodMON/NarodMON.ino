// Скетч для Arduino для отправки данных о температуре на Народный мониторинг.
//
// По исходнику: Гладышев Дмитрий (2012-2013)
// http://goo.gl/gP4yTi
//
// Доработка: igorkkk http://igorkandaurov.ru
//
//	Добавлена возможность одновременной работы с несколькими датчиками
//	Пределано для  wifi shield.
//	Изменен протокол и порт передачи на документированный на сайте narodmon.ru.

 
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <OneWire.h>
 
//****************************************************************************************
#define mac "#AA-AA-AA-AA-AA-00"												// # + MAC.  Адрес Arduino, придумать самостоятельно, должен быть уникальным!!!;
OneWire ds(2);																	// пин подключения термодатчиков DS18B20!
const unsigned long postingInterval = 420000;									// интервал между отправками данных в миллисекундах, не менее 5 минут!!! 
char ssid[] = "your_ssid";														// SSID (name) wifi сети
char pass[] = "your_password";													// Пароль wifi сети 
//****************************************************************************************
int status = WL_IDLE_STATUS;
char server[] = "narodmon.ru";													// Сервер narodmon.ru (использование DNS)
//IPAddress server(94,19,113,221);												// или IP сервера narodmon.ru , но одно из двух
 
WiFiClient client;																// Инициализация клиента wifi
 
unsigned long lastConnectionTime = 0;											// время последней передачи данных
boolean lastConnected = false;													// текущее состояние подключения к narodmon.ru
 
int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;					// Переменные и массивы для пересчетов температуры
char CurTemp[6];
byte i;
byte present = 0;
byte data[12];
char temp[3];

char replyBuffer[80];															// Промежуточный массив информации
char replyBuffer00[80];															// Массивы для вывода информации для отправки
char replyBuffer01[80];															// количество - по числу датчиков!!!
//char replyBuffer02[80];														

byte addr[8];																	// Массивы для хранения адресов датчиков
byte addr00[8];																	// количество - по числу датчиков!!!
byte addr01[8];																	
//byte addr02[8]; 

void setup() 
{
 		Serial.begin(9600);
 	
		ds.search(addr00);														// Ищем датчики. Количество запросов - по числу датчиков
		ds.search(addr01);
		//ds.search(addr02)
	
		delay(2000);
		Serial.print("Address 00 =");											// Напечатаем адреса обнаруженных датчиков
	
		for( i = 0; i < 8; i++) {
			Serial.write(' ');
			Serial.print(addr00[i], HEX);
		}
			
		Serial.println();
		Serial.print("Address 01 =");
			
		for( i = 0; i < 8; i++) {
			Serial.write(' ');
			Serial.print(addr01[i], HEX);
		}
			
		Serial.println();
				
		//Serial.print("Address 02 =");
				
		//for( i = 0; i < 8; i++) {
		//	Serial.write(' ');
		//	Serial.print(addr00[i], HEX);
		//}
				
		//Serial.println();
				
		delay(2000);

		if (WiFi.status() == WL_NO_SHIELD) {									// Проверим, есть ли шилд
			Serial.println("WiFi shield not present");
			while(true);														// Если нет - остановка
		}
	
		while (status != WL_CONNECTED) {										// Соединяемся с сетью Wifi
			Serial.print("Attempting to connect to SSID: ");
			Serial.println(ssid);
			status = WiFi.begin(ssid, pass);									// Соединение с сетью WPA/WPA2
		}

		delay(10000);															// 10 секунд для инициализации wifi;
   
		lastConnectionTime = millis()-postingInterval+15000;					//первое соединение через 15 секунд после запуска
			
		Serial.println("Connected to wifi");									// Опишем подключение к Wifi
		printWifiStatus();
		Serial.println("\nStarting connection to server...");
}
void loop()
{
		if (!client.connected() && lastConnected)  {
				Serial.println();
				Serial.println("All right! disconnecting.");
				client.stop();
		}
 
		if(!client.connected() && (millis() - lastConnectionTime > postingInterval))	
		{																				
																				//если не подключены и прошло время, то делаем замер,
																				//переподключаемся и отправляем данные
			
			//********************** Первый датчик ************************************//			
			
						for (i=0; i<8; i++)	{
							addr[i] = addr00[i];
						}
						getTemperature();
						itos(Whole,temp);
						makeString();
						Serial.println(replyBuffer);
			
						for (i=0; i<80; i++) {
							replyBuffer00[i] = replyBuffer[i]; 
						}
			
			//********************* Второй датчик ************************************//			 
			 for (i=0; i<8; i++)	{
				 addr[i] = addr01[i];
			 }
 
			 getTemperature();
			 itos(Whole,temp);
			 makeString();
			 Serial.println(replyBuffer);
			 for (i=0; i<80; i++) {
				 replyBuffer01[i] = replyBuffer[i];
			 }
 			
			//********************* Третий датчик ************************************//
			//for (i=0; i<8; i++)	{
				//addr[i] = addr02[i];
			//}
			//
			//getTemperature();
			//itos(Whole,temp);
			//makeString();
			//Serial.println(replyBuffer);
			//for (i=0; i<80; i++) {
				//replyBuffer02[i] = replyBuffer[i];
			//}

			httpRequest();														// Отправляем на сервер;
 
		}
			lastConnected = client.connected();									//храним последнее состояние подключения;
}
 
void httpRequest() {															// Отправка данных на сервер порт 8283 по следующему алгоритму:
  if (client.connect(server, 8283)) {											// #мак-адрес arduino#код датчика#температура#код датчика#температура...## (конец передачи)
	  Serial.println("connected to narodmon.ru");								
	  client.println(mac);														
      client.println(replyBuffer00);
	  client.println(replyBuffer01);
	  //client.println(replyBuffer02);
	  client.print("##");
      
      lastConnectionTime = millis();
  } 
  
  else {
	  Serial.println("connection to narodmon.ru failed");
      Serial.println("disconnecting.");
	  
      client.stop();
  }
}
 
void itos(int n, char bufp[3])													// Превращаем int в string;
{
  char buf[3]={'0','0','\0'}; 
  int i = 1;
 
  while (n > 0) {
    buf[i] = (n % 10)+48;
    i--;
    n /= 10;
  }
 
  for (i=0; i<3; i++)
    bufp[i]=buf[i];
}

void printWifiStatus() {
																				// Напечатаем имя SSID сети:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());
																				// Напечатаем WiFi shield's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);
																				// Напечатаем силу сигнала Wifi:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}

//****************************************** Запрос на датчик *****************************************************
	void getTemperature (void)
	{
		ds.reset();
		ds.select(addr);
		ds.write(0x44,1);

		delay(750);

		present = ds.reset();
		ds.select(addr);
		ds.write(0xBE);

		for ( i = 0; i < 9; i++) {												// Необходимо 9 байтов
			data[i] = ds.read();
		}

		LowByte = data[0];
		HighByte = data[1];
		TReading = (HighByte << 8) + LowByte;
		SignBit = TReading & 0x8000;											// Проверка бита отрицательной температуры
		if (SignBit) {															// Если отрицательный
			TReading = (TReading ^ 0xffff) + 1;									// Продолжаем расчет
		}
		
		Tc_100 = (6 * TReading) + TReading / 4;									// Умножаем (100 * 0.0625) то есть 6.25

		Whole = Tc_100 / 100;													// Отдельно целая и дробная часть температуры
		Fract = Tc_100 % 100;
	//*****************************************************************************************************************
}
	void makeString (void)														// Изготавливаем строку для отправки 
	{	
		memset(CurTemp, 0, sizeof(CurTemp));									// Очистка CurTemp
		strcpy(CurTemp,temp);													// И запись в него
		strcat(CurTemp,".");													// Добавим точку
																				
		memset(replyBuffer, 0, sizeof(replyBuffer));							// Очистка 
		strcpy(replyBuffer,"#");												// Добавим разделитель # 
																				// конвертируем адрес термодатчика
		for (int k=7; k>=0; k--) {
			int b1=addr[k]/16;
			int b2=addr[k]%16;
			char c1[2],c2[2];
				
			if (b1>9) c1[0]=(char)(b1-10)+'A';
			else c1[0] = (char)(b1) + '0';
			if (b2>9) c2[0]=(char)(b2-10)+'A';
			else c2[0] = (char)(b2) + '0';
						
			c1[1]='\0';
			c2[1]='\0';
						
			strcat(replyBuffer,c1);
			strcat(replyBuffer,c2);
		}
		strcat(replyBuffer,"#");
		
		if (SignBit) {															// Если температура отрицательная - добавим "минус"
			strcat(replyBuffer,"-");
		}
		
		strcat(replyBuffer,temp);
		strcat(replyBuffer,".");
		itos(Fract,temp);
		strcat(replyBuffer,temp);
		strcat(CurTemp,temp);
}