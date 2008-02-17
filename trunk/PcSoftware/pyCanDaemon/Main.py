#!/bin/env python

import os
import sys
import logging as log
import getopt
import atexit

from CanPktHandlerBase import CanPktHandlerBase
from CanPktHandler1 import CanPktHandler1

from NodeIfBase import NodeIfBase
from NodeIfCanStim import NodeIfCanStim
from NodeIfSerial import NodeIfSerial

from DaemonConfig import DaemonConfig

daemonApp = None

class CanDaemon():
    
    nodeIf = None
    
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
    
    
    def exceptHelper(self, type, value, tb):
          import traceback
          traceback.print_exc()
          print 'Exception in main program, shutting down threads'
          self.exitHelper()
    
    
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
    #    cfg.filterCfg.importFilter('DefaultFilter')
    #    cfg.stateSpaceCfg.importSpace('DefaultStateSpace')
        cfg.stateSpaceCfg.loadSpaces()
        cfg.filterCfg.loadFilters()
        cfg.setupFilterBindings()
        cfg.setupFilterChain()
        
        print 'INIT: Starting selected can interface'
        ''' nodeif thread launch '''
        pktHandler = CanPktHandler1(cfg)
        
     #   cfg = NodeIfCanStim.DEFAULT_CONFIG
     #   nodeif = NodeIfCanStim(cfg, pkthandler)
        ifConfig = NodeIfSerial.DEFAULT_CONFIG
    
        atexit.register(self.exitHelper)
        sys.excepthook = self.exceptHelper
    
        self.nodeIf = NodeIfSerial(pktHandler, ifConfig)
        
        if not self.nodeIf.start():
            print 'FATAL: Failed to initialize node interface.'
            print 'This may indicate that the interface is busy or that you don\'t have ' \
                  'permission to access it.'
            sys.exit(-1)

        print 'Initialize/gather info on connected can nodes and load associated state'
        print 'Start selected TCP and UDP server(s)'
        # server threads launch
        
        print 'CanDaemon v1 ready'
        print 'Enter command or type \"help\" for a list of commands'
        terminated = False
        while not terminated:
            input = raw_input('> ').strip().lower()
            if input == 'quit' or input == 'exit':
                terminated = True
            elif input == 'help':
                helpCommands()
        
        print 'Shutdown'
        self.nodeIf.stop()
        cfg.stateSpaceCfg.saveSpaces()
        cfg.filterCfg.saveFilters()

#---------------------------------------------------------------------------

#def install_thread_excepthook():
#    """Workaround for sys.excepthook thread bug
#       (https://sourceforge.net/tracker/?func=detail&atid=105470&aid=1230540&group_id=5470).
#       Call once from __main__ before creating any threads.
#       If using psyco, call psyco.cannotcompile(threading.Thread.run)
#       since this replaces a new-style class method.
#    """
#    import sys, threading
#    run_old = threading.Thread.run
#    def run(*args, **kwargs):
#        print 'WORKAROUND RUN'
#        try:
#            run_old(*args, **kwargs)
#        except (KeyboardInterrupt, SystemExit):
#            raise
#        except:
#            sys.excepthook(*sys.exc_info())
#    threading.Thread.run = run
        
#---------------------------------------------------------------------------

def main():
    daemonApp = CanDaemon()
#    install_thread_excepthook()
    daemonApp.run()

#---------------------------------------------------------------------------

if __name__ == '__main__':
    __name__ = 'Main'
    main()
