#include <DS3231.h>
DS3231 RTC;



//RTC Pins
const uint8_t timingInterruptPin = 2; //Hardware interrupt 4
//Hall Effect Sensor Pins
const uint8_t eqHallInterruptPin = 3;//Eq
const uint8_t decHallInterruptPin = 18;//Dec
//DEC Stepper Pins
const uint8_t decEnPin = 38;// Enable 22
const uint8_t decStepPin = 36;// Step 24
const uint8_t decDirPin = 34;// Direction 26
//EQ Stepper Pins
const uint8_t eqEnPin = 24;// Enable 23
const uint8_t eqStepPin = 23;// Step 25
const uint8_t eqDirPin = 22;// Direction 27
//Focus Stepper Pins
const uint8_t fcEnPin = 27;//Enable   36
const uint8_t fcStepPin = 26;//Step  34
const uint8_t fcDirPin = 25;//Direction 32
//Camera Power Disable Pin 37
const uint8_t cameraPowerPin = 52; //Camera Relay is Normally Closed.

//Serial communication variables
String serialCommand;
bool commandFinished = false;



void setup() {
  initializeSerialInterfaces();
  initializePinDirections();
  initializeRTCModule();
}

void loop() {
  checkCommandFinished();
}





void checkCommandFinished(){
  if (commandFinished){
    executeCommand(serialCommand);
    clearCommand();
  }
}

void initializeSerialInterfaces(){
  //Begin Serial Interfaces
  Serial.begin(115200);
  Wire.begin();
  delay(1);
  Serial.println("Serial Initialized");
}

void initializeRTCModule(){
  //RTC Initialization
  pinMode(timingInterruptPin, INPUT);
  RTC.enable32kHz(true);
  Serial.println("RTC Initialized");
}

void initializePinDirections(){
  //initialize the stepper motor pins
  pinMode(decEnPin, OUTPUT);
  pinMode(decStepPin, OUTPUT);
  pinMode(decDirPin, OUTPUT);

  pinMode(eqEnPin, OUTPUT);
  pinMode(eqStepPin, OUTPUT);
  pinMode(eqDirPin, OUTPUT);

  pinMode(fcEnPin, OUTPUT);
  pinMode(fcStepPin, OUTPUT);
  pinMode(fcDirPin, OUTPUT);

  pinMode(cameraPowerPin, OUTPUT);
  Serial.println("Stepper Motor Control Pins Initialized");
}

void serialEvent() {
  while (Serial.available()){
    char inChar = (char)Serial.read();
    serialCommand += inChar;
    if (inChar == ';'){
      commandFinished = true;
    }
  }
}

void executeCommand(String command){
  
  String commandType = command.substring(0,1);
  long movement = command.substring(1).toInt();
  
  if(commandType == "D"){
    Serial.println("Declination: "+ String(movement*2));
  }
  if(commandType == "E"){
    Serial.println("Equitorial: "+ String(movement*2));
  }
  if(commandType == "F"){
    Serial.println("Focus: "+ String(movement*2));
  }

}

void clearCommand(){
  serialCommand = "";
  commandFinished = false;
}
