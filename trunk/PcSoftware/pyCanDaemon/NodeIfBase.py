###########################################################
#
# Template for node interface
#
###########################################################

class NodeIfBase:
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
