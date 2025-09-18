<?php

/**
 * default.php
 *
 * default application controller
 *
 * @package		MVC
 * @author		Gopal Sikhawal
 */

class User_Controller extends MVC_Controller
{
	function index()
	{
		$this->redirect('user/login');
	}
	function logout() {
		$this->load->model('User_Model', 'user');
		if(isset($_SESSION['user']) && !empty($_SESSION['user'])) {
			$this->user->auditLogOut('SUCCESSFULL LOG-OUT', $_SESSION['user']);
		}
		$this->user->unsetUserSession();
		$this->redirect('user/login/');
	}
	function login() {
		$this->load->model('User_Model', 'user');
		$data = array();
		if(isset($_SERVER['REQUEST_METHOD']) && $_SERVER['REQUEST_METHOD'] == 'POST') {
			$temp = $_POST['data'];
			$data['UserName'] = isset($temp['UserName']) ? trim($temp['UserName']) : '';
			$data['Password'] = isset($temp['Password']) ? trim($temp['Password']) : '';
			$data['RememberMe'] = isset($temp['RememberMe']) ? trim($temp['RememberMe']) : false;
			$userinfo = array();
			$errors = $this->user->validateSigninPost($data);
			//var_dump($errors);
			if($this->checkvalues($errors, $data)) {
				$errors = $this->user->getUserInfo($data);
				if($this->checkvalues($errors, $data)) {
					$errors = $this->user->matchUserPassword($errors, $data);
					//var_dump($errors);
					if(isset($errors['user'])) {
						$userinfo = $errors['user'];
						unset($errors['user']);
					} //die();
					if($this->checkvalues($errors, $data)) {
						$this->user->checkRememberMe($data);
						$this->user->setUserSession($userinfo);
						$this->user->auditLog($data, $errors);
						$this->redirect('tlf/transaction/');
					}
				}
			}
		} elseif(isset($_SESSION['user']['NAME'])) {
			$this->redirect('tlf/transaction');
		}
        if(isset($_COOKIE['RememberMe']) && $_COOKIE['RememberMe'] != '' && isset($_COOKIE['UserName'])) {
			$data['UserName'] = $_COOKIE['UserName'];
			$data['RememberMe'] = 1;
			$this->view->assign('data', $data);
		}
		$this->view->assign('data', $data);
        $this->view->display('user_login_view', 'ajax');
	}
	private function checkvalues($errors, $data) {
		if(isset($errors['is_error']) && $errors['is_error'] === true) {
			$this->view->assign('data', $data);
			$this->view->assign('errors', $errors);
			$this->load->model('User_Model', 'userlog');
			$this->userlog->auditLog($data, $errors);
			return false;
		}
		return true;
	}
	public function exp_access() {
		$this->load->model('User_Model', 'user');
		$data = array();
		if(isset($_SERVER['REQUEST_METHOD']) && $_SERVER['REQUEST_METHOD'] == 'POST') {
			$temp = $_POST;
			$data['UserName'] = isset($temp['UserName']) ? trim($temp['UserName']) : '';
			$data['Password'] = isset($temp['Password']) ? trim($temp['Password']) : '';
			
			$userinfo = array();
			$errors = $this->user->validateSigninPost($data);
			if($this->checkvalues($errors, $data)) {
				$errors = $this->user->getUserInfo($data);
				if($this->checkvalues($errors, $data)) {
					$errors = $this->user->matchUserPassword($errors, $data);
					if($this->checkvalues($errors, $data)) {
						$userinfo = $errors['user'];
						$errors = $this->user->hasExportPermission($userinfo);
						//echo '<pre>';print_r($errors);exit;
					}
				}
			}
			if(isset($errors['user'])) unset($errors['user']);
			$result['is_error'] = $errors['is_error'];
			unset($errors['is_error']);
			$result['error'] = $errors;
			echo json_encode($result);
			exit;
		}
	}
}

?>
