/*******************************************************************************
*  
* MODULE:      txdsapi.c
*  
* TITLE:       
*  
* DESCRIPTION:
*
* APPLICATION: ATP library
*
* AUTHOR:  Ram Malathkar
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\Source\TXDSAPI.C  $
   
      Rev 1.31   Jun 24 2005 09:33:42   dirby
   Added support for Fraudguard
   SCR 14421
   
      Rev 1.30   Dec 20 2004 13:38:02   dirby
   Modified to not free the PTE message when an error comes back
   from the database.  Let it get freed in the same place it does when
   there is no error.  Did this so I could rework the logic after an update
   all to return the auth_tx buffer.  On an error, the auth_tx buffer was
   not being returned; the original one was, but if the data server made
   any changes to it, you would not see those changes.  Updatds has
   been modified to return an audit trail in TLF01.dcf01_retired_cwk
   for update all commands.
   SCR 12446
   
      Rev 1.29   Dec 05 2003 08:46:00   dirby
   Added que_name as an input parameter to txdsapi_send_amex_record.
   This is because EB changes the service names of the Amex
   Network Controllers.  This value will be can be input on the GUI.
   SCR 1200
   
      Rev 1.28   Nov 21 2003 16:36:02   dirby
   Added function for Amex Online Updates
   SCR 1200
   
      Rev 1.27   Mar 03 2003 11:12:54   dirby
   Modified function send_mcc_record to combine the 2 request/
   response structures into a single structure.  Also restructured
   the whole function to be more efficient.
   SCR 891
   
      Rev 1.26   Jul 03 2002 12:28:58   dirby
   Modified to include windows.h only if building Atptcp.lib.
   Modified to use variables (connect_str, ini_file, and i1) only if
   building Atptcp.lib.
   These changes are to allow the library to build on Unix and NT
   without any warnings or errors.
   SCR 803
   
      Rev 1.25   May 01 2002 15:55:16   jgrguric
   Added new configuration logic for CONNECTMODE
   parameter to allow multiple or single connections.
   
      Rev 1.24   Feb 19 2002 17:06:18   jgrguric
   Modify process_request in txdsapi.c to fix random overwrite
   problem of automatic buffer. Also, shorten the error message
   on send/receive errors to allow the ptetcp.lib auto reconnect
   message to be seen on the screen.
   
      Rev 1.23   Nov 02 2001 16:24:06   dirby
   Added function 'txdsapi_send_jcb_record'.
   This is for JCB Online File Updates.
   SCR 251
   
      Rev 1.22   Jun 13 2001 14:24:38   dirby
   Revert back to revision 1.20.  Revision 1.21 contains Base product
   stuff that is not currently in Equitable.
   
      Rev 1.21   Dec 01 2000 18:08:50   rmalathk
   1.updated txdsapi to add UCF03 and UCF03L
   2. updated project settings to use core 1_3_3 and equitable 3_2_3.
   
      Rev 1.20   Mar 15 2000 17:47:54   iarustam
   added code for alpha_lookup
   
      Rev 1.19   Jan 14 2000 11:17:36   iarustam
   send insert tlf01 to the updateds
   
      Rev 1.18   Dec 20 1999 14:03:42   mbysani
   changes made to include the repds in lib
   
      Rev 1.18   Dec 17 1999 16:38:16   mbysani
   changes made to include the repds in lib
   
      Rev 1.17   Nov 17 1999 17:19:04   mbysani
    
   
      Rev 1.16   Nov 16 1999 14:13:18   mbysani
    
   
      Rev 1.15   Nov 16 1999 11:19:12   mbysani
   Added 3 functions
   
      Rev 1.15   Nov 16 1999 09:50:40   mbysani
    
   
      Rev 1.14   Jun 04 1999 17:15:30   iarustam
   Added function find_bin_range
   
      Rev 1.13   Mar 24 1999 17:09:06   iarustam
   bug fixes
   
      Rev 1.12   Mar 24 1999 16:58:58   iarustam
   added the new data_type
   
      Rev 1.11   Mar 12 1999 13:26:44   iarustam
   Bug fixes
   
      Rev 1.10   Feb 26 1999 14:50:28   iarustam
   Bug fixes for delete function
   
      Rev 1.9   Feb 24 1999 17:14:28   iarustam
   Added reconnect function
   
      Rev 1.8   Feb 23 1999 07:49:18   iarustam
   GUI change
   
      Rev 1.7   Feb 20 1999 14:28:50   iarustam
   Added send_voice_auth_tran
   
   
      Rev 1.6   Feb 18 1999 17:04:54   iarustam
    
   
      Rev 1.5   Feb 12 1999 18:02:56   iarustam
   Added logic to send messages to all
   dataservers
   
      Rev 1.4   08 Feb 1999 15:53:22   dirby
   Added function to get batch for NCF30.
   
      Rev 1.3   05 Feb 1999 13:40:18   dirby
   Modified 'insert_record' to send to netds_que for
   Network Control tables.
   
      Rev 1.2   Feb 03 1999 15:25:36   IGOLDFIE
   Added select_bch01_service_list and
   select_tlf01_service_list functions.
   
      Rev 1.1   Feb 02 1999 15:39:04   IGOLDFIE
   Changed include files and also changed
   que_names for some fuctions.
   
      Rev 1.0   01 Feb 1999 16:24:48   jhapitas
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:58:18   MSALEH
   Initial revision
*
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>

// Following line added by JMG on 4/26/02 for use of GetPrivateProfileString
#ifdef WIN32
   #include <windows.h>
#endif

#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "tx_dbstruct.h"
#include "nc_dbstruct.h"
#include "txdsapi.h"
#include <string.h>
#include "ntutils.h"
#include <time.h>

#ifdef XIPC
   #include "pteipc.h"
#else
   #include "ptetcp.h"
#endif

#define MONITOR1  1
#define MONITOR2  2
#define MONITOR3  3
#define MONITOR4  4
#define CLEAR     5
#define LOAD		"LOADING..."
#define SUCCESS		"SUCCESS"
#define FAIL		"FAILED"
#define NOT_DEFINED "NOT DEFINED"

int resetCFileFlag = 0;
int IsMon2Up = FALSE, IsMon3Up = FALSE, IsMon4Up = FALSE;
char mon1_status[10] = {0}, mon2_status[10] = {0}, mon3_status[10] = {0}, mon4_status[10] = {0};
PRIVATE CHAR trands_que_name  [] = "trandsA";
PRIVATE CHAR updateds_que_name[] = "updatdsA";
PRIVATE CHAR devds_que_name   [] = "devdsA";
PRIVATE CHAR netds_que_name   [] = "netdsA";
PRIVATE CHAR repds_que_name   [] = "repdsA";
PRIVATE CHAR fguard_que_name  [] = "fguardA";
PRIVATE CHAR utilds_que_name  [] = "trandsA";
PRIVATE CHAR archds_que_name  [] = "archdsA";
PRIVATE CHAR posds_que_name   [] = "posdsA";
PRIVATE CHAR mrads_que_name   [] = "mradsA";
PRIVATE CHAR reps_que_name	  [] = "repdsA";
PRIVATE CHAR fconds_que_name  [] = "fcondsA";
/*********************************************************************************************
/*********************************************************************************************/
void txdsapi_free ( pPTE_MSG p_msg_in)
{
	#ifdef XIPC
		free( p_msg_in );
   #endif
}


/*********************************************************************************************
/*********************************************************************************************/
PRIVATE BYTE process_request( pPTE_MSG p_msg_out,
                              pPTE_MSG *p_msg_in,
                              pCHAR error_msg,
                              BOOLEAN route_via_logger )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;

   // Next three variables added on 4/26/02 by JMG for new connection logic.
#ifndef XIPC
   CHAR connect_str[25];
   CHAR ini_file[256];
   int i1;
#endif

	// JMG Mod on 1/7/02 to prevent possible automatic buffer overwrite from below
	// Increased buffer size from 100 to 200. Really should use a global define for
	// the size of message buffer, but this will do for now.
	CHAR			temp_str[200];
	BYTE			dberror;
   pCHAR       dest_que;
   LONG			timeout;

   // Following line added by JMG on 4/26/02
   CHAR        strFileRecord[56] = "";


   #ifdef XIPC

   LONG			retcode;
   CHAR        que_name[12];
   #endif

   timeout = 30;   // in seconds
	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

   dest_que = ptemsg_get_msg_dest_queue( p_msg_out );

   #ifdef XIPC    // using xipc
/*
      if( route_via_logger )
         strcpy( que_name, logger_que_name );
      else
*/
      strcpy( que_name, dest_que );
	   *p_msg_in = pteipc_send_receive( p_msg_out, que_name, interactive_que_name, &retcode );

   #else       // using TCP/IP via applink
/*
      if( route_via_logger )
      {
         sprintf( que_name, "@%s", dest_que );
         ptemsg_set_dest_queue( p_msg_out, que_name );
      }
*/

