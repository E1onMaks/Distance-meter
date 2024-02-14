#define BTN_PIN 4
#define TRIG_PIN 7
#define ECHO_PIN 8

#include <iarduino_4LED.h>    // подключаем библиотеку для работы с четырёхразрядным LED индикатором
iarduino_4LED dispLED(10, 9); // объявляем объект для работы с функциями библиотеки iarduino_4LED, с указанием выводов дисплея (CLK, DIO). Можно указывать любые выводы Arduino

#include <EncButton.h>
Button btn(BTN_PIN);

float dist_3[3] = {0.0, 0.0, 0.0};  // массив для хранения трёх последних измерений
float middle, dist, dist_filtered, k;
byte i, delta;
bool flag = true;
unsigned long sensTimer;

void setup() {
  dispLED.begin();  // инициируем LED дисплей

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

float distance() {
  // отправка звукового сигнала
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // прием эхо-сигнала
  float tUs = pulseIn(ECHO_PIN, HIGH);  // мкс
  return tUs / 58.27;  // см/с при 20 °C
}

void loop() {
  btn.tick();
  if (btn.click()) flag = !flag;

  if (flag && millis() - sensTimer > 100) {
    if (i > 1) i = 0; // счётчик от 0 до 2 для фильтра последних 3х значений
    else i++;

    dist_3[i] = distance(); // получить расстояние в текущую ячейку массива
    dist = middle_of_3(dist_3[0], dist_3[1], dist_3[2]);  // фильтровать медианным фильтром из 3ёх последних измерений

    delta = abs(dist_filtered - dist);  // расчёт изменения с предыдущим
    if (delta > 1) k = 0.7;             // если большое - резкий коэффициент
    else k = 0.1;                       // если маленькое - плавный коэффициент

    dist_filtered = dist * k + dist_filtered * (1 - k); // фильтр "бегущее среднее"

    dispLED.print(dist_filtered, 1);  // выводим число с одним знаком после запятой.
    sensTimer = millis();
  }
}

// медианный фильтр из 3ёх значений
float middle_of_3(float a, float b, float c) {
  if ((a <= b) && (a <= c)) {
    middle = (b <= c) ? b : c;
  }
  else {
    if ((b <= a) && (b <= c)) {
      middle = (a <= c) ? a : c;
    }
    else {
      middle = (a <= b) ? a : b;
    }
  }
  return middle;
}