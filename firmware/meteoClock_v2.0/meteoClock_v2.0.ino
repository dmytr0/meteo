

// ------------------------- НАСТРОЙКИ --------------------
#define RESET_CLOCK 0                 // сброс часов на время загрузки прошивки (для модуля с несъёмной батарейкой). Не забудь поставить 0 и прошить ещё раз!
#define SENS_TIME 30000               // время обновления показаний сенсоров на экране, миллисекунд
    
#define BRIGHT_CONTROL 1              // 0/1 - запретить/разрешить управление яркостью по датчику освещенности (при отключении яркость всегда будет макс.)
#define BRIGHT_THRESHOLD 40           // величина сигнала, ниже которой яркость переключится на минимум (0-1023)
#define BRIGHT_HIGH_THRESHOLD 600           // величина сигнала, ниже которой яркость переключится на минимум (0-1023)


#define BRIGHT_CHECK_PERIOD 2000      // Период проверки яркости в миллисекундах  
#define F_BRIGHTNESS_DURATION 20000   // Длительность принудительной подсветки в миллисекундах
    
#define DISPLAY_BRIGHT_MAX 255        // макс яркость дисплея (0 - 255)
#define DISPLAY_BRIGHT_MIN 0          // мин яркость дисплея (0 - 255)
    
#define INDICATOR_BRIGHT_MAX 0        // макс яркость индикатора (255 - 0) 0   - maximum
#define INDICATOR_BRIGHT_MIN 255      // мин яркость индикатора (255 - 0)  255 - minimum
    
#define DEBUG 0                       // вывод на дисплей лог инициализации датчиков при запуске.
#define CO2_SENSOR 1                  // включить или выключить поддержку/вывод с датчика СО2 (1 вкл, 0 выкл)
    
#define INVERT_LEVEL 0                // инвертировать показания индикатора


// адрес BME280 жёстко задан в файле библиотеки Adafruit_BME280.h
// стоковый адрес был 0x77, у китайского модуля адрес 0x76.
// Так что если юзаете НЕ библиотеку из архива - не забудьте поменять


// -------------------------  Пины  ------------------------- 
//CO2 датчика
#define MHZ_RX 2
#define MHZ_TX 3

#define PHOTO A3                      // пин фоторезистора
#define BTN_PIN 4                     // пин кнопки

//дисплей
#define BACKLIGHT 9                   // пин подсветки дисплея
#define __CS 10
#define __DC 7          
#define __RES 8

//пины для 74HC595
#define L_L A0                        //защелка           12 ST_CP 
#define L_D A1                        //вход данных       14 DS       
#define L_C A2                        //тактирование      11 SH_CP 
#define L_PWM 6                       //яркость (шим пин) 13 OE     




// ------------------------- Отображение  -------------------------  

//разметка 
#define CO2_y 30        // Положение строки показаний CO2
#define T_y 55          // Положение строки показаний Температуры
#define H_y 80          // Положение строки показаний Влажности
#define R_y 105         // Положение строки предсказаний дождя

//цвета (self descriptive)
#define GREEN   0x07E0
#define CYAN    0x07FF
#define BLUE    0x001F
#define RED     0xF800
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define MAGENTA 0xF81F
#define GREY    0x555555

// ------------------------- No need to change -------------------------

#define SEALEVELPRESSURE_HPA (1013.25)

#if (INVERT_LEVEL == 0)
  #define L_DIRECTION LSBFIRST
#else
  #define L_DIRECTION MSBFIRST
#endif

// библиотеки
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <GyverTimer.h>
#include "GyverButton.h"

#if (CO2_SENSOR == 1)
  #include <MHZ19_uart.h>
  MHZ19_uart mhz19;
#endif

TFT_ILI9163C display = TFT_ILI9163C(__CS, __DC, __RES);
RTC_DS3231 rtc;
DateTime now;
Adafruit_BME280 bme;
GButton button(BTN_PIN, LOW_PULL, NORM_OPEN);

// ------------------------- Timers -------------------------
GTimer_ms sensorsTimer(SENS_TIME);
GTimer_ms drawSensorsTimer(SENS_TIME);
GTimer_ms clockTimer(1000);
GTimer_ms predictTimer((long)10 * 60 * 1000);         // 10 минут
GTimer_ms brightTimer(BRIGHT_CHECK_PERIOD);
GTimer_ms forcedBrightnessTimer(F_BRIGHTNESS_DURATION);

// -------------------------------------------------- 


// ------------------------- Переменные определения яркости -------------------------
byte indicatorCurrBrightness = INDICATOR_BRIGHT_MAX;    // Текущая автоматическая яркость индикатора
byte displayCurrBrightness = DISPLAY_BRIGHT_MAX;        // Текущая автоматическая яркость дисплея
boolean brightnessForced = false;                       // Включена ли подсветка принудительно на максимум

