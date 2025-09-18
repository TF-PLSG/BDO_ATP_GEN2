/******************************************************************************
  
        Module: cdsapi.c
  
         Title: Terminal Dataserver API Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\cdsapi\CDSAPI.C  $
   
      Rev 1.8   20 Aug 1998 09:48:28   rmalathk
   changed some of the free's to cdsapi_free
   
      Rev 1.7   Aug 20 1998 09:28:46   drodden
   Fixed the get-router-record to pass the coreds queue name in the
   ptemsg build call.  Was passing null-ptr.
   
   
      Rev 1.6   10 Aug 1998 12:04:36   rmalathk
   1. moved these files out of ptedb.lib.
   2. made modifications to seperate pteipc calls
       from ptetcp calls.
   3. This file is now shared by two library projects,
       pteipc.lib and ptetcp.lib
   
      Rev 1.5   May 14 1998 09:21:48   JXIAO
   Add more functions required by new merged dialog and hostcon GUI
   
      Rev 1.4   May 05 1998 15:53:06   JXIAO
   Fix freeing in message problems
   
      Rev 1.3   01 May 1998 11:13:32   MSALEH
   include "ptemsg.h", "coreinfo.h" in cdsapi.c
   to access definitions needed for functions.
   corrected data structure names that were 
   misspelled.
   
      Rev 1.2   May 01 1998 10:07:56   JXIAO
    
   
      Rev 1.1   Apr 28 1998 17:19:48   drodden
   Changed the process-request routine to be local (private), rather than public,
   to avoid conflicts with other similarly named functions in other modules.
   
   
      Rev 1.0   Apr 23 1998 11:01:54   drodden
   Initial code for the Core Dataserver.
   

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "basictyp.h"
#include "pte.h"
#include "coreinfo.h"

#include "cdsapi.h"

#ifdef XIPC
   #include "pteipc.h"
#else
   #include "ptetcp.h"
#endif



PRIVATE pCHAR coreds_que_name = "coredsA";


//**********************************************************************
//**********************************************************************
void cdsapi_free ( pPTE_MSG p_msg_in )
{
	#ifdef XIPC
		free( p_msg_in );
   #endif
}



//***********************************************************************
//***********************************************************************
PRIVATE BYTE process_request( pPTE_MSG p_msg_out,
                              pPTE_MSG *p_msg_in,
                              pCHAR error_msg,
                              BOOLEAN route_via_logger )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	CHAR			temp_str[100];
	BYTE			dberror;
   pCHAR       dest_que;
   CHAR        que_name[12];
   LONG			timeout;

   #ifdef XIPC
	   LONG			retcode;
   #endif

   timeout = 5; //seconds
	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

   dest_que = ptemsg_get_msg_dest_queue( p_msg_out );

   #ifdef XIPC    // using xipc
      if( route_via_logger )
         strcpy( que_name, logger_que_name );
      else
         strcpy( que_name, dest_que );
	   *p_msg_in = pteipc_send_receive( p_msg_out, que_name, interactive_que_name, &retcode );

   #else       // using TCP/IP via applink
      if( route_via_logger )
      {
         sprintf( que_name, "@%s", dest_que );
         ptemsg_set_dest_queue( p_msg_out, que_name );
      }
	   *p_msg_in = ptetcp_send_receive( p_msg_out, timeout, temp_str );
   #endif

	if( *p_msg_in == NULL_PTR )
	{
      #ifdef XIPC
		   pteipc_get_errormsg( retcode, temp_str );
      #endif

		sprintf( error_msg, "Communication Error during request to Core DataServer: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

	if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
	{
		p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
		p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

		strncpy( temp_str, (pCHAR)p_data, sizeof ( temp_str) );
		sprintf( error_msg, "Database Error: %s", temp_str );

      cdsapi_free( *p_msg_in );
	
	}

	return dberror;
}


/************************************************************************/
/************************************************************************/
BYTE cdsapi_delete_record( pGENERIC_KEYS generic_keys, 
                           BYTE          app_data_type,
                           pCHAR         error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_DELETE,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)generic_keys,
                          sizeof( GENERIC_KEYS ),
                          app_data_type );

	if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, true ) ) == PTEMSG_OK )	
		cdsapi_free( p_msg_in );
   return ( rcode );
}  

