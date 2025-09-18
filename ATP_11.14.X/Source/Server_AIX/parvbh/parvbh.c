/******************************************************************************
 *
 *  Name			:       parvbh.c
 *
 *  Version			:		ATP_11.10.1
 *
 *  Service Name	: 		parvbh
 *
 *  Parameters		:	  	None
 *
 *  DB Tables used	:  		BCH20, TLF01, TLF01_STIP
 *
 *  Config. file	:  		tf.ini
 *
 *  Description		:  		1. Reads the BCH20 table based on card type which are not processed PreAuths.
 *  			  	 		2. Checks the tran date and picks the Preauth records which falls beyond 30 days.
 *  			  		    3. Get the TLF01 records and generate the Reversal for Preauth and send to Host.
 *
 *  Scheduler		:		Cron Jobs
 *  						Example:-
 *  						Enter 'crontab -e' command, add this below line & save it.
 *  						0 2 * * * . $HOME/.profile; $HOME/ascendent/bin/parvbh
 *  						This utility will execute everyday @2AM.
 *  						Including .profile to get access to the environment variables.
 *  						Cron jobs can be seen using this command, 'crontab -l'
 *
 *  Author			:       Amith Karunakar
 *
 ******************************************************************************/

/*
 * Rev 1.0 June 11, 2024
 * Rev version: ATP_11.10.1 by Amith
 * BNDL-3: Changes to ATP handling of pre-auth and completion handling across
 * all brands including MC Global 512 (as per discussion with CBG Merchant
 * Partnerships on February 7, 2024)
 * BNDL-4: Automatic Reversal of Not Completed/Reversed Pre-Authorizations
 * (as per discussion with CBG Merchant Partnerships on February 7, 2024)
 * */

#include "parvbh.h"

CHAR   Version[] = "ATP_11.12.0";
int    tf_debug;
CHAR   sys_current_date[9];
CHAR   CUP_merchant_indicator[8]={0};

AUTH_TX Auth_Tx;
TLF01	tlf01_tmp_struct, db_tlf01_struct;
BOOLEAN atp_stip_processed = false;
int  MaxRetryCount = 0;
INT  DB_NullCheck_Flag = 0;
UINT Max_File_Size_Defined = 0 ;

