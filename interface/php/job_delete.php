<?php
include "connect.php";

if ( !empty($_POST['id']) )
{
	$id = $_POST['id'];
	$result = mysql_query("DELETE FROM jobs_table WHERE `sched_id` = ".$id);
	$result = mysql_query("DELETE FROM scheduler WHERE `id` = ".$id);
	echo "Задание удалено";
}
mysql_close();
?>