/************************************************************************/
/************************************************************************/
BYTE cdsapi_update_dm_config( pDM_CONFIG dm_config, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_UPDATE,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)dm_config,
                          sizeof( DM_CONFIG ),
                          DM_CONFIG_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, true ) ) == PTEMSG_OK )	
		cdsapi_free( p_msg_in );
   return ( rcode );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_update_router_config( pROUTER_TABLE router_table, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_UPDATE,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)router_table,
                          sizeof( ROUTER_TABLE ),
                          ROUTER_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, true ) ) == PTEMSG_OK )	
		cdsapi_free( p_msg_in );
   return ( rcode );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_update_tpdu_config( pTPDU tpdu, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_UPDATE,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)tpdu,
                          sizeof( TPDU ),
                          TPDU_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, true ) ) == PTEMSG_OK )	
		cdsapi_free( p_msg_in );
   return ( rcode );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_insert_router_config( pROUTER_TABLE router_table, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_INSERT,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)router_table,
                          sizeof( ROUTER_TABLE ),
                          ROUTER_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, true ) ) == PTEMSG_OK )	
		cdsapi_free( p_msg_in );
   return ( rcode );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_insert_tpdu_config( pTPDU tpdu, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_INSERT,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)tpdu,
                          sizeof( TPDU ),
                          TPDU_DATA );

   if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, true ) ) == PTEMSG_OK )	
		cdsapi_free( p_msg_in );
   return ( rcode );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_get_dm_config( pDM_CONFIG dm_config, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   LONG        data_len;
   pBYTE       p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_SELECT,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)dm_config->db_subsystem_name,
                          sizeof( dm_config->db_subsystem_name ),
                          DM_CONFIG_DATA );

	if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) != PTEMSG_OK )	
		return ( rcode );


   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memcpy( dm_config, p_data, data_len );
	cdsapi_free( p_msg_in );
   return ( PTEMSG_OK );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_get_router_record( pROUTER_TABLE router, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   LONG        data_len;
   pBYTE       p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_SELECT,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)router->router_identifier,
                          sizeof( router->router_identifier ),
                          ROUTER_DATA );

	if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) != PTEMSG_OK )	
		return ( rcode );

   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memcpy( router, p_data, data_len );
	cdsapi_free( p_msg_in );
   return ( PTEMSG_OK );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_get_tpdu_record( pTPDU tpdu, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   LONG        data_len;
   pBYTE       p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_SELECT,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)tpdu->tpdu_id,
                          sizeof( tpdu->tpdu_id ),
                          TPDU_DATA );

	if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) != PTEMSG_OK )	
		return ( rcode );

   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memcpy( tpdu, p_data, data_len );
	cdsapi_free( p_msg_in );
   return ( PTEMSG_OK );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_get_router_table( pROUTER_TABLE_LIST router_list, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   LONG        data_len;
   pBYTE       p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_GET_ALL,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          NULL_PTR,
                          0,
                          ROUTER_DATA );

	if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) != PTEMSG_OK )	
		return ( rcode );

   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memcpy( router_list, p_data, data_len );
	cdsapi_free( p_msg_in );
   return ( PTEMSG_OK );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_get_tpdu_table( pTPDU_TABLE_LIST tpdu_list, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   LONG        data_len;
   pBYTE       p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_GET_ALL,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          NULL_PTR,
                          0,
                          TPDU_DATA );

	if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) != PTEMSG_OK )	
		return ( rcode );

   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memcpy( tpdu_list, p_data, data_len );
	cdsapi_free( p_msg_in );
   return ( PTEMSG_OK );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_get_dmconfig_table( pDM_CONFIG_LIST dmconfig_list, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   pPTE_MSG    p_msg_in;
   LONG        data_len;
   pBYTE       p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE        rcode;

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_GET_ALL,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          NULL_PTR,
                          0,
                          DM_CONFIG_DATA );

	if( ( rcode = process_request( p_msg_out, &p_msg_in, error_msg, false ) ) != PTEMSG_OK )	
		return ( rcode );

   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memcpy( dmconfig_list, p_data, data_len );
	cdsapi_free( p_msg_in );
   return ( PTEMSG_OK );
}




