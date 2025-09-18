<?php

/**
 * default.php
 *
 * default application controller
 *
 * @package		MVC
 * @author		Gopal Sikhawal
 */

class Ency_Controller extends MVC_Controller
{
	function index()
	{
		//disable errors and warning 
		//error_reporting(E_ALL);
		//ini_set('display_errors', 1);
		$err = array();
		$err['is_error'] = false;
		$encrypted = '';
		$data = array();
		$this->load->model('Ency_Model', 'ency');
		if(isset($_SERVER['REQUEST_METHOD']) && $_SERVER['REQUEST_METHOD'] == 'POST') {
			$temp = $_POST['data'];
			
			$data['SALT'] = isset($temp['SALT']) ? trim($temp['SALT']) : '';
			$data['PASSWORD'] = isset($temp['PASSWORD']) ? trim($temp['PASSWORD']) : '';
			
			if(empty($data['SALT'])) {
				$err['is_error'] = true;
				$err['message'] = 'Please enter Salt for encryption.';
			}
			if(empty($data['PASSWORD'])) {
				$err['is_error'] = true;
				$err['message'] = 'Please enter Password for encryption.';
			}
			
			if($err['is_error'] === false) {//echo '<pre>';print_r($temp);print_r($err);exit;
				//$encrypted = $this->ency->decrypt($data['PASSWORD'], $data['SALT']);
				$encrypted = $this->ency->encrypt($data['PASSWORD'], $data['SALT']);
			}//
		}
		$this->view->assign('encrypted', $encrypted);
		$this->view->assign('err', $err);
		$this->view->assign('data', $data);
        $this->view->display('ency_index_view');
	}
}
?>