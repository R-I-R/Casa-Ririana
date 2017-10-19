import time
import serial
from serial.serialutil import SerialException
import serial.tools.list_ports

class Board:

	port = ''
	description = ''
	serialNumber = ''
	device = ''

	def __init__(self, port, description, serialNumber):
		self.port = port
		self.description = description
		self.serialNumber = serialNumber

	def isAvailable(self):
		try:
			self.device = serial.Serial(self.port, 9600)
			return True
		except SerialException:
			print ("\tBOARD IN ",self.port,"IS IN USE BY OTHER PROGRAM OR YOU HAVE NOT PERMISSIONS")
			return False


	def sendMessage(self, message):
		time.sleep(2)
		self.device.write(message)
		self.device.close()

	def receiveMessage(self):
		attempts = 5
		result = ""
		while (result == "" and attempts > 0):
			time.sleep(1)
			while self.device.inWaiting() > 0:
				message = self.device.readline()
				if message:
					result = message
			attempts -= 1
		return result


	def toString(self):
		return self.port+": "+self.description

class BoardManager:

	def search(self):
		self.boards = []

		ports = list(serial.tools.list_ports.comports())

		for p in ports:
			if "Arduino" in p[1]:
				board = Board(p[0].strip(), p[1].strip(), p[2].strip())				
				if board.isAvailable():
					self.boards.append(board)

		return self.boards

	def getBoards(self):
		return self.boards


boardManager = BoardManager()

print ("Searching for boards...")
boards = boardManager.search()

print (len(boards),"boards found")
for board in boards:
	board.sendMessage(b'p')



