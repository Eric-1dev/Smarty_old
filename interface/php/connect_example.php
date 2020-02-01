<?php
mysql_connect('localhost','smarty','password') OR DIE("Не могу создать соединение");
mysql_select_db('smarty') or die(mysql_error());
mysql_query("set names utf8");
?>