#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SCREEN_ADDRESS 0x3C // LCD I2C Address


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

void setup() {
  Serial.begin(115200);

  if(!apds.begin()){
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else Serial.println("Device initialized!");

  //enable color sensign mode
  apds.enableColor(true);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
}
  lcdsetup();
}

void loop() {
  //create some variables to store the color data in
  uint16_t r, g, b, c;
  
  //wait for color data to be ready
  while(!apds.colorDataReady()){
    delay(5);
  }

  //get the data and print the different channels
  apds.getColorData(&r, &g, &b, &c);
  Serial.print("red: ");
  Serial.print(r);
  
  Serial.print(" green: ");
  Serial.print(g);
  
  Serial.print(" blue: ");
  Serial.print(b);
  
  Serial.print(" clear: ");
  Serial.println(c);
  Serial.println();
  
  delay(500);

    if (r > g && r > b && r >= c) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("RED");
    }
    else if (g > r && g > b && g >= c){
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("GREEN");
    }
    else if (b > r && b > g && b >= c){
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("BLUE");
    }
    else if (c > r && c > g && c >= b){
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("CLEAR");
    }
       display.display();
  }

 void lcdsetup(){
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
 }
