<?php
	include "connect.php";

	$new_script = false;
	if ( !empty($_POST['script_id']) )
		$script_id = $_POST['script_id'];
	else
	{
		$new_script = true;
		$result = mysql_query("SELECT id FROM scripts ORDER BY `id` DESC LIMIT 1");
		$script_id = mysql_fetch_row($result)[0] + 1;
	}
	$script_name = $_POST['script_name'];
	$script_node_id = $_POST['script_node_id'];
	$script_param = $_POST['script_param'];
	$script_value_min = $_POST['script_value_min'];
	$script_value_max = $_POST['script_value_max'];
	$script_active = $_POST['script_active'];
	$actions = json_decode($_POST['actions']);

	if ( !$new_script )
	{
		$result = mysql_query("DELETE FROM scripts_table WHERE `script_id` = ".$script_id);
		$result = mysql_query("UPDATE scripts SET `full_name`='".$script_name."', `node_id`='".$script_node_id."',
												`param`='".$script_param."', `value_min`='".$script_value_min."',
												`value_max`='".$script_value_max."', `active`='".$script_active."'
											WHERE `id` = ".$script_id);
	}
	else
		$result = mysql_query("INSERT INTO scripts (`id`, `full_name`, `node_id`, `param`, `value_min`, `value_max`, `active`)
											VALUES ('".$script_id."', '".$script_name."', '".$script_node_id."', '".$script_param."', 
													'".$script_value_min."', '".$script_value_max."', '".$script_active."')");

	for ( $i=0; $i<count($actions); $i++ )
	{
		$result = mysql_query("INSERT INTO scripts_table (`script_id`, `order`, `action_id`, `value1`, `value2`)
													VALUES ('".$script_id."', '".($i+1)."', '".$actions[$i][0]."', 
															'".$actions[$i][1]."', '".$actions[$i][2]."')");
	}
	mysql_close();
	$str = $new_script?"|".$script_id:"";
	echo "Изменения сохранены".$str;
?>
