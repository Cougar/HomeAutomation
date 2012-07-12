<?php
$aliasesDimmer = array("bardisk_tv", "TakSovrum");
$aliasesOther = array("Power", "PowerBRF", "PID");

require 'atom_interface.php';
require 'getSetData.php';

$response = array();

if (isset($_GET["DimmerStrength"]))
{
  if (intval($_GET["DimmerStrength"]) < 10)
  {
    $_GET["DimmerStrength"] = 0;
  }
  SetDimmerValue($_GET["alias"], $_GET["DimmerStrength"]);
  echo json_encode($response);
  exit(0);
}
else if (isset($_GET["getdata"]))
{
  foreach ($_GET["alias"] as $alias)
  {
    $responseData = GetLastData($alias);

    foreach ($responseData as $name => $value)
    {
      $response[$name] = $value;
    }
  }
  
  echo json_encode($response);
  exit(0);
}
?>


<!DOCTYPE html> 
<html> 
<head> 
  <meta charset="utf-8"> 
  <meta name="viewport" content="width=device-width, initial-scale=1"> 
  <title>Light switch</title> 
<link rel="stylesheet" href="http://code.jquery.com/mobile/1.1.0/jquery.mobile-1.1.0.min.css" />
	<script src="http://code.jquery.com/jquery-1.7.1.min.js"></script>
	<script src="http://code.jquery.com/mobile/1.1.0/jquery.mobile-1.1.0.min.js"></script>
<?php
//  <link rel="stylesheet"  href="jquery.mobile-1.0b1.css" /> 
//  <script src="jquery-1.6.1.min.js"></script> 
//  <script src="jquery.mobile-1.0b1.min.js"></script> 
?>
  <script>

    $(document).bind("pagecreate", function(event, ui)
    {
      <?php
      foreach ($aliasesDimmer as $alias)
      {
      ?>
        $('#slider<?php  echo $alias?>' ).bind('change', function(event, ui){ makeAjaxChange("<?php  echo $alias?>"); });
      <?php
      }
      ?>
      delayed_start_of_poll();
    });

    var current_value = {};
    var next_value = {};
    
    <?php
    foreach ($aliasesDimmer as $alias)
    {
    ?>
      current_value['<?php echo $alias?>'] = 0;
      next_value['<?php echo $alias?>'] = -1;
    <?php
    }
    ?>
    var updating_current = false;
    var timer = null;

    function checkCurrentValue()
    {
      updating_current = true;
      jQuery.getJSON("?getdata=1", "<?php echo "alias[]=" . implode("&alias[]=", $aliasesDimmer)?>", function(data)
      {
        
        
        jQuery.each(data, function(alias, value)
        {
          current_value[alias] = value.Level.value;
          $('#slider' + alias).val(value.Level.value).slider("refresh");
        });
        
        updating_current = false;
      });
      jQuery.getJSON("?getdata=1", "<?php echo "alias[]=" . implode("&alias[]=", $aliasesOther)?>", function(data)
      {
	  var element = $("#current-information");

	  element.empty();

	  var text = "";

	  jQuery.each(data, function(alias, aliasData)
	  {
	    text += "<b>" + alias + "</b><br/>";

	    jQuery.each(aliasData, function(name, value)
	    {
	      if (typeof value.value == "string")
	      {
		text += name + "=" + value.value + "<br/>";
	      }
	      else
	      {
		text += name + "<br/>";
		jQuery.each(value.value, function(name2, value2)
		{
		  text += "&nbsp;&nbsp;" + name2 + "=" + value2 + "<br/>";
		});
	      }
	    });

	    text += "<br/>";
	  });

	  element.html(text);


      });
      timer = setTimeout(checkCurrentValue, 5000);
    }

    function makeAjaxChange(alias)
    {
      if (updating_current)
      {
        return;
      }
    
      if ($('#slider' + alias).val() == current_value)
      {
        return;
      }
      
      if (next_value[alias] == -1) // Nothing in progress, do a call
      {
        do_call(alias, $('#slider' + alias).val());
      }
      else
      {
        next_value[alias] = $('#slider' + alias).val();
      }
      // console.log($('#slider').val());
    }
    
    function do_call(alias, strength)
    {
      next_value[alias] = strength;
      
      clearInterval(timer);
      timer = null;
      
      jQuery.get("?alias=" + alias + "&DimmerStrength=" + strength, "", function()
      {
        current_value[alias] = strength;
        
        if (next_value[alias] != current_value[alias])
        {
          do_call(alias, next_value[alias]);
        }
        else
        {
          timer = setTimeout(delayed_start_of_poll, 3000);
          next_value[alias] = -1;
        }
      });
    }
    
    function delayed_start_of_poll()
    {
      timer = setTimeout(checkCurrentValue, 1000);
    }

  </script>
  
</head> 
<body> 
  <div data-role="page" id="jqm-home" class="type-home" data-theme="a"> 
    <div data-role="header" data-position="inline">
	    <a href="index.html" data-icon="delete">Cancel</a>
	    <h1>Testar</h1>
	    <a href="index.html" data-icon="check">Save</a>
    </div>
    <div data-role="content"> 
      <div data-role="collapsible-set" data-theme="" data-content-theme="">
	<div data-role="collapsible" data-collapsed="true">
	  <h3>
	    Dimmers
	  </h3>
	  <?php
	  foreach ($aliasesDimmer as $alias)
	  {
	  ?>
	  <div data-role="fieldcontain" id="slider-container">
	    <label for="slider<?php echo $alias?>"><?php echo ucfirst($alias)?>:</label>
	    <input type="range" name="slider<?php echo $alias?>" id="slider<?php echo $alias?>" value="<?php echo intval($level)?>" min="0" max="255"  />
	  </div>

	  <div style="">

	  </div>
	  <?php
	  }
	  ?>
	</div>
      </div>
      <div data-role="collapsible-set" data-theme="" data-content-theme="">
	<div data-role="collapsible" data-collapsed="true">
	  <h3>
	    Power
	  </h3>
	  <?php
	  foreach ($aliasesPower as $alias)
	  {
	  ?>
	  <div data-role="fieldcontain" id="slider-container">
	    <label for="slider<?php echo $alias?>"><?php echo ucfirst($alias)?>:</label>
	  </div>

	  <div style="">

	  </div>
	  <?php
	  }
	  ?>
	</div>
      </div>
      <div data-role="collapsible-set" data-theme="" data-content-theme="">
	<div data-role="collapsible" data-collapsed="true">
	  <h3>
	    Ute temperatur
	  </h3>
	  <div id="current-information">None</div>
<?php
//	  <div style="width: 997px; height: 519px; position: relative; background-color: #fbfbfb; border: 1px solid #b8b8b8;">
//	    <img style=" top: 50%; left: 50%; " src="http://linuxz-home.mine.nu/CANgraph/outside_day.png" />
//	  </div>
?>
	</div>
      </div>
    </div> 
  </div> 
</body> 
</html> 