// Following connection logic added by JMG on 4/26/02, as we can now connect on each operation
// This change made in concert with atptcp.lib change to process_request() to connect
// and disconnect on each call, based on CONNECTMODE parameter in tf.ini.

   // Get the connection configuration parameter "CONNECTMODE". If not present
   // default to "MULTIPLE". The other valid value is "SINGLE" to enable connect and
   // disconnect only at start up and exit. All other values same as MULTIPLE.
   GetAscendentConfigDirectory( ini_file );
   strcat( ini_file, "tf.ini" );

   (void) GetPrivateProfileString(
        "GUI", 	                   // points to section name 
        "CONNECTMODE",	           // points to key name 
        "multiple",	               // points to default string 
        connect_str,               // points to destination buffer 
        sizeof(connect_str) - 1,   // size of destination buffer 
        ini_file );                // points to initialization filename 

   connect_str[sizeof(connect_str)] = 0; //Make sure string is terminated
   i1 = 0;
   while ( 0 != connect_str[i1] )
      connect_str[i1++] = toupper(connect_str[i1]); // Convert string to upper case

   if ( 0 != strcmp("SINGLE", connect_str) )
   {
      // Configured for MULTIPLE applnk connections - on each operation
      if ( !GetFirstServerId (strFileRecord))
      {
         strcpy( error_msg, "Missing Primary Machine ID in registry. Run the executable from installation Bin directory." );
         return PTEMSG_LOGIN_FAILED;
      }

      if( !ptetcp_connect( strFileRecord, error_msg ) )
      {
         strcpy( error_msg, "Connection to applnk failed" );
         return PTEMSG_LOGIN_FAILED;
      }
   }

   *p_msg_in = ptetcp_send_receive( p_msg_out, timeout, temp_str );

   if ( 0 != strcmp("SINGLE", connect_str) )
   {
      // Configured for MULTIPLE applnk connections - on each operation
      ptetcp_disconnect();
   }
   #endif

   if( *p_msg_in == NULL_PTR )
   {
      #ifdef XIPC
         pteipc_get_errormsg( retcode, temp_str );
      #endif
      //JMG - 1/7/02 - Shortened message to allow room in window
      //               for message from below to be seen!
      sprintf( error_msg, "Comm Error: %s", temp_str );
	  write_to_reset_file_c(error_msg);
      return PTEMSG_IPC_ERROR;
   }

   if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      strncpy( temp_str, (pCHAR)p_data, sizeof ( temp_str) );
      sprintf( error_msg, "Database Error: %s", temp_str );
	  return dberror;
   }
   sprintf( error_msg, "dberror : %d", dberror);

   return dberror;
}

PRIVATE BYTE process_request2( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in, pCHAR error_msg, BOOLEAN route_via_logger )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	CHAR			temp_str[200];
	BYTE			dberror;
    pCHAR			dest_que;
    LONG			timeout;
    CHAR			host_name2[50] = "";

	timeout = 15;   // in seconds(30)

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "GH2:Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

	if(IsMon2Up == FALSE)
	{
		if(!GetThisMachineId_2( host_name2 ))
	{
		strcpy( error_msg, "GH2:Failed to get machine Id 2" );
		return PTEMSG_NOT_FOUND;
	}

		if( !ptetcp_connect2( host_name2, error_msg ) )
    {
        strcpy( error_msg, "GH2:Connection to applnk failed" );
        return PTEMSG_LOGIN_FAILED;
    }
	}

    *p_msg_in = ptetcp_send_receive2( p_msg_out, timeout, temp_str );

	ptetcp_disconnect2();
	IsMon2Up = FALSE;

    if( *p_msg_in == NULL_PTR )
    {
       //JMG - 1/7/02 - Shortened message to allow room in window
       //               for message from below to be seen!
       sprintf( error_msg, "GH2:Comm Error: %s", temp_str );
	   write_to_reset_file_c(error_msg);
       return PTEMSG_IPC_ERROR;
    }

    if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
    {
       p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
       p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
       data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
       p_data        = p_data + sizeof(AUTH_TX);

       strncpy( temp_str, (pCHAR)p_data, sizeof ( temp_str) );
       sprintf( error_msg, "GH2:Database Error: %s", temp_str );
	   return dberror;
    }
	sprintf( error_msg, "GH2:dberror : %d", dberror);

    return dberror;
}

PRIVATE BYTE process_request3( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in, pCHAR error_msg, BOOLEAN route_via_logger )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	CHAR			temp_str[200];
	BYTE			dberror;
    pCHAR			dest_que;
    LONG			timeout;
    CHAR			host_name3[50] = "";

	timeout = 15;   // in seconds(30)

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "MKT1:Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

	if(IsMon3Up == FALSE)
	{
		if(!GetThisMachineId_3( host_name3 ))
	{
		strcpy( error_msg, "MKT1:Failed to get machine Id 3" );
		return PTEMSG_NOT_FOUND;
	}

		if( !ptetcp_connect3( host_name3, error_msg ) )
    {
        strcpy( error_msg, "MKT1:Connection to applnk failed" );
        return PTEMSG_LOGIN_FAILED;
    }
	}

    *p_msg_in = ptetcp_send_receive3( p_msg_out, timeout, temp_str );

	ptetcp_disconnect3();
	IsMon3Up = FALSE;

    if( *p_msg_in == NULL_PTR )
    {
       //JMG - 1/7/02 - Shortened message to allow room in window
       //               for message from below to be seen!
       sprintf( error_msg, "MKT1:Comm Error: %s", temp_str );
	   write_to_reset_file_c(error_msg);
       return PTEMSG_IPC_ERROR;
    }

    if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
    {
       p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
       p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
       data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
       p_data        = p_data + sizeof(AUTH_TX);

       strncpy( temp_str, (pCHAR)p_data, sizeof ( temp_str) );
       sprintf( error_msg, "MKT1:Database Error: %s", temp_str );
	   return dberror;
    }
	sprintf( error_msg, "MKT1:dberror : %d", dberror);

    return dberror;
}

PRIVATE BYTE process_request4( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in, pCHAR error_msg, BOOLEAN route_via_logger )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	CHAR			temp_str[200];
	BYTE			dberror;
    pCHAR			dest_que;
    LONG			timeout;
    CHAR			host_name4[50] = "";

	timeout = 15;   // in seconds(30)

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "MKT2:Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

	if(IsMon4Up == FALSE)
	{
		if(!GetThisMachineId_4( host_name4 ))
	{
		strcpy( error_msg, "MKT2:Failed to get machine Id 4" );
		return PTEMSG_NOT_FOUND;
	}

		if( !ptetcp_connect4( host_name4, error_msg ) )
    {
        strcpy( error_msg, "MKT2:Connection to applnk failed" );
        return PTEMSG_LOGIN_FAILED;
    }
	}

    *p_msg_in = ptetcp_send_receive4( p_msg_out, timeout, temp_str );

	ptetcp_disconnect4();
	IsMon4Up = FALSE;

    if( *p_msg_in == NULL_PTR )
    {
       //JMG - 1/7/02 - Shortened message to allow room in window
       //               for message from below to be seen!
       sprintf( error_msg, "MKT2:Comm Error: %s", temp_str );
	   write_to_reset_file_c(error_msg);
       return PTEMSG_IPC_ERROR;
    }

    if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
    {
       p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
       p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
       data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
       p_data        = p_data + sizeof(AUTH_TX);

       strncpy( temp_str, (pCHAR)p_data, sizeof ( temp_str) );
       sprintf( error_msg, "MKT2:Database Error: %s", temp_str );
	   return dberror;
    }
	sprintf( error_msg, "MKT2:dberror : %d", dberror);

    return dberror;
}

BYTE process_dco_reset_connections( pCHAR error_msg )
{
	CHAR temp_str[200];
    CHAR local_host_name2[50] = "";
	CHAR local_host_name3[50] = "";
	CHAR local_host_name4[50] = "";

	write_to_reset_file_c("GH1: Already Connected");
	//Monitor2 Connection
	if(!GetThisMachineId_2( local_host_name2 ))
	{
		IsMon2Up = FALSE;
		strcpy( error_msg, "GH2:Failed to get machine Id 2" );
		write_to_reset_file_c(error_msg);
		return PTEMSG_NOT_FOUND;
	}

	if( !ptetcp_connect2( local_host_name2, error_msg ) )
    {
		IsMon2Up = FALSE;
        strcpy( error_msg, "GH2:Connection to applnk failed" );
		write_to_reset_file_c(error_msg);
        return PTEMSG_LOGIN_FAILED;
    }
	IsMon2Up = TRUE;
    write_to_reset_file_c("GH2: Connected");


    //Monitor3 Connection
	if(!GetThisMachineId_3( local_host_name3 ))
	{
		IsMon3Up = FALSE;
		strcpy( error_msg, "MKT1:Failed to get machine Id 3" );
		write_to_reset_file_c(error_msg);
		return PTEMSG_NOT_FOUND;
	}

	if( !ptetcp_connect3( local_host_name3, error_msg ) )
    {
		IsMon3Up = FALSE;
        strcpy( error_msg, "MKT1:Connection to applnk failed" );
		write_to_reset_file_c(error_msg);
        return PTEMSG_LOGIN_FAILED;
    }
	IsMon3Up = TRUE;
	write_to_reset_file_c("MKT1: Connected");


	//Monitor4 Connection
	if(!GetThisMachineId_4( local_host_name4 ))
	{
		IsMon4Up = FALSE;
		strcpy( error_msg, "MKT2:Failed to get machine Id 4" );
		write_to_reset_file_c(error_msg);
		return PTEMSG_NOT_FOUND;
	}

	if( !ptetcp_connect4( local_host_name4, error_msg ) )
    {
		IsMon4Up = FALSE;
        strcpy( error_msg, "MKT2:Connection to applnk failed" );
		write_to_reset_file_c(error_msg);
        return PTEMSG_LOGIN_FAILED;
    }
	IsMon4Up = TRUE;
	write_to_reset_file_c("MKT2: Connected");

    return PTEMSG_OK;
}

void process_disconnect_dco_reset_connections()
{
	int ret = -1;

	if(IsMon2Up == TRUE)
	{
		ret = ptetcp_disconnect2();
		if(ret == 1)
		{
			IsMon2Up = FALSE;
			write_to_reset_file_c("GH2: Disconnected");
		}
		else 
			write_to_reset_file_c("GH2: Failed to Disconnect");
	}

	if(IsMon3Up == TRUE)
	{
		ret = ptetcp_disconnect3();
		if(ret == 1)
		{
			IsMon3Up = FALSE;
			write_to_reset_file_c("MKT1: Disconnected");
		}
		else 
			write_to_reset_file_c("MKT1: Failed to Disconnect");
	}

	if(IsMon4Up == TRUE)
	{
		ret = ptetcp_disconnect4();
		if(ret == 1)
		{
			IsMon4Up = FALSE;
			write_to_reset_file_c("MKT2: Disconnected");
		}
		else
			write_to_reset_file_c("MKT2: Failed to Disconnect");
	}
}

