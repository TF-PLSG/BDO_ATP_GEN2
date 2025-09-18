/*
  Application: hostsim Manager
  Description: The source code combines two styles - console for debugging
	       and NT service for release. Code generation is controled by
	       conditional compiling
  Author     : Jibin 11/97

*/

/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <process.h>

#include "basictyp.h"
#include "pte.h"
#include "ntutils.h"
#include "ptecom.h"


/*************************************************************************************/
/*************************************************************************************/
int connection_type = DM_PORTTYPE_TCP;;
int pbsn_tcpip_port = 1401;
int volatile program_done;

LOCAL_DM_CONFIG dm_config;
SERIAL_BLOCK    serial_block;

BYTE incoming_buf[MAX_INCOMING_BUFFER_SIZE];

extern int  volatile connection_up;

/* Functions */
extern INT  serial_convert();
extern INT  tcp_convert();

/*************************************************************************************/
/*************************************************************************************/
void set_connection_status(INT status)
{
  connection_up = status;
}

/*************************************************************************************/
/*************************************************************************************/
void terminate_connection()
{
  if (port_close) port_close(connection_type);
  set_connection_status(FALSE);
}

/*************************************************************************************/
/*************************************************************************************/
INT nac_connection()
{
  INT dm_conn_successful=0;
  INT nofield = 0;

  switch (connection_type)
  {
	  case DM_PORTTYPE_SERIAL: 
      dm_conn_successful = serial_convert(serial_block.pbsn_com_port, serial_block.pbsn_baud_rate, 
		                                      serial_block.pbsn_byte_size, serial_block.pbsn_parity, 
						                              serial_block.pbsn_stop_bits, incoming_buf);
    	break;

    case DM_PORTTYPE_TCP:
      if (pbsn_tcpip_port <= 1024)
         LogEvent("invalid socket port number, must be bigger than 1024", INFO_MSG);
      else
	      dm_conn_successful = tcp_convert(pbsn_tcpip_port, incoming_buf);
      break;

    default: 
      LogEvent("illegal value for connection type", INFO_MSG);
  }

  if(dm_conn_successful) 
    set_connection_status(true);
  else 
    set_connection_status(false);
  return(dm_conn_successful);
}

/*************************************************************
 *************************************************************/
void check_connection()
{
  if(connection_up)
    return;

  terminate_connection();

  if (!nac_connection())
    LogEvent("PINNACLE: check_connection, hostsim reconnection failed", ERROR_MSG); 
}

/*********************************************************************************
 *********************************************************************************/
void connection_spy()
{

  while (TRUE)
  {
    check_connection();
    Sleep(3000);

    if(program_done)
      break;
  }
  _endthread();
}

/*************************************************************************************/
/*************************************************************************************/
INT establish_connection()
{
  if (!nac_connection()) 
  {
    LogEvent("nac connection not established" , INFO_MSG);
    return false;
  }
  else 
  {
    if (connection_type == DM_PORTTYPE_TCP)
    {
      _beginthread((void *)connection_spy, 0, NULL);
    }
    return(TRUE);
  }
}

/*************************************************************************************/
/*************************************************************************************/
void config_hostsim()
{
  strcpy(serial_block.pbsn_com_port, dm_config.db_com_port);
  strcpy(serial_block.pbsn_baud_rate, dm_config.db_baud_rate);
  strcpy(serial_block.pbsn_byte_size, dm_config.db_byte_size);
  strcpy(serial_block.pbsn_parity, dm_config.db_parity);
  strcpy(serial_block.pbsn_stop_bits, dm_config.db_stop_bits);
}

/*************************************************************************************/
/*************************************************************************************/
void MainProcessor(int conn_type)
{
  program_done = FALSE;
  set_connection_status(false);  
  config_hostsim();
  establish_connection();
}

/*************************************************************************************/
/*************************************************************************************/
void EndProcess()
{
  /* make sure we terminate the connection spy thread */
  program_done = TRUE;
  Sleep(4000);
	terminate_connection();
  LogEvent("pinnacle hostsim manager service is shutting down", INFO_MSG);
}
