import struct
import time

class DataPoint:
    def __init__(self, canId, strHeader):
        self.canId = canId
        self.length = 8
        self.data = bytearray(8)
        self.strHeader = strHeader
        self.timeStamp = 0
        self.elements = 10 # 2 + num of properties of this data point

    def packString(self):
        resultList = list(0 for x in range(self.elements))
        resultList[0] = self.strHeader
        resultList[1] = struct.pack('L',self.timeStamp).hex()

        if(len(self.data) != 8):
            print("Data is nto 8 bytes long! Instead:",len(self.data))
        else:
            for i in range(self.elements-2):
                resultList[i+2] = struct.pack('B', self.data[i]).hex()
        result = "\t".join(resultList)
        return result

    def checkMatchString(self, inputStr):
        return inputStr[0:2] == self.strHeader

    def unpackString(self, inputStr):
        resultList = inputStr.split("\t")
        if(len(resultList) != self.elements):
            print("String segment number inadequate! Expecting", self.elements, "received", len(resultList))
        self.timeStamp = resultList[1]
        for i in range(self.elements-2):
            self.data[i] = struct.unpack('B', bytearray.fromhex(resultList[i+2]))[0]
            print(i, "unpacked:", self.data[i])
        print("payload:",self.data)


class NV11DataSpeedo(DataPoint):
    pass

b = DataPoint(10, "YO")
print(b.packString())
b.unpackString("ha\tdeadbeef\tfe\tea\tda\t9e\t00\t00\t00\t00")