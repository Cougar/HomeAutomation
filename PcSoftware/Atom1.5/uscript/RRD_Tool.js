

function RRD_Tool(config)
{

/* Declaration of instance variables, for static variables remove prototype */

	/* We must always call the parent constructor, initialization
	   of variables could be done here, but initialize is a better place */
	var self = this;
	
	Log("\033[33mRRD Tool updater starting...\033[0m");
	
	var last_value_string = Storage_GetParameters(config);
	this.StoredData = new Array();
	this.Timers = new Array();
	for (var name in last_value_string)
	{
		this.StoredData[name] = eval("(" + last_value_string[name] + ")");
		Log("\033[33mFound: "+name+" with update rate of "+this.StoredData[name]["rrd"]["Period_s"]+"s.\033[0m");
		this.Timers[name] = Timer_SetTimer(function(timer) {self.timerUpdate2(timer)}, this.StoredData[name]["rrd"]["Period_s"]*1000, true);
	}
	Log("\033[33mRRD Tool updater created.\033[0m");
	
}

/* Declaration of instance variables, for static variables remove prototype */
RRD_Tool.prototype.myInterval = null;
RRD_Tool.prototype.StoredData = null;
RRD_Tool.prototype.Timers = null;


RRD_Tool.prototype.timerUpdate2 = function(timer)
{
	var alias_string = Storage_GetParameters("LastValues");
	for (var name in this.Timers)
	{	
		if (this.Timers[name] == timer) {
			//Log("Timer: "+timer+", named: "+name+" with data: "+this.StoredData[name]["rrd"]["file"]+"\n");
			/* Create command to execute...*/
			var names = "";
			var values = "N";
			var cmd = "/usr/bin/rrdtool update --template ";
			var first = 1;
			for (var label in this.StoredData[name]["rrdNames"]) 
			{
				//Log("Found label: "+label+"\n");
				var data = eval("(" + alias_string[this.StoredData[name]["rrdNames"][label]["Module"]] + ")");
				if (first == 1) {
					names += label;
					first = 0;
				} else {
					names += ":"+label;  
				}
				values += ":"+data[this.StoredData[name]["rrdNames"][label]["data"]][this.StoredData[name]["rrdNames"][label]["value"]];
			}
			cmd += names + " "+this.StoredData[name]["rrd"]["file"]+" "+values;
			//Log("Command: "+cmd);
			Execute(cmd);
			break;
		}
	}
}
