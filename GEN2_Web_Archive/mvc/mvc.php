<?php
/*
 *@author: Gopal Sikhawal
 * @date: 25/09/2014
 */
/* set include_path for spl_autoload */
set_include_path(get_include_path()
  . PATH_SEPARATOR . MYAPP . 'controllers' . DS
  . PATH_SEPARATOR . MYAPP . 'models' . DS
  . PATH_SEPARATOR . MYAPP . 'configs' . DS
  . PATH_SEPARATOR . MYAPP . 'plugins' . DS
  . PATH_SEPARATOR . MYAPP . 'views' . DS
  . PATH_SEPARATOR . MYAPP . 'layouts' . DS
  . PATH_SEPARATOR . MYAPP . 'languages' . DS
  . PATH_SEPARATOR . MYAPP . 'libraries' . DS
  . PATH_SEPARATOR . MVC_CORE . 'libraries' . DS
  );

/* set .php first for speed */
spl_autoload_extensions('.php,.inc');

$spl_funcs = spl_autoload_functions();
if($spl_funcs === false) {
  spl_autoload_register();
} elseif(!in_array('spl_autoload',$spl_funcs)) {
  spl_autoload_register('spl_autoload');
}

class mvc
{
    /* config file values */
     var $config = null;
     /* controller object */
     var $controller = null;
    /* controller method name */
    var $action = null;
    /* server path_info */
    var $path_info = null;
    /* array of url path_info segments */
    var $url_segments = null;

  /**
   * class constructor
   *
   * @access	public
   */
	public function __construct($id='default') {
      self::instance($this, $id);
	}

	/**
   * execute method of execution
   *
   * @access	public
   */
    public function execute() {
        include('application.php');
        $this->config = $config;
		
        $this->path_info = !empty($_SERVER['PATH_INFO']) ? $_SERVER['PATH_INFO'] : 'index.php';
        $this->setupSegments();
        $this->setupController();
        $this->setupAction();
        $this->controller->assignParameters($this->url_segments);
        $this->controller->{$this->action}();
    }
    public function setupSegments() {
		$my_path_info = str_replace('index.php', '', $this->path_info);
        $my_path_info = ltrim($my_path_info, '/');
		$my_path_info = rtrim($my_path_info, '/');
        $this->url_segments = !empty($my_path_info) ? array_filter(explode('/', $my_path_info)) : null;
    }

    public function setupController() {

		$controller_name = !empty($this->url_segments[0]) ? preg_replace('!\W!','',$this->url_segments[0]) : $this->config['default_controller'];
		$controller_file = "{$controller_name}.php";

		/* if no controller, use default */
	 
		if(!stream_resolve_include_path($controller_file)) {
            throw new NotFoundException(__('Controller name not found.'));
		}
    

      include($controller_file);

      /* see if controller class exists */
      $controller_class = $controller_name.'_Controller';

      /* instantiate the controller */
      $this->controller = new $controller_class(true);

    }
    public function setupAction() {

        /* get from url if present, else use default */
         $this->action = !empty($this->url_segments[1]) ? $this->url_segments[1] :
            (!empty($this->config['default_action']) ? $this->config['default_action'] : 'index');

        /* cannot call method names starting with underscore */
        if(substr($this->action, 0, 1)=='_') {
            throw new Exception("Action name not allowed '{$this->action}'");
        }
    }

  /**
   * @access	public
   * @param   object $new_instance reference to new object instance
   * @param   string $id object instance id
   * @return  object $instance reference to object instance
   */
  public static function &instance($new_instance=null, $id='default')
  {
    static $instance = array();
    if(isset($new_instance) && is_object($new_instance))
      $instance[$id] = $new_instance;
    return $instance[$id];
  }

}
?>