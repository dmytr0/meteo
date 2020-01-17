#if (DISPLAY_TYPE == 1)
void setIndicatorCO2(int ppm) {
  int red = constrain(ppm, 800, 1000);
  red = map(red, 800, 1000, 0, 255);

  int green = constrain(ppm, 1000, 1200);
  green = map(green, 1000, 1200, 255, 0);

  setLEDinRGB(red, green, 0);
}

void setLEDinRGB(byte R, byte G, byte B) {
  setOneLed(LED_R, R);
  setOneLed(LED_G, G);
  setOneLed(LED_B, B);
}

void setOneLed(byte pinNumber, byte value) {
  value = constrain(value, 0, 255);
  value = map(value, 0, 255, 0, LED_CURRENT_BRIGHTNESS);
  analogWrite(pinNumber, value);
}
#else
void setIndicatorCO2(int ppm) {

  int level = defineLevel(ppm);
  digitalWrite(L_L, LOW);                        // устанавливаем синхронизацию "защелки" на LOW
  shiftOut(L_D, L_C, L_DIRECTION, level);        // передаем последовательно на dataPin
  digitalWrite(L_L, HIGH);                       //"защелкиваем" регистр, тем самым устанавливая значения на выходах
  digitalWrite(L_L, LOW);  
}

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
#endif
