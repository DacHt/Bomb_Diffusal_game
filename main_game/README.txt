//Setup
Connect each arduino(bomb) to the usb hub and connect it to the computer.
Note the COM number for each bomb, in windows it is 'COM6' 'COM7' etc. Linux I don't now. 
Flash the arduinos with a bomb program, note which COM number corresponds to which bomb.

alter the bombs = {Bomb('OddEvenBomb', 'COM7'), Bomb('SerialNumberBomb', 'COM8')} in the game_engine.py

Run the python script it will:
1. Reset all bombs.
2. Wait for a ready signal from all bombs.
3. Start the game when all bombs are ready.
4. Monitor the serial input from each bomb for a 'Kaboom' or 'Defused' and sets the bomb deactive.
   The bombs will always send a 'Kaboom' or 'Defused'. The kaboom is sent when the counter reaches 0. 
5. If all bomb has exploded, been defused or are deactive the game ends. 

All bombs can be connected the same. 
Arduino pinout:

Arduino		
Pin			Function			Color				Function
													SerialNumberBomb		OddEvenBomb
The bomb wires connects to GND, optionally with a +5V pullup.
D2 		-	Red wire								Correct wire			Cut at even 10 sec
D3 		-	Green wire								Wrong wire				Cut at odd 10 sec
D4 		-	Brown wire								Wrong wire				Cut at even 10 sec
D5 		-	White wire	                            Wrong wire				Cut at odd 10 sec
                                                    
//Counter pins
D6 		-	ResetCounter	-	Yellow / Green		OUTPUT Set to HIGH for 2 sec when reseting the game 
D7 		-	KaboomCounter	-	Red / Black			INPUT Set to HIGH by the counter when timer reaches 0
D8 		-	Even10Counter	-	Green / Red			INPUT Set to HIGH when counter is on even 10 sec, LOW when on odd 10 sec
D9 		-	DoubleSpeed		-	Red / Blue			OUTPUT Set to HIGH when double speed is activated
D10 	-	ExplodeCounter	-	White / Brown		OUTPUT Set to HIGH when game logic wants to explode bomb, set to HIGH when counter reach 0

The counter can be supplied 5V directly from arduino.
+5V - Brown/Red
GND - Brown/Black

// SerialNumberBomb
A counter is counting down from 10 minutes. If the counter reaches 0 the bomb has exploded.
The RED wire is the correct wire to cut, if the RED wire is cut the bumb will be defused!
If any other wire (Green, Brown, White) is cut it will result in a wrong cut.
The first wrong cut the counter will go double the speed, but not explode.
The second wrng cut the bomb will explode.

// OddEvenBomb:
A counter is counting down from 10 minutes. If the counter reaches 0 the bomb has exploded.
At an even 10 of a second either the RED or the BROWN wire has to be cut. 
At odd 10 seconds the GREEN or WHITE wire has to be cut.
If a wire is cut at the wrong time the counter will go double the speed, but not explode and is still defusable.
If a wire is cut at the wrong time a second time the bomb will NOT explode and is still defusable.
If a wire is cut at the wrong time a third time the bomb WILL explode.
The second wrng cut the bomb will explode.

The counter will output a high signal when at even 10 seconds.
 
 