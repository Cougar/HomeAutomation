###########################################################
#
# Template for a state space
#
###########################################################

class StateSpace:
    
    DESCRIPTIVE_NAME = 'New Packet Filter'

    ''' Called to restore the default state '''
    def reset(self):
        pass

    ''' Called when state space is loaded, before any calls to run '''
    def load(self):
        pass
    
    ''' Called just before state space is dumped to disk '''
    def unload(self):
        pass
    
    ''' Triggered by filters as response to incoming packets '''
    def run(self, args):
        pass
