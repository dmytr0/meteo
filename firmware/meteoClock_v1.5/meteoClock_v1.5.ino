

// ------------------------- НАСТРОЙКИ --------------------
#define RESET_CLOCK 0           // сброс часов на время загрузки прошивки (для модуля с несъёмной батарейкой). Не забудь поставить 0 и прошить ещё раз!
#define SENS_TIME 30000         // время обновления показаний сенсоров на экране, миллисекунд
#define LED_MODE 0              // тип RGB светодиода: 0 - главный катод, 1 - главный анод

// управление яркостью
#define HOURS_BRIGHT_CONTROL 0  // 0/1 - запретить/разрешить управление яркостью по времения 
#define RISE_HOUR 7             // час включения подсветки дисплея
#define HOUR_TO_SLEEP 22        // час выключения подсветки дисплея

#define BRIGHT_CONTROL 0        // 0/1 - запретить/разрешить управление яркостью по датчику освещенности (при отключении яркость всегда будет макс.)
#define BRIGHT_THRESHOLD 255    // величина сигнала, ниже которой яркость переключится на минимум (0-1023)

#define BRIGHT_MAX 200          // макс яркость светодиода/индикатора и дисплея (0 - 255)
#define BRIGHT_MIN 0

#define DEBUG 0                 // вывод на дисплей лог инициализации датчиков при запуске. Для дисплея 1602 не работает! Но дублируется через порт!
#define CO2_SENSOR 1            // включить или выключить поддержку/вывод с датчика СО2 (1 вкл, 0 выкл)
#define DISPLAY_TYPE 0          // тип дисплея: 1 - 2004 (большой LCD), 0 - TFT 1.44 (маленький)
#define DISPLAY_ADDR 0x27       // адрес платы дисплея тип1 : 0x27 или 0x3f. Если дисплей не работает - смени адрес! На самом дисплее адрес не указан

#define INVERT_LEVEL 0          // инвертировать показания индикатора


// адрес BME280 жёстко задан в файле библиотеки Adafruit_BME280.h
// стоковый адрес был 0x77, у китайского модуля адрес 0x76.
// Так что если юзаете НЕ библиотеку из архива - не забудьте поменять

// если дисплей не заводится - поменяйте адрес (строка 54)

// пины
#define MHZ_RX 2
#define MHZ_TX 3
#define PHOTO A3                  // пин фоторезистора
#define BTN_PIN 4

#if (DISPLAY_TYPE == 1)
  #define BACKLIGHT 10            // пин подсветки дисплея

  #define LED_COM 7
  #define LED_R 9
  #define LED_G 6
  #define LED_B 5

#else
  #define BACKLIGHT 9             // пин подсветки дисплея
  #define __CS 10
  #define __DC 7
  #define __RES 8

  //пины для 74HC595
  #define L_L A0           //защелка           12 ST_CP 
  #define L_D A1           //вход данных       14 DS       
  #define L_C A2           //тактирование      11 SH_CP 
  #define L_PWM 6          //яркость (шим пин) 13 OE       
#endif

#if (DISPLAY_TYPE == 0)
  //разметка
  #define CO2_y 30
  #define T_y 55
  #define H_y 80
  #define R_y 105

  //цвета
  #define GREEN   0x07E0
  #define CYAN    0x07FF
  #define BLUE    0x001F
  #define RED     0xF800
  #define YELLOW  0xFFE0
  #define WHITE   0xFFFF
  #define MAGENTA 0xF81F

#endif

#if (INVERT_LEVEL == 0)
  #define L_DIRECTION LSBFIRST
#else
  #define L_DIRECTION MSBFIRST
#endif

// библиотеки
#include <Wire.h>

#if (DISPLAY_TYPE == 1)
  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(DISPLAY_ADDR, 20, 4);
#else
  #include <Adafruit_GFX.h>
  #include <TFT_ILI9163C.h>
  TFT_ILI9163C display = TFT_ILI9163C(__CS, __DC, __RES);
#endif

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

byte LED_CURRENT_BRIGHTNESS = 0;

// переменные для вывода
int8_t year, month, day, hrs, mins, secs;

float dispTemp;
byte dispHum;
int dispPres;
int dispCO2;
int dispRain;

//для прогноза
uint32_t pressure_array[6];
uint32_t sumX, sumY, sumX2, sumXY;
float a, b;
int delta;
///////////////

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


///////////////////////// SETUP //////////////////////////


