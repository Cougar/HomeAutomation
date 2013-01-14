
pages.flipswitches = {
  init: function(pageInstance)
  {
    pageInstance.pageFlipswitchElements = {};

    jQuery.each(pageInstance.flipswitches, function(n, arr)
    {
	pageInstance.pageFlipswitchElements["flipswitch" + n] = $("#page-flipswitches-flipswitch-template").tmpl({idx: n, lbl: arr[0]}).appendTo(pageInstance.pageContentElement).trigger("create");

        $('#flipswitch' + n).bind('change', function() {
     	        if (jQuery(this).val() == "on") {
                        sendCommand("IROut_SendBurst('"+pageInstance.aliases[0]+"','"+arr[1]+"','"+arr[2]+"');");
                }
                else {
			sendCommand("IROut_SendBurst('"+pageInstance.aliases[0]+"','"+arr[1]+"','"+arr[3]+"');");
                        //sendCommand("IROut_SendBurst(pageInstance.aliases[0],arr[1],arr[3]);");
                }
                //alert("mu");
                //$("#notification" + n).append('change event detected with this value: ' + jQuery(this).val() + '<BR>');
                //event.preventDefault();
          });
    });
  }

//    pageInstance.pageFlipswitchElements = $("#page-flipswitches-flipswitch-template").tmpl().appendTo(pageInstance.pageContentElement).trigger("create");
/*
.find("flipswitch").bind("change", function()
	{
		if (jQuery(this).val() == "on") {
			sendCommand("IROut_SendBurst('RFLaundryAlias','FR002T','A_A_I_1_On');");
		}
		else {
			sendCommand("IROut_SendBurst('RFLaundryAlias','FR002T','A_A_I_1_Off');");
		}
		//alert("mu");
		$("#notification").append('change event detected with this value: ' + jQuery(this).val() + '<BR>');
		event.preventDefault();
      	});
*/
/*
	$('#flipswitch').bind('change', function() {
	  if (jQuery(this).val() == "on") {
                        sendCommand("IROut_SendBurst('RFLaundryAlias','FR002T','A_A_I_1_On');");
                }
                else {
                        sendCommand("IROut_SendBurst('RFLaundryAlias','FR002T','A_A_I_1_Off');");
                }
                //alert("mu");
                $("#notification").append('change event detected with this value: ' + jQuery(this).val() + '<BR>');
                //event.preventDefault();
          });
*/

/*
    jQuery.each(pageInstance.buttons, function(button, command)
    {
      pageInstance.pageButtonElements[button] = $("#page-buttons-button-template").tmpl({ button: button }).appendTo(pageInstance.pageContentElement).trigger("create").find("button").on("click", function(event)
      {
        sendCommand(command);

        event.preventDefault();
      });
    });
*/
  //}
};
