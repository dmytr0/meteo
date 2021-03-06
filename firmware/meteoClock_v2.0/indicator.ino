void setIndicatorBrightness(byte brightness) {
  analogWrite(L_PWM, brightness);
}

void setIndicatorCO2(int ppm) {

  int level = defineLevel(ppm);
  shiftOut(L_D, L_C, L_DIRECTION, level);        // передаем последовательно на dataPin
  digitalWrite(L_L, HIGH);                       //"защелкиваем" регистр, тем самым устанавливая значения на выходах
  digitalWrite(L_L, LOW);                        // устанавливаем синхронизацию "защелки" на LOW
}


// Определение уровня индикатора от показаний CO2
int defineLevel(int ppm) {

  if (ppm <= 400) {
    return B00000000;
  } else if (ppm <= 650) {
    return B10000000;
  } else if (ppm <= 800) {
    return B10000001;
  } else if (ppm <= 1000) {
    return B10000011;
  } else if (ppm <= 1100) {
    return B10000111;
  } else if (ppm <= 1200) {
    return B10001111;
  } else if (ppm <= 1500) {
    return B10011111;
  } else if (ppm <= 2000) {
    return B10111111;
  }

  return B11111111;
}

