###########################################################
#
# Node interface through raw TCP socket (telnet)
#
###########################################################
import logging as log
from NodeIfBase import NodeIfBase
from threading import Thread
from TCPClient import TCPClient
import time

class TCPThread(Thread):
    ownerNotifier = None
    terminated = False
    tcpClient = None
    pktHandler = None
    host = None
    port = None

    def __init__ (self, pktHandler, ownerNotifier, host, port):
        Thread.__init__(self)
        self.pktHandler = pktHandler
        self.ownerNotifier = ownerNotifier
        self.host = host
        self.port = port
        
    def cleanup(self):
        if self.tcpClient is not None:
            self.tcpClient.disconnect()
        self.ownerNotifier.notify(self.ownerNotifier.TERMINATE)

    def run(self):
        try:
            log.debug('TCPThread.run')
            
            self.tcpClient = TCPClient(self.host, self.port)
            if not self.tcpClient.connect(10):
                print 'Connection to host failed'
                self.terminated = True

            while not self.terminated: 
                data = self.tcpClient.read()
                if data is not None:
                    data = data[1:-3] # remove leading ' and trailing \n'
                    self.pktHandler.input(data)
                    
                time.sleep(0.001)
            self.cleanup()
        except: # sys.excepthook does not work in threads
            import traceback
            traceback.print_exc()
            self.terminated = True
            self.cleanup()
    
    def terminate(self):
        log.debug('TCPThread.terminate')
        self.terminated = True

class NodeIfTCP:
    
    DEFAULT_CONFIG = {'host':'sra.eta.chalmers.se',
                      'port':8002,
                      }
    
    config = {}
    pktHandler = None
    tcpThread = None
    ownerNotifier = None
    
    def __init__(self, pktHandler, ownerNotifier, cfg = None):
        if cfg is None:
            self.config = self.DEFAULT_CONFIG
        else:
            self.confg = cfg
        self.pktHandler = pktHandler
        self.ownerNotifier = ownerNotifier
        
    def setPktHandler(self, pktHandler):
        self.pktHandler = pktHandler
    
    def start(self):
        host = self.config['host']
        port = self.config['port']
        self.tcpThread = TCPThread(self.pktHandler, self.ownerNotifier, host, port)
        self.tcpThread.start()
        return True

    def stop(self):
        if self.running():
            self.tcpThread.terminate()
        else:
            log.debug('TCP interface not started, or already stopped')
    
    def running(self):
        if self.tcpThread is not None:
            return not self.tcpThread.terminated
        else:
            return False
