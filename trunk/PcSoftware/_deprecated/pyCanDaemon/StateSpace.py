###########################################################
#
# Template for a state space
#
###########################################################

class StateSpace:
    
    DESCRIPTIVE_NAME = 'New Packet Filter'
    RELATED_SPACES = []

    ''' Called to restore the default state '''
    def reset(self):
        pass

    ''' Called when state space is loaded, before any calls to run '''
    def load(self):
        pass
    
    ''' Called just before state space is dumped to disk '''
    def unload(self):
        pass
    
    """ Triggered by filters as response to incoming packets
        related: array of statespace objects related to this state space
        args: optional arguments
    """
    def run(self, related, args):
        pass
