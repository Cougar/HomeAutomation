SOURCES			= ../csharp-lib/Arguments.cs ../csharp-lib/CanPacket.cs ../csharp-lib/CanNMT.cs ../csharp-lib/HexFile.cs ../csharp-lib/DaemonConnection.cs ../csharp-lib/Downloader.cs Source/Program.cs Source/Properties/AssemblyInfo.cs
# Source/Downloader.cs Source/HexFile.cs 
#OUT				= canBootloader.exe
PRG				= canDude
# You should not have to change anything below here.

GMCS			= gmcs
CFLAGS			= -out:$(PRG).exe

default: $(PRG).exe
#default: 
#	$(GMCS) $(CFLAGS) $(SOURCES)
	
all: $(PRG).exe
#all: 
#	$(GMCS) $(CFLAGS) $(SOURCES)

$(PRG).exe: $(SOURCES)
	@echo ---------------------------------------------------------------------
	@echo You need mono-gmcs to compile canDude, sudo apt-get install mono-gmcs
	@echo ---------------------------------------------------------------------
	$(GMCS) $(CFLAGS) $(SOURCES)

clean:
	rm -rf $(PRG).exe

