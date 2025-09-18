<?php
/**
* PDO SINGLETON CLASS
* @author Gopal Sikhawal
*/

class database {
    const FIRST_INSTANCE = 'FIRST_INSTANCE';
    const SECOND_INSTANCE = 'SECOND_INSTANCE';
    
    static private $PDO_FIRST_INSTANCE;
    static private $PDO_SECOND_INSTANCE;
    static private $PDO_FIRST_PWD;
    static private $PDO_SECOND_PWD;

    private static function getConnection($host=database::FIRST_INSTANCE)
    {
		include('config_database.php');
		//$instance = $config[$host]['host'];
		$user = $config[$host]['user'];

        if($host==database::FIRST_INSTANCE) {
			if(!self::$PDO_FIRST_PWD) {
				$pwd = $config[$host]['password'];
				$secret = $config[$host]['salt'];
				self::$PDO_FIRST_PWD = self::decrypt($pwd, $secret);
			}
            if(!self::$PDO_FIRST_INSTANCE) {
                try {
                    self::$PDO_FIRST_INSTANCE = oci_connect("equitable", "equitable", '(DESCRIPTION = (ADDRESS = (PROTOCOL = TCP)(HOST = 10.1.5.183)(PORT = 1539)) (CONNECT_DATA = (SERVICE_NAME = ORCL) (SID = ORCL)))');
                } catch (Exception $e) {
                    die("PDO CONNECTION ERROR: " . $e->getMessage() . "<br/>");
                }
            }
            return self::$PDO_FIRST_INSTANCE;
        } elseif($host==database::SECOND_INSTANCE) {
			if(!self::$PDO_SECOND_PWD) {
				$pwd = $config[$host]['password'];
				$secret = $config[$host]['salt'];
				self::$PDO_SECOND_PWD = self::decrypt($pwd, $secret);
			}
            if(!self::$PDO_SECOND_INSTANCE) {
                try {
                    self::$PDO_SECOND_INSTANCE = oci_connect("equitable", "equitable", '(DESCRIPTION = (ADDRESS = (PROTOCOL = TCP)(HOST = 10.1.5.183)(PORT = 1539)) (CONNECT_DATA = (SERVICE_NAME = ORCL) (SID = ORCL)))');
                } catch (Exception $e) {
                    die("PDO CONNECTION ERROR: " . $e->getMessage() . "<br/>");
                }
            }
            return self::$PDO_SECOND_INSTANCE;
		}
    }
	public static function decrypt($data, $secret)
	{
	
		//Generate a key from a hash
		$key = md5(utf8_encode($secret), true);
		

		//Take first 8 bytes of $key and append them to the end of $key.
		$key .= substr($key, 0, 8);

		//$data = base64_decode($data); 
		 $data = mcrypt_decrypt('tripledes', $key, $data, 'ecb');
$block = mcrypt_get_block_size('tripledes', 'ecb');
		$len = strlen($data);  
		$pad = ord($data[$len-1]);

$data = openssl_encrypt($key, 'DES-EDE3', $data, OPENSSL_RAW_DATA);	
$data = base64_encode($data);

		//$data = mcrypt_decrypt('tripledes', $key, $data, 'ecb');


$ciphertext = base64_decode($data);

       $block= openssl_decrypt($ciphertext, 'des-ede3-ecb', $key, OPENSSL_RAW_DATA);
 //$data = mcrypt_decrypt('tripledes', $key, $data, 'ecb');
//$block = mcrypt_get_block_size('tripledes', 'ecb');
		$len = strlen($data);  
		$pad = ord($data[$len-1]);
		 
		return substr($data, 0, strlen($data) - $pad);
	}
	public static function doInsert($query, $param=array(), $host=database::FIRST_INSTANCE) {
        
		$con = self::getConnection($host);
        $stmt = oci_parse($con, $query);

       // $queryhash = md5($query);

        foreach ($param as $key => $value) {
            oci_bind_by_name($stmt, ":b$key", $param[$key], -1);
        }
		
        oci_execute($stmt, OCI_COMMIT_ON_SUCCESS);
		
        return true;
    }
    
	public static function doSelectOne($query, $param=array(), $host=database::FIRST_INSTANCE) {
         $con = self::getConnection($host);
        $stmt = oci_parse($con, $query);

        $queryhash = md5($query);

        foreach ($param as $key => $value) {
            oci_bind_by_name($stmt, ":b$key", $param[$key], -1);
        }
        oci_execute($stmt, OCI_DEFAULT);
       // print_r(oci_fetch_array($stmt, OCI_ASSOC+OCI_RETURN_NULLS));
        $res = array();
        while ($row = oci_fetch_array($stmt, OCI_ASSOC+OCI_RETURN_NULLS)) {
           return $row;
        }
        return $res;
    }

    public static function doSelect($query, $param=array(), $host=database::FIRST_INSTANCE) {
        $con = self::getConnection($host);
        $stmt = oci_parse($con, $query);
        
        $queryhash = md5($query);

        foreach ($param as $key => $value) {
            oci_bind_by_name($stmt, ":b$key", $param[$key], -1);
        }
        oci_execute($stmt, OCI_DEFAULT);
        $res = array();
        while ($row = oci_fetch_array($stmt, OCI_ASSOC+OCI_RETURN_NULLS)) {
            $res[] = $row;
        }
        return $res;
    }
    public static function doProcedure($query, $param=array(), $host=database::FIRST_INSTANCE) {
        $con = self::getConnection($host);
        $stmt = oci_parse($con, $query);
		
		foreach ($param as $key => $value) {
            oci_bind_by_name($stmt, ":$key", $param[$key], -1);
        }
		
        $entries = oci_new_cursor($con);
		
        oci_bind_by_name($stmt, ":result", $entries, -1, OCI_B_CURSOR);
        oci_execute($stmt);
        oci_execute($entries);
        $res = array();
        while ($row = oci_fetch_assoc($entries)) {
             $res[] = $row;
        }
        return $res;
    }
}
?>