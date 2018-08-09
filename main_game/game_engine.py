import serial
import time

DEBUG = 1

class Bomb:
	def __init__(self, name, comChannel):
		# instance variable unique to each instance
		self.name = name    
		self.serialChannel = serial.Serial(comChannel, 9600, timeout=2)
		self.reset()
		
	def reset(self):
		self.exploded = 0;
		self.defused = 0;
		self.ready = 0;
		self.active = 1;
		
		
		
bombs = {Bomb('OddEvenBomb', 'COM7'), Bomb('SerialNumberBomb', 'COM8')}
rxByte = '0'


def readSerialData(bomb):
	if DEBUG:
		print("Reading data from: " + bomb.name)
	try:
		rxByte = bomb.serialChannel.readline()
		print(bomb.name + ": " + rxByte.decode('utf-8'	))
		time.sleep(1)
	except ser.SerialTimeoutException:
		print('Data could not be read')
		time.sleep(1)
	return rxByte
	
def writeSerialData(bomb, var):
	if DEBUG:
		print("Sending " + var.decode('utf-8') + " to: " + bomb.name)
	bomb.serialChannel.write(var)
		
def main():
	print("*** Welcome to the Bomb Diffsual Game ***")
	# Reset game
	for bomb in bombs:
		writeSerialData(bomb, b'Reset\n')
	
	while 1:
		# Wait for ready signal from all arduinos
		for bomb in bombs:
			if(readSerialData(bomb) == b'Ready!\r\n'):
				bomb.ready = 1;
		if(all(bomb.ready == 1 for bomb in bombs)):
			print("All bombs are ready!")
			break
		time.sleep(1);
	
	for bomb in bombs:
		writeSerialData(bomb, b'Start\n')
	
	while 1:
		for bomb in bombs:
			str = readSerialData(bomb)
			if(str == b'KABOOM!\r\n'):
				print("Oh no! Bomb: " + bomb.name + " exploded!")
				bomb.exploded = 1
				bomb.active = 0
			elif(str == b'Defused!\r\n'):
				print("Pheww! Bomb: " + bomb.name + " was defused!")
				bomb.defused = 1
				bomb.active = 0
				
		if(all(bomb.exploded == 1 for bomb in bombs)):
			print("All bombs has exploded!")
			break
			
		if(all(bomb.defused == 1 for bomb in bombs)):
			print("All bombs has been defused!")
			break
			
		if(all(bomb.active == 0 for bomb in bombs)):
			print("Times out!")
			break	
		time.sleep(1);
		
main()


