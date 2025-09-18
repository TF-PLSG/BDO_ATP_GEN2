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
 *              withing atpnetmon.
 *
 * APPLICATION: ATPNETMAN
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\atpnetmon\prototypes.h  $
   
      Rev 1.0   Nov 04 2004 15:22:02   dirby
   Initial revision.
 *
 *****************************************************************************/

#ifndef PROTOTYPES_H
#define PROTOTYPES_H

/* MAIN.C */
void  display_usage( void );
INT   xipc_instance( INT );
INT   get_ncf01_records( void );
void  populate_network_list( void );
void  populate_bins( pNCF01 ncf01_rec, pNCF01_REC rec_list );
void  get_host_state( void );

#endif
