<?php
include "connect.php";

$command_id = $_POST['command_id'];
$value = $_POST['value'];
if ( $command_id == 123 )
{
	$result = mysql_query("SELECT `protocol_id` FROM `nodes` WHERE `node_id` = ".$value);
	$row = mysql_fetch_row($result);
	$str = "smarty-sender -n".$value." -c".$command_id." -p0 -v0 -t".$row[0];
	exec($str);
	echo 0;
}
else
{
	$result = mysql_query("SELECT c.node_id AS node_id, c.value_min AS value_min, c.value_max AS value_max,
						   c.get_set AS get_set, c.param AS param, n.protocol_id AS prot
						   FROM commands c JOIN nodes n ON c.node_id = n.node_id WHERE c.id = ".$command_id);
	$row = mysql_fetch_assoc($result);
	$node_id = $row['node_id'];
	if ( ($value < $row['value_min']) || ($value > $row['value_max']) )
	{
		echo "Значение вне диапазона";
		return 1;
	}
	if ( $row['get_set'] )
		$command = 125;
	else
		$command = 126;
	$str = "smarty-sender -n".$node_id." -c".$command." -p".$row['param']." -v".$value." -t".$row['prot'];
	exec($str);
	echo 0;
}
mysql_close();
?>
