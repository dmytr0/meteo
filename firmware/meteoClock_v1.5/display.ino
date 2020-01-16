///// DATE TIME ////////////

#if (DISPLAY_TYPE == 1)
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

#else
void drawDateTime() {

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(35,5);

  //DATE
  if (now.day() < 10) display.print(0);
  display.print(now.day());
  display.print(".");
  if (now.month() < 10) display.print(0);
  display.print(now.month());
  display.print("." + now.year());

  //TIME
  boolean dotState = (secs % 2) == 0;
  
  if(hrs < 10) display.print(0);
  display.print(hrs); 
  if(dotState) display.print(":");
  else display.print(" ");
  if(mins < 10) display.print(0);
  display.print(mins);
  if(dotState) display.print(":");
  else display.print(" ");
  if(secs < 10) display.print(0);
  display.print(secs);
}

#endif




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


    if(dispHum < 40) {
        lcd.print("low");
    } else if(dispHum > 60) {
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

    //values
    printCO2();
    printTemp();
    printHum();
    printRain();

  #endif
}

#if (DISPLAY_TYPE == 0)

void printCO2() {
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
  display.print(complementLeadingSpaces(dispHum, 2));
}

void printRain() {
  if (dispRain > 10) {
    display.setTextColor(BLUE);
  } else {
    display.setTextColor(WHITE);
  }
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
  for(int i = 0; i < n; i++){
    strValue = " " + strValue;
  }

  return strValue;
}

#endif















////////////////////// DEBUG ///////////////////////

void printDebugMhz(String stat) {
  String message = "MHZ-19... " + stat;

  Serial.println(F(message));

  #if (DISPLAY_TYPE == 1)
    lcd.setCursor(0, 0);
    lcd.print(F(message));
  #else
    display.setCursor(1,1);
    display.println(message);
  #endif
}

void printDebugRtc(String stat) {
  String message = "RTC... " + stat;

  Serial.println(F(message));

  #if (DISPLAY_TYPE == 1)
    lcd.setCursor(0, 1);
    lcd.print(F(message));
  #else
    display.println(message);
  #endif
}

void printDebugBme(String stat) {
  String message = "BME280... " + stat;

  Serial.println(F(message));

  #if (DISPLAY_TYPE == 1)
    lcd.setCursor(0, 2);
    lcd.print(F(message));
  #else
    display.println(message);
  #endif
}

void printDebugFinal(String stat) {
  String message = stat;

  Serial.println(F(message));

  #if (DISPLAY_TYPE == 1)
    lcd.setCursor(0, 2);
    lcd.print(F(message));
  #else
    display.println(message);
  #endif
}

