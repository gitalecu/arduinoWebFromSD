<?php

$dayz = time();
$aa = (($_GET['Temperature']));
$ab = (($_GET['Setpoint']));
$ac = (($_GET['Input']));
$ad = (($_GET['Kp']));
$ae = (($_GET['Ki']));
$af = (($_GET['Kd']));
$ag = (($_GET['Heater']));
$db="example_data";
$link = mysql_connect('', 'userid_1', 'userid1_password');
if (! $link) die(mysql_error());
mysql_select_db($db , $link) or die("Couldn't open $db: ".mysql_error());

$queryResult = mysql_query("INSERT INTO temp (Time, Temp, Setpoint, Input, Kp, Ki, Kd, Heat) VALUES ('$dayz', '$aa', '$ab', '$ac', '$ad', '$ae', '$af', '$ag')");

?>

- See more at: http://fermentationriot.com/arduinopid.php#sthash.PNFZ8nMW.dpuf
