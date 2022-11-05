<?php

echo "Basic Checks";
echo " ";
if ($_SERVER["REQUEST_METHOD"] == "POST")
{
	echo $_SERVER["REQUEST_METHOD"];
	echo " ";
	echo $_SERVER["QUERY_STRING"];
	echo " ";
	echo $_GET["PFA"];
	echo " ";
} 
else
{
	echo $_SERVER["REQUEST_METHOD"];
}


?>