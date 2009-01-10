
function quit()
{
	if (ClientId)
	{
		_quit(ClientId);
	}
}

function print(text)
{
	if (ClientId)
	{
		_print(ClientId, text);
	}
	else
	{
		log(text);
	}
}

function printTo(clientId, text)
{
	_print(clientId, text);
}

function services()
{
	var list = new Array();
	
	var row = new Array();
	
	row[row.length] = "Type";
	row[row.length] = "Name";
	row[row.length] = "Id";
	row[row.length] = "Online";
	
	list[list.length] = row;
	
	for (var n in ServiceManager.Services)
	{
		var row = new Array();
		
		row[row.length] = ServiceManager.Services[n].getType();
		row[row.length] = ServiceManager.Services[n].getName();
		row[row.length] = ServiceManager.Services[n].getId();
		if (ServiceManager.Services[n].getType() == "Can")
		{
			row[row.length] = ServiceManager.Services[n].isOnline();
		}
		else
		{
			row[row.length] = "";
		}
		
		list[list.length] = row;
	}

	var lines = TextTable.create(list);
	
	for (var n in lines)
	{
		print(lines[n] + "\n");
	}
}

function getService(id, name)
{
	var fullId = name + id;

	if (!ServiceManager.Services[fullId])
	{
		print("No such service found.\n");
	}
	
	return ServiceManager.Services[fullId];
}

var ProgrammingClientId = null;

function programNodeCallback(status, event, text)
{
	printTo(ProgrammingClientId, event + ": " + text + "\n");
}

function testCRC()
{
	
}

function programNode(hardwareId, hexData, bios)
{
	ProgrammingClientId = ClientId;

	if (hardwareId.length > 2)
	{
		if (hardwareId.substr(0,2)=="0x") 
		{
			hardwareId = hex2uint(hardwareId.substring(2,hardwareId.length));
			//hardwareId += hex2uint(hardwareId.substring(0,hardwareId.length-1));
		}
	}
	
	var node = CanNodes[hardwareId];

	if (!node)
	{
		print("Failed. No node is online that has that hardware id, " + hardwareId + "\n");
		return;
	}

	hexData = unescape(hexData);

	//print("helo, hexData: " + hexData);

	//var hexData = getFileContents(hexFilename);
	if (hexData)
	{
		var hexLinesOrg = hexData.split('\n');
		var hexLines = new Array();
	
		for (var n = 0; n < hexLinesOrg.length; n++)
		{
			var line = hexLinesOrg[n].trim(' ');
		
			if (line != "")
			{
				hexLines[hexLines.length] = line;
			}
		}

		var hexObj = new IntelHex(hexLines);
		if (hexObj.isValid()) {
			//print("hexfile is valid");
			//for (var i = 0; i < 16; i++) { print(hexObj.getByte(i)+" "); }
			//print("len: "+hexObj.getLength()+" laddr: "+hexObj.getAddrLower()+" uaddr: "+hexObj.getAddrUpper()+"\n");
			node.startProgramming(hexObj, programNodeCallback, bios);
		}
	}
	else
	{
		//print("File was not found, " + hexFilename + "\n");
	}
}
