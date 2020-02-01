<?php
include "connect.php";
include "timetable.php";
if ( isset($_POST['id']) )
{
	$id = $_POST['id'];

	$job_name = "";
	$job_command = "";
	$job_active = "";
	$job_desc = "";
	$job_node_id = "";
	$jobs_arr = array();
// Запрашиваем списки узлов и команд
	$result = mysql_query("SELECT node_id, full_name FROM nodes");
	$i = 0;
	while ( $array = mysql_fetch_array($result) )
	{
		$nodes[$i]['node_id'] = $array['node_id'];
		$nodes[$i]['full_name'] = $array['full_name'];
		$i++;
	}
	$num_nodes = count($nodes);

	$result = mysql_query("SELECT id, node_id, name, value_min, value_max, comment FROM commands WHERE id > 0");
	$i = 0;
	while ( $array = mysql_fetch_array($result) )
	{
		$commands[$i]['id'] = $array['id'];
		$commands[$i]['node_id'] = $array['node_id'];
		$commands[$i]['name'] = $array['name'];
		$commands[$i]['value_min'] = $array['value_min'];
		$commands[$i]['value_max'] = $array['value_max'];
		$commands[$i]['comment'] = $array['comment'];
		$i++;
	}
	$num_commands = count($commands);
/////////////////////////////////////
	if ( $id )	// Если не новое задание - загружаем из базы
	{
////// Запрашиваем основные реквизиты задания
		$query = "SELECT s.name AS name, s.command_id AS command_id, s.active AS active,
						 s.description AS description, c.node_id AS job_node_id
						 FROM scheduler s JOIN commands c ON c.id = s.command_id WHERE s.id = $id";
		$result = mysql_query($query);
		$array = mysql_fetch_assoc($result);
		$job_name = $array['name'];
		$job_command = $array['command_id'];
		$job_active = $array['active'];
		$job_desc = $array['description'];
		$job_node_id = $array['job_node_id'];
////////////////////////////////////////////
////// Запрашиваем график работы
		$query = "SELECT starttime, stoptime, startvalue, stopvalue, days
						 FROM jobs_table WHERE sched_id = $id";
		$result = mysql_query($query);
		$i = 0;
		while ( $array = mysql_fetch_assoc($result) )
		{
			$jobs_arr[$i]['starttime'] = $array['starttime'];
			$jobs_arr[$i]['stoptime'] = $array['stoptime'];
			$jobs_arr[$i]['startvalue'] = $array['startvalue'];
			$jobs_arr[$i]['stopvalue'] = $array['stopvalue'];
			$jobs_arr[$i]['days'] = $array['days'];
			$i++;
		}
	}
/////////////// Основные реквизиты /////////////////////////////////////
	$out = "<br><table class='script_header'><tr>";
	$out.= "<th>Название</th>";
	$out.= "<th>Узел</th>";
	$out.= "<th>Команда</th>";
	$out.= "<th>Описание</th>";
	$out.= "<th>Активно</th>";
	$out.= "</tr><tr>";
	$out.= "<td><input type='text' size='30' value='".$job_name."' class='job_name'></td>";
// Вывод узлов
	$out.= "<td><select class='act_select script_node_id' autocomplete='off'>";
	$out.= "<option></option>";
	for ( $i=0; $i<count($nodes); $i++ )
	{
		if ($i)
		{
			if ( $nodes[$i]['node_id'] == $job_node_id )
				$str = "selected";
			else
				$str = "";
			$out.= "<option $str value=\"".$nodes[$i]['node_id']."\">".$nodes[$i]['full_name']."</option>";
		}
	}
	$out.= "</select></td>";
////////////
// Вывод команд
	$out.= "<td><select class='script_param' autocomplete='off'>";
	$out.= "<option value='0' class='key_0'></option>";
	for ( $i=0; $i<$num_commands; $i++ )
	{
		if ( ($commands[$i]['node_id'] == $job_node_id) && ($commands[$i]['id'] == $job_command) )
			$str = "selected";
		else
			$str = "";
		$out.= "<option ".$str." value='".$commands[$i]['id']."' class='key_".$commands[$i]['node_id']."'>".$commands[$i]['name']."</option>";
	}
	$out.= "</select><br>";
	for ( $i=0; $i<$num_commands; $i++ )
	{
		$out.= "<div class='com_key com_key_".$commands[$i]['id']."'>";
			$out.= "<div>Знач.: </div>";
			$out.= "<div class='norm_value_min'>".$commands[$i]['value_min']."</div>";
			$out.= "<div>&nbsp-&nbsp</div>";
			$out.= "<div class='norm_value_max'>".$commands[$i]['value_max']."</div>";
		$out.= "</div>";
	}
	$out.= "</td>";
//////////////
	$out.= "<td><textarea class='job_desc' rows=2>".$job_desc."</textarea></td>";
// Кнока активизации
	if ( $job_active )
		$str = "checked";
	else
		$str = "";
	$out.= "<td><input type='checkbox' class='script_active' $str></td>";
////////////////////
	$out.= "</tr></table>";
////////////////////////////////////////////////////////////////////////////////////////////////////
// Отрисовка графика работы
	for ( $i=0; $i<$num_commands; $i++ )
		$out.= "<div id='com_desc' class='com_key com_key_".$commands[$i]['id']."'><i class='fa fa-info-circle' aria-hidden='true'></i>&nbsp".$commands[$i]['comment']."</div>";
	$out.= "<h2 align='center'>График работы</h2>";
	$out.= "<div id='timetable'>";
	$out.= putTimetable(); // hidden
	for ( $i=0; $i<count($jobs_arr); $i++ )
		$out.= putTimetable($jobs_arr[$i]);
	$out.= "<button class='button add_time' type='button'><i class='fa fa-plus'></i></button>";
	$out.= "</div>";
///////////////////////////
	echo $out;
}
mysql_close();
?>