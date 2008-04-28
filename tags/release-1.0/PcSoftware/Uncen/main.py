#!/usr/bin/python

import uncen
import terminal
import sys

import macro

def handleterm( core, term, fd, event ):
    lines = term.lines()
    for line in lines:
        doline( core, term, line )
    term.prompt("> ")

def doline( core, term, line ):
    args = line.split(" ")
    cmd = args.pop(0)
    if cmd == "":
        pass
    elif cmd[0] == ".":
        try:
            jid = cmd[1:]
            job = core.job_get( jid )
            job.termcmd( args )
        except:
            term.write( "Error" )
    elif cmd == "run":
        try:
            jobname = args.pop(0)
            if jobname[0] == '.':
                name = jobname[1:]
                jobname = args.pop(0)
            else:
                name = None
            jobclass = getajob( jobname )
            if jobclass == None:
                term.write( "Error loading \"%s\"" % (jobname,) )
            else:
                try:
                    job = jobclass( core, args )
                    if name != None:
                        core.job_setname( job.jid, name )
                    term.write( "Started job #%s" % (core.job_name(job.jid),) )
                except:
                    term.write( "Error starting \"%s\"" % (jobname,) )
        except:
            term.write( "Error loading" )
    elif cmd == "kill":
        core.job_close( args[0] )
    elif cmd == "list":
        jids = core.job_list()
        term.write( "Active Jobs:" )
        for jid in jids:
            job = core.job_get( jid )
            term.write( "%s%8s - %s (v. %s) - %s" % ({True:"L", False:" "}[job.listen], core.job_name(job.jid), job.name, job.version, job.getstatus()) )
    elif cmd == "lastjob":
        core.set_lastjob( args[0] )
    else:
        term.write( "Illegal command: " + cmd )

def getajob( name ):
    filename = (apppath + "jobs/%s.py") % (name,)
    g = {}
    try:
        execfile( filename, g )
        return g['Job']
    except IOError:
        return None

def getmacro( name, cmdpars ):
    filename = (apppath + "macros/%s.macro") % (name,)
    return macro.parse( filename, cmdpars )

apppath = "/home/pengi/devel/can/Uncen/" #FIXME!!!

args = sys.argv[1:]

cmdpars = {}
try:
    while len(args):
        arg = args.pop(0)
        if arg[0] == "-":
            name = arg[1:]
            val = args.pop(0)
            cmdpars[name] = val
except IndexError:
    pass

if cmdpars.has_key('m'):
    macrofile = cmdpars.pop('m')
    init = getmacro( macrofile, cmdpars )
else:
    init = []

core = uncen.UncenCore()

term = terminal.TerminalStd()
core.trigger_register( term.fileno_in(), lambda fd, event: handleterm( core, term, fd, event ) )
core.term=term

for line in init:
    term.write( "> "+line )
    doline( core, term, line )

term.prompt("> ")

running = True
while running:
    try:
        core.do_poll(100)
    except KeyboardInterrupt:
        term.write( "Keyboard interrupt, exit" )
        running = False
    except SystemExit:
        term.write( "System exit" )
        running = False

core.close()
term.close()
