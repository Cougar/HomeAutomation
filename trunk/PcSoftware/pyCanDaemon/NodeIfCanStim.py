###########################################################
#
# CanStim interface, used to emulate connected can nodes
#
###########################################################
import logging as log
from NodeIfBase import NodeIfBase
from threading import Thread
import time

''' Generic dummy node '''
class DummyNode():
    pass

''' Temperature sensor node '''
class TempNode(DummyNode):
    pass

''' Relay node, controls switches '''
class RelayNode(DummyNode):
    pass

''' View node, controls an LCD or similar '''
class ViewNode(DummyNode):
    pass

''' IO Node, a bridge to some other protocol over can '''
class IONode(DummyNode):
    pass

class CanStimThread(Thread):
    dummyList = []
    numDummies = 0
    terminated = False
    
    def __init__ (self):
        Thread.__init__(self)
    def run(self):
        log.debug('CanStimThread.run')
        while not self.terminated:
            time.sleep(0.1)
    def terminate(self):
        self.terminated = True

class NodeIfCanStim(NodeIfBase):
    DEFAULT_CONFIG = {'DummyNodes':[]}
    
    stimThread = None
    
    def __init__ (self, cfg = None, pktHandler = None):
        if cfg is None:
            self.config = DEFAULT_CONFIG
            
        NodeIfBase.__init__(self, cfg, pktHandler)
        
    def start(self):
        stimThread = CanStimThread()
        stimThread.start()
        
    def stop(self):
        stimThread.dummyList.clear()

    def removeDummy(self, DummyNode):
        ''' if possible'''
        stimThread.dummyList.remove(DummyNode)
        DummyNode.terminate()
    
    def addDummy(self, DummyNode):
        '''if possible'''
        stimThread.dummyList.add(DummyNode)
        DummyNode.start()

