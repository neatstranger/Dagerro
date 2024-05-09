#include <DS3231.h>


DS3231 RTC;

//RTC Pins
const uint8_t timingInterruptPin = 2; //Hardware interrupt 4
//Hall Effect Sensor Pins
//Eq
//Dec

//DEC Stepper Pins
const uint8_t decEnPin = 22;// Enable 22
const uint8_t decStepPin = 24;// Step 24
const uint8_t decDirPin = 26;// Direction 26
 

//EQ Stepper Pins
const uint8_t eqEnPin = 23;// Enable 23
const uint8_t eqStepPin = 25;// Step 25
const uint8_t eqDirPin = 27;// Direction 27


//Focus Stepper Pins
const uint8_t fcEnPin = 36;//Enable   36
const uint8_t fcStepPin = 34;//Step  34
const uint8_t fcDirPin = 32;//Direction 32


//Camera Power Disable Pin 37
const uint8_t cameraPowerPin = 37;




void setup() {
  //Begin Serial Interfaces
  Serial.begin(115200);
  Wire.begin();
  delay(1);

  //Notify Serial Port That Mount Is Initializing
  Serial.println("Mount is initializing");
  
  //Initialization of RTC
  pinMode(timingInterruptPin, INPUT);
  RTC.enable32kHz(true);

}

void loop() {
  // put your main code here, to run repeatedly:

}
