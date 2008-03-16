###########################################################
#
# Implements the TCP server interface to administrative
# or monitoring apps
#
###########################################################

from TCPServerBase import TCPServerBase

from threading import Thread
from threading import Lock
import logging as log
import time
from TCPServer import TCPServer

class TCPServer1Thread(Thread):
    ownerNotifier = None
    terminated = False
    serverListener = None
    tcpServer = None
    port = None

    def __init__ (self, ownerNotifier, serverListener, port):
        Thread.__init__(self)
        self.ownerNotifier = ownerNotifier
        self.serverListener = serverListener
        self.port = port
        
    def cleanup(self):
        if self.tcpServer.running():
            self.tcpServer.stop()
        self.ownerNotifier.notify(self.ownerNotifier.TERMINATE)

    def run(self):
        try:
            log.debug('TCPServer1Thread.run')
            self.tcpServer = TCPServer(self.port, self.ownerNotifier)
            self.tcpServer.start()

            while not self.terminated:
                while not self.serverListener.empty():
                    data = self.serverListener.read() + '\n'
                    self.tcpServer.writeAll(data)
                time.sleep(0.01)
            self.cleanup()
        except: # sys.excepthook does not work in threads
            import traceback
            traceback.print_exc()
            self.terminated = True
            self.cleanup()
    
    def terminate(self):
        log.debug('TCPServer1Thread.terminate')
        self.terminated = True

class TCPServer1(TCPServerBase):
    
    DEFAULT_CONFIG = {'ethif':'any',
                      'port':1200,
                      'compat_1': True,
                      'canif': None,
                      }
    
    pktHandler = None
    serverThread = None
    ownerNotifier = None
    
    pktBuffer = []
    pktBufferLock = None
    
    class ServerListener():
        
        parent = None
        
        def __init__(self, parent):
            self.parent = parent
        
        def empty(self):
            if len(self.parent.pktBuffer) > 0:
                return False
            return True
        
        def full(self):
            return False
        
        def lock(self):
            self.parent.pktBufferLock.acquire()
            
        def unlock(self):
            self.parent.pktBufferLock.release()
        
        def write(self, data):
            self.lock()
            self.parent.pktBuffer.append(data)
            self.unlock()
            
        def read(self):
            self.lock()
            data = self.parent.pktBuffer.pop(0)
            self.unlock()
            return data
            
    
    def __init__(self, pktHandler, cfg = None):
        if cfg is None:
            self.config = self.DEFAULT_CONFIG
        else:
            self.config = cfg
        self.pktBufferLock = Lock()
        self.pktHandler = pktHandler
        log.debug('Creating tcpd server, config: ' + str(self.config))

    def setPktHandler(self, pktHandler):
        self.pktHandler = pktHandler
        
    def setIfNotifier(self, ifNotifier):
        self.ownerNotifier = ifNotifier
    
    def start(self):
        ethif = self.config['ethif']
        port = self.config['port']
        compat_1 = self.config['compat_1']
        canif = self.config['canif']
        
        serverListener = self.ServerListener(self)
        self.pktHandler.addServerListener(serverListener)
        
        self.serverThread = TCPServer1Thread(self.ownerNotifier, serverListener, port)
#        self.serverThread = TCPServer(port, self.ownerNotifier)
        self.serverThread.start()
        return True

    def stop(self):
        if self.running():
            self.serverThread.terminate()
        else:
            log.debug('TCP interface not started, or already stopped')
    
    def running(self):
        if self.serverThread is not None:
            return not self.serverThread.terminated
        else:
            return False
