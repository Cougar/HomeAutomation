
pages.phonecalls = {
  init: function(pageInstance)
  {
    pageInstance.pagePhoneCallsElement = null;

    /* Function to update values of sliders based on new server data */
    function handleServerData(jsonData)
    {
      var list = [];

      pageInstance.pageContentElement.empty();
      pageInstance.pagePhoneCallsElement = $("#page-phonecalls-template").tmpl().appendTo(pageInstance.pageContentElement);
    
      /* Loop through all */
      jQuery.each(jsonData.results, function(timestamp, data)
      {
        var data = JSON.parse(data);
        var found = false;

        jQuery.each(pageInstance.aliases, function(n, alias)
        {
          if (data.module === alias)
          {
            found = true;
            return false; // break
          }
        });

        if (!found)
        {
          return true; // continue
        }

        var d = new Date(data.timestamp * 1000);
        data.time = d.toLocaleString();

        if (data.direction === "in" && pageInstance.inText)
        {
          data.direction = pageInstance.inText;
        }
        else if (data.direction === "out" && pageInstance.outText)
        {
          data.direction = pageInstance.outText;
        }

        list.push(data);
      });

      list.sort(function(a, b)
      {
        return b.timestamp - a.timestamp;
      });

      var count = 0;

      jQuery.each(list, function(n, item)
      {
        if (count >= pageInstance.limit)
        {
          return false;
        }
        
        $("#page-phonecalls-item-template").tmpl(item).appendTo(pageInstance.pageContentElement.find("ul"));

        count++;
      });

      pageInstance.pagePhoneCallsElement.trigger("create");
    }

    
    /* Function to request new values from the server */
    function requestServerData()
    {
      sendCommand("Storage_ListParametersRaw PhoneCalls", handleServerData);
    }
    
    
    /* Request the initial value from the server */
    requestServerData();
    
    
    /* Set timer for continious polling of the current values */
    pageInstance.pollTimer = setInterval(requestServerData, 4000);
  }
};
