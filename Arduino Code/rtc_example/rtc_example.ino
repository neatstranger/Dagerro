#include <DS3231.h>

DS3231 RTC;
const uint8_t interruptPin = 2;

const uint32_t smallInterruptCountPerCycle = 16588;
const uint8_t smallInterruptMaxCycles = 60;


const uint32_t largeInterruptCountPerCycle = 16590;
const uint8_t largeInterruptMaxCycles = 40;

uint8_t currentCycle = 0;
uint32_t currentInterruptCount = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  RTC.enable32kHz(true);
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), countTime, CHANGE);
  Serial.println("Setup Loop Finished");
}

void loop() {
}


void countTime() {
  currentInterruptCount++;
  if(currentCycle < 60 && currentInterruptCount == smallInterruptCountPerCycle){
    timer();
    currentCycle++; 
    currentInterruptCount = 0;
  }else if(currentCycle >=60 && currentCycle < 100 && currentInterruptCount == largeInterruptCountPerCycle){
    timer();
    currentCycle++;
    currentInterruptCount = 0;
  }else{
    currentCycle = 0;
  }
    
}

void timer(){
  Serial.println("Hardware Timer Activated");
}