pages.irdevicestate = {
  init: function(pageInstance)
  {
    /* List to keep referenses to all the different switch elements */
    pageInstance.pageSwitchElements = {};

    this.pageshow = function(arg) {
      /* Request the initial value from the server */
      requestServerData();
      /* Set timer for continious polling of the current values */
      pageInstance.pollTimer = setInterval(requestServerData, 4000);
    }
    
    this.pagehide = function(arg) {
      clearInterval(pageInstance.pollTimer);
    }

    /* Function to update values of switches based on new server data */
    function handleServerData(jsonData)
    {
      /* Loop through all results */
      jQuery.each(jsonData, function(alias, data)
      {
        /* Only do something if we know the device and have a value */
        if (pageInstance.pageSwitchElements[alias] && data.State && data.State.value  && data.State.timestamp)
        {
          /* Enable the switch if it was disabled by default */
          pageInstance.pageSwitchElements[alias].find("select").slider("enable");
          /* Update the slider with the new value */
          pageInstance.pageSwitchElements[alias].find("select").val(data.State.value.toLowerCase()).slider("refresh");
          
          var date = new Date(data.State.timestamp*1000);
          //pageInstance.pageSwitchElements[device].find(":jqmData(id=page-irdevice-timestamp-"+device+")").text(date.toDateString()+", "+date.toLocaleTimeString());
          pageInstance.pageSwitchElements[alias].find(":jqmData(id=page-irdevice-timestamp-"+alias+")").text(jQuery.timeago(date));
        }
      });
    }
    
    
    /* Function to request new values from the server */
    function requestServerData()
    {
      sendCommand("JSON.stringify(Module_GetLastDataNative('" + pageInstance.aliases.join("','") + "'));", handleServerData);
    }
    
    
    /* Loop through all the devices and create switches */
    jQuery.each(pageInstance.aliases, function(n, alias)
    {
      var name = alias;

      if (aliasNames[alias])
      {
        name = aliasNames[alias];
      }
      
      /* Create the slider HTML from the template and add it to the DOM */
      pageInstance.pageSwitchElements[alias] = $("#page-irdevicestate-template").tmpl({ device: alias, name: name }).appendTo(pageInstance.pageContentElement).trigger("create");
      
      /* Before we have any value it should be disabled */
      pageInstance.pageSwitchElements[alias].find("select").slider("disable");
    });
  }
};

