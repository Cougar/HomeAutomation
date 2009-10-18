###########################################################
#
# Template for a packet filter
#
###########################################################

class PktFilter:
    
    DESCRIPTIVE_NAME = 'New Packet Filter'
    ASSOCIATED_SPACES = ['MyStateSpace']

    ''' Called when the filter is attached to the packet filter chain '''
    def attach(self):
        pass
    
    ''' Called when the filter is removed to the filter chain '''
    def detach(self):
        pass
    
    ''' Defines the filter behavior '''
    def filter(self, pkt):
        pass
