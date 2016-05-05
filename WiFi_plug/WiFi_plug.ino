 /**
* WiFi розетка. Версия 1.0
* Copyright (C) 2015 http://samopal.pro
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// GPIO, куда подцелено реле
uint8_t PowerPin = 2;
bool    PowerOn  = false;

// Имя хоста
const char* host = "esp8266-power";
// Вместо звездочек свои параметры WiFi сети
const char* ssid = "PC-Woody";
const char* password = "DustMyBroom";

ESP8266WebServer server(80);

void setup() {
// Последовательный порт для отладки
  Serial.begin(115200);
  Serial.printf("\n\nFree memory %d\n",ESP.getFreeHeap());

  // Инициализация выхода реле
  pinMode(PowerPin , OUTPUT);
  digitalWrite(PowerPin , PowerOn);

// Подлючение к WiFi

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  if(WiFi.waitForConnectResult() == WL_CONNECTED){
// Запуск WEB-сервера
     MDNS.begin(host);
     server.on ( "/", HTTP_handleRoot );
     server.onNotFound ( HTTP_handleRoot );
     server.begin();
     Serial.printf( "HTTP server ok! Open http://%s.local in your browser\n", host);
     Serial.print("WiFi connected, using IP address: ");
     Serial.println(WiFi.localIP());
  }
  else {
     Serial.printf("WiFi started is fail");
  }

}


void loop() {
  server.handleClient();
   delay(50);
}


/*
 * Оработчик главной страницы сервера
 */
void HTTP_handleRoot(void) {
  bool stat = false;

  if( server.hasArg("stat") ){
     if( strncmp(server.arg("stat").c_str(),"1",1) == 0 )stat = true;
  }
  else {
     stat = PowerOn;
  }

  String out = "";

  out =
"<html>\
  <head>\
    <meta charset=\"utf-8\" />\
    <title>WiFi розетка</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>WiFi розетка. Версия 1.0</h1>\n";

  if( stat ){
      out+="\
    <h2>Состояние: Включено</br>\
    <a href=\"/?stat=0\">Выключить</a></h2>\
    ";
  }
  else {
      out+="\
    <h2>Состояние: Выключено</br>\
    <a href=\"/?stat=1\">Включить</a><h2>\
    ";
  }
   out+= "\
  </body>\
</html>";
   server.send ( 200, "text/html", out );
   if( stat != PowerOn ){
      PowerOn = stat;
      digitalWrite(PowerPin , PowerOn);
      if( PowerOn )Serial.println("Power is ON");
      else Serial.println("Power is OFF");
   }

}
