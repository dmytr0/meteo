
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


void printDebugMhz(String stat) {
  String message = "MHZ-19... " + stat;

  Serial.println(F(message));

  #if (DISPLAY_TYPE == 0)
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

  #if (DISPLAY_TYPE == 0)
    lcd.setCursor(0, 1);
    lcd.print(F(message));
  #else
    display.println(message);
  #endif
}

void printDebugBme(String stat) {
  String message = "BME280... " + stat;

  Serial.println(F(message));

  #if (DISPLAY_TYPE == 0)
    lcd.setCursor(0, 2);
    lcd.print(F(message));
  #else
    display.println(message);
  #endif
}

void printDebugFinal(String stat) {
  String message = stat;

  Serial.println(F(message));

  #if (DISPLAY_TYPE == 0)
    lcd.setCursor(0, 2);
    lcd.print(F(message));
  #else
    display.println(message);
  #endif
}

