/*******************************************************************************
 * Copyright (c) 2001, Hypercom, Inc.
 * All Rights Reserved.
 *  
 * MODULE: jcbmain.h
 *  
 * TITLE:       
 *  
 *	DESCRIPTION: This module the header file for jcbmain.c
 *
 * APPLICATION: 
 *
 * AUTHOR:  
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\PVCS\PTE\Equitable\ncjcb\jcb0302.h  $
   
      Rev 1.0   Nov 13 2001 16:06:24   dirby
   Initial revision.
*
*******************************************************************/


/* Protoypes of functions used in jcb0302.c */
#ifndef JCB0302_H
#define JCB0302_H

INT  process_file_update_request( pPTE_MSG p_msg );
void process_file_update_response( pAUTH_TX p_auth_tx );
void map_0302_to_tlf01( pAUTH_TX p_auth_tx );
INT  send_response_to_GUI( pAUTH_TX p_auth_tx );
INT  validate_gui_inputs( pCHAR err_msg );

INT  populate_0312_resp_struct( pBYTE     resp_code,
                                pAUTH_TX  p_auth_tx,
                                pCHAR     err_msg );

void issue_0302_response( pBYTE     resp_code,
                          pAUTH_TX  p_auth_tx,
                          pCHAR     err_msg );


#endif
