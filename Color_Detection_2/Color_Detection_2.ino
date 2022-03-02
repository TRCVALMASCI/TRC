#include <TimerOne.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#define RUN_DELAY 4750
#define GALLOP_DELAY 5000
#define WAL_DELAY 1200
#define SET_PWM 100  //0 to 100
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define STNDBY1 36
#define STNDBY2 37
#define LIFTER_PIN 7
#define LED_PIN 10
#define LED_COUNT 1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

enum Direction{
  Forward,
  Stop
};

enum LiftState{
  Up,
  Down
};

Servo myservo;

unsigned int _pwmvalue = 0;
static bool stopmotor = true;
static int pwmcounter = 0;
bool test_directions = true;

int i;
int x = 1;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  setupMecanum();

  pixels.begin();
  pixels.clear();

  if(!apds.begin()){
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else Serial.println("Device initialized!");

  apds.enableColor(true);

  myservo.attach(LIFTER_PIN);
  Serial.println("Done Initialization");

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  lcdsetup();
}

void loop() {
  lcdsetup();
  display.print("<1,0>:0, 0 (White LED)");
  display.display();
  
  for(i = 1; i < 4; i++){

    LED(255, 255, 255, 500);
    run(Forward, RUN_DELAY);
    
    uint16_t r, g, b, c;
    while(!apds.colorDataReady()){
    delay(5);
    }

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
    colorsensor(r, g, b, c, x, 100, 1, 1000, 500);
    x++;
  }

  run(Stop, 0);
  stopMotors();
}

//=============LCD=============//

void lcdsetup(){
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
}

void lcddisplay(int x){
  display.print("<1,");
  display.print(x);
}

//=============MOVEMENT=============//

void setupMecanum(){
  pinMode(STNDBY1, OUTPUT);
  pinMode(STNDBY2, OUTPUT);

  DDRA = DDRA | 0b11111111; //set PORTA to output
  DDRL = DDRL | 0x0F;       //set PORTL to output (Pins 46,47,48,49)

  // DDRA |= 0xFF;
  PORTA = 0b00000000; //set PORT A to LOW
  PORTL &= ~0x0F;     //reverses value of PORTL
  initMotorPwm();

  setMotorPWM(SET_PWM);
}

void initMotorPwm(void){
  Timer1.initialize(100); // 1000us interval for each % of duty cycle
  Timer1.attachInterrupt(MotorPwmHandler);
  Timer1.stop();
}

void MotorPwmHandler(void){
  if (pwmcounter > _pwmvalue){
    PORTL &= ~0x0F;
  }
  pwmcounter++;
  
  if (pwmcounter >= 100){
    pwmcounter = 0;
    PORTL |= 0x0F;
  }
}

void setMotorPWM(int pwm){
  _pwmvalue = pwm;
}

void run(Direction direction, uint16_t duration){
  setMotorDir(direction);
  runMotors();
  if (duration > 0){
    delay(duration);
    stopMotors();
  }
}

void setMotorDir(Direction _cmd){
  switch (_cmd){
  case Forward:
    PORTA = 0xAA;
    break;
  case Stop:
    PORTA = 0xFF;
    break;
  default:
    break;
  }
}

void runMotors(void){
  pwmcounter = 0;
  digitalWrite(STNDBY1, HIGH);
  digitalWrite(STNDBY2, HIGH);
  Timer1.start();
}

void stopMotors(void){
  Timer1.stop();
  setMotorDir(Stop);
  delay(200);
  digitalWrite(STNDBY1, LOW);
  digitalWrite(STNDBY2, LOW);
}

//=============COLOR SENSOR=============//

void colorsensor(int r, int g, int b, int c, int x, int LIFTER_ANGLE, int LIFTER_SPEED, int DELAY, int LED_DELAY){
  if (r >= g && r >= b && r >= (c-1)){
    lcddisplay(x);
    display.print(">:1, RED (RED LED)");  
    lifter(LIFTER_ANGLE, LIFTER_SPEED, DELAY);
    LED(255, 0, 0, LED_DELAY);
    
  }
  else if (b >= r && b >= g && b >= (c-1)){
    lcddisplay(x);
    display.print(">:1, BLUE (BLUE LED)");
    lifter(LIFTER_ANGLE, LIFTER_SPEED, DELAY);
    LED(0, 0, 255, LED_DELAY);
  }
}

//=============LIFTER=============//

void lifter(int LIFTER_ANGLE,int LIFTER_SPEED, int DELAY){
  display.display();
  lift(Up, LIFTER_ANGLE, LIFTER_SPEED);
  delay(DELAY);
  lift(Down, LIFTER_ANGLE, LIFTER_SPEED);
  delay(DELAY);
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

//=============LED=============//

void LED(int RED, int GREEN, int BLUE, int LED_DELAY){
  pixels.setPixelColor(0, pixels.Color(RED, GREEN, BLUE));
  pixels.show();
  delay(LED_DELAY);
}
