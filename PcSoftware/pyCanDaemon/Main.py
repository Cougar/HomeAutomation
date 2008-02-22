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
    
    cfg = None
    nodeIf = None
    ifNotifier = None
    terminated = False
    command_list = []
    command_map = {}
    help_command_map = {}

    def __init__ (self):
        self.command_list = [['help', self.helpCmd, 'display help'],
                       ['log', self.logCmd, 'log control: \"help log\"'],
                       ['exec', self.execCmd,  'run command listing <filename>\n'],
                       ['filter', self.filterCmd, 'filter commands: \"help filter\"'],
                       ['state', self.stateCmd, 'state commands: \"help state\"\n'],
                       ['addif', self.ifAddCmd, 'add interface: \"help addif\"'],
                       ['remif', self.ifRemCmd, 'remove interface <name>'],
                       ['ifup', self.ifUpCmd, 'bring up interface <name>'],
                       ['ifdown', self.ifDownCmd, 'bring down interface <name>\n'],
                       ['addbrigde', self.addBridgeCmd, 'add bridge: \"help addbridge\"'],
                       ['rembridge', self.remBridgeCmd, 'remove bridge <name>\n'],
                       ['tcpd', self.tcpdCmd, 'tcpd server control: \"help tcpd\"'],
                       ['tlsd', self.tlsdCmd, 'tlsd server control: \"help tlsd\"'],
                       ['canctld', self.canCtldCmd, 'canctld server control: \"help canctld\"\n'],
                       ['status', self.statusCmd, 'show daemon status'],
                       ['stats', self.statsCmd, 'show statistics\n'],
                       ['nodelist', self.nodesCmd, 'list can nodes'],
                       ['filterlist', self.filterlistCmd, 'list active filters\n'],
                       ['sim', self.simCmd, 'simulator commands: \"help sim\"']]
        
        index = 0
        for cmd in self.command_list:
            self.command_map[cmd[0]] = index
            index += 1

        logHelp = 'not implemented'
        stateHelp = 'not implemented'
        addifHelp = 'not implemented'
        addbridgeHelp = 'not implemented'
        tcpdHelp = 'not implemented'
        tlsdHelp = 'not implemented'
        canctldHelp = 'not implemented'
        simHelp = 'not implemented'
        self.help_command_map = {'log' : logHelp,
                                'state' : stateHelp,
                                'addif' : addifHelp,
                                'addbridge' : addbridgeHelp,
                                'tcpd' : tcpdHelp,
                                'tlsd' : tlsdHelp,
                                'canctld' : canctldHelp,
                                'sim' : simHelp}

        self.cfg = DaemonConfig()
    
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
        args = args[0]
        if len(args) < 1:
            quitStr = 'quit, exit'
            print quitStr, self.__rightAdjust(quitStr, 'shutdown pyCanDaemon')
            for cmd in self.command_list:
                print cmd[0], self.__rightAdjust(cmd[0], cmd[2])
        elif len(args) < 2:
            subCmd = args[0]
            if self.help_command_map.has_key(subCmd):
                print self.help_command_map[subCmd]
            else:
                print 'no help for ' + subCmd
        else:
            print 'usage: help [topic]'

    def filterCmd(self, *args):
        cmd = args[0]
        filterName = args[1]
        # FIXME: verify 
        if cmd == 'add':
            print 'Importing filter: ' + filterName
            self.cfg.filterCfg.importFilter(filterName)
        elif cmd == 'rem' or cmd == 'remove':
            print 'Deleting filter: ' + filterName
            # FIXME: actually delete
    
    def stateCmd(self, *args):
        cmd = args[0]
        spaceName = args[1]
        if cmd == 'add':
            print 'Importing state space: ' + spaceName
            self.cfg.stateSpaceCfg.importSpace(spaceName)
        elif cmd == 'rem' or cmd == 'remove':
            print 'Deleting state space: ' + spaceName
            # FIXME: actually delete statespace
    
    def ifAddCmd(self, *args):
        print 'not implemented'
    
    def ifRemCmd(self, *args):
        print 'not implemented'

    def ifUpCmd(self, *args):
        print 'not implemented'
 
    def ifDownCmd(self, *args):
        print 'not implemented'
        pass
    
    def addBridgeCmd(self, *args):
        print 'not implemented'

    def remBridgeCmd(self, *args):
        print 'not implemented'
    
    def tcpdCmd(self, *args):
        print 'not implemented'
    
    def tlsdCmd(self, *args):
        print 'not implemented'
    
    def canCtldCmd(self, *args):
        print 'not implemented'
    
    def statusCmd(self, *args):
        print 'not implemented'
    
    def statsCmd(self, *args):
        print 'not implemented'
    
    def nodesCmd(self, *args):
        print 'not implemented'
    
    def filterlistCmd(self, *args):
        print 'not implemented'
    
    def simCmd(self, *args):
        print 'not implemented'
    
    def logCmd(self, *args):
        print 'not implemented'

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
#       self.cfg.filterCfg.importFilter('DefaultFilter')
#       self.cfg.stateSpaceCfg.importSpace('DefaultStateSpace')
        self.cfg.stateSpaceCfg.loadSpaces()
        self.cfg.filterCfg.loadFilters()
        self.cfg.setupFilterBindings()
        self.cfg.setupFilterChain()

        print 'INIT: Starting selected can interface'
        ''' nodeif thread launch '''
        self.ifNotifier = self.IfNotifier()
        pktHandler = CanPktHandler1(self.cfg)
        
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
        while not self.terminated:
            input = raw_input('> ').strip().lower()
            cmd = input.split(' ')
            if len(cmd[0]) < 2:
                continue
            if cmd[0] == 'quit' or cmd[0] == 'exit':
                self.terminated = True
            elif self.command_map.has_key(cmd[0]):
                self.command_list[self.command_map[cmd[0]]][1](cmd[1:])
            else:
                print 'unknown command: ' + cmd[0]

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

        self.cfg.stateSpaceCfg.saveSpaces()
        self.cfg.filterCfg.saveFilters()
        
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
