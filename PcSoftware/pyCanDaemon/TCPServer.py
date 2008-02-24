import socket
from threading import Thread
from threading import Lock
import logging as log
import time

class TCPServerThread(Thread):
    
    ownerNotifier = None
    sock = None
    terminated = False
    
    clientSocks = []
    clientSocksLock = None
    
    def __init__ (self, sock, ownerNotifier):
        Thread.__init__(self)
        self.sock = sock
        self.ownerNotifier = ownerNotifier
        self.clientSocksLock = Lock()
        
    def cleanup(self):
        self.sock.close()
        self.clientSocksLock.acquire()
        for clsock, addr in self.clientSocks:
            clsock.close()
        self.clientSocksLock.release()

        if self.ownerNotifier is not None:
            self.ownerNotifier.notify(self.ownerNotifier.TERMINATE)

    def run(self):
        try:
            log.debug('TCPServerThread.run')
            while not self.terminated:
                try:
                    conn, addr = self.sock.accept()
                    print 'Client connected from ' + str(addr)
                    conn.setblocking(0)
                    self.clientSocksLock.acquire()
                    self.clientSocks.append((conn, addr))
                    self.clientSocksLock.release()
                except socket.timeout:
                    pass
            self.cleanup()
        except: # sys.excepthook does not work in threads
            import traceback
            traceback.print_exc()
            self.terminated = True
            self.cleanup()
    
    def terminate(self):
        log.debug('TCPServerThread.terminate')
        self.terminated = True

class TCPServer():
    
    serverThread = None
    ownerNotifier = None
    port = 50007
    host = ''
    
    def __init__(self, port, ownerNotifier = None):
        self.port = port
        self.ownerNotifier = ownerNotifier
        
    def start(self):
        if self.serverThread is not None:
            log.debug('TCPServer is already running')
            return
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except Exception, e:
            print 'TCP Server start failed:', e
            return False
        try:
            s.bind((self.host, self.port))
            s.listen(1)
            s.settimeout(0.5)
        except Exception, e:
            print 'TCP Server start failed:', e
            s.close()
            return False
        try:
            self.serverThread = TCPServerThread(s, self.ownerNotifier)
            self.serverThread.start()
        except Exception, e:
            print 'TCP Server start failed:', e
            if self.serverThread is not None and self.serverThread.running():
                self.serverThread.terminate()
            s.close()
            return False
        return True

    def stop(self):
        if self.running():
            self.serverThread.terminate()
        else:
            log.debug('TCPServer not started, or already stopped')

    def running(self):
        if self.serverThread is not None:
            return not self.serverThread.terminated
        else:
            return False

    def readAll(self):
        cldata = []
        self.serverThread.clientSocksLock.acquire()
        for clsock,addr in self.serverThread.clientSocks:
            try:
                data = clsock.s.recv(1024)
            except Exception, e:
                log.debug('TCPServer client socket error: ' + str(e))
                self.serverThread.clientSocksLock.release()
                return None
            cldata.append((addr, data))
        self.serverThread.clientSocksLock.release()
        return cldata

    def writeAll(self, data):
        self.serverThread.clientSocksLock.acquire()
        for clsock,addr in self.serverThread.clientSocks:
            try:
                clsock.send(data)
            except Exception, e:
                log.debug('TCPServer client socket error: ' + str(e))
                self.serverThread.clientSocksLock.release()
                return None
        self.serverThread.clientSocksLock.release()

