#include <SPI.h>
#include <DRV8434S.h>
#include <DS3231.h>

DS3231 RTC;
DRV8434S RightAscensionStepperDriver;
DRV8434S DeclinationStepperDriver;


const uint8_t RightAscensionChipSelectPin = 4;
const uint8_t DeclinationChipSelectPin = 5;
const uint8_t interruptPin = 2;

const float ArcSecondsPerStep = 0.253125;


const uint32_t smallInterruptCountPerCycle = 1105;
const uint8_t smallInterruptMaxCycles = 810;

const uint32_t largeInterruptCountPerCycle = 1107;
const uint8_t largeInterruptMaxCycles = 690;

uint8_t currentCycle = 0;
uint32_t currentInterruptCount = 0;


String MovementCommand = "";
bool CommandFinishedReceiving = false;

bool RightAscensionReverseEnabled = true;
bool DeclinationReverseEnabled = true;

bool TakingAdjustmentSteps = false;







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
  RightAscensionStepperDriver.setCurrentMilliamps(2000);
  Serial.println("Right Driver Ascension Reset, Faults Cleared, Max Current Set");
  DeclinationStepperDriver.resetSettings();
  DeclinationStepperDriver.clearFaults();
  DeclinationStepperDriver.setCurrentMilliamps(2000);
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
  }else if(currentCycle >=smallInterruptMaxCycles && currentCycle < 100 && currentInterruptCount == largeInterruptCountPerCycle){
    makeTrackingStep();
    currentCycle += 1;
    currentInterruptCount = 0;
  }else if(currentCycle == 100){
    currentCycle = 0;
  }

  
}



void makeTrackingStep(){
  RightAscensionStepperDriver.step();
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
  if(!RightAscensionReverseEnabled){RightAscensionDirection = !RightAscensionDirection;}
  bool DeclinationDirection = DeclinationMovement > 0;
  if(!DeclinationReverseEnabled){DeclinationDirection = !DeclinationDirection;}

  RightAscensionMovement = abs(RightAscensionMovement);
  DeclinationMovement = abs(DeclinationMovement);

  long RightAscensionSteps = arcSecondsToSteps(RightAscensionMovement);
  long DeclinationSteps = arcSecondsToSteps(DeclinationMovement);
  
  Serial.println("Starting RA Movement");
  RightAscensionStepperDriver.setDirection(RightAscensionDirection);
  for ( long i = 0; i < RightAscensionSteps; i++){
    RightAscensionStepperDriver.step();
    delayMicroseconds(10);
  }
  RightAscensionStepperDriver.setDirection(RightAscensionReverseEnabled);
  Serial.println("Finished RA Movement");
  

  Serial.println("Starting DEC Movement");
  DeclinationStepperDriver.setDirection(DeclinationDirection);
  for (long i = 0; i < DeclinationSteps; i++){
    DeclinationStepperDriver.step();
    delayMicroseconds(10);
  }
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
