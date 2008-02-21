###########################################################
#
# This class represents a can packet which should be
# identifiable by its type and contents
#
###########################################################

class CanPkt:
    
    STANDARD_ID_MASK = 0x1FF
    EXTENDED_ID_MASK = 0x1FFFFFFF
    DATA_MASK = 0xFF
    DATA_LENGTH = 8
    
    nodeId = 0
    extended = True
    remote = False
    data = []

    def __init__( self, id, data, extended = True, remote = False ):
        if extended:
            self.nodeId = id & self.EXTENDED_ID_MASK
        else:
            self.nodeId = id & self.STANDARD_ID_MASK
        self.data = [val & self.DATA_MASK for val in data[:self.DATA_LENGTH]]
        self.extended = extended
        self.remote = remote

    def toString(self):
        strpkt = 'PKT ' + hex(self.nodeId)[2:] # '0x' is cut from can2serial link
        if self.extended:
            strpkt += ' 1'
        else:
            strpkt += ' 0'
        if self.remote:
            strpkt += ' 1'
        else:
            strpkt += ' 0'
        bytestrings = ''
        for byte in self.data:
            bytestrings = bytestrings + ' ' + str(byte)
        return (strpkt + bytestrings)
    
    @staticmethod
    def stringToCanPkt(self, str):
        """ Converts can message string to CanPkt class """
        strbyteslist = str.split(' ')
        nodeid = int('0x'+strbyteslist[1], 16)
        intvalues = [int(val, 10) for val in strbyteslist[2:]]
        extended = (intvalues[0] == 1)
        remote = (intvalues[1] == 1)
        pkt = CanPkt(nodeid, intvalues[2:], extended, remote)
        return pkt

    