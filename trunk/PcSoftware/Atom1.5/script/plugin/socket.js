
Socket_Connections = {};

function Socket_OnNewData(socket_id, data)
{
	if (Socket_Connections[socket_id])
	{
		Socket_Connections[socket_id]["ondata"](socket_id, data);
	}
}

function Socket_OnNewState(socket_id, state)
{
	if (Socket_Connections[socket_id])
	{
		Socket_Connections[socket_id]["onstate"](socket_id, state);
	}
}

function Socket_Connect(address, port, ondata_callback, onstate_callback)
{
	socket_id = SocketExport_Connect(address, port);
	
	if (socket_id > 0)
	{
		Socket_Connections[socket_id] = { "ondata" : ondata_callback, "onstate" : onstate_callback };
	}
	
	return socket_id;
}

function Socket_Disconnect(socket_id)
{
	SocketExport_Disconnect(socket_id);
	
	delete Socket_Connections[socket_id];
}

function Socket_Send(socket_id, data)
{
	SocketExport_Send(socket_id, data);
}
