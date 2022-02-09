#define CLK 19
#define DT 18
#define SW 6
#define PIN_LED_RED 8
#define PIN_LED_GREEN 9
#define PIN_LED_BLUE 10
#define DELAY 500

int counter = 0;
int currentStateCLK;
int lastStateCLK;
int mod;
String currentDir ="";


void setup() {
  
  // Set encoder pins as inputs
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  Serial.begin(9600);
  lastStateCLK = digitalRead(CLK);
}

void loop() {
  
  currentStateCLK = digitalRead(CLK);

  if (currentStateCLK != lastStateCLK  && currentStateCLK == 0){
    if (digitalRead(DT) != currentStateCLK) {
      currentDir = "CW";
      CW_LED();    
    } 
    else { 
      currentDir = "CCW";
      CCW_LED();
    }
  }

  else if (currentStateCLK == lastStateCLK  && currentStateCLK == 0){
    if (currentDir == "CW") {
      CW_LED();    
    } 
    else { 
      CCW_LED();
    }
  }


  lastStateCLK = currentStateCLK;
  
  delay(1);
}

void CW_LED(){

  rgb_color(255, 0, 0);
  delay(DELAY);
  rgb_color(0, 255, 0);
  delay(DELAY);
  rgb_color(0, 0, 255);
  delay(DELAY);
  rgb_color(255, 255, 255);
  delay(DELAY);
  
}

void CCW_LED(){
  
  rgb_color(255, 0, 0);
  delay(DELAY);
  rgb_color(255, 255, 255);
  delay(DELAY);
  rgb_color(0, 0, 255);
  delay(DELAY);
  rgb_color(0, 255, 0);
  delay(DELAY);
  
}

void rgb_color(int red, int green, int blue){
  analogWrite(PIN_LED_RED, red);
  analogWrite(PIN_LED_GREEN, green);
  analogWrite(PIN_LED_BLUE, blue);
}
