#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#include <Ticker.h>

#include <DHT.h>
#define DHTPIN 14
#define DHTTYPE DHT11   // DHT 11/22

#include <DallasTemperature.h>
#include <OneWire.h>
#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 12 // точность бит. Если глючит или врет, уменьшить до 9

#include <Adafruit_BMP085.h>

#include <BH1750.h>

const char *ssid = "In_God_we_trust";
const char *password = "937999201";

const char* host = "api.thingspeak.com";
const char* apikey = "YIR58CFT1SIPMUJ0"; // ключик от thingsspeak.com
String Hostname;

const int led15 = 15; //red read data
const int led13 = 13; //blue load pages
const int led12 = 12; //green send to cloud
const int pinPhoto = A0;

bool ts_send  = false;
bool nm_send  = false;
bool ac_send  = false;

//****************
int oldSendInterval = 60;
int sendInterval = 60;
	
Ticker sender;

bool tickOccured;

void timerCallback() {
	tickOccured = true;	
}
/*	void timerCallback(void *pArg) {
		
		if (nm_send || ts_send) {
		
			digitalWrite(led15, 1);
			
				read_data();
				
			digitalWrite(led15, 0);
			
			digitalWrite(led12, 1);
			
				if (nm_send) {
					narodmon_send();
				}
				
				if (ts_send) {
					thingspeak_send();
				}
				
			digitalWrite(led12, 0);
			
			tickOccured = true;
		}
	}

	void user_init(int milliseconds) {
			
		os_timer_setfn(&myTimer, timerCallback, NULL);
		os_timer_arm(&myTimer, milliseconds, true);
	}*/

//****************

unsigned long currentMillis;

unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor (ms)


int raw = 0; // Photoresistor

float h; // Values for DHT11
float t;
float hi;

float pressure; // values for BMP180
float temp180;

float lux;

ESP8266WebServer server ( 80 );

Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE, 15);
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.

BH1750 lightMeter; // setting BH1750 (GY-302)

//setting DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;
int NumberOfDevices;

String base = "<!DOCTYPE html>\
        <head>\
			<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\
			<meta name=\"viewport\" content=\"width=400px\">\
			<title>Wi-Fi ESP8266</title>\
			<style type=\"text/css\">\
				body \{background-color: #7D8EE2;color:#FFF;}\
				a \{color: white;}\
				.blockk \{\
					border:double 3px #000000;\
					text-align:center;\
					background:#0059B3;\
					padding: 10px 10px 10px 10px;\
					-moz-border-radius: 5px;\
					-webkit-border-radius: 5px;\
					border-radius: 5px;}\
				form p {margin: 0;}\
				.caption {\
					font-size: 25px;\
					text-decoration: underline;}\
				.caption:hover {\
					text-decoration: none;\
					opacity: 0.4;}\
			</style>\
			<style type=\"text/css\" media=\'(min-width: 810px)\'>\
				body\{font-size:18px;}\
				.blockk \{width: 400px;}</style>\
            <style type=\"text/css\" media=\'(max-width: 800px) and (orientation:landscape)\'>\
				body\{font-size:8px;}\
			</style>";



