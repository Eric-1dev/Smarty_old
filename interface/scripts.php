<script src="js/scripts.js"></script>
<title>Скрипты Smarty</title>

<?php
include "php/redirect.php";
include "php/connect.php";

$out = "<h1 align='center'>Скрипты</h1>";
$out.= "<h2 align='center'>Выберите скрипт для просмотра его состава</h2>";
$out.= "<div align='center'>";
$out.= "<button class='button add_script'><i class='fa fa-lg fa-plus-square-o'></i></button></div><br>";
$out.= "<table class='table_blur' align='center'><thead><tr>";
$out.= "<th>Название скрипта</th>";
$out.= "<th>Инициатор</th>";
$out.= "<th>Параметр</th>";
$out.= "<th>Мин. значение</th>";
$out.= "<th>Макс. значение</th>";
$out.= "<th>Активный</th>";
$out.= "<th></th>";
$out.= "</tr></thead><tbody>";
$result = mysql_query("SELECT scripts.id AS id, scripts.full_name AS full_name, nodes.full_name AS node,
							scripts.param AS param, scripts.value_min AS value_min, scripts.value_max AS value_max,
							scripts.active AS active FROM scripts JOIN nodes ON scripts.node_id = nodes.node_id WHERE scripts.id > 1");
while ( $array = mysql_fetch_array($result) )
{
	$out.= "<tr class='tbl_row' id='".$array['id']."'>";
	$out.= "<td>".$array['full_name']."</td>";
	$out.= "<td>".$array['node']."</td>";
	$out.= "<td>".$array['param']."</td>";
	$out.= "<td align='center'>".$array['value_min']."</td>";
	$out.= "<td align='center'>".$array['value_max']."</td>";
	$out.= "<td align='center'>";
	if ( $array['active'] )
		$out.= "<i class='fa fa-check-square-o fa-lg active'></i>";
	else
		$out.= "<i class='fa fa-square-o fa-lg active'></i>";
	$out.= "</td>";
	$out.= "<td><div class='but_grp'>";
		$out.= "<div><button class='button edit_script'><i class='fa fa-lg fa-pencil-square-o'></i></button></div>";
		$out.= "<div><button class='button del_script'><i class='fa fa-lg fa-minus-square-o'></i></button></div>";
	$out.= "</div></td>";
	$out.= "</tr>";
}
$out.= "</tbody></table>";
mysql_close();
echo $out;
?>
