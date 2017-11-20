#!/usr/bin/env python
'''
Created on November 20, 2010

@author: Dr. Rainer Hessmer
'''
import threading
import serial
import time
try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO


class SerialServerError(Exception):
    pass

class SerialServer():
    '''
    Helper class for receiving lines from a serial port
    '''

    def __init__(self, port="COM4", baudrate=115200, callback=None):
        '''
        Initializes the receiver class.
        port: The serial port to listen to.
        receivedLineHandler: The function to call when a line was received.
        '''
        self._port = port
        self._baudrate = baudrate
        self._received_line_handler = self._line_handler
        self._keep_running = False
        self._string_io = None
        self._receiver_thread = None
        self._serial = None

    def _line_handler(self, line):
        print("LH: ", line)
        
    def start(self):
        print("Starting serial server")
        try:
            self._serial = serial.Serial(port=self._port, baudrate=self._baudrate, timeout=1)
        except:
            raise SerialServerError("SERIAL PORT Start Error")

        self._keep_running = True
        self._receiver_thread = threading.Thread(target=self._listen)
        self._receiver_thread.setDaemon(True)
        self._receiver_thread.start()

    def stop(self):
        print("Stopping serial server")
        self._KeepRunning = False
        time.sleep(.1)
        try:
            self._serial.close()
        except:
            raise SerialServerError("SERIAL PORT Stop Error")

    def _listen(self):
        self._string_io = StringIO()
        while self._keep_running:
            try:
                data = self._serial.read().decode('utf8')
                print("received data from serial port")
            except:
                raise SerialServerError("SERIAL PORT Listen Error")
            if data == '\r':
                print(r"receved \r")
                pass
            if data == '\n':
                print(r"received \n")
                self._received_line_handler(self._string_io.getvalue())
                self._string_io.close()
                self._string_io = StringIO()
            else:
                print("writing to string io")
                self._string_io.write(data)

    def write(self, data):
        try:
            self._serial.write(data)
        except AttributeError:
            raise SerialServerError("SERIAL PORT Write Error")
            
    def read(self):
        # Read all and restart string_io
        results = self._string_io.getvalue()
        self._string_io.close()
        self._string_io = StringIO()

        return results


if __name__ == "__main__":
    prompt_received = False
    def callback(line):
        print("CB: ", line)
        if "Enter x/X to exit validation" in line:
            prompt_received = True

    ss = SerialServer()

    ss.start()
    #entry = input("Type 'x': ")
    #ss.write(bytes('x\n'.encode('utf8')))
    #time.sleep(0.1)
    entry = input("Type 'v': ")
    ss.write(bytes('v\n'.encode('utf8')))
    print("Waiting for lines ... ")
    #while not prompt_received:
    #    time.sleep(0.01)
    #prompt_received = False
    time.sleep(20)
    print("Read in the prompt ...")
    prompt = ss.read()
    print(prompt)
    entry = input("Type 'x': ")
    ss.write(bytes('x\n'.encode('utf8')))
    ss.stop()

# --- EOF ---
