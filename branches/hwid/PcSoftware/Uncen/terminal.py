#!/usr/bin/python

import sys
import os
#import curses

class Terminal:
    def __init__( self ):
        self.buffer = ""
        self.init()

    def init( self ):
        pass

    def lines( self ):
        lines = (self.buffer + self.read()).split("\n")
        self.buffer = lines[-1]
        return lines[:-1]

    def read( self ):
        return ""

    def write( self, text, attr=0 ):
        pass

    def prompt( self, text ):
        pass

    def fileno_in( self ):
        return 0
    def fileno_out( self ):
        return 1

    def close( self ):
        pass


class TerminalStd( Terminal ):
    def init( self ):
        self.stdin = sys.stdin.fileno()
        self.stdout = sys.stdout.fileno()

    def read( self ):
        return os.read( self.stdin, 8192 )

    def write( self, text, attr=0 ):
        os.write( self.stdout, "%s\n" % (text,) )

    def prompt( self, text ):
        os.write( self.stdout, text )

    def fileno_in( self ):
        return self.stdin

    def fileno_out( self ):
        return self.stdout

#class TerminalCurses( Terminal ):
#    def init( self ):
#        self.stdin = 0
#        self.stdout = 1
#
#        self.linebuffer = [""]
#
#        self.stdscr = curses.initscr()
#        self.height, self.width = self.stdscr.getmaxyx()
#        self.input = self.stdscr.subwin( self.height-1, 0 )
#        self.output = self.stdscr.subwin( self.height-1, self.width, 0, 0 )
#
#    def read( self ):
#        return self.input.getstr()
#
#    def write( self, text, attr=0 ):
#        self.linebuffer += [text]
#        y=0
#        lines = self.linebuffer[-self.height:]
#        for line in lines:
#            self.output.addstr( y, 0, line )
#            y+=1
#        self.output.refresh()
#
#    def fileno_io( self ):
#        return self.stdin
#
#    def fileno_out( self ):
#        return self.stdout
#
#    def close( self ):
#        self.stdscr.keypad(0)
#        curses.nocbreak()
#        curses.echo()
#        curses.endwin()
#
#    def prompt( self, text ):
#        pass
