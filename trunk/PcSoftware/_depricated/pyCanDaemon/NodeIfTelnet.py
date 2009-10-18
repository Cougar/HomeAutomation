###########################################################
#
# Node interface through raw TCP socket (telnet)
#
###########################################################
import logging as log
from NodeIfBase import NodeIfBase
from threading import Thread

class TelnetThread(Thread):
    ownerNotifier = None
    terminated = False
    socket = None
    pktHandler = None

    def __init__ (self, pktHandler, ownerNotifier, socket):
        Thread.__init__(self)
        self.pktHandler = pktHandler
        self.ownerNotifier = ownerNotifier
        
    def cleanup(self):
 #       self.port.close()
        self.ownerNotifier.notify(self.ownerNotifier.TERMINATE)

    def run(self):
        try:
            log.debug('TelnetThread.run')
            readBuffer = []
                    
            while not self.terminated: 
  #              inByte = self.port.read()
  #              blocking read call??
                if inByte == '\n':
                    msg = ''.join(readBuffer)
                    readBuffer = []
                    #print 'Incoming:', msg
                    self.pktHandler.input(msg)
                else:
                    readBuffer.append(inByte)
    
                time.sleep(0.001)
            self.cleanup()
        except: # sys.excepthook does not work in threads
            import traceback
            traceback.print_exc()
            self.terminated = True
            self.cleanup()
    
    def terminate(self):
        log.debug('TelnetThread.terminate')
        self.terminated = True

class NodeIfTelnet:
    config = []
    pktHandler = None
    telThread = None
    
    def __init__(self, cfg, pktHandler):
        self.config = cfg
        self.pktHandler = pktHandler
        
    def setPktHandler(self, pktHandler):
        self.pktHandler = pktHandler
    
    def start(self):
        self.telThread = TelnetThread(self.pktHandler, self.ifNotifier, socket)
        self.telThread.start()
        return True

    def stop(self):
        if self.running():
            self.telThread.terminate()
        else:
            log.debug('Telnet interface not started, or already stopped')
    
    def running(self):
        if self.telThread is not None:
            return not self.telThread.terminated
        else:
            return False
