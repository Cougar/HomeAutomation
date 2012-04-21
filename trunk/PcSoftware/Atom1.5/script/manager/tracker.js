
function TrackerManager(host, username, password, database)
{
  var self = this;
  
  this.tracked_node_ids_ = {};
  this.db_resource_ = null;

  /* Initialize connection */
  self.db_resource_ = MySql_Connect(host, username, password);

  MySql_SelectDb(self.db_resource_, database);

  MySql_Query(self.db_resource_, "SET time_zone='+0:00';");
  
  
  /* Subscribe to MBB tracker events */
  Mbb_SubscribeToDeviceStatus(function(event_string, data)
  {
    if (event_string == "connected")
    {
      self._HandleConnectedTracker(data);
    }
    else if (event_string == "disconnected")
    {
      delete self.tracked_node_ids_[data.Info.Imei];
    }
    else if (event_string == "position")
    {
      if (data.Position !== false && self.tracked_node_ids_[data.Info.Imei])
      {
        if (data.Position.Type != "NO")
        {
          var query  =  "INSERT INTO `Positions` (`node_id`, `source`, `type`, `latitude_longitude`, `datetime`, `pdop`, `hdop`, `vdop`, `satellites`, `altitude`, `height_of_geoid`, `speed`, `angle`) VALUES (";
          query     +=  self.tracked_node_ids_[data.Info.Imei] + ", ";
          query     +=  "'gps', ";
          query     +=  "'" + data.Position.Type + "', ";
          query     +=  "GeomFromText('POINT(" + data.Position.Latitude + " " + data.Position.Longitude + ")'), ";
          query     +=  "'" + data.Position.Datetime + "', ";
          query     +=  data.Position.Pdop + ", ";
          query     +=  data.Position.Hdop + ", ";
          query     +=  data.Position.Vdop + ", ";
          query     +=  data.Position.Satellites + ", ";
          query     +=  data.Position.Altitude + ", ";
          query     +=  data.Position.HeightOfGeoid + ", ";
          query     +=  data.Position.Speed + ", ";
          query     +=  data.Position.Angle;
          query     +=  ");";
          
          MySql_Query(self.db_resource_, query);
          
          /*query = "SELECT `node_id`, `source`, AsText(latitude_longitude), `created`, `datetime`, `hdop`, `satellites`, `altitude`, `height_of_geoid`, `speed`, `angle` FROM `Positions` WHERE `id` = " + MySql_InsertId(self.db_resource_) + " LIMIT 1";
          
          Log("From DB:" + JSON.stringify(MySql_Query(self.db_resource_, query)));*/
        }
        
        Log(JSON.stringify(data.Position));
      }
    }
  });
  
  
  /* Function for finding tracked object */
  this._HandleConnectedTracker = function(data)
  {
    var result = MySql_Query(self.db_resource_, "SELECT `Nodes`.* FROM `Nodes`, `Attributes` WHERE `Nodes`.`type` = 'tracker' AND `Nodes`.`id` = `Attributes`.`node_id` AND `Attributes`.`name` = 'Imei' AND `Attributes`.`value` = '" + data.Info.Imei + "'");
      
    if (result === false || result.length == 0)
    {
      Log("Unable to find a tracker with IMEI " + data.Info.Imei);
    }
    else
    {
      var tracker_node_id = result[0]["id"];
      var tracker_node_name = result[0]["name"];
      
      Log("Found tracker \"" + tracker_node_name + "\" (NodeId " + tracker_node_id + ") that corresponds to IMEI " + data.Info.Imei);
       
      result = MySql_Query(self.db_resource_, "SELECT * FROM `Links` WHERE `node_id_down` = " + tracker_node_id + " AND `Role` = 'tracker'");
      
      if (result === false || result.length == 0)
      {
        Log("Unable to find an object that is being tracked with this tracker, will store positions on the tracker itself!");
        
        self.tracked_node_ids_[data.Info.Imei] = tracker_node_id;
      }
      else
      {
        result = MySql_Query(self.db_resource_, "SELECT * FROM `Nodes` WHERE `id` = " + result[0]["node_id_up"]);
      
        if (result === false || result.length == 0)
        {
          Log("Failed to fetch the name of the tracked object (node_id " + result[0]["node_id_up"] + ")!");
        }
        else
        {
          Log("Found \"" + result[0]["name"] + "\" (node_id " + result[0]["id"] + ") which is being tracked by \"" + tracker_node_name + "\".");

          self.tracked_node_ids_[data.Info.Imei] = result[0]["id"];
        }
      }
    }
  }
}
