#!/usr/bin/python

class ParseError:
    pass

class MissingArgument:
    pass

def parse( filename, cmdpars ):
    transl = {}
    lines = []
    f = open( filename )
    for line in f:
        line = line.rstrip("\n\r")
        if line == "":
            pass
        elif line[0] == "-":
            arg, name, default, desc = line[1:].split(" ",3)
            if default == "-" and not cmdpars.has_key( arg ):
                raise MissingArgument
            transl[ name ] = cmdpars.get(arg,default)
        elif line[0] == ">":
            l = line[1:]
            for key,val in transl.items():
                l = l.replace( ("{%s}"%(key,)), val )
            lines.append( l )
    f.close()
    return lines
