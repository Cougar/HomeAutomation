#!/usr/bin/python

import string

CAN_NMT             = 0x00

CAN_NMT_RESET       = 0x04
CAN_NMT_BIOS_START  = 0x08
CAN_NMT_PGM_START   = 0x0C
CAN_NMT_PGM_DATA    = 0x10
CAN_NMT_PGM_END     = 0x14
CAN_NMT_PGM_COPY    = 0x18
CAN_NMT_PGM_ACK     = 0x1C
CAN_NMT_PGM_NACK    = 0x20
CAN_NMT_START_APP   = 0x24
CAN_NMT_APP_START   = 0x28
CAN_NMT_HEARTBEAT   = 0x2C

def nmt_id( type, nodeid, snodeid=0 ):
    return (CAN_NMT<<24) | (type<<16) | (snodeid<<8) | (nodeid)

class CanPacket:
    """Can packet stortage

    """
    def __init__( self, id, data, extended = True, remote = False ):
        if extended:
            self.id = id & 0x1FFFFFFF
        else:
            self.id = id & 0x1FF
        self.data = [val & 0xFF for val in data[:8]]
        self.extended = extended
        self.remote = remote

    def __repr__( self ):
        return "CanPacket( " + hex(self.id) + \
                        ", [" + string.join( [hex(val) for val in self.data], ", " ) + "]" + \
                        ", " + str(self.extended) + \
                        ", " + str(self.remote) + \
                        ")"
