###########################################################
#
# Node interface through raw TCP socket (telnet)
#
###########################################################

class NodeIfTelnet:
    config = []
    pktHandler = None
    
    def __init__(self, cfg, pktHandler):
        self.config = cfg
        self.pktHandler = pktHandler
        
    def setPktHandler(self, pktHandler):
        self.pktHandler = pktHandler
    
    def start(self):
        pass

    def stop(self):
        pass
    
    def running(self):
        pass
