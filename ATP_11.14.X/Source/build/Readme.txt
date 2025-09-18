		 ------------------
		| BDO Build folder |
		 ------------------

This folder provides a Makefile to build entire BDO source code
with a single command.

  **********************************************************************
  * nt_to_unix and nt_to_unix_all scripts to convert source files to   *
  * UNIX format are NOT called by default. As these scripts reset the  *
  * timestamps of files even if they are already in UNIX format. This  *
  * will make all the source files getting compiled again everytime,   *
  * even if they are not modified. This is not what we need as a       *
  * default behavior.                                                  *
  *                                                                    *
  * If the files are copied freshly from a windows system, see the     *
  * usage of "make convert" below. Use convert option only once and    *
  * subsequently, only "makemt release" can be used to build the bins. *
  **********************************************************************

Use the commands provided below to use the
Makefile (Explaination provided below):
----------------------------------------------
makemt clean
makemt release
makemt debug
makemt convert
makemt convAndBldRel
makemt convAndBldDbg

  *******************************************************************
  * Note 1: "makemt" is a script present in this folder, that can   *
  * be used to replace the repetitive use of the target name. For   *
  * example, instead of typing "make MT=release release", you can   *
  * type "makemt release".                                          *
  *                                                                 *
  * The script will internally convert and build the make command   *
  * as required by the Makefile. Make sure that execute permissions *
  * are provided to the script file. If not, issue the command:     *
  *     $ chmod +x ./makemt                                         *
  *                                                                 *
  *******************************************************************

  *******************************************************************
  * Note 2: /usr/bin/gcc and /usr/bin/cc are modified and used as a *
  * script. The script in turn calls compiler executable            *
  * /usr/vacpp/bin/xlC.                                             *
  *                                                                 *
  *******************************************************************

	Various builds provided and their meaning
	-----------------------------------------

$ make
$ makemt
	This will list down all the options provided by this Makefile.

$ makemt help
	Typing make help will print this Readme.txt file

$ makemt clean

	to clean up entire BDO source. This will delete all objects
	files, temporary files, built modules etc. from all modules,
	as well as bin/ and lib/ folders across the project.

	Not recommended, when making changes in just one module/file.
	Normally, make autmatically detects changed files. Use
	it only when making changes in common header files that
	are not set as a dependency in the makefiles.

$ makemt release

	Use to build client releases.
	-----------------------------
	to build all BDO modules (core libs, core modules,
	App libs and Server modules). It will not clean up, meaning,
	it will compile only modified/latest files and ignore
	source files that are older than the corresponding object
	files dependencies.

$ makemt debug

	Use to build debug versions of modules
	--------------------------------------
	to build all BDO modules (core libs, core modules,
	App libs and Server modules). It will not clean up, means,
	it will compile only modified/latest files and ignore
	source files that are older than the corresponding object
	files dependencies.

	Daemon object will be built in debug mode, so that it puts
	25 seconds delay before calling MainProcessor(). This will
	give sufficient time to find out the final PID of the
	process using "ps -ef" command and then attaching the process
	to gdb debugger.
	This is helpful when the process crashes even before
	initializaion. As gdb doesn't automatically switch to forked
	processes, it fails to debug ATP modules that fork twice
	before call MainProcessor.

$ makemt convert
	Use this command to convert complete BDO source code from
	Windows format (with CR/LF) to UNIX format (only CR). This
	command only 'converts' the files, it will not 'build' or
	'compile' the source code. For both converting as well as
	building at the same time, use the last two makefile options
	- convAndBldRel or convAndBldDbg.

	Be warned that using convert will reset timestamps of all
	*.c/*.h/*.pc files even if they are already in UNIX format.
	Building the source code after using "make convert" will
	always compile each and every source file. This option can
	be useful when copying complete source freshly from SVN
	(on Windoes) to a Solaris box.

	It only needs to be used once. Subsequently, use either
	"make MT=release release" or
	"make MT=debug debug"
	can be used as desired.

	If only a few files are copied from windows, use "build1" or
	'nt_to_unix_all' command in corresponding folder to convert
	only those files.

$ makemt convAndBldRel

	Use this command to convert complete BDO source code from
	Windows format (with CR/LF) to UNIX format (only CR). This
	command is euivalent to a clean compile as all files will
	get modified and will be built in release mode.
	This option can be useful when copying complete source
	freshly from SVN to a Solaris box. However, it is same as
	using 'convert' followed by 'release'.

	If only a few files are copied from windows, use "build1" or
	'nt_to_unix_all' command in corresponding folder to convert
	only those files.


$ makemt convAndBldDbg

	Use this command to convert complete BDO source code from
	Windows format (with CR/LF) to UNIX format (only CR). This
	command is euivalent to a clean compile as all files will
	get modified and will be built in debug mode.
	This option can be useful when copying complete source
	freshly from SVN to a Solaris box. However, it is same as
	'convert' followed by 'debug'.

	If only a few files are copied from windows, use "build1" or
	'nt_to_unix_all' command in corresponding folder to convert
	only those files.






-----------------X--------------X---------------X--------------------