void handle_root() {
	
	
	currentMillis = millis();

	if(currentMillis - previousMillis >= interval) {
		
		digitalWrite(led13, 1);
	// save the last time you read the sensor
	previousMillis = currentMillis;
	
	if( server.hasArg("send-period") ) {
		sendInterval = server.arg("send-period").toInt();
		if (sendInterval == 0) sendInterval = oldSendInterval;
		if (sendInterval < 10) sendInterval = 10;
		sender.attach(sendInterval, timerCallback);
		oldSendInterval = sendInterval;
	}
	

	if( server.hasArg("ts-send") ){
		if( strncmp(server.arg("ts-send").c_str(),"1",1) == 0 ) {
			ts_send = true;
			sender.attach(sendInterval, timerCallback); // enable timer
		}
		else {
			if( strncmp(server.arg("ts-send").c_str(),"0",1) == 0 ) {
				ts_send = false;
			}
		}
	}
	
	if( server.hasArg("nm-send") ){
		if( strncmp(server.arg("nm-send").c_str(),"1",1) == 0 ) {
			nm_send = true;
			sender.attach(sendInterval, timerCallback);
		}
		else {
			if( strncmp(server.arg("nm-send").c_str(),"0",1) == 0 ) {
				nm_send = false;
			}
		}
	}
    
	//***************************

	if (!nm_send && !ts_send) {
		read_data();
		sender.detach();
	}

	//***************************

    String out = "";

    out += base;
	
	out +="<meta http-equiv=\"REFRESH\" content=\"" + String(sendInterval) + "\">\
		</head><body><center><div class=\"blockk\"><a href=\"/\" class=\"caption\">ESP8266 Weather Station</a><br><hr>"; // Добавление обновления страницы в соответствии с интервалом отправки данных
	
    out +="<b>BMP180:</b><br>Температура: " + String(temp180) + " &deg;C.<br>Давление (атм.): " + String(pressure) + " мбар/мм.рт.ст.<hr>\
	<b>DHT11:</b><br>Температура: " + String(t) + " &deg;C.<br>Влажность (отн.): "+String(h)+" %.<br>Heat index: "+String(hi)+ " &deg;C.<hr>\
	<b>Фотодиод:</b><br>" + String(raw) + " /1024.<hr>\
	<b>Люксметр:</b><br>" + String(lux) + " lux.<hr>";
	
	sensors.begin(); //ds18b20
	NumberOfDevices = sensors.getDeviceCount(); //поищем.
	sensors.requestTemperatures(); // Send the command to get temperatures
	
	if (NumberOfDevices) {
		out += "<b>DS18B20 (" + String(NumberOfDevices) + "):</b><br>";
		
		for (int i = 0; i < NumberOfDevices; i++)  { //перечисляем датчики и их показания
		
			sensors.getAddress(tempDeviceAddress, i);
			
			out +="Sensor " + String(i+1) + ":<br>\
			Temperature: " + String(sensors.getTempCByIndex(i)) + " &deg;C.<br>\
			Address: " + String(returnAddress(tempDeviceAddress)) + ".<br>";
			
		}
		out +="<hr>";
	}
	
    if( ts_send ){
        out+="\
          <a href=\"https://thingspeak.com/channels/110382\" target=\"_blank\"><b>Thingspeak.com</b></a><span style=\" color: lightgreen\"> &#10004;</span><hr>\
        ";
    }
    else {
        out+="\
          <a href=\"https://thingspeak.com/channels/110382\" target=\"_blank\"><b>Thingspeak.com</b></a><span style=\" color: tomato\"> &#10006;</span><hr>\
        ";
    }
	
	if( nm_send ){
        out+="\
          <a href=\"http://narodmon.ru/\" target=\"_blank\"><b>Narodmon.com</b></a><span style=\" color: lightgreen\"> &#10004;</span>\
        ";
    }
    else {
        out+="\
          <a href=\"http://narodmon.ru/\" target=\"_blank\"><b>Narodmon.com</b></a><span style=\" color: tomato\"> &#10006;</span>\
        ";
    }

    out+="<hr>\
		<b>Send Interval:</b><br>" + String(sendInterval) + " sec.<hr>\
		<a href=\"/services\">Отправка на сервисы</a><br><hr></div></center></body></html>\
    ";

    //Веб сервер
	
    server.send(200, "text/html", out);

    digitalWrite(led13, 0);
  //***************************
  }
}

String returnAddress(DeviceAddress deviceAddress) {
	String buf;
	buf += "#";
	
	for (uint8_t i = 0; i < 8; i++) { 
		// zero pad the address if necessary
		//if (tempDeviceAddress[i] < 16) buf = buf + "0";  
		buf += String(tempDeviceAddress[i], HEX);
	} // адрес датчика
	
	for(uint8_t i=0; buf[i]!=0; i++) {
		if(buf[i]<=122 && buf[i]>=97) buf[i]-=32;
	}
	
	return buf;
}

