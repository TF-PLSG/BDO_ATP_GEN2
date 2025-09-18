/**************************************************************************************************

MODULE:      falcontroller.C

TITLE:       Falcon Controller Main.

DESCRIPTION: Main startup and end process routines.

APPLICATION: Equitable

AUTHOR:  Amith Karunakar

REVISION HISTORY: version 1.0 ATP 11.6
*******************************************************************************/

#include "falcontroller.h"

void MainProcessor()
{
	pPTE_MSG p_msg = 0 ;
	LONG     lReturnCode = 0L ;
	CHAR     strBuffer [256] = {0} ;
	CHAR     strTemp [256] = {0} ;
	CHAR     strAppName [MAX_APP_NAME_SIZE+1] = {0} ;
	CHAR     AuthNumber[7];
	BOOLEAN  bReturnCode = TRUE ;
	INT      err_ctr = 0;
	INT      rcode;

	memset (&Auth_Tx, 0, sizeof Auth_Tx) ;

	GetAppName(strAppName) ;
	strcpy (falcntl_que_name, strAppName) ;
	strcat (falcntl_que_name, "A") ;
	falcntl_get_error_warning_file_name_path();

	#ifdef _DEBUG
	   if( !pteipc_init_single_instance_app( strAppName, "pte_ipc" ) )
	#else
	   if( !pteipc_init_multiple_instance_app( strAppName, ServiceName,"pte_ipc"))
	#endif
	   {
	      sprintf (strTemp,
	              "%s: MainProcessor: Failed to Create XIPC queues",
	               strAppName) ;
	      falcntl_log_message( 1, 3,strTemp, "MainProcessor", 0 );

	      /* signal that we are ready to shut down and quit */
	#ifdef _DEBUG
	      pteipc_shutdown_single_instance_app();
	#else
	      pteipc_shutdown_multiple_instance_app();
	#endif
		  MainProcessDone = 1 ;
	      return;
	   }
	sprintf(strTemp, "Started the Falcon Controller Service: %s", ServiceName);
	falcntl_log_message( 1, 1,strTemp, "MainProcessor", 0 );

	get_FRT_timeout_for_Falcon();

	while (!EndProcessSignalled)
	{
		/* You are blocked here waiting for a message on either app queue or
	       * control que.  If there is no message on either que for 5 seconds,
	       * the blocking call returns.  Application_que_name and control_que_name
	       * are populated in pteipc_init_multiple_instance_app (pteipc.c).
	    */
	    p_msg = pteipc_receive( application_que_name, control_que_name,
	                              5, &lReturnCode);

	    if(p_msg != NULL_PTR)
	    {
	    	err_ctr = 0;
	        PinnacleMsgHandler (p_msg);
	        free(p_msg);
	    }
	    else if(lReturnCode == QUE_ER_TIMEOUT)
	    {
	    	/* Queue has timed out. There is nothing in it. */
	    }
	    else
	    {
	    	pteipc_get_errormsg(lReturnCode, strTemp);
	        sprintf( strBuffer,
	                 "Error on pteipc_receive %s, error code: %ld",
	                  strTemp, lReturnCode );
	        falcntl_log_message( 1, 3,strTemp, "MainProcessor", 0 );

	        /* There is a problem with the queue.  Delay 1 second
	          * so as to not flood Applnk's queue and syslog.
	        */
	        #ifdef WIN32
	           Sleep( 1000 );
	        #else
	           usleep( 1000000 );
	        #endif

	        /* Inform Operators if service's queue is no longer readable. */
	        if ( ++err_ctr > 10 )
	        {
	        	err_ctr = 0;
	            sprintf( strBuffer,
	                       "SHUT DOWN %s. UNABLE TO ACCESS QUEUE!!!!",
	                     ServiceName );
	            falcntl_log_message( 1, 3,strTemp, "MainProcessor", 0 );
	            #ifdef WIN32
	               Sleep (5000);
	            #else
	               usleep (5000000);
	            #endif
	        }
	     }
	}

	sprintf( strTemp, "Shutting down the Falcon Authorization Service: %s", ServiceName);
	falcntl_log_message( 1, 1,strTemp, "MainProcessor", 0 );

	/* Log out of XIPC */
	#ifdef _DEBUG
	   pteipc_shutdown_single_instance_app();
	#else
	   pteipc_shutdown_multiple_instance_app();
	#endif
	   MainProcessDone = 1 ;
	return;
}

