#include <QTRSensors.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define scrwid 128
#define scrhei 64
#define scradr 0x3C
#define DELAY 1000

QTRSensors qtr_front;
QTRSensors qtr_back;
QTRSensors qtr_left;
QTRSensors qtr_right;

const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];

Adafruit_SSD1306 display(scrwid, scrhei, &Wire, -1);

void setupLineSensors()
{
  const uint16_t def_timeout = 1000;
  Serial.println("initializing line sensors . . .");

  //front line sensor
  qtr_front.setTimeout(def_timeout);
  qtr_front.setTypeRC();
  qtr_front.setSensorPins((const uint8_t[]) {
    A0, A1, A2, A3
  }, SensorCount);
  qtr_front.setEmitterPin(33);

  //back line sensor
  qtr_back.setTimeout(def_timeout);
  qtr_back.setTypeRC();
  qtr_back.setSensorPins((const uint8_t[]) {
    A15, A14, A13, A12
  }, SensorCount);
  qtr_back.setEmitterPin(32);

  //left line sensor
  qtr_left.setTimeout(def_timeout);
  qtr_left.setTypeRC();
  qtr_left.setSensorPins((const uint8_t[]) {
    A7, A6, A5, A4
  }, SensorCount);
  qtr_left.setEmitterPin(30);

  //right line sensor
  qtr_right.setTimeout(def_timeout);
  qtr_right.setTypeRC();
  qtr_right.setSensorPins((const uint8_t[]) {
    A8, A9, A10, A11
  }, SensorCount);
  qtr_right.setEmitterPin(31);
}

void setup()
{
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, scradr)){
    Serial.println(F("SSD12306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  
  setupLineSensors();
  calibrate_sensor(qtr_front);
  

}

void loop()
{
  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t position = qtr_front.readLineBlack(sensorValues);

  // print the sensor values as numbers from 0 to 1000, where 0 means maximum
  // reflectance and 1000 means minimum reflectance, followed by the line
  // position

 
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println(position);

  delay(250);
}

void calibrate_sensor(QTRSensors &lineSensor)
{
  Serial.println("Calibrating . . .");
  Serial.println("Slowly move the sensor across the electrical tape");
  // 2.5 ms RC read timeout (default) * 10 reads per calibrate() call
  // = ~25 ms per calibrate() call.
  // Call calibrate() 400 times to make calibration take about 10 seconds.
  for (uint16_t i = 0; i < 400; i++)
  {
    lineSensor.calibrate();
  }
  Serial.println();

  Serial.print("Minimum Values: ");
  // print the calibration minimum values measured when emitters were on
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(lineSensor.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();

  Serial.print("Maximum Values: ");
  // print the calibration maximum values measured when emitters were on
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(lineSensor.calibrationOn.maximum[i]);
    Serial.print(' ');
  }

  for (int x = 0; x < SensorCount; x++){
    display.print("Sensor ");
    display.print(x+1);
    display.print(": ");
    display.print(lineSensor.calibrationOn.minimum[x]);
    display.print(" ");
    display.print(lineSensor.calibrationOn.maximum[x]);
    display.println();
    display.display();
  }

  Serial.println();
  Serial.println("Done calibrating!");
  delay(1000);
  Serial.println();
}
