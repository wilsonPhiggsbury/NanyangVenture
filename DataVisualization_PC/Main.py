import sys
from Data import getData
from PlotData import viewGraph

if len(sys.argv) != 2:
    print("Invalid number of inputs! Only include 1 file type.")
else:
    for header in getData():
        if sys.argv[1] == header:
            viewGraph(sys.argv[1])
            break
        else:
            print("Wrong file type! Check input.")
