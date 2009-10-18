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

class SerialThread(Thread):
    
    terminated = False
    port = None
    pktHandler = None
    
    def __init__ (self, pktHandler, port):
        Thread.__init__(self)
        self.pktHandler = pktHandler
        self.port = port

    def run(self):
        try:
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
    
                time.sleep(0.001)
                
            self.port.close()
            self.ownerNotifier.notify(self.ownerNotifier.TERMINATE)
            
        except: # sys.excepthook does not work in threads
            import traceback
            traceback.print_exc()
            self.terminate()
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
    ownerNotifier = None
    config = {}
    
    def __init__ (self, pktHandler, cfg = None):
        if cfg is None:
            self.config = self.DEFAULT_CONFIG
        else:
            self.confg = cfg
        
        self.pktHandler = pktHandler
        NodeIfBase.__init__(self, cfg, pktHandler)
        
    def setPktHandler(self, pktHandler):
        self.pktHandler = pktHandler
        
    def setIfNotifier(self, ifNotifier):
        self.ownerNotifier = ifNotifier
    
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
        
        self.serialThread = SerialThread(self.pktHandler, port)
        self.serialThread.start()
        return True

    def stop(self):
        if self.serialThread is not None and not self.stimThread.terminated:
            self.serialThread.terminate()
        else:
            log.debug('Serial interface not started, or already stopped')
            
    def running(self):
        if self.serialThread is not None:
            return not self.serialThread.terminated
        else:
            return False
    