
pages.buttons = {
  init: function(pageInstance)
  {
    pageInstance.pageButtonElements = {};


    jQuery.each(pageInstance.buttons, function(button, command)
    {
      pageInstance.pageButtonElements[button] = $("#page-buttons-button-template").tmpl({ button: button }).appendTo(pageInstance.pageContentElement).trigger("create").find("button").on("click", function(event)
      {
        sendCommand(command);

        event.preventDefault();
      });
    });
  }
};
