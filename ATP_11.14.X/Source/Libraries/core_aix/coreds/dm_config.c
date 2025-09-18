/******************************************************************************
  
   Module: dm_config.c
  
   Title: Core Dataserver dm_config table
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\COREDS\dm_config.c  $
   
      Rev 1.4   Nov 12 1999 13:51:02   rsolis
   Added support for SQLSERVER
   
      Rev 1.3   Nov 05 1999 16:32:58   MSALEH
   added sqlproto to include list
   
      Rev 1.2   Oct 08 1999 15:52:18   MSALEH
   include windows.h for GetPrivateProfileString
   
      Rev 1.1   Sep 30 1999 10:17:00   MSALEH
   AIX Modifications
   
      Rev 1.0   Apr 27 1999 15:08:10   MSALEH
   initial release
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#ifndef SQLSERVER
#include <sqlproto.h>
#endif
#include <windows.h>
#endif

#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "ptetimer.h"
#include "coreinfo.h"
#include "coredb.h"
#include "dbcommon.h"
#include "ntutils.h"
#include "dm_config.h"

BYTE PRIVATE DataSource;

/**************************************************************************
**************************************************************************/
BYTE get_data_source ()
{
   return(DataSource);
}

/**************************************************************************
**************************************************************************/
void set_data_source (BYTE value)
{
   DataSource = value;
}

/**************************************************************************
**************************************************************************/
void init_data_source ()
{
   BYTE datasource;
	CHAR temp_str[6];
	CHAR filename[256];

   GetAscendentConfigDirectory( filename );
   strcat( filename, "tf.ini" );

	GetPrivateProfileString( "DATA_SOURCE",
							       "DM_CONFIG",
							       "0",
							       temp_str,
							       sizeof( temp_str ),
							       filename );

   datasource = (BYTE)(atoi(temp_str));
	set_data_source(datasource);
}

/**************************************************************************
**************************************************************************/
BYTE parse_input_line( pCHAR buffer, pDM_CONFIG DMConfig )
{
   pCHAR ptr, fsptr;

   /* format of each input line is:
   name:contype:comport:baud:bytesize:parity:stopbits:tcpport:hostname:headlen:socktype:tpduproc:
   */
   ptr = buffer;

   /* Get Subsystem Name */
   fsptr = strchr(ptr, ':');
   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_subsystem_name, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   /* Get Connection Type */
   fsptr = strchr(ptr, ':');
 
   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_connection_type, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   /* Get Com Port */
   fsptr = strchr(ptr, ':');

   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_com_port, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   /* Get baud rate */
   fsptr = strchr(ptr, ':');

   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_baud_rate, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   /* Get byte size */
   fsptr = strchr(ptr, ':');

   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_byte_size, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   /* Get parity */
   fsptr = strchr(ptr, ':');

   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_parity, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   /* Get stop bits */
   fsptr = strchr(ptr, ':');

   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_stop_bits, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   /* Get tcp port */
   fsptr = strchr(ptr, ':');

   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_tcp_port, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   /* Get host name */
   fsptr = strchr(ptr, ':');

   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_host_name, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   /* Get header length */
   fsptr = strchr(ptr, ':');

   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_tcp_header_length, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   /* Get socket type */
   fsptr = strchr(ptr, ':');

   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_tcp_socket_type, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   /* Get tpdu processing */
   fsptr = strchr(ptr, ':');

   if (fsptr == NULL_PTR)
      return(PTEMSG_NOT_FOUND);
 
   *fsptr = (CHAR)NULL;
   strcpy(DMConfig->db_tpdu_processing, strlen(ptr) ? ptr : NULL_STR);
   ptr = fsptr + 1;

   return(PTEMSG_OK); 
}

/*************************************************************************************/
/*************************************************************************************/
BYTE ini_select_dmconfig_record( pCHAR Subsystem, pDM_CONFIG DmConfig, pCHAR ErrorMsg )
{
   BYTE           rc;
   INT            counter = 0;
   DM_CONFIG_LIST DMConfigList;

   rc = ini_get_dmconfig_table(&DMConfigList, ErrorMsg);
   
   if (rc != PTEMSG_OK)
      return(PTEMSG_NOT_FOUND);

   while(counter < DMConfigList.num_returned)
   {
      if (strcmp(DMConfigList.dm_config_record[counter].db_subsystem_name, Subsystem) == 0)
      {
         memcpy(DmConfig, &DMConfigList.dm_config_record[counter], sizeof(DM_CONFIG));
         return(PTEMSG_OK);

      }
      counter++;
   }
   return(PTEMSG_NOT_FOUND);
}