CHAR maximum_validity_of_preauth_in_days_for_MC[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_VISA[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_AMEX[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_DCI[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_JCB[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_UPI[3]={0};

CHAR smcc_maximum_validity_of_preauth_in_days_for_MC[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_VISA[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_AMEX[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_DCI[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_JCB[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_UPI[3]={0};


#define  DEFAULT_REPEAT_COUNT 3

#define MAX_CARD_BRANDS 6
#define MC_CARD 		1
#define VSA_CARD 		2
#define JCB_CARD 		3
#define DIN_CARD 		4
#define AMX_CARD 		5
#define CUP_CARD		6

CHAR mc_onus_link[8] 		= "ncmcr2";
CHAR visa_onus_link[8] 		= "ncvis2";
CHAR jcb_onus_link[8] 		= "ncjcb2";
CHAR diners_onus_link[8]	= "ncdci2";
CHAR amex_onus_link[8] 		= "nposa2";
CHAR cup_onus_link[8] 		= "nccup2";

CHAR mc_offus_link[8] 		= "ncmcrd";
CHAR visa_offus_link[8] 	= "ncvisa";
CHAR jcb_offus_link[7] 		= "ncjcb";
CHAR diners_offus_link[7]	= "ncdci";
CHAR amex_offus_link[8] 	= "ncposa";
CHAR cup_offus_link[7] 	    = "nccup";

void main(int argc, char *argv[])
{
	CHAR     Buffer[256];
	CHAR     strError[512] = {0} ;
	CHAR     time_date[25] = {0};
	CHAR 	 sys_date[25]  = {0};
	INT 	 ret_val = 0;

	if(argc > 1 && strcmp(argv[1], "debug") == 0)
		tf_debug = 1;

	if(strstr(argv[0], "parvbh") != NULL)
	{
		strcpy(ExeName, "parvbh");
	}

	GetAppName( AppName );
    strcpy(ServiceName, AppName);

	if (MainProcessor() != 0)
	{
		printf("Error Running parvbh: send non-processed preauth auto reversal to host program \n\n");
	}

	Stop();

    return;
}

/****************************************PREAUTH REVERSALS FUNCTIONS - START***************************************/
int MainProcessor()
{
	CHAR     msg[256] = {0} ;
	CHAR     strTemp [256] = {0} ;
	CHAR 	 time_date[25] = {0};
	INT      ret_val = 0;
	CHAR     Buffer[256] = {0};
	CHAR 	 sys_date[25]  = {0};
	//new variable to get lookup days
	int	     lookup_days;

	memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
    get_max_retry_count();

	#ifdef _DEBUG
	pteipc_trace_on();
	#endif

	/* Create queues sbatchA, sbatchI, sbatchC */
	if( !pteipc_init_single_instance_app( AppName, "pte_ipc" ) )
	{
		sprintf (strTemp,
		  "%s: MainProcessor: Failed to Create XIPC queues",
		   ServiceName) ;
		parvbh_log_message( 2, 3, strTemp, "MainProcessor");
		/* signal that we are ready to shut down and quit */
		pteipc_shutdown_single_instance_app() ;
		MainProcessDone = 1 ;
		return 1;
	}

	ret_val = get_dataserver_ini_db_error_statics_value( &db_error_statics_flag,
														 &db_error_statics_value,
	                                                     ServiceName,
														 Buffer );

	sprintf(strTemp, "Started the %s Service, Version : %s", ServiceName, Version);
	parvbh_log_message( 3, 1, strTemp, "MainProcessor");

	/* Try to connect to the database */
	if (dbcommon_connect("equitable", "equitable", "equitable", "ORCL", msg) != PTEMSG_OK)
	{
		strcpy(msg, "Unable to Connect to Oracle ");
		parvbh_log_message( 2, 3, msg, "MainProcessor");
		shut_down();
		return 1;
	}

	mchutil_read_CUP_merchant_indicator();
	ptetime_get_timestamp( time_date );
	get_date( time_date, sys_date); // Reformat to get date "YYYYMMDD"
	memset(sys_current_date, 0, sizeof(sys_current_date));
	memcpy(sys_current_date, sys_date, strlen(sys_date));

	/* Initialize structure used to build TLF01 detail */
	memset(&Auth_Tx, 0, sizeof(Auth_Tx));
	automatic_preauth_reversal_checks();

	shut_down();
	return 0;
}

void shut_down(void)
{
   CHAR Buffer[100] = "";

   /* Added to fix a timing problem */
   sleep(2);

   /* Added to fix Equitable error report #852 */
   sprintf( Buffer,"Stopping parvbh program, version %s",Version);
   display_error_message( Buffer, "MainProcessor");

   pteipc_shutdown_single_instance_app();
   MainProcessDone = 1;

   return;

} /* shut_down */

void automatic_preauth_reversal_checks()
{
	int  ResultCode = 0, preauthCount = 0, rc;
	char errorMsg[100] = {0};
	BCH20 bch20 = {0};
	int next_portion = 0;
	char msg[256], cardLookupDate[9], cardBrandFlag[5];
	char LookupDate[9];
	int index = 0;
	int processedCount = 0;

	parvbh_log_message(2, 1, "----------------All Card Brands : PREAUTH_AUTOMATIC_REVERSAL_CHECKS----------------", "");
    get_maximum_validity_of_preauth_in_days();
    get_maximum_validity_of_preauth_in_days_for_smcc();

    /* Process the preauth reversal based on each Card brands */
    for(index = MC_CARD; index <= MAX_CARD_BRANDS; index++)
	{
		next_portion = 0;
    	preauthCount = 0;
		processedCount = 0;
    	memset(cardLookupDate, 	  0, sizeof(cardLookupDate));
    	memset(cardBrandFlag, 	  0, sizeof(cardBrandFlag));

    	if(index == MC_CARD)
    	{
    		strcpy(cardBrandFlag, "MC");
    		get_beyond_start_date(&cardLookupDate, atoi(maximum_validity_of_preauth_in_days_for_MC));
    	}
    	else if(index == VSA_CARD)
    	{
    		strcpy(cardBrandFlag, "VISA");
    		get_beyond_start_date(&cardLookupDate, atoi(maximum_validity_of_preauth_in_days_for_VISA));
    	}
    	else if(index == JCB_CARD)
    	{
    		strcpy(cardBrandFlag, "JCB");
    		get_beyond_start_date(&cardLookupDate, atoi(maximum_validity_of_preauth_in_days_for_JCB));
    	}
    	else if(index == DIN_CARD)
    	{
    		strcpy(cardBrandFlag, "DCI");
    		get_beyond_start_date(&cardLookupDate, atoi(maximum_validity_of_preauth_in_days_for_DCI));
    	}
    	else if(index == AMX_CARD)
    	{
    		strcpy(cardBrandFlag, "AMEX");
    		get_beyond_start_date(&cardLookupDate, atoi(maximum_validity_of_preauth_in_days_for_AMEX));
    	}
    	else if(index == CUP_CARD)
    	{
    		strcpy(cardBrandFlag, "CUP");
    		get_beyond_start_date(&cardLookupDate, atoi(maximum_validity_of_preauth_in_days_for_UPI));
    	}

		sprintf(msg, "----------------Process started for %s Card Brand----------------", cardBrandFlag);
		parvbh_log_message(2, 1, msg, "automatic_preauth_reversal_checks");

    	preauthCount = db_select_non_processed_preauths_count(cardLookupDate, cardBrandFlag);
    	if(preauthCount == -1)
    	{
    		continue;
    	}
    	else if(preauthCount == 0)
		{
			sprintf(msg, "Unprocessed preauths are not found in BCH20 table for %s Card Brand", cardBrandFlag);
			parvbh_log_message(2, 1, msg, "automatic_preauth_reversal_checks");
			continue;
		}
		else if(preauthCount > 0)
		{
			sprintf(msg, "Total unprocessed preauths in BCH20 table count:%d qualified for sending Automatic Reversals for %s Card Brand",
					preauthCount, cardBrandFlag);
			parvbh_log_message(2, 1, msg, "automatic_preauth_reversal_checks");
		}

		while(next_portion == 0)
		{
			memset(&bch20, 0x00, sizeof(bch20));
			ResultCode = db_select_non_processed_preauth(cardLookupDate, cardBrandFlag, &bch20);
			if(ResultCode == PTEMSG_OK)
			{
				if(strcmp(bch20.product_codes[1].code, STANDIN) == 0)
				{
					atp_stip_processed = true;
				}
				else
				{
					atp_stip_processed = false;
				}
				rc = ProcessTransactions(&bch20);
				if(rc != PTEMSG_OK && rc != PTEMSG_NOT_FOUND && rc != PTEMSG_COULD_NOT_FORWARD)
				{
					next_portion = 1;
				}
				else
				{
					processedCount++;
					if(processedCount == preauthCount)
					{
						next_portion = 1;
						sprintf(msg, "Total [%d] Preauth Automatic Reversals sent for %s Card Brand",
								processedCount, cardBrandFlag);
						parvbh_log_message(2, 1, msg, "automatic_preauth_reversal_checks");
						processedCount = 0;
					}
				}
			}
			else if(ResultCode == PTEMSG_NOT_FOUND)
			{
				parvbh_log_message(2, 1, "No more non-processed preauths are not found from BCH20 table",
										 " automatic_preauth_reversal_checks");
				next_portion = 1;
			}
			else if( NOT_CONNECTED_TO_DB 			 == ResultCode ||
					 NOT_LOGGED_INTO_DB 			 == ResultCode ||
					 TNS_PACKET_WRITER_FAILED 		 == ResultCode ||
					 SHUTDOWN_OR_STARTUP_IN_PROGRESS == ResultCode ||
					 PTEMSG_NOT_CONNECTED_TO_DB		 == ResultCode )
			{
				dbcommon_disconnect(msg);
				if (dbcommon_connect("equitable", "equitable", "equitable", "ORCL", msg) != PTEMSG_OK)
				{
					strcat (msg, " DB Reconnection failed");
					parvbh_log_message(2, 3, msg, "automatic_preauth_reversal_checks");
					next_portion = 1;
					shut_down();
					Stop();
				}
				else
				{
					parvbh_log_message(2, 1, "DataBase Reconnected", "automatic_preauth_reversal_checks");
				}
			}
		}
	}

    /***************************SMCCC Portion Begins********************/
    for(index = MC_CARD; index <= MAX_CARD_BRANDS; index++)
	{
		next_portion = 0;
    	preauthCount = 0;
		processedCount = 0;
    	memset(cardLookupDate, 	  0, sizeof(cardLookupDate));
    	memset(cardBrandFlag, 	  0, sizeof(cardBrandFlag));

    	if(index == MC_CARD)
    	{
    		strcpy(cardBrandFlag, "MC");
    		get_beyond_start_date(&cardLookupDate, atoi(smcc_maximum_validity_of_preauth_in_days_for_MC));
    	}
    	else if(index == VSA_CARD)
    	{
    		strcpy(cardBrandFlag, "VISA");
    		get_beyond_start_date(&cardLookupDate, atoi(smcc_maximum_validity_of_preauth_in_days_for_VISA));
    	}
    	else if(index == JCB_CARD)
    	{
    		strcpy(cardBrandFlag, "JCB");
    		get_beyond_start_date(&cardLookupDate, atoi(smcc_maximum_validity_of_preauth_in_days_for_JCB));
    	}
    	else if(index == DIN_CARD)
    	{
    		strcpy(cardBrandFlag, "DCI");
    		get_beyond_start_date(&cardLookupDate, atoi(smcc_maximum_validity_of_preauth_in_days_for_DCI));
    	}
    	else if(index == AMX_CARD)
    	{
    		strcpy(cardBrandFlag, "AMEX");
    		get_beyond_start_date(&cardLookupDate, atoi(smcc_maximum_validity_of_preauth_in_days_for_AMEX));
    	}
    	else if(index == CUP_CARD)
    	{
    		strcpy(cardBrandFlag, "CUP");
    		get_beyond_start_date(&cardLookupDate, atoi(smcc_maximum_validity_of_preauth_in_days_for_UPI));
    	}

		sprintf(msg, "----------------Process started for %s Card Brand with SMCC----------------", cardBrandFlag);
		parvbh_log_message(2, 1, msg, "automatic_preauth_reversal_checks");

    	preauthCount = db_select_non_processed_preauths_count_smcc(cardLookupDate, cardBrandFlag);
    	if(preauthCount == -1)
    	{
    		continue;
    	}
    	else if(preauthCount == 0)
		{
			sprintf(msg, "Unprocessed preauths are not found in BCH20 table for %s Card Brand With SMCC", cardBrandFlag);
			parvbh_log_message(2, 1, msg, "automatic_preauth_reversal_checks");
			continue;
		}
		else if(preauthCount > 0)
		{
			sprintf(msg, "Total unprocessed preauths in BCH20 table count:%d qualified for sending Automatic Reversals for %s Card Brand With SMCC",
					preauthCount, cardBrandFlag);
			parvbh_log_message(2, 1, msg, "automatic_preauth_reversal_checks");
		}

		while(next_portion == 0)
		{
			memset(&bch20, 0x00, sizeof(bch20));
			ResultCode = db_select_non_processed_preauth_smcc(cardLookupDate, cardBrandFlag, &bch20);
			if(ResultCode == PTEMSG_OK)
			{
				if(strcmp(bch20.product_codes[1].code, STANDIN) == 0)
				{
					atp_stip_processed = true;
				}
				else
				{
					atp_stip_processed = false;
				}
				rc = ProcessTransactions(&bch20);
				if(rc != PTEMSG_OK && rc != PTEMSG_NOT_FOUND && rc != PTEMSG_COULD_NOT_FORWARD)
				{
					next_portion = 1;
				}
				else
				{
					processedCount++;
					if(processedCount == preauthCount)
					{
						next_portion = 1;
						sprintf(msg, "Total [%d] Preauth Automatic Reversals sent for %s Card Brand",
								processedCount, cardBrandFlag);
						parvbh_log_message(2, 1, msg, "automatic_preauth_reversal_checks");
						processedCount = 0;
					}
				}
			}
			else if(ResultCode == PTEMSG_NOT_FOUND)
			{
				parvbh_log_message(2, 1, "No more non-processed preauths are not found from BCH20 table",
										 " automatic_preauth_reversal_checks");
				next_portion = 1;
			}
			else if( NOT_CONNECTED_TO_DB 			 == ResultCode ||
					 NOT_LOGGED_INTO_DB 			 == ResultCode ||
					 TNS_PACKET_WRITER_FAILED 		 == ResultCode ||
					 SHUTDOWN_OR_STARTUP_IN_PROGRESS == ResultCode ||
					 PTEMSG_NOT_CONNECTED_TO_DB		 == ResultCode )
			{
				dbcommon_disconnect(msg);
				if (dbcommon_connect("equitable", "equitable", "equitable", "ORCL", msg) != PTEMSG_OK)
				{
					strcat (msg, " DB Reconnection failed");
					parvbh_log_message(2, 3, msg, "automatic_preauth_reversal_checks");
					next_portion = 1;
					shut_down();
					Stop();
				}
				else
				{
					parvbh_log_message(2, 1, "DataBase Reconnected", "automatic_preauth_reversal_checks");
				}
			}
		}
	}

    /***************************SMCCC Portion Ends********************/
}

int ProcessTransactions(pBCH20 bch20)
{
	char           msg[256];
	char		   strBuffer[256];
	BYTE           rc;
	pPTE_MSG       p_msg = 0 ;
	int  		   dest_len=0;
	LONG           lReturnCode = 0L ;
	INT            err_ctr = 0;
	CHAR           strTemp [256] = {0} ;
	BYTE           tranid[21];
    EMV_RAW        emvraw;
    PAD_RAW        pad_raw_data;
	BYTE		   emv_result_code;
	BYTE           padraw_result_code;
	int 		   temp_len = 0;
	char 		   str_temp_len [4] = {0};
	BYTE 		   bstring[12];
	BYTE 		   ReturnCode = true;
	INT			   retryCount = 0;
	char    	   NetCntlName[12] = {0};
	char		   authorizing_host_queue[9] = {0};
	char		   tran_id[21] = {0};
	NCF01 		   ncf01_tmp_struct;

	memset(&tlf01_tmp_struct,0,sizeof(TLF01));
	memset(&ncf01_tmp_struct,0,sizeof(NCF01));
	usleep (1000000);// 1 sec delay

	memcpy(tran_id, bch20->transaction_id, sizeof(tran_id));
	rc = db_get_tlf01_rec(tran_id, &tlf01_tmp_struct, msg);

	if(rc == PTEMSG_OK)
	{
		memset(&Auth_Tx, 0, sizeof(Auth_Tx));
		memcpy (&Auth_Tx.TLF01_details, &tlf01_tmp_struct, sizeof (TLF01)) ;

		if(strlen(Auth_Tx.TLF01_details.acquirer_id) == 0)
		{
			if(get_network_name())
			{
				sprintf(msg,"PARVBH: Unable to get the network name for %s card brand", bch20->product_codes[4].code);
				parvbh_log_message( 2, 3, msg, "ProcessTransactions");
				return PTEMSG_COULD_NOT_FORWARD;
			}
		}

		strcpy(NetCntlName, Auth_Tx.TLF01_details.acquirer_id);
		if(NetCntlName[strlen(NetCntlName)-1] == '2')
		{
			sprintf(msg, "Automatic Preauth reversal is not supported for %s", NetCntlName);
			parvbh_log_message( 2, 2, msg, "ProcessTransactions");
			return PTEMSG_COULD_NOT_FORWARD;
		}
		strcpy(authorizing_host_queue, Auth_Tx.TLF01_details.acquirer_id);//copy based on card brands, if empty
		strcat(authorizing_host_queue, "A");

		rc = db_select_ncf01(NetCntlName, &ncf01_tmp_struct, msg);
		if ( rc == PTEMSG_OK)
		{
			advice_repeat_ctr = atoi(ncf01_tmp_struct.advice_repeat_counter);
			if (advice_repeat_ctr == 0)
			{
				advice_repeat_ctr = DEFAULT_REPEAT_COUNT;
			}
			printf("Obtained advice_repeat_counter from NCF01[%s]: %d\n", NetCntlName, advice_repeat_ctr);
		}
		else
		{
			sprintf(msg, "The Oracle error number to get NCF01[%s] advice_repeat_counter %d::%s \n", NetCntlName, dbcommon_oracle_to_pte_rcode( rc, msg), msg);
			parvbh_log_message( 2, 2, msg, "ProcessTransactions");
			sprintf(msg, "Setting the advice_repeat_counter to 3\n");
			parvbh_log_message( 2, 2, msg, "ProcessTransactions");
			advice_repeat_ctr = DEFAULT_REPEAT_COUNT;
		}

		//Check for host status
		if(Check_Is_Host_Up(NetCntlName) == false)
		{
			sprintf(msg,"PARVBH: NC is not ONLINE, Plz make network ONLINE : Network controller - %s",NetCntlName);
			parvbh_log_message( 2, 3, msg, "ProcessTransactions");
			return PTEMSG_INVALID_TRID;
		}

		/* total preauth amount */
		if(bch20->product_codes[4].amount[0] != '\0')
		{
			memcpy(Auth_Tx.TLF01_details.total_amount,bch20->product_codes[4].amount,12);
			memcpy(Auth_Tx.TLF01_details.reversal_amount,bch20->product_codes[4].amount,12);
		}
		else
		{
			memcpy(Auth_Tx.TLF01_details.reversal_amount,Auth_Tx.TLF01_details.reversal_amount,12);
		}
		strcpy(Auth_Tx.TLF01_details.orig_message,Auth_Tx.TLF01_details.message_type);
		strcpy(Auth_Tx.TLF01_details.orig_retrieval_ref_num, Auth_Tx.TLF01_details.retrieval_ref_num);
		strcpy(Auth_Tx.TLF01_details.retrieval_ref_num,"");
		memcpy(Auth_Tx.TLF01_details.orig_sys_trace_num,Auth_Tx.TLF01_details.sys_trace_audit_num,6);
		strcpy (Auth_Tx.orig_local_time_date, Auth_Tx.TLF01_details.transmission_timestamp);
		Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL;

		if(true == mchutil_check_If_MC_Transaction() || true == mchutil_check_If_VISA_Transaction())
		{
			strcpy(Auth_Tx.TLF01_details.message_type,"0400");
		}
		else if(true == mchutil_check_If_AMEX_Transaction() || true == mchutil_check_If_Diners_Transaction())
		{
			strcpy(Auth_Tx.TLF01_details.message_type,"1420");
		}
		else
		{
			strcpy(Auth_Tx.TLF01_details.message_type,"0420");
		}

		strcpy(Auth_Tx.TLF01_details.originator_queue,application_que_name);
		strcpy(Auth_Tx.TLF01_details.handler_queue, application_que_name);
		strcpy(Auth_Tx.TLF01_details.authorizing_host_queue, authorizing_host_queue);

		/* Copy indutry code*/
		strcpy(Auth_Tx.industry_code, Auth_Tx.TLF01_details.product_codes[8].code);

		/* If txn from DCPSIO or voice no need to send fld 6 and 51. */
		strcpy(Auth_Tx.ch_billing_curr_code,"");
		strcpy(Auth_Tx.ch_billing_amt,"");

		dest_len = strlen(authorizing_host_queue);
		if (authorizing_host_queue[dest_len - 1] == 'A'){}
		else
		{
			strcat(authorizing_host_queue,"A");
		}
		/* Get EMV data */
		if ( 0 == strncmp("005", Auth_Tx.TLF01_details.pos_entry_mode, 3) ||
			 0 == strncmp("007", Auth_Tx.TLF01_details.pos_entry_mode, 3))
		{
			/* Fetch any EMV data associated with this txn. */
			memset( &emvraw, 0x00, sizeof(EMV_RAW) );
			memset( tranid, 0x00, sizeof(tranid) );
			memcpy( tranid, Auth_Tx.TLF01_details.primary_key.transaction_id, sizeof(tranid));
			emv_result_code = db_select_emvraw(tranid, &emvraw, strBuffer);
			if ( emv_result_code == PTEMSG_OK )
			{
				memcpy(Auth_Tx.EMV_details.emv_block,
					   emvraw.RAW_DATA,
					   atoi(emvraw.DATA_LENGTH) );
			}
			else
			{
				/* Orig txn has been found, so even if
				 * EMV data is not found, still return success.
				 */
				emv_result_code = PTEMSG_OK;
			}
		}
		/* End of EMV data */

		/* Validate amount*/
		if(strlen(Auth_Tx.TLF01_details.total_amount) < 12)
		{
			//Some thing wrong with conversion Make sure length will be 12
			INT i = atoi(Auth_Tx.TLF01_details.total_amount);
			sprintf(Auth_Tx.TLF01_details.total_amount,"%012d",i);
		}

		// Lets check for VISA DE56 value.
		if(0 == strncmp(Auth_Tx.TLF01_details.product_codes[0].quantity,"56",2) &&
		   ('4' == Auth_Tx.TLF01_details.card_num[0]))
		{
			padraw_result_code = db_select_padraw(tranid, &pad_raw_data, strBuffer);
			temp_len = atoi(pad_raw_data.DATA_LENGTH);
			Auth_Tx.future_use_1[0] =  temp_len ;
			memcpy( Auth_Tx.future_use_1 + 1,pad_raw_data.RAW_DATA,temp_len );
		}
		err_ctr = 0;retryCount = 0;
		sprintf(msg, "Processing Preauth Reversal transaction for %s with tlf01 ID: %s, Date_yyyymmdd : %s",
				NetCntlName, Auth_Tx.TLF01_details.primary_key.transaction_id, Auth_Tx.TLF01_details.date_yyyymmdd);
		parvbh_log_message( 2, 1, msg, "ProcessTransactions");

		SEND_REQUEST_AGAIN:
		lReturnCode = Send_Request_To_Service ( authorizing_host_queue,
												MT_AUTH_REQUEST,
												0,
												0,
												0,
												(pBYTE) (&Auth_Tx),
												sizeof(AUTH_TX)) ;

		if(lReturnCode != DEVICE_OK)
		{
			retryCount++;
			sprintf(msg, "Failed to send request to %s for : %s\n",
					authorizing_host_queue,
					Auth_Tx.TLF01_details.primary_key.transaction_id);
			parvbh_log_message( 2, 3, msg, "ProcessTransactions");

			if(retryCount > MaxRetryCount)
			{
				sprintf( msg, "Reached maximum retry count %d for Tran_ID: %s, processing next preauth",
						 retryCount, Auth_Tx.TLF01_details.primary_key.transaction_id);
				display_error_message(msg, "ProcessTransactions");
				return PTEMSG_GUI_TIMEDOUT;
			}
			goto SEND_REQUEST_AGAIN;
		}

		if( reponseWaitFlag == 0)
		{
			//don't wait for the response from Vertexon/Schemes, send and update the batch table, process next transaction
			if(update_atp_batch_tables() == PTEMSG_OK)
			{
				sprintf( msg, "ATP Batch tables updated for Tran_ID : %s, Network : %s\n", Auth_Tx.TLF01_details.primary_key.transaction_id, NetCntlName);
			}
			else
			{
				sprintf( msg, "Failed to update ATP Batch tables for Tran_ID : %s, Network : %s\n", Auth_Tx.TLF01_details.primary_key.transaction_id, NetCntlName );
			}
			parvbh_log_message( 2, 1, msg, "ProcessTransactions");
		}
		else
		{
			while (err_ctr <= advice_repeat_ctr )
			{
				/* You are blocked here waiting for a message on either app queue or
				* control que.  If there is no message on either que for 5 seconds,
				* the blocking call returns.  Application_que_name and control_que_name
				* are populated in pteipc_init_multiple_instance_app (pteipc.c).
				*/
				p_msg = pteipc_receive( application_que_name, control_que_name, 20, &lReturnCode);

				if(p_msg != NULL_PTR)
				{
					err_ctr = 0;
					PinnacleMsgHandler (p_msg);
					total_tx_num++;
					free(p_msg);
					return PTEMSG_OK;
				}
				else if(lReturnCode == QUE_ER_TIMEOUT)
				{
					/* Queue has timed out. There is nothing in it. */
					/*increment the retry count and try sending repeat message now*/
					err_ctr++;

					/*Amex host does not identifys repeat msg. So send same msg even though its repeat. TF-Ajay 28-Jul-09*/
					if(true == mchutil_check_If_AMEX_Transaction())
					{
						/* Don't change the message type */
					}
					if(true == mchutil_check_If_MC_Transaction() || true == mchutil_check_If_VISA_Transaction())
					{
						strcpy(Auth_Tx.TLF01_details.message_type,"0420");
					}
					else if(true == mchutil_check_If_Diners_Transaction())
					{
						strcpy(Auth_Tx.TLF01_details.message_type,"1421");
					}
					else
					{
						strcpy(Auth_Tx.TLF01_details.message_type,"0421");
					}

					retryCount = 0;
					sprintf(msg, "Processing Preauth Reversal Repeat transaction for %s with tlf01 ID: %s, Date_yyyymmdd : %s",
							NetCntlName, Auth_Tx.TLF01_details.primary_key.transaction_id, Auth_Tx.TLF01_details.date_yyyymmdd);
					parvbh_log_message( 2, 1, msg, "ProcessTransactions");

					SEND_REPEAT_REQUEST_AGAIN:
					lReturnCode = Send_Request_To_Service (  authorizing_host_queue,
															 MT_AUTH_REQUEST,
															 0,
															 0,
															 0,
															 (pBYTE) (&Auth_Tx),
															 sizeof(AUTH_TX));

					if(lReturnCode != DEVICE_OK)
					{
						retryCount++;
						printf("Failed to send repeat request to %s for : %s\n",
								authorizing_host_queue,
								Auth_Tx.TLF01_details.primary_key.transaction_id);

						if(retryCount > MaxRetryCount)
						{
							sprintf( msg, "Reached maximum retry count %d for Tran_ID : %s, processing next preauth",
									 retryCount, Auth_Tx.TLF01_details.primary_key.transaction_id);
							display_error_message(msg, "ProcessTransactions");
							return PTEMSG_GUI_TIMEDOUT;
						}
						goto SEND_REPEAT_REQUEST_AGAIN;
					}
				}
				else
				{
					err_ctr++;
					pteipc_get_errormsg(lReturnCode, strTemp);
					sprintf( strBuffer,
							 "Error on pteipc_receive %s, error code: %ld, retry attempt:%d",
							 strTemp, lReturnCode, err_ctr );
					parvbh_log_message( 2, 3, strTemp, ProcessTransactions);
					/* There is a problem with the queue.  Delay 1 second
					* so as to not flood Applnk's queue and syslog.
					*/
					usleep( 1000000 );

					/* Inform Operators if service's queue is no longer readable. */
					if ( err_ctr > 10 )
					{
						err_ctr = 0;
						sprintf( strBuffer,
								 "UNABLE TO ACCESS QUEUE!!!!");
						parvbh_log_message( 2, 3, strBuffer, ProcessTransactions);
						usleep (50000000);
					}
				}
				if( err_ctr >= advice_repeat_ctr)
				{
					sprintf( msg, "Reached maximum auth advice retry count %d , "
								  "No Response from Vertexon/Schemes, Transaction ID:%s, Network:%s",
							advice_repeat_ctr,
							Auth_Tx.TLF01_details.primary_key.transaction_id,
							NetCntlName);
					display_error_message(msg,"ProcessTransactions");
					return PTEMSG_GUI_TIMEDOUT;
				}
			}//while advice_repeat_ctr
		}
	}
	else if (rc == PTEMSG_NOT_FOUND)
	{
    	sprintf (msg, " TLF Record not found for tran_id:%s, network:%s", bch20->transaction_id, NetCntlName);
    	parvbh_log_message(2, 3, msg, "ProcessTransactions");

		return PTEMSG_NOT_FOUND;
	}
	else if( NOT_CONNECTED_TO_DB 			 == rc ||
			 NOT_LOGGED_INTO_DB 			 == rc ||
			 TNS_PACKET_WRITER_FAILED 		 == rc ||
			 SHUTDOWN_OR_STARTUP_IN_PROGRESS == rc ||
			 PTEMSG_NOT_CONNECTED_TO_DB		 == rc )
	{
		dbcommon_disconnect(msg);
	    if (dbcommon_connect("equitable", "equitable", "equitable", "ORCL", msg) != PTEMSG_OK)
	    {
	    	strcat (msg, " DB Reconnection failed, Shutting Down service");
	    	parvbh_log_message(2, 3, msg, "ProcessTransactions");
	    	shut_down();
	    	Stop();
	    }
	    else
	    {
	    	parvbh_log_message(2, 1, "DataBase Reconnected", "ProcessTransactions");
	    }
	}

	return PTEMSG_OK;
}

BYTE update_atp_batch_tables()
{
	BYTE ReturnCode = 0;

	memset(Auth_Tx.TLF01_details.product_codes[1].code, 0x00, sizeof(Auth_Tx.TLF01_details.product_codes[1].code));
	memset(Auth_Tx.TLF01_details.product_codes[4].code, 0x00, sizeof(Auth_Tx.TLF01_details.product_codes[4].code));
	memset(Auth_Tx.TLF01_details.voice_auth_text, 0x00, sizeof(Auth_Tx.TLF01_details.voice_auth_text));

	strcpy(Auth_Tx.TLF01_details.product_codes[1].code, PROCESSED);
	strcpy(Auth_Tx.TLF01_details.product_codes[4].code, USED);

	if(true == mchutil_check_If_MC_Transaction())
	{
		sprintf(Auth_Tx.TLF01_details.voice_auth_text,
				"Pre-Auth Reversed >%s days",
				maximum_validity_of_preauth_in_days_for_MC);
	}
	else if( true == mchutil_check_If_VISA_Transaction())
	{
		sprintf(Auth_Tx.TLF01_details.voice_auth_text,
				"Pre-Auth Reversed >%s days",
				maximum_validity_of_preauth_in_days_for_VISA);
	}
	else if(true == mchutil_check_If_AMEX_Transaction())
	{
		sprintf(Auth_Tx.TLF01_details.voice_auth_text,
				"Pre-Auth Reversed >%s days",
				maximum_validity_of_preauth_in_days_for_AMEX);
	}
	else if(true == mchutil_check_If_JCB_Transaction())
	{
		sprintf(Auth_Tx.TLF01_details.voice_auth_text,
				"Pre-Auth Reversed >%s days",
				maximum_validity_of_preauth_in_days_for_JCB);
	}
	else if(true == mchutil_check_If_Diners_Transaction())
	{
		sprintf(Auth_Tx.TLF01_details.voice_auth_text,
				"Pre-Auth Reversed >%s days",
				maximum_validity_of_preauth_in_days_for_DCI);
	}
	else if(true == mchutil_check_If_CUP_Transaction())
	{
		sprintf(Auth_Tx.TLF01_details.voice_auth_text,
				"Pre-Auth Reversed >%s days",
				maximum_validity_of_preauth_in_days_for_UPI);
	}

	ReturnCode = db_update_bch20_stip(&Auth_Tx.TLF01_details);

	ReturnCode = db_update_tlf01(&Auth_Tx.TLF01_details);

	if(atp_stip_processed == true)
	{
		ReturnCode = db_update_tlf01_stip(&Auth_Tx.TLF01_details);
		atp_stip_processed = false;
	}

	return ReturnCode;
}

int get_network_name()
{
	int ret_val = 0;

	memset(Auth_Tx.TLF01_details.acquirer_id, 0, sizeof(Auth_Tx.TLF01_details.acquirer_id));

	if(mchutil_check_If_MC_Transaction() == true)
	{
		if(strcmp(Auth_Tx.TLF01_details.product_codes[6].code, "OFFUS") == 0)
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, mc_offus_link);
		}
		else
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, mc_onus_link);
		}
	}
	else if(mchutil_check_If_VISA_Transaction() == true)
	{
		if(strcmp(Auth_Tx.TLF01_details.product_codes[6].code, "OFFUS") == 0)
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, visa_offus_link);
		}
		else
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, visa_onus_link);
		}
	}
	else if(mchutil_check_If_JCB_Transaction() == true)
	{
		if(strcmp(Auth_Tx.TLF01_details.product_codes[6].code, "OFFUS") == 0)
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, jcb_offus_link);
		}
		else
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, jcb_onus_link);
		}
	}
	else if(mchutil_check_If_Diners_Transaction() == true)
	{
		if(strcmp(Auth_Tx.TLF01_details.product_codes[6].code, "OFFUS") == 0)
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, diners_offus_link);
		}
		else
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, diners_onus_link);
		}
	}
	else if(mchutil_check_If_AMEX_Transaction() == true)
	{
		if(strcmp(Auth_Tx.TLF01_details.product_codes[6].code, "OFFUS") == 0)
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, amex_offus_link);
		}
		else
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, amex_onus_link);
		}
	}
	else if(mchutil_check_If_CUP_Transaction() == true)
	{
		if(strcmp(Auth_Tx.TLF01_details.product_codes[6].code, "OFFUS") == 0)
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, cup_offus_link);
		}
		else
		{
			strcpy(Auth_Tx.TLF01_details.acquirer_id, cup_onus_link);
		}
	}

	if(strlen(Auth_Tx.TLF01_details.acquirer_id) == 0)
	{
		return 1;
	}

	return ret_val;
}