void EndProcess(void)
{
	CHAR Buffer[100] = "";

	sprintf( Buffer, "Shutting down %s Service, version %s", ServiceName, Version );
	falcntl_log_message( 1, 1, Buffer, "EndProcess", 0);

	#ifdef _DEBUG
    	pteipc_trace_off();
	#endif
}

void PinnacleMsgHandler(pPTE_MSG p_msg_in)
{
    pPTE_MSG_DATA   p_msg_data = NULL_PTR;
    pBYTE			p_data;
    CHAR     		destination_que[7];
    pPTE_MSG 		p_cur_msg = 0 ;
    pPTE_MSG 		p_msg_out = 0 ;
    BYTE     		MessageType = 0 ;
    BYTE     		AppDataType = 0 ;
    pPTE_MSG_DATA 	pATEMessageData = 0;
    pBYTE			pATEMessageDataData = 0;
    BOOLEAN  		bReturnCode = true ;
    BYTE     		ReturnCode = 0 ;
    CHAR     		strTemp[256] = {0} ;
    pCHAR orig_queue=NULL_PTR;
    memset (&Auth_Tx, 0, sizeof (AUTH_TX)) ;

    p_cur_msg = p_msg_in ;

    MessageType = ptemsg_get_msg_type (p_cur_msg) ;
    switch (MessageType)
    {
		case MT_AUTH_RT_REQUEST:
			/*Reading requests from txcntl*/
			process_txcntl_request(p_cur_msg);

			break;

		case MT_AUTH_RESPONSE:

			falcntl_process_response_message(p_cur_msg);

			break;

		case MT_TIMER_REPLY:

			falcntl_process_clear_timer_reply(p_cur_msg);

			break;

		case MT_TIMER_TIMEOUT:

			falcntl_process_time_out_message(p_cur_msg);

			break;

		case MT_DB_REPLY:
			/*fconds will send a reply with  PTEMSG_OK ignore it*/
			if ( PTEMSG_OK != ptemsg_get_result_code(p_cur_msg) )
			{
				orig_queue = ptemsg_get_msg_orig_queue(p_cur_msg);
				if(strncmp(orig_queue,ftimer_que_name,strlen(ftimer_que_name)))
				{
					/*Key not found in the ftimerds
					 * need to take action for this
					 * */
					falcntl_log_message(2, 1, "Lookup key not found in ftimer, dropping transaction", "PinnacleMsgHandler", 1);
				}
			}
			break;
		case MT_AUTH_RT_EMPTY_RESPONSE:

			falcntl_log_message(3, 1, "Empty response message recived from falcon to falcntl", "PinnacleMsgHandler", 1);
			break;

	    case MT_SYS_MONITOR:

	        if ( ST1_MONITOR_UP == ptemsg_get_msg_subtype1(p_msg_in) )
	        {
	        	/* This is a test message from Monitor. Just respond to it. */
	            (void)FalService_Send_To_Monitor( MT_SYS_MONITOR,
	                                             ST1_MONITOR_UP,
	                                             ST2_NONE,
	                                             NULL_STR,
	                                             0 );
	         }
	         break;

		case MT_SYSTEM_REQUEST:
            ptesystm_system_msg_handler (p_msg_in) ;
            if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
            {
               /* When pinged, display application version on Monitor. */
               sprintf( strTemp,
                       "Pinged -> Falcon Controller Service: %s, version %s",
                        ServiceName, Version );
               falcntl_log_message( 1, 1,strTemp, "PinnacleMsgHandler", 0 );
            }
			break;

		case MT_SYSTEM_REPLY:
			break ;

	    default:
	    	orig_queue = ptemsg_get_msg_orig_queue(p_cur_msg);
	          sprintf (strTemp, "Unknown message type received in the Pinnacle Message Handler recived from(%s), msg type(%d)",orig_queue,MessageType) ;
	          falcntl_log_message( 2, 2,strTemp, "PinnacleMsgHandler", 0 );
			break ;
    }
}

