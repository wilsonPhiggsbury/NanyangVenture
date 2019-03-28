try:
    import Tkinter as tk # this is for python2
except:
    import tkinter as tk # this is for python3
import time
import colorsys




ELEMENT_SIZE = 125
DEFAULT_LIGHT_COLOR = "#000000"
NO_OF_TELEMETRYCAGES = 9
FONT_SIZE = 15

FONT_COLOR = "#ffffff"

SPEEDO_FONT_SIZE = 120

GREEN = "#42f47a"
RED = "#FF0000"




## serial imports
import sys
import glob


#threading imports
from threading import Thread


num_row = 4
num_col = 4
screen_width = 1024
screen_height = 800

ELEMENT_SIZE = screen_height/num_col
ELEMENT_WIDTH = screen_width/num_row


#[x,y ,row span, column span,description, height, width]
pouch_pannel_configuration = [0 for i in range(NO_OF_TELEMETRYCAGES)]
pouch_pannel_configuration[0] = [0,0,2,1,"left", ELEMENT_SIZE*2, ELEMENT_WIDTH] #left signal light
pouch_pannel_configuration[1] = [1,0,2,1,"speed", ELEMENT_SIZE*2, ELEMENT_WIDTH] #speed 
pouch_pannel_configuration[2] = [2,0,2,1,"right", ELEMENT_SIZE*2, ELEMENT_WIDTH] #right signal light
pouch_pannel_configuration[3] = [1,2,1,1,"brake", ELEMENT_SIZE, ELEMENT_WIDTH] #brake
pouch_pannel_configuration[4] = [1,3,1,1,"headlights", ELEMENT_SIZE, ELEMENT_WIDTH] #headlights 
pouch_pannel_configuration[5] = [3,0,1,1, "current",ELEMENT_SIZE, ELEMENT_WIDTH] #current
pouch_pannel_configuration[6] = [3,1,1,1, "voltage",ELEMENT_SIZE, ELEMENT_WIDTH] #voltage
pouch_pannel_configuration[7] = [3,2,1,1, "cycletime",ELEMENT_SIZE, ELEMENT_WIDTH] #cycle time
pouch_pannel_configuration[8] = [3,3,1,1, "totaltime",ELEMENT_SIZE, ELEMENT_WIDTH] #total time



class DriveUserInterface(tk.Frame):
    def __init__(self, master = None):
        self.master = master
        pad = 3
        self._geom = '200x200+0+0'
        master.geometry("{0}x{1}+0+0".format(master.winfo_screenwidth()-pad, master.winfo_screenheight()-pad))
        master.bind('<Escape>', self.toggle_geom)
        
        tk.Frame.__init__(self, master, bg =DEFAULT_LIGHT_COLOR)
        screen_width = master.winfo_screenwidth()-pad
        screen_height = master.winfo_screenheight()-pad
        self.scrollbar = None
        self.grid()
        self.telemetrycages = []
        self.createCells(NO_OF_TELEMETRYCAGES)
        
    
    def toggle_geom(self, event):
        geom= self.master.winfo_geometry()
        print(geom, self._geom)
        self.master.geometry(self._geom)
        self._geom = geom
        
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
        ##pass
        self.UISelector("speed").setText(round(dataSpeed.speedKmh))
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
        self.text_id = self.create_text((ELEMENT_WIDTH/2,ELEMENT_SIZE/2), text=self.description, font=("Courier", FONT_SIZE), fill = FONT_COLOR, anchor=tk.CENTER)
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
            self.setColor(RED)
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
    elif description == "left" or description == "right":
        return Indicator(parent, height, width, description)
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
        self.text_id = self.create_text((ELEMENT_WIDTH/2,ELEMENT_SIZE), text=self.description, font=("Courier", SPEEDO_FONT_SIZE), anchor=tk.CENTER)
        xOffset = self.findXCenter(self, self.text_id)
##        self.move(self.text_id, xOffset, 0)
    def indicate(self):
        rgb = hsv_hex_conversion(float(self.text),1,1)
        self.setColor(rgb)



class Indicator(TelemetryElement):
    def initText(self): #override
        self.text_id = self.create_text((ELEMENT_SIZE//2,ELEMENT_SIZE), text=self.description, font=("Courier", FONT_SIZE), anchor=tk.S)
##        self.move(self.text_id, 0, -10)
        xOffset = self.findXCenter(self, self.text_id)

    def indicate(self):
        padding = 10
        center_y = self.height / 2
        center_x = self.width / 2
        if self.description == "left":
            triangle_top = [self.width - padding, 0 + padding]
            triangle_mid = [0 + padding, center_y]
            triangle_btm = [self.width - padding, self.height - padding]
        else:
            triangle_top = [0 + padding, 0 + padding]
            triangle_mid = [self.width - padding, center_y]
            triangle_btm = [0 + padding, self.height - padding]
        points = triangle_top + triangle_mid + triangle_btm
        if self.text == "1":
            self.create_polygon(points, outline='#f11', 
            fill='#1f1', width=2)
        else:
            self.create_polygon(points, outline='#f11', 
            fill=DEFAULT_LIGHT_COLOR, width=2)
            





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
        



mode = "run"


if mode != "dev":
    import serial
    from DataPoint import NV11DataAccessories, NV11DataSpeedo



def funct2():
    print("Display")
    while True:
        try:
            root = tk.Tk()
            root.wm_attributes('-type', 'splash')
            app = DriveUserInterface(root)
            app.background = DEFAULT_LIGHT_COLOR
            ##app.master.title('DRIVE GUI')
            break
        except tk.TclError:
            time.sleep(0.1)
            print("TRYING!!!")
    ##########
    if mode != "dev":
        s = serial.Serial("/dev/serial0", baudrate=9600, timeout=1)
        dataSpeed = NV11DataSpeedo(0x0A)
        dataAcc = NV11DataAccessories(0x10)
    while True:
        if mode != "dev":
            line = s.readline().decode()
            if line is None:
                continue
            if dataSpeed.checkMatchString(line):
                dataSpeed.unpackString(line)
                
            elif dataAcc.checkMatchString(line):
                dataAcc.unpackString(line)
            
            for i in range(NO_OF_TELEMETRYCAGES):
                app.updateDriveUI(dataAcc, dataSpeed)
        else:
            app.updateDriveUI(0, 0)
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




