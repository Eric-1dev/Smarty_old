<?php
// Запрашиваем списки действий, узлов, команд
	$actions = array();
	$nodes = array();
	$commands = array();
	$num_actions = 0;
	$num_nodes = 0;
	$num_commands = 0;
	$result = mysql_query("SELECT id, name FROM actions");
	while ( $array = mysql_fetch_array($result) )
		$actions[$array['id']] = htmlspecialchars($array['name']);
	$num_actions = count($actions);

	$result = mysql_query("SELECT node_id, full_name FROM nodes");
	$i = 0;
	while ( $array = mysql_fetch_array($result) )
	{
		$nodes[$i]['node_id'] = $array['node_id'];
		$nodes[$i]['full_name'] = $array['full_name'];
		$i++;
	}
	$num_nodes = count($nodes);

	$result = mysql_query("SELECT id, node_id, name, get_set, param FROM commands");
	$i = 0;
	while ( $array = mysql_fetch_array($result) )
	{
		$commands[$i]['id'] = $array['id'];
		$commands[$i]['node_id'] = $array['node_id'];
		$commands[$i]['name'] = $array['name'];
		$commands[$i]['get_set'] = $array['get_set'];
		$commands[$i]['param'] = $array['param'];
		$i++;
	}
	$num_commands = count($commands);
// ------------------------------------------

function putArguments($actionId, $value1, $value2)
{
	global $actions, $nodes, $commands;
	global $num_actions, $num_nodes, $num_commands;

	$out = $actions[$actionId];
	switch ($actionId)
	{
		case 1:
			$cur_node = 0;
			$tmp = "<select class='value1' autocomplete='off'>";
			for ($i=0; $i<$num_commands; $i++)
			{
				if ( $commands[$i]['get_set'] == 0 )
				{
					if ( $commands[$i]['id'] == $value1 )
					{
						$str = "selected";
						$cur_node = $commands[$i]['node_id'];
					}
					else
						$str = "";
					$tmp.= "<option $str value='".$commands[$i]['id']."' class='key_".$commands[$i]['node_id']."'>".$commands[$i]['name']."</option>";
				}
			}
			$tmp.= "</select>";

			$out.= "<input type='hidden' class='actionId' value='".$actionId."'>";
			$out.= "<select class='act_select' autocomplete='off'>";
			$out.= "<option></option>";
			for ($i=0; $i<$num_nodes; $i++)
			{
				if ( $nodes[$i]['node_id'] )
				{
						if ( $nodes[$i]['node_id'] == $cur_node )
							$str = "selected";
						else
							$str = "";
					$out.= "<option $str value='".$nodes[$i]['node_id']."'>".$nodes[$i]['full_name']."</option>";
				}
			}
			$out.= "</select><br>";

			$out.= $tmp;
			$out.= "<input type='text' size='3' value='$value2' class='value_text value2'>";
			return $out;
		case 3:
		case 4:
		case 5:
		case 6:
			$out.= "<input type='hidden' class='actionId' value='".$actionId."'>";
			$out.= "<input type='text' size='3' value='$value1' class='value_text value1'>";
			$out.= "<input type='hidden' class='value2' value='0'>";
			return $out;
		case 2:
			$out.= "<input type='hidden' class='actionId' value='".$actionId."'>";
			$out.= "<select class='act_select value1' autocomplete='off'>";
			$out.= "<option></option>";
			for ($i=0; $i<$num_nodes; $i++)
			{
				if ( $nodes[$i]['node_id'] )
				{
					if ( $nodes[$i]['node_id'] == $value1 )
						$str = "selected";
					else
						$str = "";
					$out.= "<option $str value='".$nodes[$i]['node_id']."'>".$nodes[$i]['full_name']."</option>";
				}
			}
			$out.= "</select><br>";

			$out.= "<select class='value2' autocomplete='off'>";
			for ($i=0; $i<$num_commands; $i++)
			{
				if ( ($commands[$i]['get_set'] == 1) )
				{
					if ( ($commands[$i]['node_id'] == $value1) && ($commands[$i]['param'] == $value2) )
						$str = "selected";
					else
						$str = "";
					$out.= "<option $str value='".$commands[$i]['param']."' class='key_".$commands[$i]['node_id']."'>".$commands[$i]['name']."</option>";
				}
			}
			$out.= "</select>";
			return $out;
	}
}
?>