INT validate_date_yyyymmdd( pBYTE date_str)
{
   INT  ret_val = false;
   INT  imonth, iday, iyear;
   CHAR temp_month[3]="";
   CHAR temp_day[3]  ="";
   CHAR temp_year[5] ="";

   /* Validate date is numeric. */
   if ( (true == isnum(date_str)) && (8 == strlen(date_str)) )
   {
      /* Validate month */
      memcpy( temp_month, date_str+4, 2 );
      imonth = atoi(temp_month);
      if ( (1 <= imonth) && (imonth <= 12) )
      {
         /* Month is valid; check the day. */
         memcpy( temp_day, date_str+6, 2 );
         iday = atoi(temp_day);
         if ( (1 <= iday) && (iday <= 31) )
         {
            if ( iday == 31 )
            {
               if ( (imonth == 1) || (imonth == 3) || (imonth == 5) ||
                    (imonth == 7) || (imonth == 8) || (imonth == 10)||
                    (imonth == 12) )
               {
                  ret_val = true;
               }
            }
            else if ( (iday == 30) && (imonth != 2) )
            {
               ret_val = true;
            }
            else if ( (iday == 29) && (imonth == 2) )
            {
               /* This must be a leap year to be valid. */
               memcpy( temp_year, date_str, 4 );
               iyear = atoi(temp_year);
               if ( (iyear%4) == 0 )
                  ret_val = true;
               /*else
                  sprintf( err_buf, "Too many days for February (%s)",date_str);*/
            }
            else
               ret_val = true;

            /* Validate the year. */
            memcpy( temp_year, date_str, 4 );
            iyear = atoi(temp_year);
            if ( (iyear < 2000) || (iyear > 2200) )
            {
               ret_val = false;
              /*sprintf( err_buf, "ERROR: Invalid date (%s). Bad Year",date_str);*/
            }
         }
         else
         {
            /*sprintf( err_buf, "ERROR: Invalid date (%s). Bad Day.", date_str );*/
         }
      }
      else
      {
         /*sprintf( err_buf, "ERROR: Invalid date (%s). Bad Month.", date_str );*/
      }
   }
   else
   {
     /* sprintf( err_buf, "ERROR: Invalid date (%s).", date_str );*/
   }
   if ( true != ret_val )
      /*strcat(  err_buf, " Must be in format YYYYMMDD." );*/

   return( ret_val );
}