/************************************************************************/
/************************************************************************/
BYTE process_request_snd( pPTE_MSG p_msg_out, pCHAR error_msg, BOOLEAN route_via_logger )
{
  CHAR			temp_str[100];
  pCHAR       dest_que;
  CHAR        que_name[12];
#ifdef XIPC
  LONG			retcode;
#else
  INT          retcode_tcp;
#endif

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

   dest_que = ptemsg_get_msg_dest_queue( p_msg_out );

   #ifdef XIPC    // using xipc
      if( route_via_logger )
         strcpy( que_name, logger_que_name );
      else
         strcpy( que_name, dest_que );
	   retcode = pteipc_send( p_msg_out, que_name );

      if( retcode < 0 )
      {
         pteipc_get_errormsg( retcode, temp_str );
         sprintf( error_msg, "%d:Communication Error during request to Core DataServer: %s",retcode, temp_str );
         return PTEMSG_IPC_ERROR;
      }

   #else       // using TCP/IP via applink
      if( route_via_logger )
      {
         sprintf( que_name, "@%s", dest_que );
         ptemsg_set_dest_queue( p_msg_out, que_name );
      }
	   retcode_tcp = ptetcp_send( p_msg_out, temp_str );

      if( !retcode_tcp  )
      {
         sprintf( error_msg, "Communication Error during request to Core DataServer: %s", temp_str );
         return PTEMSG_IPC_ERROR;
      }
   #endif

  return PTEMSG_OK;
}




BYTE cdsapi_parse_dm_config( pDM_CONFIG dm_config, pPTE_MSG p_msg_in )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	
  if (p_msg_in == NULL_PTR) return (PTEMSG_NOT_FOUND);

	p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
	p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
	data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

	memcpy( dm_config, (pDM_CONFIG)p_data, data_len );
	return ( PTEMSG_OK );
}

BYTE cdsapi_get_dm_config_snd( pDM_CONFIG dm_config, pCHAR error_msg )
{
	pPTE_MSG		p_msg_out;
	BYTE			rcode;

	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_SELECT,
								  0,
								  coreds_que_name,
								  interactive_que_name,
								  (BYTE *)dm_config->db_subsystem_name,
								  sizeof( dm_config->db_subsystem_name ),
								  DM_CONFIG_DATA );

	rcode = process_request_snd( p_msg_out, error_msg, false );
   free( p_msg_out );
   return ( rcode );
}

BYTE cdsapi_parse_router_record( pROUTER_TABLE router, pPTE_MSG p_msg_in )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;

	if (p_msg_in == NULL_PTR) return (PTEMSG_NOT_FOUND);

	p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
	p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
	data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

	memcpy( router, (pROUTER_TABLE)p_data, data_len );
	return ( PTEMSG_OK );
}

BYTE cdsapi_get_router_record_snd( pROUTER_TABLE router, pCHAR error_msg )
{
	pPTE_MSG		p_msg_out;
	BYTE			rcode;

	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_SELECT,
								  0,
								  coreds_que_name,
								  interactive_que_name,
								  (BYTE *)router->router_identifier,
								  sizeof( router->router_identifier ),
								  ROUTER_DATA );

	rcode = process_request_snd( p_msg_out, error_msg, false );
   free( p_msg_out );
   return ( rcode );
}

BYTE cdsapi_parse_tpdu_record( pTPDU tpdu, pPTE_MSG p_msg_in )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;

	if (p_msg_in == NULL_PTR) return(PTEMSG_NOT_FOUND);

	p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
	p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
	data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

	memcpy( tpdu, (pTPDU)p_data, data_len );
	return ( PTEMSG_OK );
}

BYTE cdsapi_get_tpdu_record_snd( pTPDU tpdu, pCHAR error_msg )
{
	pPTE_MSG		p_msg_out;
	BYTE			rcode;

	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_SELECT,
								  0,
								  coreds_que_name,
								  interactive_que_name,
								  (BYTE *)tpdu->tpdu_id,
								  sizeof( tpdu->tpdu_id ),
								  TPDU_DATA );

	rcode = process_request_snd( p_msg_out, error_msg, false );
   free( p_msg_out );
   return ( rcode );
}

BYTE cdsapi_parse_router_table( pROUTER_TABLE_LIST router_list, pPTE_MSG p_msg_in )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;

  if (p_msg_in == NULL_PTR) return(PTEMSG_NOT_FOUND);

	p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
	p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
	data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

	memcpy( router_list, (pROUTER_TABLE_LIST)p_data, data_len );
	return ( PTEMSG_OK );
}

