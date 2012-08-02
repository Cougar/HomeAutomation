pages.irdevicestate = {
  init: function(pageInstance)
  {
    /* List to keep referenses to all the different switch elements */
    pageInstance.pageSwitchElements = {};


    /* Function to update values of switches based on new server data */
    function handleServerData(jsonData)
    {
      /* Loop through all results */
      jQuery.each(jsonData, function(device, data)
      {
        /* Only do something if we know the device and have a value */
        if (pageInstance.pageSwitchElements[device] && data.state  && data.timestamp)
        {
          /* Enable the switch if it was disabled by default */
          pageInstance.pageSwitchElements[device].find("select").slider("enable");
          /* Update the slider with the new value */
          pageInstance.pageSwitchElements[device].find("select").val(data.state).slider("refresh");
          
          var date = new Date(data.timestamp*1000);
          pageInstance.pageSwitchElements[device].find(":jqmData(id=page-irdevice-timestamp-"+device+")").text(date.toDateString()+", "+date.toLocaleTimeString());
        }
      });
    }
    
    
    /* Function to request new values from the server */
    function requestServerData()
    {
      sendCommand("JSON.stringify(IrDeviceState_GetDataNative('" + pageInstance.devices.join("','") + "'));", handleServerData);
    }
    
    
    /* Loop through all the devices and create switches */
    jQuery.each(pageInstance.devices, function(n, device)
    {
      var name = device;

      if (aliasNames[device])
      {
        name = aliasNames[device];
      }
      
      /* Create the slider HTML from the template and add it to the DOM */
      pageInstance.pageSwitchElements[device] = $("#page-irdevicestate-template").tmpl({ device: device, name: name }).appendTo(pageInstance.pageContentElement).trigger("create");
      
      /* Before we have any value it should be disabled */
      pageInstance.pageSwitchElements[device].find("select").slider("disable");
    });
    
    
    /* Request the initial value from the server */
    requestServerData();
    
    
    /* Set timer for continious polling of the current values */
    pageInstance.pollTimer = setInterval(requestServerData, 4000);
  }
};

