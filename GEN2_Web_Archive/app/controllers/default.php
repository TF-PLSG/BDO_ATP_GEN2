<?php

/**
 * default.php
 *
 * default application controller
 *
 * @package		MVC
 * @author		Gopal Sikhawal
 */

class Default_Controller extends MVC_Controller
{
  function index()
  {
        echo 'sfasfas';
        $this->view->assign('name', 'Jamadagni');
        $this->view->display('default_index_view');
  }
}

?>