void PinnacleMsgHandler(pPTE_MSG p_msg_in)
{
	pPTE_MSG p_cur_msg = 0 ;
	pPTE_MSG p_msg_out = 0 ;
	BYTE     MessageType = 0 ;
	BYTE     ReturnCode = 0 ;
	CHAR     strTemp[256] = {0} ;

	memset (&Auth_Tx, 0, sizeof (AUTH_TX)) ;

    p_cur_msg = p_msg_in ;
    p_msg_out = 0 ;

    MessageType = ptemsg_get_msg_type (p_cur_msg) ;
    switch (MessageType)
    {
     	 case MT_AUTH_RESPONSE:
     		 	 ReturnCode = Process_Preauth_Reversal_Response (p_cur_msg) ;
     		 	 break ;

     	 default:
     		 	 strcpy (strTemp, "Unknown message type received in the Pinnacle Message Handler") ;
     		 	 parvbh_log_message( 2, 3, strTemp, PinnacleMsgHandler);
     		 	 break ;
    }
} /* PinnacleMsgHandler */

BYTE Process_Preauth_Reversal_Response (pPTE_MSG p_msg_in)
{
	BYTE ReturnCode = true;
	pBYTE pATEMessageDataData = 0 ;
	pPTE_MSG_DATA pATEMessageData = 0 ;
	char strTemp[256] = {0} ;
    int txnCount=0;
	char strBuffer[50]={0};

	pATEMessageData			= ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData		= ptemsg_get_pte_msg_data_data (pATEMessageData) ;

	/*******************************************************************/
    memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
	memcpy (&Auth_Tx, (AUTH_TX *)pATEMessageDataData, sizeof Auth_Tx) ;
	/*******************************************************************/
	sprintf (strTemp,"Received msg from host-RespCode:%s  ",
		      Auth_Tx.TLF01_details.response_code) ;
	parvbh_log_message( 2, 1, strTemp, "Process_Preauth_Reversal_Response");

	 if( (0== strncmp(Auth_Tx.TLF01_details.response_code,
			          REFER_TO_CARD_ISSUER,ISO_RESP_LEN)) ||
		 (0== strncmp(Auth_Tx.TLF01_details.response_code,
			          DESTINATION_UNAVAILABLE,ISO_RESP_LEN)))
	 {
		 sprintf( strBuffer, "Host is not online");
		 parvbh_log_message( 2, 3, strBuffer, "Process_Preauth_Reversal_Response");
		 return 1;
	 }
	 else if ((Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE   		||
			   Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE ||
			   Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE               ||
			   Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_REPEAT_RESPONSE))
	{
		if ((0== strncmp(Auth_Tx.TLF01_details.response_code,
						 TRANSACTION_APPROVED,ISO_RESP_LEN)) ||
			(0== strncmp(Auth_Tx.TLF01_details.response_code,
						 DUPLICATE_TRANSACTION,ISO_RESP_LEN)) ||
			(0== strncmp(Auth_Tx.TLF01_details.product_codes[12].code,
						 DUPLICATE_TRANSACTION_AMEX_ACTION_CODE,ACTION_CODE_LEN)) ||
			(0== strncmp(Auth_Tx.TLF01_details.product_codes[12].code,
						 DUPLICATE_TRANSACTION_DCI_ACTION_CODE,ACTION_CODE_LEN)))
		{
			printf("Response returned from Host for Auth Reversal Advice \n");
			memset( &db_tlf01_struct, 0, sizeof( db_tlf01_struct ) );
			memcpy( &db_tlf01_struct, ( pTLF01 )pATEMessageDataData, sizeof( TLF01 ) );

			if (DB_NullCheck_Flag==1)
			{
				CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			}

			ReturnCode = update_atp_batch_tables();
		}
		else if(0 == strncmp(Auth_Tx.TLF01_details.response_code,
							  REENTER_TRANSACTION,ISO_RESP_LEN))
		{
			sprintf( strBuffer, "Timeout Recieved, Reversal transaction is not processed");
			parvbh_log_message( 2, 3, strBuffer, "Process_Preauth_Reversal_Response");
		}
	}
   else
   {
	   sprintf( strBuffer, "Its not a valid response-%s with -%s ",Auth_Tx.TLF01_details.tx_key,Auth_Tx.TLF01_details.message_type);
	   parvbh_log_message( 2, 3, strBuffer, "Process_Preauth_Reversal_Response");
   }

	return 0;
}

