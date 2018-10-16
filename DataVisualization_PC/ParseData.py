def parseData(stream):
    streamList = stream[3:].replace("/t", " ").split()
    # streamList[0] = int(streamList[0], 16)
    for i in range(1, len(streamList)-1):
        streamList[i] = float(streamList[i])
    return streamList
