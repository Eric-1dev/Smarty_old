<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="shortcut icon" href="/img/home.png" type="image/x-icon">
<script src="js/jquery-3.2.1.min.js"></script>
<script src="js/jquery-ui.min.js"></script>
<script src="js/jquery.maskedinput.min.js"></script>
<script src="js/jsmain.js"></script>
<script src="js/window.js"></script>
<link rel="stylesheet" type="text/css" href="css/jquery-ui.min.css">
<link rel="stylesheet" type="text/css" href="css/font-awesome.min.css">
<link rel="stylesheet" type="text/css" href="css/main.css">
<link rel="stylesheet" type="text/css" href="css/window.css">
<link rel="stylesheet" type="text/css" href="css/commands.css">
<link rel="stylesheet" type="text/css" href="css/scheduler.css">
<link rel="stylesheet" type="text/css" href="css/scripts_edit.css">
</head>

<body>
<div id="main_menu">
	<a href="nodes.php" class="menu_but">Список узлов</a>
	<a href="status.php" class="menu_but">Состояние</a>
	<a href="scheduler.php" class="menu_but">Планировщик заданий</a>
	<a href="scripts.php" class="menu_but">Скрипты</a>
	<a href="prots.php" class="menu_but">Протоколы</a>
</div>

<div id="work_space">
</div>

<div id="blocker" style="display: none">
</div>

<div id="message">
	<div id="message_icon_place"><i class="fa" id="message_icon"></i></div>
	<div id="message_content"></div>
</div>

<img src="img/ajax-loader.gif" id="waiting" style="display: none">


</body>
</html>

<?php
$str = "";
if ( isset($_GET['page']) )
	$str = $_GET['page'];
echo "<input type='hidden' id='page' value='".$str."'>";
?>