BYTE Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1,
							  BYTE nSubType2, BYTE nAppDataType,
							  pBYTE p_data,   INT nDataSize)
{
	LONG retCode = 0L ;
	pPTE_MSG p_msg_out = NULL_PTR ;
	INT nBufferSize = 0 ;
	char strError[512] = {0} ;

	memset (p_buffer, 0, sizeof p_buffer) ;
	memcpy (p_buffer, &Auth_Tx, sizeof Auth_Tx) ;

	if (p_data != 0)
		memcpy (p_buffer + sizeof Auth_Tx, p_data, nDataSize) ;

	nBufferSize = sizeof Auth_Tx + nDataSize ;

	p_msg_out = ptemsg_build_msg (  nType,
								    nSubType1,
								    nSubType2,
								    que_name,
								    application_que_name,
								    (pBYTE) p_buffer,
								    nBufferSize,
								    nAppDataType) ;

	if (p_msg_out == NULL_PTR)
	{
		strcpy (strError,"Insufficient Memory for building terminal message") ;
		parvbh_log_message( 2, 3, strError, "Send_Request_To_Service");
		return SYSTEM_ERROR ;
 	}

	retCode = pteipc_send (p_msg_out, que_name) ;
	free (p_msg_out) ;

	if (retCode != 0)
	{
		sprintf (strError,"Error %ld has occurred sending message to %s\n", retCode, que_name) ;
		parvbh_log_message( 2, 3, strError, "Send_Request_To_Service");
		return SYSTEM_ERROR ;
	}

	return DEVICE_OK ;
} /* Send_Request_To_Service */

