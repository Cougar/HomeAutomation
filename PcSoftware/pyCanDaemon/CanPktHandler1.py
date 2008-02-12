###########################################################
#
# The default implementation of the packet handler
#
###########################################################
from CanPktHandlerBase import CanPktHandlerBase

class CanPktHandler1(CanPktHandlerBase):
    
    daemonCfg = None
    
    def __init__ (self, daemonCfg):
        self.daemonCfg = daemonCfg
    
    def input(self, data):
        """Make data buffer into CanPkt and do early discard check """
        """Call all filters in chain """
        
        if len(data) < 2:
            return
        if data[0:3] != 'PKT':
            return
        
        print data
        
        # FIXME: convert data to class canpkt before passing to filter chain
        
        for filter in daemonConfig.filterChain:
            fObj = cfg.filterCfg.filterModules[filter]
            fObj.filter(data, fObj.__ASSOCIATED_SPACES__)
    
    def output(self, data):
        pass