char *get_reset_status(int mon)
{
	if(mon == MONITOR1)
		return mon1_status;
	else if(mon == MONITOR2)
		return mon2_status;
	else if(mon == MONITOR3)
		return mon3_status;
	else if(mon == MONITOR4)
		return mon4_status;
	else if(mon == CLEAR)
	{
		memset(mon1_status, 0, sizeof(mon1_status));
		memset(mon2_status, 0, sizeof(mon2_status));
		memset(mon3_status, 0, sizeof(mon3_status));
		memset(mon4_status, 0, sizeof(mon4_status));
	}

	return NOT_DEFINED;
}

/*********************************************************************************************/
PRIVATE BYTE process_request_update_user_status( pPTE_MSG p_msg_out,
                              pPTE_MSG *p_msg_in,
                              pCHAR error_msg,
                              BOOLEAN route_via_logger )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;

   // Next three variables added on 4/26/02 by JMG for new connection logic.
#ifndef XIPC
   CHAR connect_str[25];
   CHAR ini_file[256];
   int i1;
#endif

	// JMG Mod on 1/7/02 to prevent possible automatic buffer overwrite from below
	// Increased buffer size from 100 to 200. Really should use a global define for
	// the size of message buffer, but this will do for now.
	CHAR		temp_str[200];
	BYTE		dberror;
	BYTE		rcode1;
    pCHAR       dest_que;
    LONG		timeout;
	CHAR		str_timeout[5]={0};
	GPCF01	    gpcf01;
   // Following line added by JMG on 4/26/02
   CHAR        strFileRecord[56] = "";


   #ifdef XIPC

   LONG			retcode;
   CHAR        que_name[12];
   #endif

   timeout = 120;   // in seconds
	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

   dest_que = ptemsg_get_msg_dest_queue( p_msg_out );

   #ifdef XIPC    // using xipc
/*
      if( route_via_logger )
         strcpy( que_name, logger_que_name );
      else
*/
      strcpy( que_name, dest_que );
	   *p_msg_in = pteipc_send_receive( p_msg_out, que_name, interactive_que_name, &retcode );

   #else       // using TCP/IP via applink
/*
      if( route_via_logger )
      {
         sprintf( que_name, "@%s", dest_que );
         ptemsg_set_dest_queue( p_msg_out, que_name );
      }
*/

// Following connection logic added by JMG on 4/26/02, as we can now connect on each operation
// This change made in concert with atptcp.lib change to process_request() to connect
// and disconnect on each call, based on CONNECTMODE parameter in tf.ini.

   // Get the connection configuration parameter "CONNECTMODE". If not present
   // default to "MULTIPLE". The other valid value is "SINGLE" to enable connect and
   // disconnect only at start up and exit. All other values same as MULTIPLE.
   GetAscendentConfigDirectory( ini_file );
   strcat( ini_file, "tf.ini" );

   (void) GetPrivateProfileString(
        "GUI", 	                   // points to section name 
        "CONNECTMODE",	           // points to key name 
        "multiple",	               // points to default string 
        connect_str,               // points to destination buffer 
        sizeof(connect_str) - 1,   // size of destination buffer 
        ini_file );                // points to initialization filename 

   connect_str[sizeof(connect_str)] = 0; //Make sure string is terminated
   i1 = 0;
   while ( 0 != connect_str[i1] )
      connect_str[i1++] = toupper(connect_str[i1]); // Convert string to upper case

   if ( 0 != strcmp("SINGLE", connect_str) )
   {
      // Configured for MULTIPLE applnk connections - on each operation
      if ( !GetFirstServerId (strFileRecord))
      {
         strcpy( error_msg, "Missing Primary Machine ID in registry. Run the executable from installation Bin directory." );
         return PTEMSG_LOGIN_FAILED;
      }

      if( !ptetcp_connect( strFileRecord, error_msg ) )
      {
         strcpy( error_msg, "Connection to applnk failed" );
         return PTEMSG_LOGIN_FAILED;
      }
   }
   rcode1 = txdsapi_get_record( (pBYTE)&gpcf01,sizeof (GPCF01), GPCF01_DATA, error_msg );
   strncpy((char *)str_timeout,(char *)gpcf01.user_status_cntrl_tmeout_val,sizeof(gpcf01.user_status_cntrl_tmeout_val));
   timeout = atoi((char *)str_timeout);
   *p_msg_in = ptetcp_send_receive( p_msg_out, timeout, temp_str );

   if ( 0 != strcmp("SINGLE", connect_str) )
   {
      // Configured for MULTIPLE applnk connections - on each operation
      ptetcp_disconnect();
   }
   #endif

   if( *p_msg_in == NULL_PTR )
   {
      #ifdef XIPC
         pteipc_get_errormsg( retcode, temp_str );
      #endif
      //JMG - 1/7/02 - Shortened message to allow room in window
      //               for message from below to be seen!
      sprintf( error_msg, "Comm Error: %s", temp_str );
      return PTEMSG_IPC_ERROR;
   }

   if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      strncpy( temp_str, (pCHAR)p_data, sizeof ( temp_str) );
      sprintf( error_msg, "Database Error: %s", temp_str );
   }

   return dberror;
}


int txdsapi_reset_connection( pCHAR strErrorMessage )
{
   #ifndef XIPC
      char strFileRecord[56];

      ptetcp_disconnect();

      if ( !GetThisMachineId (strFileRecord))
      {
         strcpy (strErrorMessage,"Missing Primary Machine ID in registry. Run the executable from installation Bin directory.");
         return 0;
      }

      if( !ptetcp_connect( strFileRecord, strErrorMessage ) )
      {
         return 0;
      }
      return 1;
   #endif

   return 1;
}

//Function added to clear the monitor sockets

BYTE txdsapi_clear_monitor_sockets( pCHAR error_msg )
{
	pPTE_MSG   p_msg_out;
	pPTE_MSG   p_msg_in;
	BYTE       rcode = 0;
	CHAR       que_name[8];
	CHAR       buffer[8];
	strcpy( que_name, netds_que_name );
	memset( buffer, 0, sizeof( buffer ) );
	
	p_msg_in = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,ST1_MONITOR_CLEAR_SOCKETS,
									0,
									que_name,
									interactive_que_name,
									(BYTE *)buffer,2,
									0 );
	
	rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
/*	if( !ptetcp_send( p_msg_out, error_msg ) )
	{
		rcode = -1;
	}
*/	txdsapi_free( p_msg_in );
	return ( rcode );
}

BYTE txdsapi_clear_monitor_sockets_all_new( int Mon )
{
	pPTE_MSG   p_msg_out;
	pPTE_MSG   p_msg_in;
	BYTE       rcode = 0;
	CHAR       que_name[8];
	CHAR       buffer[8];
	CHAR       error_msg[200] = {0};

	strcpy( que_name, netds_que_name );
	memset( buffer, 0, sizeof( buffer ) );
	
	p_msg_in = NULL_PTR;
	p_msg_out = ptemsg_build_msg( MT_SYS_MONITOR,ST1_MONITOR_CLEAR_SOCKETS,
									0,
									que_name,
									interactive_que_name,
									(BYTE *)buffer,2,
									0 );
	
	sprintf(error_msg, "Calling - process_request : [%d]", Mon);
	write_to_reset_file_c(error_msg);
	memset(error_msg, 0, sizeof(error_msg));

	if(Mon == 1)
	{
		rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
		if(rcode == PTEMSG_OK)
			strcpy(mon1_status, SUCCESS);
		else
			strcpy(mon1_status, FAIL);
	}
	else if(Mon == 2)
	{
		rcode = process_request2( p_msg_out, &p_msg_in, error_msg, true );
		if(rcode == PTEMSG_OK)
			strcpy(mon2_status, SUCCESS);
		else
			strcpy(mon2_status, FAIL);
	}
	else if(Mon == 3)
	{
		rcode = process_request3( p_msg_out, &p_msg_in, error_msg, true );
		if(rcode == PTEMSG_OK)
			strcpy(mon3_status, SUCCESS);
		else
			strcpy(mon3_status, FAIL);
	}
	else if(Mon == 4)
	{
		rcode = process_request4( p_msg_out, &p_msg_in, error_msg, true );
		if(rcode == PTEMSG_OK)
			strcpy(mon4_status, SUCCESS);
		else
			strcpy(mon4_status, FAIL);
	}

	if(rcode != PTEMSG_OK)
		write_to_reset_file_c(error_msg);
	else
	{
	sprintf(error_msg, "Completed - process_request : [%d]", Mon);
		write_to_reset_file_c(error_msg);
}

	txdsapi_free( p_msg_in );
	return rcode;
	}


/*************************************************************************
/*************************************************************************/
BYTE txdsapi_delete_record( pBYTE psStructurePtr, int nStructureSize, 
                            BYTE  app_data_type,
                            pCHAR error_msg )
{
   pPTE_MSG   p_msg_out;
   pPTE_MSG   p_msg_in;
   BYTE       rcode;
   CHAR       buffer[sizeof(AUTH_TX) + 4000];
   CHAR       que_name[8];

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);

    switch( app_data_type )
    {
       case  NCF01_DATA:
       case  NCF02_DATA:
       case  NCF20_DATA:
       case  NCF21_DATA:
       case  NCF30_DATA:
       case  NEG01_DATA:
       case  ATM01_DATA:
       case  FG01_DATA:
	   case OBS01_DATA:
	   case VISA3DS2_DATA:
	   case NCF01_KEYBLOCK01_DATA:
          strcpy( que_name, netds_que_name );
          break;

       case  OCF01_DATA:
       case  OCF20_DATA:
       case  MCF01_DATA:
       case  MCF20_DATA:
       case  DCF01_DATA:
       case  BCH01_DATA:
       case  BCH10_DATA:
       case  BCH11_DATA:
       case  BCH20_DATA:
       case  NGF01_DATA:
       case  NFI01_DATA:
       case  NFI02_DATA:
       case  NMF01_DATA:
       case  NTF01_DATA:
       case  NBF01_DATA:
       case  DRF01_DATA:
    //   case  DLF01_DATA:
       case  TLF01_DATA:
	   case  MGF01_DATA:
	       strcpy( que_name, devds_que_name );
          break;
	   case	ARCH01_DATA:
		   strcpy( que_name, archds_que_name );
		   break;
	   case FCLF01_DATA:
	   case FRULE01_DATA:
		   strcpy( que_name, fconds_que_name );
		   break;
       default:
          strcpy( que_name, trands_que_name );
          break;
    }

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_DELETE,
                                 0,
                                 que_name,
                                 interactive_que_name,
                                 (BYTE *)buffer,
                                 sizeof(AUTH_TX) + nStructureSize,
                                 app_data_type );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}  

