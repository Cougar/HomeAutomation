###########################################################
#
# Serial interface, connects with can bus over a serial
# line
#
###########################################################
import logging as log
from NodeIfBase import NodeIfBase
from threading import Thread
import serial
import sys
import time

UART_START_BYTE = 253
UART_END_BYTE = 250
PACKET_LENGTH = 17
readBufferSize = 8192
UART_CONN_BYTE = 252

class SerialThread(Thread):
    
    terminated = False
    port = None
    pktHandler = None
    
    def __init__ (self, pktHandler, port, exceptHook):
        Thread.__init__(self)
        self.pktHandler = pktHandler
        self.port = port
        sys.excepthook = exceptHook

    def run(self):
        log.debug('SerialThread.run')
        readBuffer = []
        while not self.terminated:
            
            inByte = self.port.read()
            if inByte == '\n':
                msg = ''.join(readBuffer)
                readBuffer = []
                #print 'Incoming:', msg
                self.pktHandler.input(msg)
            else:
                readBuffer.append(inByte)

            time.sleep(0.1)
        self.port.close()
    
    def terminate(self):
        log.debug('SerialThread.terminate')
        self.terminated = True
        

class NodeIfSerial(NodeIfBase):
    
    DEFAULT_CONFIG = {'serialPort':1, #com1
                      'baudRate':9600,
                      'byteSize':8,
                      'parity': 'N',
                      'stopBits':1,
                      'timeOut': 0, # must be 0 (non-blocking mode)
                      'softFlowCtl': 0,
                      'hardFlowCtl': 0
                      }
    
    serialThread = None
    pktHandler = None
    exceptHook = None
    
    def __init__ (self, pktHandler, exceptHook, cfg = None):
        if cfg is None:
            self.config = self.DEFAULT_CONFIG
        
        self.pktHandler = pktHandler
        self.exceptHook = exceptHook
        NodeIfBase.__init__(self, cfg, pktHandler)
    
    def start(self):
        
        try:
            port = serial.Serial(self.config['serialPort'],
                                 self.config['baudRate'],
                                 self.config['byteSize'],
                                 self.config['parity'],
                                 self.config['stopBits'],
                                 self.config['timeOut'],
                                 self.config['softFlowCtl'],
                                 self.config['hardFlowCtl'])
        except (serial.SerialException), Error:
            print Error
            return False
        
        self.serialThread = SerialThread(self.pktHandler, port, self.exceptHook)
        self.serialThread.start()
        return True

    def stop(self):
        if self.serialThread is not None:
            self.serialThread.terminate()
        else:
            log.debug('Serial interface not started, or already stopped')
