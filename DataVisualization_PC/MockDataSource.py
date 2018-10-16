from random import randint
from ParseData import parseData

headers = {
    "Millis": 0,
    "V_cI": 0,
    "A_cI": 0,
    "V_cO": 0,
    "A_cO": 0,
    "V_MT": 0,
    "A_MT": 0
    }

def readData():
    with open("SampleLogs/CS.TXT") as f:
        # remove header row
        lines = f.readlines()[1:]
        randomLine = lines[randint(0, len(lines)-1)]
        dataList = parseData(randomLine)

        j = 0
        for item in headers:
            headers[item] = dataList[j]
            j += 1

        return headers