INT send_msg_to_authorizer( BYTE msgtype )
{
   INT   ret_val=0;
   CHAR  err_buf[100]={0};
   CHAR  Buffer[200] = {0};

   if((0 == strncmp(Auth_Tx.TLF01_details.card_num,"7",1)) &&
		   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp",3)))
   {
	   ret_val = build_and_send_request_message( msgtype,
												0,
												0,
												txcntl_que_name,
												(pBYTE)&Auth_Tx,
												Auth_Tx_Size,
												0,
												NULL_STR,
												err_buf   );
	}

   else if((0 == strncmp(Auth_Tx.TLF01_details.product_codes[2].quantity,"1",1)) &&
		   (0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp",3)))
   {
	   if (MT_RT_RESPONSE_DECLINE == msgtype)
	   {
		   ret_val = build_and_send_request_message( MT_RT_RESPONSE_DECLINE,
												  	  0,
													  0,
													  (pBYTE)mp_pos_que_name,
													  (pBYTE)&Auth_Tx,
													  Auth_Tx_Size,
													  0,
													  NULL_STR,
													  err_buf   );
	   }
	   else
	   {
		   ret_val = build_and_send_request_message( MT_AUTH_REQUEST,
													 0,
													 0,
													 (pBYTE)mp_nclink_que_name,
													 (pBYTE)&Auth_Tx,
													 Auth_Tx_Size,
													 0,
													 NULL_STR,
													 err_buf   );
	   }

   }
   else
   {
		ret_val = build_and_send_request_message( msgtype,
													0,
													0,
													(pBYTE)txcntl_que_name,
													(pBYTE)&Auth_Tx,
													Auth_Tx_Size,
													0,
													NULL_STR,
													err_buf   );
   }

   if ( ret_val == false )
   {
      memset( Buffer, 0x00, sizeof(Buffer) );
      sprintf( Buffer,
              "Unable to forward txn to authorizer, %s.  Error = %s",
			  txcntl_que_name, err_buf );
      falcntl_log_message( 1, 3, Buffer, "send_msg_to_authorizer",1 );
   }
   falcntl_log_message( 2, 1, "Sent response back to the txcntl", "send_msg_to_authorizer",1 );
   return( ret_val );
}

void falcntl_process_time_out_message(pPTE_MSG p_cur_msg)
{
	pBYTE          p_data            = NULL_PTR;
	pPTE_MSG_DATA  p_msg_data        = NULL_PTR;
	pPTE_MSG       p_echo_msg        = NULL_PTR;
	LONG           timer_reply_size  = 0;
	LONG           data_len;
	CHAR           app_data1[32];
	CHAR           app_data2[32];
	CHAR           lookup_key[LOOKUP_KEY_SIZE]={0};
	CHAR           err_buf[100];
	CHAR  system_date[25] = {0};
	CHAR	date_str [ 9] = {0};
	CHAR	time_str [ 7] = {0};
	CHAR	time_date[25] = {0};
	CHAR	temp_time[11] = {0};
	CHAR	temp_secs[11] = {0};
	CHAR 	temp_tran[21] = {0};
	CHAR 	strError[512] = {0} ;
	CHAR    yyyymmdd[25] = {0};
	INT 	time_diff	 = 0;
	AUTH_TX        host_auth_tx;
	BYTE           buffer2[AUTH_TX_SIZE]= {0};

	memset( &Auth_Tx, 0x00, Auth_Tx_Size );
	timer_reply_size = ptetimer_get_timer_reply( p_cur_msg, lookup_key, app_data1,
	   app_data2, (pBYTE)&Auth_Tx,
	   Auth_Tx_Size );

	p_echo_msg = ptemsg_get_chained_pte_msg(          p_cur_msg, 2 );
	p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
	p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
	data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );


	memset(&Ftlf01, 0x00, sizeof(Ftlf01));

	strncpy(Ftlf01.tran_id, Auth_Tx.TLF01_details.primary_key.transaction_id, sizeof(Ftlf01.tran_id) -1);

	strcpy (strError,"Timeout Mode - no response from FAI") ;
	falcntl_log_message(2,1,strError,"falcntl_process_time_out_message",1);

	if (strlen(Auth_Tx.TLF01_details.system_date) < 8 || (validate_date_yyyymmdd(Auth_Tx.TLF01_details.system_date) == false))
	{
	   memset( yyyymmdd, 0x00, sizeof(yyyymmdd) );
	   format_date(yyyymmdd);

	   strncpy(Auth_Tx.TLF01_details.system_date, yyyymmdd, sizeof(Auth_Tx.TLF01_details.system_date)-1);
	   strncpy(Ftlf01.sys_date,Auth_Tx.TLF01_details.system_date, sizeof(Ftlf01.sys_date)-1);
	   strncpy(Ftlf01.handler_queue,Auth_Tx.TLF01_details.handler_queue, sizeof(Ftlf01.handler_queue)-1);
	}
	else
	{
	   strncpy(Ftlf01.sys_date,Auth_Tx.TLF01_details.system_date, sizeof(Ftlf01.sys_date)-1);
	   strncpy(Ftlf01.handler_queue,Auth_Tx.TLF01_details.handler_queue, sizeof(Ftlf01.handler_queue)-1);
	}

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );
	get_date( time_date, date_str );
	get_time( time_date, time_str );
	strncpy(temp_time, &date_str[4], 4);/* MMDD */
	strncat(temp_time, time_str, 6);	/* hhmmss   */
	strncpy( Ftlf01.tran_resp_recv_time, temp_time, sizeof(Ftlf01.tran_resp_recv_time)-1);
	Ftlf01.tran_resp_recv_time[sizeof(Ftlf01.tran_resp_recv_time)-1] ='\0';

	strncpy(temp_secs, &Auth_Tx.TLF01_details.date_yyyymmdd[4], 4);
	strncat(temp_secs, Auth_Tx.TLF01_details.time_hhmmss, 6);
	strncpy(Ftlf01.tran_req_sent_time, temp_secs, sizeof(Ftlf01.tran_req_sent_time)-1);
	Ftlf01.tran_req_sent_time[sizeof(Ftlf01.tran_req_sent_time)-1] ='\0';

	strncpy(Ftlf01.tran_req_sent_mode, "RT", 2);

	/*copy resp recv mode */
	strncpy(Ftlf01.tran_resp_recv_mode, "TO", 2);
	strcpy(Ftlf01.response_text, "timeout, no response from FAI");

	/* copy decision codes */
	strncpy(Ftlf01.decision_code_1_value, 0x00, sizeof(Ftlf01.decision_code_1_value)-1);
	strncpy(Ftlf01.decision_code_2_value, 0x00, sizeof(Ftlf01.decision_code_2_value)-1);
	strncpy(Ftlf01.decision_code_3_value, 0x00, sizeof(Ftlf01.decision_code_3_value)-1);
	strncpy(Ftlf01.decision_code_4_value, 0x00, sizeof(Ftlf01.decision_code_4_value)-1);
	strncpy(Ftlf01.decision_code_5_value, 0x00, sizeof(Ftlf01.decision_code_5_value)-1);
	strncpy(Ftlf01.decision_code_6_value, 0x00, sizeof(Ftlf01.decision_code_6_value)-1);
	strncpy(Ftlf01.decision_code_7_value, 0x00, sizeof(Ftlf01.decision_code_7_value)-1);
	strncpy(Ftlf01.decision_code_8_value, 0x00, sizeof(Ftlf01.decision_code_8_value)-1);
	strncpy(Ftlf01.decision_code_9_value, 0x00, sizeof(Ftlf01.decision_code_9_value)-1);
	strncpy(Ftlf01.decision_code_10_value, 0x00, sizeof(Ftlf01.decision_code_10_value)-1);

	(void)insert_db_record( FTLF01_DATA );

	if ( false == send_msg_to_authorizer(MT_RT_RESPONSE_TIMEOUT) )
	{
		sprintf( err_buf,
				"Unable to complete processing of response msg. MsgType: %s, processing code: %s",
				Auth_Tx.TLF01_details.message_type,
				Auth_Tx.TLF01_details.processing_code);
		falcntl_log_message( 1, 3, err_buf, "falcntl_process_time_out_message",1 );
	}


}
INT insert_db_record( BYTE data_type )
{
   INT    ret_val = true;
   INT    length;
   CHAR   db_table[6]  = "";
   CHAR   err_buf[100] = "";
   CHAR   Buffer[200];
   BYTE   db_buffer[AUTH_TX_SIZE + FTLF01_SIZE] = "";

   if ( data_type == FTLF01_DATA )
   {

         memcpy( db_buffer, &Auth_Tx, sizeof(Auth_Tx) );
         memcpy( db_buffer+Auth_Tx_Size, &Ftlf01, FTLF01_SIZE );
         length   = Auth_Tx_Size + FTLF01_SIZE;
         strcpy( db_table, "FTLF01" );
   }


      ret_val = build_and_send_request_message( MT_DB_REQUEST,
                                                ST1_DB_INSERT,
                                                0,
												fconds_que_name,
                                                db_buffer,
                                                length,
                                                data_type,
                                                NULL_STR,
                                                err_buf   );
      if ( ret_val == false )
      {
         memset( Buffer, 0x00, sizeof(Buffer) );
         sprintf( Buffer,
                 "Unable to insert record into %s.  Error = %s",
                  db_table, err_buf );
         falcntl_log_message( 1, 3, Buffer, "insert_db_record",1 );
      }


   return( ret_val );
}

INT Check_Rsp_Msg_for_approval(FAI_RSP_MSG *Rsp_msg)
{
	 if(0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_1, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_2, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_3, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_4, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_5, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_6, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_7, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_8, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_9, RT_FAIL_D, FT_FAIL_CODE_LEN) ||
		0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_10, RT_FAIL_D, FT_FAIL_CODE_LEN))
	 {
		 return false;
	 }

	 else if(0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_1, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_2, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_3, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_4, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_5, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_6, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_7, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_8, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_9, RT_FAIL_d, FT_FAIL_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_10, RT_FAIL_d, FT_FAIL_CODE_LEN))
	 {
		 return false;
	 }

	 else if(0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_1, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_2, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_3, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_4, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_5, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_6, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_7, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_8, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_9, RT_PASS_A, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_10, RT_PASS_A, FT_APPROVED_CODE_LEN))
	 {
		return true;
	 }
	 else if(0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_1, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_2, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_3, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_4, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_5, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_6, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_7, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_8, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
		 	 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_9, RT_PASS_a, FT_APPROVED_CODE_LEN) ||
			 0 == strncmp(Rsp_msg->Scr_Rsp_Dat.DECISION_CODE_10, RT_PASS_a, FT_APPROVED_CODE_LEN))
	 {
		 return true;
	 }
	 else
	 {
		 /*default case: if decision codes are empty We will approve dit*/
		 return true;
	 }
}

