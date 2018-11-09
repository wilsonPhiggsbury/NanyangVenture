import sys
from Data import getData
from PlotData import viewGraph

if len(sys.argv) != 2:
    print("Invalid number of inputs! Only include 1 file type.")
    sys.exit()
else:
    input = sys.argv[1]
    for header in getData():
        if input == header:
            viewGraph(input)
            sys.exit()
    print("Wrong file type! Check input.")
