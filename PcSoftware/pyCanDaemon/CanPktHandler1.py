###########################################################
#
# The default implementation of the packet handler
#
###########################################################
from CanPktHandlerBase import CanPktHandlerBase
from CanPkt import CanPkt
import logging as log

class SpacePort():
    
    filterSpaces = {}
    
    def __init__ (self, filterSpaces):
        for fs in filterSpaces:
            sName = fs.__module__.split('.')[1] # implies module name is statespace.XXX
            self.filterSpaces[sName] = fs
    
    def runAll(self, args):
        for fs in self.filterSpaces.values():
            relatedSpaces = fs.__RELATED_SPACES__
            fs.run(relatedSpaces, args)
        return True
    
    def run(self, spaceName, args):
        if not self.filterSpaces.has_key(spaceName):
            return False
        else:
            relatedSpaces = self.filterSpaces[spaceName].__RELATED_SPACES__
            self.filtersSpaces[spaceName].run(relatedSpaces, args)
            return True

class CanPktHandler1(CanPktHandlerBase):
    
    daemonCfg = None
    serverListeners =[]
    
    def __init__ (self, daemonCfg):
        self.daemonCfg = daemonCfg
        
    def input(self, strdata):
        """Make data buffer into CanPkt and do early discard check """
        """Call all filters in chain """

        if len(strdata) < 2:
            return
        if strdata[0:3] != 'PKT':
            return

#        print strdata
        pkt = CanPkt.stringToCanPkt(self, strdata)
#        print 'incoming: ' + pkt.toString()

        for filter in self.daemonCfg.filterChain:
            fObj = self.daemonCfg.filterCfg.filterModules[filter]
            sp = SpacePort(fObj.__ASSOCIATED_SPACES__)
            fObj.filter('', pkt, sp) # FIXME: use if name
            
        for sl in self.serverListeners:
            if not sl.full():
                sl.write(pkt.toString())
            else:
                log.debug('WARNING: Overflow in server buffer: ' + sl + ', packet dropped')

    def output(self, data):
        pass
    
    def addServerListener(self, serverListener):
        self.serverListeners.append(serverListener)
    
    def remServerListener(self, serverListener):
        self.serverListeners.remove(serverListerner)
    