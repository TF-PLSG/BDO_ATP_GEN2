#include "falcontroller.h"

INT get_FRT_timeout_for_Falcon( void )
{
    DWORD rc;
    CHAR filename     [MAX_FILENAME_SIZE]= {0};
    CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
    CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
    CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
    INT  ret_val = true;

    /* Determine directory and filename of the .ini file. */
    memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
             "FALCON_REAL_TIME",     /* points to section name       */
             "TIMEOUT",  			/* points to key name           */
             "10",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
    );

    /* Convert entry from a string into a number. */
    memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
    strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
    t_secs =  atoi(tmpstring);
    if(t_secs == 0)
    {
	   t_secs = 10;
    }

    return( ret_val );
}

void falcntl_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( falcntl_Erro_warning_Filename, falcntl_error_warning_file_path );
    strcat( falcntl_Erro_warning_Filename, falcntl_module_error_warning_file_name );
	strcat( falcntl_Erro_warning_Filename, ".EW." );
    strncat(falcntl_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(falcntl_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(falcntl_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

void falcntl_get_error_warning_file_name_path(void )
{
	DWORD rc;
	CHAR  filename   [80] = {0};
	CHAR  tmpstr     [80] = {0};
	CHAR  appname[512] = {0};
	CHAR  falcntl_error_warning_file_size[5] = {0};

	/* Get path to the tf.ini file. */
	memset( tmpstr, 0x00, sizeof(tmpstr) );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	GetAppName (appname) ;

	/*Read path for creating file in order to log db statics and db oracle error messages */
	rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   falcntl_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(falcntl_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	    );

	rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  appname,       	/* points to key name             */
								  "",  					/* Default string                 */
								  falcntl_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(falcntl_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	rc = GetPrivateProfileString(
							  	  "ERROR_WARNING",             /* points to section name         */
								  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
								  "500",  					/* Default string                 */
								  falcntl_error_warning_file_size,              	/* points to destination buffer   */
								  sizeof(falcntl_error_warning_file_size)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(falcntl_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/


	if((strlen(falcntl_error_warning_file_path) > 0) &&
	  (strlen(falcntl_module_error_warning_file_name)> 0))
	{
		falcntl_create_Error_Filename();
	}
}

INT falcntl_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR  Buffer[1024]={0};
	CHAR  card_tid_mid_details [256] = {0};
	CHAR  tempcard[21] = {0};
	CHAR  masked_cardnum[21] = {0};
	char  *cardIndex = NULL ;
	FILE  *fp;
	UINT  file_size = 0 ;
	char  buf[256] = {0} ;

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(falcntl_Erro_warning_Filename);
    path_len = strlen(falcntl_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strncmp(&falcntl_Erro_warning_Filename[len-4], current_mmdd,4) )
    {
		/* Now create the new filename.*/
    	falcntl_create_Error_Filename();
    }
	if((fp = fopen(falcntl_Erro_warning_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return 0;
	}
	strcpy(Buffer,time_date);
	strcat(Buffer,":");
	if(sev == 1)
	{
		strcat(Buffer," INFO");
	}
	else if (sev == 2)
	{
		strcat(Buffer," WARN");
	}
	else
	{
		strcat(Buffer," ERROR");
	}
	strcat(Buffer,": ");
	strcat(Buffer,Error_Buf);
	strcat(Buffer," ");
	if(detail == 1)
	{
		if (strlen(Auth_Tx.TLF01_details.card_num)> 0)
		{
			/* Masking card number first 5 and last 4 in clear form remaining all masked. */
			cardlen = strlen( Auth_Tx.TLF01_details.card_num );
			strncpy( tempcard, Auth_Tx.TLF01_details.card_num, cardlen);
			for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
			{
				tempcard[nIndex] = '*';
			}
#ifdef TESTING
			sprintf(card_tid_mid_details,"TRAN_ID %s CNBR %s TID %s MID %s STAN %s RRN %s",
					Auth_Tx.TLF01_details.primary_key.transaction_id,
					tempcard,
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		else
		{
			sprintf(card_tid_mid_details,"TRAN_ID %s TID %s MID %s STAN %s RRN %s",
					Auth_Tx.TLF01_details.primary_key.transaction_id,
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
#else
			sprintf(card_tid_mid_details,"CNBR %s TID %s MID %s STAN %s RRN %s",
					tempcard,
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		else
		{
			sprintf(card_tid_mid_details,"TID %s MID %s STAN %s RRN %s",
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
#endif

		strcat(Buffer,card_tid_mid_details);
		strcat(Buffer," ");
	}
	strcat(Buffer, func);
	strcat(Buffer,"\n");
	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
		fseek (fp, 0, SEEK_END);
		file_size=ftell (fp);
	}
	fseek (fp, 0, SEEK_END);
	file_size=ftell (fp);

	if( file_size >= Max_File_Size_Defined )
	{
		sprintf(buf,"ERROR_WARNING_FILE  size is exceeding the permissible size, need attention" );
		TxUtils_Send_Msg_To_Operator( 1, buf,1, ALERT_MSG, "", 4, INFO_ERROR,NULL,NULL,NULL );
	}
	fclose(fp);
	return  0;
}

void falcntl_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
	INT  monitor_flag = 0;
	INT  eventlog_flag = 0;
	INT  msg_type = 0;
	BYTE severity = 0;
	BYTE log_type[20] = {0};
	CHAR text_message [200] = {0};
	INT  Appname_len = 0;
	CHAR  appname[512] = {0};

	GetAppName (appname) ;
	/* Set Monitor flag */
	if ( dest == 1  ||  dest == 3 )
		monitor_flag = 1;
	else
		monitor_flag = 0;

	/* Set message type and severity */
	if ( sev == 2 )
	{
		msg_type = WARN_MSG;
		severity = '3';
		strcpy( log_type, WARNING_ERROR );
	}
	else if ( sev == 1 )
	{
		msg_type = INFO_MSG;
		severity = '0';
		strcpy( log_type, INFO_ERROR );
	}
	else if ( sev == 3 )
	{
		msg_type = ALERT_MSG;
		severity = '4';
		strcpy( log_type, FATAL_ERROR );
	}

	/* Make sure text message is not too long. */
	sprintf(text_message,"%s ,",appname);
	Appname_len = strlen(text_message);

	memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
	/* Call function to post the message. */
	falcntl_Log_error_warning_to_File(text_message,sev,func,details);

	if(monitor_flag == 1)
	{
		if(details == 1)
		{
			TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
			   	   	   	   	   	   	 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 Auth_Tx.TLF01_details.terminal_id,
										 Auth_Tx.TLF01_details.card_num,
										 Auth_Tx.TLF01_details.merchant_id );
		}
		else
		{
			TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
										 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
		}
	}

	return;
}

BOOLEAN FalService_Send_To_Monitor( BYTE mtype, BYTE st1, BYTE st2,
                                    pBYTE msg,   INT  len )
{
	BOOLEAN  retval = true;
	pPTE_MSG p_msg = 0;
	CHAR     ConnectionId[MAX_ORIG_INFO_SIZE+1] = "";
	CHAR     err_mesg[200] = "";
	CHAR     Buff[256];
	LONG     ret_code;
	XINT     ReturnCode = 0;

	p_msg = ptemsg_build_msg( mtype,
                             st1,
                             st2,
                             applnk_que_name,
                             application_que_name,
                             msg,
                             len,
                             0 );

	if ( p_msg != NULL_PTR )
	{
		memset( ConnectionId, 0x00, sizeof(ConnectionId) );
		ReturnCode = ReadMemMonitorTable( ConnectionId );
		if ( ConnectionId[0] != 0 )
		{
			ptemsg_set_orig_comms_info( p_msg, ConnectionId );
			ret_code = pteipc_send( p_msg, applnk_que_name );
			if ( 0 > ret_code )
			{
				pteipc_get_errormsg( ret_code, err_mesg );
				sprintf( Buff,
                    "Failed transmit to que: %s. %s",
                     applnk_que_name, err_mesg );
				falcntl_log_message( 1, 3,Buff,"FalService_Send_To_Monitor", 1 );
				retval = false;
			}
		}
		free( p_msg );
	}
	else
	{
		retval = false;
		falcntl_log_message( 1, 3,"Insufficient Memory to build response to Monitor",
    		  	  	  	  	  "FalService_Send_To_Monitor", 1 );
	}

	return( retval );
}
INT build_and_send_request_message( BYTE     msgtype,
                                    BYTE     subtype1,
                                    BYTE     subtype2,
                                    pBYTE    que_name,
                                    pBYTE    buffer,
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg )
{
   CHAR            Buff[200];
   pPTE_MSG        p_msg    = NULL_PTR;
   INT             ret_code = true;
   LONG            msg_ret_code;

   p_msg = ptemsg_build_msg( msgtype,
                             subtype1,
                             subtype2,
                             que_name,
                             application_que_name,
                             buffer,
                             length,
                             app_type );

   if ( NULL_PTR == p_msg  )
   {
      strcpy( err_msg, "Insufficient Memory to build PTE message" );
      ret_code = false;
   }
   else
   {
      /* Set originator communications information. */
      if ( 0 != strcmp( NULL_STR, comm_info ) )
         ptemsg_set_orig_comms_info( p_msg, comm_info );

      /* Send message que_name */
      msg_ret_code = pteipc_send(p_msg, que_name);
      if(msg_ret_code < 0)
	  {
         pteipc_get_errormsg(msg_ret_code, err_msg);
         memset( Buff, 0x00, sizeof(Buff) );
         strcpy( Buff, "Failed transmit to queue: " );
         strcat( Buff, que_name );
         strncat(Buff, err_msg, 65 );
         ret_code = false;
	  }
      free(p_msg);
   }
   return( ret_code );
}
INT isnum(pCHAR in_str)
{
   INT i;
   INT length =0;

   length = (INT)strlen(in_str);
	if(length == 0)
	{
		return(false);
	}
	for (i = 0; i < length; i++)
	{
	  if (!isdigit(in_str[i]))
	    return(false);
	}
   return(true);
}
void format_date(pCHAR temp)
{
   struct timeb   time_structure;   /* Time for ftime*/
   time_t long_time_since_1970;     /* Number of seconds since 01/01/1970 A.D. 12:00 A.M.*/
   struct tm *current_time;         /* Current time structure*/
   CHAR   temp1[4] = "0";

   ftime(&time_structure);
   long_time_since_1970 = time_structure.time;
   current_time = localtime(&long_time_since_1970);
   current_time->tm_mon++;
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;
   itoa(current_time->tm_year, temp, 10);
   itoa(current_time->tm_mon, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 4, temp1, 2);
   else
      memcpy(temp + 4, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_mday, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 6, temp1, 2);
   else
      memcpy(temp + 6, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_hour, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 8, temp1, 2);
   else
      memcpy(temp + 8, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_min, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 10, temp1, 2);
   else
      memcpy(temp + 10, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_sec, temp1 + 1, 10);
   if (strlen(temp1) == 2)
      memcpy(temp + 12, temp1, 2);
   else
      memcpy(temp + 12, temp1 + 1, 2);
   return;
}

void get_date( pCHAR time_date, pCHAR date_str )
{
	memset ( date_str,  0x00,         9 );
	strncpy( date_str,  time_date,    4 );
	strncat( date_str, &time_date[5], 2 );
	strncat( date_str, &time_date[8], 2 );
}


 void get_time( pCHAR time_date, pCHAR time_str )
 {
    memset ( time_str,0x00,7 );
    strncpy( time_str, &time_date[11], 2 );
    strncat( time_str, &time_date[14], 2 );
    strncat( time_str, &time_date[17], 2 );

 }
