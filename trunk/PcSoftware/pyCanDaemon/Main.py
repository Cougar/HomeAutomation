#!/bin/env python

import os
import sys
import logging as log
import getopt
import atexit
import time

from NodeIfBase import NodeIfBase
from NodeIfCanStim import NodeIfCanStim
from NodeIfSerial import NodeIfSerial
from NodeIfTCP import NodeIfTCP

from DaemonConfig import DaemonConfig

daemonApp = None

class CanDaemon():
    
    cfg = None
    ifNotifier = None
    terminated = False
    
    command_list = []
    command_map = {}
    CMDLIST_NAME = 0
    CMDLIST_HANDLER = 1
    CMDLIST_PARAMINFO = 2
    CMDLIST_HELP = 3

    logHelp = 'usage:\t log [options]\n\n' \
              '\t options:\n' \
              '\t level <num>, where: 0 < num < 3\n'
    stateHelp = 'usage: state [name/add/rem]'
    filterHelp = 'usage: filter [name/add/rem]'
    addifHelp = 'usage: addif [name] [options]'
    addbridgeHelp = 'usage: addbridge [interface names]'
    tcpdHelp = 'usage: tcpd [start/stop/configure] [options]'
    tlsdHelp = 'usage: tlsd [start/stop/configure] [options]'
    canctldHelp = 'usage: canctld [start/stop/configure] [options]'
    simHelp = 'usage: sim [record/play/add/rem/save/load] [options]'
    help_command_map = {}
    
    logOptions = {}
    ifOptions = {}
    bridgeOptions = {}
    tcpdOptions = {}
    tlsdOptions = {}
    canctldOptions = {}
    simOptions = {}

    logValidOptions = {'level' : ['range', 0, 3]}
    ifValidOptions = {'type' : ['alt', 'serial','udp']}
    
    ifSerialOptions = {'' : []}
    ifCanStimOptions = {'' : []}
    ifTelnetOptions = {'' : []}
    ifTcpTlsOptions = {'' : []}
    ifUDPOptions = {'' : []}
    
    bridgeValidOptions = {'' : ['']}
    tcpdValidOptions = {'' : []}
    tlsdValidOptions = {'' : []}
    canctldValidOptions = {'' : []}
    simValidOptions = {'' : []}

    def __init__ (self):
        
        self.command_list = [['help', self.helpCmd, '', 'display help'],
                       ['log', self.logCmd, '<options>', 'log control: \"help log\"'],
                       ['exec', self.execCmd, '<filename>', 'run command listing\n'],
                       ['filter', self.filterCmd, '<cmd>', 'filter commands: \"help filter\"'],
                       ['state', self.stateCmd, '<cmd>', 'state commands: \"help state\"\n'],
                       ['addif', self.ifAddCmd, '<options>', 'add interface: \"help addif\"'],
                       ['remif', self.ifRemCmd, '<name>', 'remove interface'],
                       ['ifup', self.ifUpCmd, '<name>', 'bring up interface'],
                       ['ifdown', self.ifDownCmd, '<name>', 'bring down interface\n'],
                       ['addbrigde', self.addBridgeCmd, '<options>', 'add bridge: \"help addbridge\"'],
                       ['rembridge', self.remBridgeCmd, '<id>', 'remove bridge\n'],
                       ['tcpd', self.tcpdCmd, '<cmd>', 'tcpd server control: \"help tcpd\"'],
                       ['tlsd', self.tlsdCmd, '<cmd>', 'tlsd server control: \"help tlsd\"'],
                       ['canctld', self.canCtldCmd, '<cmd>', 'canctld server control: \"help canctld\"\n'],
                       ['status', self.statusCmd, '', 'show daemon status'],
                       ['stats', self.statsCmd, '', 'show statistics'],
                       ['nodelist', self.nodesCmd, '', 'list can nodes\n'],
                       ['sim', self.simCmd, '<cmd>', 'simulator commands: \"help sim\"']]
        
        index = 0
        for cmd in self.command_list:
            self.command_map[cmd[0]] = index
            index += 1

        self.help_command_map = {'log' : self.logHelp,
                                'filter' : self.filterHelp,
                                'state' : self.stateHelp,
                                'addif' : self.addifHelp,
                                'addbridge' : self.addbridgeHelp,
                                'tcpd' : self.tcpdHelp,
                                'tlsd' : self.tlsdHelp,
                                'canctld' : self.canctldHelp,
                                'sim' : self.simHelp}

        self.cfg = DaemonConfig()
    
    class IfNotifier():
        """ Used by if threads to send messages to the main program thread """
        
        parent = None
        UNDEFINED = 0
        TERMINATE = 1
        
        def __init__(self, parent):
            self.parent = parent
    
        def notify(self, msg):
            if msg == self.TERMINATE:
                print 'Main terminate notify'
            else:
                print 'UNDEFINED NOTIFY'
        
    
    def __rightAdjust(self, refstr, str):
        spcstr = ''
        endpos = 25-len(refstr)
        for i in range(1,endpos):
            if i == endpos-5:
                spcstr += ':'
            else:
                spcstr += ' '
        return (spcstr+str)
    
    def helpCmd(self, *args):
        try:
            args = args[0]
        except:
            args = []
        if len(args) < 1:
            quitStr = 'quit, exit'
            print quitStr, self.__rightAdjust(quitStr, 'shutdown pyCanDaemon')
            for cmd in self.command_list:
                cmdstr = cmd[self.CMDLIST_NAME] + ' ' + cmd[self.CMDLIST_PARAMINFO]
                print cmdstr, \
                      self.__rightAdjust(cmdstr, cmd[self.CMDLIST_HELP])
        elif len(args) < 2:
            subCmd = args[0]
            if self.help_command_map.has_key(subCmd):
                print self.help_command_map[subCmd]
            else:
                print 'no help for ' + subCmd
        else:
            print 'usage: help [topic]'

    def filterCmd(self, *args):
        try:
            args = args[0]
        except:
            args = []
        log.debug('filterCmd ' + str(args))
        if len(args) < 2:
            print 'Invalid arguments\n' + self.filterHelp
            return
        cmd = args[0]
        filterName = args[1]
        # FIXME: verify 
        if cmd == 'add':
            print 'Importing filter: ' + filterName
            self.cfg.filterCfg.importFilter(filterName)
        elif cmd == 'rem' or cmd == 'remove':
            print 'Deleting filter: ' + filterName
            # FIXME: actually delete
            raise 'Not implemented'
    
    def stateCmd(self, *args):
        try:
            args = args[0]
        except:
            args = []
        log.debug('stateCmd ' + str(args))
        if len(args) < 2:
            print 'Invalid arguments\n' + self.stateHelp
            return
        cmd = args[0]
        spaceName = args[1]
        # FIXME: verify 
        if cmd == 'add':
            print 'Importing state space: ' + spaceName
            self.cfg.stateSpaceCfg.importSpace(spaceName)
        elif cmd == 'rem' or cmd == 'remove':
            print 'Deleting state space: ' + spaceName
            # FIXME: actually delete statespace
            raise 'Not implemented'
    
    def ifAddCmd(self, *args):
        raise 'not implemented'
    
    def ifRemCmd(self, *args):
        raise 'not implemented'

    def ifUpCmd(self, *args):
        raise 'not implemented'
 
    def ifDownCmd(self, *args):
        raise 'not implemented'
    
    def addBridgeCmd(self, *args):
        raise 'not implemented'

    def remBridgeCmd(self, *args):
        raise 'not implemented'
    
    def tcpdCmd(self, *args):
        raise 'not implemented'
    
    def tlsdCmd(self, *args):
        raise 'not implemented'
    
    def canCtldCmd(self, *args):
        raise 'not implemented'
    
    def statusCmd(self, *args):
        raise 'not implemented'
    
    def statsCmd(self, *args):
        raise 'not implemented'
    
    def nodesCmd(self, *args):
        raise 'not implemented'
    
    def simCmd(self, *args):
        raise 'not implemented'
    
    def logCmd(self, *args):
        raise 'not implemented'

    def execCmd(self, *args):
        raise 'not implemented'
    
    def parseCommand(self, cmdstr):
        incmd = cmdstr.strip()
        cmd = incmd.split(' ')
        if len(cmd[0]) < 2:
            return
        cmd[0] = cmd[0].lower()
        if cmd[0] == 'quit' or cmd[0] == 'exit':
            self.terminated = True
        elif self.command_map.has_key(cmd[0]):
            self.command_list[self.command_map[cmd[0]]][self.CMDLIST_HANDLER](cmd[1:])
        else:
            print 'unknown command: ' + cmd[0]
    
    def exitHelper(self):
        pass
