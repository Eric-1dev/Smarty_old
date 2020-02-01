<script src="js/scheduler.js"></script>
<title>Планировщик заданий</title>

<?php
include "php/redirect.php";
include "php/connect.php";

$out = "<h1 align='center'>Планировщик заданий</h1>";
$out.= "<div align='center'>";
$out.= "<button class='button add_job'><i class='fa fa-lg fa-plus-square-o'></i></button></div><br>";
$out.= "<table class='table_blur' align='center'><thead>";
$out.= "<tr>";
$out.= "<th>Задание</th>";
$out.= "<th>Имя узла</th>";
$out.= "<th>Команда</th>";
$out.= "<th>Последнее выполнение</th>";
$out.= "<th>Описание</th>";
$out.= "<th>Активен</th>";
$out.= "<th></th>";
$out.= "</tr></thead><tbody>";
$result = mysql_query("SELECT s.id AS id, s.name AS name, s.active AS active, s.description AS description,
								c.name AS com_name, c.node_id AS node_id, c.comment AS comment,
								n.full_name AS node_name, (SELECT max(jt.done) FROM jobs_table jt WHERE jt.sched_id = s.id) AS last_run
                                FROM scheduler s JOIN commands c
								ON c.id = s.command_id JOIN nodes n ON n.node_id = c.node_id");
while ( $jobs = mysql_fetch_array($result) )
{
	$out.= "<tr class='tbl_row' id='".$jobs['id']."'>";
	$out.= "<td>".$jobs['name']."</td>";
	$out.= "<td>".$jobs['node_name']."</td>";
	$out.= "<td>".$jobs['com_name']."</td>";
	$out.= "<td align='center'>".($jobs['last_run'] != 0 ? $jobs['last_run'] : 'Неизвестно')."</td>";
	$out.= "<td>".$jobs['description']."</td>";
	$out.= "<td align='center'>";
	if ( $jobs['active'] )
		$out.= "<i class='fa fa-check-square-o fa-lg active'></i>";
	else
		$out.= "<i class='fa fa-square-o fa-lg active'></i>";
	$out.= "</td>";
	$out.= "<td><div class='but_grp'>";
		$out.= "<div><button class='button edit_job'><i class='fa fa-lg fa-pencil-square-o'></i></button></div>";
		$out.= "<div><button class='button del_job'><i class='fa fa-lg fa-minus-square-o'></i></button></div>";
	$out.= "</div></td>";
	$out.= "</tr>";
}

$out.= "</tbody></table>";
mysql_close();
echo $out;

function mintohour($min)
{
	$minutes = $min%60;
	if ( $minutes < 10 ) $minutes = "0".$minutes;
	return $min/60 . ":".$minutes;
}
?>