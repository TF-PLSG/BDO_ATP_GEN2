<?php

/**
 * @package		MVC
 * @author		Gopal Sikhawal
 */

class User_Model extends MVC_Model
{
	const ValidateEmptyField = 'Please enter %s.';
	const InvalidUserName = 'Please enter valid %s.';
	const InvalidUserPassword = 'Please enter valid %s.';
	const NoPermission = 'You do not have permission to Log-in. Please contact Administrator.';
	const NoPermissionExp = 'You do not have permission to export csv. Please contact Administrator.';
	const AccountLocked = 'Your Account is locked. Please contact Administrator.';
   
   function getUserInfo($data) {
        $errors['is_error'] = false;
        $param['userName'] = $data['UserName'];
		$sql = 'BEGIN GET_USER_INFO.SP_GET_USER_INFO(:userName, :result); END;';
		$result = database::doProcedure($sql, $param, database::SECOND_INSTANCE);
		
		//$param['Name'] = $data['UserName'];
		//$sql = 'SELECT uc.NAME, uc.password, uc.group_id, uc.SUBSYSTEM_NAME_16, uc.ACTION_16, uc.accountstatus FROM UCF01 uc WHERE name=:bName';          
		//$sql = 'SELECT * FROM UCF01 uc WHERE name=:bName';          
		//$result = database::doSelect($sql, $param, database::SECOND_INSTANCE);
		
		if(!empty($result) && isset($result[0])) {
			if(isset($result[0]['ACCOUNTSTATUS']) && $result[0]['ACCOUNTSTATUS'] == 'locked') {
				$errors['is_error'] = true;
				$errors[] = User_Model::AccountLocked;
			} else {
				$errors['user'] = $result[0];
			
				$_SESSION['export_user'] = $result[0];
			}
		} else {
			$errors['is_error'] = true;
			$errors[] = sprintf(User_Model::InvalidUserName, 'User Name');
			
		}
		return $errors;
    }
	function validateSigninPost($data) {
		$errors['is_error'] = false;
		
		$errors = $this->validateEmpty('User Name', $data['UserName'], $errors);
		$errors = $this->validateEmpty('Password', $data['Password'], $errors);
		
		return $errors;
	}
	function validateEmpty($fieldname, $value, $errors) {
		if(trim($value) == '') {
			$errors['is_error'] = true;
			$errors[] = sprintf(User_Model::ValidateEmptyField, $fieldname);
		}
		return $errors;
	}
	/* function matchUserPassword($errors, $data) {
		$errors['is_error'] = false;
		$password = $data['Password'];
		$h = (float)sprintf('%u', 0);
		$g = (float)sprintf('%u' , 3435973836);
		$password_array = str_split($password);

		foreach ($password_array as $ch) {
		
			$h = (float)sprintf('%u' , ($h << 4) +ord($ch));
			$g = (float)sprintf('%u' , $h & 0xF0000000);
			 if($g) {
                $h ^= $this->uRShift($g , 24);
			}
			$k =  (float)sprintf('%u' , ~$g) ;
			$h = (float)sprintf('%u' , ($h&$k));
		}
		$enc = str_pad($h, 32, 0, STR_PAD_LEFT);
		
		if($enc != $errors['user']['PASSWORD']) {
			$errors['is_error'] = true;
			$errors[] = sprintf(User_Model::InvalidUserPassword, 'Password');
			if(isset($_SESSION['INVALID_ATTEMPT'])) {
				$_SESSION['INVALID_ATTEMPT'] += 1;
			} else {
				$_SESSION['INVALID_ATTEMPT'] = 1;
			}
			if(isset($_SESSION['INVALID_ATTEMPT']) && $_SESSION['INVALID_ATTEMPT'] >= 3) {
				$this->lockUser($data);
				$errors['is_error'] = true;
				$errors[] = User_Model::AccountLocked;
				$_SESSION['INVALID_ATTEMPT'] = 0;
			}
		}
		if($errors['is_error'] === false && isset($errors['user']['ACTION_16']) && !empty($errors['user']['ACTION_16'])) {
			if(substr($errors['user']['ACTION_16'], 3, 1) != 1) {
				$errors['is_error'] = true;
				$errors[] = User_Model::NoPermission;
			}
		}
		return $errors;
	} */