#        if self.nodeIf is not None and self.nodeIf.running():
#            self.nodeIf.stop()
            
    def exceptHelper(self, type, value, tb):
        import traceback
        traceback.print_exception(type, value, tb)
        print '\nException in main program, shutting down threads'
        self.terminated = True
        self.exitHelper()


    def run(self):
        try:
            myPath = os.path.dirname(__file__)
            os.chdir(myPath)
        except:
            return
        
        print 'INIT: Start logging'
        log.basicConfig(level=log.DEBUG)
        
        """command parameters only dictate verbosity and use of config file"""
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

        """load config defaults, or last config if exists"""
        """run startup.cfg"""
        print 'INIT: Loading daemon configuration'
        self.cfg = DaemonConfig()
        self.cfg.load()

        self.ifNotifier = self.IfNotifier(self)        
        for nodeIf in self.cfg.nodeInterfaces:
            nodeIf.setIfNotifier(self.ifNotifier)
            
        atexit.register(self.exitHelper)
        sys.excepthook = self.exceptHelper

        print 'INIT: Starting configured node interfaces'
        for nodeIf in self.cfg.nodeInterfaces:
            if not nodeIf.start():
                print 'FATAL: Failed to initialize node interface.'
                print 'This may indicate that the interface is unavailable, ' \
                      'or that you do not have permission to access it.'
                sys.exit(-1)

        print 'INIT: Initialize/gather info on connected can nodes and load associated state'
        print 'INIT: Starting configured servers'
        
        print 'CanDaemon v1 ready'
        print 'Enter command or type \"help\" for a list of commands'
        while not self.terminated:
            input = raw_input('> ')
            self.parseCommand(input)

        print 'Shutdown'
        self.cfg.save()
        
        for nodeIf in self.cfg.nodeInterfaces:
            if self.nodeIf.running():
                print 'Stopping If thread'
                self.nodeIf.stop()
        
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
