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
    </style>

    <?php
      foreach ($pages as $page)
      {
        include("pages/" . $page . "/template.html");
      }
    ?>

    <script id="page-template" type="text/x-jquery-tmpl" data-enhance="false">
      <div id="page-${name}" data-role="page" data-add-back-btn="true" data-theme="a">
        <div data-role="header"><h1>Home Automation Control Center - ${title}</h1></div>
        <div data-role="content"></div>
      </div>
    </script>

    <script id="pagelink-template" type="text/x-jquery-tmpl" data-enhance="false">
      <span>
        <a href="#pagelink?page=${name}" data-role="button">${title}</a>
      </span>
    </script>

    <script>
      $.mobile.ignoreContentEnabled = true;

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

      var pageConfigurations = <?php echo json_encode($pageConfiguration); ?>;
      var pageInstances = {};

      $(document).ready(function(event)
      {
        jQuery.each(pageConfigurations, function(n, configuration)
        {
          var pageName = configuration.page + "_" + n;

          pageInstances[pageName] = configuration;

          pageInstances[pageName].name = pageName;

          pageInstances[pageName].pageLinkElement = $("#pagelink-template").tmpl(pageInstances[pageName]).appendTo($("#home").find(":jqmData(role=content)")).trigger("create");
          pageInstances[pageName].pageElement = $("#page-template").tmpl(pageInstances[pageName]).appendTo($("body")).page();
          pageInstances[pageName].pageContentElement = pageInstances[pageName].pageElement.find(":jqmData(role=content)");

          if (pages[configuration.page] && pages[configuration.page].init)
          {
            pages[configuration.page].init(pageInstances[pageName]);
          }
        });
      });

      $(document).bind("pagebeforechange", function(e, data)
      {
        if (typeof data.toPage === "string")
        {
          var u = $.mobile.path.parseUrl(data.toPage);
          var re = /^#pagelink/;

          if (u.hash.search(re) !== -1)
          {
            var pageName = u.hash.replace(/.*page=/, "");
            var page = $("#page-" + pageName);

            if (page)
            {
              data.options.dataUrl = u.href;

              $.mobile.changePage(page, data.options);
            }

            e.preventDefault();
          }
        }
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
