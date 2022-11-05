<html>
<body>
<?php
for ($i = 1; $i < 5; $i++)
{
	echo "<h$i>Hello World</h$i>";
}
echo "Hello World";
 echo "<br>";
echo $_SERVER["REQUEST_METHOD"];
echo "<br>";
echo $_SERVER["QUERY_STRING"];
echo "<br>";
echo $_GET["PFA"];
echo "<br>";

?>
</body>
</html>