/*************************************************************************
/*************************************************************************/

BYTE txdsapi_insert_record( pBYTE psStructurePtr, int nStructureSize, 
                            BYTE  app_data_type,
                            pCHAR error_msg )
{
   pPTE_MSG   p_msg_out;
   pPTE_MSG   p_msg_in;
   BYTE       rcode;
   CHAR       que_name[8];
   CHAR       buffer[sizeof(AUTH_TX) + 7000];


    memset( buffer, 0, sizeof( buffer ) );
    memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);
    switch( app_data_type )
    {
       case  NCF01_DATA:
       case  NCF02_DATA:
       case  NCF20_DATA:
       case  NCF21_DATA:
       case  NCF30_DATA:
       case  NEG01_DATA:
       case  ATM01_DATA:
       case  FG01_DATA:
		case  OBS01_DATA:
		case VISA3DS2_DATA:
		case NCF01_KEYBLOCK01_DATA:

          strcpy( que_name, netds_que_name );
          break;

       case  OCF01_DATA:
       case  OCF20_DATA:
       case  MCF01_DATA:
       case  MCF20_DATA:
       case  DCF01_DATA:
       case  BCH01_DATA:
       case  BCH10_DATA:
       case  BCH11_DATA:
       case  BCH20_DATA:
       case  NGF01_DATA:
       case  NFI01_DATA:
       case  NFI02_DATA:
       case  NMF01_DATA:
       case  NTF01_DATA:
       case  NBF01_DATA:
       case  DRF01_DATA:
       case  BCH01_BCH10_TLF01_DATA:
	   case  MGF01_DATA:
          strcpy( que_name, devds_que_name );
          break;
       case  TLF01_DATA:
          strcpy( que_name, updateds_que_name );
          break;
	   case	ARCH01_DATA:
		   strcpy( que_name, archds_que_name );
		   break;
	   case FCLF01_DATA:
	   case FRULE01_DATA:
		   strcpy( que_name, fconds_que_name );
		   break;
       default:
          strcpy( que_name, trands_que_name );
          break;
    }

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_INSERT,
                                 0,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
} 

/*************************************************************************
/*************************************************************************/

BYTE txdsapi_update_record( pBYTE psStructurePtr, int nStructureSize, 
                            BYTE  app_data_type,
                            pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   char      buffer[sizeof(AUTH_TX) + 4000];
   CHAR      que_name[8];



   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);
    
   switch( app_data_type )
   {
       case  NCF01_DATA:
       case  NCF02_DATA:
       case  NCF20_DATA:
       case  NCF21_DATA:
       case  NCF30_DATA:
       case  NEG01_DATA:
       case  ATM01_DATA:
       case  FG01_DATA:
	   case	 OBS01_DATA:
	   case  VISA3DS2_DATA:
	   case NCF01_KEYBLOCK01_DATA:
          strcpy( que_name, netds_que_name );
          break;

       case  OCF01_DATA:
       case  OCF20_DATA:
       case  MCF01_DATA:
       case  MCF20_DATA:
       case  DCF01_DATA:
       case  BCH01_DATA:
       case  BCH10_DATA:
       case  BCH11_DATA:
       case  BCH20_DATA:  
       case  NGF01_DATA:
       case  NFI01_DATA:
       case  NFI02_DATA:
       case  NMF01_DATA:
       case  NTF01_DATA:
       case  NBF01_DATA:
       case  DRF01_DATA:
       case  TLF01_DATA:
	   case  MGF01_DATA:
          strcpy( que_name, devds_que_name );
          break;
       case  ALL_DATA:
          strcpy( que_name, updateds_que_name );
          break;
	   case	ARCH01_DATA:
		   strcpy( que_name, archds_que_name );
		   break;
	   case FCLF01_DATA:
	   case FRULE01_DATA:
		   strcpy( que_name, fconds_que_name );
	   break;
       default:
          strcpy( que_name, trands_que_name );
          break;
    }

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 0,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
} 

/************************************************************************/
/*************************************************************************/

BYTE txdsapi_update_bch01_record( pBYTE psStructurePtr, int nStructureSize, 
                                  BYTE  app_data_type,
                                  pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   CHAR      buffer[sizeof(AUTH_TX) + 4000];


   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 ST2_DB_UPDATE_BCH01_TO_CLOSE_BATCH,
                                 devds_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );    

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
} 

/************************************************************************/
/*************************************************************************/

BYTE txdsapi_update_bch10_record( pBYTE psStructurePtr, int nStructureSize, 
                                  BYTE  app_data_type,
                                  pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   CHAR      buffer[sizeof(AUTH_TX) + 4000];

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 ST2_DB_UPDATE_BCH10_TO_CLOSE_BATCH,
                                 devds_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );    

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
} 

/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_record( pBYTE psStructurePtr, int nStructureSize, 
                         BYTE  app_data_type,
                         pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode = 0;
   char           buffer[sizeof(AUTH_TX) + 4000] = {0};
   BYTE           st_1 = 0,st_2 = 0, data_type =0;
   CHAR           que_name[8] = {0};

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);

  

   switch( app_data_type )
   {
       case  NCF01_DATA:
       case  NCF02_DATA:
       case  NCF20_DATA:
       case  NCF21_DATA:
       case  NCF30_DATA:
       case  NEG01_DATA:
       case  ATM01_DATA:
       case  FG01_DATA:
	   case  OBS01_DATA:
	   case VISA3DS2_DATA:
	   case NCF01_KEYBLOCK01_DATA:
          strcpy( que_name, netds_que_name );
          break;

       case  OCF01_DATA:
       case  OCF20_DATA:
       case  MCF01_DATA:
       case  MCF20_DATA:
       case  DCF01_DATA:
       case  BCH01_DATA:
       case  BCH10_DATA:
       case  BCH11_DATA:
       case  BCH20_DATA:
       case  NGF01_DATA:
       case  NFI01_DATA:
       case  NFI02_DATA:
       case  NMF01_DATA:
       case  NTF01_DATA:
       case  NBF01_DATA:
       case  DRF01_DATA:
	   case  MGF01_DATA:
          strcpy( que_name, devds_que_name );
          break;

       case  TLF01_DATA:
	   case  TLF01_STIP_DATA:
       case  FGUARD_DATA:
	   case  MRA01_DATA:
	  case FTLF01_DATA:
	  case PTSN01_DATA:
          strcpy( que_name, repds_que_name );
          break;

       case  ALL_DATA:
          strcpy( que_name, updateds_que_name );
          break;

	   case  ARCH01_DATA:
		   strcpy( que_name, archds_que_name );
		   break;

	   case TPOS01_DATA:
			strcpy( que_name, posds_que_name );
			break;

	   case FCLF01_DATA:
	   case FRULE01_DATA:
		   strcpy( que_name, fconds_que_name );
			break;



       default:
          strcpy( que_name, trands_que_name );
          break;
    }

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )	
   {
      st_1          = ptemsg_get_msg_subtype1( p_msg_in );
      st_2          = ptemsg_get_msg_subtype2( p_msg_in );
      data_type     = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );
      if ( (st_1 == ST1_DB_SELECT) && (st_2 == 0) && (data_type == app_data_type) )
      {
         p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
         p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
         data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
         p_data        = p_data + sizeof(AUTH_TX);

         memcpy( psStructurePtr, p_data, nStructureSize );
      }
      else
      {   
		  //Krishna@TF, 15.6.11: Updated Error Msg whenever Invalid response is received
		 sprintf(error_msg, "PLEASE TRY AGAIN...Errorcode : %d. "
						    "Request Msg Subtype1: %d, App Data Type : %d. "
							"Response Msg SubType1: %d App Data Type : %d. ", 
							rcode, ST1_DB_SELECT, app_data_type, st_1, data_type);

		 rcode = PTEMSG_INVALID_DATATYPE;
         txdsapi_reset_connection(error_msg);
      }
   }

   txdsapi_free( p_msg_in );
   return ( rcode );
}    

BYTE txdsapi_get_record_bin_range( pBYTE psStructurePtr, int nStructureSize, 
                         BYTE  app_data_type,
                         pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode = 0;
   char           buffer[sizeof(AUTH_TX) + 4000] = {0};
   BYTE           st_1 = 0,st_2 = 0, data_type =0;
   CHAR           que_name[8] = {0};

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);

  

   switch( app_data_type )
   {
       case  NCF01_DATA:
       case  NCF02_DATA:
       case  NCF20_DATA:
       case  NCF21_DATA:
       case  NCF30_DATA:
       case  NEG01_DATA:
       case  ATM01_DATA:
       case  FG01_DATA:
	   case  OBS01_DATA:
	   case VISA3DS2_DATA:
          strcpy( que_name, netds_que_name );
          break;

       case  OCF01_DATA:
       case  OCF20_DATA:
       case  MCF01_DATA:
       case  MCF20_DATA:
       case  DCF01_DATA:
       case  BCH01_DATA:
       case  BCH10_DATA:
       case  BCH11_DATA:
       case  BCH20_DATA:
       case  NGF01_DATA:
       case  NFI01_DATA:
       case  NFI02_DATA:
       case  NMF01_DATA:
       case  NTF01_DATA:
       case  NBF01_DATA:
       case  DRF01_DATA:
	   case  MGF01_DATA:
          strcpy( que_name, devds_que_name );
          break;

       case  TLF01_DATA:
	   case  TLF01_STIP_DATA:
       case  FGUARD_DATA:
	   case  MRA01_DATA:
	  case FTLF01_DATA:
          strcpy( que_name, repds_que_name );
          break;

       case  ALL_DATA:
          strcpy( que_name, updateds_que_name );
          break;

	   case  ARCH01_DATA:
		   strcpy( que_name, archds_que_name );
		   break;

	   case TPOS01_DATA:
			strcpy( que_name, posds_que_name );
			break;

	   case FCLF01_DATA:
	   case FRULE01_DATA:
		   strcpy( que_name, fconds_que_name );
			break;



       default:
          strcpy( que_name, trands_que_name );
          break;
    }

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 ST2_DB_SELECT_BIN_RANGE_CFB_TXN,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )	
   {
      st_1          = ptemsg_get_msg_subtype1( p_msg_in );
      st_2          = ptemsg_get_msg_subtype2( p_msg_in );
      data_type     = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );
      if ( (st_1 == ST1_DB_SELECT) && (st_2 == 0) && (data_type == app_data_type) )
      {
         p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
         p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
         data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
         p_data        = p_data + sizeof(AUTH_TX);

         memcpy( psStructurePtr, p_data, nStructureSize );
      }
      else
      {   
		  //Krishna@TF, 15.6.11: Updated Error Msg whenever Invalid response is received
		 sprintf(error_msg, "PLEASE TRY AGAIN...Errorcode : %d. "
						    "Request Msg Subtype1: %d, App Data Type : %d. "
							"Response Msg SubType1: %d App Data Type : %d. ", 
							rcode, ST1_DB_SELECT, app_data_type, st_1, data_type);

		 rcode = PTEMSG_INVALID_DATATYPE;
         txdsapi_reset_connection(error_msg);
      }
   }

   txdsapi_free( p_msg_in );
   return ( rcode );
}    

