/****************************************************************************************

   Module: srvccomm.h

   Title: Ascendent Services Monitor

   $Log:   N:\pvcs\PTE\CORE\Srvcmgr2\Gui\SRVCCOMM.H  $
   
      Rev 1.3   Nov 17 1998 12:12:32   MSALEH
   Added new functionality to manager
   logger/reply Primary/secondary designation,
   it works with logger usiong special message
   subtypes under MT_SYSTEM_REQUEST
   
      Rev 1.2   Sep 04 1998 13:06:26   skatuka2
   1. changed 'pinnacle' to 'ascendent'
   
      Rev 1.1   20 Aug 1998 09:03:08   rmalathk
   minor bug fixes and changes needed for applink.
   
      Rev 1.0   27 May 1998 16:54:22   rmalathk
   Initial Revision
   
*****************************************************************************************/
#ifndef SRVCCOMM
#define SRVCCOMM

#include "srvcinfo.h"

BYTE srvccomm_get_service_table( SERVICE_INFO services_table[], pINT num_services,
                                   pCHAR err_msg );

BYTE srvccomm_control_service( pCHAR service_name, BYTE action, pCHAR error_msg );

BYTE srvccomm_load_rps_flag_service(  pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_load_OBS_records_service(  pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_load_VISA3DS2_records_service(  pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_load_AMEX3DS2_records_service(  pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_load_DINERS3DS2_records_service(  pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_load_JCB3DS2_records_service(  pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_load_JCB3DS2_tfinirecords_service(  pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_load_MidCpng_tfinirecords_service(pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_load_CUP3DS2_records_service(  pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_reload_falcon_flag_service(  pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_reload_SecExpiryDateValidation_flag_service(  pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_process_all( BYTE action, pCHAR error_msg );

BYTE srvccomm_confirm_status( pCHAR error_msg );

BYTE srvccomm_ping_service( pCHAR que_name, pCHAR error_msg );

BYTE srvccomm_DB_control_service( pCHAR que_name, BYTE action ,pCHAR error_msg   );

BYTE srvccomm_get_primary_info( pCHAR que_name, pCHAR error_msg, pINT status );

INT srvccomm_change_connection( pCHAR machine_id );

INT srvccomm_disconnect();

BYTE srvccomm_send_be_primary_command(pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_send_switch_primary_command(pCHAR que_name, pCHAR error_msg);

BYTE srvccomm_load_time_out_RC_and_reversal_attempts_values_service(pCHAR que_name,  pCHAR error_msg, BYTE ST1 );

BYTE srvccomm_vertexon_card_status_reload_service( pCHAR que_name, pCHAR error_msg );

#endif


