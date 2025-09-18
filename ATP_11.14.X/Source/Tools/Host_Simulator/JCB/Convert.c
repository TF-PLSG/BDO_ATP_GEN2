/******************************************************************************
  
        Module: convert.c
  
         Title: Host Sim API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\Hostsim_Jcb\files\CONVERT.C  $
   
      Rev 1.0   Jun 18 1999 13:41:26   rmalathk
   Initial release
   
      Rev 1.0   Oct 14 1998 14:55:38   drodden
   Initial checkin of the ISO Retail host simulator.
   
   
      Rev 1.2   09 Apr 1998 13:07:06   MSALEH
   Modified some files to use the new 
   ptecom library and header file structures
   
      Rev 1.1   18 Mar 1998 13:47:08   MSALEH
   1) Use the new ptecom library
   2) cleanup code and files
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "basictyp.h"
#include "pte.h"
#include "ptecom.h"


extern void host_notify( LPBYTE p_req, WORD len, pIPC_INFO p_ipc_info );


IPC_INFO            ipc_info;

USER_SERIAL_STRUCT  sd;
USER_TCP_STRUCT	    td;
USER_DC_STRUCT      dc;

//***********************************************************
//***********************************************************
BOOL serial_convert(pCHAR pComPort, 
                    pCHAR pBaud        , pCHAR pByte,  
                    pCHAR pParity      ,  pCHAR pStop, 
                    pCHAR pBuffer)
{
  if(!pComPort || !pBaud || !pByte || !pParity || !pStop)
    return(FALSE);

  // COM port so populate DM_SERIAL_STRUCT
  memset(&sd, 0, sizeof(sd));

  strncpy(sd.cPort, pComPort, 4);
  sscanf(pBaud, "%lu", &sd.dwBaudRate);
  
  if(strncmp(pByte, "7", 1) == 0)
    sd.bByteSize = DATABITS_7;
  else
    if(strncmp(pByte, "8", 1) == 0)
      sd.bByteSize = DATABITS_8;

  if(strncmp(pParity, "none", 4) == 0)
    sd.bParity = NOPARITY;
  else
    if(strncmp(pParity, "odd", 3) == 0)
      sd.bParity = ODDPARITY;
    else
      if(strncmp(pParity, "even", 4) == 0)
        sd.bParity = EVENPARITY;

  if(strncmp(pStop, "1", 3) == 0)
    sd.bStopBits = ONESTOPBIT;
  else
    if(strncmp(pStop, "1.5", 3) == 0)
      sd.bStopBits = ONE5STOPBITS;
    else
      if(strncmp(pStop, "2", 1) == 0)
        sd.bStopBits = TWOSTOPBITS;

  sd.lpBuf      = (LPBYTE) pBuffer;
  sd.wBufferLen = MAX_OUTGOING_BUFFER_SIZE;
  sd.pfNotify   = host_notify;

  memset(&ipc_info, 0, sizeof(IPC_INFO));
  strcpy(ipc_info.instancename, "@pte_ipc");
  strcpy(ipc_info.loginname, "hostsim");

  if(port_init(DM_PORTTYPE_SERIAL, &sd, &ipc_info) == FALSE)
  {
    return(FALSE);
  }

  return(TRUE);
}	    

//***********************************************************
//***********************************************************
BOOL tcp_convert(u_short tcp_port, pCHAR pBuffer)
{
  // TCP/IP so populate USER_TCP_STRUCT
  memset(&td, 0, sizeof(td));
  td.usPort     = tcp_port;
  td.lpBuf      = (LPBYTE)pBuffer;
  td.wBufferLen = MAX_OUTGOING_BUFFER_SIZE;
  td.pfNotify   = host_notify;

  memset(&ipc_info, 0, sizeof(IPC_INFO));
  strcpy(ipc_info.instancename, "@pte_ipc");
  strcpy(ipc_info.loginname, "hostsim");

  if(port_init(DM_PORTTYPE_TCP, &td, &ipc_info) == FALSE)
  {
    return(FALSE);
  }

  return(TRUE);
}

/***********************************************************
 ***********************************************************/
BOOL direct_tcp_convert(u_short host_tcp_port, 
                        pCHAR   host_name, 
                        pCHAR   pBuffer)
{
  // TCP/IP so populate USER_TCP_STRUCT
  memset(&dc, 0, sizeof(dc));
  dc.DirectPort   = host_tcp_port;
  dc.HostName     = host_name;
  dc.lpBuf        = (LPBYTE) pBuffer;
  dc.wBufferLen   = MAX_OUTGOING_BUFFER_SIZE;
  dc.pfNotify     = host_notify;

  memset(&ipc_info, 0, sizeof(IPC_INFO));
  strcpy(ipc_info.instancename, "@pte_ipc");
  strcpy(ipc_info.loginname, "hostsim");

  if(port_init(DM_PORTTYPE_DIRECT, &dc, &ipc_info) == FALSE)
  {
    return(FALSE);
  }

  return(TRUE);
}
