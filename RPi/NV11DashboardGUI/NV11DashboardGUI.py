try:
    import Tkinter as tk # this is for python2
except:
    import tkinter as tk # this is for python3
import time
import colorsys



ELEMENT_SIZE = 125
DEFAULT_LIGHT_COLOR = "#a3abb7"
NO_OF_TELEMETRYCAGES = 6
FONT_SIZE = 15

SPEEDO_FONT_SIZE = 30

GREEN = "#42f47a"




## serial imports
import sys
import glob
import serial
from DataPoint import NV11DataAccessories, NV11DataSpeedo


#threading imports
from threading import Thread


#[x,y ,row span, column span,description, height, width]
pouch_pannel_configuration = [0 for i in range(NO_OF_TELEMETRYCAGES)]
pouch_pannel_configuration[0] = [0,0,1,1,"left", ELEMENT_SIZE *2, ELEMENT_SIZE] #left signal light
pouch_pannel_configuration[1] = [3,0,1,1,"right", ELEMENT_SIZE *2, ELEMENT_SIZE] #right signal light
pouch_pannel_configuration[2] = [0,1,1,1, "throttle",ELEMENT_SIZE, ELEMENT_SIZE] #throttle 
pouch_pannel_configuration[3] = [0,2,1,1,"brake", ELEMENT_SIZE, ELEMENT_SIZE] #brake
pouch_pannel_configuration[4] = [1,1,2,3,"message", ELEMENT_SIZE *2, ELEMENT_SIZE*3] #message box
pouch_pannel_configuration[5] = [1,0,1,2,"speed", ELEMENT_SIZE *2, ELEMENT_SIZE*2] #speed 


class DriveUserInterface(tk.Frame):
    def __init__(self, master = None):
        tk.Frame.__init__(self, master)
        self.scrollbar = None
        self.grid()
        self.telemetrycages = []
        self.createCells(NO_OF_TELEMETRYCAGES)
        
    def createCells(self, no_of_pannels):
        for i in range(NO_OF_TELEMETRYCAGES):
            configuration = pouch_pannel_configuration[i]
            x = configuration[0]
            y = configuration[1]
            span_x = configuration[2]
            span_y = configuration[3]
            description = configuration[4]
            height = configuration[5]
            width = configuration[6]
            new_pannel = UI_switcher(self, height, width, description) #switcher here 
##            new_pannel = TelemetryCage(self, height, width, description)
            self.telemetrycages.append(new_pannel)
            new_pannel.grid(column = x, row = y, columnspan = span_y, rowspan = span_x)
        self.initText()


    def setText(self,x ,y):     
##         new_text = str(self.scrollbar.get())
         self.text_to_change_var.set(str(x +"\n " + y))
         self.scrollbar.set(y, 0)
            
##        listbox = tk.Listbox(self, yscrollcommand=scrollbar.set)
##        for i in range(1000):
##            listbox.insert(tk.END, str(i))
##        listbox.pack(side=tk.LEFT, fill=tk.BOTH)
##
##        scrollbar.config(command=listbox.yview)
        
    def initText(self):
        for element in self.telemetrycages:
            element.initText()


    def updateDriveUI(self, dataAccessory, dataSpeed):
        self.UISelector("speed").setText(dataSpeed.speedKmh)
        self.UISelector("left").setText(dataAccessory.lsig)
        self.UISelector("right").setText(dataAccessory.rsig)
        self.UISelector("brake").setText(dataAccessory.brake)
        
    def UISelector(self, description):
        for cage in self.telemetrycages:
            if cage.description == description:
                return cage
        

