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
        canDaemon.helpCmd()

    def testImportFilter(self):
        log.debug('testImportFilter')
        canDaemon.filterCmd('DefaultFilter')

    def testImportSpace(self):
        log.debug('testImportSpace')
        canDaemon.stateCmd('DefaultStateSpace')