/*************************************************************************************/
/*************************************************************************************/
BYTE ini_update_dmconfig_record( pDM_CONFIG DmConfig, pCHAR ErrorMsg )
{
   BYTE           rc;
   INT            counter = 0;
   DM_CONFIG_LIST DMConfigList;

   rc = ini_get_dmconfig_table(&DMConfigList, ErrorMsg);
   
   if (rc != PTEMSG_OK)
      return(PTEMSG_NOT_FOUND);

   while(counter < DMConfigList.num_returned)
   {
      if (strcmp(DMConfigList.dm_config_record[counter].db_subsystem_name, DmConfig->db_subsystem_name) == 0)
      {
         memcpy(&DMConfigList.dm_config_record[counter], DmConfig, sizeof(DM_CONFIG));
         rc = ini_save_dmconfig_table( &DMConfigList, ErrorMsg );
         return(rc);
      }
      counter++;
   }
   return (PTEMSG_NOT_FOUND);
}

/*************************************************************************************/
/*************************************************************************************/
BYTE ini_get_dmconfig_table( pDM_CONFIG_LIST DMConfigList, pCHAR ErrorMsg )
{
   FILE       *dm_config;
   BYTE       rc;
   INT        num_records = 0;
   CHAR       filename  [256]; 
   CHAR       buffer    [256];

   /* try to open dm_config.ini */ 
   GetAscendentConfigDirectory( filename );
   strcat( filename, "dm_config.ini" );
   dm_config = fopen( filename, "r+" );

   if( dm_config == NULL )
   {
      sprintf( ErrorMsg, "ASCENDENT: ini_get_dm_config_table, Error opening Ascendent dm_config.ini" );
      return(PTEMSG_NOT_FOUND);
   } 

   /* file is open, read in one line at a time.... */  
   while( num_records <  DIALOG_LIST_SIZE)
   {
      fgets( buffer, sizeof( buffer ), dm_config );
      if( feof( dm_config ) )
         break;

      if( ferror( dm_config ) )
      {
         sprintf( ErrorMsg, "ASCENDENT: ini_get_dm_config_table, Error reading dm_config" );
         return(PTEMSG_NOT_FOUND);
      }

      /* ignore all comment lines, lines starting with a # */
      if( buffer[0] == '#' )
	continue;

      /* ....and parse the line into a structure... */
      rc = parse_input_line( buffer, &DMConfigList->dm_config_record[num_records] );
      if (rc != PTEMSG_OK)
      {
         sprintf( ErrorMsg, "ASCENDENT: parse_input_line, Error parsing dm_config");
         return(PTEMSG_NOT_FOUND);
      }


      /* keep track of the number of records processed. */
      num_records++;
   }

   DMConfigList->num_returned = num_records;
   /* we have reached end of file. All services have been processed. */ 
   fclose( dm_config );
   return (PTEMSG_OK);
}

/**************************************************************************
**************************************************************************/
BYTE ini_save_dmconfig_table( pDM_CONFIG_LIST DMConfigList, pCHAR ErrorMsg )
{
   FILE  *dm_config;
   CHAR  filename [256]; 
   CHAR  buffer   [256];
	INT   temp;

   /* try to open dm_config.ini */ 
   GetAscendentConfigDirectory( filename );
   strcat( filename, "dm_config.ini" );
   dm_config = fopen( filename, "w+" );

   if( dm_config == NULL )
   {
      sprintf( ErrorMsg, "ASCENDENT: ini_save_dm_config_table, Error opening dm_config.ini");
      return(PTEMSG_NOT_FOUND);
   } 

	for( temp = 0; temp < DMConfigList->num_returned; temp++ )
	{
		/* write each service info to the file */
		sprintf( buffer, "%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:%s:     \n", 
				 DMConfigList->dm_config_record[temp].db_subsystem_name,
				 DMConfigList->dm_config_record[temp].db_connection_type,
				 DMConfigList->dm_config_record[temp].db_com_port,
				 DMConfigList->dm_config_record[temp].db_baud_rate,
				 DMConfigList->dm_config_record[temp].db_byte_size,
				 DMConfigList->dm_config_record[temp].db_parity,
				 DMConfigList->dm_config_record[temp].db_stop_bits,
			         DMConfigList->dm_config_record[temp].db_tcp_port,
                                 DMConfigList->dm_config_record[temp].db_host_name,        
                                 DMConfigList->dm_config_record[temp].db_tcp_header_length,
                                 DMConfigList->dm_config_record[temp].db_tcp_socket_type,  
                                 DMConfigList->dm_config_record[temp].db_tpdu_processing);  

		fwrite( buffer, sizeof( CHAR ), strlen( buffer ), dm_config );
	}

	/* close dm_config.ini */
	fclose( dm_config );
	return(PTEMSG_OK);
}

