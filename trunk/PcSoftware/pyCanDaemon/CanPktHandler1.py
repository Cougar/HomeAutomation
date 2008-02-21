###########################################################
#
# The default implementation of the packet handler
#
###########################################################
from CanPktHandlerBase import CanPktHandlerBase
from CanPkt import CanPkt

class CanPktHandler1(CanPktHandlerBase):
    
    daemonCfg = None
    
    def __init__ (self, daemonCfg):
        self.daemonCfg = daemonCfg
        
    def input(self, strdata):
        """Make data buffer into CanPkt and do early discard check """
        """Call all filters in chain """
        
        if len(strdata) < 2:
            return
        if strdata[0:3] != 'PKT':
            return

        pkt = CanPkt.stringToCanPkt(self, strdata)

        for filter in self.daemonCfg.filterChain:
            fObj = self.daemonCfg.filterCfg.filterModules[filter]
            fObj.filter(pkt, fObj.__ASSOCIATED_SPACES__)
    
    
    def output(self, data):
        pass