int falcntl_process_clear_timer_reply(pPTE_MSG p_msg_in)
{
	INT   	ret_val = 0;
	CHAR	date_str [ 9] = {0};
	CHAR	time_str [ 7] = {0};
	CHAR	time_date[25] = {0};
	CHAR	temp_time[11] = {0};
	CHAR  		   system_date[25] = {0};
	CHAR  		   yyyymmdd[25] = {0};
	CHAR		   temp_secs[11] = {0};
	pBYTE          p_data            = NULL_PTR;
	pPTE_MSG_DATA  p_msg_data        = NULL_PTR;
	pPTE_MSG       p_echo_msg        = NULL_PTR;
	LONG           timer_reply_size  = 0;
	LONG           data_len = 0;
	CHAR           app_data1[32]={0};
	CHAR           app_data2[32]={0};
	CHAR           lookup_key[LOOKUP_KEY_SIZE]="";
	FAI_RSP_MSG fai_response_message;
	CHAR java_response_indicator=0;
	CHAR err_buf[512]={0};
	CHAR 	temp_tran[21] = {0};
	 BYTE     subtype1=0;

	memset( &Auth_Tx, 0x00, Auth_Tx_Size );


	timer_reply_size = ptetimer_get_timer_reply( p_msg_in, lookup_key, app_data1,
													app_data2, (pBYTE)&Auth_Tx,
														(Auth_Tx_Size) );

	/* Get auth_tx record that was chained to the 'clear timer' message.
	* This is the auth_tx record built from response data from the
	* host.
	*/
	p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
	p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
	p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
	data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

	subtype1 = ptemsg_get_msg_subtype1(p_msg_in);


   if (ST1_TIMER_START == subtype1)
   {
	  /* do nothing */
	   falcntl_log_message(2,1,"ST1_TIMER_START ","falcntl_process_clear_timer_reply",0 );
	  return true;
   }

	if ( data_len != FAI_RSP_MSG_SIZE+1 )
	{
		sprintf( err_buf,
				"Did not receive chained host response data with timer reply. Key = %s",
				lookup_key );
		falcntl_log_message( 2, 2, err_buf, "falcntl_process_clear_timer_reply",0 );
		return false;
	}

	falcntl_log_message(2, 1, "Received response ftimer, Timer Cleared", "falcntl_process_clear_timer_reply", 1);
	/* Read FTLF01 Data from Timer reply message*/
	memset(&fai_response_message, 0x00, sizeof(fai_response_message));
	memset(fai_response_buffer,0x00,sizeof(fai_response_buffer));

	memcpy(&fai_response_buffer, p_data, sizeof(fai_response_buffer));
	memcpy(&fai_response_message, fai_response_buffer+1, sizeof(fai_response_message));

	java_response_indicator= fai_response_buffer[0];

	strncpy(temp_tran, fai_response_message.Scr_Rsp_Hdr.EH_EXT_HDR, 16);
	strcat(temp_tran, ".");
	strncat(temp_tran, &fai_response_message.Scr_Rsp_Hdr.EH_EXT_HDR[16], 3);
	strncpy(Ftlf01.tran_id, temp_tran, sizeof(Ftlf01.tran_id)-1);

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );
	get_date( time_date, date_str );
	get_time( time_date, time_str );
	strncpy(temp_time, &date_str[4], 4);/* MMDD */
	strncat(temp_time, time_str, 6);	/* hhmmss   */
	strncpy( Ftlf01.tran_resp_recv_time, temp_time, sizeof(Ftlf01.tran_resp_recv_time)-1);
	Ftlf01.tran_resp_recv_time[sizeof(Ftlf01.tran_resp_recv_time)-1] ='\0';
	/* copy decision codes */
	strncpy(Ftlf01.decision_code_1_value, fai_response_message.Scr_Rsp_Dat.DECISION_CODE_1, sizeof(Ftlf01.decision_code_1_value)-1);
	strncpy(Ftlf01.decision_code_2_value, fai_response_message.Scr_Rsp_Dat.DECISION_CODE_2, sizeof(Ftlf01.decision_code_2_value)-1);
	strncpy(Ftlf01.decision_code_3_value, fai_response_message.Scr_Rsp_Dat.DECISION_CODE_3, sizeof(Ftlf01.decision_code_3_value)-1);
	strncpy(Ftlf01.decision_code_4_value, fai_response_message.Scr_Rsp_Dat.DECISION_CODE_4, sizeof(Ftlf01.decision_code_4_value)-1);
	strncpy(Ftlf01.decision_code_5_value, fai_response_message.Scr_Rsp_Dat.DECISION_CODE_5, sizeof(Ftlf01.decision_code_5_value)-1);
	strncpy(Ftlf01.decision_code_6_value, fai_response_message.Scr_Rsp_Dat.DECISION_CODE_6, sizeof(Ftlf01.decision_code_6_value)-1);
	strncpy(Ftlf01.decision_code_7_value, fai_response_message.Scr_Rsp_Dat.DECISION_CODE_7, sizeof(Ftlf01.decision_code_7_value)-1);
	strncpy(Ftlf01.decision_code_8_value, fai_response_message.Scr_Rsp_Dat.DECISION_CODE_8, sizeof(Ftlf01.decision_code_8_value)-1);
	strncpy(Ftlf01.decision_code_9_value, fai_response_message.Scr_Rsp_Dat.DECISION_CODE_9, sizeof(Ftlf01.decision_code_9_value)-1);
	strncpy(Ftlf01.decision_code_10_value, fai_response_message.Scr_Rsp_Dat.DECISION_CODE_10, sizeof(Ftlf01.decision_code_10_value)-1);

	if (strlen(Auth_Tx.TLF01_details.system_date) < 8 || (validate_date_yyyymmdd(Auth_Tx.TLF01_details.system_date) == false))
	{
	   format_date(yyyymmdd);
	   strncpy(Auth_Tx.TLF01_details.system_date, yyyymmdd, sizeof(Auth_Tx.TLF01_details.system_date)-1);
	   strncpy(Ftlf01.sys_date,Auth_Tx.TLF01_details.system_date, sizeof(Ftlf01.sys_date)-1);
	   strncpy(Ftlf01.handler_queue,Auth_Tx.TLF01_details.handler_queue, sizeof(Ftlf01.handler_queue)-1);
	}
	else
	{
	   strncpy(Ftlf01.sys_date,Auth_Tx.TLF01_details.system_date, sizeof(Ftlf01.sys_date)-1);
	   strncpy(Ftlf01.handler_queue,Auth_Tx.TLF01_details.handler_queue, sizeof(Ftlf01.handler_queue)-1);
	}


	strncpy(Ftlf01.tran_req_sent_mode, "RT", 2);
	strncpy(temp_secs, &Auth_Tx.TLF01_details.date_yyyymmdd[4], 4);
	strncat(temp_secs, Auth_Tx.TLF01_details.time_hhmmss, 6);
	strncpy(Ftlf01.tran_req_sent_time, temp_secs, sizeof(Ftlf01.tran_req_sent_time)-1);
	Ftlf01.tran_req_sent_time[sizeof(Ftlf01.tran_req_sent_time)-1] ='\0';


	if (java_response_indicator == 'S')
	{
		falcntl_log_message(1,3,"Failed to write into ActiveMQ","falcntl_process_clear_timer_reply",1);
		strcpy(Ftlf01.response_text, "failed to write into ActiveMQ");
		strncpy(Ftlf01.tran_resp_recv_mode, "SF", 2);
		(void)insert_db_record( FTLF01_DATA );
		if ( false == send_msg_to_authorizer(MT_RT_RESPONSE_TIMEOUT) )
		{
			sprintf( err_buf,
					"Unable to complete processing of response msg. MsgType: %s, processing code: %s",
					Auth_Tx.TLF01_details.message_type,
					Auth_Tx.TLF01_details.processing_code);
			falcntl_log_message( 1, 3, err_buf, "falcntl_process_clear_timer_reply",1 );
		}

	}
	else
	{
		strncpy(Ftlf01.tran_resp_recv_mode, "RT", 2);

		strcpy (err_buf,"Real Time Response Mode") ;
		falcntl_log_message(2,1,err_buf,"falcntl_process_clear_timer_reply",1);

		/* validate the decision code in response( appr or decl) */
		ret_val = Check_Rsp_Msg_for_approval(&fai_response_message);
		if(true == ret_val)
		{
			strcpy(Ftlf01.response_text, "Approved response from FAI");
			(void)insert_db_record( FTLF01_DATA );
			if ( false == send_msg_to_authorizer(MT_RT_RESPONSE_APPROVED) )
			{
				sprintf( err_buf,
						"Unable to complete processing of response msg. MsgType: %s, processing code: %s",
						Auth_Tx.TLF01_details.message_type,
						Auth_Tx.TLF01_details.processing_code);
				falcntl_log_message( 1, 3, err_buf, "falcntl_process_clear_timer_reply",1 );
			}
		}
		else if (false == ret_val)
		{
			strcpy(Ftlf01.response_text, "Declined response from FAI");
			(void)insert_db_record( FTLF01_DATA );
			if ( false == send_msg_to_authorizer(MT_RT_RESPONSE_DECLINE) )
			{
				sprintf( err_buf,
						"Unable to complete processing of response msg. MsgType: %s, processing code: %s",
						Auth_Tx.TLF01_details.message_type,
						Auth_Tx.TLF01_details.processing_code);
				falcntl_log_message( 1, 3, err_buf, "falcntl_process_clear_timer_reply",1 );
			}
		}
	}
	return ret_val;
}