class TelemetryElement(tk.Canvas):
    def __init__(self, parent, passHeight, passWidth, passDescription):
        #default color set as white
        self.color = "#000" #rgb format, hexidecimal
        self.width = passWidth
        self.height = passHeight
        tk.Canvas.__init__(self, parent, background = DEFAULT_LIGHT_COLOR, height = passHeight, width = passWidth)
        self.description = passDescription
        self.text = ""

    #to be overwritten if necessary
    def initText(self):
        self.text_id = self.create_text((ELEMENT_SIZE//2,ELEMENT_SIZE), text=self.description, font=("Courier", FONT_SIZE),anchor=tk.CENTER)
        xOffset = self.findXCenter(self, self.text_id)
##        self.move(self.text_id, xOffset, 0)


    def findXCenter(self, canvas, item):
      coords = canvas.bbox(item)
      print("Coords", coords)
      xOffset = (self.width / 2) - ((coords[2] - coords[0]) / 2)
      print("window width", self.width)
      print("xOffset", xOffset)
      return -1  * xOffset

        
    def setColor(self, new_color):
        self.configure(background = new_color)

    def setText(self, text_to_change ):
        text_to_change = str(text_to_change)
        self.itemconfigure(self.text_id, text=text_to_change)
        self.text = text_to_change
        self.indicate()

    def indicate(self):
        if self.text == "1":
            self.setColor(GREEN)
        else:
            self.setColor(DEFAULT_LIGHT_COLOR)
        

#switch case python style


##    elif description == "left":
##        return left_indcator
##    elif description == "right":
##        return right_indcator
##    elif description == "throttle":
##        throttle_element()
##    elif description == "brake":
##        return brake_element


def UI_switcher(parent, height, width, description):
    if description == "speed":
        return Speedometer(parent, height, width, description)
    elif description == "throttle" or description == "brake":
        return ThrottleBrake(parent, height, width, description)
    else:
        return TelemetryElement(parent, height, width, description)
    



#funciton to find the horizontal center



##class TurnIndicator(TelemetryElement):
##    def __init__(self,parent, passWidth, passHeight, description):
##        TelemetryElement.__init__(self,parent, passWidth, passHeight, description)
####        if is_left:
####            points = [55, 85, 55, 155, 0, 120, 55, 85]
####        else:
####            points = [55, 85,  55, 155, 105, 120, 55, 85]
####        self.create_polygon(points, outline='#f11', 
####            fill='#1f1', width=2)
####        self.pack(fill=tk.BOTH, expand=1)
##        self.indicate()
##    

class Speedometer(TelemetryElement):
    def __init__(self, parent, passWidth, passHeight, description):
        TelemetryElement.__init__(self,parent, passWidth, passHeight, description)

    def initText(self): #override
        self.text_id = self.create_text((ELEMENT_SIZE//2,ELEMENT_SIZE), text=self.description, font=("Courier", SPEEDO_FONT_SIZE), anchor=tk.W)
        xOffset = self.findXCenter(self, self.text_id)
##        self.move(self.text_id, xOffset, 0)
    def indicate(self):
        rgb = hsv_hex_conversion(float(self.text),1,1)
        self.setColor(rgb)


class Throttle(TelemetryElement):
##    def __init__(self, parent, passWidth, passHeight, description):
##        TelemetryElement.__init__(self,parent, passWidth, passHeight, description)

    def initText(self): #override
        self.text_id = self.create_text((ELEMENT_SIZE//2,ELEMENT_SIZE), text=self.description, font=("Courier", D), anchor=tk.W)
        xOffset = self.findXCenter(self, self.text_id)

class ThrottleBrake(TelemetryElement):
    def initText(self): #override
        self.text_id = self.create_text((ELEMENT_SIZE//2,ELEMENT_SIZE), text=self.description, font=("Courier", FONT_SIZE), anchor=tk.S)
##        self.move(self.text_id, 0, -10)
        xOffset = self.findXCenter(self, self.text_id)



def funct1():
    ser = serial.Serial(serial_ports()[0])
    print(temperature_array)
    while True:
        data = ser.read_until()
        data = data.decode('UTF-8') #conversion to string
        module, temperature, index = parse_temp(data)
##        print("index", index)
##        print("module", module)
        temperature_array[module][index] = temperature
        



def funct2():
    print("Display")
    app = DriveUserInterface()
    app.master.title('Thermal Management System')
    ##########
    s = serial.Serial("/dev/serial0", baudrate=9600, timeout=1)
    dataSpeed = NV11DataSpeedo(0x0A)
    dataAcc = NV11DataAccessories(0x10)
    while True:
        line = s.readline().decode()
        if line is None:
            continue
        if dataSpeed.checkMatchString(line):
            dataSpeed.unpackString(line)
            
        elif dataAcc.checkMatchString(line):
            dataAcc.unpackString(line)
            
        for i in range(NO_OF_TELEMETRYCAGES):
            app.updateDriveUI(dataAcc, dataSpeed)
        ##            i = twoCompliment(int(ser.readline()))
        ##            j = scale(i, 100, 130, 150,200)
        ##            print(j)
##            new_color = hsv_hex_conversion(analytics_array[i][2], 1, 1)
        ##            new_color = hsv_hex_conversion(scale(new_color, 100, 130, 175,230), 1, 1)
##            pannel.changeColor(new_color)
            #truncate 1 dp
##            pannel.changeText(max_temp)
            app.update()


## To convert HSV into simulation friendly hex codes
# h values(min:0 max:100), s values(min:0 max:1), v values(min:0 max:1)

def my_map(s, min_s = 0, max_s = 100, min_h = 0, max_h =45):
    h = max_h - s * (max_h - min_h) / (max_s - min_s)
    return h

def hsv_hex_conversion(h, s, v):
    h = my_map(h)
    rgb = colorsys.hsv_to_rgb(h/100, s, v)
    r = hex(int(rgb[0]* 255))[2:]
    g = hex(int(rgb[1]* 255))[2:]
    b = hex(int(rgb[2]* 255))[2:]
    if len(r) == 1:
        r = r + r
    if len(g) == 1:
        g = g + g
    if len(b) == 1:
        b = b + b    
    return ('#' + r + g + b)            
        

if __name__ == '__main__':
##    Thread(target = funct1).start()
    Thread(target = funct2).start()





 

##app.after(1, task1)
##app.mainloop()




