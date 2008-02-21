#!/bin/env python

import os
import sys
import logging as log
import getopt
import atexit
import time

from CanPktHandlerBase import CanPktHandlerBase
from CanPktHandler1 import CanPktHandler1

from NodeIfBase import NodeIfBase
from NodeIfCanStim import NodeIfCanStim
from NodeIfSerial import NodeIfSerial

from DaemonConfig import DaemonConfig

daemonApp = None

class CanDaemon():
    
    nodeIf = None
    ifNotifier = None
    terminated = False
    
    class IfNotifier():
    
        UNDEFINED = 0
        TERMINATE = 1
    
        def notify(self, msg):
            if msg == self.TERMINATE:
                print 'Main terminate notify'
                self.terminated = True
            else:
                print 'UNDEFINED NOTIFY'
        
    def __init__ (self):
        pass
    
    
    def usage(self):
        pass
    
    
    def helpCommands(self):
        print 'quit, exit     : shutdown CanDaemon'
        print 'help           : display help'
    
    
    def exitHelper(self):
        if self.nodeIf is not None and self.nodeIf.running():
            self.nodeIf.stop()
          
    
    def run(self):
        try:
            demoPath = os.path.dirname(__file__)
            os.chdir(demoPath)
        except:
            return
        
        print 'INIT: Start logging'
        log.basicConfig(level=log.DEBUG)
        
        print 'INIT: Get config options from command line / config file'
        cfg = DaemonConfig()
        
        try:
            opts, args = getopt.getopt(sys.argv[1:], "ho:v", ["help", "output="])
        except getopt.GetoptError:
            print 'Invalid usage'
            usage()
            sys.exit(2)
     #   output = None
     #   verbose = False
        for o, a in opts:
            if o == "-v":
                pass
     #           verbose = True
            if o in ("-h", "--help"):
                usage()
                sys.exit()
            if o in ("-o", "--output"):
                pass
    #            output = a
    
        print 'INIT: Loading filters and spaces'
  #      cfg.filterCfg.importFilter('DefaultFilter')
  #      cfg.stateSpaceCfg.importSpace('DefaultStateSpace')
        cfg.stateSpaceCfg.loadSpaces()
        cfg.filterCfg.loadFilters()
        cfg.setupFilterBindings()
        cfg.setupFilterChain()
        
        print 'INIT: Starting selected can interface'
        ''' nodeif thread launch '''
        self.ifNotifier = self.IfNotifier()
        pktHandler = CanPktHandler1(cfg)
        
        ifConfig = NodeIfCanStim.DEFAULT_CONFIG
        self.nodeIf = NodeIfCanStim(ifConfig, pktHandler, self.ifNotifier)
 #       ifConfig = NodeIfSerial.DEFAULT_CONFIG
    
        atexit.register(self.exitHelper)
 #       sys.excepthook = self.exceptHelper
 #       self.nodeIf = NodeIfSerial(pktHandler, ifConfig)
        
        if not self.nodeIf.start():
            print 'FATAL: Failed to initialize node interface.'
            print 'This may indicate that the interface is unavailable, ' \
                  'or that you do not have permission to access it.'
            sys.exit(-1)

        print 'Initialize/gather info on connected can nodes and load associated state'
        print 'Start selected TCP and UDP server(s)'
        # server threads launch
        
        print 'CanDaemon v1 ready'
        print 'Enter command or type \"help\" for a list of commands'

        while not self.terminated:
            input = raw_input('> ').strip().lower()
            if input == 'quit' or input == 'exit':
                self.terminated = True
            elif input == 'help':
                self.helpCommands()
            else:
                print 'Unknown command: ' + input
        
        print 'Shutdown'
        if self.nodeIf.running():
            self.nodeIf.stop()

        cfg.stateSpaceCfg.saveSpaces()
        cfg.filterCfg.saveFilters()
        
        time.sleep(0.1) # wait for threads
        sys.exit(0) # will except if there are threads remaining

#---------------------------------------------------------------------------

def main():
    daemonApp = CanDaemon()
#    install_thread_excepthook()
    daemonApp.run()

#---------------------------------------------------------------------------

if __name__ == '__main__':
    __name__ = 'Main'
    main()
