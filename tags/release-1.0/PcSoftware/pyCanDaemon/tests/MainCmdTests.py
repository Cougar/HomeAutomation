import logging as log
log.basicConfig(level=log.DEBUG,
                format='%(asctime)s %(levelname)s %(message)s',
                filename=__name__ + '.log',
                filemode='w')
import unittest
import sys
import os

os.chdir(os.getcwd()[0:-6]) # ouch that hurts!
STDOUT_REDIRECT_FILE = 'tests/' + __name__ + '2.log'

if os.path.exists(STDOUT_REDIRECT_FILE):
    os.remove(STDOUT_REDIRECT_FILE)

#--------------------------------------------------------------------------

from Main import CanDaemon
canDaemon = CanDaemon()

class MainCmdTests(unittest.TestCase):
    
    def setUp(self):
        """Call before every test case."""
        sys.stdout = open(STDOUT_REDIRECT_FILE, 'a')

    def tearDown(self):
        """Call after every test case."""
        sys.stdout = sys.__stdout__ 
    
    def testA_CmdBasic(self):
        log.debug('testCmdBasic')
        canDaemon.parseCommand('hello')
        
    def testB_CmdHelp(self):
        log.debug('testCmdBasic')
        canDaemon.parseCommand('help')
        canDaemon.parseCommand('help log')
        canDaemon.parseCommand('help filter')
        canDaemon.parseCommand('help state')
        canDaemon.parseCommand('help addif')
        canDaemon.parseCommand('help addbridge')
        canDaemon.parseCommand('help tcpd')
        canDaemon.parseCommand('help tlsd')
        canDaemon.parseCommand('help canctld')
        canDaemon.parseCommand('help sim')

    def testC_ImportFilter(self):
        log.debug('testImportFilter')
        canDaemon.parseCommand('filter add DefaultFilter')
        
    def testD_RemoveFilter(self):
        log.debug('testRemoveFilter')
        canDaemon.parseCommand('filter rem DefaultFilter')

    def testE_ImportSpace(self):
        log.debug('testImportSpace')
        canDaemon.parseCommand('state add DefaultStateSpace')
        
    def testF_RemoveSpace(self):
        log.debug('testRemoveSpace')
        canDaemon.parseCommand('state rem DefaultStateSpace')
        
    def testG_AddCanStimIf(self):
        log.debug('testAddCanStimIf')
        canDaemon.parseCommand('addif stim0 stim')
    
    def testH_RemCanStimIf(self):
        log.debug('testRemCanStimIf')
        canDaemon.parseCommand('remif stim0')
        
    def testI_AddUDPIf(self):
        log.debug('testAddUDPIf')
        canDaemon.parseCommand('addif udp0 udp host=192.168.10.2 port=1000')
    
    def testJ_RemUDPIf(self):
        log.debug('testRemUDPIf')
        canDaemon.parseCommand('remif udp0')

    def testK_AddTCPIf(self):
        log.debug('testAddTCPIf')
        canDaemon.parseCommand('addif telnet0 host=192.168.10.2 port=1000')
    
    def testL_RemTCPIf(self):
        log.debug('testRemTCPIf')
        canDaemon.parseCommand('remif telnet0')
        
    def testM_AddTcpTlsIf(self):
        log.debug('testAddTcpTlsIf')
        canDaemon.parseCommand('addif tcptls0 tcptls host=192.168.10.2 port=1000 key=mykey.pub')
            
    def testN_RemTcpTlsIf(self):
        log.debug('testRemTcpTlsIf')
        canDaemon.parseCommand('remif tcptls0')

    