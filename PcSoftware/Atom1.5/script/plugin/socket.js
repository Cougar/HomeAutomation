
var Socket_Connections = {};

var SOCKET_STATE_CONNECTED      = 0x00;
var SOCKET_STATE_DISCONNECTED   = 0x01;
var SOCKET_STATE_ACCEPTED       = 0x02;

function Socket_OnNewData(id, data)
{
  if (Socket_Connections[id])
  {
    Socket_Connections[id]["ondata"](id, data);
  }
  
  return true;
}

function Socket_OnNewClient(id, server_id)
{
  if (Socket_Connections[server_id])
  {
    Socket_Connections[id] = { "ondata" : Socket_Connections[server_id]["ondata"], "onstate" : Socket_Connections[server_id]["onstate"] };
    
    Socket_Connections[server_id]["onaccept"](id, server_id);
  }
  
  return true;
}

function Socket_OnNewState(id, state)
{
  if (Socket_Connections[id])
  {
    Socket_Connections[id]["onstate"](id, state);
    
    if (state == SOCKET_STATE_DISCONNECTED)
    {
      delete Socket_Connections[id];
    }
  }
  
  return true;
}

function Socket_StartServer(port, onaccept_callback, ondata_callback, onstate_callback)
{
  socket_id = SocketExport_StartServer(port);
  
  if (socket_id > 0)
  {
    Socket_Connections[socket_id] = { "onaccept" : onaccept_callback, "ondata" : ondata_callback, "onstate" : onstate_callback };
  }
  
  return socket_id;
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

function Socket_StopServer(socket_id)
{
  SocketExport_StopServer(socket_id);
  
  delete Socket_Connections[socket_id];
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
