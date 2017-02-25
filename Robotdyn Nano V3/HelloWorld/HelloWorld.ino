int val; // Задаем переменную val для отслеживания нажатия клавиши
int ledpin = 13; // задаем цифровой интерфейс ввода/вывода 13 - это наш светодиод

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200); // Задаем скорость обмена com-порта 9600
  pinMode (ledpin, OUTPUT); // Задаем ledpin = 13 как интерфейс вывода информации

}

void loop() {
  // put your main code here, to run repeatedly:
    val = Serial.read (); // Считываем команду посланную с компьютера через консоль IDE Arduino
    if (val == 'R') // Задаем букву условие на букву "R", при нажатии которой в консоли будет зажигался светодиод и появится строка "Hello World!"
  {
    digitalWrite (ledpin, HIGH); // Включаем светодиод на 13 выходе платы
    delay (500);
    digitalWrite (ledpin, LOW); // Выключаем светодиод на 13 выходе платы
    Serial.println ("Hello World!"); // Пишем в консоль "Hello World!"
  }
}
