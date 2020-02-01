<?php
include "connect.php";

$id = $_POST['id'];
$job_name = $_POST['job_name'];
$command_id = $_POST['command_id'];
$job_desc = $_POST['job_desc'];
$active = $_POST['active'];
$times = json_decode($_POST['times']);

$result = mysql_query("SELECT value_min, value_max FROM commands WHERE `id`=".$command_id." LIMIT 1");
$norm_values = mysql_fetch_row($result);

if ( $id )
{
	mysql_query("DELETE FROM jobs_table WHERE `sched_id` = ".$id);
	mysql_query("UPDATE scheduler SET `name`='".$job_name."', `command_id`='".$command_id."',
									  `active`='".$active."', `description`='".$job_desc."' WHERE `id`=".$id);
}
else
{
	$id = 0;
	$result = mysql_query("SELECT `id` FROM scheduler ORDER BY `id` DESC LIMIT 1");
	$id = mysql_fetch_row($result)[0] + 1;
	mysql_query("INSERT scheduler (`id`, `name`, `command_id`, `active`, `description`)
						   VALUES ('".$id."', '".$job_name."', '".$command_id."', '".$active."', '".$job_desc."')");
}

// Автоинкремент не нужен. Вычисляем id для каждой строки расписания
$start_id = 0;
$result = mysql_query("SELECT `id` FROM jobs_table ORDER BY `id` DESC LIMIT 1");
$start_id = mysql_fetch_row($result)[0] + 1;

for ( $i=0; $i<count($times); $i++ ) // Запиливаем jobs_table
{
	// Жжжжжъесть, но вроде работает
	if ( !(is_numeric($times[$i][0]) && is_numeric($times[$i][1]) && is_numeric($times[$i][4]) && ( (is_numeric($times[$i][2]) && is_numeric($times[$i][3])) || $times[$i][3] == -1 )) )
	{
		mysql_close();
		echo "Значения могут быть только цифровыми!";
		return;
	}

	// Жжжжжъесть, но вроде работает
	if ( ($times[$i][0] < $norm_values[0]) || ($times[$i][0] > $norm_values[1]) || ( (($times[$i][2] < $norm_values[0]) || ($times[$i][2] > $norm_values[1])) && ( $times[$i][3] != -1 ) ) )
	{
		mysql_close();
		echo "Значения вне допустимого диапазона!";
		return;
	}

	if ( $times[$i][1] == -1 )
	{
		mysql_close();
		echo "Не заполнено время начала!";
		return;
	}

	// Жжжжжъесть, но вроде работает
	if ( ($times[$i][1] >= $times[$i][3]) && $times[$i][3] != -1 || (($times[$i][1] < 0) || ($times[$i][1] >= 1440) || ( (($times[$i][3] < 0) || ($times[$i][3] >= 1440)) && $times[$i][3] != -1 )) )
	{
		mysql_close();
		echo "Некорректно заполнено время!";
		return;
	}
	if ( $times[$i][3] == -1 )
		$times[$i][3] = $times[$i][2] = 'NULL';

	mysql_query("INSERT jobs_table (`id`, `sched_id`, `starttime`, `stoptime`, `startvalue`, `stopvalue`, `days`)
						   VALUES (".($start_id+$i).", ".$id.", ".$times[$i][1].", ".$times[$i][3].", ".$times[$i][0].", ".$times[$i][2].", ".$times[$i][4].")");
}

mysql_close();
echo "Изменения сохранены|".$id;
?>