
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
  Log(data);
  
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
      
      Mbb_Clients[client_id]["Position"] = _Mbb_NmeaToPosition(client_id);
      
      for (var n = 0; n < Mbb_Susbscribers.length; n++)
      {
        Mbb_Susbscribers[n]("position", Mbb_Clients[client_id]);
      }
      
      Mbb_Clients[client_id]["LastNmea"]["GGA"] = "";
      Mbb_Clients[client_id]["LastNmea"]["RMC"] = "";
      Mbb_Clients[client_id]["Position"] = false;
    }
  }
  else /*if (json_data.method == "MBB.Connected")*/
  {
    json_data = { "params" : { "Imei" : "004401700721448" } };
    
    Log("Client " + client_id + " has IMEI " + json_data.params.Imei);
    
    Mbb_Clients[client_id]["Id"] = client_id;
    Mbb_Clients[client_id]["Info"] = json_data.params;
    Mbb_Clients[client_id]["Position"] = false;
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

function _Mbb_NmeaToPosition(client_id)
{
  /*
      GGA          Global Positioning System Fix Data
*     123519       Fix taken at 12:35:19 UTC
*     4807.038,N   Latitude 48 deg 07.038' N
*     01131.000,E  Longitude 11 deg 31.000' E
  1            Fix quality: 0 = invalid
                            1 = GPS fix (SPS)
                            2 = DGPS fix
                            3 = PPS fix
                            4 = Real Time Kinematic
                            5 = Float RTK
                            6 = estimated (dead reckoning) (2.3 feature)
                            7 = Manual input mode
                            8 = Simulation mode
*     08           Number of satellites being tracked
*     0.9          Horizontal dilution of position
*     545.4,M      Altitude, Meters, above mean sea level
*     46.9,M       Height of geoid (mean sea level) above WGS84
                  ellipsoid
  (empty field) time in seconds since last DGPS update
  (empty field) DGPS station ID number
  *47          the checksum data, always begins with *
  
  
  $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

      RMC          Recommended Minimum sentence C
*     123519       Fix taken at 12:35:19 UTC
*     A            Status A=active or V=Void.
*     4807.038,N   Latitude 48 deg 07.038' N
*     01131.000,E  Longitude 11 deg 31.000' E
*     022.4        Speed over the ground in knots
*     084.4        Track angle in degrees True
*     230394       Date - 23rd of March 1994
*     003.1,W      Magnetic Variation
  *6A          The checksum data, always begins with *
*/
  
  
  var nmea_rmc_parts = Mbb_Clients[client_id]["LastNmea"]["RMC"].split(",");
  
  if (nmea_rmc_parts[2] == "A")
  {
    var nmea_gga_parts = Mbb_Clients[client_id]["LastNmea"]["GGA"].split(",");
    var position = {};
  
    position["Datetime"]            = "20" + nmea_rmc_parts[9].substr(4, 2) + "-" + nmea_rmc_parts[9].substr(2, 2) + "-" + nmea_rmc_parts[9].substr(0, 2);
    position["Datetime"]            += " " + nmea_rmc_parts[1].substr(0, 2) + ":" + nmea_rmc_parts[1].substr(2, 2) + ":" + nmea_rmc_parts[1].substr(4, 2) + " UTC";
  
    
    /* Calculate decimal latitude */
    var degrees = parseFloat(nmea_rmc_parts[3].substr(0, 2));
    var decimal = parseFloat(nmea_rmc_parts[3].substr(2));
    
    position["Latitude"] = degrees + (decimal / 60.0);
    
    if (nmea_rmc_parts[4] == "S")
    {
      position["Latitude"] = -position["Latitude"];
    }

    /* Calculate decimal longitude */
    var degrees = parseFloat(nmea_rmc_parts[5].substr(0, 3));
    var decimal = parseFloat(nmea_rmc_parts[5].substr(3));
    
    position["Longitude"] = degrees + (decimal / 60.0);
    
    if (nmea_rmc_parts[6] == "W")
    {
      position["Longitude"] = -position["Longitude"];
    }
    
    position["Hdop"] = nmea_gga_parts[8];
    
    position["Satellites"] = nmea_gga_parts[7];
    
    position["Altitude"] = nmea_gga_parts[9]; // Meters
    position["HeightOfGeoid"] = nmea_gga_parts[11] // Meters
    
    position["Speed"] = nmea_rmc_parts[7]; // Knots
    position["Angle"] = nmea_rmc_parts[8];
    
    if (nmea_rmc_parts[10] != "")
    {
      position["MagneticDeclination"] = parseFloat(nmea_rmc_parts[10]);
      
      if (nmea_rmc_parts[11].substr(0, 1) == "W")
      {
        position["MagneticDeclination"] = -position["MagneticDeclination"];
      }
    }
    
    return position;
  }
  
  return false;
}
