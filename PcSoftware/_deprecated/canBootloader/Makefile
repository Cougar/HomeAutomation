SOURCES			= canBootloader/CanPacket.cs canBootloader/Downloader.cs canBootloader/HexFile.cs canBootloader/Program.cs canBootloader/SerialConnection.cs canBootloader/Properties/AssemblyInfo.cs
#OUT				= canBootloader.exe
PRG				= canBootloader
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

