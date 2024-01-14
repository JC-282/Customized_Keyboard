import RPi.GPIO as GPIO
from time import sleep

initpin = [13, 4, 5, 6,27, 23, 24, 25, 26, 0, 1, 2, 3]
dataPin = [23, 24, 25, 26, 0, 1, 2, 3]
DELAY = 0.001


class OutputSc:
    def __init__(self):
        self.count = 0
        self.tempcount = 0
        self.currentcursor = 0
        self.result = []
        self.lineout = 0
        OutputSc.initscreen_GPIO()
        OutputSc.initscreen()
    
        return
    def show_screen(self, out):
        self.clear_screen()
        self.result = out.copy()

        self.print_screen()

        return
    def print_screen(self):
        for i in self.result:
            for j in i:
                if (self.tempcount < self.currentcursor):
                    self.tempcount += 1
                else:
                    OutputSc.display(ord(j),self.lineout)
                    self.count += 1
                    if (self.count % 80 == 0):
                        self.lineout ^= 1
                    if (self.count == 160):
                        return

            if (self.tempcount < self.currentcursor):
                self.tempcount += 1
            else:
                OutputSc.display(ord(' '), self.lineout)
                self.count += 1
                if (self.count % 80 == 0):
                    self.lineout ^= 1
                if (self.count == 160):
                    return

        return
    def move_screen_up(self):
        OutputSc.clear_display()
        self.count = 0
        self.tempcount = 0
        self.lineout = 0
        self.currentcursor -= 40
        if (self.currentcursor < 0):
            self.currentcursor = 0
        self.print_screen()

        return

    def move_screen_down(self):
        OutputSc.clear_display()
        self.count = 0
        self.tempcount = 0
        self.lineout = 0
        self.currentcursor += 40
        self.print_screen()
        return

    def clear_screen(self):
        OutputSc.clear_display()
        self.count = 0
        self.currentcursor = 0
        self.lineout = 0
        self.tempcount = 0
        self.result = []

        return

    def initscreen_GPIO():
        GPIO.setwarnings(False)
        GPIO.setmode(GPIO.BCM)
        for i in initpin:
            GPIO.setup(i, GPIO.OUT)

        # enable E pin and initialize the output
        GPIO.output(6, GPIO.LOW)

        # enable E pin and initialize the output
        GPIO.output(27, GPIO.LOW)

        # setup PSB pin, setting to 8 bit parallel
        GPIO.output(13, GPIO.HIGH)

        return
    def initscreen():
        OutputSc.send_cmd(0x3c,0)  # set 8 bit instruction
        OutputSc.send_cmd(0x08,0)  # turn the display off
        OutputSc.send_cmd(0x06,0)  # auto increment the cursor
        OutputSc.send_cmd(0x1c,0)
        OutputSc.send_cmd(0x1,0)   # clear the display
        OutputSc.send_cmd(0xc,0)   # turn the display on

        OutputSc.send_cmd(0x3c,1)  # set 8 bit instruction
        OutputSc.send_cmd(0x08,1)  # turn the display off
        OutputSc.send_cmd(0x06,1)  # auto increment the cursor
        OutputSc.send_cmd(0x1c,1)
        OutputSc.send_cmd(0x1,1)   # clear the display
        OutputSc.send_cmd(0xc,1)   # turn the display on

        return
    def send_cmd(cmd,a):
        GPIO.output(4, GPIO.LOW)
        GPIO.output(5, GPIO.LOW)
        for i in range(len(dataPin)):
            if (cmd >> i & 1) == 0:
                GPIO.output(dataPin[i], GPIO.LOW)
            else:
                GPIO.output(dataPin[i], GPIO.HIGH)

        OutputSc.trigger(a)
        return
    def return_cur():
        OutputSc.send_cmd(0x02,0)
        OutputSc.send_cmd(0x02,1)

        return
    def clear_display():
        OutputSc.send_cmd(0x01,0)
        OutputSc.send_cmd(0x01,1)
        OutputSc.return_cur()


        return
    def display(out,a):
        GPIO.output(4, GPIO.HIGH)
        GPIO.output(5, GPIO.LOW)
        for i in range(len(dataPin)):
            if (out >> i & 1) == 0:
                GPIO.output(dataPin[i], GPIO.LOW)
            else:
                GPIO.output(dataPin[i], GPIO.HIGH)

        OutputSc.trigger(a)

        return
    def trigger(a):
        if (a == 0):
            GPIO.output(6, GPIO.HIGH)
            sleep(DELAY)
            GPIO.output(6, GPIO.LOW)
            sleep(DELAY)
        else:
            GPIO.output(27, GPIO.HIGH)
            sleep(DELAY)
            GPIO.output(27, GPIO.LOW)
            sleep(DELAY)

        return
