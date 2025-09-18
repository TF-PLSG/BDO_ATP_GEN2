#ifndef _ARCHDS_H
#define _ARCHDS_H
BYTE db_get_ucf01_count_by_group( pUCF02 ucf02_ptr, pCHAR crf_count, pCHAR ErrorMsg );
BYTE db_get_hst01_list_all( pHST01 hst01_ptr, pHST01_LIST Hst01List, pCHAR ErrorMsg );
BYTE db_get_hst01_list_by_user( pHST01 hst01_ptr, pHST01_LIST Hst01List, pCHAR ErrorMsg );
BOOLEAN GetArchiveServiceName  ( pCHAR app_srvc_name );
BOOLEAN GetArchiveUserId       ( pCHAR app_user_id );
BOOLEAN GetArchivePassword     ( pCHAR app_password );
BYTE dbcommon_connect_Archds    ( pCHAR username, pCHAR password,  pCHAR alias, pCHAR database, pCHAR ErrorMsg );
INT isnum(pCHAR string , INT length);
void archds_get_error_warning_file_name_path(void );
INT archds_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail);
void archds_create_Error_Filename( );
void archds_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );
INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename();
void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);
#endif
