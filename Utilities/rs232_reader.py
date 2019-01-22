import serial
from time import sleep

while True:
	try:
		#mega = serial.Serial('COM5',19200)
		rs = serial.Serial('COM9',19200)
		break
	except serial.SerialException:
		print("Try again...")
thisline = ''
while True:
	while(rs.in_waiting):
		try:
			thisline = rs.read().decode()
			print(thisline,end='')
		except UnicodeDecodeError:
			print("Decode error!")