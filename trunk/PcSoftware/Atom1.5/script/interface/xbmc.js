/*

Enable TCP interface (normally port 9090):
in System/Settings/Network/Services activate 
Allow programs on this system to control XBMC for localhost access only 
and Allow programs on other systems to control XBMC for access from other computers as well

http://wiki.xbmc.org/?title=JSON-RPC_API


https://groups.google.com/forum/#!msg/commandfusion/eYMgfNSfTuE/sdBuLO5TdeAJ
Quit 		jsonrpc?{"jsonrpc":"2.0","method":"Application.Quit","id":1}
Home 		jsonrpc?{"jsonrpc":"2.0","method":"Input.Home","id":1}
Update Vid 		jsonrpc?{"jsonrpc":"2.0","method":"VideoLibrary.Scan","id":1}
Clean Vid 		jsonrpc?{"jsonrpc":"2.0","method":"VideoLibrary.Clean","id":1}
Hibernate 		jsonrpc?{"jsonrpc":"2.0","method":"System.Hibernate","id":1}
Reboot 		jsonrpc?{"jsonrpc":"2.0","method":"System.Reboot","id":1}
30Sec Forward 		jsonrpc?{"jsonrpc":"2.0","id":1,"method":"Player.Seek","params":{"playerid":1,"value":"smallforward"}}
30Sec Back 		jsonrpc?{"jsonrpc":"2.0","id":1,"method":"Player.Seek","params":{"playerid":1,"value":"smallbackward"}}
10Min Forward 		jsonrpc?{"jsonrpc":"2.0","id":1,"method":"Player.Seek","params":{"playerid":1,"value":"bigforward"}}
10Min Back 		jsonrpc?{"jsonrpc":"2.0","id":1,"method":"Player.Seek","params":{"playerid":1,"value":"bigbackward"}}
Play/Pause 		jsonrpc?{"jsonrpc":"2.0","id":1,"method":"Player.PlayPause","params":{"playerid":1}}
Stop 		jsonrpc?{"jsonrpc":"2.0","id":1,"method":"Player.Stop","params":{"playerid":1}}
FF 		jsonrpc?{"jsonrpc":"2.0","id":1,"method":"Player.SetSpeed","params":{"playerid":1,"speed":"increment"}}
Rew 		jsonrpc?{"jsonrpc":"2.0","id":1,"method":"Player.SetSpeed","params":{"playerid":1,"speed":"decrement"}}
Enter 		jsonrpc?{"jsonrpc":"2.0","method":"Input.Select","id":1}
Up 		jsonrpc?{"jsonrpc":"2.0","method":"Input.Up","id":1}
Down 		jsonrpc?{"jsonrpc":"2.0","method":"Input.Down","id":1}
Left 		jsonrpc?{"jsonrpc":"2.0","method":"Input.Left","id":1}
Right 		jsonrpc?{"jsonrpc":"2.0","method":"Input.Right","id":1}
Back 		jsonrpc?{"jsonrpc":"2.0","method":"Input.Back","id":1}
Subtitle Off		 jsonrpc?{"jsonrpc":"2.0","id":1,"method":"Player.SetSubtitle","params":{"playerid":1,"subtitle":"off"}}
Subtitle On		 jsonrpc?{"jsonrpc":"2.0","id":1,"method":"Player.SetSubtitle","params":{"playerid":1,"subtitle":"on"}}
Select 		jsonrpc?{"jsonrpc":"2.0","method":"Input.Select","id":1}
Suspend		 jsonrpc?{"jsonrpc":"2.0","method":"System.Suspend","id":1}
Menu 		jsonrpc?{"jsonrpc":"2.0","method":"Input.ContextMenu","id":1}
Info		 jsonrpc?{"jsonrpc":"2.0","method":"Input.Info","id":1}
Show 		OSD jsonrpc?{"jsonrpc":"2.0","method":"Input.ShowOSD","id":1}

*/

var Xbmc_Socket_Id = null;
var Xbmc_Susbscribers = [];
var Xbmc_Sessions = {};
Xmbc_SeekValues      = function() { return [ "30sec", "10min" ]; };

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
    if (state == 0) // Connected
    {
      Log("Got connect on socket id " + socket_id);
      
      for (var n = 0; n < Xbmc_Susbscribers.length; n++)
      {
        Xbmc_Susbscribers[n]("connected", socket_id);
      }
    }
    else if (state == 1) // Disconnected
    {
      Log("Got disconnect on socket id " + socket_id);
      
      if (socket_id == Xbmc_Socket_Id)
      {
        Xbmc_Socket_Id = null;
      }
      
      for (var n = 0; n < Mbb_Susbscribers.length; n++)
      {
        Xbmc_Susbscribers[n]("disconnected", socket_id);
      }
    }
  });
  
  if (Xbmc_Socket_Id != 0)
  {
    Log("Connected to XBMC at " + host + ":" + port + " successfully!");
    return true;
  }
  
  Log("Failed to connect to XBMC at " + host + ":" + port + "!");
  
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

function Xbmc_Forward(value)
{
  seekValue = "smallforward";

  if (value == "30sec")
    seekValue = "smallforward";
  if (value == "10min")
    seekValue = "bigforward";

  return Xbmc_SendCommand("Player.Seek", { "playerid" : 1, "value" : seekValue });
}
Console_RegisterCommand(Xbmc_Forward, function(arg_index, args) { return Console_StandardAutocomplete( arg_index, args, Xmbc_SeekValues() ); } );

function Xbmc_Backward(value)
{
  seekValue = "smallforward";

  if (value == "30sec")
    seekValue = "smallforward";
  if (value == "10min")
    seekValue = "bigforward";

  return Xbmc_SendCommand("Player.Seek", { "playerid" : 1, "value" : seekValue });
}
Console_RegisterCommand(Xbmc_Backward, function(arg_index, args) { return Console_StandardAutocomplete( arg_index, args, Xmbc_SeekValues() ); } );
