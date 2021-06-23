
# coding: utf-8

# In[ ]:

import serial 
from tkinter import *
import tkinter as tk
from tkinter.simpledialog import askfloat
from tkinter.simpledialog import askinteger

#specify which port is being used for serial comm
commPort = '/dev/cu.usbmodem1421'

#initialize the serial port with same baudrate
ser = serial.Serial(commPort, baudrate = 9600, timeout = 0.5)

#fucntions for what the buttons will command 
def DISABLE(): 
    dis = 'D\n'
    ser.write(dis.encode('ascii'))
    

def STEP_CCW():
    
    aski = askinteger('Total Steps', 'How many steps?')
    askf1 = askfloat('Initial Flick Speed', 'How much time between flicks?')
    askf2 = askfloat('Acceleration', 'What Acceleration?')
    aski = str(aski)
    askf1 = str(askf1)
    askf2 = str(askf2)
    askf1 = askf1 + ',' + askf2 + ',' + aski +'\n'
    l = 'L'
    l += askf1
  
    ser.write(l.encode('ascii'))

    

def STEP_CW():
    
    s = askinteger('Total Steps', 'How many steps?')
    e = askfloat('Initial Flick Speed', 'How much time between flicks?')
    i = askfloat('Acceleration', 'What Acceleration?')
    i = str(i)
    s = str(s)
    e = str(e)
    s = s + ',' + e + ',' + i +'\n'
    h = 'H'
    h += s
  
    ser.write(h.encode('ascii'))

    
#create the window
root = Tk()

#title the window
root.title('Control STEPPER')

#creating each button, naming them, placing them and binding them to the correct function
btn_Dis = tk.Button(root, text = 'DISABLE', command = DISABLE)
btn_Dis.grid(row = 0, column = 0)

btn_CW = tk.Button(root, text = 'STEP CW', command = STEP_CW)
btn_CW.grid(row = 0, column = 1)

btn_CCW = tk.Button(root, text = 'STEP CCW', command = STEP_CCW)
btn_CCW.grid(row = 0, column = 2)

#size of the window
root.geometry('200x200')
#makes sure it runs forever
root.mainloop()


# In[ ]:



