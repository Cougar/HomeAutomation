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
    
    def testCmdBasic(self):
        log.debug('testCmdBasic')
        canDaemon.parseCommand('hello')
        
    def testCmdHelp(self):
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

    def testImportFilter(self):
        log.debug('testImportFilter')
        canDaemon.parseCommand('filter add DefaultFilter')
        
    def testRemoveFilter(self):
        log.debug('testRemoveFilter')
        canDaemon.parseCommand('filter rem DefaultFilter')

    def testImportSpace(self):
        log.debug('testImportSpace')
        canDaemon.parseCommand('state add DefaultStateSpace')
        
    def testRemoveSpace(self):
        log.debug('testRemoveSpace')
        canDaemon.parseCommand('state rem DefaultStateSpace')
        
    def testAddCanStimIf(self):
        log.debug('testAddCanStimIf')
        canDaemon.parseCommand('addif stim0 stim')
    
    def testRemCanStimIf(self):
        log.debug('testRemCanStimIf')
        canDaemon.parseCommand('remif stim0')
        
    def testAddUDPIf(self):
        log.debug('testAddUDPIf')
        canDaemon.parseCommand('addif udp0 udp host=192.168.10.2 port=1000')
    
    def testRemUDPIf(self):
        log.debug('testRemUDPIf')
        canDaemon.parseCommand('remif udp0')

    def testAddTelnetIf(self):
        log.debug('testAddTelnetIf')
        canDaemon.parseCommand('addif telnet0 host=192.168.10.2 port=1000')
    
    def testRemTelnetIf(self):
        log.debug('testRemTelnetIf')
        canDaemon.parseCommand('remif telnet0')
        
    def testAddTcpTlsIf(self):
        log.debug('testAddTcpTlsIf')
        canDaemon.parseCommand('addif tcptls0 tcptls host=192.168.10.2 port=1000 key=mykey.pub')
            
    def testRemTcpTlsIf(self):
        log.debug('testRemTcpTlsIf')
        canDaemon.parseCommand('remif tcptls0')

    