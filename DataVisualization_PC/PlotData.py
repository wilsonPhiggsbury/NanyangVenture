import numpy as np
from matplotlib import animation
import matplotlib.pyplot as plt
from Data import getData
from MockDataSource import readStreamData

def viewGraph(fileType):
    fileTypeData = getData()[fileType]
    # Setup plot settings. Excludes Millis as a plot
    fig, ax = plt.subplots(len(fileTypeData) - 1, 1, sharex= True)
    ani = animation.FuncAnimation(fig, _animate, fargs=(ax, fileType, fileTypeData, readStreamData), interval = 1000)

    plt.show()

def _animate(i, ax, fileType, fileTypeData, readStreamData):
    # call readSteamData function
    readStreamData()
    # Setup plot for each data
    i = 0
    for header in fileTypeData:
        if header == "Millis":
            continue
        ax[i].clear()
        ax[i].plot(fileTypeData["Millis"], fileTypeData[header])
        ax[i].set_ylabel(header)
        i += 1

    # Format plot
    plt.xticks(rotation=45, ha='right')
    plt.subplots_adjust(bottom=0.30)