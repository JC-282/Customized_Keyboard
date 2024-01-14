import spidev
import time 

SPIDELAY = 0.1
spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 0
spi.max_speed_hz = 125000

class InputSc:
    def __init__(self):
        self.inp = ""
        self.cursor = 0
        self.cursorp = 0
        InputSc.spi_init()



    def show_screen(self):
        ind = InputSc.spi_string(self.inp, self.cursor)
        InputSc.spi_cursor(ind)
        return

    def input(self, char):
        self.inp = self.inp[0:self.cursor] + char + self.inp[self.cursor:]
        self.cursor += 1
        ind = InputSc.spi_string(self.inp, self.cursor)
        InputSc.spi_cursor(ind)
        return

    def bksp(self): 
        if (self.cursor != 0):
            self.cursor -= 1
        else:
            self.cursor = 0
        self.inp = self.inp[0:self.cursor] + self.inp[self.cursor+1:]
        ind = InputSc.spi_string(self.inp, self.cursor)
        InputSc.spi_cursor(ind)
        return
    
    def left_cursor(self):
        if (self.cursor != 0):
            self.cursor -= 1
        else:
            self.cursor = len(self.inp)
        ind = InputSc.spi_string(self.inp, self.cursor)
        InputSc.spi_cursor(ind)
        return


    def right_cursor(self):
        if (self.cursor != len(self.inp)):
            self.cursor += 1
        else:
            self.cursor = 0
        ind = InputSc.spi_string(self.inp, self.cursor)
        InputSc.spi_cursor(ind)
        return

    def isEmpty(self):
        if (self.inp == ""):
            return True
        else:
            return False

    def getInput(self):
        return self.inp

    def clear(self):
        self.inp = ""
        self.cursor = 0
        self.line = 0
        InputSc.spi_clear()
        return


    def spi_cmd(num):
        num = num << 6
        a = num >> 8
        b = num & 0xFF
        spi.xfer3([a,b], 2)
        return


    def spi_init():
        # spi_cmd(0x8f)   # move to the lest pos on the first row
        InputSc.spi_cmd(0x38)    # 0x38  8bit english
        InputSc.spi_cmd(0x08)    # 0x08  off 
        InputSc.spi_clear()
        InputSc.spi_cmd(0x0c)    # 0x0c  turn display on 
        InputSc.spi_cmd(0x0f)    # 0x0c  on (cursor on & blink)
        # spi_cmd(0x0e)   # 0x0e  on (cursor on & no blink)
        InputSc.spi_cmd(0x02)    # 0x02  home position
        InputSc.spi_cmd(0x06)    # 0x07  incr & shift 
        return

    def spi_clear(): 
        InputSc.spi_cmd(0x01)    # 0x01  clear
        InputSc.spi_cmd(0x02)    # 0x02  home position
        return

    # change to move the cursor from beginning
    def spi_cursor(pos):
        
        InputSc.spi_cmd(0x80 | pos)

        return

    # change to display string can shift
    def spi_string(string, cursor):

        InputSc.spi_clear()
        cursorcount = cursor // 16
        cursorleft = cursor % 16
        count = len(string)
        line = count // 16
        left = count % 16
        if (cursorcount == 0):
            ind = cursorleft
            if (line > 1):
                show = string[0:32]
            else:
                show = string[0:]
        else:
            ind = 0x40 | cursorleft
            if (cursorleft + (len(string) - cursor) >= 16):
                show = string[(cursorcount-1) * 16 : (cursorcount + 1) * 16]
            else:
                show = string[(cursorcount-1) * 16 : ]

        for pos in range(len(show)):
            num = 0x200 | ord(show[pos])
            num = num << 6
            a = num >> 8
            b = num & 0xFF
            spi.xfer3([a,b], 2)

            if(pos == 15): 
                InputSc.spi_cmd(0xc0)

        return ind


