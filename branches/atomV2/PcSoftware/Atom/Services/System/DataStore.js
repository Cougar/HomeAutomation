
function DataStore()
{
}

DataStore.myStores = new Array();

DataStore.getStore = function(name)
{
	if (!DataStore.myStores[name])
	{
		loadDataStore(name);
	}
	
	return DataStore.myStores[name];
}

DataStore.addStore = function(name, data)
{
	DataStore.myStores[name] = data;
}
