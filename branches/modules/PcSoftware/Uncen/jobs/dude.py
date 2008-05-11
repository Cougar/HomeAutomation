#!/usr/bin/python

import can
import uncen

import sys
from array import array
from binascii import unhexlify

class Job( uncen.UncenJob ):
    name = "UncenDude"
    version = "0.01"
    listen = False

    mynodeid = 0

    cnt = 0

    programming = False

    MODE_APP   = 0
    MODE_BIOS  = 1
    MODE_START = 2
    MODE_RESET = 3

    def getstatus( self ):
        return "Pkt sent: %d" % (self.cnt,)

    def __init__( self, core, args ):
        self.job_init( core )


        self.cid = self.can_connect( args[0] )

        if len(args[1])>2 and args[1][:2] == "0x":
            self.nodeid = int( args[1], 16 )
        else:
            self.nodeid = int( args[1] )

        self.mode = { \
            "app":   self.MODE_APP,   \
            "bios":  self.MODE_BIOS,  \
            "start": self.MODE_START, \
            "reset": self.MODE_RESET, \
            }[args[2]]

        if self.mode == self.MODE_APP or self.mode == self.MODE_BIOS:
            self.hexfile = Hexfile( args[3] )
            self.finished = False
            self.programming = True
            self.dosend_file()
        elif self.mode == self.MODE_RESET:
            self.finished = True
            canp = can.CanPacket( can.nmt_id( can.CAN_NMT_RESET, self.nodeid, self.mynodeid ), [] )
            self.can_send( self.cid, canp )
        elif self.mode == self.MODE_START:
            self.finished = True
            canp = can.CanPacket( can.nmt_id( can.CAN_NMT_START_APP, self.nodeid, self.mynodeid ), [] )
            self.can_send( self.cid, canp )
            self.close()

    def dosend_file( self ):
        if self.programming:
            rec = self.hexfile.pop( 6 )

            if rec[0] == Hexfile.TYPE_DATA:
                type, offset, data = rec
                self.cnt += 1
                canp = can.CanPacket( can.nmt_id( can.CAN_NMT_PGM_DATA, self.nodeid, self.mynodeid ), [(offset) & 0xFF, (offset>>8) & 0xFF] + data )
                self.can_send( self.cid, canp )
            elif rec[0] == Hexfile.TYPE_OFFSET:
                type, offset = rec
                self.cnt += 1
                canp = can.CanPacket( can.nmt_id( can.CAN_NMT_PGM_START, self.nodeid, self.mynodeid ), [ (offset>>24) & 0xFF, (offset>>16) & 0xFF, (offset>>8 ) & 0xFF, (offset    ) & 0xFF ] )
                self.can_send( self.cid, canp )
            elif rec[0] == Hexfile.TYPE_EOF:
                self.finished = True
                canp = can.CanPacket( can.nmt_id( can.CAN_NMT_PGM_END, self.nodeid, self.mynodeid ), [] )
                self.programming=False
                self.can_send( self.cid, canp )
        else:
            canp = can.CanPacket( can.nmt_id( can.CAN_NMT_RESET, self.nodeid, self.mynodeid ), [] )
            self.can_send( self.cid, canp )


    def can_recieve( self, cid, canp ):
        if canp.id == can.nmt_id( can.CAN_NMT_PGM_ACK, self.mynodeid, self.nodeid ):
            self.dosend_file()
        elif canp.id == can.nmt_id( can.CAN_NMT_PGM_NACK, self.mynodeid, self.nodeid ):
            self.core.term.write( "Error programming" )
        elif canp.id == can.nmt_id( can.CAN_NMT_BIOS_START, self.mynodeid, self.nodeid ):
            self.core.term.write( "Node resetted, bios 0x%02x%02x, application: %d" % (canp.data[1], canp.data[0], canp.data[2]) )
            if not self.finished:
                self.core.term.write( "WARNING: Programming not finished" )
            self.close()

    def close( self ):
        self.job_close()

class Hexfile:
    TYPE_DATA = 0
    TYPE_EOF = 1
    TYPE_OFFSET = 2

    def __init__( self, filename ):
        self.file = file( filename, "r" )
        self.records = [(self.TYPE_OFFSET, 0)]
        for line in self.file:
            line = line.rstrip("\n\r")
            try:
                self.records.append( self.parse( line ) )
            except IllegalRecord:
                pass
        self.file.close()

    def parse( self, line ):
        if line == "":
            raise IllegalRecord()

        if line[0] != ":":
            raise IllegalRecord()

        data = array('B', unhexlify( line[1:] ) )
        if len( data ) < 5:
            raise IllegalRecord()

        crc = 0
        for i in range( len( data )-1 ):
            crc += data[i]
        crc = (1+(0xFF^crc))&0xFF

        if data[-1] != crc:
            raise IllegalRecord()

        length = data[0]
        addr = data[1]*256 + data[2]
        type = data[3]
        content = data[4:-1].tolist()

        val = 0
        for x in content:
            val = (val<<8) | x

        if len( content ) != length:
            raise IllegalRecord()

        if type==0x00:
            return (self.TYPE_DATA, addr, content)

        if type==0x01:
            return (self.TYPE_EOF,)

        if type==0x02:
            return (self.TYPE_OFFSET, contentint*0x10)

        if type==0x04:
            return (self.TYPE_OFFSET, contentint*0x10000)

    def pop( self, maxlength=None ):
        try:
            rec = self.records.pop(0)
        except IndexError:
            rec = (self.TYPE_EOF,)
        if rec[0] != self.TYPE_DATA or maxlength is None:
            return rec

        type, addr, content = rec
        length = len( content )

        if length > maxlength:
            newaddr = addr+maxlength
            newcontent = content[maxlength:]
            content = content[:maxlength]

            self.records.insert(0, (type, newaddr, newcontent ) )
            return (type, addr, content)
        elif length < maxlength:
            if self.records[0][0] == type and self.records[0][1] == addr+length:
                nextrec = self.pop( maxlength-length )
                content = content+nextrec[2]
                return (type, addr, content)
            return rec
        else:
            return rec

class IllegalRecord:
    pass
