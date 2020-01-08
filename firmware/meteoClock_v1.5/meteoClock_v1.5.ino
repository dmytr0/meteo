

// ------------------------- НАСТРОЙКИ --------------------
#define RESET_CLOCK 0       // сброс часов на время загрузки прошивки (для модуля с несъёмной батарейкой). Не забудь поставить 0 и прошить ещё раз!
#define SENS_TIME 30000     // время обновления показаний сенсоров на экране, миллисекунд
#define LED_MODE 0          // тип RGB светодиода: 0 - главный катод, 1 - главный анод

// управление яркостью
#define HOURS_BRIGHT_CONTROL 1  // 0/1 - запретить/разрешить управление яркостью по времения 
#define RISE_HOUR 7             // час включения подсветки дисплея
#define HOUR_TO_SLEEP 22        // час выключения подсветки дисплея
#define BRIGHT_CONTROL 0        // 0/1 - запретить/разрешить управление яркостью по датчику освещенности (при отключении яркость всегда будет макс.)
#define BRIGHT_THRESHOLD 255    // величина сигнала, ниже которой яркость переключится на минимум (0-1023)
#define LED_BRIGHT_MAX 255      // макс яркость светодиода СО2 (0 - 255)
#define LED_BRIGHT_MIN 0        // мин яркость светодиода СО2 (0 - 255)
#define LCD_BRIGHT_MAX 200      // макс яркость подсветки дисплея (0 - 255)
#define LCD_BRIGHT_MIN 0        // мин яркость подсветки дисплея (0 - 255)

#define DEBUG 0             // вывод на дисплей лог инициализации датчиков при запуске. Для дисплея 1602 не работает! Но дублируется через порт!
#define CO2_SENSOR 1        // включить или выключить поддержку/вывод с датчика СО2 (1 вкл, 0 выкл)
#define DISPLAY_TYPE 1      // тип дисплея: 1 - 2004 (большой), 0 - 1602 (маленький)
#define DISPLAY_ADDR 0x27   // адрес платы дисплея: 0x27 или 0x3f. Если дисплей не работает - смени адрес! На самом дисплее адрес не указан


// адрес BME280 жёстко задан в файле библиотеки Adafruit_BME280.h
// стоковый адрес был 0x77, у китайского модуля адрес 0x76.
// Так что если юзаете НЕ библиотеку из архива - не забудьте поменять

// если дисплей не заводится - поменяйте адрес (строка 54)

// пины
#define BACKLIGHT 10     // пин подсветки дисплея
#define PHOTO A3         // пин фоторезистора

#define MHZ_RX 2
#define MHZ_TX 3

#define LED_COM 7
#define LED_R 9
#define LED_G 6
#define LED_B 5
#define BTN_PIN 4

// библиотеки
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(DISPLAY_ADDR, 20, 4);

#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;

#if (CO2_SENSOR == 1)
#include <MHZ19_uart.h>
MHZ19_uart mhz19;
#endif

#include <GyverTimer.h>
GTimer_ms sensorsTimer(SENS_TIME);
GTimer_ms drawSensorsTimer(SENS_TIME);
GTimer_ms clockTimer(1000);
GTimer_ms predictTimer((long)10 * 60 * 1000);         // 10 минут
GTimer_ms brightTimer(2000);

#include "GyverButton.h"
GButton button(BTN_PIN, LOW_PULL, NORM_OPEN);

int8_t hrs, mins, secs;
byte LED_CURRENT_BRIGHTNESS = 0;

// переменные для вывода
float dispTemp;
byte dispHum;
int dispPres;
int dispCO2;
int dispRain;

// массивы графиков
int tempHour[15], tempDay[15];
int humHour[15], humDay[15];
int pressHour[15], pressDay[15];
int co2Hour[15], co2Day[15];
int delta;
uint32_t pressure_array[6];
uint32_t sumX, sumY, sumX2, sumXY;
float a, b;
byte time_array[6];


static const char *dayNames[]  = {
  "Sun",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat",
};

void drawData() {
  int dayofweek = now.dayOfTheWeek();
  lcd.setCursor(10, 0);
  lcd.print(dayNames[dayofweek]);
  lcd.print("  ");

  if (now.day() < 10) lcd.print(0);
  lcd.print(now.day());
  lcd.print(".");
  if (now.month() < 10) lcd.print(0);
  lcd.print(now.month());
 
}

void drawClock() {
  boolean dotState = (secs % 2) == 0;
  
  lcd.setCursor(0, 0);
  if(hrs < 10) lcd.print(0);
  lcd.print(hrs); 
  
  if(dotState) lcd.print(":");
  else lcd.print(" ");

  if(mins < 10) lcd.print(0);
  lcd.print(mins);

  if(dotState) lcd.print(":");
  else lcd.print(" ");
  
  if(secs < 10) lcd.print(0);
  lcd.print(secs);
}