BYTE Check_Is_Host_Up (BYTE* destination_name)
{
   XINT  ReturnCode ;
   CHAR  strTemp [256] = {0} ;
   BYTE  tmouts[100] = {0}, txns[100] = {0}, state[100] = {0} ;
   BYTE  ret_val = false;int i;

   if (destination_name[0] != 0)
   {
      for( i = 0; destination_name[i] != '\0'; i++);
	  destination_name[--i];
	  if(destination_name[i] == 'A')
	  {
		  destination_name[i] = '\0';
	  }
	  strcpy (strTemp, destination_name) ;
      strcat (strTemp, "Table") ;

      ReturnCode = ReadGenericTable (strTemp, tmouts, txns, state) ;
      if (ReturnCode == MEMACCESSERROR)
      {
         memset (strTemp, 0, sizeof strTemp) ;
         sprintf (strTemp,
                 "Err accessing shared memory (%s)\n",
                  destination_name) ;
         parvbh_log_message( 2, 3, strTemp, "Check_Is_Host_Up");
      }
      else
      {
         /* Forward request to network controller
          * if the network controller is Online.
          */

		  if (0 ==  strncmp(state, ONLINE,1) )
		  {
            ret_val = true;
      }
   }
   }
   return( ret_val );

} /* Check_Is_Host_Up */

