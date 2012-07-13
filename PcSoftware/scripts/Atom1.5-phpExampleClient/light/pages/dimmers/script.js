
pages.dimmers = {
  init: function(pageInstance)
  {
    /* List to keep referenses to all the different slider elements */
    pageInstance.pageSliderElements = {};


    /* Function to update values of sliders based on new server data */
    function handleServerData(jsonData)
    {
      /* Loop through all results */
      jQuery.each(jsonData.results, function(alias, data)
      {
        /* Only do something if we have a Level value and know the alias*/
        if (pageInstance.pageSliderElements[alias] && data.Level)
        {
          /* Turn of the change event so we do not trigger it when updating the value */
          pageInstance.pageSliderElements[alias].find("input").off("change");

          /* Update the slider with the new value */
          pageInstance.pageSliderElements[alias].find("input").val(data.Level.value).slider("refresh");
          
          /* Turn on the change event again */
          pageInstance.pageSliderElements[alias].find("input").on("change", function(event, ui) { requestServerAction(alias); });
        }
      });

      /* Check for all aliases and disable if no value was received */
      jQuery.each(pageInstance.aliases, function(n, alias)
      {
        /* If no valid value was given, the alias is offline */
        if (!jsonData.results[alias] || !jsonData.results[alias].Level/* || jsonData.results[alias].Level.timestamp + 60 < ((new Date()).getTime() / 1000)*/)
        {
          /* Disable the slider component */
          pageInstance.pageSliderElements[alias].find("input").slider("disable");
        }
        else
        {
          /* Enable the slider component */
          pageInstance.pageSliderElements[alias].find("input").slider("enable");
        }
      });
    }
    
    
    /* Function to request new values from the server */
    function requestServerData()
    {
      sendCommand("Module_GetLastDataRaw " + pageInstance.aliases.join(" "), handleServerData);
    }
    
    
    /* Function to set new values to the server */
    function requestServerAction(alias)
    {
      sendCommand("Dimmer_AbsoluteFade " + alias + " 135 " + pageInstance.pageSliderElements[alias].find("input").val());
    }
    

    /* Loop through all the aliases and create sliders */
    jQuery.each(pageInstance.aliases, function(n, alias)
    {
      /* Create the slider HTML from the template and add it to the DOM */
      pageInstance.pageSliderElements[alias] = $("#page-dimmers-slider-template").tmpl({ alias: alias, value: 0 }).appendTo(pageInstance.pageContentElement).trigger("create");
      
      /* Before we have any value it should be disabled */
      pageInstance.pageSliderElements[alias].find("input").slider("disable");
      
      /* Bind a change event to the slider */
      pageInstance.pageSliderElements[alias].find("input").on("change", function(event, ui) { requestServerAction(alias); });

      /* Bind a click event to the minimum button */
      pageInstance.pageSliderElements[alias].find(".page-dimmers-button-minimum").on("click", function(event, ui) { pageInstance.pageSliderElements[alias].find("input").val(0).slider("refresh") });

      /* Bind a click event to the maximum button */
      pageInstance.pageSliderElements[alias].find(".page-dimmers-button-maximum").on("click", function(event, ui) { pageInstance.pageSliderElements[alias].find("input").val(255).slider("refresh") });
    });
    
    
    /* Request the initial value from the server */
    requestServerData();
    
    
    /* Set timer for continious polling of the current values */
    pageInstance.pollTimer = setInterval(requestServerData, 4000);
  }
};
