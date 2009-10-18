import logging as log
log.basicConfig(level=log.DEBUG,
                format='%(asctime)s %(levelname)s %(message)s',
                filename=__name__ + '.log',
                filemode='w')
import unittest

from TCPServer import TCPServer
from TCPClient import TCPClient
import time

SERVER_BASE_PORT = 27000
NUMBER_OF_SERVERS_FOR_MULTISERVER_TEST = 10
NUMBER_OF_CLIENT_PER_SERVER_FOR_MULTISERVER_TEST = 10

class CanPktTests(unittest.TestCase):

    def setUp(self):
        """Call before every test case."""
        pass
        
    def tearDown(self):
        """Call after every test case."""
        pass
    # -----------------------------------------

    class ServerRunner():
        
        serverNotifier = None
        serverRunning = False
        server = None
        serverPort = None
        
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
        
        def __init__(self, port):
            log.debug('Creating ServerRunner, port ' + str(port))
            self.serverPort = port
            pass
        
        def startServer(self):
            self.serverNotifier = self.ServerNotifier(self)
            self.server = TCPServer(self.serverPort, self.serverNotifier)
            assert self.server.start() == True
            self.serverRunning = True
            return True
            
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
            self.server = None
            return True
        
    # -----------------------------------------
        
    class ClientRunner():
        
        client = None
        serverPort = None
        
        def __init__(self, port):
            log.debug('Creating ClientRunner, port ' + str(port))
            self.serverPort = port
            pass
        
        def startClient(self):
            self.client = TCPClient('localhost', self.serverPort)
            return self.client.connect(2)
    
        def stopClient(self):
            self.client.disconnect()
            time.sleep(0.1)
            self.client = None
            
    # -----------------------------------------
    
    def testA_TcpServer(self):
        log.debug('testATcpServer')
        serverRunner = self.ServerRunner(SERVER_BASE_PORT)
        assert serverRunner.startServer() == True
        assert serverRunner.stopServer() == True
        serverRunner = None

    def testB_TcpClientNegative(self):
        log.debug('testBTcpClientNegative')
        clientRunner = self.ClientRunner(SERVER_BASE_PORT)
        assert clientRunner.startClient() == False
        clientRunner.stopClient()

    def testC_TcpServerWithSingleClient(self):
        log.debug('testCTcpServerWithSingleClient')
        serverRunner = self.ServerRunner(SERVER_BASE_PORT)
        clientRunner = self.ClientRunner(SERVER_BASE_PORT)
        assert serverRunner.startServer() == True
        assert clientRunner.startClient() == True
        clientRunner.stopClient()
        assert serverRunner.stopServer() == True

    def testD_TcpConnection(self):
        log.debug('testDTcpConnection')
        serverRunner = self.ServerRunner(SERVER_BASE_PORT)
        clientRunner = self.ClientRunner(SERVER_BASE_PORT)
        assert serverRunner.startServer() == True
        assert clientRunner.startClient() == True
        log.debug('Server sends hej')
        assert serverRunner.server.writeAll('hej') == True
        time.sleep(1.0)
        resp = str(clientRunner.client.read())
        log.debug('Client gets:' + resp)
        assert (resp == '\'hej\'')
        clientRunner.stopClient()
        assert serverRunner.stopServer() == True
        
    def testE_MultipleServersAndClients(self):
        log.debug('testEMultipleServers')
        serverList = []
        clientList = []
        for i in range(0, NUMBER_OF_SERVERS_FOR_MULTISERVER_TEST):
            currentPort = SERVER_BASE_PORT + i
            newServer = self.ServerRunner(currentPort)
            serverList.append(newServer)
            newServer.startServer()
            for j in range(0, NUMBER_OF_CLIENT_PER_SERVER_FOR_MULTISERVER_TEST):
                newClient = self.ClientRunner(currentPort)
                clientList.append(newClient)
                newClient.startClient()
                log.debug('Client to ' + str(newClient.serverPort) + ' writing')
                newClient.client.write('hej')
                
        log.debug('Waiting 2 s for threads to work..')
        time.sleep(2.0)
        for s in serverList:
            clientData = s.server.readAll()
            log.debug(clientData)
            log.debug(len(clientData))
            assert len(clientData) == NUMBER_OF_CLIENT_PER_SERVER_FOR_MULTISERVER_TEST
            assert s.stopServer() == True
    
        