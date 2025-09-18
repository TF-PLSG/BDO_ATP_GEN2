<?php

/**
 * Name:  	MVC
 * Author:     	Gopal Sikhawal
 ***/

// ------------------------------------------------------------------------


class MVC_Model
{
/**
 * $db
 *
 * the database object instance
 *
 * @access	public
 */
  var $db = null;  
    
/**
 * class constructor
 *
 * @access	public
 */
  function __construct($poolname=null) {
    //$this->db = tmvc::instance()->controller->load->database($poolname);
  }
  
}

?>
