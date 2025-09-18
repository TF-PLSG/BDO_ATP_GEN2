<?php

/*
 *@author: Gopal Sikhawal
 * @date: 25/09/2014
 */
if(session_id() == '') {
    session_start();
}
define('AUTH_AUTHORIZATION_OVERRIDE_G', 53);
define('AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_G', 52);
/* directory separator alias */
if(!defined('DS'))
  define('DS',DIRECTORY_SEPARATOR);
/* set the base directory */
if(!defined('MVC_BASEDIR'))
  define('MVC_BASEDIR', dirname(__FILE__) . DS );
/* set the application directory */
if(!defined('MYAPP'))
  define('MYAPP', MVC_BASEDIR . 'app'. DS);
/* set the core mvc directory */
if(!defined('MVC_CORE'))
  define('MVC_CORE',  MVC_BASEDIR. 'mvc'. DS);
/* set the application directory */
if(!defined('WEBPATH'))
  define('WEBPATH', '/BDO/app/web/');
/* set the application directory */
if(!defined('PATHURL'))
  define('PATHURL', '/BDO/');
/* include main mvc class */
require(MVC_CORE . 'mvc.php');//echo 'Takalu';exit;
/* instantiate */
$mvc = new mvc();
/* execute */
$mvc->execute();
?>