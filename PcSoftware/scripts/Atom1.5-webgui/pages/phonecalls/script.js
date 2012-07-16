
pages.phonecalls = {
  init: function(pageInstance)
  {
    pageInstance.pagePhoneCallsElement = null;
    pageInstance.lastPhoneCallTimestamp = 0;

    /* Function to update values of sliders based on new server data */
    function handleServerData(jsonData)
    {
      var list = [];

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
          data.directionText = pageInstance.inText;
        }
        else if (data.direction === "out" && pageInstance.outText)
        {
          data.directionText = pageInstance.outText;
        }
        else
        {
          data.directionText = data.direction;
        }

        list.push(data);
      });

      list.sort(function(a, b)
      {
        return b.timestamp - a.timestamp;
      });

      if (list.length > 0 && pageInstance.lastPhoneCallTimestamp === list[0].timestamp)
      {
        return; // Do not update if no change
      }

      pageInstance.pageContentElement.empty();
      pageInstance.pagePhoneCallsElement = $("#page-phonecalls-template").tmpl().appendTo(pageInstance.pageContentElement);

      pageInstance.lastPhoneCallTimestamp = list[0].timestamp;

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

      var timestamp = getServerTimestamp() / 1000;

      if (list.length > 0 && list[0].direction == "in" && list[0].timestamp + 10 > timestamp)
      {
        /* Pause polling timer */
        clearInterval(pageInstance.pollTimer);
        pageInstance.pollTimer = null;

        openDialog("Event", pageInstance.inText + " " + list[0].number + ".<br/>" + list[0].names.toString(), 15000, function()
        {
          pageInstance.pollTimer = setInterval(requestServerData, 4000);
        });
      }
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
