#include <SPI.h>
#include <DRV8434S.h>
#include <DS3231.h>

DS3231 RTC;
DRV8434S RightAscensionStepperDriver;
DRV8434S DeclinationStepperDriver;


const uint8_t RightAscensionChipSelectPin = 5;
const uint8_t DeclinationChipSelectPin = 4;
const uint8_t interruptPin = 2;

const float ArcSecondsPerStep = 0.253125;


const int smallInterruptCountPerCycle = 11050;
const int smallInterruptMaxCycles = 81;

const int largeInterruptCountPerCycle = 11070;
const int maxInterruptCycles = 150;

int currentCycle = 0;
int currentInterruptCount = 0;


String MovementCommand = "";
bool CommandFinishedReceiving = false;

bool RightAscensionReverseEnabled = false;
bool DeclinationReverseEnabled = false;

bool TakingAdjustmentSteps = false;

int RightAscensionStandbyCurrent = 2000;
int RightAscensionSlewingCurrent = 2000;

int DeclinationStandbyCurrent = 500;
int DeclinationSlewingCurrent = 2000;





void setup(){
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();
  delay(1);

  Serial.println("Mount is initializing");
  MovementCommand.reserve(200);
  
  RightAscensionStepperDriver.setChipSelectPin(RightAscensionChipSelectPin);
  DeclinationStepperDriver.setChipSelectPin(DeclinationChipSelectPin);
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), countCycles, CHANGE);
  delay(1);

  RightAscensionStepperDriver.resetSettings();
  RightAscensionStepperDriver.clearFaults();
  RightAscensionStepperDriver.setCurrentMilliamps(RightAscensionStandbyCurrent);
  Serial.println("Right Driver Ascension Reset, Faults Cleared, Max Current Set");
  DeclinationStepperDriver.resetSettings();
  DeclinationStepperDriver.clearFaults();
  DeclinationStepperDriver.setCurrentMilliamps(DeclinationStandbyCurrent);
  Serial.println("Declination Driver Reset, Faults Cleared, Max Current Set");


  RightAscensionStepperDriver.enableSPIDirection();
  RightAscensionStepperDriver.enableSPIStep();
  Serial.println("Right ascension driver SPI controle enabled");
  DeclinationStepperDriver.enableSPIDirection();
  DeclinationStepperDriver.enableSPIStep();
  Serial.println("Declination driver SPI control enabled");


  RightAscensionStepperDriver.enableDriver();
  DeclinationStepperDriver.enableDriver();
  Serial.println("Motor output enabled.");


  RightAscensionStepperDriver.setStepMode(DRV8434SStepMode::MicroStep128);
  DeclinationStepperDriver.setStepMode(DRV8434SStepMode::MicroStep128);
  RightAscensionStepperDriver.setDirection(RightAscensionReverseEnabled);

  Serial.println("Mount initialization complete.");
  RTC.enable32kHz(true);
}





void loop(){
  if (CommandFinishedReceiving) {
    executeCommand(MovementCommand);
    clearCommand();
  }
  
}




void countCycles(){
  currentInterruptCount += 1;
  if(currentCycle < smallInterruptMaxCycles && currentInterruptCount == smallInterruptCountPerCycle){
    makeTrackingStep();
    currentCycle += 1; 
    currentInterruptCount = 0;
  }else if(currentCycle >=smallInterruptMaxCycles && currentCycle < maxInterruptCycles && currentInterruptCount == largeInterruptCountPerCycle){
    makeTrackingStep();
    currentCycle += 1;
    currentInterruptCount = 0;
  }
  if(currentCycle == maxInterruptCycles){
    currentCycle = 0;
    Serial.println("Cycle Reset");
  }

  
}



void makeTrackingStep(){
  noInterrupts();
  for (int i = 0; i < 10; i++){
    RightAscensionStepperDriver.step();
    delayMicroseconds(10);
  }
  interrupts();
}





void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    MovementCommand += inChar;
    if (inChar == '\r') {
      CommandFinishedReceiving = true;
    }
  }
}





void executeCommand(String command){
  noInterrupts();
  int DeclinationIndex = command.indexOf(";");

  String RightAscensionMovementText = command.substring(0, DeclinationIndex);
  String DeclinationMovementText = command.substring(DeclinationIndex+1, command.length()-1);

  long RightAscensionMovement = RightAscensionMovementText.toInt();
  long DeclinationMovement = DeclinationMovementText.toInt();
  
  bool RightAscensionDirection = RightAscensionMovement > 0;
  if(!RightAscensionReverseEnabled){RightAscensionDirection = RightAscensionDirection;}
  bool DeclinationDirection = DeclinationMovement > 0;
  if(!DeclinationReverseEnabled){DeclinationDirection = DeclinationDirection;}

  RightAscensionMovement = abs(RightAscensionMovement);
  DeclinationMovement = abs(DeclinationMovement);

  long RightAscensionSteps = arcSecondsToSteps(RightAscensionMovement);
  long DeclinationSteps = arcSecondsToSteps(DeclinationMovement);
  
  Serial.println("Starting RA Movement");
  RightAscensionStepperDriver.setCurrentMilliamps(RightAscensionSlewingCurrent);
  RightAscensionStepperDriver.setDirection(RightAscensionDirection);
  for ( long i = 0; i < RightAscensionSteps; i++){
    RightAscensionStepperDriver.step();
    delayMicroseconds(10);
  }
  RightAscensionStepperDriver.setDirection(RightAscensionReverseEnabled);
  RightAscensionStepperDriver.setCurrentMilliamps(RightAscensionStandbyCurrent);
  Serial.println("Finished RA Movement");
  

  Serial.println("Starting DEC Movement");
  DeclinationStepperDriver.setCurrentMilliamps(DeclinationSlewingCurrent);
  DeclinationStepperDriver.setDirection(DeclinationDirection);
  for (long i = 0; i < DeclinationSteps; i++){
    DeclinationStepperDriver.step();
    delayMicroseconds(10);
  }
  DeclinationStepperDriver.setCurrentMilliamps(DeclinationStandbyCurrent);
  Serial.println("Finished DEC Movement");
  currentCycle = 0;
  currentInterruptCount = 0;
  interrupts();
}





void clearCommand(){
  MovementCommand = "";
  CommandFinishedReceiving = false;
}





long arcSecondsToSteps(long arcSeconds){
  return long(arcSeconds/ArcSecondsPerStep);
}
