<title>Протоколы Smarty</title>

<?php
include "php/redirect.php";
include "php/connect.php";

echo "<h1 align='center'>Протоколы Smarty</h1>";
echo "<table class='table_blur' align='center'><thead>";
echo "<tr>";
echo "<th>ID</th>";
echo "<th>Протокол</th>";
echo "</tr></thead><tbody>";

$result = mysql_query("SELECT * FROM protocols");
while ( $nodes_array = mysql_fetch_array($result) )
{
	echo "<tr>";
	echo "<td>".$nodes_array['id']."</td>";
	echo "<td>".$nodes_array['name']."</td>";
	echo "</tr>";
}
echo "</tbody></table>";
mysql_close();
?>
