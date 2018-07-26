#include <ESP8266WiFi.h>

#include <DHT.h>
#define DHTPIN 2 // номер пина, к которому подсоединен датчик
// Раскоментируйте в соответствии с используемым датчиком
// Инициируем датчик
DHT dht(DHTPIN, DHT22);
//DHT dht(DHTPIN, DHT11);


const char* ssid = "PC-Woody"; //имя WIFI сети
const char* password = "DustMyBroom"; // пароль WIFI сети

const char* host = "api.thingspeak.com";
const char* apikey="YIR58CFT1SIPMUJ0"; // ключик от thingsspeak.com

int zaderjka = 40;// задержка в секундах


void setup() {
Serial.begin(115200);
dht.begin();
delay(100);

// подключаемся к WiFi сети

Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
}

int temperatura = 0;
long chislo = 0;


void loop() {
delay(zaderjka*1000);


dht.begin();
delay(1500);

// Считываем температуру
float temperatura = dht.readTemperature();
//Считываем влажность
float humidity = dht.readHumidity();
// Проверка удачно прошло ли считывание.
if (isnan(humidity) || isnan(temperatura)) {
Serial.println("Can't find DHT on PIN");
return;
}

Serial.print("Humidity: ");
Serial.print (humidity);
Serial.print ("% Temperature: ");
Serial.println (temperatura);

Serial.print("connecting to ");
Serial.println(host);

// Use WiFiClient class to create TCP connections
WiFiClient client;
const int httpPort = 80;
if (!client.connect(host, httpPort)) {
Serial.println("connection failed");
Serial.println();
Serial.println();
Serial.println();
return;
}
Serial.println("connected -)");
Serial.println("");
// Создаем URI для запроса
String url = "/update?key=";
url += apikey;
url += "&field1=";
url += temperatura;
url+="&field2=";
url+=humidity;

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

Serial.println();
Serial.println("closing connection");
Serial.println();
}