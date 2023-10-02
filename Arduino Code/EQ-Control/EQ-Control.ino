#include <SPI.h>
#include <DRV8434S.h>

//Definiitions
//Polar Axis == Right Ascension
//Other axis == Declination

const uint8_t RightAscensionChipSelectPin = 4;
const uint8_t DeclinationChipSelectPin = 5;


const uint32_t TrackingMicroSecondsDelay = 126562;
const float ArcSecondsPerStep = 0.126562;


DRV8434S RightAscensionStepperDriver;
DRV8434S DeclinationStepperDriver;

uint32_t LastTimeCheckpoint = 0;
uint32_t ElapsedTime = 0;
uint32_t Now = 0;

String MovementCommand = "";
bool CommandFinishedReceiving = false;

bool RightAscensionReverseEnabled = true;
bool DeclinationReverseEnabled = true;







void setup(){
  Serial.begin(115200);
  Serial.println("Mount is initializing");
  MovementCommand.reserve(200);


  SPI.begin();
  RightAscensionStepperDriver.setChipSelectPin(RightAscensionChipSelectPin);
  DeclinationStepperDriver.setChipSelectPin(DeclinationChipSelectPin);
  
    
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
  Serial.println("Declination driver SPI controle enabled");


  RightAscensionStepperDriver.enableDriver();
  DeclinationStepperDriver.enableDriver();
  Serial.println("Motor output enabled.");


  RightAscensionStepperDriver.setStepMode(DRV8434SStepMode::MicroStep256);
  DeclinationStepperDriver.setStepMode(DRV8434SStepMode::MicroStep256);


  LastTimeCheckpoint = 0;
  Serial.println("Mount initialization complete.");
}





void loop(){
  Now = micros();
  ElapsedTime += Now-LastTimeCheckpoint;
  LastTimeCheckpoint = Now;
  if (ElapsedTime >= TrackingMicroSecondsDelay){
    ElapsedTime = 0;
    makeTrackingStep();
  }

  
  if (CommandFinishedReceiving) {
    executeCommand(MovementCommand);
    clearCommand();
  }
  
}





void makeTrackingStep(){
 
  RightAscensionStepperDriver.setDirection(RightAscensionReverseEnabled);
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
  Serial.println("Finished RA Movement");
  

  Serial.println("Starting DEC Movement");
  DeclinationStepperDriver.setDirection(DeclinationDirection);
  for (long i = 0; i < DeclinationSteps; i++){
    DeclinationStepperDriver.step();
    delayMicroseconds(10);
  }
  Serial.println("Finished DEC Movement");
}





void clearCommand(){
  MovementCommand = "";
  CommandFinishedReceiving = false;
}





long arcSecondsToSteps(long arcSeconds){
  return long(arcSeconds/ArcSecondsPerStep);
}
