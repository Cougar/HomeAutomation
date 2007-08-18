#!/usr/bin/python

import select
import can

class UncenJob:
    version = "unknown"
    name = "unknown"
    listen = False

    job_name = None

# load/unload
    def __init__( self, core, attr ):
        self.job_init( core )

    def close( self ):
        self.job_close()

# handlers
    def getstatus( self ):
        return "..."

    def termcmd( self, args ):
        self.core.term.write( "Commands not accepted" )

# callbacks
    def trigger( self, fd, eventmask ):
        pass

    def can_recieve( self, cid, canp ):
        pass

    def can_accept( self, sjid, cid ):
        return False

    def can_close( self, cid ):
        pass

# helpers
    def can_accept_raw( self, sjid, cid ):
        if not self.can_accept( sjid, cid ):
            return False
        self.can_connections[cid] = sjid
        return True

    def can_close_raw( self, cid ):
        self.can_close( cid )
        self.can_connections.pop( cid, None )

    def can_connect( self, djid ):
        djid = self.core.job_id( djid )
        cid = self.core.can_connect( self.jid, djid )
        if cid == None:
            return None
        self.can_connections[cid] = djid
        return cid

    def can_disconnect( self, cid ):
        del self.can_connections[cid]
        return self.core.can_disconnect( self.jid, cid )

    def can_send( self, cid, canp ):
        if not self.core.can_send( self.jid, cid, canp ):
            self.can_close_raw( cid )

    def trigger_add( self, fd, eventmask = select.POLLIN | select.POLLPRI | select.POLLERR ):
        self.trigger_fds.append( fd )
        self.core.trigger_register( fd, self.trigger, eventmask )

    def trigger_del( self, fd ):
        self.trigger_fds.remove( fd )
        self.core.trigger_unregister( fd )

    def job_init( self, core ):
        self.core = core
        self.jid = self.core.job_register( self )
        self.trigger_fds = []
        self.can_connections = {}

    def job_close( self ):
        for fd in self.trigger_fds:
            self.core.trigger_unregister( fd )
        cids = self.can_connections.keys()
        for cid in cids:
            self.can_disconnect( cid )
        self.core.job_unregister( self.jid )

class UncenCore:
    def __init__( self ):
        self.jobs = {}

        self.connections = {}

        self.triggers = {}
        self.poll = select.poll()
        self.nextjid = 1
        self.nextcid = 1

        self.term = None

        self.job_names = {}

        self.lastjob = 0

    def close( self ):
        jids = self.jobs.keys()
        for jid in jids:
            self.jobs[jid].close()

# Jobs
    def job_register( self, job ):
        jid = self.nextjid
        self.jobs[jid] = job
        self.nextjid += 1
        return jid

    def job_unregister( self, jid ):
        jid = self.job_id( jid )
        job = self.jobs.pop( jid, None )
        if job is not None:
            if job.job_name is not None:
                del self.job_names[job.job_name]
        if self.lastjob == jid:
            raise SystemExit()
        return job

    def job_list( self ):
        return self.jobs.keys()

    def job_get( self, jid ):
        jid = self.job_id( jid )
        return self.jobs[jid]

    def job_close( self, jid ):
        jid = self.job_id( jid )
        if self.jobs.has_key( jid ):
            self.jobs[jid].close()

    def job_id( self, jname ):
        s = str( jname )
        if s.isdigit():
            return int( s )
        if self.job_names.has_key( s ):
            return self.job_names[s]
        return None

    def job_name( self, jid ):
        jid = self.job_id( jid )
        for name, id in self.job_names.items():
            if jid == id:
                return name
        return str( jid )

    def job_setname( self, jid, jname ):
        jid = self.job_id( jid )
        if self.job_names.has_key( jname ):
            oldjid = self.job_names[jname]
            if self.jobs.has_key( oldjid ):
                self.jobs[oldjid].job_name = None
        if self.jobs.has_key( jid ):
            self.job_names[jname] = jid
            self.jobs[jid].job_name = jname


# Triggers
    def trigger_register( self, fd, callback, eventmask = select.POLLIN | select.POLLPRI | select.POLLERR ):
        self.triggers[fd] = callback
        self.poll.register( fd, eventmask )

    def trigger_unregister( self, fd ):
        self.triggers.pop(fd,None)
        self.poll.unregister( fd )

    def can_connect( self, sjid, djid ):
        sjid = self.job_id( sjid )
        djid = self.job_id( djid )
        if self.jobs.has_key( djid ):
            job = self.jobs[djid]
            if job.listen:
                cid = self.nextcid
                self.nextcid += 1
                self.connections[cid] = (sjid, djid)
                if job.can_accept_raw( sjid, cid ):
                    return cid
                else:
                    del self.connections[cid]
                    return None
        return None

    def can_disconnect( self, sjid, cid ):
        sjid = self.job_id( sjid )
        if self.connections.has_key(cid):
            jida, jidb = self.connections[cid]
            if jida == sjid:
                djid = jidb
            elif jidb == sjid:
                djid = jida
            else:
                return False
            if self.jobs.has_key( djid ):
                djob = self.jobs[ djid ]
                djob.can_close_raw( cid )
                self.connections.pop(cid, None)
                return True
            else:
                del self.connections[cid]
                return False
        else:
            return False

    def can_send( self, sjid, cid, canp ):
        sjid = self.job_id( sjid )
        if self.connections.has_key(cid):
            jida, jidb = self.connections[cid]
            if jida == sjid:
                djid = jidb
            elif jidb == sjid:
                djid = jida
            else:
                return False
            if self.jobs.has_key( djid ):
                djob = self.jobs[ djid ]
                djob.can_recieve( cid, canp )
                return True
            else:
                del self.connections[cid]
                return False
        else:
            return False

    def set_lastjob( self, jid ):
        jid = self.job_id( jid )
        if jid==None:
            raise SystemExit()
        if not self.jobs.has_key( jid ):
            raise SystemExit()
        self.lastjob = jid

# Main loop
    def do_poll( self, timeout=0 ):
        events = self.poll.poll( timeout )
        for fd, event in events:
            self.triggers[fd]( fd, events )
