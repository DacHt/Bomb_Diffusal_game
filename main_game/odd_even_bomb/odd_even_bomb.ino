#define NUMBER_OF_ALLOWED_ERRORS 2
#define NUMBER_OF_DOUBLE_SPEED 1
#define DEBUG 0
#define PWM_DEBUG 0

const int redWirePin = 2;
const int greenWirePin = 3;
const int brownWirePin = 4;
const int whiteWirePin = 5;

const int resetCounterPin = 6;
const int kaboomCounterPin = 7;
const int even10CounterPin = 8;
const int doubleSpeedCounterPin = 9;
const int explodeBombCounterPin = 10;
const int pwmPin = 12;

boolean isRedCut = false;
boolean isGreenCut = false;
boolean isBrownCut = false;
boolean isWhiteCut = false;

boolean resetFlag = true;
boolean startFlag = false;

int wireWrongCount = 0;


//PWM Values:
//100% = 0
#define PWM_STATE_RESET 0
#define PWM_RESET_LOW 0
#define PWM_RESET_HIGH 100
//50% ~= 3180 range(3150, 3200)
#define PWM_STATE_START 1
#define PWM_START_LOW 3100
#define PWM_START_HIGH 3250
//25% ~= 4770 range(4750, 4800)
#define PWM_STATE_OTHER 2
#define PWM_OTHER_LOW 4700
#define PWM_OTHER_HIGH 4850

int pwmValue;
int pwmState, pwmStateOld;

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete


// Avialable colors: Red, Black, Yellow, Green, White, Brown

// Mini game one:
// A counter is counting down from 5 minutes. If the counter reaches 0 the bomb has exploded.
// At an even 10 of a second either the red or the brown wire has to be cut. 
// At odd 10 seconds the green or white wire has to be cut.
// The counter will output a high signal when at even 10 seconds.
 
void serialEvent() {
  Serial.println("SERIAL: Recivied transmission");
  inputString = "";
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  delay(10);
  Serial.println("SERIAL: Message: " + inputString);
  
  if(inputString.startsWith("Reset")) {
    Serial.println("SERIAL: Setting reset flag");
    resetFlag = true;
  }
  else if(inputString.startsWith("Start")) {
    Serial.println("SERIAL: Setting start flag");
    startFlag = true;
  }
  else {
    Serial.println("ERROR: Could not read message: " + inputString);
  }
}


void setup() {
  // Setup debug
  Serial.begin(9600);

  // set input mode on wire inputs
  pinMode(redWirePin, INPUT_PULLUP);
  pinMode(greenWirePin, INPUT_PULLUP);
  pinMode(brownWirePin, INPUT_PULLUP);
  pinMode(whiteWirePin, INPUT_PULLUP);

  // Set mode on counter pin
  pinMode(resetCounterPin, OUTPUT);
  pinMode(kaboomCounterPin, INPUT);
  pinMode(even10CounterPin, INPUT);
  pinMode(doubleSpeedCounterPin, OUTPUT);
  pinMode(explodeBombCounterPin, OUTPUT);

  pinMode(pwmPin, INPUT);
  
  Serial.println("Odd even bomb is active!");
  pwmState = PWM_STATE_RESET;
  pwmStateOld = PWM_STATE_RESET;
  
  inputString.reserve(200);
  resetFlag = true;
}

void reset() {
  Serial.println("RESET: Reseting game....");
  //Reset game
  //Send reset signal to counter. A high input will reset the counter to 5 min, when output goes low the counter starts counting.
  digitalWrite(resetCounterPin, HIGH);
  delay(2000); // Wait for counter to be reset.

  // Set double speed to LOW
  digitalWrite(doubleSpeedCounterPin, LOW);

  //Reset the bomb
  digitalWrite(explodeBombCounterPin, LOW);
  
  isRedCut = false;
  isGreenCut = false;
  isBrownCut = false;
  isWhiteCut = false;
  wireWrongCount = 0;

  startFlag = false;
  resetFlag = false;

  Serial.println("RESET: Ready!"); 
}

void outputWireStates() {
  Serial.println("Output wire states:");
  Serial.print("red wire is: ");
  Serial.println(digitalRead(redWirePin));
  Serial.print("Green wire is: ");
  Serial.println(digitalRead(greenWirePin));
  Serial.print("brown wire is: ");
  Serial.println(digitalRead(brownWirePin));
  Serial.print("White wire is: ");
  Serial.println(digitalRead(whiteWirePin));
  Serial.print("Kaboom pin is: ");
  Serial.println(digitalRead(kaboomCounterPin));
  Serial.print("Explode pin is: ");
  Serial.println(digitalRead(explodeBombCounterPin));
  Serial.print("Double Speed pin is: ");
  Serial.println(digitalRead(doubleSpeedCounterPin));
  Serial.print("Reset pin is: ");
  Serial.println(digitalRead(resetCounterPin));
  Serial.print("Even 10 pin is: ");
  Serial.println(digitalRead(even10CounterPin));
}

