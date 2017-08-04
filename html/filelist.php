 <?php
$directory = "/var/www/html/";

$dir = opendir($directory);

$structure = array();
$circles = 0;
$pies = 0;
$squares = 0;

while($file = readdir($dir)){
  $structure[] = $file;
	
}
foreach ($structure as $value){
	//echo "Value: $value<br/>\n";
	if(strpos($value, 'expanding') !== false){
		$circles++;
	}
	if(strpos($value, 'pie') !== false){
		$pies++;
	}
	if(strpos($value, 'square') !== false){
		$squares++;
	}
}
if(abs($pies-$circles)>10 or abs($pies-$squares) >10 or abs($circles-$squares)>10){
	if($circles>$pies){
		if($pies>$squares){
			echo "squares";
		}else{
			echo "pies";
		}
	}else{
		if($circles>$squares){
			echo "squares";
		}else{
			echo "circles";
		}
	}
}else{
	echo "none";	
}
echo abs($pies-$circles) ." ". abs($pies-$squares) ." ". abs($circles-$squares);
echo "Circle:".$circles." Pie:".$pies." Square:".$squares;
//print json_encode($structure);
exit();
?> 
