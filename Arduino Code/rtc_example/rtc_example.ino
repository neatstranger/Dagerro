#include <DS3231.h>

DS3231 RTC;
uint32_t timePassed  = 0;
const int interruptPin = 2;
const uint32_t microsecondLimit = 2000000;

uint32_t timePassed2 = 0;
uint32_t checkPoint = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  RTC.enable32kHz(true);
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), countTime, CHANGE);
  Serial.println("Setup Loop Finished");
  checkPoint = micros();
}

void loop() {
  timePassed2 += micros() - checkPoint;
  checkPoint = micros();
  // put your main code here, to run repeatedly:
  if(timePassed >= microsecondLimit){
    timePassed = 0;
    timer();
  }
  if(timePassed2 >= microsecondLimit){
    timePassed2 = 0;
    timer2();
  }
}


void countTime() {
  timePassed += 30;
}

void timer(){
  Serial.println("Hardware Timer Activated");
}
void timer2(){
  Serial.println("Software Timer Activated");
}