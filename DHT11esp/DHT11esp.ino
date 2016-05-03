

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#include <DHT.h>
#define DHTPIN 12

#define DHTTYPE DHT11   // DHT 11/22
DHT dht(DHTPIN, DHTTYPE, 15);

const char *ssid = "PC-Woody";
const char *password = "DustMyBroom";
const int led = 13;
const int pinPhoto = A0;
      int raw = 0;


ESP8266WebServer server ( 80 );

void handle_root() {

  raw = analogRead( pinPhoto );

  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  float hi = dht.computeHeatIndex(f, h);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  /*Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
    Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");*/
  float ti = ((hi-32)/2)+(((hi-32)/2)/10);


  digitalWrite(led, 1);
  //Веб сервер
  server.send(200, "text/html", "<!DOCTYPE html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><meta name=\"viewport\" content=\"width=400px\"><title>Wi-Fi ESP8266 Метеостанция</title>"
  "<style type=\"text/css\">body\{background-color: #7D8EE2;color:#FFF;}"
  "a {color:#73B9FF;}"
  ".blockk {border:solid 1px #2d2d2d;text-align:center;background:#0059B3;padding:10px 10px 10px 10px;-moz-border-radius: 5px;-webkit-border-radius: 5px;border-radius: 5px;}"
  ".blockk{border:double 2px #000000;-moz-border-radius: 5px;-webkit-border-radius: 5px;border-radius: 5px;}"
  "</style><style type=\"text/css\" media=\'(min-width: 810px)\'>body\{font-size:18px;}.blockk \{width: 400px;}</style>"
  "<style type=\"text/css\" media=\'(max-width: 800px) and (orientation:landscape)\'>body\{font-size:8px;}</style><meta http-equiv=\"REFRESH\" content=\"60\"></head><body><center><div class=\"blockk\">Мини Метеостанция на ESP8266<br><hr><b>DHT22:</b><br>Температура: "+String(t)+" &deg;C. Влажность (отн.): "+String(h)+" %.<br>Ощущаемая температура: "+String(ti)+" &deg;C.<br><hr><b>BMP180:</b><br><hr><b>Фотодиод:</b><br>" + String(raw) + " /1024.<br><hr></div></center></body></html>");

  digitalWrite(led, 0);

}

void setup(void)
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);


  Serial.begin(115200);
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

    server.on("/", handle_root);
    server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });
  server.begin();
  Serial.println("HTTP server started");

  Serial.println("DHT11 test!");
    dht.begin();

}

void loop ( void ) {
	server.handleClient();
}
