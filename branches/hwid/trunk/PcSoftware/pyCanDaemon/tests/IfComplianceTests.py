import logging as log
log.basicConfig(level=log.DEBUG,
                format='%(asctime)s %(levelname)s %(message)s',
                filename=__name__ + '.log',
                filemode='w')
import unittest

class IfComplianceTests(unittest.TestCase):

    def setUp(self):
        """Call before every test case."""
        pass
        
    def tearDown(self):
        """Call after every test case."""
        pass

    def testA(self):
        """Test compliance with NodeIfBase
        Members should be:
        DEFAULT_CONFIG
        __init(self, pktHandler, ownerNotifier, optional args)
        setPktHandler(pktHandler) : None
        start() : True or False
        stop() : None
        running() : True or False if interface is started
        """
        pass
    
