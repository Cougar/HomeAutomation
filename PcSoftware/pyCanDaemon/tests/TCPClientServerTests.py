import logging as log
log.basicConfig(level=log.DEBUG,
                format='%(asctime)s %(levelname)s %(message)s',
                filename=__name__ + '.log',
                filemode='w')
import unittest

from TCPServer import TCPServer
from TCPClient import TCPClient
import time

class CanPktTests(unittest.TestCase):
    
    client = None
    
    server = None
    serverNotifier = None
    serverRunning = False

    def setUp(self):
        """Call before every test case."""
        pass
        
    def tearDown(self):
        """Call after every test case."""
        if self.serverRunning:
            self.stopServer()
    
    class ServerNotifier():
        
        parent = None
        UNDEFINED = 0
        TERMINATE = 1
        
        def __init__(self, parent):
            self.parent = parent
    
        def notify(self, msg):
            if msg == self.TERMINATE:
                log.debug('Server terminate notify')
                self.parent.serverRunning = False
    
    def startServer(self):
        self.serverNotifier = self.ServerNotifier(self)
        self.server = TCPServer(27000, self.serverNotifier)
        assert self.server.start() == True
        self.serverRunning = True
        
    def stopServer(self):
        assert self.serverRunning == True
        self.server.stop()
        waitTime = 0
        while self.serverRunning:
            log.debug('Waiting 0.2s for server shutdown..')
            time.sleep(0.2)
            waitTime += 0.2
            if waitTime > 4:
                log.debug('Giving up!')
                assert False
                break
        
    def startClient(self):
        self.client = TCPClient('localhost', 27000)
        return self.client.connect(1)

    def stopClient(self):
        self.client.disconnect()
        time.sleep(0.1)
    
    def testATcpServer(self):
        self.startServer()
        self.stopServer()

    def testBTcpClientNegative(self):
        assert self.startClient() == False
        self.stopClient()

    def testCTcpServerWithSingleClient(self):
        self.startServer()
        self.startClient()
        self.stopClient()
        self.stopServer()

    def testDTcpConnection(self):
        log.debug('testTcpConnection')
        self.startServer()
        assert self.startClient() == True
        log.debug('Server sends hej')
        self.server.writeAll('hej')
        time.sleep(0.5)
        resp = str(self.client.read())
        log.debug('Client gets:' + resp)
        assert (resp == '\'hej\'')
        self.stopClient()
        self.stopServer()
