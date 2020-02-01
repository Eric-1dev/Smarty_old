<script src="js/status.js"></script>
<title>Состояние системы</title>

<?php
include "php/redirect.php";
include "php/connect.php";

$cur_node = -1;

$out = "<div style='float: left'><button class='button but_update' type='button'><i class='fa fa-refresh'></i></button></div>";
$out.= "<h1 align='center'>Текущие показатели системы</h1>";
$result = mysql_query("SELECT status.name AS name, nodes.node_id AS node_id, nodes.full_name AS full_name, status.param AS param,
							status.value AS value, status.multiplier AS multiplier, status.update_command AS update_command,
							status.last_update AS last_update FROM status JOIN nodes ON status.node_id = nodes.node_id ORDER BY nodes.node_id ASC, status.param ASC");
while ( $array = mysql_fetch_array($result) )
{
	if ( $cur_node != $array['node_id'] )
	{
		if ( $cur_node != -1 )
			$out.= "</tbody></table><br>";
		$out.= "<h2 align='center'>".$array['full_name']."</h2>";
		$out.= "<table class='table_blur' align='center'><thead><tr>";
		$out.= "<th>Показатель</th>";
		$out.= "<th>Параметр</th>";
		$out.= "<th>Значение</th>";
		$out.= "<th>Множитель</th>";
		$out.= "<th>Последнее обновление</th>";
		$out.= "</tr></thead><tbody>";
		$out.= "<tr>";
		$cur_node = $array['node_id'];
	}
	$out.= "<td>".$array['name']."</td>";
	$out.= "<td align='center'>".$array['param']."</td>";
	$out.= "<td align='center'>".$array['value']."</td>";
	$out.= "<td align='center'>".$array['multiplier']."</td>";
	$refresh_class = "";
	if ( $array['update_command'] )
		$refresh_class = "<i class='fa fa-refresh fa-lg update'>";
	$out.= "<td>".$array['last_update'].$refresh_class."<input type='hidden' class='update_command' value='".$array['update_command']."'></i></td>";
	$out.= "</tr>";
}
mysql_close();
echo $out;
?>