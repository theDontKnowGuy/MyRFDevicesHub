<!DOCTYPE html><html lang='en' class=''>
<head>

   <style>
    html,    body {    height: 100%;    }
    body {    margin: 0;    font-size:15px;    background:#49a09d;    font-family: sans-serif;        font-weight: 100;}
    .container {    width:99%;    padding:10px;}
    table {    border-radius:10px;    padding:10px;    width: 99%;    border-collapse: collapse; overflow: hidden;}
    th,    td {    padding: 15px;    background-color: rgba(255,255,255,0.2);    color: #050e58;    }
    th {color:white;    text-align: left;    }
    thead th {    background-color: #55608f;    }
    tbody tr:hover {    background-color: rgba(255, 255, 255, 0.3);    }
    tbody td {    position: relative;    }
	tbody td:hover:before {
content: "";
position: absolute;
left: 0;right: 0;
top: -9999px;bottom: -9999px;
background-color: rgba(255, 255, 255, 0.2);z-index: -1;
}</style>

</head>

<body>

<br><br>
<div style="text-align:center; font-size:25px;">MyRFDevicesHub Log</div><br><br>
<div style="text-align:center; font-size:14px;">To see Logs from other days use the format /MyRFDevicesHub/MyRFDevicesHubLog.php?logdate=29.7.2018</div><br>

<div class="container"><table><thead><tr>
<th>Server time</th><th>Log</th>
</tr></thead>
<tbody>
<tr>
<td>

<?php
$fn = "";

if(isset($_GET['logdate'])) $fn = $_GET['logdate'];

if($fn=="")
        {$filename = '/home/pi/MyRFDevicesHub/log_'.date("j.n.Y").'.log';	}
else
        {$filename = '/home/pi/MyRFDevicesHub/log_'.$fn.'.log';}

$eol = "</td></tr><tr><td>";
$logContent = file_get_contents($filename);
$logContent = str_replace(chr(10),$eol,$logContent);
$logContent = str_replace("#!","</td><td>",$logContent);
echo $logContent;

?>

</td><td>End of log file</td></tr>
</tbody>
</table>
</div>

</body>
</html>

