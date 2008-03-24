import socket

class TCPClient():
    
    host = 'localhost'
    port = 50007
    sock = None
    
    def __init__(self, host, port):
        self.host = host
        self.port = port
    
    def connect(self, timeout = None):
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            if timeout is not None:
                self.sock.settimeout(timeout)
            self.sock.connect((self.host, self.port))
            self.sock.settimeout(0.5)
            return True
        except Exception, e:
            print 'TCP Client connect failed:', e
            return False
        
    def disconnect(self):
        if self.sock is not None:
            self.sock.close()

    def read(self):
        try:
            data = self.sock.recv(1024)
            return repr(data)
        except socket.timeout:
            pass
        return None
        
    def write(self, data):
        self.sock.send(data)
