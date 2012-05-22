
function TrackerManager(host, username, password, database, server_port)
{
  this.host_              = host;
  this.username_          = username;
  this.password_          = password;
  this.database_          = database;
  this.tracked_node_ids_  = {};
  this.db_resource_       = null;
  this.server_port_       = server_port;
  this.server_id_         = null;
  
 
  this._ConnectToDatabase = function()
  {
    if (this.db_resource_)
    {
      MySql_Close(this.db_resource_);
      this.db_resource_ = null;
    }
    
    this.db_resource_ = MySql_Connect(this.host_, this.username_, this.password_);

    if (this.db_resource_ === false)
    {
      return false;
    }
    
    MySql_SelectDb(this.db_resource_, this.database_);

    MySql_Query(this.db_resource_, "SET time_zone='+0:00';");
    
    return true;
  }
  
  
  /* Function for finding tracked object */
  this._Identify = function(data)
  {
    var result = MySql_Query(this.db_resource_, "SELECT `Nodes`.* FROM `Nodes`, `Attributes` WHERE `Nodes`.`type` = 'tracker' AND `Nodes`.`id` = `Attributes`.`node_id` AND `Attributes`.`name` = 'Imei' AND `Attributes`.`value` = '" + data.Imei + "'");
      
    if (result === false || result.length == 0)
    {
      Log("Unable to find a tracker with IMEI " + data.Imei);
    }
    else
    {
      var tracker_node_id = result[0]["id"];
      var tracker_node_name = result[0]["name"];
      
      Log("Found tracker \"" + tracker_node_name + "\" (NodeId " + tracker_node_id + ") that corresponds to IMEI " + data.Imei);
       
      result = MySql_Query(this.db_resource_, "SELECT * FROM `Links` WHERE `node_id_down` = " + tracker_node_id + " AND `Role` = 'tracker'");
      
      if (result === false || result.length == 0)
      {
        Log("Unable to find an object that is being tracked with this tracker, will store positions on the tracker itself!");
        
        return tracker_node_id;
      }
      else
      {
        result = MySql_Query(this.db_resource_, "SELECT * FROM `Nodes` WHERE `id` = " + result[0]["node_id_up"]);
      
        if (result === false || result.length == 0)
        {
          Log("Failed to fetch the name of the tracked object (node_id " + result[0]["node_id_up"] + ")!");
        }
        else
        {
          Log("Found \"" + result[0]["name"] + "\" (node_id " + result[0]["id"] + ") which is being tracked by \"" + tracker_node_name + "\".");

          return result[0]["id"];
        }
      }
    }
    
    return false;
  }
  
  /* Function for inserting new position in database */
  this._InsertPosition = function(tracker_node_id, data)
  {
    var result = false;
    
    var query  =  "INSERT INTO `Positions` (`node_id`, `source`, `type`, `latitude_longitude`, `datetime`, `pdop`, `hdop`, `vdop`, `satellites`, `altitude`, `height_of_geoid`, `speed`, `angle`) VALUES (";
    query     +=  tracker_node_id + ",";
    query     +=  "'gps',";
    query     +=  "'" + data.Type + "',";
    query     +=  "GeomFromText('POINT(" + data.Latitude + " " + data.Longitude + ")'),";
    query     +=  "'20" + data.Datetime + "',";
    query     +=  data.Pdop + ",";
    query     +=  data.Hdop + ",";
    query     +=  data.Vdop + ",";
    query     +=  data.Satellites + ",";
    query     +=  data.Altitude + ",";
    query     +=  data.HeightOfGeoid + ",";
    query     +=  data.Speed + ",";
    query     +=  data.Angle;
    query     +=  ");";
    
    result = MySql_Query(this.db_resource_, query);
    
    if (!result)
    {
      Log("Insert failed will try to reconnect to MySql database!");
      
      if (this._ConnectToDatabase())
      {
        Log("Reconnected to MySql database!");
        
        result = MySql_Query(this.db_resource_, query);
      }
    }
    
    /*query = "SELECT `node_id`, `source`, AsText(latitude_longitude), `created`, `datetime`, `hdop`, `satellites`, `altitude`, `height_of_geoid`, `speed`, `angle` FROM `Positions` WHERE `id` = " + MySql_InsertId(this.db_resource_) + " LIMIT 1";
    
    Log("From DB:" + JSON.stringify(MySql_Query(this.db_resource_, query)));*/
      
    //Log(JSON.stringify(data));
    
    return !(result === false);
  }
  
  /* Function for handling tracker connected disconnected */
  this._HandleClientStatusUpdate = function(client_id, state)
  {
    switch (state)
    {
      case SOCKET_STATE_CONNECTED:
      {
        Log("Client " + client_id + " connected!");
        
        this.tracked_node_ids_[client_id] = true;
        
        break;
      }
      case SOCKET_STATE_DISCONNECTED:
      {
        Log("Client " + client_id + " disconnected!");
        
        if (this.tracked_node_ids_[client_id])
        {
          delete this.tracked_node_ids_[client_id];
        }
        
        break;
      }
      default:
      {
        Log("Unknown state (" + state + ") for client " + client_id + "!");
        break;
      }
    }
  }
  
  this._SendResponse = function(client_id, id, success)
  {
    var data = "";
    var json_data = {};
    
    json_data["jsonrpc"]  = "2.0";
    json_data["result"]   = success;
    json_data["id"]       = id;

    var data = JSON.stringify(json_data);
    
    Log("Answer:" + data);

    Socket_Send(client_id, data);
  }
  
  /* Functio for handling tracker data */
  this._HandleClientDataReceived = function(client_id, data)
  {
    if (this.tracked_node_ids_[client_id])
    {
      var result = false;
      
      Log("Receive:" + data);
    
      var json_data = eval("(" + data + ")");
      
      if (json_data.method == "Tracker.AddPosition")
      {
        if (this.tracked_node_ids_[client_id])
        {
          result = this._InsertPosition(this.tracked_node_ids_[client_id], json_data.params);
        }
      }
      else if (json_data.method == "Tracker.Identify")
      {
        this.tracked_node_ids_[client_id] = this._Identify(json_data.params);
        
        result = !(this.tracked_node_ids_[client_id] === false);
      }
      else if (json_data.method == "Tracker.StatusReport")
      {
        result = true;
      }
      else
      {
        Log("Got unknown data \"" + data + "\" from client " + client_id);
      }
      
      this._SendResponse(client_id, json_data.id, result);
    }
  }
  
  this._HandleNewClient = function(client_id, server_id)
  {
    Log("Client " + client_id + " connected to tracker server " + server_id);
  }
  
  this._StartServer = function()
  {
    var self = this;
    
    this.server_id_ = Socket_StartServer(this.server_port_, function(client_id, server_id) { self._HandleNewClient(client_id, server_id); }, function(client_id, data) { self._HandleClientDataReceived(client_id, data); }, function(client_id, state) { self._HandleClientStatusUpdate(client_id, state); });
    
    if (this.server_id_ === false)
    {
      return false;
    }
    
    return true;
  }
  
  /* Initialize connection */
  this._ConnectToDatabase();
  this._StartServer();
}
