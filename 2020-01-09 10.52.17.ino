#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define REDD    0xC800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF


#define CO2_y 30
#define T_y 55
#define H_y 80
#define R_y 105

#define DISPLAY_LED_PIN 6

#define __CS 10
#define __DC 9
#define __RES 8

TFT_ILI9163C display = TFT_ILI9163C(__CS, __DC, __RES);

int brightness = 50;

void setup() {
  pinMode(DISPLAY_LED_PIN, OUTPUT);
  Serial.begin(115200); 
  Serial.println("Display started");
  display.begin();

  draw();

  analogWrite(DISPLAY_LED_PIN, brightness);

}

int co2 = 400;
float tmp = 15.1;
int rain = -21;
int hum = 15;

void draw(){
  display.setRotation(1);
  display.clearScreen();
  
  //date time
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(35,5);
  display.println("25.05.90  10:54");

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

}

void printCO2() {
  if (co2 < 800) {
    display.setTextColor(CYAN);
  } else if (co2 < 1000) {
    display.setTextColor(YELLOW);
  } else {
    display.setTextColor(RED);
  }
  
  display.setCursor(80, CO2_y);
  display.print(complementLeadingSpaces(co2, 4));
}

void printTemp() {
  display.setTextColor(WHITE);
  display.setCursor(80, T_y);
  display.print(complementLeadingSpaces(tmp, 3));
}

void printHum() {
  if (hum < 40) {
    display.setTextColor(RED);
  } else if (hum > 60) {
    display.setTextColor(BLUE);
  } else {
    display.setTextColor(WHITE);
  }
  display.setCursor(100, H_y);
  display.print(complementLeadingSpaces(hum, 2));
}

void printRain() {
  if (rain > 10) {
    display.setTextColor(BLUE);
  } else {
    display.setTextColor(WHITE);
  }
  display.setCursor(90, R_y);
  display.print(complementLeadingSpaces(rain, 3));
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

void loop() {

  delay(2000);


  co2 += 100;
  tmp += 1.1;
  rain += 4;
  hum += 5;

  draw();

}
