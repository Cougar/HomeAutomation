import logging as log
log.basicConfig(level=log.DEBUG,
                format='%(asctime)s %(levelname)s %(message)s',
                filename=__name__ + '.log',
                filemode='w')
import unittest

#--------------------------------------------------------------------------

from CanPkt import CanPkt

class CanPktTests(unittest.TestCase):

    def setUp(self):
        """Call before every test case."""
        pass
        
    def tearDown(self):
        """Call after every test case."""
        pass
    
    def testA_CanPktBasic1(self):
        log.debug('testCanPktBasic1')
        pkt = None
        pktList = []
        for i in range(0,256):
            pkt = CanPkt(i, [0xff, 0x01, 0x3c])
            pktList.append(pkt)
            pktstr = pkt.toString()
            log.debug('pkt ' + str(i) + ' str: ' + pktstr)
            assert pkt.nodeId == i

    def testB_CanPktBasic2(self):
        log.debug('testCanPktBasic2')
        pkt = CanPkt(0xffaaaa, [0x1b, 0xff])
        str = pkt.toString()
        log.debug('pkt str: ' + str)
        assert str == 'PKT ffaaaa 1 0 27 255'
        log.debug(pkt.nodeId)
        assert pkt.nodeId == 0xffaaaa
        log.debug(pkt.extended)
        assert pkt.extended == True
        log.debug(pkt.remote)
        assert pkt.remote == False
        log.debug(pkt.data)
        log.debug(pkt.data[0])
        assert pkt.data[0] == 0x1b
        log.debug(pkt.data[1])
        assert pkt.data[1] == 0xff
        return True
    
    def testC_CanPktBasic3(self):
        log.debug('testCanPktBasic3')
        pktstr = CanPkt(0x1f8f0100,[0x5f,0x3a,0x40,0x01]).toString()
        log.debug(pktstr)
        pkt = CanPkt.stringToCanPkt(self, pktstr)
        log.debug(pkt.nodeId)
        assert pkt.nodeId == 0x1f8f0100
        log.debug(pkt.data[1])
        assert pkt.data[3] == 0x01
    
    def testC_PktHandler1_1(self):
        log.debug('testPktHandler1_1')
        pktstr = 'PKT 1f8f0100 1 0 43 01'
        pkt = CanPkt.stringToCanPkt(self, pktstr)
        log.debug(pkt.nodeId)
        assert pkt.nodeId == 0x1f8f0100
        log.debug(pkt.extended)
        assert pkt.extended == True
        log.debug(pkt.remote)
        assert pkt.remote == False
        log.debug(pkt.data)
        log.debug(pkt.data[0])
        assert pkt.data[0] == 0x2b
        log.debug(pkt.data[1])
        assert pkt.data[1] == 0x01
        
if __name__ == "__main__":
    unittest.main() # run all tests
