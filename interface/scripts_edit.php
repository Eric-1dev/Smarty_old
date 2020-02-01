<script src="js/scripts.js"></script>
<title>Редактирование скрипта</title>

<?php
include "php/redirect.php";
include "php/connect.php";
include "php/action_args.php";

$script_id = "";
$script_name = "";
$script_node_id = "";
$script_param = "";
$script_value_min = "";
$script_value_max = "";
$script_active = 0;
$script_actions = array();
if ( isset($_GET['script']) )
{
	$script_id = $_GET['script'];

	$result = mysql_query("SELECT * FROM scripts WHERE id = $script_id");
	$array = mysql_fetch_assoc($result);
	$script_name = $array['full_name'];
	$script_node_id = $array['node_id'];
	$script_param = $array['param'];
	$script_value_min = $array['value_min'];
	$script_value_max = $array['value_max'];
	$script_active = $array['active'];

	$i=0;
	$result = mysql_query("SELECT action_id, value1, value2 FROM scripts_table WHERE script_id = $script_id ORDER BY `order` ASC");
	while ( $array = mysql_fetch_array($result) )
	{
		$script_actions[$i]['action_id'] = $array['action_id'];
		$script_actions[$i]['value1'] = $array['value1'];
		$script_actions[$i]['value2'] = $array['value2'];
		$i++;
	}
}
$out = "<div style='float: right;'><button class='button but_save'><i class='fa fa-save'></i></button></div>";
$out.= "<h1 align='center'>Редактирование скрипта&nbsp<input type='text' size='40' value='".$script_name."' class='script_name'></h1>";
$out.= "<input type='hidden' class='script_id' value='".$script_id."'>";

$out.= "<table class='script_header'><tr>";
$out.= "<th>Инициатор скрипта</th>";
$out.= "<th>Показатель</th>";
$out.= "<th>Мин. значение</th>";
$out.= "<th>Макс. значение</th>";
$out.= "<th>Статус</th></tr><tr>";
$out.= "<td><select class='act_select script_node_id' autocomplete='off'>";
$out.= "<option></option>";
for ( $i=0; $i<count($nodes); $i++ )
{
	if ($i)
	{
		if ( $nodes[$i]['node_id'] == $script_node_id )
			$str = "selected";
		else
			$str = "";
		$out.= "<option $str value=\"".$nodes[$i]['node_id']."\">".$nodes[$i]['full_name']."</option>";
	}
}
$out.= "</select></td>";

$out.= "<td><select class='script_param' autocomplete='off'>";
for ($i=0; $i<$num_commands; $i++)
{
	if ( ($commands[$i]['get_set'] == 1) )
	{
		if ( ($commands[$i]['node_id'] == $script_node_id) && ($commands[$i]['param'] == $script_param) )
			$str = "selected";
		else
			$str = "";
		$out.= "<option ".$str." value='".$commands[$i]['param']."' class='key_".$commands[$i]['node_id']."'>".$commands[$i]['name']."</option>";
	}
}
$out.= "</select></td>";
$out.= "<td><input type='text' size='3' class='value_text script_value_min' value='".$script_value_min."'></td>";
$out.= "<td><input type='text' size='3' class='value_text script_value_max' value='".$script_value_max."'></td>";
if ( $script_active )
	$str = "checked";
else
	$str = "";
$out.= "<td><input type='checkbox' class='script_active' $str></td>";
$out.= "</tr></table>";

$out.= "<table class='script_main_table'><thead><tr>";
$out.= "<th><div class='div_th'>Состав скрипта</div></th>";
$out.= "<th></th>";
$out.= "<th><div class='div_th'>Доступные действия</div></th>";
$out.= "</tr></thead>";
$out.= "<tbody><tr><td><div class='div_left_td'>";
$out.= "<div align='center'><button class='button up_down but_up fa fa-arrow-up fa-lg'></button>";
$out.= "<button class='button up_down but_down fa fa-arrow-down fa-lg'></button></div>";
for ( $i=0; $i<count($script_actions); $i++ )
{
	$out.= "<div class='div_action'>";
	$out.= putArguments($script_actions[$i]['action_id'], $script_actions[$i]['value1'], $script_actions[$i]['value2']);
	$out.= "</div>";
}
$out.= "</div></td><td><div class='div_middle_td'>";
$out.= "<button class='button move_but'><i class='fa fa-arrow-left'></i><i class='fa fa-arrow-right'></i></button>";
$out.= "</div></td><td><div class='div_right_td'>";
for ( $i=1; $i<7; $i++ )
{
	$out.= "<div class='div_action'>";
	$out.= putArguments($i, '', '');
	$out.= "</div>";
}
$out.= "</div></td></tr></tbody>";
$out.= "</table>";
mysql_close();
echo $out;
?>

