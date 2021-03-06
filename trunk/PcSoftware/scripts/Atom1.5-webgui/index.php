<?php
require_once("backend.php");
require_once("config.php");
?>
<!DOCTYPE HTML>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Home Automation Control Center</title>
    <link rel="stylesheet" href="jquery.mobile-1.1.1.min.css" />
    <script src="jquery-1.7.2.min.js"></script>
    <script src="jquery.timeago.js"></script>
    <script src="jquery.mobile-1.1.1.min.js"></script>
    <script src="jquery.tmpl.min.js"></script>

    <style>
    <?php
      foreach ($pages as $page)
      {
        include("pages/" . $page . "/style.css");
      }
    ?>

      .ui-li-heading {
        white-space: normal;
      }
    </style>

    <?php
      foreach ($pages as $page)
      {
        include("pages/" . $page . "/template.html");
      }
    ?>

    <script id="page-template" type="text/x-jquery-tmpl" data-enhance="false">
      <div id="page-${name}" data-role="page" data-theme="a" id="page-{name}">
        <div data-id="PersistentFooter" data-role="header" data-position="fixed" data-tap-toggle="false">
          <div data-role="navbar">
            <ul class="navbar-container"></ul>
          </div>
        </div>
        <div data-role="content"></div>
      </div>
    </script>

    <script id="pagelink-template" type="text/x-jquery-tmpl" data-enhance="false">
      <span>
        <a href="#" data-role="button">${title}</a>
      </span>
    </script>

    <script>
      $.mobile.ignoreContentEnabled = true;
      $.mobile.transitionFallbacks.pop = "none";
      $.mobile.transitionFallbacks.flip = "none";
      $.mobile.transitionFallbacks.turn = "none";
      $.mobile.transitionFallbacks.flow = "none";
      $.mobile.transitionFallbacks.slidefade = "none";
      $.mobile.transitionFallbacks.slide = "none";
      $.mobile.transitionFallbacks.slideup = "none";
      $.mobile.transitionFallbacks.slidedown = "none";
      $.mobile.defaultPageTransition = "none";
      $.mobile.ajaxEnabled = false;

      var commandActive = false;
      var commandQueue = [];

      function sendCommand(command, callback)
      {
        if (commandActive)
        {
          commandQueue.push({ command: command, callback: callback });
        }
        else
        {
          commandActive = true;

          jQuery.getJSON("backend.php?ajax", { command: command }, function(jsonData)
          {
            handleCommandResponse(true, jsonData, callback);
          }).error(function()
          {
            handleCommandResponse(false, {}, callback);
          });
        }
      }

      function handleCommandResponse(success, jsonData, callback)
      {
        commandActive = false;

        if (commandQueue.length > 0)
        {
          var commandItem = commandQueue.shift();

          sendCommand(commandItem.command, commandItem.callback);
        }

        if (success)
        {
          if (callback)
          {
            callback(jsonData);
          }
        }
        else
        {
          console.log("command failed!");
        }
      }
    </script>

    <script>
      var pages = {};

    <?php
      foreach ($pages as $page)
      {
        include("pages/" . $page . "/script.js");
      }
    ?>
    </script>

    <script>

      var aliasNames = <?php echo json_encode($aliasNames); ?>;
      var pageConfigurations = <?php echo json_encode($pageConfiguration); ?>;
      var pageInstances = { };
      var serverTimeOffset = 0;

      function getServerTimestamp()
      {
        return ((new Date()).getTime()) - serverTimeOffset
      }

      $(document).ready(function(event)
      {
        sendCommand("(new Date()).getTime();", function(timestamp)
        {
          serverTimeOffset = ((new Date()).getTime()) - parseInt(timestamp, 10);
        });

        jQuery.each(pageConfigurations, function(n, configuration)
        {
          var pageName = configuration.page + "_" + n;

          pageInstances[pageName] = configuration;

          pageInstances[pageName].name = pageName;

          pageInstances[pageName].pageElement = $("#page-template").tmpl(pageInstances[pageName]).appendTo($("body")).page();
          pageInstances[pageName].pageContentElement = pageInstances[pageName].pageElement.find(":jqmData(role=content)");

          if (pages[configuration.page] && pages[configuration.page].init)
          {
            pages[configuration.page].init(pageInstances[pageName]);
          }
        });

        
        jQuery.each(pageInstances, function(pageName, pageInstance)
        {
          var navbarContainerElement = pageInstances[pageName].pageElement.find("ul.navbar-container");

          jQuery.each(pageInstances, function(pageName2, pageInstance2)
          {
            if (pageInstance === pageInstance2)
            {
              $("<li class=''><a href='#page-" + pageName2 + "' class='ui-btn-active ui-state-persist'>" + pageInstance2.title + "</a></li>").appendTo(navbarContainerElement);
            }
            else
            {
              $("<li class=''><a href='#page-" + pageName2 + "'>" + pageInstance2.title + "</a></li>").appendTo(navbarContainerElement);
            }
          });
        });

        $(":jqmData(role=header)").navbar();

        
        /*$(".ui-page").on("swipeleft", function()
        {
          var nextpage = $(this).nextAll(":jqmData(role=page)");

          if (nextpage.length > 0)
          {
            $.mobile.changePage("#" + nextpage[0].id, { transition: "none", changeHash: true });
          }
          else
          {
            nextpage = $(":jqmData(role=page)");

            $.mobile.changePage("#" + nextpage[0].id, { transition: "none", changeHash: true });
          }
        });
        
        $(".ui-page").on("swiperight", function()
        {
          var prevpage = $(this).prevAll(":jqmData(role=page)");

          if (prevpage.length > 0)
          {
            $.mobile.changePage("#" + prevpage[0].id, { transition: "none", reverse: true, changeHash: true });
          }
          else
          {
            prevpage = $(":jqmData(role=page)");

            $.mobile.changePage("#" + prevpage[prevpage.length - 1].id, { transition: "none", reverse: true, changeHash: true });
          }
        });*/

	$(".ui-page").on("pagebeforeshow", function(arg1, arg2)
        {
	    var res=arg1.currentTarget.id.split("-");
	    if (res[1] != null) {
		res=res[1].split("_");	    
		if (pages[res[0]].pageshow) {
		    pages[res[0]].pageshow();
		}
	    }
        });

	$(".ui-page").on("pagehide", function(arg1, arg2)
        {
	    
	    var res=arg1.currentTarget.id.split("-");
	    if (res[1] != null) {
		res=res[1].split("_");	    
		if (pages[res[0]].pagehide) {
		    pages[res[0]].pagehide();
		}
	    }
        });

        $(":jqmData(role=navbar)").delegate("a", "vclick", function(event)
        {
          if(!$(event.target).hasClass("ui-disabled"))
          {
            $(":jqmData(role=navbar)").find("a").not(".ui-state-persist").removeClass($.mobile.activeBtnClass);
            $(this).addClass($.mobile.activeBtnClass);
          }
        });

        if (document.location.hash === "" || document.location.hash === "#")
        {
          document.location.hash = "#" + $(":jqmData(role=page)").attr("id");
        }

        $.mobile.changePage($(document.location.hash));

      });

      var dialogCloseTimer = null;
      var dialogCloseCallback = null;

      function openDialog(titleText, contentsHtml, autoCloseTimeout, closeCallback)
      {
        closeDialog();

        $("#dialog .dialog-title").text(titleText);
        $("#dialog .dialog-contents").html(contentsHtml);
        $("#dialog a").on("click", function(event)
        {
          closeDialog();
          event.preventDefault();
          return false;
        });
      
        dialogCloseCallback = closeCallback;

        $.mobile.changePage($("#dialog"), { role: "dialog"});

        $("#dialog").dialog({ close: function(event)
        {
          var callback = dialogCloseCallback;

          closeDialog();

          if (callback)
          {
            callback();
          }
        }});


        if (autoCloseTimeout)
        {
          dialogCloseTimer = setTimeout(closeDialog, autoCloseTimeout);
        }
      }

      function closeDialog()
      {
        if (dialogCloseTimer)
        {
          clearTimeout(dialogCloseTimer);
          dialogCloseTimer = null;
        }

        if ($("#dialog").is(":visible"))
        {
          $("#dialog").dialog("close");

          if (dialogCloseCallback)
          {
            dialogCloseCallback();
          }
        }

        dialogCloseCallback = null;
      }


    </script>
  </head>

  <body>
    <div data-role="dialog" id="dialog">
      <div data-role="content">
        <h3 class="dialog-title"></h3>
        <p class="dialog-contents"></p>
        <a href="#" data-role="button" data-theme="c">Close</a>
      </div>
    </div>

  </body>
</html>
