var sensorServices = new Array();
var sensorValues = new Array();

ServiceManager.addCallback(this._TempSensors_SensorSeviceOnline);

function _TempSensors_SensorSeviceOnline(service) {
  if (service.getName() == "DS18x20" || service.getName() == "TC1047A" || service.getName() == "FOST02") {
    _TempSensors_updateSensorList();
  }
}

function _TempSensors_updateSensorList() {
  this.sensorServices = new Array();
  for (var id in ServiceManager.Services)
  {
    if (ServiceManager.Services[id].getName() == "DS18x20" || ServiceManager.Services[id].getName() == "TC1047A" || ServiceManager.Services[id].getName() == "FOST02")
  {
    ServiceManager.Services[id].registerEventCallback("newValue", function(args) { _TempSensors_NewTemperature(args); });
    ServiceManager.Services[id].registerEventCallback("offline", function(args) { _TempSensors_SensorOffline(args); });
    ServiceManager.Services[id].setReportInterval(2);
    this.sensorServices[sensorServices.length] = ServiceManager.Services[id];
  }
  }
}

//dataArray["service"] = self;
//dataArray["sensor"] = canMessage.getData("SensorId");
//dataArray["value"] = canMessage.getData("Value");
//dataArray["moduleName"] = canMessage.getModuleName();
//dataArray["moduleId"] = canMessage.getModuleId();
function _TempSensors_NewTemperature(array){
  sensorValues[""+array["moduleName"]+array["moduleId"]+array["sensor"]] = array["value"];
}

function _TempSensors_SensorOffline(data) {
  var sensorValues = new Array();
}
		
function getSensorValue(SensorName)
{
	var sensorStore = DataStore.getStore("SensorNames");
	if (sensorStore)
	{
		for (var n = 0; n < sensorStore['SensorNames'].length; n++)
		{
			if (sensorStore['SensorNames'][n]['name'] == SensorName) 
			{
			
				var returnvalue = sensorValues[sensorStore['SensorNames'][n]['moduleType']+sensorStore['SensorNames'][n]['moduleId']+sensorStore['SensorNames'][n]['sensorId']];
				if (returnvalue != null) {
					return returnvalue;
				} else {
					return -50;
				}
			}
			
		}
	}
	return null;
	
}

function getSensorInfo(SensorName)
{
	var sensorStore = DataStore.getStore("SensorNames");
	if (sensorStore)
	{
		for (var n = 0; n < sensorStore['SensorNames'].length; n++)
		{
			if (sensorStore['SensorNames'][n]['name'] == SensorName) 
			{
				var returnvalue = new Array();
				returnvalue['name'] = sensorStore['SensorNames'][n]['name'];
				returnvalue['description'] = sensorStore['SensorNames'][n]['description'];
				returnvalue['shortName'] = sensorStore['SensorNames'][n]['shortName'];
				returnvalue['moduleType'] = sensorStore['SensorNames'][n]['moduleType'];
				returnvalue['moduleId'] = sensorStore['SensorNames'][n]['moduleId'];
				returnvalue['sensorId'] = sensorStore['SensorNames'][n]['sensorId'];
				return returnvalue;	
			}
			
		}
	}
	return null;
}


