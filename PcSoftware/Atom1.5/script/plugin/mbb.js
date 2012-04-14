
Mbb_Clients = {};

Mbb_Susbscribers = [];

function Mbb_SubscribeToDeviceStatus(callback)
{
  Mbb_Susbscribers.push(callback);
}

function Mbb_Connected(client_id)
{
  Log(client_id + " connected!");
  
  Mbb_Clients[client_id] = {};
}

function Mbb_Disconnected(client_id)
{
  Log(client_id + " disconnected!");
  
  for (var n = 0; n < Mbb_Susbscribers.length; n++)
  {
    Mbb_Susbscribers[n]("disconnected", Mbb_Clients[client_id]);
  }
  
  delete Mbb_Clients[client_id];
}

function Mbb_ReceivedData(client_id, data)
{
  var json_data = eval("(" + data + ")");
  
  //Log(JSON.stringify(json_data));
  
  if (json_data.method == "MBB.GpsNmeaData")
  {
    if (json_data.params.nmea.substr(0, 6) == "$GPGGA")
    {
      Mbb_Clients[client_id]["LastNmea"]["GGA"] = json_data.params.nmea;
    }
    else if (json_data.params.nmea.substr(0, 6) == "$GPRMC")
    {
      Mbb_Clients[client_id]["LastNmea"]["RMC"] = json_data.params.nmea;
      
      for (var n = 0; n < Mbb_Susbscribers.length; n++)
      {
        Mbb_Susbscribers[n]("nmea", Mbb_Clients[client_id]);
      }
      
      Mbb_Clients[client_id]["LastNmea"]["GGA"] = "";
      Mbb_Clients[client_id]["LastNmea"]["RMC"] = "";
    }
  }
  else /*if (json_data.method == "MBB.Connected")*/
  {
    json_data = { "params" : { "Imei" : "004401700721448" } };
    
    Log("Client " + client_id + " has IMEI " + json_data.params.Imei);
    
    Mbb_Clients[client_id]["Id"] = client_id;
    Mbb_Clients[client_id]["Info"] = json_data.params;
    Mbb_Clients[client_id]["LastNmea"] = {};
    Mbb_Clients[client_id]["LastNmea"]["GGA"] = "";
    Mbb_Clients[client_id]["LastNmea"]["RMC"] = "";
    
    for (var n = 0; n < Mbb_Susbscribers.length; n++)
    {
      Mbb_Susbscribers[n]("connected", Mbb_Clients[client_id]);
    }
  }
  /*else
  {
    Log("Got unknown data \"" + json_data + "\" from client " + client_id);
  }*/
}

function Mbb_SendData(client_id, json_data)
{
  return MbbExport_SendData(client_id, JSON.Stringify(json_data));
}
