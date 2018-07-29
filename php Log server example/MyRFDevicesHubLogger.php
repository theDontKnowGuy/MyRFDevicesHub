<?php
    $msg = $_POST['msg'];
    $arMsg = ( explode( '|', $msg ) );		
    foreach ($arMsg as &$value) {
	if (strlen($value)>3){
	$log  = date("Ymd His ")."#!".$value.PHP_EOL;
    	file_put_contents('/home/pi/MyRFDevicesHub/log_'.date("j.n.Y").'.log', $log, FILE_APPEND);
	}}
	unset($value); // break the reference with the last element
?>

