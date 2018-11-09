from random import randint
from Data import updateData, getData

def readStreamData():
    with open("SampleLogs/CS.TXT") as f:
        # remove header row
        lines = f.readlines()[1:]
        randomLine = lines[randint(0, len(lines)-1)]

        updateData(randomLine)