void setDisplayBrightness(byte brightness) {
  analogWrite(BACKLIGHT, brightness);
}

// ------------------------- Отображения статики ------------------------- //

void drawSensorsGrid() {
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
}


// ------------------------- DATE TIME ------------------------- //
void drawDate() {
  clearDate();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5, 5);

  String date = "";
  if (day < 10) date += 0;
  date += day + ".";
  if (month < 10) date += 0;
  date += month + ".";
  date += year + " ";

  display.println(date);
}

void drawClock() {
  // Переменная может быть использована для моргания двоеточием раз в две секунды
  //boolean dotState = (secs % 2) == 0; 
  
  clearTime();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(95, 5);

   //TIME
  String dateTime = "";
  if (hrs < 10) dateTime += 0;
  dateTime += hrs;
  if (dotState) dateTime += ":";
  else dateTime += " ";
  if (mins < 10) dateTime += 0;
  dateTime += mins;

  // -- time without seconds if need: 
  //1) Change 'display.setCursor(95, 5);' to 'display.setCursor(80, 5);'  
  //2) In method 'clockTick()' uncomment drackClock() at the bottom

  //dateTime += ":";
  // if (secs < 10) dateTime += 0;
  // dateTime += secs;

  display.println(dateTime);
}


// ------------------------- SENSORS ------------------------- //
void drawSensors() {
  clearSensors();
  
  // print values
  display.setTextSize(2);
  printCO2();
  printTemp();
  printHum();
  printRain();
}


// ------ print CO2 value ------ //
void printCO2() {
  display.setCursor(80, CO2_y);
  #if (CO2_SENSOR == 1)
    if (dispCO2 < 800) {
      display.setTextColor(CYAN);
    } else if (dispCO2 < 1000) {
      display.setTextColor(YELLOW);
    } else {
      display.setTextColor(RED);
    }

    display.print(complementLeadingSpaces(dispCO2, 4));
  #else
    display.setTextColor(WHITE);
    display.print(complementLeadingSpaces(dispCO2, 4));
  #endif
}

// ------ print temerature value ------ //
void printTemp() {
  display.setTextColor(WHITE);
  display.setCursor(80, T_y);
  display.print(complementLeadingSpaces(dispTemp, 3));
}

// ------ print humidity value ------ //
void printHum() {
  if (dispHum < 40) {
    display.setTextColor(RED);
  } else if (dispHum > 60) {
    display.setTextColor(BLUE);
  } else {
    display.setTextColor(WHITE);
  }
  display.setCursor(100, H_y);
  display.print("  ");
  display.setCursor(100, H_y);
  display.print(complementLeadingSpaces(dispHum, 2));
}

// ------ print rain prediction value ------ //
void printRain() {
  if (dispRain > 10) {
    display.setTextColor(BLUE);
  } else {
    display.setTextColor(WHITE);
  }
  display.setCursor(90, R_y);
  display.print("   ");
  display.setCursor(90, R_y);
  display.print(complementLeadingSpaces(dispRain, 3));
}




// ------ дополняет лидирующими пробелами для печати ------ //
String complementLeadingSpaces(int value, int expected) {
  String strValue = String(value);
  return complementLeadingSpaces(strValue, expected);
}

String complementLeadingSpaces(float value, int expected) {
  String strValue = String(value, 1);
  return complementLeadingSpaces(strValue, expected);
}

String complementLeadingSpaces(String strValue, int expected) {
  int len = strValue.length();
  if (len >= expected) {
    return strValue;
  }
  int n = expected - len;
  for (int i = 0; i < n; i++) {
    strValue = " " + strValue;
  }

  return strValue;
}
// ------------------- END -------------------------------- //


// ---------------- Очистка старых значений --------------- //
void clearSensors() {
  clearZone(80, 30, 128, 128);
}

void clearDate() {
  clearZone(5, 5, 80, 20);
}

void clearTime() {
  clearZone(80, 5, 128, 20);
}

void clearZone(int x1, int y1, int x2, int y2) {
  display.startPushData(x1, y1, x2, y2);
  int areaLength = (x2 -x1) * (y2-y1);
  for(int i=0; i < areaLength; i++) {
      display.pushData(0);
  }
  display.endPushData();
}
// ------------------- END -------------------------------- //








// ------------------------- DEBUG ------------------------- //
void printDebugMhz(String stat) {
  String message = "MHZ-19... " + stat;

  Serial.println(message);

#if (DISPLAY_TYPE == 1)
  lcd.setCursor(0, 0);
  lcd.print(F(message));
#else
  display.setCursor(1, 1);
  display.println(message);
#endif
}

void printDebugRtc(String stat) {
  String message = "RTC... " + stat;

  Serial.println(message);

#if (DISPLAY_TYPE == 1)
  lcd.setCursor(0, 1);
  lcd.print(F(message));
#else
  display.println(message);
#endif
}

void printDebugBme(String stat) {
  String message = "BME280... " + stat;

  Serial.println(message);

#if (DISPLAY_TYPE == 1)
  lcd.setCursor(0, 2);
  lcd.print(F(message));
#else
  display.println(message);
#endif
}

void printDebugFinal(String stat) {
  String message = stat;

  Serial.println(message);

#if (DISPLAY_TYPE == 1)
  lcd.setCursor(0, 2);
  lcd.print(F(message));
#else
  display.println(message);
#endif
}
