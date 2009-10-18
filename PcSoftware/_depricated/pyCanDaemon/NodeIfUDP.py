###########################################################
#
# IP/UDP interface, connects with can through an
# ethernet node
#
###########################################################

class NodeIfUDP:
    config = []
    pktHandler = None
    ownerNotifier = None
    
    def __init__(self, pktHandler, cfg = None):
        self.config = cfg
        self.pktHandler = pktHandler
        
    def setPktHandler(self, pktHandler):
        self.pktHandler = pktHandler
        
    def setIfNotifier(self, ifNotifier):
        self.ownerNotifier = ifNotifier
    
    def start(self):
        pass

    def stop(self):
        pass

    def running(self):
        pass
    