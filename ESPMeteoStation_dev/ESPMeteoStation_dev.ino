#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#include <DHT.h>
#define DHTPIN 14
#define DHTTYPE DHT11   // DHT 11/22

#include <Adafruit_BMP085.h>

const char *ssid = "PC-Woody";
const char *password = "DustMyBroom";

const char* host = "api.thingspeak.com";
const char* apikey = "YIR58CFT1SIPMUJ0"; // ключик от thingsspeak.com
String Hostname = "ESP18FE34D85AC2";

const int led15 = 15; //red
const int led13 = 13; //blue
const int led12 = 12; //green
const int pinPhoto = A0;

unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor

int raw = 0; // Photoresistor

float h; // Values for DHT11
float t;

float pressure; // values for BMP180
float temp180;

ESP8266WebServer server ( 80 );

Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE, 15);
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.

String base = "<!DOCTYPE html>\
        <head>\
          <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\
          <meta name=\"viewport\" content=\"width=400px\">\
          <title>Wi-Fi ESP8266 Метеостанция</title>\
          <style type=\"text/css\">body\{background-color: #7D8EE2;color:#FFF;}\
            a \{color: white;}\
            .blockk \{border:solid 1px #2d2d2d;\
                      text-align:center;background:#0059B3;padding:10px 10px 10px 10px;\
                      -moz-border-radius: 5px;-webkit-border-radius: 5px;border-radius: 5px;}"
            ".blockk{border:double 2px #000000;-moz-border-radius: 5px;-webkit-border-radius: 5px;border-radius: 5px;}"
            "</style><style type=\"text/css\" media=\'(min-width: 810px)\'>body\{font-size:18px;}.blockk \{width: 400px;}</style>"
            "<style type=\"text/css\" media=\'(max-width: 800px) and (orientation:landscape)\'>body\{font-size:8px;}</style><meta http-equiv=\"REFRESH\" content=\"600\"></head><body><center><div class=\"blockk\"><span style=\"font-size: 25px\">ESP8266 Weather Station</span><br><hr>";

bool ts_send  = false;
bool nm_send  = false;

void handle_root() {

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor
    previousMillis = currentMillis;

	if( server.hasArg("ts-send") ){
		if( strncmp(server.arg("ts-send").c_str(),"1",1) == 0 ) ts_send = true;
		else {
			if( strncmp(server.arg("ts-send").c_str(),"0",1) == 0 ) ts_send = false;
		}
	}
	
	if( server.hasArg("nm-send") ){
		if( strncmp(server.arg("nm-send").c_str(),"1",1) == 0 ) nm_send = true;
		else {
			if( strncmp(server.arg("nm-send").c_str(),"0",1) == 0 ) nm_send = false;
		}
	}

    raw = analogRead( pinPhoto );

  //***************************


    float pressure = 0;
    float temp180 = 0;

    if (bmp.begin()) {
      pressure = (bmp.readPressure() / 133.3);
      temp180 = (bmp.readTemperature());
    }
  //***************************
  digitalWrite(led15, 1);

  
  h = dht.readHumidity();
  // Read temperature as Celsius
  t = dht.readTemperature();
  //float hi = dht.computeHeatIndex(t, h, false);
  //float ti = ((hi-32)/2)+(((hi-32)/2)/10);

  if (isnan(t) || isnan(h)) {
    h = 0;
    t = 0;
  }

  digitalWrite(led15, 0);
  //***************************
  digitalWrite(led13, 1);

    String out = "";

    out += base;
    out +="<b>BMP180:</b><br>Температура: " + String(temp180) + " &deg;C.<br> Давление(атм.): " + String(pressure) + " мм.рт.ст.<br><hr><b>DHT11:</b><br>Температура: " + String(t) + " &deg;C.<br>Влажность (отн.): "+String(h)+" %.<br>Ощущаемая температура: "+"String(hi)"+" &deg;C.<br><hr><b>Фотодиод:</b><br>" + String(raw) + " /1024.<br><hr>";

    if( ts_send ){
        out+="\
          <a href=\"https://thingspeak.com/channels/110382\" target=\"_blank\"><b>Thingspeak.com</b></a><span> - Sending enabled</span><hr>\
        ";
    }
    else {
        out+="\
          <a href=\"https://thingspeak.com/channels/110382\" target=\"_blank\"><b>Thingspeak.com</b></a><span> - Sending disabled</span><hr>\
        ";
    }
	
	if( nm_send ){
        out+="\
          <a href=\"http://narodmon.ru/\" target=\"_blank\"><b>Narodmon.com</b></a><span> - Sending enabled</span>\
        ";
    }
    else {
        out+="\
          <a href=\"http://narodmon.ru/\" target=\"_blank\"><b>Narodmon.com</b></a><span> - Sending disabled</span>\
        ";
    }

    out+="\
      <hr><a href=\"/services\">Отправка на сервисы</a><br><hr></div></center></body></html>\
    ";

    //Веб сервер
    server.send(200, "text/html", out);

    digitalWrite(led13, 0);
  //***************************
	if (ts_send) {
		digitalWrite(led12, 1);

		thingspeak_send();

		digitalWrite(led12, 0);
		delay(200);
	}
	
	//***************************
	
	if (nm_send) {
		digitalWrite(led12, 1);

		narodmon_send();

		digitalWrite(led12, 0);
	}
	
  }
}

