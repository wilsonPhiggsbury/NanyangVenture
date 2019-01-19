import serial
import random
from time import sleep

towrite = '>>58.7V 00.8A 0051W 00000Wh 023.0C 028.9C 029.7C 029.2C 0.86B 31.0V 035.3C OP 41.2C 20 0205'
towrite_long = [1,2,3,4,5,6,7,8,9,0,1]
towrite_long[0] = "000.0C 000.0C 000.0C 000.0C 0.10B 00.0V 000.0C IN 00.0C 00 0000"
towrite_long[1] = "000.0C 000.0C 000.0C 000.0C 0.20B 00.0V 000.0C IN 00.0C 00 0000"
towrite_long[2] = "019.3C 026.2C 026.3C 026.3C 0.30B 34.8V 026.6C IN 00.0C 00 0000"
towrite_long[3] = "021.7C 028.9C 029.2C 032.4C 0.43B 52.9V 028.1C IN 41.5C 20 0214"
towrite_long[4] = "021.8C 028.9C 029.3C 032.5C 0.65B 52.9V 028.2C IN 41.5C 20 0214"
towrite_long[5] = "021.8C 029.0C 029.4C 032.6C 0.75B 52.9V 028.2C OP 41.4C 20 0213"
towrite_long[6] = "021.9C 029.0C 029.5C 032.7C 0.95B 53.6V 028.2C OP 41.4C 20 0212"
towrite_long[7] = "022.0C 029.1C 029.6C 032.8C 1.12B 53.6V 028.2C OP 41.4C 20 0213"
towrite_long[8] = "022.0C 029.2C 029.7C 032.9C 1.20B 53.6V 028.1C OP 41.5C 20 0214"
towrite_long[9] = "022.2C 029.2C 029.8C 033.0C 1.31B 53.6V 028.2C OP 41.4C 20 0213"
towrite_long[10] = "022.3C 029.3C 029.9C 033.1C 0.84B 53.6V 028.3C OP 41.4C 20 0212"
# towrite_long[0] = "O.O"
# towrite_long[1] = ">"
# towrite_long[2] = ">"
# towrite_long[3] = " "
# towrite_long[4] = "55.3V "
# towrite_long[5] = "34.2A "
# towrite_long[6] = "0033W "
# towrite_long[7] = "11111"
# towrite_long[8] = ""
# towrite_long[9] = ""
# towrite_long[10] = ""
towrite_rubbish = [1,2,3,4,5]
towrite_rubbish[0] = "H182_noSync_v1.3.1"
towrite_rubbish[1] = "v1.3.1"
towrite_rubbish[2] = "System Init."
towrite_rubbish[3] = "Starting Up"
towrite_rubbish[4] = "Random rubbish from the\r\nFuel Cell to screw up my beautiful\r\nday!"


rs = serial.Serial('COM5',19200)
counter = 0
hits = 0
while True:
	if counter >= 10:
		counter = 0
	else:
		counter += 1
	prepend = '>>' + str(random.randint(0,100)/10).zfill(4) + 'V ' + \
                  str(random.randint(0,20)/10).zfill(4) + 'A ' + str(random.randint(100,1000)).zfill(4) + 'W ' + str(random.randint(0,100)).zfill(5) + 'Wh '

	printValid = random.randint(0,7) > 3
	if(printValid):
		hits += 1
		contents = prepend+towrite_long[counter]
	else:
		contents = towrite_rubbish[counter%5]

	contents += '\r\n'
	print('Written:',rs.write(contents.encode()))
	


	if(rs.out_waiting):
		print(rs.out_waiting)
	else:
		print('Sent line',counter,'----:',contents)
	sleep(1.0)
