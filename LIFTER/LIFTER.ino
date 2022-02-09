#include <Servo.h>

#define LIFTER_PIN 7
#define LIFTER_ANGLE 100   //90 degrees max
#define LIFTER_SPEED  1  //milliseconds
#define DELAY 1000

#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

enum LiftState{
  Up,
  Down
};

Servo myservo; // create servo object to control a servo

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  if(!apds.begin()){
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else Serial.println("Device initialized!");

  //enable color sensign mode
  apds.enableColor(true);
  
  myservo.attach(LIFTER_PIN);
  Serial.println("Done Initialization");
}

void loop()
{
  // put your main code here, to run repeatedly:
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

    if (r > g && r > b && r >= (c-1)) {
      lift(Up, LIFTER_ANGLE, LIFTER_SPEED);
      delay(DELAY);
      lift(Down, LIFTER_ANGLE, LIFTER_SPEED);
      delay(DELAY);
    }
    else if (b > r && b > g && b >= (c-1)){
      lift(Up, LIFTER_ANGLE, LIFTER_SPEED);
      delay(DELAY);
      lift(Down, LIFTER_ANGLE, LIFTER_SPEED);
      delay(DELAY);
    }

}

void lift(LiftState state, int _angle, int _speed)
{
  int COUNTER_WIDTH_MIN = 500;   //0 angle
  int COUNTER_WIDTH_MAX = 2500;  //270 angle

  if (_angle < 0)
    _angle = 0;
  else if (_angle > 270)
    _angle = 270;

  int pulseWidth = map(_angle, 0, 270, COUNTER_WIDTH_MIN, COUNTER_WIDTH_MAX);
  Serial.println(pulseWidth);
  
  if (state == Up)
  {
    for (int pos = COUNTER_WIDTH_MIN; pos <= pulseWidth; pos += 1)
    {
      myservo.writeMicroseconds(pos);
      delay(_speed);  //increment delay in milliseconds
    }
  }
  else if (state == Down)
  {
    for (int pos = pulseWidth; pos >= COUNTER_WIDTH_MIN; pos -= 1)
    {
      myservo.writeMicroseconds(pos);
      delay(_speed);  //increment delay in milliseconds
    }
  }
}
