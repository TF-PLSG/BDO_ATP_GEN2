#include "falcontroller.h"

void create_request_timer_key( pCHAR timer_key )
{
	CHAR   unique_tran_str_id[21] = "";
	BYTE   tx_key;
	BYTE   error_buffer[100] = {0};
	CHAR   transcn_id[21] = {0};
	CHAR   temp_mid[16]    = {0};
	CHAR   rrn[12] = {0};
	CHAR   temp_unique_tran[15] = "";
	INT    tid_len = 0;
	INT    mid_len = 0;
	INT    pan_len;
	INT    unique_tran_str_id_len=0;


	if (0 == strlen(Auth_Tx.TLF01_details.primary_key.transaction_id))
	{
		/* Create a unique transaction id for the new auth_tx record. */
		memset(unique_tran_str_id, 0x00, sizeof(unique_tran_str_id));
		ptetime_get_strtime(unique_tran_str_id);
		strncpy( Auth_Tx.TLF01_details.tran_start_time, unique_tran_str_id,16 );

		memset(Auth_Tx.TLF01_details.primary_key.transaction_id, 0x00,
		       sizeof(Auth_Tx.TLF01_details.primary_key.transaction_id));
		tid_len= strlen(Auth_Tx.TLF01_details.terminal_id);
		if(tid_len == 8)
		{
			memcpy(Auth_Tx.TLF01_details.primary_key.transaction_id,
		     				Auth_Tx.TLF01_details.terminal_id,
		             		sizeof(Auth_Tx.TLF01_details.terminal_id) - 1);
		    strncat(Auth_Tx.TLF01_details.primary_key.transaction_id,
		              		unique_tran_str_id+2, 12);
		}
	    else
		{
	    	/* We have to now prepare 20 digit unique Transaction ID.
		   	   unique_tran_str_id will be having 14 digit length
		   	   now we will make it as 20 by adding YYMMDD in the beginning of it.
		   	   20 byte will look like
		   	   YYMMDD"unique_tran_str_id"
		   	   for example if unique_tran_str_id is "1444752896.334" then it will become
		   	   YYMMDD1444752896.334
		   	   Based on ther terminal length we will copy data from new unique_tran_str_id
		   	*/
		   	strncpy(temp_unique_tran,unique_tran_str_id,14);
		   	memset(unique_tran_str_id,0x00,sizeof(unique_tran_str_id));

		   	strncpy(unique_tran_str_id,
		   			Auth_Tx.TLF01_details.date_yyyymmdd+2, 6); /* YYMMDD */
		   	strncat (unique_tran_str_id,temp_unique_tran,14);

		   	unique_tran_str_id_len= strlen(unique_tran_str_id);
		   	memcpy( Auth_Tx.TLF01_details.primary_key.transaction_id,
		   			Auth_Tx.TLF01_details.terminal_id, tid_len );
		   	strncat(Auth_Tx.TLF01_details.primary_key.transaction_id,
		   	  	    unique_tran_str_id+tid_len,
		   			(unique_tran_str_id_len-tid_len) );
		   	Auth_Tx.TLF01_details.primary_key.transaction_id[20]=0x00;
		}
	}

	strncpy( transcn_id, Auth_Tx.TLF01_details.primary_key.transaction_id, TRNID_LEN_BEFOREDECIMAL);
	strncpy(&transcn_id[TRNID_LEN_BEFOREDECIMAL], &Auth_Tx.TLF01_details.primary_key.transaction_id[TRNID_LEN_BEFOREDECIMAL + 1], TRNDID_AFTERDECIMAL );
	strncat(transcn_id, "F",1);
	memcpy( rrn, &Auth_Tx.TLF01_details.retrieval_ref_num, 12);

	strcpy( timer_key, transcn_id );


/*testing code for Hostgen load
	memcpy(Auth_Tx.TLF01_details.primary_key.transaction_id+0,Auth_Tx.TLF01_details.sys_trace_audit_num,sizeof(Auth_Tx.TLF01_details.sys_trace_audit_num) - 1);

	strncpy( transcn_id, Auth_Tx.TLF01_details.primary_key.transaction_id, TRNID_LEN_BEFOREDECIMAL);
	strncpy(&transcn_id[TRNID_LEN_BEFOREDECIMAL], &Auth_Tx.TLF01_details.primary_key.transaction_id[TRNID_LEN_BEFOREDECIMAL + 1], TRNDID_AFTERDECIMAL );
	strncat(transcn_id, "F",1);
	strcpy( timer_key, transcn_id );
	*/
	return;
}

