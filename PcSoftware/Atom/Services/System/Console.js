
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
		row[row.length] = ServiceManager.Services[n].isOnline();
		
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
