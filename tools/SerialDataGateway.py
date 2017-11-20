#!/usr/bin/env python
'''
Created on November 20, 2010

@author: Dr. Rainer Hessmer
'''
import threading
import serial
try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO
import time

class SerialDataGateway(object):
    '''
    Helper class for receiving lines from a serial port
    '''

    def __init__(self, port="COM4", baudrate=115200):
        '''
        Initializes the receiver class.
        port: The serial port to listen to.
        receivedLineHandler: The function to call when a line was received.
        '''
        self._Port = port
        self._Baudrate = baudrate
        self.ReceivedLineHandler = self.lineHandler
        self._KeepRunning = False
        self.stringIO = None
        self.filter = self.defaultFilter

    def defaultFilter(self, line):
        pass
        
    def lineHandler(self, line):
        if self.filter is not None:
            self.filter(line)
        #print("LH: ", line)
        
    def SetFilter(self, filter):
        self.filter = filter;
    
    def ClearFilter(self):
        self.filter = None
        self.filter_str = ''
        
    def Start(self):
        try:
            self._Serial = serial.Serial(port=self._Port, baudrate=self._Baudrate, timeout=1)
        except:
            print("SERIAL PORT Start Error")
            raise
        self._KeepRunning = True
        self._ReceiverThread = threading.Thread(target=self._Listen)
        self._ReceiverThread.setDaemon(True)
        self._ReceiverThread.start()

    def Stop(self):
        print("Stopping serial gateway")
        self._KeepRunning = False
        time.sleep(.1)
        try:
            self._Serial.close()
        except:
            print("SERIAL PORT Stop Error")
            raise

    def _Listen(self):
        self.stringIO = StringIO()
        while self._KeepRunning:
            try:
                data = self._Serial.read().decode('utf8')
            except:
                print("SERIAL PORT Listen Error")
                raise
            if data == '\r':
                pass
            if data == '\n':
                self.ReceivedLineHandler(self.stringIO.getvalue())
                self.stringIO.close()
                self.stringIO = StringIO()
            else:
                self.stringIO.write(data)

    def Write(self, data):
        #AttributeError: 'SerialDataGateway' object has no attribute '_Serial'
        try:
            self._Serial.write(data)
        except AttributeError:
            print("SERIAL PORT Write Error")
            raise
            
    def Read(self):
        results = self.stringIO.getvalue()
        self.stringIO.close()
        self.stringIO = StringIO()
        return results

def enter_calibration():
    dataReceiver.Write(b'c')
    time.sleep(0.5)
    print(dataReceiver.Read())
    
def str_to_bytes(s):
    return [bytes(i.encode('utf8')) for i in list(s)]
    
def write_echo_char(s):
    b = str_to_bytes(s)
    for ch in b:
        dataReceiver.Write(ch)
        dataReceiver.Read()
    
def parse_and_plot_pid_data(data):
    target = []
    response = []
    
    for d in data:
        tr = d.split(' ')[2:4]
        target.append(tr[0])
        response.append(tr[1])
    
    import matplotlib.pyplot as plt

    plt.plot(target, linewidth=2.0)
    plt.plot(response, linewidth=2.0)
    plt.show()
    
    
def create_xy(l):
    cps_list = []
    pwm_list = []
    for t in l:
        cps, pwm = t.split(':')
        cps_list.append(cps)
        pwm_list.append(pwm)
    
    return pwm_list, cps_list

def parse_and_plot_motor_data(data):
    after = '\n'.join(data)

    lb = after[after.find('Left-Backward'):after.find('Left-Forward')].rstrip().split('\n')[1]
    lf = after[after.find('Left-Forward'):after.find('Right-Backward')].rstrip().split('\n')[1]
    rb = after[after.find('Right-Backward'):after.find('Right-Forward')].rstrip().split('\n')[1]
    rf = after[after.find('Right-Forward'):].rstrip().split('\n')[1]

    lb_tuples = lb.split(' ')
    lf_tuples = lf.split(' ')
    rb_tuples = rb.split(' ')
    rf_tuples = rf.split(' ')

    lb_x, lb_y = create_xy(lb_tuples)
    lf_x, lf_y = create_xy(lf_tuples)

    left_x = lb_x + lf_x
    left_y = lb_y + lf_y

    rb_x, rb_y = create_xy(rb_tuples)
    rf_x, rf_y = create_xy(rf_tuples)

    right_x = rb_x + rf_x
    right_y = rb_y + rf_y
    
    import matplotlib.pyplot as plt

    plt.plot(left_x, left_y, linewidth=2.0)
    plt.plot(right_x, right_y, linewidth=2.0)
    plt.show()

def motor_calibrate(data):
    def filter(line):
        if 'Printing motor calibration results' in line:
            filter.capture_data = True
        if filter.capture_data:
            data.append(line)
    filter.capture_data = False
    dataReceiver.SetFilter(filter)
    dataReceiver.Write(b'a')
    time.sleep(180)
    filter.capture_data = False

def pid_calibrate(which_pid, data):
    def filter(line):
        if which_pid_filter in line:
            data.append(line)
            
    if which_pid == 'left':
        command = b'b'
        which_pid_filter = 'left pid: '
    elif which_pid == 'right':
        command = b'c'
        which_pid_filter = 'right pid: '
        
    dataReceiver.SetFilter(filter)
    dataReceiver.Write(command)
    time.sleep(0.25)
    value = input("Enter P-gain: ")
    write_echo_char(value[:max(len(value), 5)])
    
    value = input("Enter I-gain: ")
    write_echo_char(value[:max(len(value), 5)])
    
    value = input("Enter D-gain: ")
    write_echo_char(value[:max(len(value), 5)])
    
    # Wait for calibration to complete
    time.sleep(3.25)
    
    dataReceiver.ClearFilter()

def exit_calibration():
    dataReceiver.Write(b'x')
    time.sleep(0.1)
    #print(dataReceiver.Read())

import sys

if __name__ == '__main__':
    dataReceiver = SerialDataGateway("COM4", 115200)
    dataReceiver.Start()
    '''
    data = []
    
    command = sys.argv[1]
    print(command)
    
    if command == 'motor':
        enter_calibration()
        motor_calibrate(data)
        exit_calibration()
        parse_and_plot_motor_data(data)
    
    elif command == 'leftpid':
        enter_calibration()
        pid_calibrate('left', data)
        exit_calibration()
        parse_and_plot_pid_data(data)
    
    elif command == 'rightpid':
        enter_calibration()
        pid_calibrate('right', data)
        exit_calibration()
        parse_and_plot_pid_data(data)
    ''' 
    dataReceiver.Stop()
