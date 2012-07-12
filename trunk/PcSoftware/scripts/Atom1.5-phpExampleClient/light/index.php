<?php
$aliasesDimmer = array("TakSovrum", "bardisk", "bardisk_tak", "bardisk_tv");
$aliasesPower = array("Power", "PowerBRF");

require 'atom_interface.php';
require 'getSetData.php';

$response = array();

if (isset($_GET["DimmerStrength"]))
{
  if (intval($_GET["strength"]) < 10)
  {
    $_GET["strength"] = 0;
  }
  
  SetDimmerValue($_GET["alias"], $_GET["strength"]);
  
  echo json_encode($response);
  exit(0);
}
else if (isset($_GET["getvalue"]))
{
  foreach ($aliasesDimmer as $alias)
  {
    $response[$alias] = GetLastValue($alias);
  }
  
  echo json_encode($response);
  exit(0);
}
else if (isset($_GET["getdata"]))
{
  foreach ($aliasesDimmer as $alias)
  {
    $response[$alias] = GetLastData($alias);
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
<?
//  <link rel="stylesheet"  href="jquery.mobile-1.0b1.css" /> 
//  <script src="jquery-1.6.1.min.js"></script> 
//  <script src="jquery.mobile-1.0b1.min.js"></script> 
?>
  <script>

    $(document).bind("pagecreate", function(event, ui)
    {
      <?
      foreach ($aliasesDimmer as $alias)
      {
      ?>
        $('#slider<?=$alias?>' ).bind('change', function(event, ui){ makeAjaxChange("<?=$alias?>"); });
      <?
      }
      ?>
      delayed_start_of_poll();
    });

    var current_value = {};
    var next_value = {};
    
    <?
    foreach ($aliasesDimmer as $alias)
    {
    ?>
      current_value['<?=$alias?>'] = 0;
      next_value['<?=$alias?>'] = -1;
    <?
    }
    ?>
    var updating_current = false;
    var timer = null;

    function checkCurrentValue()
    {
      updating_current = true;
      jQuery.getJSON("?getvalue=1", "", function(data)
      {
        
        
        jQuery.each(data, function(alias, value)
        {
          current_value[alias] = value;
          $('#slider' + alias).val(value).slider("refresh");
        });
        
        updating_current = false;
      });
      timer = setTimeout(checkCurrentValue, 1000);
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
	  <?
	  foreach ($aliasesDimmer as $alias)
	  {
	  ?>
	  <div data-role="fieldcontain" id="slider-container">
	    <label for="slider<?=$alias?>"><?=ucfirst($alias)?>:</label>
	    <input type="range" name="slider<?=$alias?>" id="slider<?=$alias?>" value="<?=intval($level)?>" min="0" max="255"  />
	  </div>

	  <div style="">

	  </div>
	  <?
	  }
	  ?>
	</div>
      </div>
      <div data-role="collapsible-set" data-theme="" data-content-theme="">
	<div data-role="collapsible" data-collapsed="true">
	  <h3>
	    Power
	  </h3>
	  <?
	  foreach ($aliasesPower as $alias)
	  {
	  ?>
	  <div data-role="fieldcontain" id="slider-container">
	    <label for="slider<?=$alias?>"><?=ucfirst($alias)?>:</label>
	  </div>

	  <div style="">

	  </div>
	  <?
	  }
	  ?>
	</div>
      </div>
      <div data-role="collapsible-set" data-theme="" data-content-theme="">
	<div data-role="collapsible" data-collapsed="true">
	  <h3>
	    Ute temperatur
	  </h3>
<?
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