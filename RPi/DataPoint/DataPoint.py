import struct
import time

class DataPoint:
    def __init__(self, canId, strHeader, numParameters=8, numBytes=8):
        # num Parameters are: how many fragments will appear in stringified data
        # num Bytes are: how many bytes will be in CAN bus
        self.canId = canId
        self.strHeader = strHeader
        self.timeStamp = 0
        self.numParameters = 2+numParameters # Length of data in bytes. 10 = 2 (strHeader, timestamp) + 8 (num of properties of this data point. In this case, 8 raw byte values)
        self.canLength = numBytes
        self.data = bytearray(numParameters)
    def __str__(self):
        return self.packString()
    def _packStringDefault(self):
        resultList = list(0 for x in range(self.numParameters))
        resultList[0] = self.strHeader
        resultList[1] = struct.pack('L',self.timeStamp).hex()
        return resultList

    def packString(self):
        resultList = self._packStringDefault()
        if(len(self.data) != 8):
            print("Data is not 8 bytes long! Instead:",len(self.data))
        else:
            for i in range(self.numParameters-2):
                resultList[i+2] = struct.pack('B', self.data[i]).hex()
        result = "\t".join(resultList)
        return result

    def checkMatchString(self, inputStr):
        return inputStr[0:2] == self.strHeader
    
    def _unpackStringDefault(self, inputStr):
        resultList = inputStr.split("\t")
        if(len(resultList) != self.numParameters):
            print("String segment number inadequate! Expecting", self.numParameters, "received", len(resultList))
            return None
        self.timeStamp = resultList[1]
        return resultList

    def unpackString(self, inputStr):
        resultList = self._unpackStringDefault(inputStr)
        if(resultList):
            for i in range(self.numParameters-2):
                self.data[i] = struct.unpack('B', bytearray.fromhex(resultList[i+2]))[0]
                print(i, "unpacked:", self.data[i], "type:", type(self.data[i]))
            print("payload:",self.data)
        else:
            print("Did not unpack anything.")
        

class NV11DataSpeedo(DataPoint):
    def __init__(self, canId):
        self.speedKmh = 0.0
        return super().__init__(canId, "SM", 1, 4) # 1 data, 4 bytes
    def packString(self):
        resultList = self._packStringDefault()
        resultList[2] = "{:.2f}".format(self.speedKmh)
        result = "\t".join(resultList)
        return result
    def unpackString(self, inputStr):
        resultList = self._unpackStringDefault(inputStr)
        if(resultList):
            self.speedKmh = float(resultList[2])
            # TODO: commit converted parameters into data array
            print("Received Speedometer")
            print("km/h:",self.speedKmh)
        else:
            print("Did not unpack anything.")
            
class NV11DataAccessories(DataPoint):
    def __init__(self, canId):
        self.lsig = 0
        self.rsig = 0
        self.hazard = 0
        self.headlights = 0
        self.brake = 0
        self.wiper = 0
        self.fourWS = 0
        self.regen = 0
        return super().__init__(canId, "ST", 8, 8) # 8 data, 8 bytes
    def packString(self):
        resultList = self._packStringDefault()
        resultList[2] = str(self.lsig)
        resultList[3] = str(self.rsig)
        resultList[4] = str(self.hazard)
        resultList[5] = str(self.headlights)
        resultList[6] = str(self.brake)
        resultList[7] = str(self.wiper)
        resultList[8] = str(self.fourWS)
        resultList[9] = str(self.regen)
        result = "\t".join(resultList)
        return result
    def unpackString(self, inputStr):
        resultList = self._unpackStringDefault(inputStr)
        if(resultList):
            self.lsig = int(resultList[2])
            self.rsig = int(resultList[3])
            self.hazard = int(resultList[4])
            self.headlights = int(resultList[5])
            self.brake = int(resultList[6])
            self.wiper = int(resultList[7])
            self.fourWS = int(resultList[8])
            self.regen = int(resultList[9])
            # TODO: commit converted parameters into data array
            print("Received Accessories")
            print("lsig rsig hazard headlight brake wiper fourWS regen:", self.lsig, self.rsig, self.hazard, self.headlights, self.brake, self.wiper, self.fourWS,self.regen)
        else:
            print("Did not unpack anything.")

class NV11DataBMS(DataPoint):
    def __init__(self, canId):
        self.volt = 0
        self.amp = 0
        self.temperature = 0
        return super().__init__(canId, "BM", 3, 6) # 8 data, 8 bytes
    def packString(self):
        resultList = self._packStringDefault()
        resultList[2] = str(self.volt)
        resultList[3] = str(self.amp)
        resultList[4] = str(self.temperature)
        result = "\t".join(resultList)
        return result
    def unpackString(self, inputStr):
        resultList = self._unpackStringDefault(inputStr)
        if(resultList):
            self.volt = int(resultList[2])
            self.amp = int(resultList[3])
            self.temperature = int(resultList[4])
            # TODO: commit converted parameters into data array
            print("Received BMS")
            print("volt amp temperature:", self.volt, self.amp, self.temperature)
        else:
            print("Did not unpack anything.")