/******************************************************************************
  
        Module: pteipc.h
  
         Title: Pinnacle Inter-Process Communication (IPC) Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\pteipc_lib\PTEIPC.H  $
   
      Rev 1.13   Jan 17 2000 17:04:04   MSALEH
   add pteipc_receive_r()
   
      Rev 1.12   Nov 10 1999 17:53:54   MSALEH
   add pteipc_format_xipc_instance_name()
   
      Rev 1.11   Oct 22 1999 11:33:24   MSALEH
   Priority based message retrieval
   
      Rev 1.10   Aug 30 1999 16:53:48   MSALEH
   remove error msg from pteipc_checkin()
   
      Rev 1.9   Aug 27 1999 16:28:34   MSALEH
   added pteipc_checkin() to send the PID to
   service manager
   
      Rev 1.8   Aug 26 1999 11:24:56   MSALEH
   add pteipc_quedestroy
   
      Rev 1.7   17 Apr 1998 11:29:32   MSALEH
   added :
   pteipc_que_spool(que_name, base_name);
   pteipc_que_spool_off(que_name);
   to enable/disable queue spooling
   
   
      Rev 1.6   16 Apr 1998 16:09:10   MSALEH
   remove pte_instance_name variable, all
   applications should call GetXipcInstanceName()
   and pass the name to any init function.
   
      Rev 1.5   17 Feb 1998 16:19:42   MSALEH
   added set_logger_queue()
   
      Rev 1.4   18 Dec 1997 11:29:22   rmalathk
   1. modified the pteipc_init_multiple_instance routine to
   create queues based on the service name.
   2. Also modified all three init routines to store the
   trace file name
   
      Rev 1.3   Dec 01 1997 16:40:12   drodden
   Added the pte_instance_name variable for applications
   to use rather than them all using a hard coded value.
   
   
      Rev 1.2   Dec 01 1997 08:58:34   drodden
   Added APIs to turn IPC tracing on/off.
   
   
      Rev 1.1   06 Nov 1997 12:22:22   rmalathk
    
   
      Rev 1.0   Oct 23 1997 11:42:22   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   

******************************************************************************/

#ifndef PTEIPC_H
#define PTEIPC_H

#include "xipc.h"
#include "basictyp.h"
#include "ptemsg.h"


#define PTEIPC_WAITFOREVER -1
#define PTEIPC_NOWAIT      0

#define PTEIPC_MALLOC_ERROR   10


extern CHAR application_que_name [];
extern CHAR control_que_name     [];
extern CHAR interactive_que_name [];
extern CHAR logger_que_name      [];


/******************************************************************************
*  function set_logger_que
*
*  This function reads the logger queue name from the registry and copies it  
*  to global variable logger_queue_name
*
*  Parameters: none
*
******************************************************************************/
void pteipc_trace_on ();


/******************************************************************************
*  function pteipc_trace_on
*
*  This function start an IPC trace file that will capture all incoming/outgoing 
*  messages for this application.  The trace file is named: [appname].ipc
*
*  Parameters: none
*
******************************************************************************/
void pteipc_trace_on ();



/******************************************************************************
*  function pteipc_trace_off
*
*  This function stop the IPC tracing.
*
*  Parameters: none
*
******************************************************************************/
void pteipc_trace_off ();


/******************************************************************************
*  function pteipc_format_xipc_instance_name
*
*  This function format a string using @xxxx:yyyy
*  where xxxx = host name, yyyy = instance name
*  The output string should have MAX_INSTANCE_NAME_SIZE bytes
*
*  This function replaces the xipc_instance_name parameter in the init functions
*
*  Parameters: 
*     pointer to string to hold the result
*
*  return value:
*     None
*
******************************************************************************/
void pteipc_format_xipc_instance_name ( pCHAR xipc_instance );



/******************************************************************************
*  function pteipc_init_gui_app
*
*  This function creates the XIPC queues for a GUI application. A GUI app needs
*  only an Interactive queue, so this is the only queue that is created. If the
*  queue was previously existing, it is destroyed and re-created.Call it once at
*  the beginning of the program.  If the init completes successfully, it returns
*  true, otherwise it will return false.  If it returns false, your
*  application should not continue.
*
*  Parameters:
*     app_name...............name of your application.
*     xipc_instance_name.....name of the xipc_instance
*
*  Return Value:
*    TRUE if successful
*    FALSE if unsuccessful
*
*  The xipc_instance_name parameter is not effective as of release 1.3.1.3, the
*  value for the xipc instance name is taken from the tf.ini file
*
******************************************************************************/
BOOLEAN pteipc_init_gui_app               ( pCHAR app_name,
                                            pCHAR xipc_instance_name );