/*************************************************************************
/************************************************************************/

BYTE txdsapi_get_list (pBYTE psStructurePtr, int nStructureSize, 
                       pBYTE psListPtr, int nListSize, 
                       BYTE  MsgSubType2, BYTE app_data_type,
                       pCHAR error_msg)
{
   pPTE_MSG       p_msg_out=NULL_PTR;
   pPTE_MSG       p_msg_in;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   char           buffer[sizeof(AUTH_TX) + 4000];
   BYTE           st_1, st_2, data_type;
   CHAR           que_name[8];

   switch( app_data_type )
   {
       case  NCF01_DATA:
       case  NCF02_DATA:
       case  NCF20_DATA:
       case  NCF21_DATA:
       case  NCF30_DATA:
       case  NEG01_DATA:
       case  ATM01_DATA:
       case  FG01_DATA:
	   case  OBS01_DATA:
	   case  VISA3DS2_DATA:
	   case NCF01_KEYBLOCK01_DATA:

          strcpy( que_name, netds_que_name );
          break;

       case  OCF01_DATA:
       case  OCF20_DATA:
       case  MCF01_DATA:
       case  MCF20_DATA:
       case  DCF01_DATA:
       case  BCH01_DATA:
       case  BCH10_DATA:
       case  BCH11_DATA:
       case  BCH20_DATA:
	   case  NGF01_DATA:
       case  NFI01_DATA:
       case  NFI02_DATA:
       case  NMF01_DATA:
       case  NTF01_DATA:
       case  NBF01_DATA:
       case  DRF01_DATA:
	   case  MGF01_DATA:
	   case  BCH01_MP_DATA:
          strcpy( que_name, devds_que_name );
          break;

	   case  TLF01_DATA:
	   case TLF01_STIP_DATA:
	   case  FGUARD_DATA:
          strcpy( que_name, repds_que_name );
          break;
		  
	   case	ARCH01_DATA:
		   strcpy( que_name, archds_que_name );
		   break;

	   case TPOS01_DATA:
			strcpy( que_name, posds_que_name );
			break;
	   
	   case FCLF01_DATA:
	   case FRULE01_DATA:
		   strcpy( que_name, fconds_que_name );
			break;
       default:
          strcpy( que_name, trands_que_name );
          break;
    }

   if ( MsgSubType2 == ST2_REPORT )
      strcpy( que_name, repds_que_name );

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);    

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_GET_GUI_LIST,
                                 MsgSubType2,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )	
   {
      st_1          = ptemsg_get_msg_subtype1( p_msg_in );
      st_2          = ptemsg_get_msg_subtype2( p_msg_in );
      data_type     = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );
      if ( (st_1 == ST1_DB_GET_GUI_LIST) && (st_2 == MsgSubType2) && (data_type == app_data_type) )
      {
         p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
         p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
         p_data        = p_data + sizeof(AUTH_TX);

         memcpy( psListPtr, p_data, nListSize );
		
      }
      else
      {
         rcode = PTEMSG_INVALID_DATATYPE;
         txdsapi_reset_connection (error_msg);
      }
   }
   txdsapi_free( p_msg_in );

   return ( rcode );
}

/*************************************************************************
/************************************************************************/

BYTE txdsapi_get_service_list (pBYTE psStructurePtr, int nStructureSize, 
                               pBYTE psListPtr, int nListSize, 
                               BYTE  MsgSubType2, BYTE app_data_type,
                               pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   char           buffer[sizeof(AUTH_TX) + 4000];
   CHAR           que_name[8];

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);    
   switch( app_data_type )
   {
       case  NCF01_DATA:
       case  NCF02_DATA:
       case  NCF20_DATA:
       case  NCF21_DATA:
       case  NCF30_DATA:
       case  NEG01_DATA:
       case  ATM01_DATA:
       case  FG01_DATA:
	   case  OBS01_DATA:
	   case  VISA3DS2_DATA:
          strcpy( que_name, netds_que_name );
          break;

       case  OCF01_DATA:
       case  OCF20_DATA:
       case  MCF01_DATA:
       case  MCF20_DATA:
       case  DCF01_DATA:
       case  BCH01_DATA:
       case  BCH10_DATA:
       case  BCH11_DATA:
       case  BCH20_DATA:
       case  NGF01_DATA:
       case  NFI01_DATA:
       case  NFI02_DATA:
       case  NMF01_DATA:
       case  NTF01_DATA:
       case  NBF01_DATA:
       case  DRF01_DATA:
       case  TLF01_DATA:
	   case  MGF01_DATA:
          strcpy( que_name, devds_que_name );
          break;

	   case  ARCH01_DATA:
		   strcpy( que_name, archds_que_name );
		   break;
       default:
          strcpy( que_name, trands_que_name );
          break;
    }

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_GET_SERVICE_LIST,
                                 MsgSubType2,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memcpy( psListPtr, p_data, nListSize );
   }
   txdsapi_free( p_msg_in );
   return ( rcode );
}

/*************************************************************************
/************************************************************************/

BYTE txdsapi_get_sch_list (pBYTE psStructurePtr, int nStructureSize, 
                           pBYTE psListPtr, int nListSize, 
                           BYTE  MsgSubType2, BYTE app_data_type,
                           pCHAR error_msg)
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   CHAR           buffer[sizeof(AUTH_TX) + 4000];

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);    

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_GET_SCH_LIST,
                                 MsgSubType2,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memcpy( psListPtr, p_data, nListSize );
   }
   txdsapi_free( p_msg_in );
   return ( rcode );
}



/*************************************************************************
/*************************************************************************/
BYTE txdsapi_truncate_ntf01_table( BYTE app_data_type, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_DELETE, 
                                 ST2_DB_DELETE_ALL_NTF01_RECORDS,
                                 devds_que_name,
                                 interactive_que_name,
                                 NULL_PTR,
                                 0,
                                 app_data_type );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}  




