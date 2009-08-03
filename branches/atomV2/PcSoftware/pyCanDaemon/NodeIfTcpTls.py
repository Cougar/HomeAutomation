###########################################################
#
# Node interface for raw tcp through TLS (static-key)
#
###########################################################

class NodeIfTCPTLS:
    config = []
    pktHandler = None
    ownerNotifier = None
    
    DEFAULT_CONFIG = {}
    
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
