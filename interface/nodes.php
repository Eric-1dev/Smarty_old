<script src="js/nodes.js"></script>
<title>Список узлов Smarty</title>
 
<?php
include "php/redirect.php";
include "php/connect.php";

$out = "<h1 align='center'>Список узлов</h1>";
$out.= "<h2 align='center'>Выберите узел для просмотра доступных команд</h2>";
$out.= "<table class='table_blur' align='center'><thead>";
$out.= "<tr>";
$out.= "<th>ID узла</th>";
$out.= "<th>Полное наименование</th>";
$out.= "<th>Имя узла</th>";
$out.= "<th>Протокол</th>";
$out.= "<th>Состояние</th>";
$out.= "</tr></thead><tbody>";
$result = mysql_query("SELECT nodes.node_id AS node_id, nodes.name AS name,
							protocols.name AS protocol,
							nodes.full_name AS full_name, nodes.status AS status FROM nodes JOIN protocols ON nodes.protocol_id = protocols.id WHERE nodes.node_id > 0");
while ( $nodes_array = mysql_fetch_array($result) )
{
	$out.= "<tr class='nodes_row'>";
	$out.= "<td class='node_id'>".$nodes_array['node_id']."</td>";
	$out.= "<td>".$nodes_array['full_name']."</td>";
	$out.= "<td>".$nodes_array['name']."</td>";
	$out.= "<td>".$nodes_array['protocol']."</td>";
	
	if ( $nodes_array['status'] )
		$out.= "<td class='green'>on-line";
	else
		$out.= "<td class='red'>off-line";
	$out.= "<i class='fa fa-refresh fa-lg update'></i>";
	$out.= "</td></tr>";
}
$out.= "</tbody></table>";
mysql_close();
echo $out;
?>