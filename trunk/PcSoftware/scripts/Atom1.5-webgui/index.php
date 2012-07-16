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
      <div id="page-${name}" data-role="page" data-theme="a">
        <div data-role="header">
          <a href="#" data-icon="home" data-rel="back" data-direction="reverse">Home</a>
          <h1>Home Automation Control Center - ${title}</h1>
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
      $.mobile.transitionFallbacks.pop = "none"
      $.mobile.transitionFallbacks.flip = "none"
      $.mobile.transitionFallbacks.turn = "none"
      $.mobile.transitionFallbacks.flow = "none"
      $.mobile.transitionFallbacks.slidefade = "none"
      $.mobile.transitionFallbacks.slide = "none"
      $.mobile.transitionFallbacks.slideup = "none"
      $.mobile.transitionFallbacks.slidedown = "none"
      $.mobile.defaultPageTransition = "none"

      function sendCommand(command, callback)
      {
        jQuery.getJSON("backend.php?ajax", { command: command }, callback);
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
      var pageInstances = {};

      $(document).ready(function(event)
      {
        jQuery.each(pageConfigurations, function(n, configuration)
        {
          var pageName = configuration.page + "_" + n;

          pageInstances[pageName] = configuration;

          pageInstances[pageName].name = pageName;

          pageInstances[pageName].pageElement = $("#page-template").tmpl(pageInstances[pageName]).appendTo($("body")).page();
          pageInstances[pageName].pageContentElement = pageInstances[pageName].pageElement.find(":jqmData(role=content)");

          pageInstances[pageName].pageElement.find(":jqmData(rel=back)").on("click", function(event)
          {
            $.mobile.changePage($("#home"), "slide", true);
            event.preventDefault();
          });

          pageInstances[pageName].pageLinkElement = $("#pagelink-template").tmpl(pageInstances[pageName]).appendTo($("#home").find(":jqmData(role=content)")).trigger("create").on("click", function(event)
          {
            $.mobile.changePage(pageInstances[pageName].pageElement, "slide");
            event.preventDefault();
          });


          if (pages[configuration.page] && pages[configuration.page].init)
          {
            pages[configuration.page].init(pageInstances[pageName]);
          }
        });

        $(".ui-page").on("swipeleft", function()
        {
          var nextpage = $(this).nextAll(":jqmData(role=page)");

          if (nextpage.length > 0)
          {
            $.mobile.changePage($(nextpage[0]), "slide");
          }
        });
        
        $(".ui-page").on("swiperight", function()
        {
          var prevpage = $(this).prevAll(":jqmData(role=page)");

          if (prevpage.length > 0)
          {
            $.mobile.changePage($(prevpage[0]), "slide", true);
          }
        });
      });

    </script>
  </head>

  <body>
    <div id="home" data-role="page" data-theme="a">
      <div data-role="header"><h1>Home Automation Control Center</h1></div>
      <div data-role="content"></div>
    </div>
  </body>
</html>
