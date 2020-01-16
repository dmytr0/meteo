void checkBrightness() {
  #if(BRIGHT_CONTROL == 1) 
      if (analogRead(PHOTO) < BRIGHT_THRESHOLD) {   // если темно
        analogWrite(BACKLIGHT, BRIGHT_MIN);
          switchLedBrightness(false);
        } else {                                      // если светло
        analogWrite(BACKLIGHT, BRIGHT_MAX);
          switchLedBrightness(true);
      }
  #endif

  #if(HOURS_BRIGHT_CONTROL == 1) 
      int brightness = defineBrightness();
      analogWrite(BACKLIGHT, brightness);
      setLedBrightness(brightness);
  
   /* if(hrs >= RISE_HOUR && hrs < HOUR_TO_SLEEP){
    
      //analogWrite(BACKLIGHT, LCD_BRIGHT_MAX);
      //switchLedBrightness(true);
      
      int brightness = defineBrightness();
      analogWrite(BACKLIGHT, brightness);
      switchLedBrightness(true);
     
    }
    else if((hrs < RISE_HOUR || hrs >= HOUR_TO_SLEEP)) {
     
      //analogWrite(BACKLIGHT, LCD_BRIGHT_MIN);
      //switchLedBrightness(false);
    }*/
  #endif
  
}

int defineBrightness() {
  if(hrs >= RISE_HOUR && hrs < HOUR_TO_SLEEP){
    if(hrs == RISE_HOUR) {
      return map(mins, 0, 60, 0, 255);
    }
    
    return BRIGHT_MAX;
  } else if((hrs < RISE_HOUR || hrs >= HOUR_TO_SLEEP)) {
    if (hrs == HOUR_TO_SLEEP) {
      return map(mins, 0, 60, 255, 0);
    }
    return BRIGHT_MIN;
  }
}

void switchLedBrightness(boolean isOn) {
  if(isOn){
    setLedBrightness(BRIGHT_MAX);
  } else {
    setLedBrightness(BRIGHT_MIN);
    #if (LED_MODE == 0)
        LED_CURRENT_BRIGHTNESS = (BRIGHT_MIN);
    #else
        LED_CURRENT_BRIGHTNESS = (255 - BRIGHT_MIN);
    #endif
  }
}

void setLedBrightness(int b) {
  #if (LED_MODE == 0)
        LED_CURRENT_BRIGHTNESS = (b);
    #else
        LED_CURRENT_BRIGHTNESS = (255 - b);
    #endif
}


void modesTick() {
  button.tick();
  if (button.isClick()) {

    if ((LED_MODE == 0 && LED_CURRENT_BRIGHTNESS == BRIGHT_MIN) || (LED_MODE == 1 && LED_CURRENT_BRIGHTNESS == (255 - BRIGHT_MIN))){
        switchLedBrightness(true);
    }
    else {
        switchLedBrightness(false);
    }
    readSensors();
    drawSensors();
  
  }
}


void readSensors() {
  bme.takeForcedMeasurement();
  dispTemp = bme.readTemperature();
  dispHum = bme.readHumidity();
  dispPres = (float)bme.readPressure() * 0.00750062;
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

    for (byte i = 0; i < 5; i++) {                   // счётчик от 0 до 5 (да, до 5. Так как 4 меньше 5)
      pressure_array[i] = pressure_array[i + 1];     // сдвинуть массив давлений КРОМЕ ПОСЛЕДНЕЙ ЯЧЕЙКИ на шаг назад
    }
    pressure_array[5] = averPress;                    // последний элемент массива теперь - новое давление
    sumX = 0;
    sumY = 0;
    sumX2 = 0;
    sumXY = 0;
    for (int i = 0; i < 6; i++) {                    // для всех элементов массива
      sumX += time_array[i];
      sumY += (long)pressure_array[i];
      sumX2 += time_array[i] * time_array[i];
      sumXY += (long)time_array[i] * pressure_array[i];
    }
    a = 0;
    a = (long)6 * sumXY;             // расчёт коэффициента наклона приямой
    a = a - (long)sumX * sumY;
    a = (float)a / (6 * sumX2 - sumX * sumX);
    delta = a * 6;      // расчёт изменения давления
    dispRain = map(delta, -250, 250, 100, -100);  // пересчитать в проценты
    //Serial.println(String(pressure_array[5]) + " " + String(delta) + " " + String(dispRain));   // дебаг
  
}

void clockTick() {
    secs++;
    if (secs > 59) {      // каждую минуту
      secs = 0;
      mins++;
    }
    if (mins > 59) {      // каждый час
      now = rtc.now();
      secs = now.second();
      mins = now.minute();
      hrs = now.hour();
      
      if (hrs > 23) {
        hrs = 0;
      }
      #if (DISPLAY_TYPE == 1)
      drawData();
      #endif
    }
    #if (DISPLAY_TYPE == 1)
    drawClock();
    #else
    drawDateTime();
    #endif
}
