void checkBrightness() {
  #if(BRIGHT_CONTROL == 1)

    if(brightnessForced) {
      setDisplayBrightness(DISPLAY_BRIGHT_MAX);       // устанавливаем максимальную яркость дисплея
      setIndicatorBrightness(INDICATOR_BRIGHT_MAX);   // устанавливаем максимальную яркость индикатора

    } else {
      int roomIllumination = analogRead(PHOTO);
      roomIllumination = constrain(roomIllumination, BRIGHT_THRESHOLD, BRIGHT_HIGH_THRESHOLD);

      byte calculatedDisplayBrightness = map(roomIllumination, BRIGHT_THRESHOLD, BRIGHT_HIGH_THRESHOLD, DISPLAY_BRIGHT_MIN, DISPLAY_BRIGHT_MAX);
      byte calculatedIndicatorBrightness = map(roomIllumination, BRIGHT_THRESHOLD, BRIGHT_HIGH_THRESHOLD, INDICATOR_BRIGHT_MIN, INDICATOR_BRIGHT_MAX);

      // For Debug
      //Serial.println("roomIllumination " + String(roomIllumination));
      //Serial.println("calculatedDisplayBrightness " + String(calculatedDisplayBrightness));
      //Serial.println("calculatedIndicatorBrightness " + String(calculatedIndicatorBrightness));
      
      setDisplayBrightness(calculatedDisplayBrightness);       
      setIndicatorBrightness(calculatedIndicatorBrightness);   
    }
  #endif
}


void modesTick() {
  button.tick();
  if (button.isClick()) {
    // Обрабатываем нажатие кнопки включаем максимальную яркость дисплея и индикатора или отключаем этот режим

    if (brightnessForced) {
      stopForcedBrightness();
    } else {
      forceBrightness();
    }
    
    //readSensors();
    //drawSensors();

  }
}

void forceBrightness() {
  if(!brightnessForced) {
    brightnessForced = true;
    forcedBrightnessTimer.reset();
    forcedBrightnessTimer.start();
  }
}

void stopForcedBrightness() {
  brightnessForced = false;
  forcedBrightnessTimer.stop();
}


void readSensors() {
  bme.takeForcedMeasurement();

  dispTemp = bme.readTemperature();
  dispHum = bme.readHumidity();
  dispPres = (float)bme.readPressure() * 0.00750062;
  // dispAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  #if (CO2_SENSOR == 1)
    dispCO2 = mhz19.getPPM();
    setIndicatorCO2(dispCO2);
  #endif
}


void rainPredict() {

  // тут делаем линейную аппроксимацию для предсказания погоды
  long averPress = 0;
  for (byte i = 0; i < 10; i++) {
    bme.takeForcedMeasurement();
    averPress += bme.readPressure();
    delay(1);
  }
  averPress /= 10;

  for (byte i = 0; i < 5; i++) {                    // счётчик от 0 до 5 (да, до 5. Так как 4 меньше 5)
    pressure_array[i] = pressure_array[i + 1];      // сдвинуть массив давлений КРОМЕ ПОСЛЕДНЕЙ ЯЧЕЙКИ на шаг назад
  }
  pressure_array[5] = averPress;                    // последний элемент массива теперь - новое давление
  sumX = 0;
  sumY = 0;
  sumX2 = 0;
  sumXY = 0;
  for (int i = 0; i < 6; i++) {                     // для всех элементов массива
    sumX += time_array[i];
    sumY += (long)pressure_array[i];
    sumX2 += time_array[i] * time_array[i];
    sumXY += (long)time_array[i] * pressure_array[i];
  }
  a = 0;
  a = (long)6 * sumXY;                              // расчёт коэффициента наклона приямой
  a = a - (long)sumX * sumY;
  a = (float)a / (6 * sumX2 - sumX * sumX);
  delta = a * 6;      // расчёт изменения давления
  dispRain = map(delta, -250, 250, 100, -100);      // пересчитать в проценты

  // Дебаг
  //Serial.println(String(pressure_array[5]) + " " + String(delta) + " " + String(dispRain));   

}

void clockTick() {
  secs++;
  if (secs > 59) {      // каждую минуту
    secs = 0;
    mins++;
    drawClock();
  }
  if (mins > 59) {      // каждый час
    now = rtc.now();
    secs = now.second();
    mins = now.minute();
    hrs = now.hour();
    day = now.day();
    month = now.month();
    year = now.year();

    if (hrs > 23) {
      hrs = 0;
    }

    //drawDate();
  }

  //drawDate();
  //drawClock();
}
