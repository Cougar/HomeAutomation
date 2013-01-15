
pages.graphs = {
  init: function(pageInstance)
  {
    this.pageshow = function(arg) {
	/* Request the initial value from the server */
      //requestServerData();
      /* Set timer for continious polling of the current values */
      pageInstance.pollTimer = setInterval(reloadImage, 60000);
      //pageInstance.pollTimer2 = setInterval(requestServerData, 2000);
      reloadImage();
    }
    
    this.pagehide = function(arg) {
      clearInterval(pageInstance.pollTimer);
      //clearInterval(pageInstance.pollTimer2);
    }
    
    
    /* List to keep referenses to all the different slider elements */
    pageInstance.pageGraphsElements = {};
    
    /* Function to update values based on new server data */
    /*function handleServerData(jsonData)
    {
      var g1,g2,g3;
      // Loop through all results 
      jQuery.each(jsonData, function(alias, data)
      {
	  // Loop through all values 
	  jQuery.each(data, function(name, value)
	  {
	    if (name == "Power") 
	    {
		g1 = value.value;
	    }
	    if (name == "Power32") 
	    {
		g2 = value.value;
	    }
	    if (name == "EnergySum") 
	    {
		g3 = value.value;
	    }
	  });
      });
      pageInstance.pageInputElements["grid"].html($("#page-Power-grid-template").tmpl({ grid1: "Power:  "+g1, grid2: "Power32:  "+g2,grid3: "EnergySum:  "+g3}));
    }
    */    
    /* Function to request new values from the server */
    /*function requestServerData()
    {
      sendCommand("JSON.stringify(Module_GetLastDataNative('" + pageInstance.alias + "'));", handleServerData);
    } */
    
    function reloadImage()
    {
	     pageInstance.pageImageElements["bilder"].find("img").each(function(){
          $(this).attr('src', pageInstance.pictures[$(this).attr('id')] + '?' + Math.random());
          //alert("ID" + $(this).id);
          });
    }
    
    /*var name = pageInstance.alias;
    
    if (aliasNames[pageInstance.alias])
    {
      name = aliasNames[pageInstance.alias];
    } */
    
    /* Create the Power HTML from the template and add it to the DOM */
   
    //pageInstance.pageInputElements = {};
    pageInstance.pageImageElements = {};
    
    var images = [];
    
    jQuery.each(pageInstance.pictures, function(n, url) {
       images.push({collapsed:!(n==1), url:url, idx:n});
    });
    
    pageInstance.pageImageElements["bilder"] = $("#page-graphs-images-template").tmpl({image: images}).appendTo(pageInstance.pageContentElement).trigger("create");

    //pageInstance.pageInputElements["grid"] = $("#page-Power-grid-template").tmpl({ grid1: "Power:  ", grid2: "Power32:  ",grid3: "EnergySum:  "}).appendTo(pageInstance.pageContentElement).trigger("create");
    
  }
 };
  