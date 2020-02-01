<?php
include "connect.php";

$table = $_POST['table'];
$id = $_POST['id'];
$state = $_POST['state'];

mysql_query("UPDATE $table SET active=$state WHERE id = $id");
mysql_close();
echo 0;
?>