void display_error_message(pBYTE msg, pBYTE function)
{
   parvbh_log_message( 2, 1, msg, function);
   /* The print macro only displays messages in debug mode */
   strcat(msg, "\n");
   PRINT(msg);

   return;

} /* display_error_message */

INT get_maximum_validity_of_preauth_in_days()
{
   CHAR  rc_value[3]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "MAX_PREAUTH_VALIDITY_DAYS_FOR_MC",       	/* points to key name             */
									  "12",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
	   strncpy( maximum_validity_of_preauth_in_days_for_MC,
			    MAX_PREAUTH_VALIDITY_DAYS,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_MC,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_VISA",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
	   strncpy( maximum_validity_of_preauth_in_days_for_VISA,
			    MAX_PREAUTH_VALIDITY_DAYS,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_VISA,rc_value,ISO_RESPONSE_CODE_LEN);
	}


	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_AMEX",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);
	if(strlen(rc_value)==0)
	{
		strncpy( maximum_validity_of_preauth_in_days_for_AMEX,
				 MAX_PREAUTH_VALIDITY_DAYS,
				 ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_AMEX,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_DCI",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy( maximum_validity_of_preauth_in_days_for_DCI,
				 MAX_PREAUTH_VALIDITY_DAYS,
				 ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_DCI,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "MAX_PREAUTH_VALIDITY_DAYS_FOR_JCB",       	/* points to key name             */
									  "12",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
		strncpy( maximum_validity_of_preauth_in_days_for_JCB,
				 MAX_PREAUTH_VALIDITY_DAYS,
				 ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_JCB,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_UPI",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy( maximum_validity_of_preauth_in_days_for_UPI,
				 MAX_PREAUTH_VALIDITY_DAYS,
				 ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_UPI,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	return (ret_val);
}

INT mchutil_check_If_VISA_Transaction (void)
{
	if (Auth_Tx.TLF01_details.card_num[0]=='4')
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT mchutil_check_If_MC_Transaction (void)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6;

	strncpy(MC_bin_series,Auth_Tx.TLF01_details.card_num,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	Auth_Tx.TLF01_details.card_num[0]=='5')
	{
		return true ;
	}
	else if( (MC_bin_value >= MC_Bin_series_low) && (MC_bin_value <= MC_Bin_series_high))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT mchutil_check_If_JCB_Transaction (void)
{
	if((Auth_Tx.TLF01_details.card_num[0]=='3') &&
	   (Auth_Tx.TLF01_details.card_num[1] == '5'))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT mchutil_check_If_AMEX_Transaction (void)
{
	if((Auth_Tx.TLF01_details.card_num[0]=='3') &&
	   ((Auth_Tx.TLF01_details.card_num[1] == '4') ||
		(Auth_Tx.TLF01_details.card_num[1] == '7')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT mchutil_check_If_Diners_Transaction (void)
{
	if((Auth_Tx.TLF01_details.card_num[0]=='3') &&
	   ((Auth_Tx.TLF01_details.card_num[1] == '0') ||
		(Auth_Tx.TLF01_details.card_num[1] == '6') ||
		(Auth_Tx.TLF01_details.card_num[1] == '8') ||
		(Auth_Tx.TLF01_details.card_num[1] == '9') ))
	{
		return true ;
	}
	else if ((Auth_Tx.TLF01_details.card_num[0]=='6') &&
			 ((Auth_Tx.TLF01_details.card_num[1] == '0') ||
			 (Auth_Tx.TLF01_details.card_num[1] == '4') ||
			 (Auth_Tx.TLF01_details.card_num[1] == '5')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT mchutil_check_If_CUP_Transaction (void)
{
	if (0 == strncmp(Auth_Tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
	{
		return true ;
	}
	else if (0 == strncmp(Auth_Tx.TLF01_details.merchant_id,CUP_merchant_indicator,CUP_MID_INDICATOR_LEN))
	{
		if((true == mchutil_check_If_VISA_Transaction()) ||
		   (true == mchutil_check_If_MC_Transaction()))
		{
			return true ;
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2);
			return true ;
		}
	}
	else
	{
		return false;
	}
}

INT mchutil_read_CUP_merchant_indicator( void )
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
			 "CUPMIDSECTION",     /* points to section name       */
			 "CUP_MID_INDICATOR",  /* points to key name           */
			 "0",                       		/* points to default string     */
			 ini_info,                      /* points to destination buffer */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
			 filename                       /* points to ini filename       */
	);

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( CUP_merchant_indicator, tmpstring, CUP_MID_INDICATOR_LEN );
	if(strlen(CUP_merchant_indicator)==0)
	{
	   strncpy( CUP_merchant_indicator,
				CUP_MID_INDICATOR_DEFAULT,
				CUP_MID_INDICATOR_LEN);// Default Value
	}
	return( ret_val );
}
/****************************************PREAUTH REVERSALS FUNCTIONS - END***************************************/


/****************************************COMMON FUNCTIONS - START***************************************/

char* mask_card_number(char *cardNum)
{
	int cardlen = 0;
	int nIndex 	= 0;
	char masked_card_num[20] = {0};

	cardlen = strlen( cardNum );
	if (cardlen > 0)
	{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		strncpy( masked_card_num, cardNum, cardlen);
		for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
		{
			masked_card_num[nIndex] = '*';
		}
	}

	return &masked_card_num;
}

void get_date( pCHAR time_date, pCHAR date_str )
{
	memset ( date_str,  0x00,         9 );
	strncpy( date_str,  time_date,    4 );
	strncat( date_str, &time_date[5], 2 );
	strncat( date_str, &time_date[8], 2 );
}

void get_beyond_start_date(char* beyond_date, int days)
{
	char buf_date[9] = {0};
	long julian_date = 0;

	strcpy(buf_date, sys_current_date);
	//convert to julian, subtract and convert back to normal
    julian_date = Txutils_Calculate_Julian_Date ((BYTE*)buf_date);
    julian_date = julian_date - days;
    Txutils_Calculate_Gregorian_Date( julian_date, buf_date);

    memcpy(beyond_date, buf_date, strlen(buf_date));
}
/****************************************COMMON FUNCTIONS - END***************************************/

void get_max_retry_count()
{
	DWORD rc;
	CHAR  filename   [80] = {0};
	CHAR  tmpstr     [80] = {0};
	CHAR  buffer	 [5]  = {0};

	/* Get path to the tf.ini file. */
	memset( tmpstr, 0x00, sizeof(tmpstr) );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	rc = GetPrivateProfileString(
								 "PARVBH",             /* points to section name         */
								 "MAX_RETRY_COUNT",       	/* points to key name             */
								 "3",  					/* Default string                 */
								 buffer,              	/* points to destination buffer   */
								 sizeof(buffer)-1,   	 /* size of destination buffer     */
								 filename                  /* points to ini filename         */
							 	);

	if(buffer[0] != '\0')
	{
		MaxRetryCount = atoi(buffer);
	}
	else
	{
		MaxRetryCount = 3;
	}

	rc = GetPrivateProfileString(
								 "PARVBH",             /* points to section name         */
								 "RESPONSE_WAIT_FLAG",       	/* points to key name             */
								 "1",  					/* Default string                 */
								 buffer,              	/* points to destination buffer   */
								 sizeof(buffer)-1,   	 /* size of destination buffer     */
								 filename                  /* points to ini filename         */
							 	);

	if(buffer[0] != '\0')
	{
		reponseWaitFlag = atoi(buffer);
	}
	else
	{
		reponseWaitFlag = 1;
	}
}

INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf )
{
   	#define sON  "1"
    #define sOFF "0"
    #define sDEFAULT_DS_ERROR_STATICS_VALUE  "0.5"

    DWORD rc;
    CHAR  filename   [80];
    CHAR  tmpstr     [80];
    CHAR  timer_flag [5] = {0};
    CHAR  timer_value[5] = {0};
    INT   ret_val = true;
    CHAR  db_nullchk_flag[2] = {0};
    CHAR  parvbh_error_warning_file_size[5] = {0};

    /* Get path to the tf.ini file. */
    memset( tmpstr, 0x00, sizeof(tmpstr) );
    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, "tf.ini" );

    memcpy( timer_value,
            sDEFAULT_DS_ERROR_STATICS_VALUE,
            sizeof(sDEFAULT_DS_ERROR_STATICS_VALUE) );

    rc = GetPrivateProfileString(
               	   	   	   	   	   "DATASERVERS",             		/* points to section name         */
								   "DB_STATICS_VALUE",       		/* points to key name             */
								   sDEFAULT_DS_ERROR_STATICS_VALUE, /* Default string                 */
								   timer_value,              		/* points to destination buffer   */
								   sizeof(timer_value)-1,    		/* size of destination buffer     */
								   filename                  		/* points to ini filename         */
    							 );

    *ds_timer_value = atof(timer_value);
    if ( (*ds_timer_value < 0) || (*ds_timer_value > 999) )
    {
         sprintf( errbuf,
                 "%s:Invalid value DB error statics Value (%s) in tf.ini. Use 0 to 999",
                  appname, timer_value );
         *ds_timer_flag = DS_TIMER_OFF;
         ret_val = false;
     }

    /*Read path for creating file in order to log db statics and db oracle error messages */
    rc = GetPrivateProfileString(
                  	  	  	  	   "DATASERVERS",
								   "DB_STATICS_PATH",
								   "",
								   DB_file_path,
								   sizeof(DB_file_path)-1,
								   filename
         	 	 	 	 	 	 );

    rc = GetPrivateProfileString(
                      	  	  	   "ERROR_WARNING",
								   appname,
								   "",
								   DB_module_file_name,
								   sizeof(DB_module_file_name)-1,
								   filename
            					 );

    rc = GetPrivateProfileString(
							  	   "ERROR_WARNING",
								   "ERROR_WARNING_FILE_SIZE",
								   "500",
								   parvbh_error_warning_file_size,
								   sizeof(parvbh_error_warning_file_size)-1,
								   filename
								 );

	/* File size size conversion */
	Max_File_Size_Defined = atoi(parvbh_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		 Max_File_Size_Defined = 500;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

    if((strlen(DB_file_path) > 0) && (strlen(DB_module_file_name)> 0))
    {
    	 Create_Db_Statics_And_Db_Oracle_Error_Filename();
    }

    return( ret_val );
}

void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf)
{
	INT   len=0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR  path[256]={0};
	CHAR  Buffer[1024]={0};
	int   path_len=0;
	FILE *fp;
	#define NUM_SIZE_WRITES  1

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );
	strcpy(Buffer,time_date);
    /* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(DB_Logging_Filename);
    path_len =strlen(DB_file_path);
    if(len==0 || path_len==0)
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return;
    }
    if ( 0 != strcmp(&DB_Logging_Filename[len-4], current_mmdd) )
    {
		Create_Db_Statics_And_Db_Oracle_Error_Filename();
	}
	if((fp = fopen(DB_Logging_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return;
	}

	strcat(Buffer,":");
	strcat(Buffer,Error_Buf);
	strcat(Buffer,"\n");
	len=0;
	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// fal below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
	}

	fclose(fp);
	return ;
}

void Create_Db_Statics_And_Db_Oracle_Error_Filename()
{
	CHAR  system_date[25] = "";

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( DB_Logging_Filename, DB_file_path );
    strcat( DB_Logging_Filename, DB_module_file_name );
	strcat( DB_Logging_Filename, ".EW." );
    strncat(DB_Logging_Filename, system_date,   4 );  /* YYYY */
    strncat(DB_Logging_Filename, system_date+5, 2 );  /* MM   */
    strncat(DB_Logging_Filename, system_date+8, 2 );  /* DD   */

    return;
}

/******************************************************************************
 *
 *  NAME:         parvbh_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = Event Log, 3 = Both
 *                sev  - 1 = Info,    2 = Warning,   3 = Error
 *                msg  - Text message to be logged
 *                func - Name of function logging the error
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Amith
 *
 ******************************************************************************/
void parvbh_log_message( INT dest, INT sev, pCHAR msg, pCHAR func)
{
   INT  monitor_flag;
   INT  eventlog_flag;
   INT  msg_type;
   BYTE severity;
   BYTE log_type[20]={0};
   CHAR text_message[512]={0};

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

   if(tf_debug == 1)
	   printf("\n%s\n", msg);

   /* Make sure text message is not too long. */
   memcpy( text_message, msg, sizeof(text_message)-1 );
   parvbh_Log_error_warning_to_File(text_message,sev,func);
   /* Call function to post the message. */
   if(monitor_flag == 1)
   {
		TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
		   							  eventlog_flag, msg_type, func,
		   							  severity, log_type,
		   							  NULL, NULL,NULL );
   }
   return;
}

INT parvbh_Log_error_warning_to_File(pCHAR Error_Buf, int sev, pCHAR func)
{
	INT   len=0;
	INT   path_len = 0;
	CHAR  time_date[25]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR  Buffer[1024]={0};
	CHAR  card_tid_mid_details [256] = {0};
	INT   cardlen = 0;
	INT   nIndex = 0;
	CHAR  tempcard[21] = {0};
	FILE  *fp;
	UINT  file_size = 0 ;
	char  buf[256] = {0} ;

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(DB_Logging_Filename);
    path_len = strlen(DB_file_path);
    if( len == 0 ||	path_len==0 )
    {
    	LogEvent(Error_Buf, INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&DB_Logging_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	Create_Db_Statics_And_Db_Oracle_Error_Filename();
    }
	if((fp = fopen(DB_Logging_Filename, "a+b"))==NULL)
	{
		LogEvent(Error_Buf, INFO_MSG);
		return 0;
	}
	strcpy(Buffer, time_date);
	strcat(Buffer, ":");
	if(sev == 1)
	{
		strcat(Buffer, " INFO");
	}
	else if (sev == 2)
	{
		strcat(Buffer, " WARN");
	}
	else
	{
		strcat(Buffer, " ERROR");
	}
	strcat(Buffer, ": ");
	strcat(Buffer, Error_Buf);
	strcat(Buffer, ",");
	strcat(Buffer, func);
	strcat(Buffer, "\n");
	len = strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
		fclose(fp);
		return  0;
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

void EndProcess()
{
   CHAR Buffer[100] = "";

   sprintf( Buffer, "Shutting down the %s Utility, version %s ", ServiceName, Version );
   parvbh_log_message(2, 1, Buffer, " EndProcess ");
}

INT get_maximum_validity_of_preauth_in_days_for_smcc(void)
{
   CHAR  rc_value[3]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_MC",       	/* points to key name             */
									  "05",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
	   strncpy( smcc_maximum_validity_of_preauth_in_days_for_MC,
			    SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_MC,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_VISA",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
	   strncpy( smcc_maximum_validity_of_preauth_in_days_for_VISA,
			    SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_VISA,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}


	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_AMEX",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);
	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_AMEX,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_AMEX,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_DCI",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_DCI,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_DCI,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_JCB",       	/* points to key name             */
									  "05",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_JCB,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_JCB,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_UPI",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_UPI,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_UPI,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	return (ret_val);
}

