/******************************************************************************
  
        Module: ptetcp.h
  
         Title: Ascendent TCP/IP library routines
  
   Description: This module contains the routines for communicating with the
                Ascendent Applink Server.

   $Log:   N:\PVCS\PTE\CORE\ptetcp_lib\ptetcp.h  $
   
      Rev 1.0   10 Aug 1998 14:23:52   rmalathk
   Initial Revision
   1. This revision is not thread safe.
   2.  need to handle fragmentation in ptetcp_send.
   

******************************************************************************/   
#ifndef __PTETCP__
#define __PTETCP__

#include "basictyp.h"
#include "ptemsg.h"

extern CHAR interactive_que_name[];

INT ptetcp_init( pCHAR error_msg );
INT ptetcp_connect( pCHAR host_addr, pCHAR error_msg );
INT ptetcp_disconnect();


INT ptetcp_send( pPTE_MSG p_msg_in, pCHAR error_msg );
pPTE_MSG ptetcp_receive( LONG time_out, pCHAR error_msg );
pPTE_MSG ptetcp_send_receive( pPTE_MSG p_msg_out, LONG time_out, pCHAR error_msg );

//MONITOR 2
INT ptetcp_connect2( pCHAR host_addr, pCHAR error_msg );
INT ptetcp_disconnect2();
INT ptetcp_send2( pPTE_MSG p_msg_in, pCHAR error_msg );
pPTE_MSG ptetcp_receive2( LONG time_out, pCHAR error_msg );
pPTE_MSG ptetcp_send_receive2( pPTE_MSG p_msg_out, LONG time_out, pCHAR error_msg );

//MONITOR 3
INT ptetcp_connect3( pCHAR host_addr, pCHAR error_msg );
INT ptetcp_disconnect3();
INT ptetcp_send3( pPTE_MSG p_msg_in, pCHAR error_msg );
pPTE_MSG ptetcp_receive3( LONG time_out, pCHAR error_msg );
pPTE_MSG ptetcp_send_receive3( pPTE_MSG p_msg_out, LONG time_out, pCHAR error_msg );

//MONITOR 4
INT ptetcp_connect4( pCHAR host_addr, pCHAR error_msg );
INT ptetcp_disconnect4();
INT ptetcp_send4( pPTE_MSG p_msg_in, pCHAR error_msg );
pPTE_MSG ptetcp_receive4( LONG time_out, pCHAR error_msg );
pPTE_MSG ptetcp_send_receive4( pPTE_MSG p_msg_out, LONG time_out, pCHAR error_msg );

#endif