// ------------------------- Переменные для вывода -------------------------
int8_t year, month, day, hrs, mins, secs;
float dispTemp;
byte dispHum;
int dispPres;
int dispCO2;
int dispRain;

// ------------------------- Переменные для прогноза  ------------------------- //
uint32_t pressure_array[6];
uint32_t sumX, sumY, sumX2, sumXY;
float a, b;
int delta;
byte time_array[6];
// ---- -----------------------------------------------------------------  ---- //


// ---- Дни недели ---- //
static const char *dayNames[]  = {
  "Sun",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat",
};


// -------------------------------------------------- SETUP -------------------------------------------------- //

void setup() {
  Serial.begin(115200);

  // Инициализация пинов
  pinMode(BACKLIGHT, OUTPUT);
  pinMode(L_L, OUTPUT);
  pinMode(L_C, OUTPUT);
  pinMode(L_D, OUTPUT);
  pinMode(L_PWM, OUTPUT);

  // Инициализация дисплея
  display.begin();
  display.setRotation(1);
  display.clearScreen();

  // Инициализация индикатора
  digitalWrite(L_L, LOW);                         // устанавливаем синхронизацию "защелки" на LOW
  setDisplayBrightness(DISPLAY_BRIGHT_MAX);       // устанавливаем максимальную яркость дисплея
  setIndicatorBrightness(INDICATOR_BRIGHT_MAX);   // устанавливаем максимальную яркость индикатора


  // Инициализация CO2 датчика
  #if (CO2_SENSOR == 1)
    mhz19.begin(MHZ_TX, MHZ_RX);
    mhz19.setAutoCalibration(false);
    mhz19.getStatus();                           // первый запрос, в любом случае возвращает -1
    delay(500);
  #endif

  // -------- START DEBUG BLOCK --------- //
  #if (DEBUG == 1)
    boolean status = true;

    #if (CO2_SENSOR == 1)
      if (mhz19.getStatus() == 0) {
        printDebugMhz("OK");
      } else {
        printDebugMhz("ERROR");
        status = false;
      }
    #endif

    delay(50);

    if (rtc.begin()) {
      printDebugRtc("OK");
    } else {
      printDebugRtc("ERROR");
      status = false;
    }

    delay(50);

    if (bme.begin(&Wire)) {
      printDebugBme("OK");
    } else {
      printDebugBme("ERROR");
      status = false;
    }

    delay(50);

    if (status) {
      printDebugFinal("All good");
    } else {
      printDebugFinal("ERROR...!");
    }
    
  #else // ------------ END DEBUG ------------ //
    rtc.begin();
    bme.begin(&Wire);
  #endif

  // Инициализация BME 280
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );

  // Настройка RTC
  if (RESET_CLOCK || rtc.lostPower()) {
    DateTime currentTime = DateTime(F(__DATE__), F(__TIME__));
    rtc.adjust(currentTime);
  }

  // ------------ Инициализация переменных ------------ //

  // Время
  now = rtc.now();
  secs = now.second();
  mins = now.minute();
  hrs = now.hour();
  day = now.day();
  month = now.month();
  year = now.year();

  // BME280
  bme.takeForcedMeasurement();
  uint32_t Pressure = bme.readPressure();

  // Для прогноза
  for (byte i = 0; i < 6; i++) {   // счётчик от 0 до 5
    pressure_array[i] = Pressure;  // забить весь массив текущим давлением
    time_array[i] = i;             // забить массив времени числами 0 - 5
  }
  

  // ------------ Чтение и отрисовка данных ------------ //

  drawSensorsGrid();  

  readSensors();

  //drawDate();
  drawClock();
  drawSensors();
}

// ------------------------- END SETUP ------------------------- //



// -------------------------------------------------- LOOP -------------------------------------------------- //

void loop() {
  if (brightTimer.isReady()) checkBrightness();                 // яркость
  if (sensorsTimer.isReady()) readSensors();                    // читаем показания датчиков с периодом SENS_TIME
  if (clockTimer.isReady()) clockTick();                        // раз в секунду пересчитываем время и мигаем точками
  if (predictTimer.isReady()) rainPredict();                    // 10 минут прогноз дождя
  if (drawSensorsTimer.isReady()) drawSensors();                // обновляем показания датчиков на дисплее с периодом SENS_TIME
  if (forcedBrightnessTimer.isReady()) stopForcedBrightness();  // выключаем принудительную подсветку

  modesTick();                                      // ловим нажатия на кнопку и форсируем включение дисплея
}

// ------------------------- END LOOP ------------------------- //