/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_num_records( pBYTE psNumRecords, 
                              BYTE app_data_type,
                              pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   CHAR           que_name[8] ={0};
    switch( app_data_type )
   {
       case  OBS01_DATA:
		   strcpy( que_name, netds_que_name );
		   break;
       default:
          strcpy( que_name, trands_que_name );
          break;
	}
   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_GET_COUNT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 NULL_PTR,
                                 0,
                                 app_data_type );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

     strcpy( (pCHAR)psNumRecords, (pCHAR)p_data );	
   }
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_mcf01_cfg( pMCF01 pmcf01, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(MCF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pmcf01, sizeof(MCF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 devds_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 MCF01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

     memset (pmcf01, 0, sizeof(MCF01));
     memcpy(pmcf01, p_data, sizeof(MCF01));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}  


/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_dcf01_cfg( pDCF01 pdcf01, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(DCF01)];

   memset(buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pdcf01, sizeof(DCF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 devds_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 DCF01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pdcf01, 0, sizeof(DCF01));
      memcpy(pdcf01, p_data, sizeof(DCF01));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_ccf01_cfg( pCCF01 pccf01, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(CCF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf01, sizeof(CCF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pccf01, 0, sizeof(CCF01));
      memcpy(pccf01, p_data, sizeof(CCF01));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_delete_ccf01_cfg( pCCF01 pccf01, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   BYTE        rcode;
   BYTE        buffer[sizeof(AUTH_TX)+sizeof(CCF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf01, sizeof(CCF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_DELETE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF01_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_update_ccf01_cfg( pCCF01 pccf01, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(CCF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf01, sizeof(CCF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF01_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_insert_ccf01_cfg( pCCF01 pccf01, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(CCF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf01, sizeof(CCF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_INSERT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF01_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}

/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_ccf02p_cfg( pCCF02P pccf02, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(CCF02P)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf02, sizeof(CCF02P));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF02P_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pccf02, 0, sizeof(CCF02P));
      memcpy(pccf02, p_data, sizeof(CCF02P));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_delete_ccf02p_cfg( pCCF02P pccf02, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(CCF02P)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf02, sizeof(CCF02P));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_DELETE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF02P_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_update_ccf02p_cfg( pCCF02P pccf02, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(CCF02P)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf02, sizeof(CCF02P));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF02P_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_insert_ccf02p_cfg( pCCF02P pccf02, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(CCF02P)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf02, sizeof(CCF02P));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_INSERT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF02P_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_ccf03_cfg( pCCF03 pccf03, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(CCF03)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf03, sizeof(CCF03));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF03P_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pccf03, 0, sizeof(CCF03));
      memcpy(pccf03, p_data, sizeof(CCF03));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_delete_ccf03_cfg( pCCF03 pccf03, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(CCF03)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf03, sizeof(CCF03));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_DELETE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF03P_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_update_ccf03_cfg( pCCF03 pccf03, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(CCF03)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf03, sizeof(CCF03));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF03P_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_insert_ccf03_cfg( pCCF03 pccf03, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(CCF03)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pccf03, sizeof(CCF03));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_INSERT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CCF03P_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}

/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_fcf02_cfg( pFCF02 pfcf02, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(FCF02)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pfcf02, sizeof(FCF02));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 FCF02_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pfcf02, 0, sizeof(FCF02));
      memcpy(pfcf02, p_data, sizeof(FCF02));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_delete_fcf02_cfg( pFCF02 pfcf02, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(FCF02)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pfcf02, sizeof(FCF02));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_DELETE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 FCF02_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_update_fcf02_cfg( pFCF02 pfcf02, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(FCF02)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pfcf02, sizeof(FCF02));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 FCF02_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_insert_fcf02_cfg( pFCF02 pfcf02, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(FCF02)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pfcf02, sizeof(FCF02));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_INSERT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 FCF02_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_acf01_cfg( pACF01 pacf01, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(ACF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pacf01, sizeof(ACF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 ACF01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pacf01, 0, sizeof(ACF01));
      memcpy(pacf01, p_data, sizeof(ACF01));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}



/************************************************************************/
/************************************************************************/
BYTE txdsapi_delete_paf01_cfg( pPAF01 ppaf01, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(PAF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),ppaf01, sizeof(PAF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_DELETE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 PAF01_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_delete_acf01_cfg( pACF01 pacf01, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(ACF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pacf01, sizeof(ACF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_DELETE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 ACF01_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_update_acf01_cfg( pACF01 pacf01, pCHAR error_msg )
{
   pPTE_MSG p_msg_out;
   pPTE_MSG p_msg_in;
   BYTE     rcode;
   BYTE     buffer[sizeof(AUTH_TX)+sizeof(ACF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pacf01, sizeof(ACF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 ACF01_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_update_paf01_cfg( pPAF01 ppaf01, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(PAF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),ppaf01, sizeof(PAF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 PAF01_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}

/************************************************************************/
/************************************************************************/
BYTE txdsapi_insert_acf01_cfg( pACF01 pacf01, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(ACF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pacf01, sizeof(ACF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_INSERT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 ACF01_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}

/************************************************************************/
/************************************************************************/
BYTE txdsapi_insert_paf01_cfg( pPAF01 ppaf01, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(PAF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),ppaf01, sizeof(PAF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_INSERT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 PAF01_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}

/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_paf01_cfg( pPAF01 ppaf01, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(PAF01)];

   memset(buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),ppaf01, sizeof(PAF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 PAF01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (ppaf01, 0, sizeof(PAF01));
      memcpy(ppaf01, p_data, sizeof(PAF01));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}

/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_bch01_cfg( pBCH01 pbch01, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(BCH01)];

   memset(buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pbch01, sizeof(BCH01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 devds_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 BCH01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pbch01, 0, sizeof(BCH01));
      memcpy(pbch01, p_data, sizeof(BCH01));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}

/************************************************************************/
/************************************************************************/
BYTE txdsapi_update_bch01_cfg( pBCH01 pbch01, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(BCH01)];

   memset(buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pbch01, sizeof(BCH01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 0,
                                 devds_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 BCH01_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}

/************************************************************************/
/************************************************************************/
BYTE txdsapi_return_bch01_delete_bch10( pBCH01 pbch01, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(BCH01)];  
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;

   memset(buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pbch01, sizeof(BCH01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 ST2_DB_RETURN_BCH01_DELETE_BCH10,
                                 devds_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 BCH01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, true ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pbch01, 0, sizeof(BCH01));
      memcpy( pbch01, p_data, sizeof(BCH01));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}

/************************************************************************/
/************************************************************************/
BYTE txdsapi_select_bch01_by_date(pBCH01 pbch01,pBCH01 bch01_rec, pCHAR error_msg)
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   BYTE           rcode;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(BCH01)];
	
   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pbch01, sizeof(BCH01));
   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 ST2_DB_SELECT_BCH01_BY_DATE,
                                 devds_que_name,
                                 interactive_que_name,
                                 (BYTE *)buffer,
                                 sizeof( buffer ),
                                 BCH01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (bch01_rec, 0, sizeof(BCH01));
      memcpy( bch01_rec, p_data, sizeof(BCH01));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}  

/************************************************************************/
/************************************************************************/
BYTE txdsapi_select_bch10_service_list(pBCH10 pbch10,pBCH10_LIST Bch10List, pCHAR error_msg)
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   BYTE           rcode;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(BCH10)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pbch10, sizeof(BCH10));
   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_GET_SERVICE_LIST,
                                 0,
                                 devds_que_name,
                                 interactive_que_name,
                                 (BYTE *)buffer,
                                 sizeof( buffer ),
                                 BCH10_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (Bch10List, 0, sizeof(BCH10_LIST));
      memcpy( Bch10List, p_data, sizeof(BCH10_LIST));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}  
/************************************************************************/
/************************************************************************/
BYTE txdsapi_select_bch11_service_list(pBCH11 pbch11,pBCH11_LIST Bch11List, pCHAR error_msg)
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   BYTE           rcode;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(BCH11)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pbch11, sizeof(BCH11));
   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_GET_SERVICE_LIST,
                                 0,
                                 devds_que_name,
                                 interactive_que_name,
                                 (BYTE *)buffer,
                                 sizeof( buffer ),
                                 BCH11_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (Bch11List, 0, sizeof(BCH11_LIST));
      memcpy( Bch11List, p_data, sizeof(BCH11_LIST));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}  
/************************************************************************/
/************************************************************************/
BYTE txdsapi_select_bch01_service_list(pBCH01 pbch01,pBCH01_LIST Bch01List, pCHAR error_msg)
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   BYTE           rcode;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(BCH01)];
	
   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pbch01, sizeof(BCH01));
   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_GET_SERVICE_LIST,
                                 0,
                                 devds_que_name,
                                 interactive_que_name,
                                 (BYTE *)buffer,
                                 sizeof( buffer ),
                                 BCH01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (Bch01List, 0, sizeof(BCH01_LIST));
      memcpy( Bch01List, p_data, sizeof(BCH01_LIST));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}  
/************************************************************************/
/************************************************************************/
BYTE txdsapi_select_tlf01_service_list(pTLF01 ptlf01,pTLF01_LIST Tlf01List, pCHAR error_msg)
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   BYTE           rcode;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(TLF01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),ptlf01, sizeof(TLF01));
   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_GET_SERVICE_LIST,
                                 0,
                                 devds_que_name,
                                 interactive_que_name,
                                 (BYTE *)buffer,
                                 sizeof( buffer ),
                                 TLF01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (Tlf01List, 0, sizeof(TLF01_LIST));
      memcpy( Tlf01List, p_data, sizeof(TLF01_LIST));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}  


/************************************************************************/
/************************************************************************/
BYTE txdsapi_check_bch01_for_settlement(pBCH01 pbch01, pCHAR error_msg)
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   BYTE           rcode;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(BCH01)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pbch01, sizeof(BCH01));
   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 ST2_DB_SELECT_BCH01_BY_STATUS,
                                 devds_que_name,
                                 interactive_que_name,
                                 (BYTE *)buffer,
                                 sizeof( buffer ),
                                 BCH01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pbch01, 0, sizeof(BCH01));
      memcpy( pbch01, p_data, sizeof(BCH01));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}  


/************************************************************************
   txdsapi_move_trans
   moves trans from auth_tx to arc or adm depending on st2_type 
************************************************************************/
BYTE txdsapi_move_trans( pBCH10 pbch10, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(BCH10)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pbch10, sizeof(BCH10));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_MOVE_TRANS,
                                 0,
                                 devds_que_name,
                                 interactive_que_name,
                                 (BYTE *)&buffer,
                                 sizeof( buffer ),
                                 BCH10_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}
 

/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_settle_tran_list( pBCH10 pbch10,
                                   pBCH10_LIST settle_trans_list,
                                   pCHAR error_msg )

// NOTE: The RRN must be blank for the first time this is called.  Each call after the
//       first, the RRN must contain the RRN from the last record returned in the
//       previous call.

{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(BCH10)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pbch10, sizeof(BCH10));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_GET_SERVICE_LIST,
                                 0,
                                 devds_que_name,
                                 interactive_que_name,
                                 (BYTE *)buffer,
                                 sizeof (buffer),
                                 BCH10_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memcpy( settle_trans_list, p_data, sizeof(BCH10_LIST));
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}

/************************************************************************/
/************************************************************************/
BYTE txdsapi_select_ncf30_service_list(pNCF30 pncf30,
                                       pNCF30_BATCH_LIST Ncf30List,
                                       pCHAR error_msg)
{
   pPTE_MSG      p_msg_out;
   pPTE_MSG      p_msg_in;
   BYTE          rcode;
   LONG          data_len;
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   BYTE          buffer[sizeof(AUTH_TX)+sizeof(NCF30)];

   memset(&buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pncf30, sizeof(NCF30));
   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_GET_SERVICE_LIST,
                                 0,
                                 netds_que_name,
                                 interactive_que_name,
                                 (BYTE *)buffer,
                                 sizeof( buffer ),
                                 NCF30_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset( Ncf30List, 0,      sizeof(NCF30_BATCH_LIST) );
      memcpy( Ncf30List, p_data, sizeof(NCF30_BATCH_LIST) );
   }
   free( p_msg_in );
   return ( PTEMSG_OK );
}  

/************************************************************************/
/************************************************************************/
BYTE txdsapi_send_mcc_record( pMCC_ONLINE_UPDATES psStructurePtr,
                              BYTE                app_data_type,
                              pCHAR               error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   LONG           MCC_Struct_Size;
   LONG           Auth_Tx_Size;
   LONG           buf_size;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   CHAR           buffer[sizeof(AUTH_TX) + sizeof(MCC_ONLINE_UPDATES)];
   CHAR           que_name[8];
   BYTE           data_type;

   Auth_Tx_Size    = sizeof(AUTH_TX);
   MCC_Struct_Size = sizeof(MCC_ONLINE_UPDATES);
   buf_size        = Auth_Tx_Size + MCC_Struct_Size;

   memset( buffer, 0, buf_size );
   memcpy((buffer + Auth_Tx_Size), psStructurePtr, MCC_Struct_Size );

   strcpy ( que_name, "ncmcrdA" );

   p_msg_in  = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_INCOMING_GUI,
                                 ST1_NONE,
                                 ST2_NONE,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 buf_size,
                                 0 );

   if( PTEMSG_OK == (rcode = process_request(p_msg_out, &p_msg_in, error_msg, false)) )
   {
      data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );

      if ( data_type == app_data_type )
      {
         p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
         p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
         data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
         p_data        = p_data + Auth_Tx_Size;

         memset( psStructurePtr, 0,      MCC_Struct_Size );
         memcpy( psStructurePtr, p_data, MCC_Struct_Size );
      }
      else
      {
         rcode = PTEMSG_INVALID_DATATYPE;
         txdsapi_reset_connection(error_msg);
      }
   }
   txdsapi_free( p_msg_in );
   return( rcode );
}    

/************************************************************************/
/************************************************************************/
BYTE txdsapi_send_jcb_record( pJCB_ONLINE_UPDATES psStructurePtr,
                              BYTE app_data_type, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   CHAR           buffer[sizeof(AUTH_TX) + sizeof(JCB_ONLINE_UPDATES)];
   CHAR           que_name[] = "ncjcbA";
   BYTE           data_type;
   INT            AUTH_TX_SIZE;
   INT            JCB_ONLINE_UPDATES_SIZE;

   AUTH_TX_SIZE            = sizeof(AUTH_TX);
   JCB_ONLINE_UPDATES_SIZE = sizeof(JCB_ONLINE_UPDATES);

   memset(buffer, 0, sizeof(buffer) );
   memcpy(buffer + AUTH_TX_SIZE, psStructurePtr, JCB_ONLINE_UPDATES_SIZE);

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_INCOMING_GUI,
                                 ST1_NONE,
                                 ST2_NONE,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 0 );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );

      if ( data_type == app_data_type )
      {
         p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
         p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );
         data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
         p_data     = p_data + AUTH_TX_SIZE;

         memset( psStructurePtr, 0,      JCB_ONLINE_UPDATES_SIZE );
         memcpy( psStructurePtr, p_data, JCB_ONLINE_UPDATES_SIZE );
      }
      else
      {
         rcode = PTEMSG_INVALID_DATATYPE;
         txdsapi_reset_connection(error_msg);
      }
   }
   txdsapi_free( p_msg_in );
   return( rcode );
}    

/************************************************************************/
/************************************************************************/
BYTE txdsapi_send_amex_record( pAMEX_ONLINE_UPDATES psStructurePtr,
                               BYTE                 app_data_type,
                               pCHAR                que_name,
                               pCHAR                error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   CHAR           buffer[sizeof(AUTH_TX) + sizeof(AMEX_ONLINE_UPDATES)];
   BYTE           data_type;
   INT            AUTH_TX_SIZE;
   INT            AMEX_ONLINE_UPDATES_SIZE;

   AUTH_TX_SIZE             = sizeof(AUTH_TX);
   AMEX_ONLINE_UPDATES_SIZE = sizeof(AMEX_ONLINE_UPDATES);

   memset(buffer, 0, sizeof(buffer) );
   memcpy(buffer + AUTH_TX_SIZE, psStructurePtr, AMEX_ONLINE_UPDATES_SIZE);

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_INCOMING_GUI,
                                 ST1_NONE,
                                 ST2_NONE,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 0 );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );

      if ( data_type == app_data_type )
      {
         p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
         p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );
         data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data );
         p_data     = p_data + AUTH_TX_SIZE;

         memset( psStructurePtr, 0,      AMEX_ONLINE_UPDATES_SIZE );
         memcpy( psStructurePtr, p_data, AMEX_ONLINE_UPDATES_SIZE );
      }
      else
      {
         rcode = PTEMSG_INVALID_DATATYPE;
         txdsapi_reset_connection(error_msg);
      }
   }
   txdsapi_free( p_msg_in );
   return( rcode );
}    

