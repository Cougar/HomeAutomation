
var Xbmc_Socket_Id = null;
var Xbmc_Susbscribers = [];
var Xbmc_Sessions = {};

function Xbmc_SendCommand(command_name, params, callback)
{
  var new_id = 1;
 
  if (!Xbmc_IsConnected())
  {
    return false;
  }
  
  while (Xbmc_Sessions[new_id])
  {
    new_id++;
  }

  Xbmc_Sessions[new_id] = callback;
  
  var command = { "jsonrpc" : "2.0", "method": command_name, "params" : params, "id" : new_id };
  
  Socket_Send(Xbmc_Socket_Id, JSON.stringify(command));
  
  return new_id;
}

function Xbmc_SubscribeToConnectionStatus(callback)
{
  Xbmc_Susbscribers.push(callback);
}

function Xbmc_IsConnected()
{
  return Xbmc_Socket_Id != null;
}

function Xbmc_Connect(host, port)
{
  if (Xbmc_IsConnected())
  {
    Xbmc_Disconnect();
  }
  
  Log("Connecting to XBMC at " + host + ":" + port + "...");
  
  Xbmc_Socket_Id = Socket_Connect(host, port, function(socket_id, data)
  {
    var json_data_list = parse_json_rpc(data)
    
    for (var n = 0; n < json_data_list.length; n++)
    {
      var json_data = json_data_list[n];
    
      if (json_data.id)
      {
        if (Xbmc_Sessions[json_data.id])
        {
          if (json_data.result)
          {
            Xbmc_Sessions[json_data.id]("command_success", socket_id, null, json_data.result);
          }
          else
          {
            Xbmc_Sessions[json_data.id]("command_error", socket_id, null, json_data.error);
          }
          
          delete Xbmc_Sessions[json_data.id];
        }
      }
      else
      {
        for (var n = 0; n < Xbmc_Susbscribers.length; n++)
        {
          Xbmc_Susbscribers[n]("notification", socket_id, json_data.method, json_data.params);
        }
      }
    }
    
  }, function(socket_id, state)
  {
    Log(state);
    
    if (state == 0) // Connected
    {
      for (var n = 0; n < Xbmc_Susbscribers.length; n++)
      {
        Xbmc_Susbscribers[n]("connected", socket_id);
      }
    }
    else if (state == 1) // Disconnected
    {
      for (var n = 0; n < Mbb_Susbscribers.length; n++)
      {
        Xbmc_Susbscribers[n]("disconnected", socket_id);
        
        if (socket_id == Xbmc_Socket_Id)
        {
          Xbmc_Socket_Id = null;
        }
      }
    }
  });
  
  if (Xbmc_Socket_Id != 0)
  {
    Log("Connected to XBMC at " + host + ":" + port + " successfully!");
    return true;
  }
  
  Xbmc_Socket_Id = null;
  return false;
}
Console_RegisterCommand(Xbmc_Connect);

function Xbmc_Disconnect()
{
  if (Xbmc_IsConnected())
  {
    Socket_Disconnect(Xbmc_Socket_Id);
  }
  
  return true;
}
Console_RegisterCommand(Xbmc_Disconnect);

function Xbmc_Right()
{
  return Xbmc_SendCommand("Input.Right", {});
}
Console_RegisterCommand(Xbmc_Right);

function Xbmc_Left()
{
  return Xbmc_SendCommand("Input.Left", {});
}
Console_RegisterCommand(Xbmc_Left);

function Xbmc_Up()
{
  return Xbmc_SendCommand("Input.Up", {});
}
Console_RegisterCommand(Xbmc_Up);

function Xbmc_Down()
{
  return Xbmc_SendCommand("Input.Down", {});
}
Console_RegisterCommand(Xbmc_Down);

function Xbmc_Select()
{
  return Xbmc_SendCommand("Input.Select", {});
}
Console_RegisterCommand(Xbmc_Select);

function Xbmc_Back()
{
  return Xbmc_SendCommand("Input.Back", {});
}
Console_RegisterCommand(Xbmc_Back);

function Xbmc_PlayPause()
{
  //Xbmc_SendCommand("Player.GetActivePlayers", {});
  
  return Xbmc_SendCommand("Player.PlayPause", { "playerid" : 1 });
}
Console_RegisterCommand(Xbmc_PlayPause);

function Xbmc_Stop()
{
  return Xbmc_SendCommand("Player.Stop", { "playerid" : 1 });
}
Console_RegisterCommand(Xbmc_Stop);