INT set_timer( pCHAR lookup_key, INT timer_length, INT type_of_timer )
{
	pPTE_MSG p_msg_set_timer = NULL_PTR;
	BYTE     source_que[20]  = "";
	BYTE     buffer1[AUTH_TX_SIZE];
	CHAR     err_buf[100]="", Buff[120] = "";
	INT      ret_val = false;

	/* Send the whole transaction record. */
	memcpy( buffer1, &Auth_Tx, Auth_Tx_Size );

	/* Set up the queue name for the reply from ftimer. */
	strcpy( source_que, falcntl_que_name);

	sprintf(err_buf,"lookupkey while sending to ftimer : %s",lookup_key);
	falcntl_log_message( 2, 1, err_buf, "set_timer", 1 );

	/* Determine if this is a timer for a transaction to the host or not. */
	if ( type_of_timer == REQUEST_TIMER )
	{
		/* Build PTE message that will start the timer. */

		/* This type of timer stores Auth_Tx with the timer. */
		p_msg_set_timer = ptetimer_bld_start_timer_req(
                            lookup_key,
                            Auth_Tx.TLF01_details.primary_key.transaction_id,
                            TimerAppData2,
                            ftimer_que_name,
                            source_que,
                            (WORD)timer_length,
                            0,
                            (pBYTE)&buffer1,
                            Auth_Tx_Size );
	}
	else /* Echo Timer */
	{
		/* This type of timer does not stores Auth_Tx with the timer. */
		p_msg_set_timer = ptetimer_bld_start_timer_req(
                            lookup_key,
                            NULL,
                            NULL,
                            ftimer_que_name,
                            source_que,
                            (WORD)timer_length,
                            0,
                            NULL_PTR,
                            0 );
	}

	if ( NULL_PTR == p_msg_set_timer )
	{
		strcpy( err_buf, "Insufficient Memory to build timer message" );
	}
	else
	{
		ret_val = pteipc_send(p_msg_set_timer, ftimer_que_name);
		if(ret_val < 0)
		{
            pteipc_get_errormsg( ret_val, err_buf );
            sprintf( Buff, "Failed transmit to que: %s. %s", ftimer_que_name, err_buf );
            falcntl_log_message( 2, 2, Buff, "set_timer", 0 );
		}
		free( p_msg_set_timer );
	}

	return( ret_val );
}

INT falcntl_clear_request_timer( pCHAR lookup_key )
{
   pPTE_MSG   p_timer_msg   = NULL_PTR;
   pPTE_MSG   p_chain_msg   = NULL_PTR;
   INT        ret_val       = false;
   CHAR       source_que[30]= {0};
   CHAR       err_buf[100]  = {0};

   sprintf(err_buf, "lookup key : %s, pid = %d, tid = %d", lookup_key, getpid(), pthread_self());


   /* Build PTEMSG to clear timer */
   p_timer_msg = ptetimer_bld_clear_timer_req( lookup_key,
                                               ftimer_que_name,
											   application_que_name);
   if ( NULL_PTR == p_timer_msg )
   {
      strcpy( err_buf, "Insufficient Memory to build clear timer message" );
   }
   else
   {
      /* Build a message to preserve our auth_tx record.
       * It gets chained (appended) to the timer record.
       */
      p_chain_msg = ptemsg_chain_new_message( p_timer_msg,
    		  	  	  	  	  	  	  	  	  MT_DB_REQUEST,
											  ST1_DB_ECHO,
											  ST2_NONE,
                                              ftimer_que_name,
                                              application_que_name,
                                              (pBYTE)(&fai_response_buffer),
											  sizeof(fai_response_buffer),
                                              0 );
      if ( NULL_PTR == p_chain_msg )
      {
         sprintf( err_buf, "Insufficient memory to chain clear timer message" );
         free( p_timer_msg );
      }
      else
      {
         /* Send clear timer request with chained data to data server.
          * Processing of this transaction will resume in function
          * process_clear_timer_reply when we receive the
          * response from timerds to our clear timer request.
          */
         ret_val = pteipc_send( p_chain_msg, ftimer_que_name );
         free( p_chain_msg );
      }
   }

   if ( false == ret_val )
	   falcntl_log_message( 2, 2, err_buf, "falcntl_clear_request_timer",0 );

   falcntl_log_message(2, 1, "Sent for timer clear to ftimer", "falcntl_clear_request_timer", 1);
   return( ret_val );
}