/************************************************************************/
/************************************************************************/
BYTE txdsapi_send_voice_auth_tran( pAUTH_TX psAuthTxPtr, BYTE app_data_type, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   CHAR           buffer[sizeof(AUTH_TX) + 4000];
   CHAR           que_name[8];
   BYTE           data_type;

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ( buffer, psAuthTxPtr, sizeof (AUTH_TX));

   strcpy ( que_name, "txcntlA" );

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
                                 0,
                                 0,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 0 );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );

      if ( data_type == app_data_type )
      {
         p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
         p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

         memset (psAuthTxPtr, 0, sizeof(AUTH_TX));
         memcpy(psAuthTxPtr, p_data, sizeof(AUTH_TX));
      }
      else
      {
         rcode = PTEMSG_INVALID_DATATYPE;
         txdsapi_reset_connection(error_msg);
      }
   }
   txdsapi_free( p_msg_in );
   return ( rcode );
}


BYTE txdsapi_find_bin( pCHAR CardNumber, pBIN01 pBin01Record, BYTE app_data_type, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   CHAR           buffer[sizeof(AUTH_TX) + 4000];
   CHAR           que_name[8];
   BYTE           data_type;

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ( (buffer + sizeof(AUTH_TX)), CardNumber, 22);

   strcpy ( que_name, trands_que_name );

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_CHECK_BIN_RANGE,
                                 0,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      data_type = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );

      if ( data_type == app_data_type )
      {
         p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
         p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );
         p_data     = p_data + sizeof(AUTH_TX);

         memset (pBin01Record, 0, sizeof(BIN01));
         memcpy(pBin01Record, p_data, sizeof(BIN01));
      }
      else
      {
         rcode = PTEMSG_INVALID_DATATYPE;
         txdsapi_reset_connection(error_msg);
      }
   }

   txdsapi_free( p_msg_in );
   return ( rcode );
}   

BYTE txdsapi_delete_bch10_update_bch01( pBCH10 bch10, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   CHAR      buffer[sizeof(AUTH_TX) + 4000];

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ( (buffer + sizeof(AUTH_TX)), bch10, sizeof ( BCH10 ) );

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 ST2_DB_UPDATE_BCH01_DELETE_BCH10_FOR_GUI,
                                 devds_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 BCH01_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}   

/*************************************************************************
/*************************************************************************/
BYTE txdsapi_delete_record_by_subtype2( pBYTE psStructurePtr, int nStructureSize, 
                          BYTE app_data_type, BYTE Subtype2,  pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   CHAR      buffer[sizeof(AUTH_TX) + 4000];
   CHAR      que_name[8];

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);

   switch( app_data_type )
   {
       case  NCF01_DATA:
       case  NCF02_DATA:
       case  NCF20_DATA:
       case  NCF21_DATA:
       case  NCF30_DATA:
       case  NEG01_DATA:
       case  ATM01_DATA:
          strcpy( que_name, netds_que_name );
          break;

       case  OCF01_DATA:
       case  OCF20_DATA:
       case  MCF01_DATA:
       case  MCF20_DATA:
       case  DCF01_DATA:
       case  BCH01_DATA:
       case  BCH10_DATA:
       case  BCH11_DATA:
       case  BCH20_DATA:
       case  NGF01_DATA:
       case  NFI01_DATA:
       case  NFI02_DATA:
       case  NMF01_DATA:
       case  NTF01_DATA:
       case  NBF01_DATA:
       case  DRF01_DATA:
       case  TLF01_DATA:
	   case  MGF01_DATA:
          strcpy( que_name, devds_que_name );
          break;
	   case	ARCH01_DATA:
		   strcpy( que_name, archds_que_name );
		   break;
       default:
          strcpy( que_name, trands_que_name );
          break;
    }

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_DELETE,
                                 Subtype2,
                                 que_name,
                                 interactive_que_name,
                                 (BYTE *)buffer,
                                 sizeof(AUTH_TX) + nStructureSize,
                                 app_data_type );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}  

/************************************************************************/
/************************************************************************/
BYTE txdsapi_get_record_by_subtype2( pBYTE psStructurePtr, int nStructureSize, 
                          BYTE app_data_type, BYTE Subtype2, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   CHAR           buffer[sizeof(AUTH_TX) + 4000];
   BYTE           st_1, st_2, data_type;
   CHAR           que_name[8];

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);

   switch( app_data_type )
   {
       case  NCF01_DATA:
       case  NCF02_DATA:
       case  NCF20_DATA:
       case  NCF21_DATA:
       case  NCF30_DATA:
       case  NEG01_DATA:
       case  ATM01_DATA:
          strcpy( que_name, netds_que_name );
          break;

       case  OCF01_DATA:
       case  OCF20_DATA:
       case  MCF01_DATA:
       case  MCF20_DATA:
       case  DCF01_DATA:
       case  BCH01_DATA:
       case  BCH10_DATA:
       case  BCH11_DATA:
       case  BCH20_DATA:
       case  NGF01_DATA:
       case  NFI01_DATA:
       case  NFI02_DATA:
       case  NMF01_DATA:
       case  NTF01_DATA:
       case  NBF01_DATA:
       case  DRF01_DATA:
       case  TLF01_DATA:
	   case  MGF01_DATA:
	   case BCH01_MP_DATA:
          strcpy( que_name, devds_que_name );
          break;
	   case	ARCH01_DATA:
		   strcpy( que_name, archds_que_name );
		   break;
       default:
          strcpy( que_name, trands_que_name );
          break;
    }

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 Subtype2,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      st_1          = ptemsg_get_msg_subtype1( p_msg_in );
      st_2          = ptemsg_get_msg_subtype2( p_msg_in );
      data_type     = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );
      if ( (st_1 == ST1_DB_SELECT) && (st_2 == Subtype2) && (data_type == app_data_type) )
      {
         p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
         p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
         data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
         p_data        = p_data + sizeof(AUTH_TX);

         memcpy( psStructurePtr, p_data, nStructureSize );
      }
      else
      {
         rcode = PTEMSG_INVALID_DATATYPE;
         txdsapi_reset_connection(error_msg);
      }
   }
   txdsapi_free( p_msg_in );
   return ( rcode );
}

