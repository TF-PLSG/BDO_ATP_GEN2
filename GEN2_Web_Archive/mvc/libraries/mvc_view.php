<?php

/*
 *@author: Gopal Sikhawal
 * @date: 25/09/2014
 */

class MVC_View
{

/**
 * $view_vars
 *
 * vars for view file assignment
 *
 * @access	public
 */
 var $view_vars = array();
  
/**
 * class constructor
 *
 * @access	public
 */
 function __construct() {}
  
/**
 * assign
 *
 * assign view variables
 *
 * @access	public
 * @param   mixed $key key of assignment, or value to assign
 * @param   mixed $value value of assignment
 */
 public function assign($key, $value=null) {
 
    if(isset($value)) {
      $this->view_vars[$key] = $value;
    } else {
      foreach($key as $k => $v) {
        if(is_int($k)) {
          $this->view_vars[] = $v;
        } else {
          $this->view_vars[$k] = $v;
        }
      }
    }
 }  

/**
* display
*
* display a view file
*
* @access	public
* @param   string $filename the name of the view file
* @return  boolean
*/
public function display($_mvc_filename, $layout='default') {
    $content = $this->_view("{$_mvc_filename}.php");
    include("{$layout}_layout.php");
}


/**
 * fetch
 *
 * return the contents of a view file
 *
 * @access	public
 * @param   string $filename
 * @return  string contents of view
 */
  public function fetch($filename, $layout='default')
  {
    ob_start();
    extract($this->view_vars);
    $_mvc_filepath = "{$filename}.php";
    $_mvc_layout = "{$layout}_layout.php";
    try {
      include($_mvc_filepath);
    } catch (Exception $e) {
      throw new Exception("Unknown file '$_mvc_filepath'");
    }
    $content = ob_get_contents();
    ob_end_clean();
    ob_start();
     try {
      include($_mvc_layout);
    } catch (Exception $e) {
      throw new Exception("Unknown file '$_mvc_layout'");
    }
    $results = ob_get_contents();
    ob_end_clean();
    return $results;
  }


/**
* _view
*
* internal: display a view file
*
* @access	public
* @param   string $_tmvc_filepath
* @param   array $view_vars
*/
  public function _view($_mvc_filepath)
  {
    // bring view vars into view scope
    ob_start();
    extract($this->view_vars);
    try {
      include($_mvc_filepath);
    } catch (Exception $e) {
      throw new Exception("Unknown file '$_mvc_filepath'");      
    }
    $results = ob_get_contents();
    ob_end_clean();
    return $results;
  }

}

?>
