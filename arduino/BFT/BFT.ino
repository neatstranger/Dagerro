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

int pulsesReceived = 0;
bool secondaryPulseCount = false;
int stepsTaken = 0;



long currentEqSteps = 0;
long maxEqSteps = 446900;

long currentDecSteps = 0;
long maxDecSteps = 223448;


void setup() {
  initializeSerialInterfaces();
  initializePinDirections();
  initializeInterruptInterfaces();
  initializeRTCModule();
}

void loop() {
  checkCommandFinished();
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



void initializeSerialInterfaces(){
  //Begin Serial Interfaces
  Serial.begin(115200);
  Wire.begin();
  delay(1);
  Serial.println("Serial Initialized");
}
void initializeRTCModule(){  
  RTC.enable32kHz(true);
  Serial.println("RTC Initialized");
}
void initializePinDirections(){
  //declination axis
  pinMode(decEnPin, OUTPUT);
  pinMode(decStepPin, OUTPUT);
  pinMode(decDirPin, OUTPUT);
  //equitorial axis
  pinMode(eqEnPin, OUTPUT);
  pinMode(eqStepPin, OUTPUT);
  pinMode(eqDirPin, OUTPUT);
  //focus control axis
  pinMode(fcEnPin, OUTPUT);
  pinMode(fcStepPin, OUTPUT);
  pinMode(fcDirPin, OUTPUT);
  //camera power control 
  pinMode(cameraPowerPin, OUTPUT);
  //interrupt pins
  pinMode(timingInterruptPin, INPUT);
  pinMode(eqHallInterruptPin, INPUT);
  pinMode(decHallInterruptPin, INPUT);
  Serial.println("Interface Pins Initialized");
}
void initializeInterruptInterfaces(){
  attachInterrupt(digitalPinToInterrupt(timingInterruptPin), trackingInterrupt, RISING);
}
void checkCommandFinished(){
  if (commandFinished){
    executeCommand(serialCommand);
    clearCommand();
  }
}



void trackingInterrupt(){
  pulsesReceived += 1;
  if (pulsesReceived == 1584 && !secondaryPulseCount){
      stepsTaken += 1;
    if (stepsTaken == 48){
      stepsTaken = 0;
      secondaryPulseCount = !secondaryPulseCount;
    }
    moveEqAxis(1, false);
    pulsesReceived = 0;
  }
  else if (pulsesReceived == 1583 && secondaryPulseCount){
    stepsTaken += 1;
    if (stepsTaken == 48){
      stepsTaken = 0;
      secondaryPulseCount = !secondaryPulseCount;
    }
    moveEqAxis(1, false);
    pulsesReceived = 0;
  }
}

void executeCommand(String command){
  String commandType = command.substring(0,1);
  long movement = command.substring(1).toInt();
  bool direction = (movement > 0);
  long steps = arcSecsToSteps(abs(movement));
  String dir = String("LOW");
  if (direction){
    dir = String("HIGH");
  }
  if(commandType == "D"){
    Serial.println("Declination Axis Move: "+ String(steps) +", Direction: "+String(direction));
    moveDecAxis(steps, direction);
  }
  if(commandType == "E"){
    Serial.println("Equitorial Axis Move: "+ String(steps)+", Direction: "+String(direction));
    moveEqAxis(steps, direction);
  }
  if(commandType == "F"){
    Serial.println("Focus Move: "+ String(movement)+", Direction: "+String(direction));
    moveFcAxis(abs(movement), direction);
  }
}



void clearCommand(){
  serialCommand = "";
  commandFinished = false;
}

void moveEqAxis(long steps, bool direction){
  digitalWrite(eqDirPin, direction);
  long i = 0;
  while (i < steps){
    digitalWrite(eqStepPin, HIGH);
    delayMicroseconds(160);
    digitalWrite(eqStepPin, LOW);
    delayMicroseconds(160);
    if(direction){
      currentEqSteps += 1;
    }else{
      currentEqSteps -= 1;
    }
    i++;
  }
}

void moveDecAxis(long steps, bool direction){
  digitalWrite(decDirPin, direction);
  long i = 0;
  while (i < steps){
    digitalWrite(decStepPin, HIGH);
    delayMicroseconds(160);
    digitalWrite(decStepPin, LOW);
    delayMicroseconds(160);
    if(direction){
      currentDecSteps += 1;
    }else{
      currentDecSteps -= 1;
    }
    i++;
  }
}

void moveFcAxis(long steps, bool direction){
  digitalWrite(fcDirPin, direction);
  long i = 0;
  while (i < steps){
    digitalWrite(fcStepPin, HIGH);
    delayMicroseconds(160);
    digitalWrite(fcStepPin, LOW);
    delayMicroseconds(160);
    i++;
  }
}

long arcSecsToSteps(long arcSeconds){
  const double arcSecondsPerStep = 0.725;
  long steps = arcSeconds / arcSecondsPerStep;
  return steps;
}