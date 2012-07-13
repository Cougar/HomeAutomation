
pages.irremotes = {
  init: function(pageInstance)
  {
    pageInstance.pageButtonElements = {};


    jQuery.each(pageInstance.remotes, function(n, remote)
    {
      sendCommand("Storage_ListParametersRaw Remote_" + remote, function(jsonData)
      {
        jQuery.each(jsonData.results, function(button, data)
        {
          pageInstance.pageButtonElements[remote + "_" + button] = $("#page-irremotes-button-template").tmpl({ button: button }).appendTo(pageInstance.pageContentElement).trigger("create").find("button").on("click", function(event)
          {
            jQuery.each(pageInstance.aliases, function(i, alias)
            {
              sendCommand("IROut_SendBurst " + alias + " " + remote + " " + button);
            });
            
            event.preventDefault();
          });
        });
      });
    });
  }
};
