// ------------ Настройка отображения дисплея ------------ //

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


///// DATE TIME ////////////


void drawDate() {
  #if (DISPLAY_TYPE == 1)
    int dayofweek = now.dayOfTheWeek();
    lcd.setCursor(10, 0);
    lcd.print(dayNames[dayofweek]);
    lcd.print("  ");

    if (day < 10) lcd.print(0);
    lcd.print(day);
    lcd.print(".");
    if (month < 10) lcd.print(0);
    lcd.print(month);
  #else
    clearDate();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 5);

    delay(50);
    Serial.println("year " + year);
    delay(50);
    Serial.println("month " + month);
    delay(50);
    Serial.println("day " + day);
    delay(50);

    String date = "";
    if (day < 10) date += 0;
    date += day + ".";
    if (month < 10) date += 0;
    date += month + ".";
    date += year + " ";

    Serial.println(date);
    display.println(date);
  #endif
}

void drawClock() {
  boolean dotState = (secs % 2) == 0;
  #if (DISPLAY_TYPE == 1)
  
    lcd.setCursor(0, 0);
    if (hrs < 10) lcd.print(0);
    lcd.print(hrs);

    if (dotState) lcd.print(":");
    else lcd.print(" ");

    if (mins < 10) lcd.print(0);
    lcd.print(mins);

    if (dotState) lcd.print(":");
    else lcd.print(" ");

    if (secs < 10) lcd.print(0);
    lcd.print(secs);
  #else
    clearTime();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(80, 5);

     //TIME
    String dateTime = "";
    if (hrs < 10) dateTime += 0;
    dateTime += hrs;
    if (dotState) dateTime += ":";
    else dateTime += " ";
    if (mins < 10) dateTime += 0;
    dateTime += mins;
    if (dotState) dateTime += ":";
    else dateTime += " ";
    if (secs < 10) dateTime += 0;
    dateTime += secs;

    Serial.println(dateTime);
    display.println(dateTime);
  #endif
}




////// SENSORS /////

void drawSensors() {


#if (DISPLAY_TYPE == 1)
  lcd.setCursor(0, 1);
  lcd.print(String(dispTemp, 1));
  lcd.write(223);

  lcd.setCursor(7, 1);
  lcd.print("        ");
  lcd.setCursor(7, 1);
  lcd.print("Hum:");
  lcd.print(" " + String(dispHum) + "% ");


  if (dispHum < 40) {
    lcd.print("low");
  } else if (dispHum > 60) {
    lcd.print("hi ");
  } else {
    lcd.print("   ");
  }

#if (CO2_SENSOR == 1)
  lcd.setCursor(0, 2);
  lcd.print(String(dispCO2) + " ppm");
  if (dispCO2 < 1000) lcd.print(" ");
#endif

  lcd.setCursor(0, 3);
  lcd.print("       ");
  lcd.setCursor(0, 3);
  lcd.print("P: ");
  lcd.print(String(dispPres) + "mm  Rain: ");
  lcd.print(String(dispRain) + "%");
#else

  clearSensors();
  display.setTextSize(2);
  //values
  printCO2();
  printTemp();
  printHum();
  printRain();

#endif
}

#if (DISPLAY_TYPE == 0)

void printCO2() {
  display.setCursor(80, CO2_y);
  display.print("    ");

  if (dispCO2 < 800) {
    display.setTextColor(CYAN);
  } else if (dispCO2 < 1000) {
    display.setTextColor(YELLOW);
  } else {
    display.setTextColor(RED);
  }

  display.setCursor(80, CO2_y);
  display.print(complementLeadingSpaces(dispCO2, 4));
}

void printTemp() {
  display.setTextColor(WHITE);
  display.setCursor(80, T_y);
  display.print("   ");
  display.setCursor(80, T_y);
  display.print(complementLeadingSpaces(dispTemp, 3));
}

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

#endif








////////////////////// DEBUG ///////////////////////

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
