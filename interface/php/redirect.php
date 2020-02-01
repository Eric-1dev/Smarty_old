<?php
if ( !isset($_POST['ajax']) )
{
	header('HTTP/1.1 200 OK');
	header("Location: index.php?page=".$_SERVER['REQUEST_URI']);
	exit();
}
?>