/******************************************************************************
*  function pteipc_init_single_instance_app
*
*  This function creates the XIPC queues for a single instance service application.
*  If the queues were already existing, it destroys and re-creates them.
*  Call it once at the beginning of the program.  If the init completes successfully,
*  it returns true, otherwise it will return false.  If it returns false, your
*  application should not continue.
*
*  Parameters:
*     app_name...............name of your application.
*     xipc_instance_name.....name of the xipc_instance
*
*  Return Value:
*    TRUE if successful
*    FALSE if unsuccessful
*
*  The xipc_instance_name parameter is not effective as of release 1.3.1.3, the
*  value for the xipc instance name is taken from the tf.ini file
*
******************************************************************************/
BOOLEAN pteipc_init_single_instance_app   ( pCHAR app_name,
                                            pCHAR xipc_instance_name );



/******************************************************************************
*  function pteipc_init_multiple_instance_app
*
*  This function creates the XIPC queues for a single instance service application.
*  Call it once at the beginning of the program.  If the init completes successfully,
*  it returns true, otherwise it will return false.  It it returns false, your
*  application should not continue.
*
*  Parameters:
*     app_name...............name of your application.
*     xipc_instance_name.....name of the xipc_instance
*
*  Return Value:
*    TRUE if successful
*    FALSE if unsuccessful
*
*  The xipc_instance_name parameter is not effective as of release 1.3.1.3, the
*  value for the xipc instance name is taken from the tf.ini file
*
******************************************************************************/
BOOLEAN pteipc_init_multiple_instance_app ( pCHAR app_name,
            									     pCHAR service_name,
                                            pCHAR xipc_instance_name );


/******************************************************************************
*  function pteipc_shutdown_gui_app
*
*  This function destroys the XIPC queues for a GUI application and logs the 
*  application out of the XIPC instance. Call this function just before closing
*  your GUI application.
*
*  Parameters:
*     NONE
*
*  Return Value:
*    NONE
*
******************************************************************************/
void pteipc_shutdown_gui_app ();


/******************************************************************************
*  function pteipc_shutdown_single_instance_app
*
*  This function destroys the XIPC queues for a single instance service application
*  and logs the application out of the XIPC instance. Call this function just before
*  closing your single instance application.
*
*  Parameters:
*     NONE
*
*  Return Value:
*    NONE
*
******************************************************************************/
void pteipc_shutdown_single_instance_app ();


/******************************************************************************
*  function pteipc_shutdown_multiple_instance_app
*
*  This function destroys the control and interactive queues for a multiple 
*  instance service application and logs the application out of the XIPC instance.
*  It leaves the shared application queue intact. Call this function just before
*  closing your multiple instance application.
*
*  Parameters:
*     NONE
*
*  Return Value:
*    NONE
*
******************************************************************************/
void pteipc_shutdown_multiple_instance_app ();




/******************************************************************************
*  function pteipc_xipclogin
*
*  This function logs in the calling thread into the XIPC instance specified
*
*  Parameters:
*    instance_name...........name of the XIPC instance
*    user_name...............user name for login ( usually the name of the app )
*
*  Return Value:
*    If successful, >= 0
*     If unsuccessful, < 0
*
******************************************************************************/
LONG pteipc_xipclogin ( pCHAR instance_name,
                        pCHAR user_name );



/******************************************************************************
*  function pteipc_xipclogout
*
*  This function logs out the calling thread out of the current XIPC instance.
*
*  Parameters:
*     NONE
*
*  Return Value:
*    If successful, >= 0
*     If unsuccessful, < 0
*
******************************************************************************/
LONG pteipc_xipclogout ();



/******************************************************************************
*  function pteipc_quecreate
*
*  This function creates the queue specified 
*
*  Parameters:
*     que_name.............name of the queue
*
*  Return Value:
*    If successful, >= 0 ( this will be the Qid )
*     If unsuccessful, < 0
*
******************************************************************************/
LONG pteipc_quecreate ( pCHAR que_name );




/******************************************************************************
*  function pteipc_quedelete
*
*  This function deletes the specified queue
*
*  Parameters:
*     Qid.............queue id of the queue
*
*  Return Value:
*    If successful, >= 0 
*     If unsuccessful, < 0
*
******************************************************************************/
LONG pteipc_quedelete ( LONG Qid );



/******************************************************************************
*  function pteipc_access
*
*  This function accesses an existing queue and returns a queue id
*
*  Parameters:
*     que_name.............name of the queue
*
*  Return Value:
*    If successful, >= 0 ( this will be the Qid )
*     If unsuccessful, < 0
*
******************************************************************************/
LONG pteipc_queaccess ( pCHAR que_name );


