/* RRD_Tool() must be called from autostart.js to start timers */
function RRD_Tool()
{
	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	var self = this;
	
	/* Constant config name */
	RRD_Tool_Config = "RRD_Tool_config";
	
	Log("\033[33mRRD Tool updater starting...\033[0m");
	
	var last_value_string = Storage_GetParameters(RRD_Tool_Config);
	RRD_Tool_StoredData = new Array();
	RRD_Tool_Timers = new Array();
	for (var name in last_value_string)
	{
		RRD_Tool_StoredData[name] = eval("(" + last_value_string[name] + ")");
		Log("\033[33mFound: "+name+" with update rate of "+RRD_Tool_StoredData[name]["rrd"]["Period_s"]+"s.\033[0m");
		RRD_Tool_Timers[name] = Timer_SetTimer(function(timer) {RRD_Tool_timerUpdate(timer)}, RRD_Tool_StoredData[name]["rrd"]["Period_s"]*1000, true);
	}
	Log("\033[33mRRD Tool updater created.\033[0m");
}

/* Declaration of static variables */
RRD_Tool_myInterval = null;
RRD_Tool_StoredData = null;
RRD_Tool_Timers = null;
RRD_Tool_Config = null;

RRD_Tool_timerUpdate = function(timer)
{
	var alias_string = Storage_GetParameters("LastValues");
	for (var name in RRD_Tool_Timers)
	{	
		if (RRD_Tool_Timers[name] == timer) {
			//Log("Timer: "+timer+", named: "+name+" with data: "+RRD_Tool_StoredData[name]["rrd"]["file"]+"\n");
			/* Create command to execute...*/
			var names = "";
			var values = "N";
			var cmd = "/usr/bin/rrdtool update --template ";
			var first = 1;
			for (var label in RRD_Tool_StoredData[name]["rrdNames"]) 
			{
				//Log("Found label: "+label+"\n");
				var data = eval("(" + alias_string[RRD_Tool_StoredData[name]["rrdNames"][label]["Module"]] + ")");
				if (first == 1) {
					names += label;
					first = 0;
				} else {
					names += ":"+label;  
				}
				values += ":"+data[RRD_Tool_StoredData[name]["rrdNames"][label]["data"]][RRD_Tool_StoredData[name]["rrdNames"][label]["value"]];
			}
			cmd += names + " "+RRD_Tool_StoredData[name]["rrd"]["file"]+" "+values;
			//Log("Command: "+cmd);
			Execute(cmd);
			break;
		}
	}
}

/* Tab complete stuff */
RRD_Tool_Aliases        = function() { return Module_GetAliasNames(); };
RRD_Tool_Intervals      = function() { return [ 5, 10, 30, 60, 120, 150, 300 ]; };
/* Function returns an array with all CAN variables existing in LastValues storage */
RRD_Tool_Variables      = function() 
{
	var last_value_string = Storage_GetParameters("LastValues");
	var last_values = new Array();
	var last_values_variables = [];
	for (var name in last_value_string)
	{
		last_values[name] = eval("(" + last_value_string[name] + ")");
		for (var name2 in last_values[name])
		{
			/* Only get unique variables */
			if (last_values_variables.indexOf(name2) == -1)
			{
				last_values_variables.push(name2);
			}
		}
	}
	return last_values_variables;
};

/*
electricPower={"rrd":{"file":"/var/www/appdata/rrd-data-new/electricPower.rrd","Period_s":"60"}, "rrdNames":{"power":{"Module":"powerMeter","data":"Power32","value":"value"}}}

<store_name>={"rrd":{"file":"<rrd_filename>","Period_s":"<push_interval>"}, "rrdNames":{"<rrd_data_source_name>":{"Module":"<alias_name>","data":"<variable_name>","value":"value"}}}
*/

/* 
TODO: 
handle more than one rrd data source (not possible with different alias/variables)
function RRDTool_AddConfig(alias_name, variable_name, push_interval, store_name, rrd_filename, rrd_data_source_name_array)
*/

/* Function to add new rrd config on atomic command line interface */
function RRDTool_AddConfig(alias_name, variable_name, push_interval, store_name, rrd_filename, rrd_data_source_name)
{
	/* In case RRD_Tool() is not called from autostart.js yet */
	if (RRD_Tool_Config == null || RRD_Tool_StoredData == null || RRD_Tool_Timers == null)
	{
		RRD_Tool_Config = "RRD_Tool_config";
		RRD_Tool_StoredData = new Array();
		RRD_Tool_Timers = new Array();
		
		Log("\033[31mRRD_Tool() must be called from autostart.js!.\033[0m\n");
	}
	
	//Log("rrd data source array length: "+rrd_data_source_name_array.length);
	//if (rrd_data_source_name_array.length > 0)
	//{
		var newString = "{\"rrd\":{\"file\":\"" + rrd_filename + "\",\"Period_s\":\"" + push_interval + "\"},\"rrdNames\":{\"" + rrd_data_source_name + "\":{\"Module\":\""+alias_name+"\",\"data\":\""+variable_name+"\",\"value\":\"value\"}}";
		//for (var rrd_data_source in rrd_data_source_name_array)
		//{
		//	newString += "";
		//}
		newString += "}";
		
		/* Push new config line to storage */
		Storage_SetParameter(RRD_Tool_Config, store_name, newString);
		//Log("New config: "+newString + " configname " + RRD_Tool_Config +"\n");
	//}

	/* Start this new config now */
	RRD_Tool_StoredData[store_name] = eval("(" + newString + ")");
	RRD_Tool_Timers[store_name] = Timer_SetTimer(function(timer) {RRD_Tool_timerUpdate(timer)}, RRD_Tool_StoredData[store_name]["rrd"]["Period_s"]*1000, true);	
	Log("\033[33mCreated: "+store_name+" with update rate of "+RRD_Tool_StoredData[store_name]["rrd"]["Period_s"]+"s.\033[0m\n");
}

Console_RegisterCommand(RRDTool_AddConfig, function(arg_index, args) { return Console_StandardAutocomplete(arg_index, args, RRD_Tool_Aliases(), RRD_Tool_Variables(), RRD_Tool_Intervals() ); });

