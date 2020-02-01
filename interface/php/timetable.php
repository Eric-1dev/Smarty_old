<?php
function putTimetable(&$item = 0)
{
	$starttime = "";
	$stoptime = "";
	$startvalue = "";
	$stopvalue = "";
	$days = 127;
	$str = "empty_tbl";
	$chkd = "";
	$dows = array(0 => "ПН",
				  1 => "ВТ",
				  2 => "СР",
				  3 => "ЧТ",
				  4 => "ПТ",
				  5 => "СБ",
				  6 => "ВС");
	if ( $item )
	{
		$starttime = $item['starttime'];
		$stoptime = $item['stoptime'];
		$startvalue = $item['startvalue'];
		$stopvalue = $item['stopvalue'];
		$days = $item['days'];
		$str = "tbl";
		if ( $stopvalue == NULL )
			$chkd = "checked";
	}
	$out = "<div class='".$str."'>";
		$out.= "<div class='tbl_cell_left'><table>";
			$out.= "<tr><th>Разово</th>";
			$out.= "<th>Начальное значение</th>";
			$out.= "<th class='col_stop'>Конечное значение</th>";
			$out.= "<th rowspan=4 class='td_placeholder' style='display: none;'></th>";
			$out.= "<th>Дни недели</th></tr>";
			$out.= "<tr>";
			$out.= "<td rowspan=3><input type='checkbox' class='single' ".$chkd."></td>";
			$out.= "<td><input class='value_text startvalue' size='5em' value='".$startvalue."'></td>";
			$out.= "<td class='col_stop'><input class='value_text stopvalue' size='5em' value='".$stopvalue."'></td>";
			$out.= "<td rowspan=3>";
			$out.= "<table class='dows_tbl' onmousedown='return false' onselectstart='return false'><tr>";
			// for ( $i=0, $j=1; $i<7; $i++ )
			// {
				// $str = "";
				// if ( $days & (1<<$j) )
					// $str = "dows_selected";
				// $out.= "<td value='".$j."' class='dows ".$str."'><b>".$dows[$j]."</b></td>";
				// $j++;	// Костыли для НОРМАЛЬНОЙ недели, начинающейся с понедельника
				// if ( $j > 6 )	// тупые пендосы, bljad'
					// $j=0;
			// }
			for ( $i=0; $i<7; $i++ )	// В mysql не пендосы, походу
			{
				$str = "";
				if ( $days & (1<<$i) )
					$str = "dows_selected";
				$out.= "<td value='".$i."' class='dows ".$str."'><b>".$dows[$i]."</b></td>";
			}
			$out.= "</tr></table>";
			$out.= "</td></tr>";
			$out.= "<tr><th>Время начала</th>";
			$out.= "<th class='col_stop'>Время окончания</th>";
			$out.= "<tr><td><input type='text' class='value_text starttime' size='4em' value='".strTime($starttime)."'></td>";
			$out.= "<td class='col_stop'><input type='text' class='value_text stoptime' size='4em' value='".strTime($stoptime)."'></td></tr>";
		$out.= "</table></div>";
		$out.= "<div class='tbl_cell_right'>";
			$out.= "<div style='width: 70px; text-align: right;'><button class='button del_time' type='button'><i class='fa fa-minus'></i></button></div>";
		$out.= "</div>";
	$out.= "</div>";
	return $out;
}

function strTime($time)
{
	if ( $time == "" )
		return "";
	$hour = $time/60 | 0;
	$minute = $time%60;
	if ( $hour < 10 )
		$hour = "0".$hour;
	if ( $minute < 10 )
		$minute = "0".$minute;
	return $hour.":".$minute;
}
?>