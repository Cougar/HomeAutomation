#!/usr/bin/python

import termios
import os

import can
import uncen

class Job( uncen.UncenJob ):
    name    = "Socket serial"
    version = "0.01"

    cid     = None
    listen  = True

    buffer  = ""

    MAGIC_START = "\xfd"
    MAGIC_END   = "\xfa"
    PKT_LEN     = 15


    def getstatus( self ):
        return "%s @ %d 8N1" % (self.path, self.baudrate)

    def __init__( self, core, attr ):
        self.job_init( core )

        self.path = attr[0]
        self.baudrate = 38400

        self.fd = os.open( attr[0], os.O_RDWR )
        termios.tcsetattr( self.fd, termios.TCSANOW, [ \
            termios.IGNPAR, \
            0, \
            termios.CS8 | termios.CLOCAL | termios.CREAD, \
            0, \
            termios.B38400, \
            termios.B38400, \
            [ 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] ] )

        self.trigger_add( self.fd )

    def close( self ):
        os.close( self.fd )
        self.job_close()

    def trigger( self, fd, eventmask ):
        self.buffer += os.read( self.fd, 8192 )

        if self.cid == None:
            self.buffer = ""
            return

        while len( self.buffer ) >= self.PKT_LEN + 2:
            if self.buffer[0] == self.MAGIC_START and self.buffer[self.PKT_LEN+1] == self.MAGIC_END:
                pktdata = self.buffer[1:self.PKT_LEN+1]
                self.buffer=self.buffer[self.PKT_LEN+2:]

                datalen = ord( pktdata[6] )
                canp = can.CanPacket( \
                    ( ord( pktdata[0] ) ) | \
                    ( ord( pktdata[1] ) << 8 ) | \
                    ( ord( pktdata[2] ) << 16 ) | \
                    ( ord( pktdata[3] ) << 24 ), \
                    [ ord( val ) for val in pktdata[7:7+datalen] ], \
                    pktdata[4] != "\x00", \
                    pktdata[5] != "\x00" )

                self.can_send( self.cid, canp )

            while   len( self.buffer )          >  self.PKT_LEN + 2 \
                and self.buffer[0]              != self.MAGIC_START \
                and self.buffer[self.PKT_LEN+1] != self.MAGIC_END:
                idxstart = self.buffer.find( self.MAGIC_START )
                if idxstart < 0:
                    self.buffer = ""
                    return
                self.buffer = self.buffer[idxstart:]


    def can_recieve( self, cid, canp ):
        pktdata = self.MAGIC_START
        pktdata += chr( ( canp.id & 0x000000FF ) >>  0 )
        pktdata += chr( ( canp.id & 0x0000FF00 ) >>  8 )
        pktdata += chr( ( canp.id & 0x00FF0000 ) >> 16 )
        pktdata += chr( ( canp.id & 0xFF000000 ) >> 24 )
        pktdata += {True: "\x01", False: "\x00"}[canp.extended]
        pktdata += {True: "\x01", False: "\x00"}[canp.remote]
        pktdata += chr( len( canp.data ) )
        for val in canp.data:
            pktdata += chr( val )
        for val in range( 8-len( canp.data ) ):
            pktdata += "\x00"
        pktdata += self.MAGIC_END
        os.write( self.fd, pktdata )

    def can_accept( self, sjid, cid ):
        self.listen = False
        self.cid = cid
        return True

    def can_close( self, cid ):
        self.cid = None
        self.listen = True
