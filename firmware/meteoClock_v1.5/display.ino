
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
