#include <TimerOne.h>

// CHECK 1
  // MOVEMENT
#define RUN_DELAY 4750
#define GALLOP_DELAY 5000
#define WAL_DELAY 1200
#define SET_PWM 100  //0 to 100
#define STNDBY1 36
#define STNDBY2 37

enum Direction
{
  Forward,
  Backward,
  CW_Center_Center,
  CCW_Center_Center,
  Left,
  Right,
  Stop
};

unsigned int _pwmvalue = 0;
static bool stopmotor = true;
static int pwmcounter = 0;
bool test_directions = true;

// CHECK 2
  //LED DISPLAY
#include <Adafruit_NeoPixel.h>
#define LED_PIN 10
#define LED_COUNT 1
#define DELAY 500

Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

  //ENCODER USAGE
#define CLK 19
#define DT 18
#define SW 6

int counter = 0;
int currentStateCLK;
int lastStateCLK;
int mod;
String currentDir ="";

  //LCD COUNTER
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define scrwid 128
#define scrhei 64
#define scradr 0x3C
#define LCD_DELAY 100

int i; 
int x;

Adafruit_SSD1306 display(scrwid, scrhei, &Wire, -1);

void setup(){
  //MOVEMENT
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  setupMecanum();

  //LED DISPLAY
  pixels.begin();
  pixels.clear();

  //ENCODER USAGE
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);

  lastStateCLK = digitalRead(CLK);

  //LCD COUNTER
  if (!display.begin(SSD1306_SWITCHCAPVCC, scradr)){
  Serial.println(F("SSD12306 allocation failed"));
  for (;;);
  }
  
}

void loop(){
  MOVEMENT();
  LEDDISPLAY();
  LCDCOUNTER();
}


//MOVEMENT
void MOVEMENT() {
    // put your main code here, to run repeatedly:
  if (test_directions)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    // Forward or Backwards
    run(Forward, RUN_DELAY);
    run(Backward, RUN_DELAY);
    run(Backward, RUN_DELAY);
    run(Forward, RUN_DELAY);

    // Clockwise or Counterclockwise-Center-Center
    run(CW_Center_Center, WAL_DELAY);
    run(CCW_Center_Center, WAL_DELAY);
    run(CCW_Center_Center, WAL_DELAY);
    run(CW_Center_Center, WAL_DELAY);

    // Left or Right
    run(Left, GALLOP_DELAY);
    run(Right, GALLOP_DELAY);
    run(Right, GALLOP_DELAY);
    run(Left, GALLOP_DELAY);

    // Stop All Motors
    run(Stop, 0);
    
    stopMotors();
    
    test_directions = false;
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void setupMecanum()
{
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

/*mechanum wheels direction*/
void setMotorDir(Direction _cmd)
{
  switch (_cmd)
  {
  case Forward:
    PORTA = 0xAA;
    break;
  case Backward:
    PORTA = 0x55;
    break;
  case CW_Center_Center:
    PORTA = 0x69;
    break;
  case CCW_Center_Center:
    PORTA = 0x96;
    break;
  case Left:
    PORTA = 0x99;
    break;
  case Right:
    PORTA = 0x66;
    break;

  case Stop:
    PORTA = 0xFF;
    break;
  default:
    break;
  }
}

void initMotorPwm(void)
{
  Timer1.initialize(100); // 1000us interval for each % of duty cycle
  Timer1.attachInterrupt(MotorPwmHandler);
  Timer1.stop();
}

void prepMotors(void)
{
  stopmotor = false;

  PORTL |= 0x0F;

  // reset pwm
  pwmcounter = 0;
  stopmotor = false;

  // initialize timer
  initMotorPwm();

  // turn on stndbay pins
  // to do: port manipulation
  digitalWrite(STNDBY1, HIGH);
  digitalWrite(STNDBY2, HIGH);
}

void stopMotors(void)
{
  Timer1.stop();
  setMotorDir(Stop);
  delay(200);
  digitalWrite(STNDBY1, LOW);
  digitalWrite(STNDBY2, LOW);
}

void runMotors(void)
{
  pwmcounter = 0;
  digitalWrite(STNDBY1, HIGH);
  digitalWrite(STNDBY2, HIGH);
  Timer1.start();
}

// Handle interrupt every 10% duty cycle
// Initially, PWM pins are high
void MotorPwmHandler(void)
{
  // Evaluate counter
  if (pwmcounter > _pwmvalue)
  {
    PORTL &= ~0x0F; // digital low of pwm
  }
  pwmcounter++;

  // reset pwm counter on overflow
  if (pwmcounter >= 100)
  {
    pwmcounter = 0;
    PORTL |= 0x0F; // digital high of pwm
  }
}

void setMotorPWM(int pwm)
{
  _pwmvalue = pwm;
}

void run(Direction direction, uint16_t duration)
{
  setMotorDir(direction);
  runMotors();

  if (duration > 0)
  {
    delay(duration);
    stopMotors();
  }
}


//LED DISPLAY
void LEDDISPLAY(){
  LED(255, 0, 0);
  LED(0, 255, 0);
  LED(0, 0, 255);
  LED(255, 255, 255);  
}

void LED(int RED, int BLUE, int GREEN){
  pixels.setPixelColor(0, pixels.Color(RED, BLUE, GREEN));
  pixels.show();
  delay(DELAY);
}

//LCD COUNTER
void LCDCOUNTER(){
  lcdsetup();
  
  for(i = 1; i <= 20; i++){

    if (i>8){
      lcdsetup();
        for (x = 7; x > 0; x--) {
        display.println(i-x);
      }
      display.println(i);
      delay(LCD_DELAY);
      }

    else {
    display.println(i);
    delay(LCD_DELAY);
    }

    display.display();
  }
}

void lcdsetup(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
}
