

function IrManager(ir_receiver, xbmc_host, xbmc_port)
{
  var self = this;
  
  this.xbmc_host_             = xbmc_host;
  this.xbmc_port_             = xbmc_port;
  this.xbmc_reconnect_timer_  = null;
  
  Xbmc_SubscribeToConnectionStatus(function(event, connection_id, method, params)
  {
    if (event == "connected")
    {
      Timer_Cancel(timer_id);
      self.xbmc_reconnect_timer_ = null;
    }
    else if (event == "disconnected")
    {
      self._SetXbmcReconnect();
    }
    else if (event == "notification")
    {
      Log("method:" + method + ", params: " + JSON.stringify(params));
    }
    else if (event == "command_success")
    {
      
    }
    else if (event == "command_error")
    {
      Log("Command failed: " + JSON.stringify(params));
    }
  });
  
  Module_RegisterToOnMessage(ir_receiver, function(alias_name, command, variables)
  {
    var button = IRIn_CodeToName(variables["IRdata"], variables["Protocol"], "LG_MKJ42519615");
    
    if (variables["Status"] == "Pressed")
    {
      switch (button)
      {
        case "RETURN":
        {
          Xbmc_Back();
          break;
        }
        case "OK":
        {
          Xbmc_Select();
          break;
        }
        case "UP":
        {
          Xbmc_Up();
          break;
        }
        case "LEFT":
        {
          Xbmc_Left();
          break;
        }
        case "DOWN":
        {
          Xbmc_Down();
          break;
        }
        case "RIGHT":
        {
          Xbmc_Right();
          break;
        }
        case "PLAY":
        case "PAUSE":
        {
          Xbmc_PlayPause();
          break;
        }
        case "STOP":
        {
          Xbmc_Stop();
          break;
        }
      }
    }
  });
  
  this._SetXbmcReconnect = function()
  {
    self.xbmc_reconnect_timer_ = Timer_SetTimer(function(timer_id)
    {
      if (!Xbmc_IsConnected())
      {
        if (Xbmc_Connect(self.xbmc_host_, self.xbmc_port_))
        {
          Timer_Cancel(timer_id);
          self.xbmc_reconnect_timer_ = null;
        }
      }
      
    }, 5000, true);
  }
  
  if (!Xbmc_Connect(self.xbmc_host_, self.xbmc_port_))
  {
    self._SetXbmcReconnect();
  }
}
