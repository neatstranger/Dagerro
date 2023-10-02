#include <DS3231.h>

DS3231 RTC;
uint32_t timePassed  = 0;
const int interruptPin = 2;
const uint32_t microsecondLimit = 2000000;
bool addedExtraSecond = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  RTC.enable32kHz(true);
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), countTime, FALLING);
  Serial.println("Setup Loop Finished");
}

void loop() {
  if(timePassed >= microsecondLimit){
    timePassed = 0;
    timer();
  }
}


void countTime() {
  if(addedExtraSecond){
    timePassed += 30;
  }else{
    addedExtraSecond = true;
    timePassed += 31;
  }
  
}

void timer(){
  Serial.println("Hardware Timer Activated");
}