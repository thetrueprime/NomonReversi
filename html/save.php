 <?php
$dir = $_POST["directory"];

 // create new directory with 744 permissions if it does not exist yet
 // owner will be the user/group the PHP script is run under
 if ( !file_exists($dir) ) {
     $oldmask = umask(0);  // helpful when used in linux server  
     mkdir ($dir, 0744);
 }

$filename =$dir.$_POST["name"].$_POST["gamesettings"].".txt";
$txt = $_POST["data"];
echo "wrote ".$txt." to ".$filename;
$myfile = fopen($filename, "w") or die("Unable to open file!");
fwrite($myfile, $txt);
fclose($myfile);
?> 
