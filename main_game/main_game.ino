const int DEBUG = 0;

const int redWirePin = 2;
const int greenWirePin = 3;
const int brownWirePin = 4;
const int whiteWirePin = 5;

const int resetCounterPin = 6;
const int kaboomCounterPin = 7;
const int even10CounterPin = 8;
const int doubleSpeedCounterPin = 9;
boolean isRedCut = false;
boolean isGreenCut = false;
boolean isBrownCut = false;
boolean isWhiteCut = false;

int wireWrongCount = 0;

// Avialable colors: Red, Black, Yellow, Green, White, Brown

// Mini game one:
// A counter is counting down from 5 minutes. If the counter reaches 0 the bomb has exploded.
// At an even 10 of a second either the red or the brown wire has to be cut. 
// At odd 10 seconds the green or white wire has to be cut.
// The counter will output a high signal when at even 10 seconds.
 

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
  Serial.write("Welcome to the bomb diffusal game!\n");

  //Reset game
  //Send reset signal to counter. A high input will reset the counter to 5 min, when output goes low the counter starts counting.
  digitalWrite(resetCounterPin, HIGH);
  delay(1000); // Wait for counter to be reset.

  // Set double speed to LOW
  digitalWrite(doubleSpeedCounterPin, LOW);
  
  isRedCut = false;
  isGreenCut = false;
  isBrownCut = false;
  isWhiteCut = false;
  wireWrongCount = 0;
  
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

  Serial.print("Even 10 pin is: ");
  Serial.println(digitalRead(even10CounterPin));
}

// Outputs if the wires has been cut correctly
boolean isWiresCutCorrect() {
  // If the red wire has not been cut.
  if(!isRedCut) {
    if (DEBUG) {
      Serial.println("Reading the red wire");
    }
    // Red wire has been cut correctly 
    if(digitalRead(redWirePin) == 1 && digitalRead(even10CounterPin) == 1) {
      Serial.println("The red wire was cut at the right time, pheew!");
      isRedCut = true;
    }
    // Red wire has been cut, but att the wrong time!
    else if( digitalRead(redWirePin) == 1 && digitalRead(even10CounterPin) == 0) {
      Serial.println("Idiot you cut the wrong wire! (Red)");
      isRedCut = true;
      wireWrongCount++; 
    }
  }

  // If the green wire has not been cut.
  if(!isGreenCut) {
    if (DEBUG) {
      Serial.println("Reading the green wire");
    }
    // Green wire has been cut correctly 
    if(digitalRead(greenWirePin) == 1 && digitalRead(even10CounterPin) == 0) {
      Serial.println("The green wire was cut at the right time, pheew!");
      isGreenCut = true;
    }
    // Green wire has been cut, but att the wrong time!
    else if( digitalRead(greenWirePin) == 1 && digitalRead(even10CounterPin) == 1) {
      Serial.println("Idiot you cut the wrong wire! (Green)");
      isGreenCut = true;
      wireWrongCount++;
    }
  }

  // If the brown wire has not been cut.
  if(!isBrownCut) {
    if (DEBUG) {
      Serial.println("Reading the brown wire");
    }
    // Brown wire has been cut correctly 
    if(digitalRead(brownWirePin) == 1 && digitalRead(even10CounterPin) == 1) {
      Serial.println("The brown wire was cut at the right time, pheew!");
      isBrownCut = true;
    }
    // Brown wire has been cut, but att the wrong time!
    else if( digitalRead(brownWirePin) == 1 && digitalRead(even10CounterPin) == 0) {
      Serial.println("Idiot you cut the wrong wire! (Brown)");
      isBrownCut = true;
      wireWrongCount++;
    }
  }
  
  // If the white wire has not been cut.
  if(!isWhiteCut) {
    if (DEBUG) {
      Serial.println("Reading the white wire");
    }
    // White wire has been cut correctly 
    if(digitalRead(whiteWirePin) == 1 && digitalRead(even10CounterPin) == 0) {
      Serial.println("The white wire was cut at the right time, pheew!");
      isWhiteCut = true;
    }
    // White wire has been cut, but att the wrong time!
    else if( digitalRead(whiteWirePin) == 1 && digitalRead(even10CounterPin) == 1) {
      Serial.println("Idiot you cut the wrong wire! (White)");
      isWhiteCut = true;
      wireWrongCount++;
    }
  }
  // All wires has been cut corretly!
  if( isRedCut && isGreenCut && isBrownCut && isWhiteCut) {
    if(wireWrongCount <= 2) {
      Serial.println("Yes, you have cut all the wires correctly!");
      return true;
    }
    else {
      Serial.println("You failed! Await your dooom! .... Idiot");
      return false;
    }   
  }

  return false;
}

void loop() {
  // Start game
  // Set reset signal to counter low to start it. 
  digitalWrite(resetCounterPin, LOW);
  
  // Check if counter has gone kaboom! or if the wires has not been cut correctly
  if(digitalRead(kaboomCounterPin) == HIGH) {
    Serial.println("KABOOM!");   
    delay(10000000);
  }
  // Check if all wires has been cut correctly
  else if(isWiresCutCorrect()) {
    Serial.println("Bomb has been diffused");
    digitalWrite(resetCounterPin, HIGH);
    delay(10000000);
  }

  // Activate double speed if 1 wire is incorretly cut
  if(wireWrongCount > 0) {
    digitalWrite(doubleSpeedCounterPin, HIGH);
  }
  
  if(DEBUG) {
    outputWireStates();
  }
  delay(1000);   

}
