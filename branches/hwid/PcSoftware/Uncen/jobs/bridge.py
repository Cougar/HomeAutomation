#!/usr/bin/python

import uncen
import can

class Job( uncen.UncenJob ):
    name = "Can Bridge"
    version = "0.01"
    listen = True

    def __init__( self, core, args ):
        self.job_init( core )

    def close( self ):
        self.job_close()

    def getstatus( self ):
        return "running"

    def termcmd( self, args ):
        cmd = args.pop(0)
        if cmd == "add":
            try:
                jid = args[0]
                self.can_connect( jid )
            except:
                self.core.term.write( "Error connecting" )
        elif cmd == "list":
            self.core.term.write( "Job      - Job name" )
            for cid in self.can_connections:
                jid = self.can_connections[cid]
                job = self.core.job_get( jid )
                self.core.term.write( "%8s - %s" % (self.core.job_name(jid), job.name) )

# callbacks
    def can_recieve( self, scid, canp ):
        cids = self.can_connections.keys()
        for cid in cids:
            if scid != cid:
                self.can_send(cid, canp)

    def can_accept( self, sjid, cid ):
        return True
