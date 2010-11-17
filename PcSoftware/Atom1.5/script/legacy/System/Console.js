
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

function ethStart()
{
	var canMessage = new CanNMTMessage("nmt", "Time");
	canMessage.send(); 
}

function resetNode(hardwareId)
{
	ProgrammingClientId = ClientId;

	if (hardwareId.length > 2)
	{
		if (hardwareId.substr(0,2)=="0x") 
		{
			hardwareId = hex2uint(hardwareId.substring(2,hardwareId.length));
		}
	}
	
	var node = CanNodes[hardwareId];

	if (!node)
	{
		//printTo(ProgrammingClientId, "STOP: Failed. No node is online that has that hardware id, " + hardwareId + "\n");
		//log("A can node with hardware id " + uint2hex(canMessage.getData("HardwareId"), 32) + " was not found, starting.\n");

		/* As a node with this hardware id has not been seen it could just be that it was started before atom and has no application installed */
		node = new CanNode(hardwareId, null);
		CanNodes[hardwareId] = node;
		//return;
	}
	
	node.reset(	function(status)
			{
				if (status)
				{
					printTo(ProgrammingClientId, "STOP: Success: Node reset and bios started okay.\n");
				}
				else
				{
					printTo(ProgrammingClientId, "STOP: Failed. Node did not respond failed.\n");
				}
			});
}

var ProgrammingClientId = null;

function programNodeCallback(status, event, text, raw)
{
	if (raw)
	{
		printTo(ProgrammingClientId, text);
	}
	else
	{
		printTo(ProgrammingClientId, event + ": " + text + "\n");
	}
}

function programNode(hardwareId, hexData, bios)
{
	ProgrammingClientId = ClientId;

	if (hardwareId.length > 2)
	{
		if (hardwareId.substr(0,2)=="0x") 
		{
			hardwareId = hex2uint(hardwareId.substring(2,hardwareId.length));
		}
	}
	
	var node = CanNodes[hardwareId];

	if (!node)
	{
		//print("STOP: Failed. No node is online that has that hardware id, " + hardwareId + "\n");
		//return;
		/* As a node with this hardware id has not been seen it could just be that it was started before atom and has no application installed */
		node = new CanNode(hardwareId, null);
		CanNodes[hardwareId] = node;
	}

	hexData = unescape(hexData);

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
			node.startProgramming(hexObj, programNodeCallback, bios);
		}
		else
		{
			print("STOP: Failed. Invalid hexfile\n");
		}
	}
	else
	{
		//print("File was not found, " + hexFilename + "\n");
	}
}

function p(time, button)
{
	var t = ServiceManager.getService("CAN", "irTransmit", 1);
	t.sendClick('KiSS_DP-1500_Remote', button, time);
}
