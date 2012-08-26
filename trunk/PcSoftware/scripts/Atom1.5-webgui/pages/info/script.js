
pages.PID = {
  init: function(pageInstance)
  {
    this.pageshow = function(arg) {
	/* Request the initial value from the server */
      requestServerDataUpdate();
      requestServerData();
      /* Set timer for continious polling of the current values */
      pageInstance.pollTimer = setInterval(reloadImage, 60000);
      pageInstance.pollTimer2 = setInterval(requestServerData, 5000);
    }
    
    this.pagehide = function(arg) {
      clearInterval(pageInstance.pollTimer);
      clearInterval(pageInstance.pollTimer2);
    }
    
    
    /* List to keep referenses to all the different slider elements */
    pageInstance.pagePIDElements = {};
    
    
    /* Function to update values based on new server data */
    function handleServerDataUpdate(jsonData)
    {
      /* Loop through all results */
      jQuery.each(jsonData, function(alias, data)
      {
	  /* Loop through all values */
	  jQuery.each(data, function(name, value)
	  {
	    /* Check if the value is simple or complex */
	    if (typeof value.value != "string")
	    {
	      jQuery.each(value.value, function(name2, value2)
	      {
		if (name2 == "Reference") 
		{
		    pageInstance.pageInputElements["reference"].find("input").val(value2)
		}
		/*if (name2 == "K_P") 
		{
		    pageInstance.pageInputElements["kp"].find("input").val(value2)
		}
		if (name2 == "K_I") 
		{
		    pageInstance.pageInputElements["ki"].find("input").val(value2)
		}
		if (name2 == "K_D") 
		{
		    pageInstance.pageInputElements["kd"].find("input").val(value2)
		}
		if (name2 == "TimeUnit") 
		{
		    pageInstance.pageInputElements["timeunit"].find("input").val(value2)
		}
		if (name2 == "RegulatorTime") 
		{
		    pageInstance.pageInputElements["time"].find("input").val(value2)
		}
		*/
	      });
	    }
	  });
      });
    }
    
    
    /* Function to update values based on new server data */
    function handleServerData(jsonData)
    {
      var g1,g2,g3,g4;
      /* Loop through all results */
      jQuery.each(jsonData, function(alias, data)
      {
	  /* Loop through all values */
	  jQuery.each(data, function(name, value)
	  {
	    /* Check if the value is simple or complex */
	    if (typeof value.value != "string")
	    {
	      jQuery.each(value.value, function(name2, value2)
	      {
		if (name2 == "Reference") 
		{
		    g2 = value2;
		}
		if (name2 == "Measurment") 
		{
		    g1 = value2;
		}
		if (name2 == "PWM") 
		{
		    g3 = value2;
		}
		if (name2 == "Sum") 
		{
		    g4 = value2;
		}
	      });
	    }
	  });
      });
      pageInstance.pageInputElements["grid"].html($("#page-PID-grid-template").tmpl({ grid1: "Uppmätt:  "+g1, grid2: "Referens:  "+g2,grid3: "PWM:  "+g3,grid4: "Förändring:  "+g4 }));
    }
    
    
    /* Function to request new values from the server */
    function requestServerDataUpdate()
    {
      sendCommand("JSON.stringify(Module_GetLastDataNative('" + pageInstance.alias + "'));", handleServerDataUpdate);
    }
    
    /* Function to request new values from the server */
    function requestServerData()
    {
      sendCommand("JSON.stringify(Module_GetLastDataNative('" + pageInstance.alias + "'));", handleServerData);
    }
    
    function reloadImage()
    {
	pageInstance.pageImageElements["regulatorbild"].find("img").attr('src', 'http://linuxz.mine.nu/CANgraph/regulator.png?' + Math.random());
    }
    
    var name = pageInstance.alias;
    
    if (aliasNames[pageInstance.alias])
    {
      name = aliasNames[pageInstance.alias];
    }
    
    /* Create the PID HTML from the template and add it to the DOM */
   
    pageInstance.pageInputElements = {};
    pageInstance.pageImageElements = {};
    pageInstance.pageButtonElements = {};
    

    
    pageInstance.pageInputElements["reference"] = $("#page-PID-input-button-template").tmpl({ input_name: "Referens:  " ,button: "Spara referens"}).appendTo(pageInstance.pageContentElement).trigger("create");
    /*.find("button").on("click", function(event)
    {
      sendCommand("JSON.stringify(PID_setValue("+pageInstance.alias+", '26'));");
      event.preventDefault();
    });
    */
    pageInstance.pageInputElements["reference"].find("button").on("click", function(event)
    {
      sendCommand("JSON.stringify(PID_setValue('" + pageInstance.alias + "', '"+pageInstance.pageInputElements["reference"].find("input").val()+"'));");
      //sendCommand("JSON.stringify(PID_setValue("+pageInstance.alias+", '26'));");
      event.preventDefault();
    });
    
    
    pageInstance.pageImageElements["regulatorbild"] = $("#page-PID-image-template").tmpl({}).appendTo(pageInstance.pageContentElement).trigger("create");
    
    pageInstance.pageInputElements["grid"] = $("#page-PID-grid-template").tmpl({ grid1: "Uppmätt: ", grid2: "Referens: ",grid3: "PWM: ",grid4: "Förändring: " }).appendTo(pageInstance.pageContentElement).trigger("create");
    /*
    pageInstance.pageInputElements["kp"] = $("#page-PID-input-number-template").tmpl({ input_name: "Kp:  " }).appendTo(pageInstance.pageContentElement).trigger("create");
    pageInstance.pageInputElements["ki"] = $("#page-PID-input-number-template").tmpl({ input_name: "Ki:  " }).appendTo(pageInstance.pageContentElement).trigger("create");
    pageInstance.pageInputElements["kd"] = $("#page-PID-input-number-template").tmpl({ input_name: "Kd:  " }).appendTo(pageInstance.pageContentElement).trigger("create");
    pageInstance.pageInputElements["timeunit"] = $("#page-PID-input-template").tmpl({ input_name: "Tidsenhet [s/ms]:  " }).appendTo(pageInstance.pageContentElement).trigger("create");
    pageInstance.pageInputElements["time"] = $("#page-PID-input-button-template").tmpl({ input_name: "Tid:  "  ,button: "Spara regulatorkonstanter"}).appendTo(pageInstance.pageContentElement).trigger("create");
    */
    
    
    
    pageInstance.pageButtonElements["Update"] = $("#page-PID-button-template").tmpl({ button: "Update" }).appendTo(pageInstance.pageContentElement).trigger("create").find("button").on("click", function(event)
    {
      requestServerDataUpdate();
      event.preventDefault();
    });
  }
 };
  