void setup() {
  Serial.begin(115200);
  pinMode(BACKLIGHT, OUTPUT);

#if (DISPLAY_TYPE == 1)
  pinMode(LED_COM, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  setLEDinRGB(0, 0, 0);

  digitalWrite(LED_COM, LED_MODE);
  analogWrite(BACKLIGHT, BRIGHT_MAX);
  lcd.init();
  lcd.backlight();
  lcd.clear();
#else
  display.begin();
  display.setRotation(1);
  display.clearScreen();

  pinMode(L_L, OUTPUT);
  pinMode(L_C, OUTPUT);
  pinMode(L_D, OUTPUT);
  pinMode(L_PWM, OUTPUT);

  digitalWrite(L_L, LOW);                        // устанавливаем синхронизацию "защелки" на LOW
  analogWrite(BACKLIGHT, 125);
  analogWrite(L_PWM, 245);

#endif


#if (DEBUG == 1)
  boolean status = true;

  #if (DISPLAY_TYPE == 1)
    setLEDinRGB(255, 0, 0);
  #endif

  #if (CO2_SENSOR == 1)

    mhz19.begin(MHZ_TX, MHZ_RX);
    mhz19.setAutoCalibration(false);
    mhz19.getStatus();    // первый запрос, в любом случае возвращает -1
    delay(500);
    if (mhz19.getStatus() == 0) {
      printDebugMhz("OK");
    } else {
      printDebugMhz("ERROR");
      status = false;
    }
  #endif

  #if (DISPLAY_TYPE == 1)
    setLEDinRGB(0, 255, 0);
  #endif

  delay(50);
  if (rtc.begin()) {
    printDebugRtc("OK");
  } else {
    printDebugRtc("ERROR");
    status = false;
  }

  #if (DISPLAY_TYPE == 1)
    setLEDinRGB(0, 0, 255);
  #endif

  delay(50);
  if (bme.begin(&Wire)) {
    printDebugBme("OK");
  } else {
    printDebugBme("ERROR");
    status = false;
  }

  #if (DISPLAY_TYPE == 1)
    setLEDinRGB(255, 255, 255);
  #endif

  if (status) {
    printDebugFinal("All good");
  } else {
    printDebugFinal("ERROR...!");
  }
  delay(2000);

  #if (DISPLAY_TYPE == 1)
    setLEDinRGB(0, 0, 0);
  #endif

  while (1) {
    #if (DISPLAY_TYPE == 1)
        lcd.setCursor(14, 1);
        lcd.print("P:    ");
        lcd.setCursor(16, 1);
        lcd.print(analogRead(PHOTO), 1);

    #else
        display.setCursor(2, 20);
        display.print(analogRead(PHOTO));
    #endif

      Serial.println(analogRead(PHOTO));
      delay(300);
  }
#else //NOT DEBUG

  #if (CO2_SENSOR == 1)
    mhz19.begin(MHZ_TX, MHZ_RX);
    mhz19.setAutoCalibration(false);
    mhz19.getStatus();    // первый запрос, в любом случае возвращает -1
    delay(500);
  #endif

  rtc.begin();
  bme.begin(&Wire);
#endif

//END DEBUG

  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );

  if (RESET_CLOCK || rtc.lostPower()) {
    DateTime currentTime = DateTime(F(__DATE__), F(__TIME__));
    rtc.adjust(currentTime);
  }

  now = rtc.now();
  secs = now.second();
  mins = now.minute();
  hrs = now.hour();

  Serial.println(now.year());
  delay(100);
   Serial.println(now.month());
  delay(100);
   Serial.println(now.day());
  delay(100);
   Serial.println();
  delay(100);
   Serial.println();
  delay(100);

  bme.takeForcedMeasurement();
  uint32_t Pressure = bme.readPressure();

  for (byte i = 0; i < 6; i++) {   // счётчик от 0 до 5
    pressure_array[i] = Pressure;  // забить весь массив текущим давлением
    time_array[i] = i;             // забить массив времени числами 0 - 5
  }
  

  #if (DISPLAY_TYPE == 1)
    switchLedBrightness(true);
  #else
    display.clearScreen();

    //names
    display.setTextSize(2);
    display.setTextColor(MAGENTA);
    display.setCursor(2, CO2_y);
    display.print("CO2:");
    display.setCursor(2, T_y);
    display.print("Temp:");
    display.setCursor(2, H_y);
    display.print("Hum:");
    display.setCursor(2, R_y);
    display.print("Rain:");

    setIndicatorCO2(300);
  #endif

  drawDate();
  drawClock();
  readSensors();
  drawSensors();
}

///////////////////////// END SETUP //////////////////////////

///////////////////////// LOOP //////////////////////////

void loop() {
  if (brightTimer.isReady()) checkBrightness(); // яркость
  if (sensorsTimer.isReady()) readSensors();    // читаем показания датчиков с периодом SENS_TIME
  if (clockTimer.isReady()) clockTick();           // раз в секунду пересчитываем время и мигаем точками
  if (predictTimer.isReady()) rainPredict();      // 10 minutes timer for predict rain
  if (drawSensorsTimer.isReady()) drawSensors();  // обновляем показания датчиков на дисплее с периодом SENS_TIME

  modesTick();                                    // тут ловим нажатия на кнопку и переключаем режимы

}

///////////////////////// END LOOP //////////////////////////
