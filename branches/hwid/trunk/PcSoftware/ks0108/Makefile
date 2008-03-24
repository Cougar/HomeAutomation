CC = gcc
CFLAGS = -O3 -Wall -ffast-math 
SRCDIR = 
INCLUDEDIR = 
OBJDIR = 

THEFILES = 	$(SRCDIR)bmp2lcd.c 
OBJFILES =	$(OBJDIR)bmp2lcd.o 

INCLUDEFILES =   

OUTFILE	= 	bmp2lcd

CODELIBS = -lpthread

all: $(OUTFILE)

$(OUTFILE) : $(OBJFILES) $(INCLUDEFILES)
	$(CC) $(CFLAGS) -o $(OUTFILE) $(OBJFILES) $(CODELIBS)
	strip $(OUTFILE)

############################################################################
####									####
############################################################################

$(OBJDIR)bmp2lcd.o :$(SRCDIR)bmp2lcd.c $(INCLUDEFILES)
	$(CC) $(CFLAGS) -c $(SRCDIR)bmp2lcd.c -o $(OBJDIR)bmp2lcd.o

clean:
	rm -f $(OUTFILE)
	rm -f $(OBJFILES)

.PHONY : clean
