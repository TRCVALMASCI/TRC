#include <Adafruit_NeoPixel.h>
#define LED_PIN 10

#define LED_COUNT 1

Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

void setup() {
  pixels.begin();
  pixels.clear();
}

void loop() {
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.show();
  delay(DELAYVAL);
  pixels.setPixelColor(0, pixels.Color(0, 128, 0));
  pixels.show();
  delay(DELAYVAL);
  pixels.setPixelColor(0, pixels.Color(0, 0, 255));
  pixels.show();
  delay(DELAYVAL);
  pixels.setPixelColor(0, pixels.Color(255, 255, 255));
  pixels.show();
  delay(DELAYVAL);

}
