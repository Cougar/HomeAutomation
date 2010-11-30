GetModule("irReceive");

var irrecord_remote_name = null;
var irrecord_remote_identification = null;
var irrecord_remote_last = null;

function irrecord(alias, remote_name)
{
	if (!remote_name)
	{
		return "You must supply a name for the remote to record";
	}
	
	irrecord_remote_name = "Remote_" + remote_name;
	irrecord_remote_identification = resolvealias(alias);

	if (irrecord_remote_identification["is_group"])
	{
		return "Alias can not be an alias group";
	}

	return prompt("What is the name of the button you just pressed?", irrecord_handle_input, "Press a key on you remote and then name it.\n");
}
RegisterConsoleCommand(irrecord, function(args) { return StandardAutocomplete(args, irReceive.instance_.GetAvailableNames()); });

function irrecord_handle_input(response)
{
	if (response == ".")
	{
		return "Finished!";
	}
	
	if (!irrecord_remote_last)
	{
		return prompt("What is the name of the button you just pressed?", irrecord_handle_input, "You did not press anything on you remote, try again!\nPress a key on you remote and then name it.\n");
	}
	
	Storage_SetParameter(irrecord_remote_name, response, JSON.stringify(irrecord_remote_last));
	
	var result = prompt("What is the name of the button you just pressed?", irrecord_handle_input, "Saved " + response + " with protocol " + irrecord_remote_last["protocol"] + " and data " + irrecord_remote_last["data"] + "\nPress a key on you remote and then name it.\n");
	
	irrecord_remote_last = null;
	
	return result;
}

function irrecord_on_ir(event, id, channel, data, protocol)
{
	if (id == irrecord_remote_identification["module_id"] && channel == irrecord_remote_identification["extra"])
	{
		irrecord_remote_last = { "protocol" : protocol, "data" : data };
	}
}


irReceive.instance_.Bind("onReleased", irrecord_on_ir);
