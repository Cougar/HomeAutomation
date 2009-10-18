from TCPClient import TCPClient
import time
import signal
import sys

HOST = 'sra.eta.chalmers.se'
PORT = 8002

client = None
serverPort = None
terminated = False

def sigIntHandler(signum, frame):
    print 'Interrupted, exiting'
    client.disconnect()
    sys.exit(0)

def Main():
    
    signal.signal(signal.SIGINT, sigIntHandler)
    client = TCPClient(HOST, PORT)
    
    print 'Connecting to ' + HOST + ':' + str(PORT) + ' ..'
    if not client.connect(10):
        print 'Connection to host failed'
        sys.exit(-1)

    runTime = 0
    while not terminated:
        data = client.read()
        if data is not None:
            print data
        time.sleep(0.1)
        runTime += 0.1
        if runTime > 10:
            print 'Terminating'
            break

Main()
