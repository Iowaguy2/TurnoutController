// This code is designed for a Arduino Nano to control 4 Atlas Turnouts and two LED outputs per turnout.
// The Nano monitors 4 inputs for pushbutton detection and toggles the state of the turnout/Led when pressed.
// The turnouts require a momentary 12vac on one of its two coils, seperate relays are used to provide the 12vac, 
// the Nano drives 5v to turn on the relay. The Nano drives one of two LEDs associated with the turnout.

const uint8_t TurnoutCount = 4;
const unsigned long CoilOnDelay = 500; //ms using delay()
const uint8_t DebounceCount = 5;
const uint8_t DebounceLimit = 15;
const uint8_t DebounceDelay = 250;
const int AnalogMidpoint = 512;
const uint8_t Thru = 0;
const uint8_t Diverge = 1;
const uint8_t RelayOn = LOW;
const uint8_t RelayOff = HIGH;

// The following elements are in order (ie: [0]=first turnout, [3]=third turnout.)
uint8_t PBInputs[] = {A7, A1, A2, A6};// using analog pins for inputs. NOTE: must use analogread() for a6, a7
uint8_t CoilOutputs[4][2] = { {A0, 13 }, {2, 3}, {4, 5}, {6, 7} };
uint8_t LEDOutputs[4][2] = { {8, 9 }, {10, 11}, {12, A3}, {A4, A5} };
uint8_t OutputStates[] = {3,3,3,3}; // Maintains state of turnout.

void setup() {
  // put your setup code here, to run once:

//initialize discrete pinmode
for (uint8_t i = 0; i < TurnoutCount; i++){
  pinMode(PBInputs[i], INPUT_PULLUP);
  pinMode(CoilOutputs[i][0], OUTPUT);
  pinMode(CoilOutputs[i][1], OUTPUT);
  pinMode(LEDOutputs[i][0], OUTPUT);
  pinMode(LEDOutputs[i][1], OUTPUT);
  //Serial.print("completed discrete initialization for turnout ");
  //Serial.print(i+1);
}
// Initialize output values, setting turnouts and turnout LEDs to known state
// Assuming state 0 is for through-traffic w/LED green, state 1 is for diverging traffic, LED red.
for (uint8_t i = 0; i < TurnoutCount; i++){
  digitalWrite(CoilOutputs[i][1], RelayOff);
  digitalWrite(CoilOutputs[i][0], RelayOn); // Set default to straight thru
  delay(CoilOnDelay);
  digitalWrite(CoilOutputs[i][0], RelayOff); // reset output
  digitalWrite(LEDOutputs[i][1], LOW);  // set LEDs to match
  digitalWrite(LEDOutputs[i][0], HIGH);
  OutputStates[i] = Thru; // default state, typically LED green and turnout goes straight
  //Serial.print("completed output initialization for turnout ");
  //Serial.print(i+1);
}

}//End setup()

void loop() {
  int pbValue = 1023;
  // put your main code here, to run repeatedly:
  for(uint8_t t = 0; t < TurnoutCount; t++){
    //NOTE: must use analogread() for a6, a7 so use it for all
    pbValue = analogRead(PBInputs[t]);
    if(pbValue < AnalogMidpoint){
      if(OutputStates[t] == Thru){
        digitalWrite(CoilOutputs[t][1], RelayOn);
        delay(CoilOnDelay);
        digitalWrite(CoilOutputs[t][1], RelayOff);
        digitalWrite(LEDOutputs[t][1], HIGH);
        digitalWrite(LEDOutputs[t][0], LOW);
        OutputStates[t] = Diverge;
      }else{//outputState == Diverge
        digitalWrite(CoilOutputs[t][0], RelayOn);
        delay(CoilOnDelay);
        digitalWrite(CoilOutputs[t][0], RelayOff);
        digitalWrite(LEDOutputs[t][0], HIGH);
        digitalWrite(LEDOutputs[t][1], LOW);
        OutputStates[t] = Thru;
      }
//      uint8_t c = 0;
//      while (c < DebounceLimit){  // provide time to take finger off button
        delay(DebounceDelay);
//      }
    }// end if PBInput
    pbValue = 1;
  }// End for
  
}// End loop()
