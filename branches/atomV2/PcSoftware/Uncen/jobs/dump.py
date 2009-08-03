#!/usr/bin/python

import uncen
import can

class Job( uncen.UncenJob ):
    name = "Dumper"
    version = "0.01"

    def __init__( self, core, args ):
        self.job_init( core )
        self.cid = self.can_connect( args[0] )

    def close( self ):
        self.job_close()

    def getstatus( self ):
        return "running"

# callbacks
    def can_recieve( self, cid, canp ):
        self.core.term.write( "Dumper #%s - %s" % (self.core.job_name(self.jid),str( canp )) )

    def can_close( self, cid ):
        self.core.term.write( "Dumper #%s - Lost conn" % (self.core.job_name(self.jid),) )
        self.close()
