import numpy as np
from matplotlib import animation
import matplotlib.pyplot as plt
from MockDataSource import readData
import time

# Setup plot settings
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
xdata, ydata = [], []

def animate(i, xdata, ydata):
    headers = readData()
    xdata.append(headers["Millis"])
    ydata.append(headers["V_cI"])

    # Limit x and y lists to 20 items
    xdata = xdata[-20:]
    ydata = ydata[-20:]

    ax.clear()
    ax.plot(xdata, ydata)

    # Format plot
    plt.xticks(rotation=45, ha='right')
    plt.subplots_adjust(bottom=0.30)
    plt.title('Log')
    plt.ylabel('V_cI')

ani = animation.FuncAnimation(fig, animate, fargs=(xdata, ydata), interval = 200)

plt.show()