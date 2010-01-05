var dimmerServices = new Array();
var dimmerValues = new Array();

ServiceManager.addCallback(this._Dimmers_DimmerSeviceOnline);

function _Dimmers_DimmerSeviceOnline(service) {
  if (service.getName() == "Dimmer230" || service.getName() == "hwPWM") {
    _Dimmers_updateDimmerList();
  }
}

function _Dimmers_updateDimmerList() {
  this.dimmerServices = new Array();
  for (var id in ServiceManager.Services)
  {
    if (ServiceManager.Services[id].getName() == "Dimmer230" || ServiceManager.Services[id].getName() == "hwPWM")
  {
    ServiceManager.Services[id].registerEventCallback("newValue", function(args) { _Dimmers_NewData(args); });
    ServiceManager.Services[id].registerEventCallback("offline", function(args) { _Dimmers_DimmerOffline(args); });
    //ServiceManager.Services[id].setReportInterval(2);
    this.dimmerServices[dimmerServices.length] = ServiceManager.Services[id];
  }
  }
}

//dataArray["service"] = self;
//dataArray["sensor"] = canMessage.getData("SensorId");
//dataArray["value"] = canMessage.getData("Value");
//dataArray["moduleName"] = canMessage.getModuleName();
//dataArray["moduleId"] = canMessage.getModuleId();
function _Dimmers_NewData(array){
dimmerValues[""+array["moduleName"]+array["moduleId"]+array["channel"]] = new Array();
  dimmerValues[""+array["moduleName"]+array["moduleId"]+array["channel"]]["service"] = array["service"];
dimmerValues[""+array["moduleName"]+array["moduleId"]+array["channel"]]["channel"] = array["channel"];
dimmerValues[""+array["moduleName"]+array["moduleId"]+array["channel"]]["connection"] = array["connection"];
dimmerValues[""+array["moduleName"]+array["moduleId"]+array["channel"]]["currentValue"] = array["currentValue"];
dimmerValues[""+array["moduleName"]+array["moduleId"]+array["channel"]]["moduleName"] = array["moduleName"];
dimmerValues[""+array["moduleName"]+array["moduleId"]+array["channel"]]["moduleId"] = array["moduleId"];
}


function _Dimmers_DimmerOffline(data) {
  var dimmerValues = new Array();
}
		
function getDimmerValue(dimmerName)
{
	var dimmerStore = DataStore.getStore("DimmerNames");
	if (dimmerStore)
	{
		for (var n = 0; n < dimmerStore['DimmerNames'].length; n++)
		{
			if (dimmerStore['DimmerNames'][n]['name'] == dimmerName) 
			{
			
				var returnvalue = dimmerValues[dimmerStore['DimmerNames'][n]['moduleType']+dimmerStore['DimmerNames'][n]['moduleId']+dimmerStore['DimmerNames'][n]['channel']]['currentValue'];
				return returnvalue;
			}
			
		}
	}
	return null;
}

function getDimmerInfo(DimmerName)
{
	var dimmerStore = DataStore.getStore("DimmerNames");
	if (dimmerStore)
	{
		for (var n = 0; n < dimmerStore['DimmerNames'].length; n++)
		{
			if (dimmerStore['DimmerNames'][n]['name'] == DimmerName) 
			{
				var returnvalue = new Array();
				returnvalue['name'] = dimmerStore['DimmerNames'][n]['name'];
				returnvalue['description'] = dimmerStore['DimmerNames'][n]['description'];
				returnvalue['shortName'] = dimmerStore['DimmerNames'][n]['shortName'];
				returnvalue['moduleType'] = dimmerStore['DimmerNames'][n]['moduleType'];
				returnvalue['moduleId'] = dimmerStore['DimmerNames'][n]['moduleId'];
				returnvalue['channel'] = dimmerStore['DimmerNames'][n]['channel'];
				return returnvalue;	
			}
			
		}
	}
	return null;
}

function getDimmerService(dimmerName)
{
	var dimmerStore = DataStore.getStore("DimmerNames");
	if (dimmerStore)
	{
		for (var n = 0; n < dimmerStore['DimmerNames'].length; n++)
		{
			if (dimmerStore['DimmerNames'][n]['name'] == dimmerName) 
			{
			//log("Hej3: "+dimmerStore['DimmerNames'][n]['moduleType']+dimmerStore['DimmerNames'][n]['moduleId']+dimmerStore['DimmerNames'][n]['channel']+"\n");
//log("Hej3: "+dimmerValues[dimmerStore['DimmerNames'][n]['moduleType']+dimmerStore['DimmerNames'][n]['moduleId']+dimmerStore['DimmerNames'][n]['channel']]['currentValue']+"\n");
				return this.dimmerValues[dimmerStore['DimmerNames'][n]['moduleType']+dimmerStore['DimmerNames'][n]['moduleId']+dimmerStore['DimmerNames'][n]['channel']]['service'];
				
			}
			
		}
	}
	return null;
}

function getDimmerList()
{
	var dimmerStore = DataStore.getStore("DimmerNames");
	var dimmerList = new Array();
	var dimmerIndex = 0;
	if (dimmerStore)
	{
		for (var n = 0; n < dimmerStore['DimmerNames'].length; n++)
		{
			if (this.dimmerValues[dimmerStore['DimmerNames'][n]['moduleType']+dimmerStore['DimmerNames'][n]['moduleId']+dimmerStore['DimmerNames'][n]['channel']] != null) 
			{
				if (this.dimmerValues[dimmerStore['DimmerNames'][n]['moduleType']+dimmerStore['DimmerNames'][n]['moduleId']+dimmerStore['DimmerNames'][n]['channel']]['service'].isOnline()) {
					dimmerList[dimmerIndex] = new Array();
					dimmerList[dimmerIndex]['name'] = dimmerStore['DimmerNames'][n]['name'];
					dimmerList[dimmerIndex]['shortName'] = dimmerStore['DimmerNames'][n]['shortName'];
dimmerList[dimmerIndex]['channel'] = dimmerStore['DimmerNames'][n]['channel'];
					dimmerList[dimmerIndex]['description'] = dimmerStore['DimmerNames'][n]['description'];
					dimmerList[dimmerIndex]['service'] = this.dimmerValues[dimmerStore['DimmerNames'][n]['moduleType']+dimmerStore['DimmerNames'][n]['moduleId']+dimmerStore['DimmerNames'][n]['channel']]['service'];
					dimmerIndex++;
				}
			}
		}
	}
	return dimmerList;
}


