#!/usr/bin/python

import string
import socket

import can
import uncen

class Job( uncen.UncenJob ):
    name    = "Socket tcp/text"
    version = "0.03"

    cid     = None
    listen  = True

    buffer  = ""


    def getstatus( self ):
        return "%s:%d" % (self.peername[0], self.peername[1])

    def __init__( self, core, attr ):
        self.job_init( core )

        host = (str( attr[0] ), int( attr[1] ) )

        self.stream = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
        self.stream.connect( host )

        self.peername = self.stream.getpeername()

        self.trigger_add( self.stream.fileno() )

    def close( self ):
        self.stream.close()
        self.job_close()

    def trigger( self, fd, eventmask ):
        data = self.buffer + self.stream.recv( 8192 )
        lines = data.split("\n")
        self.buffer = lines.pop()

        if self.cid == None:
            return

        for line in lines:
            canp = self.decode( line )
            self.can_send( self.cid, canp )


    def can_recieve( self, cid, canp ):
        line = self.encode( canp )
        self.stream.send( line + "\n" )

    def can_accept( self, sjid, cid ):
        self.listen = False
        self.cid = cid
        return True

    def can_close( self, cid ):
        self.cid = None
        self.listen = True

    def decode( self, line ):
        words = line.split(" ")
        if words[0] == "PKT":
            return can.CanPacket(   int( words[1], 16 ), \
                                [ int( val, 16 ) for val in words[4:] ], \
                                words[2]!="0", \
                                words[3]!="0" )
        return None

    def encode( self, canp ):
        return ("PKT %08x %d %d" % (canp.id, canp.extended, canp.remote) ) + \
            string.join( [ " %02x"%val for val in canp.data ], "" )
