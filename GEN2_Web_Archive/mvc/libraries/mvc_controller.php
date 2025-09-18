<?php


/**
 * Name:        MVC
 * Author:     	Gopal Sikhawal
 ***/

// ------------------------------------------------------------------------

/**
 * MVC_Controller
 *
 * @package	MVC
 * @author	Gopal Sikhawal
 */

class MVC_Controller
{

/**
 * class constructor
 *
 * @access	public
 */
  function __construct()
  {
    mvc::instance($this, 'controller');
    /* instantiate load library */
    $this->load = new MVC_Load;
    $this->view = new MVC_View;
  }

/**
 * index
 *
 * the default controller method
 *
 * @access	public
 */
  function index() { }

/**
 * __call
 *
 * gets called when an unspecified method is used
 *
 * @access	public
 */
	function __call($function, $args) {
  
		throw new Exception("Unknown controller method '{$function}'");

	}
	public function redirect($temp) {
		header('Location: '.PATHURL.$temp);
	}
	public function authorize() {
		$this->auto_logout();
		if(isset($_SESSION['user']['NAME'])) {
			return true;
		} else {
			$this->redirect('user/login');
		}
		
	}
	public function assignParameters($params) {
      $this->parameter = array();
      if(!empty($params) && count($params) > 2) {
        unset($params[0]);unset($params[1]);
        foreach($params as $k => $v) {
            if(is_int($k)) {
              $this->parameter[] = $v;
            } else {
              $this->parameter[$k] = $v;
            }
        }
      }
	}  
	public function auto_logout() {
		$cur_time = time();
		if(isset($_SESSION['user_time'])) {
			$start_time = $_SESSION['user_time'];
			$inactive_time = $cur_time - $start_time;
			$max_inactive_time = ini_get('session.gc_maxlifetime');
			if($inactive_time > $max_inactive_time) {
				if(isset($_SESSION['user']) && !empty($_SESSION['user'])) {
					$this->auditLogOut('LOGOUT DUE TO SESSION TIMEOUT', $_SESSION['user']);
				}
				unset($_SESSION['user']);
				session_unset();
				session_destroy();
			} else {
				$_SESSION['user_time'] = $cur_time;
			}
		} else {
			$_SESSION['user_time'] = $cur_time;
		}
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
		$op = database::doInsert($sql, $param, database::SECOND_INSTANCE);
        return $op;
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
}

?>
