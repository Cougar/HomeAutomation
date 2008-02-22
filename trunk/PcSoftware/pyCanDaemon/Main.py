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
    command_list = []

    def __init__ (self):
        pass
    
    class IfNotifier():
        """ Used by if threads to send messages to the main program thread """
        UNDEFINED = 0
        TERMINATE = 1
    
        def notify(self, msg):
            if msg == self.TERMINATE:
                print 'Main terminate notify'
                self.terminated = True
            else:
                print 'UNDEFINED NOTIFY'
        
    
    def __rightAdjust(self, refstr, str):
        spcstr = ''
        endpos = 20-len(refstr)
        for i in range(1,endpos):
            if i == endpos-5:
                spcstr += ':'
            else:
                spcstr += ' '
        return (spcstr+str)
    
    def helpCmd(self, *args):
        quitStr = 'quit, exit'
#        print quitStr, self.__rightAdjust(quitStr, 'shutdown pyCanDaemon')
#        for cmd in cmdNames:
#            print cmd, self.__rightAdjust(cmd, command_map[cmd][1])
        
    def filterCmd(self, *args):
        pass
    
    def stateCmd(self, *args):
        pass
    
    def ifAddCmd(self, *args):
        pass
    
    def ifRemCmd(self, *args):
        pass
    
    def ifUpCmd(self, *args):
        pass
    
    def ifDownCmd(self, *args):
        pass
    
    def addBridgeCmd(self, *args):
        pass
    
    def remBridgeCmd(self, *args):
        pass
    
    def tcpdCmd(self, *args):
        pass
    
    def tlsdCmd(self, *args):
        pass
    
    def canCtldCmd(self, *args):
        pass
    
    def statusCmd(self, *args):
        pass
    
    def statsCmd(self, *args):
        pass
    
    def nodesCmd(self, *args):
        pass
    
    def filterlistCmd(self, *args):
        pass
    
    def simCmd(self, *args):
        pass
    
    def logCmd(self, *args):
        pass
    
    def execCmd(self, *args):
        pass
    
    def exitHelper(self):
        if self.nodeIf is not None and self.nodeIf.running():
            self.nodeIf.stop()
            
    def exceptHelper(self, type, value, tb):
        import traceback
        traceback.print_exception(type, value, tb)
        print '\nException in main program, shutting down threads'
        self.terminated = True
        self.exitHelper()
 #       sys.exit(-1)

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
        sys.excepthook = self.exceptHelper
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
        
        global command_map
        command_map = {'help' : (self.helpCmd, 'display help'),
                       'filter' : (self.filterCmd, 'filter commands: \"help filter\"'),
                       'state' : (self.stateCmd, 'state commands: \"help state\"'),
                       'addif' : (self.ifAddCmd, 'add interface: \"help addif\"'),
                       'remif' : (self.ifRemCmd, 'remove interface <name>'),
                       'ifup' : (self.ifUpCmd, 'bring up interface <name>'),
                       'ifdown' : (self.ifDownCmd, 'bring down interface <name>'),
#                       'addbrigde' : (self.addBridgeCmd, 'add bridge: \"help addbridge\"'),
#                       'rembridge' : (self.remBridgeCmd, 'remove bridge <name>'),
                       'tcpd' : (self.tcpdCmd, 'tcpd server control: \"help tcpd\"'),
#                       'tlsd' : (self.tlsdCmd, 'tlsd server control: \"help tlsd\"'),
#                       'canctld' : (self.canCtldCmd, 'canctld server control: \"help canctld\"'),
#                       'status' : (self.statusCmd, 'show daemon status'),
#                       'stats' : (self.statsCmd, 'show statistics'),
#                       'nodelist' : (self.nodesCmd, 'list can nodes'),
#                       'filterlist' : (self.filterlistCmd, 'list active filters'),
#                       'sim' : (self.simCmd, 'simulator commands: \"help sim\"'),
#                       'log' : (self.logCmd, 'log control: \"help log\"'),
#                       'exec' : (self.execCmd,  'run command listing <filename>')}

        while not self.terminated:
            input = raw_input('> ').strip().lower()
            cmd = input.split(' ')
            if cmd[0] == 'quit' or cmd[0] == 'exit':
                self.terminated = True
            elif command_map.has_key(cmd[0]):
                command_map[cmd[0]][0](cmd[1:])
            else:
                print 'Unknown command: ' + cmd[0]

        """ -- suggested commands -- p n = priority to implement (1 is highest)
            base: (p 1)
            filter add <name>
            filter rem <name>
            state add <name>
            state rem <name>
            state reset <name>
            
            	node interfaces:  (p 1)
            addif <serial/sim/udp/telnet> <options> (help protocol gives options)
            remif <name> - delete an interface
            ifup <name> - bring up an interface
            ifdown <name> - bring down an interface
            
            	bridging: (p 2)
            addbridge <name> <ifname1> <ifname2> <ifname3> ... - create if bridge
            rembridge <name>
            
            	servers:
            tcpd start [port] - raw tcp (telnet, compat) server, all pkts  (p 1)
            tcpd stop
            tlsd start [port] - tcp+tls server, all pkts  (p 2)
            tlsd stop
            canctld start [port] - pyCanGUI server (tcp/tls)  (p 2)
            canctld stop
            
            info commands: (p 3)
            status - shows active settings, interfaces, servers
            stats - shows some traffic statistics
            nodes - shows active nodes 
            filters - shows active filters
            state [name] - shows active states, opt specific state info\
            log - log control (TBD)
            
            simulator commands: (p 2)
            simrecord <file> - record incoming packets
            simplay <file> [timescale] - play recorded stream, opt accel time:
            simadd <nodeid> (nodetype) 
            simrem <nodeid>
            simsave <config>
            simload <config>
        """
 
        """ -- other features --
            startup.cfg - script to run at startup (command listing)
            ie: ifup 0
                tlsd start
                canguid start
            config.cfg - last daemon config (param override)
            default.cfg - default daemon config (only read if config.cfg does not exist)
        """

        print 'Shutdown'
        if self.nodeIf.running():
            print 'Stopping If thread'
            self.nodeIf.stop()

        cfg.stateSpaceCfg.saveSpaces()
        cfg.filterCfg.saveFilters()
        
        print 'Waiting 0.1..'
        time.sleep(0.1) # wait for threads
        print 'Terminating'
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