int falcntl_process_response_message(pPTE_MSG p_msg_in)
{
	INT   	ret_val = 0;
	pBYTE 			pATEMessageDataData = 0 ;
	pPTE_MSG_DATA 	pATEMessageData = 0 ;
	CHAR  			lookup_key[LOOKUP_KEY_SIZE] = "";
	CHAR logbuffer[512]={0};

	pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;
	memcpy(fai_response_buffer, pATEMessageDataData+sizeof(Auth_Tx), sizeof(FAI_RSP_MSG));

	memcpy(&fai_response_Rsp_msg,fai_response_buffer+1,sizeof(FAI_RSP_MSG));

	sprintf (logbuffer,"Received the response from falcon to Servicename:%s, pid:%d, tid:%d", ServiceName, getpid(), pthread_self() );
	falcntl_log_message(2,1,logbuffer,"falcntl_process_response_message",1);

	strncpy( lookup_key, fai_response_Rsp_msg.Scr_Rsp_Hdr.EH_EXT_HDR, 19);
	strncat(lookup_key, "F",1);
	falcntl_clear_request_timer(lookup_key);

	return ret_val;
}


int process_txcntl_request(pPTE_MSG p_msg_in)
{
	INT   			ret_val, ret_code;
	INT   			seconds, buf_temp_len = 0;
	CHAR  			err_buf[100] = "";
	CHAR 			msgBuffer [400] = {0} ;
	pBYTE 			pATEMessageDataData = 0 ;
	pPTE_MSG_DATA 	pATEMessageData = 0 ;
	CHAR 			key[100] = {0};
	CHAR  			lookup_key[LOOKUP_KEY_SIZE] = "";

	memset(&Auth_Tx, 0, sizeof(AUTH_TX));
	pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;
	memcpy(&Auth_Tx, (AUTH_TX*)pATEMessageDataData, sizeof(Auth_Tx));

	falcntl_log_message(2, 1, "Received transaction from txcntl", "process_txcntl_request", 1);
	/*First create the lookup timer key and send to ftimer to start timer
	 * Then send request to Falcon */

	/* Set the request timer. */
	(void)create_request_timer_key( lookup_key );

	//read t_secs from tf.ini
	seconds = t_secs;

	ret_val = set_timer( lookup_key, seconds, REQUEST_TIMER );
	if (0 < ret_val)
	{
		sprintf( err_buf, "Cannot process %s txn request to host. Unable to set timer.",
							Auth_Tx.TLF01_details.message_type );
		falcntl_log_message(2, 1, err_buf, "process_txcntl_request", 1);

		return -1;
	}

	//now send request to falcon
	send_request_to_falcon();

	return 0;
}


