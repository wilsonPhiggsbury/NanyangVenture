def parseData(stream):
    streamList = stream[3:].replace("/t", " ").split()
    return streamList