// Outputs if the wires has been cut correctly
boolean isWiresCutCorrect() {
  // If the red wire has not been cut.
  if(!isRedCut) {
    if (DEBUG) {
      Serial.println("WIRE: Reading the red wire");
    }
    // Red wire has been cut correctly 
    if(digitalRead(redWirePin) == 1 && digitalRead(even10CounterPin) == 1) {
      Serial.println("WIRE: The red wire was cut at the right time, pheew!");
      isRedCut = true;
    }
    // Red wire has been cut, but att the wrong time!
    else if( digitalRead(redWirePin) == 1 && digitalRead(even10CounterPin) == 0) {
      Serial.println("WIRE: Idiot you cut the wrong wire! (Red)");
      isRedCut = true;
      wireWrongCount++; 
    }
  }

  // If the green wire has not been cut.
  if(!isGreenCut) {
    if (DEBUG) {
      Serial.println("WIRE: Reading the green wire");
    }
    // Green wire has been cut correctly 
    if(digitalRead(greenWirePin) == 1 && digitalRead(even10CounterPin) == 0) {
      Serial.println("WIRE: The green wire was cut at the right time, pheew!");
      isGreenCut = true;
    }
    // Green wire has been cut, but att the wrong time!
    else if( digitalRead(greenWirePin) == 1 && digitalRead(even10CounterPin) == 1) {
      Serial.println("WIRE: Idiot you cut the wrong wire! (Green)");
      isGreenCut = true;
      wireWrongCount++;
    }
  }

  // If the brown wire has not been cut.
  if(!isBrownCut) {
    if (DEBUG) {
      Serial.println("WIRE: Reading the brown wire");
    }
    // Brown wire has been cut correctly 
    if(digitalRead(brownWirePin) == 1 && digitalRead(even10CounterPin) == 1) {
      Serial.println("WIRE: The brown wire was cut at the right time, pheew!");
      isBrownCut = true;
    }
    // Brown wire has been cut, but att the wrong time!
    else if( digitalRead(brownWirePin) == 1 && digitalRead(even10CounterPin) == 0) {
      Serial.println("WIRE: Idiot you cut the wrong wire! (Brown)");
      isBrownCut = true;
      wireWrongCount++;
    }
  }
  
  // If the white wire has not been cut.
  if(!isWhiteCut) {
    if (DEBUG) {
      Serial.println("WIRE: Reading the white wire");
    }
    // White wire has been cut correctly 
    if(digitalRead(whiteWirePin) == 1 && digitalRead(even10CounterPin) == 0) {
      Serial.println("WIRE: The white wire was cut at the right time, pheew!");
      isWhiteCut = true;
    }
    // White wire has been cut, but att the wrong time!
    else if( digitalRead(whiteWirePin) == 1 && digitalRead(even10CounterPin) == 1) {
      Serial.println("WIRE: Idiot you cut the wrong wire! (White)");
      isWhiteCut = true;
      wireWrongCount++;
    }
  }
  // All wires has been cut corretly!
  if( isRedCut && isGreenCut && isBrownCut && isWhiteCut) {
      Serial.println("WIRE: Yes, you have cut all the wires correctly!");
      return true;   
  }
  // Activate double speed if 1 wire is incorretly cut
  else if(wireWrongCount >= NUMBER_OF_DOUBLE_SPEED) {
    digitalWrite(doubleSpeedCounterPin, HIGH);
  }
  else if(wireWrongCount >= NUMBER_OF_ALLOWED_ERRORS) {
    Serial.println("WIRE: You failed! The bomb will now explode! .... Idiot");
    return false;
  }
  return false;
}

void readPwmValue() {
  pwmValue = pulseIn(pwmPin, HIGH);
  if(PWM_DEBUG) {
    Serial.println("PWM: Reading PWM value: ");
    Serial.println(pwmValue);
  }

  pwmStateOld = pwmState;
  
  if((pwmValue >= PWM_RESET_LOW) && (pwmValue <= PWM_RESET_HIGH)) {
    if(PWM_DEBUG) {
      Serial.println("PWM: Setting PWM state Reset");
    }
    pwmState = PWM_STATE_RESET;
  }
  else if((pwmValue >= PWM_START_LOW) && (pwmValue <= PWM_START_HIGH)) {
    if(PWM_DEBUG) {
      Serial.println("PWM: Setting PWM state Start");
    }
    pwmState = PWM_STATE_START;
  }
  else if((pwmValue >= PWM_OTHER_LOW) && (pwmValue <= PWM_OTHER_HIGH)) {
    if(PWM_DEBUG) {
      Serial.println("PWM: Setting PWM state Other");
    }
    pwmState = PWM_STATE_OTHER;
  }

  if(PWM_DEBUG) {
    Serial.println("PWM: pwmState = " + String(pwmState));
    Serial.println("PWM: pwmStateOld = " + String(pwmStateOld));
  }
  
  if((pwmState == PWM_STATE_RESET) && (pwmStateOld != PWM_STATE_RESET)) {  
     Serial.println("PWM: Setting reset flag");
     resetFlag = 1;
  }
  else if((pwmState == PWM_STATE_START) && (pwmStateOld != PWM_STATE_START)) {  
     Serial.println("PWM: Setting start flag");
     startFlag = 1;
  }
  else if((pwmState == PWM_STATE_OTHER) && (pwmStateOld != PWM_STATE_OTHER)) {
     Serial.println("PWM: Setting other state"); 
  }  
}

void loop() {
  if(resetFlag) {
    if(DEBUG) {
      Serial.println("GAME: Resetflag is set!");
    }
    reset();  
  }
  else if(startFlag) {
    // Start game
    // Set reset signal to counter low to start it. 
    digitalWrite(resetCounterPin, LOW);
    
    // Check if counter has gone kaboom! or if the wires has not been cut correctly
    if((digitalRead(kaboomCounterPin) == HIGH) || (!isWiresCutCorrect() && wireWrongCount >= NUMBER_OF_ALLOWED_ERRORS)) {
      digitalWrite(explodeBombCounterPin, HIGH);
      Serial.println("GAME: KABOOM!");   
      startFlag = false;
    }
    // Check if all wires has been cut correctly
    else if(isWiresCutCorrect()) {
      Serial.println("GAME: Defused!");
      digitalWrite(resetCounterPin, HIGH);
      startFlag = false;
    }
    
    if(DEBUG) {
      outputWireStates();
    }
  }

  readPwmValue();
  delay(200);   
}