/******************************************************************************
*  function pteipc_quespool
*
*  This function enables spooling on a queue
*
*  Parameters:
*     que_name.............name of the queue
*     base_name............base name used to create spoole files
*
*  Return Value:
*    If successful, >= 0 
*     If unsuccessful, < 0
*
******************************************************************************/
LONG pteipc_quespool ( pCHAR que_name, pCHAR base_name );


/******************************************************************************
*  function pteipc_quespool_off
*
*  This function disables spooling on a queue
*
*  Parameters:
*     que_name.............name of the queue
*
*  Return Value:
*    If successful, >= 0 
*     If unsuccessful, < 0
*
******************************************************************************/
LONG pteipc_quespool_off ( pCHAR que_name );


/******************************************************************************
*  function pteipc_quedestroy
*
*  This function destroys a queue regardless of any messages currently on the q
*
*  Parameters:
*     que_name.............name of the queue
*
*  Return Value:
*    If successful, >= 0 
*     If unsuccessful, < 0
*
******************************************************************************/
LONG pteipc_quedestroy ( pCHAR que_name );


/******************************************************************************
*  function pteipc_quepurge
*
*  This function purges all the messages from the queue specified
*
*  Parameters:
*    Qid...........queue id of the queue
*
*  Return Value:
*    NONE
*
******************************************************************************/
void pteipc_quepurge  ( LONG Qid );



/******************************************************************************
*  function pteipc_sleep
*
*  This function suspends the calling process for the time specified
*
*  Parameters:
*    msecs...........time in milliseconds to sleep
*
*  Return Value:
*    NONE
*
******************************************************************************/
void pteipc_sleep ( LONG msecs );



/******************************************************************************
*  function pteipc_send
*
*  This function sends a message to the specified queue. If successful, it returns
*  a value greater than or equal to zero. If unsuccessful, the return value is 
*  less than zero.
*
*  Parameters:
*     p_msg...................Pointer to a PTE_MSG structure
*    que_name................name of the destination queue.
*
*  Return Value:
*    If successful, >= 0
*    If Unsuccessful, < 0
*
*  On an Unsuccessful return, use the function pteic_get_errormsg to determine
*  the error.
*
******************************************************************************/
LONG pteipc_send ( pPTE_MSG p_msg,
                   pCHAR    que_name );

/******************************************************************************
*  function pteipc_send_hp
*
*  This function sends a message to the specified queue with a specific priority.
*  If successful, it returns a value greater than or equal to zero. 
*  If unsuccessful, the return value is less than zero.
*
*  Parameters:
*     p_msg...................Pointer to a PTE_MSG structure
*     que_name................name of the destination queue.
*     priority................The priority assigned to this message
*
*  Return Value:
*    If successful, >= 0
*    If Unsuccessful, < 0
*
*  On an Unsuccessful return, use the function pteic_get_errormsg to determine
*  the error.
*
******************************************************************************/
LONG pteipc_send_hp ( pPTE_MSG p_msg,
                      pCHAR    que_name,
                      LONG     priority);


/******************************************************************************
*  function pteipc_receive
*
*  This function attempts to retrieve a message from either of the two specified
*  queues and returns as soon as there is a message on either queue. This is a 
*  blocking function call and the period of time for which the application is 
*  blocked is specified in the time_out parameter described below. If successful,
*  the function returns a pointer to a PTE_MSG structure, otherwise it returns
*  NULL_PTR, and a error code is returned in the ret_code parameter.
*
*  Parameters:
*     que_name1...........name of the first queue
*    que_name2...........name of the second queue.
*     time_out............can be one of the following values:
*                    PTEIPC_WAITFOREVER
*                    PTEIPC_NOWAIT
*                    An integer specifying the timeout value in seconds.
*    ret_code............address of a variable in which a error code is returned
*                   on a unsuccessful call.
*
*  Return Value:
*    If successful, a pointer to a PTE_MSG structure.
*    If Unsuccessful( including timeouts ), the function returns NULL_PTR
*    and sets an error code in the ret_code parameter.
*
*  On an Unsuccessful return, use the function pteic_get_errormsg to determine
*  the error.
*
******************************************************************************/
pPTE_MSG pteipc_receive ( pCHAR que_name1,
                          pCHAR que_name2,
                          SHORT time_out,
                          pLONG  ret_code );