int send_request_to_falcon()
{
	pPTE_MSG p_msg_out;
	BYTE p_buffer[25000];
	char strTemp[256] = "";
	BYTE ReturnCode;

	memset( p_buffer, 0x00,     sizeof(p_buffer) );
	memcpy( p_buffer, &Auth_Tx, sizeof(AUTH_TX) );

	p_msg_out = ptemsg_build_msg( MT_AUTH_RT_REQUEST,
	                              ST1_NONE,
	                              ST2_NONE,
								  faisnd_que_name,
	                              falcntl_que_name,
								  (pBYTE) p_buffer,
								  sizeof(AUTH_TX),
	                              0 );

	if (p_msg_out == NULL)
	{
		sprintf( strTemp, "Mem allocate Err build msg for %s", faisnd_que_name );

		//send_authorizer_response(MT_RT_RESPONSE_DECLINE);

	    return -1;
	}
	else
	{

		ReturnCode = pteipc_send (p_msg_out, faisnd_que_name) ;
	    if (ReturnCode != 0)
	    {
	    	sprintf(strTemp, "Err %ld sending msg to %s", ReturnCode, faisnd_que_name);

	    	//send_authorizer_response(MT_RT_RESPONSE_DECLINE);

	        return -1;
	    }
	    falcntl_log_message(2, 1, "Sent transaction to Falcon", "send_request_to_falcon", 0);
	    free( p_msg_out );
	}

	return 0;
}
