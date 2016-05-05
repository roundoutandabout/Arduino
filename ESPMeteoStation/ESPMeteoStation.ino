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

const int led15 = 15; //red
const int led13 = 13; //blue
const int led12 = 12; //green
const int pinPhoto = A0;
int raw = 0;

unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor

Adafruit_BMP085 bmp;
ESP8266WebServer server ( 80 );

DHT dht(DHTPIN, DHTTYPE, 15);
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.

void handle_root() {

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor
    previousMillis = currentMillis;

    bool send  = false;

      if( server.hasArg("send") ){
         if( strncmp(server.arg("send").c_str(),"1",1) == 0 ) send = true;
      }
      else {
        send = false;
      }

    raw = analogRead( pinPhoto );

  //***************************

    /*Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");*/

    /*Serial.print("Pressure = ");
    Serial.print(bmp.readPressure() / 133.3);
    Serial.println(" mm");*/

    float pressure = (bmp.readPressure() / 133.3);
    float temp180 = (bmp.readTemperature());

  //***************************
  digitalWrite(led15, 1);

  float h;
  float t;

  do
  {
    delay(50);
    h = dht.readHumidity();
    // Read temperature as Celsius
    t = dht.readTemperature();
    //float hi = dht.computeHeatIndex(t, h, false);
  } while(isnan(h) || isnan(t));

    /*if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }*/

    /*float ti = ((hi-32)/2)+(((hi-32)/2)/10);*/

  digitalWrite(led15, 0);
  //***************************
  digitalWrite(led13, 1);

    String out = "";

    out = "<!DOCTYPE html>\
            <head>\
              <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\
              <meta name=\"viewport\" content=\"width=400px\">\
              <title>Wi-Fi ESP8266 Метеостанция</title>\
              <style type=\"text/css\">body\{background-color: #7D8EE2;color:#FFF;}\
                a \{color:#73B9FF;}\
                .blockk \{border:solid 1px #2d2d2d;\
                          text-align:center;background:#0059B3;padding:10px 10px 10px 10px;\
                          -moz-border-radius: 5px;-webkit-border-radius: 5px;border-radius: 5px;}"
                ".blockk{border:double 2px #000000;-moz-border-radius: 5px;-webkit-border-radius: 5px;border-radius: 5px;}"
                "</style><style type=\"text/css\" media=\'(min-width: 810px)\'>body\{font-size:18px;}.blockk \{width: 400px;}</style>"
                "<style type=\"text/css\" media=\'(max-width: 800px) and (orientation:landscape)\'>body\{font-size:8px;}</style><meta http-equiv=\"REFRESH\" content=\"60\"></head><body><center><div class=\"blockk\"><span style=\"font-size: 25px\">Мини Метеостанция на ESP8266</span><br><hr><b>BMP180:</b><br>Температура: " + String(temp180) + " &deg;C.<br> Давление(атм.): " + String(pressure) + " мм.рт.ст.<br><hr><b>DHT11:</b><br>Температура: " + String(t) + " &deg;C. Влажность (отн.): "+String(h)+" %.<br>Ощущаемая температура: "+"String(hi)"+" &deg;C.<br><hr><b>Фотодиод:</b><br>" + String(raw) + " /1024.<br><hr><a href=\"https://thingspeak.com/channels/110382\" target=\"_blank\" style=\"color: white\"><b>Thingspeak.com</b></a><br>\n";

    if( send ){
        out+="\
          <a href=\"/?send=0\"><button>Отключить передачу</button></a>\
        ";
    }
    else {
        out+="\
          <a href=\"/?send=1\"><button>Включить передачу</button></a>\
        ";
    }

    out+="\
      <br><hr></div></center></body></html>\
    ";

    //Веб сервер
    server.send(200, "text/html", out);

    digitalWrite(led13, 0);
  //***************************
  if (send) {
      digitalWrite(led12, 1);

      Serial.print("connecting to ");
      Serial.println(host);

      // Use WiFiClient class to create TCP connections
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
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
      url += raw;

      Serial.print("Requesting URL: ");
      Serial.print(host);
      Serial.println(url);

      // отправляем запрос на сервер
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
      client.flush(); // ждем отправки всех данных

      String lines = "";

      // Read all the lines of the reply from server and print them to Serial
      /*while (client.available()) {
        String line = client.readStringUntil('\r');
        lines += line + "<br>";
        //char line = client.read();
        Serial.print(line);
      }*/

      Serial.println();
      Serial.println("closing connection");
      Serial.println();

      digitalWrite(led12, 0);
    }
  //***************************
  }
}

void setup(void)
{
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

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  server.on("/", handle_root);
  /*server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });*/
  server.begin();
  Serial.println("HTTP server started");

  Wire.pins(0, 2);// устанавливаем пины SDA,SCL для i2c

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

}

void loop ( void ) {

  server.handleClient();

}
