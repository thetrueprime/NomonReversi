 <?php
$dir = $_POST["directory"];
$filename =$dir.$_POST["name"].".txt";
$myfile = unlink($filename);
echo "deleted ".$txt." to ".$filename;
?> 
