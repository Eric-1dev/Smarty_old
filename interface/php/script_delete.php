<?php
include "connect.php";

if ( !empty($_POST['id']) )
{
	$id = $_POST['id'];
	$result = mysql_query("DELETE FROM scripts_table WHERE `script_id` = ".$id);
	$result = mysql_query("DELETE FROM scripts WHERE `id` = ".$id);
	echo "Скрипт удален";
}
mysql_close();
?>
