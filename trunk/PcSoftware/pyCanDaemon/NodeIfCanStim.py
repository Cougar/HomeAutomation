###########################################################
#
# CanStim interface, used to emulate connected can nodes
#
# - Create fake node network
# - Replay log
#
###########################################################
import logging as log
from NodeIfBase import NodeIfBase
from threading import Thread
from CanPkt import CanPkt
import time

''' Generic dummy node '''
class DummyNode():
    def tick(self):
        pass


''' Temperature sensor node '''
class TempNode(DummyNode):
    
    frames = 0
    
    def __init__ (self):
        pass
    
    def read(self):
        self.frames = self.frames + 1
        if self.frames % 100 == 0:
            pktstr = CanPkt(0x1f8f0100,[0x5f,0x3a,0x40,0x01]).toString()
            return pktstr

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
    ownerNotifier = None
    pktHandler = None
    
    def __init__ (self, pktHandler, ownerNotifier):
        self.ownerNotifier = ownerNotifier
        self.pktHandler = pktHandler
        Thread.__init__(self)
        
        
    def run(self):
        log.debug('CanStimThread.run')
        try:
            readBuffer = None
            while not self.terminated:
                for dummy in self.dummyList:
                    readBuffer = dummy.read()
                    if readBuffer and len(readBuffer) > 0:
                        self.pktHandler.input(readBuffer)
                time.sleep(0.001)
                
            self.ownerNotifier.notify(self.ownerNotifier.TERMINATE)
            
        except: # sys.excepthook does not work in threads
            import traceback
            traceback.print_exc()
            self.terminate()
            
    def terminate(self):
        log.debug('CanStimThread.terminate')
        self.terminated = True
        

class NodeIfCanStim(NodeIfBase):
    
    DEFAULT_CONFIG = {'DummyNodes':[]}
    config = None
    stimThread = None
    ownerNotifier = None
    pktHandler = None
    
    def __init__ (self, pktHandler, cfg = None):
        if cfg is None:
            self.config = DEFAULT_CONFIG
            
        self.pktHandler = pktHandler
        NodeIfBase.__init__(self, cfg, pktHandler)
    
    def setPktHandler(self, pktHandler):
        self.pktHandler = pktHandler
        
    def setIfNotifier(self, ifNotifier):
        self.ownerNotifier = ifNotifier
        
    def start(self):
        self.stimThread = CanStimThread(self.pktHandler, self.ownerNotifier)
        tempDummy = TempNode()
        self.stimThread.dummyList.append(tempDummy)
        self.stimThread.start()
        return True
    
    def stop(self):
        if self.stimThread is not None and not self.stimThread.terminated:
            self.stimThread.terminate()
        else:
            log.debug('CanStim interface not started, or already stopped')
            
    def running(self):
        if self.stimThread is not None:
            return not self.stimThread.terminated
        else:
            return False

    def removeDummy(self, DummyNode):
        ''' if possible'''
        self.stimThread.dummyList.remove(DummyNode)
    
    def addDummy(self, DummyNode):
        '''if possible'''
        self.stimThread.dummyList.add(DummyNode)

