
pages.info = {
  init: function(pageInstance)
  {
    /* List to keep referenses to all the different slider elements */
    pageInstance.pageInfoElements = {};


    /* Function to update values of sliders based on new server data */
    function handleServerData(jsonData)
    {
      /* Loop through all results */
      jQuery.each(jsonData.results, function(alias, data)
      {
        /* Only do something if we know the alias*/
        if (pageInstance.pageInfoElements[alias])
        {
          /* Clear existing information */
          pageInstance.pageInfoElements[alias].empty();

          var text = "";

          /* Loop through all values */
          jQuery.each(data, function(name, value)
          {
            /* Check if the value is simple or complex */
            if (typeof value.value == "string")
            {
              text += "<b>" + name + "</b> : <i>" + value.value + "</i><br/>";
            }
            else
            {
              text += "<b>" + name + "</b><br/>";
              jQuery.each(value.value, function(name2, value2)
              {
                text += "&nbsp;&nbsp;" + name2 + " : <i>" + value2 + "</i><br/>";
              });
            }
          });

          /* If no values were found print a nice text */
          if (text.length == 0)
          {
            text = "<i>No data available</i>";
          }

          /* Add the title */
          text = "<h3>" + alias + "</h3>" + text;

          /* Update the element markup */
          pageInstance.pageInfoElements[alias].html(text);
        }
      });
    }
    
    
    /* Function to request new values from the server */
    function requestServerData()
    {
      sendCommand("Module_GetLastDataRaw " + pageInstance.aliases.join(" "), handleServerData);
    }
        

    /* Loop through all the aliases and create sliders */
    jQuery.each(pageInstance.aliases, function(n, alias)
    {
      /* Create the slider HTML from the template and add it to the DOM */
      pageInstance.pageInfoElements[alias] = $("#page-info-template").tmpl({ alias: alias }).appendTo(pageInstance.pageContentElement).trigger("create");
    });
    
    
    /* Request the initial value from the server */
    requestServerData();
    
    
    /* Set timer for continious polling of the current values */
    pageInstance.pollTimer = setInterval(requestServerData, 4000);
  }
};