/******************************************************************************
*  function pteipc_receive_r
*
*  This function attempts to retrieve a message from either of the two specified
*  queues and returns immediately with the result . This function is identical 
*  to pteipc_receive(), except it is intended for use on UNIX platforms in 
*  multi threaded applications, AIX blocks forever on a call to pteipc_receive()
*  in a multi threaded application, this workaround is to retrieve messages 
*  while the queue is not empty, then sleep time_out value if there are no 
*  messages on the queue to relinquish the timeslice.
*  the function returns a pointer to a PTE_MSG structure, otherwise it returns
*  NULL_PTR, and a error code is returned in the ret_code parameter.
*
*  Parameters:
*     que_name1...........name of the first queue
*     que_name2...........name of the second queue.
*     time_out............can be one of the following values:
*                    An integer specifying the timeout value in seconds.
*    ret_code............address of a variable in which a error code is returned
*                    on a unsuccessful call.
*
*  Return Value:
*    If successful, a pointer to a PTE_MSG structure.
*    If Unsuccessful( including timeouts ), the function returns NULL_PTR
*    and sets an error code in the ret_code parameter.
*
*  On an Unsuccessful return, use the function pteic_get_errormsg to determine
*  the error.
*
******************************************************************************/
pPTE_MSG pteipc_receive_r ( pCHAR que_name1,
                            pCHAR que_name2,
                            SHORT time_out,
                            pLONG  ret_code );


/******************************************************************************
*  function pteipc_send_receive
*
*  This function sends a message to a specified destination queue and retrieves
*  a response message from the specified receive queue. After sending the 
*  message, it waits for a response on the receive queue, timing out after 5
*  seconds. If successful, the function returns a pointer to a PTE_MSG structure,
*  otherwise it returns NULL_PTR, and a error code is returned in the ret_code
*  parameter.
*
*  Parameters:
*    p_msg_out...........pointer to an outgoing PTE_MSG structure
*     send_queue..........name of the destination queue
*    recv_queue..........name of the receive queue.
*    ret_code............address of a variable in which a error code is returned
*                   on a unsuccessful call.
*
*  Return Value:
*    If successful, a pointer to a PTE_MSG structure.
*    If Unsuccessful( including timeouts ), the function returns NULL_PTR
*    and sets an error code in the ret_code parameter.
*
*  On an Unsuccessful return, use the function pteic_get_errormsg to determine
*  the error.
*
******************************************************************************/
pPTE_MSG pteipc_send_receive ( pPTE_MSG p_msg_out,
                               pCHAR    send_queue,
                               pCHAR    recv_queue,
                               pLONG    ret_code );


/******************************************************************************
*  function pteipc_send_receive_hp
*
*  This function sends a message to a specified destination queue and retrieves
*  a response message from the specified receive queue. After sending the 
*  message, it waits for a response on the receive queue, timing out after 5
*  seconds. If successful, the function returns a pointer to a PTE_MSG structure,
*  otherwise it returns NULL_PTR, and a error code is returned in the ret_code
*  parameter.
*
*  Parameters:
*    p_msg_out...........pointer to an outgoing PTE_MSG structure
*    send_queue..........name of the destination queue
*    recv_queue..........name of the receive queue.
*    ret_code............address of a variable in which a error code is returned
*                   on a unsuccessful call.
*    priority................The priority assigned to this message
*
*  Return Value:
*    If successful, a pointer to a PTE_MSG structure.
*    If Unsuccessful( including timeouts ), the function returns NULL_PTR
*    and sets an error code in the ret_code parameter.
*
*  On an Unsuccessful return, use the function pteic_get_errormsg to determine
*  the error.
*
******************************************************************************/
pPTE_MSG pteipc_send_receive_hp ( pPTE_MSG p_msg_out,
                                  pCHAR    send_queue,
                                  pCHAR    recv_queue,
                                  pLONG    ret_code,
                                  LONG     priority );

/******************************************************************************
*  function pteipc_get_errormsg
*
*  This function takes in a error_code as it's first parameter and returns a 
*  error message in the second parameter.
*
*  Parameters:
*    ret_code...........An error code returned from a unsuccessful pteipc call
*     error_msg..........A character string in which an error message describing
*                       the error code is returned.
*
*  Return Value:
*    NONE
*
*  Call this function immediately after an unsuccessful call to pte_send, 
*  pte_receive or pte_send_receive.
*  
******************************************************************************/
void pteipc_get_errormsg ( LONG ret_code, pCHAR error_msg );


/******************************************************************************
*  function pteipc_checkin
*
*  This function takes in a service_name and sends the PID to the service  
*  manager to update the service file.
*
*  Parameters:
*    service_name...........The service name
*    errr_msg..........A character string in which an error message describing
*                      the error code is returned.
*
*  Return Value:
*    If successful, >= 0
*    If Unsuccessful, < 0
*  
******************************************************************************/
LONG pteipc_checkin(pCHAR service_name);

#endif


