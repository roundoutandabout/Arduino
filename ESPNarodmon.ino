// Простой пример снятия температуры с ds18b20 и отправка его на сайт народного мониторинга. http://narodmon.ru
 
#include <ESP8266WiFi.h>        
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <OneWire.h>
#include <DallasTemperature.h>
 
 
#define debug true // вывод отладочных сообщений
#define ONE_WIRE_BUS 2 // GPIO к которому подключен DS18B20
#define TEMPERATURE_PRECISION 12 // точность бит. Если глючит или врет, уменьшить до 9
#define postingInterval  300000 // интервал между отправками данных в миллисекундах (5 минут)
 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
unsigned long lastConnectionTime = 0;           // время последней передачи данных
int NumberOfDevices; //сколько датчиков найдем.
String Hostname; //имя железки - выглядит как ESPAABBCCDDEEFF т.е. ESP+mac адрес.
 
void wifimanstart() { // Волшебная процедура начального подключения к Wifi.
                      // Если не знает к чему подцепить - создает точку доступа ESP8266 и настроечную таблицу http://192.168.4.1
                      // Подробнее: https://github.com/tzapu/WiFiManager
  WiFiManager wifiManager;
  wifiManager.setDebugOutput(debug);
  wifiManager.setMinimumSignalQuality();
  if (!wifiManager.autoConnect("ESP8266")) {
  if (debug) Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000); }
if (debug) Serial.println("connected...");
}
 
void setup() {
  DeviceAddress tempDeviceAddress;
  Hostname = "ESP"+WiFi.macAddress();
  Hostname.replace(":","");
  Serial.begin(115200);
  sensors.begin(); //ds18b20
  NumberOfDevices = sensors.getDeviceCount(); //поищем.
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.print(" devices. ");
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON"); else Serial.println("OFF");
  for (int i = 0; i < NumberOfDevices; i++) { if (sensors.getAddress(tempDeviceAddress, i)) sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION); }//настроим.
  WiFi.hostname(Hostname);
  wifimanstart();
  Serial.println(WiFi.localIP()); Serial.println(WiFi.macAddress()); Serial.print(" Narodmon ID: "); Serial.println(Hostname);
  lastConnectionTime = millis() - postingInterval + 15000; //первая передача на народный мониторинг через 15 сек.
}
 
  bool SendToNarodmon() { // Собственно формирование пакета и отправка.
    WiFiClient client;
    DeviceAddress tempDeviceAddress;
    String buf;
    buf = "#" + Hostname + "\r\n"; // заголовок
    NumberOfDevices = sensors.getDeviceCount(); //количество датчиков ?
    sensors.requestTemperatures(); //Начали измерение
    delay(20);
    for (int i = 0; i < NumberOfDevices; i++)  { //перечисляем датчики и их показания
      sensors.getAddress(tempDeviceAddress, i);
        buf = buf + "#";
        for (uint8_t i = 0; i < 8; i++) { if (tempDeviceAddress[i] < 16) buf = buf + "0"; buf = buf + String(tempDeviceAddress[i], HEX); } // адрес датчика
        buf = buf + "#" + String(sensors.getTempCByIndex(i)) + "\r\n"; //и температура
       }
    buf = buf + "##\r\n"; // закрываем пакет
 
    if (!client.connect("narodmon.ru", 8283)) { // попытка подключения
      Serial.println("connection failed");
      return false; // не удалось;
    } else
    {
      client.print(buf); // и отправляем данные
      if (debug) Serial.print(buf);
      while (client.available()) {
        String line = client.readStringUntil('\r'); // если что-то в ответ будет - все в Serial
        Serial.print(line);      }
    }
      return true; //ушло
  }
 
  void loop() {
    if (millis() - lastConnectionTime > postingInterval) { // ждем 5 минут и отправляем
      if (WiFi.status() == WL_CONNECTED) { // ну конечно если подключены
      if (SendToNarodmon()) {
      lastConnectionTime = millis();
      }else{  lastConnectionTime = millis() - postingInterval + 15000; }//следующая попытка через 15 сек    
      }else{  lastConnectionTime = millis() - postingInterval + 15000; Serial.println("Not connected to WiFi");}//следующая попытка через 15 сек
    }  yield(); // что за команда - фиг знает, но ESP работает с ней стабильно и не глючит.
  }