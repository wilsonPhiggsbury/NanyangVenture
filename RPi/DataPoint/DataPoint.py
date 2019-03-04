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
        return super().__init__(canId, "SM", 1, 4) # average of Left & Right wheel speed (1x float)
    def packString(self):
        resultList = self._packStringDefault()
        resultList[2] = "{:.2f}".format(self.speedKmh)
        print(len(resultList))
        result = "\t".join(resultList)
        return result
    def unpackString(self, inputStr):
        resultList = self._unpackStringDefault(inputStr)
        if(resultList):
            self.speedKmh = float(resultList[2])
            # TODO: commit converted parameters into data array
            print("speed:",self.speedKmh)
        else:
            print("Did not unpack anything.")


# Test code for main
b = DataPoint(10, "YO")
print(b.packString())
b.unpackString("ha\tdeadbeef\tfe\tea\tda\t9e\t00\t00\t00\t00")

dataPointSpeedometer = NV11DataSpeedo(11)
stringSpeedometer = dataPointSpeedometer.packString()
print(stringSpeedometer)
tmpList = list(stringSpeedometer)
tmpList[-4:-1] = "1.54"
stringSpeedometer = ''.join(tmpList)
dataPointSpeedometer.unpackString(stringSpeedometer)
