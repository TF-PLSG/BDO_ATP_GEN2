/******************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      prototypes.h
 *
 * TITLE:       ATPNETMAN FUNCTION PROTOTYPE DEFINITIONS
 *
 *	DESCRIPTION: This file contains prototypes of functions used
 *              withing atpnetman.
 *
 * APPLICATION: ATPNETMAN
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\atpnetman\prototypes.h  $
   
      Rev 1.0   Nov 04 2004 15:22:02   dirby
   Initial revision.
 *
 *****************************************************************************/

#ifndef PROTOTYPES_H
#define PROTOTYPES_H

/* MAIN.C */
void  display_usage( void );
INT   get_input_params( INT argc, pCHAR argv[] );

INT   xipc_instance( INT );
INT   get_ncf01_records( void );
void  populate_network_list( void );
void  populate_bins( pNCF01 ncf01_rec, pNCF01_REC rec_list );
void  verify_inputs( void );
void  get_host_state( void );
void  log_command( void );
void  compare_host_states( void );
INT   send_request( void );
void  wait_for_response( void );

/* MENUS.C */
void show_menus( void );
void display_netid_menu  ( void );
void display_netcmd_menu ( void );
void display_nettype_menu( void );
void display_netbin_menu ( void );
void display_main_header ( void );
INT  isnum(pCHAR in_str);

#endif
