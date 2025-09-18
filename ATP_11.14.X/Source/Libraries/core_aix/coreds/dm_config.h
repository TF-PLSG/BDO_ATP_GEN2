#ifndef DM_CONFIG_H
#define DM_CONFIG_H

/* Data source definitions */
#define DATA_SOURCE_IS_DB  0
#define DATA_SOURCE_IS_INI 1

#define NULL_STR           ""

BYTE get_data_source    ();
void set_data_source    (BYTE value);
void init_data_source   ();

BYTE ini_select_dmconfig_record( pCHAR Subsystem, pDM_CONFIG DmConfig, pCHAR ErrorMsg );
BYTE ini_update_dmconfig_record( pDM_CONFIG DmConfig, pCHAR ErrorMsg );
BYTE ini_get_dmconfig_table( pDM_CONFIG_LIST DMConfigList, pCHAR ErrorMsg );
BYTE ini_save_dmconfig_table( pDM_CONFIG_LIST DMConfigList, pCHAR ErrorMsg );

#endif

