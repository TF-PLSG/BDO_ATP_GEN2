/******************************************************************************
  
        Module: notify.c
  
         Title: email API Routines
  
   Description: 

   $Log$
   Revision 1.0  2002/01/21 17:12:12  msaleh
   Initial revision.

 * 
 *    Rev 1.0   Jan 21 2002 17:12:12   msaleh
 * Initial revision.
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "basictyp.h"
#include "common.h"
#include "ntutils.h"

extern INT volatile GotResponse;
extern INT volatile State;

void dm_notify ( pBYTE tmpbuf, WORD len, pIPC_INFO ipc_info )
{ 
  pCHAR p;

  p = tmpbuf;
  while (isdigit(*p)) p++;
  *p = '\0';

  if (atoi(tmpbuf) == State)
     GotResponse = TRUE;
  else
     GotResponse = -State;
}

