<?php

include('config.php');

$content = file_get_contents($IRRemotes);
$content = trim($content); // Trim whitespace
$content = rtrim($content, "}"); // Trim trailing }

$content = strstr($content, "[");

$content = "var remotes = " . $content;

echo $content;

?> 
