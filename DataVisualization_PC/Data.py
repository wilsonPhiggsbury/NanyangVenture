from collections import deque

def updateData(stream):
    # parse data stream
    dataList = _parseData(stream)

    # Obtain fileType from dataList
    fileType = dataList[0]

    # Store received info to the correct header fields
    j = 1
    for item in data[fileType]:
        # Skip dataType section
        if item == fileType:
            pass
        data[fileType][item].popleft()
        data[fileType][item].append(dataList[j])
        j += 1

def getData():
    return data

# Parses data in order. Removes tabs and spaces to form a list
def _parseData(stream):
    streamList = stream.replace("/t", " ").split()
    # Uncomment on real input data
    # streamList[1] = int(streamList[1], 16)
    for i in range(2, len(streamList)):
        streamList[i] = float(streamList[i])
    return streamList

# Structure the data in order of receiving to parse. Make sure Millis is first!
data = {
    "CS": {
        "Millis": deque([0]* 20),
        "V_cI": deque([0]* 20),
        "A_cI": deque([0]* 20),
        "V_cO": deque([0]* 20),
        "A_cO": deque([0]* 20),
        "V_MT": deque([0]* 20),
        "A_MT": deque([0]* 20)
    },
    "FC": {
        "Millis": deque([0]* 20),
        "V_m": deque([0]* 20),
        "A_m": deque([0]* 20),
        "W_m": deque([0]* 20),
        "Wh_m": deque([0]* 20),
        "Tmp_m": deque([0]* 20),
        "Pres_m": deque([0]* 20),
        "Vcap_m": deque([0]* 20),
        "State_m": deque([0]* 20),
        "V_s": deque([0]* 20),
        "A_s": deque([0]* 20),
        "W_s": deque([0]* 20),
        "Wh_s": deque([0]* 20),
        "Tmp_s": deque([0]* 20),
        "Pres_s": deque([0]* 20),
        "Vcap_s": deque([0]* 20),
        "State_s": deque([0]* 20)
    },
    "SM": {
        "Millis": deque([0]* 20),
    }
}