<script src="js/commands.js"></script>
<script src="js/farbtastic.js"></script>
<title>Команды узла</title>

<?php
include "php/redirect.php";
include "php/connect.php";

$node = $_GET['node'];
$result = mysql_query("SELECT full_name, node_id FROM nodes WHERE node_id = $node");
$row = mysql_fetch_row($result);
$out = "<h1 align='center'>Команды узла</h1>";
$out.= "<h2 align='center'>".$row[0]."</h2>";
$out.= "<input type='hidden' value=".$row[1].">";
$out.= "<table class='table_blur' align='center'><thead>";
$out.= "<tr>";
$out.= "<th>Команда</th>";
$out.= "<th>Get/Set</th>";
$out.= "<th>Параметр</th>";
$out.= "<th>Мин.значение</th>";
$out.= "<th>Макс.значение</th>";
$out.= "<th>Комментарий</th>";
$out.= "</tr></thead><tbody>";
$result = mysql_query("SELECT commands.id AS id, commands.name AS name, commands.get_set AS get_set,
							commands.param AS param, commands.value_min AS value_min, commands.value_max AS value_max,
							commands.comment AS comment FROM commands JOIN nodes ON commands.node_id = nodes.node_id WHERE nodes.node_id=$node ORDER BY param");
while ( $array = mysql_fetch_array($result) )
{
	$id = $array['id'];
	$out.= "<tr>";
	$out.= "<td>".$array['name']."</td>";
	$out.= "<td class='get_set' align='center'>";
	if ( $array['get_set'] )
		$out.= "Get";
	else
		$out.= "Set";
	$out.= "</td>";
	$out.= "<td align='center'>".$array['param']."</td>";
	$out.= "<td align='center'>".$array['value_min']."</td>";
	$out.= "<td align='center'>".$array['value_max']."</td>";
	$out.= "<td>".$array['comment'];
	$out.= "<input type='hidden' value='".$id."'></td>";
	$out.= "</tr>";
}
$out.= "</tbody></table>";

$out.= "<div id='value_selector'>";
$out.= "<i id='value_header'></i>";
$out.= "<input id='value_text'><br>";
$out.= "<input type='hidden' id='cur_command'>";
$out.= "<button class='button' id='send_but'>Отправить</button>";
$out.= "</div>";

echo $out;
?>