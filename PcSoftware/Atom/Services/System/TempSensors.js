var sensorServices = new Array();
var sensorValues = new Array();
//var sensorNames = new Array();

ServiceManager.addCallback(this._TempSensors_SensorSeviceOnline);

function _TempSensors_SensorSeviceOnline(service) {
	if (service.getName() == "DS18x20" || service.getName() == "TC1047A" || service.getName() == "FOST02") {
		_TempSensors_updateSensorList();
	}
	//log("Sensor online\n");
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
			//log("found sensors\n");
		}
	}
}

function _TempSensors_NewTemperature(array){
      sensorValues[""+array["moduleName"]+array["moduleId"]+array["sensor"]] = new Array();
      sensorValues[""+array["moduleName"]+array["moduleId"]+array["sensor"]]['value'] = array["value"];
      sensorValues[""+array["moduleName"]+array["moduleId"]+array["sensor"]]['service'] = array["service"];
      //log("new value: from: "+""+array["moduleName"]+array["moduleId"]+array["sensor"] +"with value: "+ sensorValues[""+array["moduleName"]+array["moduleId"]+array["sensor"]]["value"]+"\n");
      //log("length is now: "+sensorValues+" \n");
/*
      if (sensorNames[""+array["moduleName"]+array["moduleId"]+array["sensor"]] == null) {
		if(getSensorName(array["moduleName"], array["moduleId"], array["sensor"]) == null) {
			sensorNames[""+array["moduleName"]+array["moduleId"]+array["sensor"]]="TempSensor_"+array["moduleName"]+array["moduleId"]+array["sensor"]
		} else {
			sensorNames[""+array["moduleName"]+array["moduleId"]+array["sensor"]]="TempSensor_"+getSensorName(array["moduleName"], array["moduleId"], array["sensor"]);
		}
		//log("Found Temperature sensor: "+ sensorNames[""+array["moduleName"]+array["moduleId"]+array["sensor"]]+"\n");
      }
      var content = array["value"]+"\n";
      var filename = "/dev/shm/can/" + sensorNames[""+array["moduleName"]+array["moduleId"]+array["sensor"]];
      setFileContents(filename, content);
*/
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
				if (sensorValues[sensorStore['SensorNames'][n]['moduleType']+sensorStore['SensorNames'][n]['moduleId']+sensorStore['SensorNames'][n]['sensorId']] != null) {
					var returnvalue = sensorValues[sensorStore['SensorNames'][n]['moduleType']+sensorStore['SensorNames'][n]['moduleId']+sensorStore['SensorNames'][n]['sensorId']]['value'];
					if (returnvalue != null) {
						return returnvalue;
					} else {
						return -50;
					}
				}
			}
			
		}
	}
	return -50;
	
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

function getSensorService(SensorName)
{
	var sensorStore = DataStore.getStore("SensorNames");
	if (sensorStore)
	{
		for (var n = 0; n < sensorStore['SensorNames'].length; n++)
		{
			if (sensorStore['SensorNames'][n]['name'] == sensorName) 
			{
				return this.sensorValues[sensorStore['SensorNames'][n]['moduleType']+sensorStore['SensorNames'][n]['moduleId']+sensorStore['SensorNames'][n]['sensorId']]['service'];
			}
			
		}
	}
	return null;
}

function getSensorList()
{
	var sensorStore = DataStore.getStore("SensorNames");
	var sensorList = new Array();
	var sensorIndex = 0;
	if (sensorStore)
	{
		for (var n = 0; n < sensorStore['SensorNames'].length; n++)
		{
			if (this.sensorValues[sensorStore['SensorNames'][n]['moduleType']+sensorStore['SensorNames'][n]['moduleId']+sensorStore['SensorNames'][n]['sensorId']] != null) 
			{
				if (this.sensorValues[sensorStore['SensorNames'][n]['moduleType']+sensorStore['SensorNames'][n]['moduleId']+sensorStore['SensorNames'][n]['sensorId']]['service'].isOnline()) {
					sensorList[sensorIndex] = new Array();
					sensorList[sensorIndex]['name'] = sensorStore['SensorNames'][n]['name'];
					sensorList[sensorIndex]['shortName'] = sensorStore['SensorNames'][n]['shortName'];
					sensorList[sensorIndex]['sensorId'] = sensorStore['SensorNames'][n]['sensorId'];
					sensorList[sensorIndex]['description'] = sensorStore['SensorNames'][n]['description'];
					sensorList[sensorIndex]['service'] = this.sensorValues[sensorStore['SensorNames'][n]['moduleType']+sensorStore['SensorNames'][n]['moduleId']+sensorStore['SensorNames'][n]['sensorId']]['service'];
//log("SensorList"+sensorIndex+" "+sensorList[sensorIndex]['name']+" \n");
					sensorIndex++;
				}
			}
		}
	}
	return sensorList;
}

function getSensorName(moduleType, moduleId, sensorId)
{
	var sensorStore = DataStore.getStore("SensorNames");
	var sensorIndex = 0;
	if (sensorStore)
	{
		for (var n = 0; n < sensorStore['SensorNames'].length; n++)
		{
			if (sensorStore['SensorNames'][n]['sensorId'] == sensorId)
			{
				if (sensorStore['SensorNames'][n]['moduleId'] == moduleId)
				{
					if (sensorStore['SensorNames'][n]['moduleType'] == moduleType) {
						return sensorStore['SensorNames'][n]['name'];
					}
				}
			}
		}
	}
	return null;
}
