<?php
define('DB_SERVER', '10.1.5.184');
define('DB_USERNAME', 'EQUITABLE');
define('DB_PASSWORD', 'equitable');
define('DB_DATABASE', 'BDOOrcl19');
$pass='equitable';
$db_user='EQUITABLE';
echo "123"
//oci_connect($username, $password, '(DESCRIPTION = (ADDRESS = (PROTOCOL = TCP)(HOST = 192.168.3.14)(PORT = 1521)) (CONNECT_DATA = (SERVICE_NAME = ORCL) (SID = ORCL)))');
//$conn= oci_connect(DB_USERNAME, DB_PASSWORD, DB_SERVER)
//$database = (DESCRIPTION=(ADDRESS = (PROTOCOL = TCP)(HOST = 10.1.5.184)(PORT = 1521))(CONNECT_DATA=((SERVICE_NAME = BDOOrcl19)SID=BDOOrcl19)));
$conn= oci_connect($username, $password, '(DESCRIPTION = (ADDRESS = (PROTOCOL = TCP)(HOST = 10.1.5.184)(PORT = 1521)) (CONNECT_DATA = (SERVICE_NAME = BDOOrcl19) (SID = BDOOrcl19)))');
//or die(oci_error());
echo "success...";
var_dump($conn);
$stid=oci_parse($conn,'SELECT * FROM UCF01');
oci_execute($stid);
//echo "<table border='1'\n";while ($row=oci_fetch_array($stid,OCI_ASSOC+OCI_RETURN_NULLS)) (
?>
