import pickle
import os
import subprocess
from time import sleep
try:
	import serial
	import serial.tools.list_ports
except:
	print("Copy this into your cmd and try again:")
	print("C:\\Users\\%USERNAME%\\AppData\\Local\\Programs\\Python\\Python36-32\\Scripts\\pip install pyserial")
	#print("""Please type \"C:\\Users\\MX\\AppData\\Local\\Programs\\Python\\Python36-32\\Scripts\\pip install pyserial\" into your cmd and try again.""")
	input()
	exit(0)

boardConfigs = {'avr':['mega'],'sam':['due']}
def findArch(boardName):
	for key in boardConfigs:
		for boardName_ in boardConfigs[key]:
			if boardName==boardName_:
				return key
boardNames = []
for value in boardConfigs.values():
	for boardName in value:
		boardNames.append(boardName)
recognizedBoards = {}# list of recognized board types, boardName:[boardID, sketchName]
recognizedPorts = {}# acts as a queue for boards to be uploaded, boardName:[COM*:sketchToUpload]
sketches = {}# board-to-sketch matching
uploadingNone = True
for boardName in boardNames:
	sketches[boardName] = list()
	recognizedPorts[boardName] = dict()
# load compiled code into a table of contents
for fileName in os.listdir('binaries'):
	for name in sketches.keys():
		if name in fileName and 'bootloader' not in fileName:
			sketches[name].append(fileName)
			break
#-----------------------------------------------------------
# load pickle file
exists = os.path.isfile(os.getcwd()+'/mappings.p')
if not exists:
	for boardName in boardNames:
		recognizedBoards[boardName] = dict()
		recognizedBoards[boardName][" "] = ""#<-- have to sidestep a bug to have a dummy "initiate" the 'for boardID in list(recognizedBoards[boardName])' below
	with open('mappings.p', 'wb') as f:
		pickle.dump(recognizedBoards, f)
		print("Created new file.\n")
else:
	with open('mappings.p', 'rb') as f:
		recognizedBoards = pickle.load(f)

print("NanyangVenture Arduino code-deployer v1.0")
print("***************************************************")
print("This program will remember all previous Arduino-to-code associations for labelling conveniences.")
print("In case you want to reuse an Arduino for a different code, delete the file \"mappings.p\" to force the code-deployer to forget everything.")
print("***************************************************")

#-----------------------------------------------------------
# grab ports to know which ones to auto upload
ports = serial.tools.list_ports.comports()
for device in ports:
	devicePort_plugged = device[0]
	deviceName_plugged = device[1]
	deviceID_plugged = device[2]
	# verify device id against stored ids
	for boardName in recognizedBoards.keys():
		#if this board is a recognized arduino type, search for matching id
		if boardName in deviceName_plugged.lower():
			if recognizedBoards[boardName].get(deviceID_plugged) == None:
				# matching ID not found, add this ID/upload pair into the list
				print("New Arduino board detected! Please choose a sketch for this Arduino",boardName.upper(),'@',devicePort_plugged,':\n')
				for i in range(len(sketches[boardName])):
					sketchName = sketches[boardName][i]
					print(i,sketchName)
				print('\nChoice:',end='')
				choice = -1
				while choice<0 or choice>=len(sketches[boardName]):
					choice = int(input())
				recognizedBoards[boardName][deviceID_plugged] = sketches[boardName][choice]
			for boardID in recognizedBoards[boardName].keys():
				# queue all board for uploading
				recognizedPorts[boardName][devicePort_plugged] = recognizedBoards[boardName][boardID]
				uploadingNone = False
			break
# save new table of contents!
with open('mappings.p', 'wb') as f:
		pickle.dump(recognizedBoards, f)

if not uploadingNone:
	for boardName in recognizedPorts.keys():
		for port in recognizedPorts[boardName].keys():
			print(port, 'will be loaded with the sketch', recognizedPorts[boardName][port])
	print('\nPress ENTER to confirm.')
	input()
else:
	print("No supported Arduino model is connected to the computer.")
	print("""If you have not installed Arduino before, you can:
	 Navigate to Device Manager
	 Right click the unknown COM device
	 lLcate and install manually by pointing to the "drivers" folder """)
	input()
	exit(0)

# load'em'up
loadingProcesses = list()
for boardName in recognizedPorts.keys():
	for port in list(recognizedPorts[boardName]):
		print("++++++++++++++++++++++++++++++")
		print("Uploading to Arduino",boardName.upper(),"on",port,"using sketch","\""+recognizedPorts[boardName][port]+"\"")
		architecture = findArch(boardName)
		if architecture=='sam':
			command1 = ' '.join(["mode.com",port+":1200"])
			command2 = ' '.join(["tools\\bossac.exe","-i","-d","-p"+port,"-U","false","-e","-w","-v","-b","binaries\\"+recognizedPorts[boardName][port],"-R"])
			output = subprocess.call(command1,stdout=subprocess.DEVNULL)
			sleep(0.2)
			output = subprocess.call(command2,stdout=subprocess.DEVNULL)
		elif architecture=='avr':
			output = subprocess.call(["tools\\avrdude.exe","-v","-p"+"atmega2560","-cwiring","-q","-q","-q","-P"+port,"-b115200","-D","-Uflash:w:binaries\\"+recognizedPorts[boardName][port]+":i"],stdout=subprocess.DEVNULL)
		#output = subprocess.check_output(["upload_script.bat",boardName,port,recognizedPorts[boardName][port]])
		print("Upload for device at",port,"has",end=' ')
		if output==0:
			print('succeeded')
			del recognizedPorts[boardName][port]
		else:
			print('FAILED')
		print("------------------------------")

allSuccess = True
for boardName in recognizedPorts.keys():
	boardSuccess = len(recognizedPorts[boardName])==0
	if not boardSuccess:
		print("Arduino",boardName.upper(),"which had failed uploads:")
		for port in recognizedPorts[boardName].keys():
			print(port, recognizedPorts[boardName][port])
	allSuccess &= boardSuccess
if allSuccess:
	print("All boards updated!")
	input()