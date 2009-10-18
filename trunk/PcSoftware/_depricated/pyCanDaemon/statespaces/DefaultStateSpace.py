from StateSpace import StateSpace

""" from SPServices import SPServices

    SpServices.registerTimer(self.function, interval)
    SpServices.registerEventHandler(self.function, event)
    SpServices.sendEvent()
    SpServices.sendPkt()
"""

class DefaultStateSpace(StateSpace):

    DESCRIPTIVE_NAME = 'Default State Space'
    RELATED_SPACES = []
    
    state_var = 0
    state_string = 'nice'

    def reset(self):
        state_var = 0
        state_string = 'reset'
        pass

    def load(self):
        pass

    def unload(self):
        pass

    def run(self, related, args = None):
        self.state_var = self.state_var + 1
        print 'DefaultStateSpace: ' + str(self.state_var)