void handle_services() {
	
	digitalWrite(led13, 1);

	String out = "";

	out += base;
	
	out +="<meta http-equiv=\"REFRESH\" content=\"" + String(sendInterval) + "\">\
		</head><body><center><div class=\"blockk\"><a href=\"/\" class=\"caption\">ESP8266 Weather Station</a><br><hr>"; // Добавление обновления страницы в соответствии с интервалом отправки данных
		
	out +="<form action=\"/\" method=\"GET\">\
				<div style=\"display: inline-block; text-align:left\">\
				  <p>Thingspeak:</p>\
				  <p>Narodmon:</p>\
				  <p>IOT-Playground:</p><br>\
				  <label for=\"interval\">Sending period (sec):\
				</div>\
					<div style=\"display: inline-block\">\
						<input id=\"a1\" type=\"radio\" name=\"ts-send\" checked value=\"1\"><label for=\"a1\">Yes</label>\
						<input id=\"a0\" type=\"radio\" name=\"ts-send\" " + check_sending(!ts_send) + " value=\"0\"><label for=\"a0\">No</label><br>\
						<input id=\"b1\" type=\"radio\" name=\"nm-send\" checked value=\"1\"><label for=\"b1\">Yes</label>\
						<input id=\"b0\" type=\"radio\" name=\"nm-send\" " + check_sending(!nm_send) + " value=\"0\"><label for=\"b0\">No</label><br>\
						<input id=\"c1\" type=\"radio\" name=\"iot-send\" checked value=\"1\"><label for=\"c1\">Yes</label>\
						<input id=\"d0\" type=\"radio\" name=\"iot-send\" " + check_sending(true) + " value=\"0\"><label for=\"d0\">No</label><br><br>\
						<input id=\"interval\" name=\"send-period\" style=\"width: 30px;\" value=\"" + String(sendInterval) + "\"></input><br>\
					</div><br>\
				  <input type=\"submit\" id=\"\" value=\"Apply\"></input>\
			  </form>\
			  <hr><a href=\"/\">Back</a><hr></div>\
		  </center>\
		</body>\
	</html>\
	";

	server.send(200, "text/html", out);
	
	digitalWrite(led13, 0);
}

void thingspeak_send() {
	digitalWrite(led12, 1);
	
	//delay(1000);
	
	//Serial.print("connecting to ");
	//Serial.println(host);

	// Use WiFiClient class to create TCP connections
	WiFiClient client;
	const int httpPort = 80;
	if (!client.connect(host, httpPort)) {
		Serial.println("connection failed");
		digitalWrite(led12, 0);
		return;
	}

	Serial.println("connected -)");
	Serial.println("");
	// Создаем URI для запроса
	String url = "/update?key=";
	url += apikey;
	url += "&field1=";
	url += temp180;
	url += "&field2=";
	url += pressure;
	url += "&field3=";
	url += h;
	url += "&field4=";
	url += t;
	url += "&field5=";
	url += lux;

	
	Serial.print("Requesting URL: ");
	Serial.print(host);
	Serial.println(url);

	
	// отправляем запрос на сервер
	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
			   "Host: " + host + "\r\n" +
			   "Connection: close\r\n\r\n");
	client.flush(); // ждем отправки всех данных
	
	
	// Read all the lines of the reply from server and print them to Serial
	while(client.available()){
		String line = client.readStringUntil('\r');
		//char line = client.read();
		Serial.print(line);
	}

	digitalWrite(led12, 0);
	return;
}

