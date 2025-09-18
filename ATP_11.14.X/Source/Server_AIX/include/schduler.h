/*******************************************************************************
*  
* MODULE:      schduler.h
*  
* TITLE:       
*  
* DESCRIPTION:         
*
* APPLICATION: Ascendent Scheduler
*
* AUTHOR:  Ram Malathkar
*
* REVISION HISTORY:
*
* $Log:   N:\PVCS\PTE\Equitable\App_lib\schduler.h  $
   
      Rev 1.1   Jan 18 2000 16:35:42   ddabberu
   removed c++ style comments
   
      Rev 1.0   02 Mar 1999 14:49:04   rmalathk
   Initial Release
   
   
      Rev 1.0   Dec 04 1998 14:44:08   MSALEH
   initial release
*
*******************************************************************************/
#ifndef __SCHEDULER__
#define __SCHEDULER__

#include "app_info.h"
#include "tx_dbstruct.h"
#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#include "equitdb.h"

#ifdef _DEBUG
   #define PRINT( x ) printf( "%s\n", x )
#else
   #define PRINT( x )
#endif

typedef struct
{
   JBF01    jbf01;
   BYTE     command_line[101];
   BYTE     parameters[101];
   BOOLEAN  is_spawned;
   BOOLEAN  is_db_updated;
} LOCAL_JOB_TABLE, *pLOCAL_JOB_TABLE;


#endif