	function matchUserPassword($errors, $data) {
		$errors['is_error'] = false;
		
		$password = $data['Password'];
		$h = (float)sprintf('%u', 0);
		$g = (float)sprintf('%u' , 3435973836);
		$password_array = str_split($password);

foreach ($password_array as $ch) {
	$h = (float)  ($h << 4) + ord($ch);
	$g = (float) $h & 0xF0000000;
	if($g) {
        $h ^= $this->uRShift($g , 24);
	}
	if($g == 0) {
	    $k = 4294967295; //bug for zero, so fixed value given.
	} else {
	    $k =  (float) ~$g;
	}
	$h = (float)sprintf('%u' , ($h & $k));
}

		$enc = str_pad($h, 32, 0, STR_PAD_LEFT);
		//var_dump($enc);
		//die();
	
		
		if($enc != $errors['user']['PASSWORD']) {
			$errors['is_error'] = true;
			$errors[] = sprintf(User_Model::InvalidUserPassword, 'Password');
			if(isset($_SESSION['INVALID_ATTEMPT'])) {
				$_SESSION['INVALID_ATTEMPT'] += 1;
			} else {
				$_SESSION['INVALID_ATTEMPT'] = 1;
			}
			if(isset($_SESSION['INVALID_ATTEMPT']) && $_SESSION['INVALID_ATTEMPT'] >= 3) {
				$this->lockUser($data);
				$errors['is_error'] = true;
				$errors[] = User_Model::AccountLocked;
				$_SESSION['INVALID_ATTEMPT'] = 0;
			}
		}
		if($errors['is_error'] === false && isset($errors['user']['ACTION_16']) && !empty($errors['user']['ACTION_16'])) {
			if(substr($errors['user']['ACTION_16'], 3, 1) != 1) {
				$errors['is_error'] = true;
				$errors[] = User_Model::NoPermission;
			}
		} 
		//$errors['is_error'] = false;
		
		return $errors;
	}
	function lockUser($data) {
		if(isset($data['UserName']) && !empty($data['UserName'])) {
			$param['ACCOUNTSTATUS'] = 'locked';
			$param['Name'] = $data['UserName'];
			$sql = 'UPDATE UCF01 SET ACCOUNTSTATUS=:bACCOUNTSTATUS WHERE Name=:bName';
			$output = database::doInsert($sql, $param, database::SECOND_INSTANCE);
			return $output;
		}
	}
	function auditLog($data, $errors) {
		$message = '';
		$status = '';
		if(!$errors['is_error']) {
			$message = 'SUCCESSFULL LOGIN';
			$status = 'SUCCESS';
		} else {
			$status = 'FAILURE';
			unset($errors['is_error']);
			$message = 'FAILURE LOGIN: ';
			if(is_array($errors)) {
				foreach($errors as $val) {
					$message = $message."\n".$val;
				}
			}
		}
		$message = substr($message, 0, 50);
		$UNIQUE_ID = date('YmdHis');
		$mt = microtime(true);
		$s = explode('.', $mt);
		$micro = $s[1]*1000;
		$UNIQUE_ID .= substr($micro, 0, 3);
		$hst_data = array('CHANGED_BY' => $data['UserName'], 
			'FILE_NAME' => 'ARCHIVE', 
			'FIELD_TAG' => 'USERNAME/PASSWORD',
			'FILE_PRIMARY_KEY' => 'USER LOGIN',
			'DATE_MODIFIED' => date('Ymd'),
			'TIME_MODIFIED' => date('His'),
			'UNIQUE_ID' => $UNIQUE_ID,
			'CURRENT_DATA' => $message,
		);
		$this->UpdateHST($hst_data);
	}
	function auditLogOut($message, $data) {
		
		$UNIQUE_ID = date('YmdHis');
		$mt = microtime(true);
		$s = explode('.', $mt);
		$micro = $s[1]*1000;
		$UNIQUE_ID .= substr($micro, 0, 3);
		$hst_data = array('CHANGED_BY' => $data['NAME'], 
			'FILE_NAME' => 'ARCHIVE', 
			'FIELD_TAG' => 'LOG-OUT',
			'FILE_PRIMARY_KEY' => 'USER LOG-OUT',
			'DATE_MODIFIED' => date('Ymd'),
			'TIME_MODIFIED' => date('His'),
			'UNIQUE_ID' => $UNIQUE_ID,
			'CURRENT_DATA' => $message,
		);
		$this->UpdateHST($hst_data);
	}
	function uRShift($a, $b) {
		if($b == 0) return $a;
		return ($a >> $b) & ~(1<<(8*PHP_INT_SIZE-1)>>($b-1));
	}
	function setUserSession($user) {
		$_SESSION['user'] = $user;
	}
	function unsetUserSession() {
		unset($_SESSION['user']);
		session_unset();
		session_destroy();
	}
	function setRememberMeCookie($data) {
		$year = time() + 31536000;
		setcookie('RememberMe', $data['RememberMe'], $year, '/');
		setcookie('UserName', $data['UserName'], $year, '/');
	}
	function checkRememberMe($data) {
		if(isset($data['RememberMe']) && $data['RememberMe'] == 1 && !isset($_COOKIE['RememberMe'])) {
			$this->setRememberMeCookie($data);
		} elseif(isset($data['RememberMe']) && !$data['RememberMe'] && isset($_COOKIE['RememberMe'])) {
			$this->unsetRememberMeCookie($data);
		}
	}
	function unsetRememberMeCookie($data) {
		$year = time() - 31536000;
		setcookie('RememberMe', $data['RememberMe'], $year, '/');
		setcookie('UserName', $data['UserName'], $year, '/');
	}
	function hasExportPermission($data) {
		$errors['is_error'] = true;
		if(isset($data['ACTION_16']) && !empty($data['ACTION_16'])) {
			if(substr($data['ACTION_16'], 1, 1) == 1) {
				$errors['is_error'] = false;
			}
		}
		if($errors['is_error']) {
			$errors[] = User_Model::NoPermissionExp;
		}
		return $errors;
	}
	
	public function UpdateHST($param) {
		$first = true;
		$keys = '';
		$values = '';
		foreach($param as $k => $v) {
			if($first) {
				$first = false;
				$keys .= $k;
				$values .= ':b'.$k;
			} else {
				$keys = $keys.', '.$k;
				$values = $values.', :b'.$k;
			}
		}
		$sql = 'INSERT INTO HST01 ('.$keys.') VALUES ('.$values.')';
		$data = database::doInsert($sql, $param, database::SECOND_INSTANCE);
        return $data;
	}
}

?>