void handle_services() {

  String out = "";

  out += base;
  out +="<form action=\"/\" method=\"GET\">\
                <div style=\"display: inline-block; text-align:left\">\
                  <span>Thingspeak:</span><br>\
                  <span>Narodmon:</span><br>\
                  <span>Flymon:</span>\
                </div>\
                    <div style=\"display: inline-block\">\
                    <input id=\"a1\" type=\"radio\" name=\"ts-send\" value=\"1\"><label for=\"a1\">Yes</label>\
                    <input id=\"a0\" type=\"radio\" name=\"ts-send\" value=\"0\"><label for=\"a0\">No</label><br>\
                    <input id=\"b1\" type=\"radio\" name=\"nm-send\" value=\"1\"><label for=\"b1\">Yes</label>\
                    <input id=\"b0\" type=\"radio\" name=\"nm-send\" value=\"0\"><label for=\"b0\">No</label><br>\
                    <input id=\"c1\" type=\"radio\" name=\"fm-send\" value=\"1\"><label for=\"c1\">Yes</label>\
                    <input id=\"c0\" type=\"radio\" name=\"fm-send\" value=\"0\"><label for=\"c0\">No</label></div><br>\
                  <input type=\"submit\" id=\"\" value=\"Apply\"></input>\
              </form>\
              <hr><a href=\"/\">Back</a><hr></div>\
          </center>\
        </body>\
  </html>\
  ";

  server.send(200, "text/html", out);
}

bool thingspeak_send() {
	Serial.print("connecting to ");
	Serial.println(host);

	// Use WiFiClient class to create TCP connections
	WiFiClient client;
	const int httpPort = 80;
	if (!client.connect(host, httpPort)) {
		Serial.println("connection failed");
		return false;
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
	url += raw;

	Serial.print("Requesting URL: ");
	Serial.print(host);
	Serial.println(url);

	// отправляем запрос на сервер
	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
			   "Host: " + host + "\r\n" +
			   "Connection: close\r\n\r\n");
	client.flush(); // ждем отправки всех данных

	/*String lines = "";

	// Read all the lines of the reply from server and print them to Serial
	while (client.available()) {
	String line = client.readStringUntil('\r');
	lines += line + "<br>";
	//char line = client.read();
	Serial.print(line);
      }

      Serial.println();
      Serial.println("closing connection");
      Serial.println();*/
	  return true;
}

bool narodmon_send() {
	
	WiFiClient client;
	String buf;
	buf = "#" + Hostname + "\r\n"; // заголовок
	buf += "#T1#" + String(temp180) + "\r\n";
	buf += "#P1#" + String(pressure) + "\r\n";
	buf += "#T2#" + String(t) + "\r\n";
	buf += "#H1#" + String(h) + "\r\n";
	buf += "#L1#" + String(raw) + "\r\n";
	buf += "##\r\n";
	
	if (!client.connect("narodmon.ru", 8283)) { // попытка подключения
      Serial.println("connection failed");
      return false; // не удалось;
    } else
    {
      client.print(buf); // и отправляем данные
      while (client.available()) {
        String line = client.readStringUntil('\r'); // если что-то в ответ будет - все в Serial
        Serial.print(line);      }
    }
	
    return true; //ушло
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

  Serial.println("");
  Serial.println("Client mode");// Говорим что мы в режиме клиент
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("mac address: ");
  Serial.println(WiFi.macAddress());

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  server.on("/", handle_root);
  server.on("/services", handle_services);
  /*server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });*/
  server.begin();
  Serial.println("HTTP server started");

  Wire.pins(0, 2);// устанавливаем пины SDA,SCL для i2c

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

  dht.begin();

}

void loop ( void ) {

  server.handleClient();

}
