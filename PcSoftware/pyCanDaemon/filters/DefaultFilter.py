from PktFilter import PktFilter
from CanPkt import CanPkt
from StateSpace import StateSpace

class DefaultFilter(PktFilter):
    
    DESCRIPTIVE_NAME = 'Default Packet Filter'
    ASSOCIATED_SPACES = ['DefaultStateSpace']

    def attach(self):
        pass
    
    def detach(self):
        pass
    
    def filter(self, pkt, spaces):
        
#        print 'DefaultFilter, PKT from node', pkt.nodeId, spaces
        
        ''' Trigger only when nodeId is not equal to 1 '''
        if pkt.nodeId is not 1:
            return
    
        ''' Call associated state spaces '''
        for s in spaces:
            s.run()