void calculateAndSetLedCO2(int ppm) {
  int red = constrain(ppm, 800, 1000);
  red = map(red, 800, 1000, 0, 255);

  int green = constrain(ppm, 1000, 1200);
  green = map(green, 1000, 1200, 255, 0);

  setLEDinRGB(red, green, 0);
}

void setLEDinRGB(byte R, byte G, byte B) {
  setOneLed(LED_R, R);
  setOneLed(LED_G, G);
  setOneLed(LED_B, B);
}

void setOneLed(byte pinNumber, byte value) {
  value = constrain(value, 0, 255);
  value = map(value, 0, 255, 0, LED_CURRENT_BRIGHTNESS);
  analogWrite(pinNumber, value);
}


void setup() {
  Serial.begin(9600);

  pinMode(BACKLIGHT, OUTPUT);
  pinMode(LED_COM, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  setLEDinRGB(0, 0, 0);

  digitalWrite(LED_COM, LED_MODE);
  analogWrite(BACKLIGHT, LCD_BRIGHT_MAX);

  lcd.init();
  lcd.backlight();
  lcd.clear();

#if (DEBUG == 1)
  boolean status = true;

  setLEDinRGB(255, 0, 0);

#if (CO2_SENSOR == 1)
  lcd.setCursor(0, 0);
  lcd.print(F("MHZ-19... "));
  Serial.print(F("MHZ-19... "));
  mhz19.begin(MHZ_TX, MHZ_RX);
  mhz19.setAutoCalibration(false);
  mhz19.getStatus();    // первый запрос, в любом случае возвращает -1
  delay(500);
  if (mhz19.getStatus() == 0) {
    lcd.print(F("OK"));
    Serial.println(F("OK"));
  } else {
    lcd.print(F("ERROR"));
    Serial.println(F("ERROR"));
    status = false;
  }
#endif

  setLEDinRGB(0, 255, 0);
  lcd.setCursor(0, 1);
  lcd.print(F("RTC... "));
  Serial.print(F("RTC... "));
  delay(50);
  if (rtc.begin()) {
    lcd.print(F("OK"));
    Serial.println(F("OK"));
  } else {
    lcd.print(F("ERROR"));
    Serial.println(F("ERROR"));
    status = false;
  }

  setLEDinRGB(0, 0, 255);
  lcd.setCursor(0, 2);
  lcd.print(F("BME280... "));
  Serial.print(F("BME280... "));
  delay(50);
  if (bme.begin(&Wire)) {
    lcd.print(F("OK"));
    Serial.println(F("OK"));
  } else {
    lcd.print(F("ERROR"));
    Serial.println(F("ERROR"));
    status = false;
  }

  setLEDinRGB(0, 0, 0);
  lcd.setCursor(0, 3);
  if (status) {
    lcd.print(F("All good"));
    Serial.println(F("All good"));
  } else {
    lcd.print(F("Upss...!"));
    Serial.println(F("Check wires!"));
  }
  while (1) {
    lcd.setCursor(14, 1);
    lcd.print("P:    ");
    lcd.setCursor(16, 1);
    lcd.print(analogRead(PHOTO), 1);
    Serial.println(analogRead(PHOTO));
    delay(300);
  }
#else

#if (CO2_SENSOR == 1)
  mhz19.begin(MHZ_TX, MHZ_RX);
  mhz19.setAutoCalibration(false);
#endif
  rtc.begin();
  bme.begin(&Wire);
#endif

  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );

  if (RESET_CLOCK || rtc.lostPower())
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  now = rtc.now();
  secs = now.second();
  mins = now.minute();
  hrs = now.hour();

  bme.takeForcedMeasurement();
  uint32_t Pressure = bme.readPressure();
  for (byte i = 0; i < 6; i++) {   // счётчик от 0 до 5
    pressure_array[i] = Pressure;  // забить весь массив текущим давлением
    time_array[i] = i;             // забить массив времени числами 0 - 5
  }

  switchLedBrightness(true);
  drawClock();
  drawData();
  readSensors();
  drawSensors();
}

void loop() {
if (brightTimer.isReady()) checkBrightness(); // яркость
if (sensorsTimer.isReady()) readSensors();    // читаем показания датчиков с периодом SENS_TIME
if (clockTimer.isReady()) clockTick();           // раз в секунду пересчитываем время и мигаем точками
if (predictTimer.isReady()) rainPredict();      // 10 minutes timer for predict rain
if (drawSensorsTimer.isReady()) drawSensors();  // обновляем показания датчиков на дисплее с периодом SENS_TIME

modesTick();                                    // тут ловим нажатия на кнопку и переключаем режимы
}