BYTE cdsapi_get_router_table_snd( pCHAR error_msg )
{
	pPTE_MSG		p_msg_out;
	BYTE			rcode;

	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_ALL,
								  0,
								  coreds_que_name,
								  interactive_que_name,
								  NULL_PTR,
								  0,
								  ROUTER_DATA );

	rcode = process_request_snd( p_msg_out, error_msg, false );
   free( p_msg_out );
   return ( rcode );
}

BYTE cdsapi_parse_tpdu_table( pTPDU_TABLE_LIST tpdu_list, pPTE_MSG p_msg_in )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;

  if (p_msg_in == NULL_PTR) return(PTEMSG_NOT_FOUND);

	p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
	p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
	data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

	memcpy( tpdu_list, (pTPDU_TABLE_LIST)p_data, data_len );
	return ( PTEMSG_OK );
}

BYTE cdsapi_get_tpdu_table_snd( pCHAR error_msg )
{
	pPTE_MSG		p_msg_out;
	BYTE			rcode;

	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_ALL,
								  0,
								  coreds_que_name,
								  interactive_que_name,
								  NULL_PTR,
								  0,
								  TPDU_DATA );

	rcode = process_request_snd( p_msg_out, error_msg, false );
   free( p_msg_out );
   return ( rcode );
}

BYTE cdsapi_parse_dmconfig_table( pDM_CONFIG_LIST dmconfig_list, pPTE_MSG p_msg_in)
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;

  if (p_msg_in == NULL_PTR) return(PTEMSG_NOT_FOUND);

	p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
	p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
	data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

	memcpy( dmconfig_list, (pDM_CONFIG_LIST)p_data, data_len );
	return ( PTEMSG_OK );
}

BYTE cdsapi_get_dmconfig_table_snd(pCHAR error_msg )
{
	pPTE_MSG		p_msg_out;
	BYTE			rcode;

	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_GET_ALL,
								  0,
								  coreds_que_name,
								  interactive_que_name,
								  NULL_PTR,
								  0,
								  DM_CONFIG_DATA );

	rcode = process_request_snd( p_msg_out, error_msg, false );
   free( p_msg_out );
   return ( rcode );
}

BYTE cdsapi_delete_record_snd( pGENERIC_KEYS generic_keys, 
                               BYTE          app_data_type,
                               pCHAR         error_msg )
{
   pPTE_MSG    p_msg_out;
   BYTE        rcode;

   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_DELETE,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)generic_keys,
                          sizeof( GENERIC_KEYS ),
                          app_data_type );

	rcode = process_request_snd( p_msg_out, error_msg, true );
   free( p_msg_out );
   return ( rcode );
} 

BYTE cdsapi_update_dm_config_snd( pDM_CONFIG dm_config, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   BYTE        rcode;

   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_UPDATE,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)dm_config,
                          sizeof( DM_CONFIG ),
                          DM_CONFIG_DATA );

	rcode = process_request_snd( p_msg_out, error_msg, true );
   free( p_msg_out );
   return ( rcode );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_update_router_config_snd( pROUTER_TABLE router_table, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   BYTE        rcode;

   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_UPDATE,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)router_table,
                          sizeof( ROUTER_TABLE ),
                          ROUTER_DATA );

	rcode = process_request_snd( p_msg_out, error_msg, true );
   free( p_msg_out );
   return ( rcode );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_update_tpdu_config_snd( pTPDU tpdu, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   BYTE        rcode;

   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_UPDATE,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)tpdu,
                          sizeof( TPDU ),
                          TPDU_DATA );

	rcode = process_request_snd( p_msg_out, error_msg, true );
   free( p_msg_out );
   return ( rcode );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_insert_router_config_snd( pROUTER_TABLE router_table, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   BYTE        rcode;

   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_INSERT,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)router_table,
                          sizeof( ROUTER_TABLE ),
                          ROUTER_DATA );

	rcode = process_request_snd( p_msg_out, error_msg, true );
   free( p_msg_out );
   return ( rcode );
}



/************************************************************************/
/************************************************************************/
BYTE cdsapi_insert_tpdu_config_snd( pTPDU tpdu, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
   BYTE        rcode;

   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                          ST1_DB_INSERT,
                          0,
                          coreds_que_name,
                          interactive_que_name,
                          (BYTE *)tpdu,
                          sizeof( TPDU ),
                          TPDU_DATA );

 	rcode = process_request_snd( p_msg_out, error_msg, true );
   free( p_msg_out );
   return ( rcode );
}