/************************************************************************/
/************************************************************************/
BYTE txdsapi_send_all_data( pAUTH_TX psAuthTxPtr,  pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE            rcode;
   CHAR            buffer[sizeof(AUTH_TX) + 4000];
   CHAR            que_name[15];

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ( buffer, psAuthTxPtr, sizeof (AUTH_TX));

   strcpy ( que_name, updateds_que_name );

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 0,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 ALL_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, false );
   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );

   memset( psAuthTxPtr, 0,      sizeof(AUTH_TX) );
   memcpy( psAuthTxPtr, p_data, sizeof(AUTH_TX) );

   txdsapi_free( p_msg_in );
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
BYTE txdsapi_fraudguard( INT      mode,
                         pAUTH_TX p_auth_tx,
                         pCHAR    fg_data,
                         pLONG    fg_data_len,
                         pCHAR    error_msg )
{
   #define        MAX_FG_DATA_LEN  4000
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   pPTE_MSG_DATA  p_msg_data;
   pBYTE          p_data;
   BYTE           rcode;
   BYTE           msgtype;
   BYTE           subtype1;
   BYTE           subtype2;
   BYTE           st_2;
   BYTE           data_type;
   CHAR           buffer[sizeof(AUTH_TX) + MAX_FG_DATA_LEN];
   INT            send = true;
   LONG           data_len;


   memset( buffer, 0x00, sizeof(buffer) );
   memcpy( buffer, p_auth_tx, sizeof(AUTH_TX) );

   if ( *fg_data_len > MAX_FG_DATA_LEN )
      *fg_data_len = MAX_FG_DATA_LEN;

   memcpy( (buffer + sizeof(AUTH_TX)), fg_data, *fg_data_len );
   if ( mode == FG_MODE_SEND_BATCH )
   {
      msgtype = MT_BATCH_SEND_REQUEST;
      if ( *fg_data_len == 0 )
      {
         /* No len provided/no tran Ids provided = resend all failed txns */
         subtype2 = ST2_GET_ALL_FG_FAILED;
      }
      else if ( NULL == strchr(fg_data,0x1F) )
      {
         /* There is only 1 transaction id. Resend just this one. */
         subtype2 = ST2_NONE;
      }
      else
      {
         /* There are multiple tran Ids (up to 10). Resend this group. */
         subtype2 = ST2_GET_10_FG_FAILED;
      }
   }
   else if ( mode == FG_MODE_GET_PROPERTIES )
   {
      /* GUI needs SOAP property file info to display in its window. */
      msgtype = MT_GET_CONFIG_INFO;
   }
   else if ( mode == FG_MODE_REFRESH_PROPERTIES )
   {
      /* SOAP property values have changed in the GUI. Update the file. */
      msgtype = MT_UPDATE_CONFIG;
   }
   else
   {
      send = false;
      sprintf( error_msg, "Invalid MODE (%d) parameter.", mode );
      rcode = PTEMSG_COULD_NOT_FORWARD;
   }

   if ( send == true )
   {
      p_msg_in  = NULL_PTR;
      p_msg_out = ptemsg_build_msg( msgtype,
                                    ST1_NONE,
                                    subtype2,
                                    fguard_que_name,
                                    interactive_que_name,
                                    (pBYTE) buffer,
                                    sizeof( buffer ),
                                    FGUARD_DATA );

      if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
      {
         subtype1   = ptemsg_get_msg_subtype1( p_msg_in );
         st_2       = ptemsg_get_msg_subtype2( p_msg_in );
         data_type  = ptemsg_get_pte_msg_data_app_data_type( ptemsg_get_pte_msg_data(p_msg_in) );
         if ( (subtype1  == ST1_NONE) &&
              (st_2      == subtype2) &&
              (data_type == FGUARD_DATA) )
         {
            p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
            data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
            p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
            memcpy( p_auth_tx, p_data, sizeof(AUTH_TX) );

            *fg_data_len = data_len - sizeof(AUTH_TX);
            if ( *fg_data_len > 0 )
            {
               p_data = p_data + sizeof(AUTH_TX);
               memcpy( fg_data, p_data, *fg_data_len );
            }
            else
            {
               *fg_data_len = 0;
            }
         }
         else
         {
            rcode = PTEMSG_INVALID_DATATYPE;
            txdsapi_reset_connection( error_msg );
         }
      }
      txdsapi_free( p_msg_in );
   }
   return ( rcode );
}


/************************************************************************/
/************************************************************************/
/*
Krishna@TF, 2.6.11,:  Below changes are for purpose of Dual Encryption DB updation & Selection 
*/

BYTE txdsapi_Select_Ucf01_DualEncryptionKey_UserCount( pUCF01 pucf01, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(UCF01)];

   memset(buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pucf01, sizeof(UCF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 ST2_DB_SELECT_FOR_DUALENCRYPTN,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 UCF01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pucf01, 0, sizeof(UCF01));
      memcpy(pucf01, p_data, sizeof(UCF01));
   }
   //txdsapi_free( p_msg_in );
   return ( PTEMSG_OK );
}

/************************************************************************/
/************************************************************************/

BYTE txdsapi_Update_Ucf01_DualEncryptionKey(pCHAR encryption_key, pCHAR error_msg )
{
   pPTE_MSG  p_msg_out;
   pPTE_MSG  p_msg_in;
   BYTE      rcode;
   BYTE      buffer[sizeof(AUTH_TX)+sizeof(UCF01)];

   memset(&buffer,0, sizeof(buffer));
   //memcpy(buffer+sizeof(AUTH_TX),pucf01, sizeof(UCF01));
   memcpy(buffer+sizeof(AUTH_TX),encryption_key, ENC_DEC_KEY_SIZE);

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 ST2_DB_UPDATE_FOR_DUALENCRYPTN,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 ENC_DEC_DATA );

   rcode = process_request( p_msg_out, &p_msg_in, error_msg, true );
   txdsapi_free( p_msg_in );
   return ( rcode );
}

/************************************************************************/
/************************************************************************/

BYTE txdsapi_Select_Ucf01_DualEncryptionKey_Other_User( pUCF01 pucf01, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(UCF01)];

   memset(buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pucf01, sizeof(UCF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 ST2_DB_SELECT_FOR_DUALENCRYPTN_OTHER_USER,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 UCF01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pucf01, 0, sizeof(UCF01));
      memcpy(pucf01, p_data, sizeof(UCF01));
   }
   txdsapi_free( p_msg_in );
   return ( PTEMSG_OK );
}


/************************************************************************/
/************************************************************************/

BYTE txdsapi_Select_Ucf01_DualEncryptionKey_Update_Count( pUCF01 pucf01, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(UCF01)];

   memset(buffer,0, sizeof(buffer));
   memcpy(buffer+sizeof(AUTH_TX),pucf01, sizeof(UCF01));

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 ST2_DB_SELECT_FOR_DUALENCRYPTN_KEY_UPDAT_COUNT,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 UCF01_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

      memset (pucf01, 0, sizeof(UCF01));
      memcpy(pucf01, p_data, sizeof(UCF01));
   }
   txdsapi_free( p_msg_in );
   return ( PTEMSG_OK );
}

/************************************************************************/
/************************************************************************/

BYTE txdsapi_get_record_ucf01_by_ST2_From_User_Status_Control(  pBYTE psStructurePtr,int nStructureSize,pCHAR error_msg )
{
	pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           rcode;
   CHAR           buffer[sizeof(AUTH_TX) + 4000];
   BYTE           st_1, st_2, data_type;
   CHAR           que_name[8];

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);
   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_UPDATE,
                                 ST2_DB_SELECT_UCF01_FOR_USR_STS_CNRL,
                                 trands_que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 UCF01_DATA );

   if( ( rcode = process_request_update_user_status( p_msg_out, &p_msg_in, error_msg, false ) ) == PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
      p_data        = p_data + sizeof(AUTH_TX);

	  txdsapi_free( p_msg_in );
	  return ( PTEMSG_OK );
   }
   else
   {
	  txdsapi_free( p_msg_in );
	  return ( PTEMSG_NOT_FOUND );
   }
   
}

int get_log_flag_ini_data()
{
	CHAR buffer[10] = "";
	CHAR ini_file[256] = {0};

	GetAscendentConfigDirectory( ini_file );
    strcat( ini_file, "tf.ini" );

	GetPrivateProfileString(
        "MONITOR_TIMEOUT_PROMPT", 	   // points to section name 
        "DCO_RESET_FILE_FLAG",	       // points to key name 
        "0",	                       // points to default string 
        buffer,                        // points to destination buffer 
        sizeof(buffer) - 1,            // size of destination buffer 
        ini_file );

	resetCFileFlag = atoi(buffer);
	return resetCFileFlag;
}


BYTE write_to_reset_file_c(char *log)
{
	FILE *tp;
	CHAR buffer[1024] = {0};
	CHAR		   time_str[50];
	struct tm * cur_time;
	time_t      clock ;

	if(resetCFileFlag == 1)
	{
		time( &clock );
		cur_time = localtime( &clock );
		sprintf( time_str, "%04d-%02d-%02d  %02d:%02d:%02d", cur_time->tm_year + 1900,
			cur_time->tm_mon + 1, cur_time->tm_mday, cur_time->tm_hour,
			cur_time->tm_min, cur_time->tm_sec ) ;

		sprintf(buffer, "%s:- %s\n", time_str, log);
		tp = fopen("..\\log\\dco_monitor_reset_logs.txt", "a+");
		fwrite( buffer, sizeof( CHAR ), strlen( buffer ), tp );
		fflush(tp);
		fclose(tp);
	}

	return 0;
}