void narodmon_send() {
	digitalWrite(led12, 1);
	
	//delay(1000);
	
	Serial.println("connecting to narodmon");
	
	WiFiClient client;
	String buf;
	buf = "#" + Hostname + "\r\n"; // заголовок
	buf += "#T1#" + String(temp180) + "\r\n";
	buf += "#P1#" + String(pressure) + "\r\n";
	buf += "#T2#" + String(t) + "\r\n";
	buf += "#H1#" + String(h) + "\r\n";
	buf += "#L1#" + String(lux) + "\r\n";
	buf += "##\r\n";
	
	if (!client.connect("narodmon.ru", 8283)) { // попытка подключения
      Serial.println("connection failed");
	  digitalWrite(led12, 0);
      return; // не удалось;
    } else
    {
      client.print(buf); // и отправляем данные
      while (client.available()) {
        String line = client.readStringUntil('\r'); // если что-то в ответ будет - все в Serial
        Serial.print(line);      }
    }
	
	digitalWrite(led12, 0);
    return; //ушло
}

void read_data(void) { // Reads data from ADC, bmp180, dht11/22, bht1750
	digitalWrite(led15, 1);

	raw = analogRead( pinPhoto );
	
	pressure = 0;
	temp180 = 0;

    if (bmp.begin()) {
		temp180 = bmp.readTemperature();
		pressure = (bmp.readPressure() / 100);
    }
	
	//***************************

	h = dht.readHumidity();
	// Read temperature as Celsius
	t = dht.readTemperature();
	hi = dht.computeHeatIndex(t, h, false);
	//float ti = ((hi-32)/2)+(((hi-32)/2)/10);

	if (isnan(t) || isnan(h) || isnan(hi)) {
		h = 0;
		t = 0;
		hi = 0;
	}
	
	lux = lightMeter.readLightLevel()/1.2;
	digitalWrite(led15, 0);
}

String check_sending(bool smth_send) { // Cтавит галочки в чекбоксах отправки данных
	if (smth_send) return "checked";
	return "";
}

void setup(void) {

	Serial.begin(115200);
	pinMode(led15, OUTPUT);
	pinMode(led13, OUTPUT);
	pinMode(led12, OUTPUT);
	digitalWrite(led15, 0);
	digitalWrite(led13, 0);
	digitalWrite(led12, 0);

	WiFi.begin ( ssid, password );
	while (WiFi.status() != WL_CONNECTED) {
	delay(500);
	Serial.print(".");
	}
	
	Hostname = "ESP" + WiFi.macAddress();
	Hostname.replace(":","");

	Serial.println("");
	Serial.println("Client mode");// Говорим что мы в режиме клиент
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	Serial.print("mac address: ");
	Serial.println(Hostname);

	if ( MDNS.begin ( "esp8266" ) ) {
		Serial.println ( "MDNS responder started" );
	}

	server.on("/", handle_root);
	server.on("/services", handle_services);
	/*server.on("/inline", []() {
	server.send(200, "text/plain", "this works as well");
	});*/
	server.begin();
	Serial.println("HTTP server started"); //Loooks like this is unnesessary

	Wire.pins(0, 2);// устанавливаем пины SDA,SCL для i2c

	if (!bmp.begin()) {
		Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	}

	dht.begin();
	
	//*********************
	
	sensors.begin(); //ds18b20
	NumberOfDevices = sensors.getDeviceCount(); //поищем.
	
	for (int i = 0; i < NumberOfDevices; i++) { 
		if (sensors.getAddress(tempDeviceAddress, i))
		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION); 
	} //настроим.
	
	//*********************
	
	lightMeter.begin();
	lightMeter.configure(BH1750_CONTINUOUS_LOW_RES_MODE);
	
		/*Wire.beginTransmission(BH1750_I2CADDR);
		Wire.write(BH1750_DOUBLE_MT_HB);
		Wire.endTransmission(); // Changing target sensor sensitivity to 2 times*/
		
		/*Wire.beginTransmission(BH1750_I2CADDR);
		Wire.write(BH1750_DOUBLE_MT_LB);
		Wire.endTransmission(); // Changing target sensor sensitivity to 2 times*/
	
	/* tickOccured = false;
	user_init(sendInterval*1000); */
	
}

void loop ( void ) {
	
	server.handleClient();
	
	if (tickOccured == true) {
		
		read_data();
		
		if (nm_send) {
			narodmon_send();
		}
		
		if (ts_send) {
			thingspeak_send();
		}
		
		tickOccured = false;
	}
	
}
