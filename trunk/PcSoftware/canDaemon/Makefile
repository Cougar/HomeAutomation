SOURCES			= Source/Daemon.cs Source/TcpServer.cs Source/CanPacket.cs Source/Program.cs Source/SerialConnection.cs Source/Properties/AssemblyInfo.cs
# Source/Downloader.cs Source/HexFile.cs 
#OUT				= canBootloader.exe
PRG				= canDaemon
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
	$(GMCS) $(CFLAGS) $(SOURCES)

clean:
	rm -rf $(PRG).exe

