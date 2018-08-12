#define NUMBER_OF_ALLOWED_ERRORS 1
#define NUMBER_OF_DOUBLE_SPEED 1
const int DEBUG = 0;

const int redWirePin = 2;
const int greenWirePin = 3;
const int brownWirePin = 4;
const int whiteWirePin = 5;

const int resetCounterPin = 6;
const int kaboomCounterPin = 7;
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
//50% ~= 3180 range(3150, 3200)
//25% ~= 4770 range(4750, 4800)
int pwmValue;
int pwmState, pwmStateOld;



String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

// Avialable colors: Red, Black, Yellow, Green, White, Brown

void serialEvent() {
  if(DEBUG) {
    Serial.println("---------------------");
    Serial.println("Recivied transmission");
  }
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
  if(DEBUG) {
    Serial.println("Message: " + inputString);
  }
  if(inputString.startsWith("Reset")) {
    resetFlag = true;
  }
  else if(inputString.startsWith("Start")) {
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
  pinMode(doubleSpeedCounterPin, OUTPUT);
  pinMode(explodeBombCounterPin, OUTPUT);

  pinMode(pwmPin, INPUT);

  pwmState = 0;
  pwmStateOld = 0;
  inputString.reserve(200);
  resetFlag = true;
}

void reset() {
  Serial.println("Reseting game....");
  //Reset game
  //Send reset signal to counter. A high input will reset the counter to 5 min, when output goes low the counter starts counting.
  digitalWrite(resetCounterPin, HIGH);
  delay(1000); // Wait for counter to be reset.

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

  Serial.println("Ready!"); 
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
}

// Outputs if the wires has been cut correctly
boolean isWiresCutCorrect() {
  // If the red wire has not been cut.
  if(!isRedCut) {
    if (DEBUG) {
      Serial.println("Reading the red wire");
    }
    
    // Red wire has been cut correctly 
    if( digitalRead(redWirePin) == 1) {
      Serial.println("The red wire was cut! Correct!");
      isRedCut = true;
    }
  }

  // If the green wire has not been cut.
  if(!isGreenCut) {
    if (DEBUG) {
      Serial.println("Reading the green wire");
    }
    // Green wire has been cut 
    if(digitalRead(greenWirePin) == 1) {
      Serial.println("Idiot you cut the WRONG wire! (Green)");
      isGreenCut = true;
      wireWrongCount++;
    }
  }

  // If the brown wire has not been cut.
  if(!isBrownCut) {
    if (DEBUG) {
      Serial.println("Reading the brown wire");
    }
    if( digitalRead(brownWirePin) == 1) {
      Serial.println("Idiot you cut the WRONG wire! (Brown)");
      isBrownCut = true;
      wireWrongCount++;
    }
  }
  
  // If the white wire has not been cut.
  if(!isWhiteCut) {
    if (DEBUG) {
      Serial.println("Reading the white wire");
    }
    // White wire has been cut. 
    if( digitalRead(whiteWirePin) == 1) {
      Serial.println("Idiot you cut the WRONG wire! (White)");
      isWhiteCut = true;
      wireWrongCount++;
    }
  }

  
  // All wires has been cut corretly!
  if(isRedCut && wireWrongCount <= 1) {
      Serial.println("Yes, you have cut all the wires correctly!");
      return true;
  }
  else if (wireWrongCount > NUMBER_OF_ALLOWED_ERRORS) {
      Serial.println("You failed! The bomb will now explode! .... Idiot");
      return false;
  }   

  // Activate double speed if 1 wire is incorretly cut
  if(wireWrongCount >= NUMBER_OF_DOUBLE_SPEED) {
    digitalWrite(doubleSpeedCounterPin, HIGH);
  }
  
  return false;
}

void readPwmValue() {
  pwmValue = pulseIn(pwmPin, HIGH);
  if(DEBUG) {
    Serial.println("Reading PWM value: ");
    Serial.println(pwmValue);
  }

  pwmStateOld = pwmState;

  //PWM States
  // 0 = Reset
  // 1 = Start
  // 2 = TBD
  
  if((pwmValue >= 0) && (pwmValue <= 100)) {
    if(DEBUG) {
      Serial.println("PWM: Setting PWM state 0");
    }
    pwmState = 0;
  }
  else if((pwmValue >= 3100) && (pwmValue <= 3250)) {
    if(DEBUG) {
      Serial.println("PWM: Setting PWM state 1");
    }
    pwmState = 1;
  }
  else if((pwmValue >= 4700) && (pwmValue <= 4850)) {
    if(DEBUG) {
      Serial.println("PWM: Setting PWM state 2");
    }
    pwmState = 2;
  }

  if(DEBUG) {
    Serial.println("pwmState = " + String(pwmState));
    Serial.println("pwmStateOld = " + String(pwmStateOld));
  }
  
  if((pwmState == 0) && (pwmStateOld != 0)) {  
    if(DEBUG) {
      Serial.println("PWM: Setting reset flag");
    }
     resetFlag = 1;
  }
  else if((pwmState == 1) && (pwmStateOld != 1)) {  
    if(DEBUG) {
      Serial.println("PWM: Setting start flag");
    }
   startFlag = 1;
  }
  else if((pwmState == 2) && (pwmStateOld != 2)) {
   startFlag = 1;
  }




  

  
  
}

void loop() {
  if(resetFlag) {
    if(DEBUG) {
      Serial.println("Resetflag is set!");
    }
    reset();  
  }
  else if(startFlag) {
    // Start game
    // Set reset signal to counter low to start it. 
    digitalWrite(resetCounterPin, LOW);
    
    // Check if counter has gone kaboom! or if the wires has not been cut correctly
    if((digitalRead(kaboomCounterPin) == HIGH) || (!isWiresCutCorrect() && wireWrongCount > NUMBER_OF_ALLOWED_ERRORS)) {
      digitalWrite(explodeBombCounterPin, HIGH);
      Serial.println("KABOOM!");   
      startFlag = false;
    }
    // Check if all wires has been cut correctly
    else if(isWiresCutCorrect()) {
      Serial.println("Defused!");
      digitalWrite(resetCounterPin, HIGH);
      startFlag = false;
    }
    
    if(DEBUG) {
      outputWireStates();
    }
  }

  readPwmValue();
  delay(100);   
}
