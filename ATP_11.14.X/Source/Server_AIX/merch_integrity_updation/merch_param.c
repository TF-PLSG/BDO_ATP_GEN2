/******************************************************************************
 *
 *  Name			:       merch_param.c
 *
 *  Version			:		ATP_11.6.1
 *
 *  Service Name	: 		merch_integrity_updation
 *
 *  Parameters		:	  	None
 *
 *  DB Tables used	:  		MCF01, MERCHANT_INTEGRITY_PARAM, SCHEME_INTEGRITY_CATEGORY
 *
 *  Config. file	:  		tf.ini, merchant_integrity.ini
 *
 *  Description		:  		1. Reads the MERCHANT_INTEGRITY_PARAM table based on card type and unblocks the merchants based on other categories.
 *  			  	 		2. Checks the re-attempts date and removes the re-attempts which falls beyond 30 days.
 *  			  		    3. Reset the merchant status if unblocked
 *  			  			4. Reads the SCHEME_INTEGRITY_CATEGORY records based on Scheme & Current system date & updates the merchant_integrity.ini file.
 *
 *  Scheduler		:		Cron Jobs
 *  						Example:-
 *  						Enter 'crontab -e' command, add this below line & save it.
 *  						0 2 * * * . $HOME/.profile; $HOME/ascendent/bin/merch_integrity_updation
 *  						This utility will execute everyday @2AM.
 *  						Including .profile to get access to the environment variables.
 *  						Cron jobs can be seen using this command, 'crontab -l'
 *
 *  Author			:       Amith Karunakar
 *
 ******************************************************************************/

/*
 * Rev 1.0 May 04, 2023
 * Fix version: ATP_11.6.0 by Amith
 * Removed merchant id's 2D array of 1000.
 * Added new SQL query to update merchant status
 * */

/*
 * Rev 2.0 July 14, 2023
 * Fix version: ATP_11.6.1 by Amith
 * Added category unblock functionality based on other categories
 * Added re-attempts check function to update the re-attempts
 * Beyond 30 re-attempts will be removed from MIP table
 * Added new SQL query to select and update re-attempt dates
 * */

/*
 * Rev 3.0 July 17, 2023
 * Fix version: ATP_11.7.0 by Amith
 * Supported the Excessive re-attempts decline for new Card Brand : Master Card
 * Supported the MAC validation for CNP declines for Master Card
 * */


#include "merch_param.h"

CHAR   Version[] = "ATP_11.8.0";
int    tf_debug;
CHAR   sys_current_date[9];
CHAR   visaEndDate[9];
CHAR   mcEndDate[9];
CHAR   macEndDate[9];
CHAR   visaBeyondDate[9];
CHAR   mcBeyondDate[9];

void main(int argc, char *argv[])
{
	CHAR     Buffer[256];
	CHAR     strError[512] = {0} ;
	CHAR     time_date[25] = {0};
	CHAR 	 sys_date[25]  = {0};
	INT 	 ret_val = 0;

	if(argc > 1 && strcmp(argv[1], "debug") == 0)
		tf_debug = 1;

    //GetAppName( AppName );
    strcpy(AppName, "merch_integrity_updation");
    strcpy(ServiceName, "merch_integr");
    ret_val = get_dataserver_ini_db_error_statics_value(&db_error_statics_flag, &db_error_statics_value, ServiceName, Buffer);

    if ( false == ret_val )
    {
        mparam_log_message(2, 2, Buffer, " Main ");
    }
    else if ( db_error_statics_flag == DS_TIMER_ON )
    {
        sprintf( Buffer, "%s:Data Server DB error statics for queries is turned on, value is %d ",
                ServiceName, db_error_statics_value );
        mparam_log_message(2,1,Buffer," Main ");
    }

    sprintf( Buffer, "Starting the Utility: %s, version %s ", AppName, Version );
    mparam_log_message(2, 1, Buffer, " Main ");
    //printf( "\n%s\n", Buffer );

	/* Try to connect to the database */
	if (dbcommon_connect("equitable", "equitable", "equitable", "ORCL", ErrorMsg) != PTEMSG_OK)
	{
		//MainProcessDone = 1;
	    //printf( "%s", ErrorMsg );
	    mparam_log_message(2, 3, ErrorMsg," Main ");
	    sprintf( Buffer, "Shutting down the Utility: %s, version %s ", AppName, Version );
	    mparam_log_message(2, 1, Buffer, " Main ");
	    //printf( "%s\n", Buffer );

        return;
    }
	//printf ("Connected to ORACLE\n\n");

    ptetime_get_timestamp( time_date );
    get_date( time_date, sys_date); // Reformat to get date "YYYYMMDD"
    memset(sys_current_date, 0, sizeof(sys_current_date));
    memcpy(sys_current_date, sys_date, strlen(sys_date));

	//merchant_integrity_checks for VISA
	visa_integrity_checks();

	//merchant_integrity_checks for MasterCard
	mc_integrity_checks();

	//merchant_integrity_checks for MasterCard MAC
	mc_mac_checks();

    //update merchant status in MCF01 table
    merchant_status_update();

    /* disconnect from database */
    if (dbcommon_disconnect (ErrorMsg) != PTEMSG_OK)
    {
    	mparam_log_message(2, 3, ErrorMsg, " Main ");
    }
    else
    {
    	;
    	//printf ( "\nDisconnected from ORACLE\n" );
    }

    EndProcess();
    /* signal that we are ready for shutdown */
    //MainProcessDone = 1;
    return;
}

void visa_integrity_checks()
{
	mparam_log_message(2, 1, "----------------VISA : MERCHANT_INTEGRITY_CHECKS----------------", "");

    //read re-attempts & reset days from tf.ini
    //read_system_integrity_parameters_for_VISA(); //not required

    //checking merchant_integrity_param table
    merchant_integrity_param_checks_visa();

    //update merchant status in MCF01 table
    merchant_status_update();//call 1

    //re-attempts date and count verify
    merchant_integrity_param_reattempt_checks_visa();

    //update merchant status in MCF01 table
    merchant_status_update();//call 2

	//read scheme_integrity_category table and update merchant_integrity.ini file
    read_scheme_integrity_category_visa();
}

void mc_integrity_checks()
{
	mparam_log_message(2, 1, "----------------MC : MERCHANT_INTEGRITY_CHECKS----------------", "");

    //checking merchant_integrity_param table
    merchant_integrity_param_checks_mc();

    //update merchant status in MCF01 table
    merchant_status_update();//call 1

    //re-attempts date and count verify
    merchant_integrity_param_reattempt_checks_mc();

    //update merchant status in MCF01 table
    merchant_status_update();//call 2

	//read scheme_integrity_category table and update merchant_integrity.ini file
    read_scheme_integrity_category_mc();
}

/****************************************VISA FUNCTIONS - START***************************************/
void merchant_integrity_param_checks_visa()
{
    get_end_date(&visaEndDate, 1);

	unblock_merchant_integrity_param_for_visa_cat1();
	unblock_merchant_integrity_param_for_visa_cat2_3_4();
}

int unblock_merchant_integrity_param_for_visa_cat1()
{
	int  ResultCode    = 0;
	int  ResultCode1   = 0;
	char errorMsg[256] = {0};

	ResultCode = db_delete_merch_int_param_visa_cat1(errorMsg);
	if(ResultCode == PTEMSG_OK)
	{
		dbcommon_commit();
		mparam_log_message(2, 1, "Removed the VISA CAT1 blockage from MERCHANT_INTEGRITY_PARAM table",
								 " unblock_merchant_integrity_param_for_visa_cat1");
	}

	ResultCode1 = db_update_merch_int_param_visa_cat1(errorMsg);
	if(ResultCode1 == PTEMSG_OK)
	{
		dbcommon_commit();
		mparam_log_message(2, 1, "Removed & Updated the VISA CAT1 blockage from MERCHANT_INTEGRITY_PARAM table",
								 " unblock_merchant_integrity_param_for_visa_cat1");
	}

	return PTEMSG_OK;
}

int unblock_merchant_integrity_param_for_visa_cat2_3_4()
{
	int  ResultCode    = 0;
	char errorMsg[256] = {0};

	ResultCode = db_delete_merch_int_param_visa_cat2(errorMsg);
	if(ResultCode == PTEMSG_OK)
	{
		dbcommon_commit();
		mparam_log_message(2, 1, "Removed the VISA CAT2 blockage from MERCHANT_INTEGRITY_PARAM table",
								 " unblock_merchant_integrity_param_for_visa_cat2_3_4");
	}

	ResultCode = db_delete_merch_int_param_visa_cat3(errorMsg);
	if(ResultCode == PTEMSG_OK)
	{
		dbcommon_commit();
		mparam_log_message(2, 1, "Removed the VISA CAT3 blockage from MERCHANT_INTEGRITY_PARAM table",
								 " unblock_merchant_integrity_param_for_visa_cat2_3_4");
	}

	ResultCode = db_delete_merch_int_param_visa_cat4(errorMsg);
	if(ResultCode == PTEMSG_OK)
	{
		dbcommon_commit();
		mparam_log_message(2, 1, "Removed the VISA CAT4 blockage from MERCHANT_INTEGRITY_PARAM table",
								 " unblock_merchant_integrity_param_for_visa_cat2_3_4");
	}

	return PTEMSG_OK;
}

int merchant_integrity_param_reattempt_checks_visa()
{
	int  ResultCode    = 0;
	char errorMsg[256] = {0}, buffer[256] = {0};

	mparam_log_message(2, 1, "----------------VISA : REATTEMPTS_CHECK----------------", "");

    memset(visaBeyondDate, 0, sizeof(visaBeyondDate));
    get_beyond_start_date(&visaBeyondDate, 29);
    sprintf(errorMsg, "BEYOND DATE : [%s]", visaBeyondDate);
    mparam_log_message(2, 1, errorMsg, " merchant_integrity_param_reattempt_checks_visa");

    ResultCode = db_select_merchant_integrity_param_reattempts_visa(&errorMsg);
    dbcommon_commit();
	if(ResultCode != PTEMSG_OK && ResultCode != PTEMSG_NOT_FOUND)
	{
		sprintf(buffer, "%s, Result Code : [%d]", errorMsg, ResultCode);
		mparam_log_message(2, 3, buffer, " merchant_integrity_param_reattempt_checks_visa");
	}
	else if(ResultCode == PTEMSG_OK)
		mparam_log_message(2, 1, "VISA Re-attempts checks completed for MIP table",
								 " merchant_integrity_param_reattempt_checks_visa");

	return PTEMSG_OK;
}

int verify_and_update_reattempt_date_visa(MERCH_INT_PARAMS mip_data)
{
	int  ResultCode    = 0, i, j, row, col;
	int  retVal = ERROR;
	char errorMsg[256] = {0}, buffer[256] = {0};
	char reattemptsDate[200][10] = {0};
	int  count, deleteCount = 0;
	int  mip_db_update = false;

	//parse the re-attempts date and copy to local array
	i=j=row=col=0;

	if(strlen(mip_data.reattempts_date) > 0 && mip_data.reattempts_date[strlen(mip_data.reattempts_date)-1] == ';')
	{
		while(mip_data.reattempts_date[j] != ';')
		{
			if(mip_data.reattempts_date[j] != '|')
			{
				reattemptsDate[row][col] = mip_data.reattempts_date[j];
				col++;
				if(mip_data.reattempts_date[j+1] == ';')
				{
					if(strlen(reattemptsDate[row]) != 4)
						return VALIDATION_ERROR;
					strcpy(reattemptsDate[row], get_complete_reattempts_date(reattemptsDate[row], strlen(reattemptsDate[row])) );
				}
			}
			else
			{
				if(mip_data.reattempts_date[j] == '|')
				{
					if(strlen(reattemptsDate[row]) != 4)
						return VALIDATION_ERROR;
					strcpy(reattemptsDate[row], get_complete_reattempts_date(reattemptsDate[row], strlen(reattemptsDate[row])) );
				}

				col=0;
				row++;
				i++;
			}
			j++;
		}
	}
	else
		return VALIDATION_ERROR;

    count = i+1;
    if(count == 1 && (strncmp(reattemptsDate[0], visaBeyondDate, 8) < 0) )
    {
    	/*	only one re-attempt and that too beyond 30days
    		delete the record if Cat1 is not blocked
    		update the record if Cat1 is blocked
    	 */
		if(mip_data.c1_block[0] != 'Y')
		{
			ResultCode = db_delete_merchant_integrity_param_visa(&mip_data, &errorMsg);
			retVal = DELETED;
		}
		else
		{
			/*Nullify all other category data, as it is blocked under CAT1
			  because, one re-attempt is present it doesn't matter if it belongs to any category
			  so Nullify the CAT 2,3,4 data
			 */
			memset(mip_data.c2_block, 0, sizeof(mip_data.c2_block));
			memset(mip_data.c3_block, 0, sizeof(mip_data.c2_block));
			memset(mip_data.c4_block, 0, sizeof(mip_data.c2_block));
			memset(mip_data.c2_count, 0, sizeof(mip_data.c2_count));
			memset(mip_data.c3_count, 0, sizeof(mip_data.c3_count));
			memset(mip_data.c4_count, 0, sizeof(mip_data.c4_count));
			memset(mip_data.first_reattempt_date, 0, sizeof(mip_data.first_reattempt_date));
			memset(mip_data.reattempts_date, 	  0, sizeof(mip_data.reattempts_date));
			ResultCode = db_update_merchant_integrity_param_visa(&mip_data, &errorMsg);
			retVal = UPDATED;
		}

    	if(ResultCode != PTEMSG_OK)
    	{
    		sprintf(buffer, "%s Result Code:[%d] %s [%s:%s]",
    						errorMsg,
							ResultCode,
    						(retVal==UPDATED)?" on UPDATE":" on DELETE",
    						mip_data.primary_key.merchant_id,
							mask_card_number(&mip_data.primary_key.card_nbr));
    		mparam_log_message(2, 3, buffer, " verify_and_update_reattempt_date_visa");
    		retVal = ERROR;
    	}
    	else
    	{
    		//dbcommon_commit();//cannot commit inside cursor
    		if(retVal == UPDATED)
    		{
    			sprintf(errorMsg, "Updated the VISA record in MIP table [%s:%s]",
	    			          	  mip_data.primary_key.merchant_id,
								  mask_card_number(&mip_data.primary_key.card_nbr));
    		}
    		else
    		{
    			sprintf(errorMsg, "Deleted the VISA record from MIP table [%s:%s]",
    	    			          mip_data.primary_key.merchant_id,
    							  mask_card_number(&mip_data.primary_key.card_nbr));
    		}
    		mparam_log_message(2, 1, errorMsg, " verify_and_update_reattempt_date_visa");
    	}

    	return retVal;
    }
    else if(count > 1)
    {
		for(i=0; i<count; i++)
		{			//0602|0603|0701|0707 0609
			if(strncmp(reattemptsDate[i], visaBeyondDate, 8) >= 0)
			{
				if(i != 0)
				{
					mip_db_update = true;
					deleteCount = i;
					memset(mip_data.first_reattempt_date, 0, sizeof(mip_data.first_reattempt_date));
					memset(mip_data.reattempts_date, 	  0, sizeof(mip_data.reattempts_date));
					while(i<count)//0701|0707
					{
						strncat(mip_data.reattempts_date, reattemptsDate[i]+4, 4);
						strncat(mip_data.reattempts_date, "|", 1);
						i++;
					}
					if(strlen(mip_data.reattempts_date) > 0)
					{
						mip_data.reattempts_date[strlen(mip_data.reattempts_date)-1] = ';';
						memcpy(mip_data.first_reattempt_date, reattemptsDate[deleteCount], strlen(reattemptsDate[deleteCount]));
					}//0701
				}
				break;
			}
			else if((i+1 == count))
			{
				mip_db_update = true;
				deleteCount = count;
				memset(mip_data.first_reattempt_date, 0, sizeof(mip_data.first_reattempt_date));
				memset(mip_data.reattempts_date,      0, sizeof(mip_data.reattempts_date));
			}
		}
    }

    if(mip_db_update == true)
    {
    	if(strlen(mip_data.reattempts_date) == 0 && strlen(mip_data.first_reattempt_date) == 0)
    	{
    		if(mip_data.c1_block[0] != 'Y')
    		{
    			ResultCode = db_delete_merchant_integrity_param_visa(&mip_data, &errorMsg);
    			retVal = DELETED;
    		}
    		else
    		{
    			/*Nullify all other category data, as it is blocked under CAT1
    			  because, all re-attempts are cleared it doesn't matter if it belongs to any category
    			  so Nullify the CAT 2,3,4 data
    			 */
    			memset(mip_data.c2_block, 0, sizeof(mip_data.c2_block));
    			memset(mip_data.c3_block, 0, sizeof(mip_data.c2_block));
    			memset(mip_data.c4_block, 0, sizeof(mip_data.c2_block));
    			memset(mip_data.c2_count, 0, sizeof(mip_data.c2_count));
    			memset(mip_data.c3_count, 0, sizeof(mip_data.c3_count));
    			memset(mip_data.c4_count, 0, sizeof(mip_data.c4_count));
    			ResultCode = db_update_merchant_integrity_param_visa(&mip_data, &errorMsg);
    			retVal = UPDATED;
    		}
    	}
    	else
    	{
        	updates_count_visa(&mip_data, deleteCount);
    		ResultCode = db_update_merchant_integrity_param_visa(&mip_data, &errorMsg);
    		retVal = UPDATED;
    	}

    	if(ResultCode != PTEMSG_OK)
    	{
    		sprintf(buffer, "%s Result Code:[%d] %s [%s:%s]",
    						errorMsg,
							ResultCode,
    						(retVal==UPDATED)?" on UPDATE":" on DELETE",
    						mip_data.primary_key.merchant_id,
							mask_card_number(&mip_data.primary_key.card_nbr));
    		mparam_log_message(2, 3, buffer, " verify_and_update_reattempt_date_visa");
    		retVal = ERROR;
    	}
    	else
    	{
			//dbcommon_commit();//cannot commit inside cursor
    		if(retVal == UPDATED)
    		{
    			sprintf(errorMsg, "Updated the VISA record in MIP table [%s:%s]",
	    			          	  mip_data.primary_key.merchant_id,
								  mask_card_number(&mip_data.primary_key.card_nbr));
    		}
    		else
    		{
    			sprintf(errorMsg, "Deleted the VISA record from MIP table [%s:%s]",
    	    			          mip_data.primary_key.merchant_id,
    							  mask_card_number(&mip_data.primary_key.card_nbr));
    		}
			mparam_log_message(2, 1, errorMsg, " verify_and_update_reattempt_date_visa");
    	}
    }

    return retVal;
}

void updates_count_visa(pMERCH_INT_PARAMS mipData, int delCount)
{
	int c2Count, c4Count;

	c2Count = atoi(mipData->c2_count);
	c4Count = atoi(mipData->c4_count);

	if(c2Count == c4Count)
	{
		c2Count -= delCount;
		c4Count -= delCount;
	}
	else if(c2Count > c4Count)
	{
		if((c2Count - c4Count) >= delCount)
		{
			c2Count -= delCount;
		}
		else
		{
			c4Count -= delCount - (c2Count - c4Count);
			c2Count -= delCount;
		}
	}
	else if(c4Count > c2Count)//5,1
	{
		if((c4Count - c2Count) >= delCount)//4 >= 2
		{
			c4Count -= delCount;
		}
		else
		{
			c2Count -= delCount - (c4Count - c2Count);
			c4Count -= delCount;
		}
	}

	if(c2Count > 0)
		sprintf(mipData->c2_count, "%02d", c2Count);
	else
	{
		memset(mipData->c2_block, 0, sizeof(mipData->c2_block));
		memset(mipData->c2_count, 0, sizeof(mipData->c2_count));
	}

	if(c4Count > 0)
		sprintf(mipData->c4_count, "%02d", c4Count);
	else
	{
		memset(mipData->c4_block, 0, sizeof(mipData->c4_block));
		memset(mipData->c4_count, 0, sizeof(mipData->c4_count));
	}

	return;
}

void read_scheme_integrity_category_visa()
{
	int  ResultCode    = 0;
	char errorMsg[256] = {0};

	mparam_log_message(2, 1, "----------------VISA : SCHEME_INTEGRITY_CATEGORY----------------", "");
	ResultCode = db_get_scheme_integrity_category_visa(errorMsg);
	if(ResultCode != PTEMSG_OK && ResultCode != PTEMSG_NOT_FOUND)
	{
		//printf("\n%s\n", errorMsg);
		mparam_log_message(2, 3, strcat(errorMsg, "VISA"), " read_scheme_integrity_category_visa");
	}
	else if(ResultCode == PTEMSG_NOT_FOUND)
	{
		sprintf(errorMsg, "No records found in SCHEME_INTEGRITY_CATEGORY table for VISA");
		mparam_log_message(2, 1, errorMsg, " read_scheme_integrity_category_visa");
		//printf("\n%s\n", errorMsg);
	}

	return;
}
/****************************************VISA FUNCTIONS - END***************************************/



/****************************************MC FUNCTIONS - START***************************************/
void merchant_integrity_param_checks_mc()
{
    get_end_date(&mcEndDate, 1);

    reset_merchant_integrity_param_for_mc_cat1();
	unblock_merchant_integrity_param_for_mc_cat1();
	unblock_merchant_integrity_param_for_mc_cat2();
}

int reset_merchant_integrity_param_for_mc_cat1()
{
	int  ResultCode    = 0;
	int  ResultCode1   = 0;
	char errorMsg[256] = {0};

	ResultCode1 = db_reset_merch_int_param_mc_cat1(errorMsg);
	if(ResultCode1 == PTEMSG_OK)
	{
		dbcommon_commit();
		mparam_log_message(2, 1, "Reset the MC CAT1 blockage from MERCHANT_INTEGRITY_PARAM table",
								 " reset_merchant_integrity_param_for_mc_cat1");
	}

	return PTEMSG_OK;
}

int unblock_merchant_integrity_param_for_mc_cat1()
{
	int  ResultCode    = 0;
	int  ResultCode1   = 0;
	char errorMsg[256] = {0};

	ResultCode1 = db_update_merch_int_param_mc_cat1(errorMsg);
	if(ResultCode1 == PTEMSG_OK)
	{
		dbcommon_commit();
		mparam_log_message(2, 1, "Removed & Updated the MC CAT1 blockage from MERCHANT_INTEGRITY_PARAM table",
								 " unblock_merchant_integrity_param_for_mc_cat1");
	}

	return PTEMSG_OK;
}

int unblock_merchant_integrity_param_for_mc_cat2()
{
	int  ResultCode    = 0;
	char errorMsg[256] = {0};

	ResultCode = db_delete_merch_int_param_mc_cat2(errorMsg);
	if(ResultCode == PTEMSG_OK)
	{
		dbcommon_commit();
		mparam_log_message(2, 1, "Removed the MC CAT2 blockage from MERCHANT_INTEGRITY_PARAM table",
								 " unblock_merchant_integrity_param_for_mc_cat2");
	}

	return PTEMSG_OK;
}

int merchant_integrity_param_reattempt_checks_mc()
{
	int  ResultCode    = 0;
	char errorMsg[256] = {0}, buffer[256] = {0};

	mparam_log_message(2, 1, "----------------MC : REATTEMPTS_CHECK----------------", "");

    memset(mcBeyondDate, 0, sizeof(mcBeyondDate));
    get_beyond_start_date(&mcBeyondDate, 29);
    sprintf(errorMsg, "BEYOND DATE : [%s]", mcBeyondDate);
    mparam_log_message(2, 1, errorMsg, " merchant_integrity_param_reattempt_checks_mc");

    ResultCode = db_select_merchant_integrity_param_reattempts_mc(&errorMsg);
    dbcommon_commit();
	if(ResultCode != PTEMSG_OK && ResultCode != PTEMSG_NOT_FOUND)
	{
		sprintf(buffer, "%s, Result Code : [%d]", errorMsg, ResultCode);
		mparam_log_message(2, 3, buffer, " merchant_integrity_param_reattempt_checks_mc");
	}
	else if(ResultCode == PTEMSG_OK)
		mparam_log_message(2, 1, "MC Re-attempts checks completed for MIP table",
								 " merchant_integrity_param_reattempt_checks_mc");

	return PTEMSG_OK;
}

int verify_and_update_reattempt_date_mc(MERCH_INT_PARAMS mip_data)
{
	int  ResultCode    = 0, i, j, row, col;
	int  retVal = ERROR;
	char errorMsg[256] = {0}, buffer[256] = {0};
	char reattemptsDate[200][10] = {0};
	int  count, deleteCount = 0;
	int  mip_db_update = false;

	//parse the re-attempts date and copy to local array
	i=j=row=col=0;

	if(strlen(mip_data.reattempts_date) > 0 && mip_data.reattempts_date[strlen(mip_data.reattempts_date)-1] == ';')
	{
		while(mip_data.reattempts_date[j] != ';')
		{
			if(mip_data.reattempts_date[j] != '|')
			{
				reattemptsDate[row][col] = mip_data.reattempts_date[j];
				col++;
				if(mip_data.reattempts_date[j+1] == ';')
				{
					if(strlen(reattemptsDate[row]) != 4)
						return VALIDATION_ERROR;
					strcpy(reattemptsDate[row], get_complete_reattempts_date(reattemptsDate[row], strlen(reattemptsDate[row])) );
				}
			}
			else
			{
				if(mip_data.reattempts_date[j] == '|')
				{
					if(strlen(reattemptsDate[row]) != 4)
						return VALIDATION_ERROR;
					strcpy(reattemptsDate[row], get_complete_reattempts_date(reattemptsDate[row], strlen(reattemptsDate[row])) );
				}

				col=0;
				row++;
				i++;
			}
			j++;
		}
	}
	else
		return VALIDATION_ERROR;

    count = i+1;
    if(count == 1 && (strncmp(reattemptsDate[0], mcBeyondDate, 8) < 0) )
    {
    	/*	only one re-attempt and that too beyond 30days
    		delete the record
    	 */
		ResultCode = db_delete_merchant_integrity_param_mc(&mip_data, &errorMsg);
		retVal = DELETED;

    	if(ResultCode != PTEMSG_OK)
    	{
    		sprintf(buffer, "%s Result Code:[%d] %s [%s:%s]",
    						errorMsg,
							ResultCode,
    						" on DELETE",
    						mip_data.primary_key.merchant_id,
							mask_card_number(&mip_data.primary_key.card_nbr));
    		mparam_log_message(2, 3, buffer, " verify_and_update_reattempt_date_mc");
    		retVal = ERROR;
    	}
    	else
    	{
    		//dbcommon_commit();//cannot commit inside cursor
    		sprintf(errorMsg, "Deleted the MC record from MIP table [%s:%s]",
    	    			      mip_data.primary_key.merchant_id,
    						  mask_card_number(&mip_data.primary_key.card_nbr));
    		mparam_log_message(2, 1, errorMsg, " verify_and_update_reattempt_date_mc");
    	}

    	return retVal;
    }
    else if(count > 1)
    {
		for(i=0; i<count; i++)
		{			//0602|0603|0603 	  0609
			if(strncmp(reattemptsDate[i], mcBeyondDate, 8) >= 0)
			{
				if(i != 0)
				{
					mip_db_update = true;
					deleteCount = i;//1
					memset(mip_data.first_reattempt_date, 0, sizeof(mip_data.first_reattempt_date));
					memset(mip_data.reattempts_date, 	  0, sizeof(mip_data.reattempts_date));
					while(i<count)//0701|0707//1<3
					{
						strncat(mip_data.reattempts_date, reattemptsDate[i]+4, 4);
						strncat(mip_data.reattempts_date, "|", 1);
						i++;
					}
					if(strlen(mip_data.reattempts_date) > 0)
					{
						mip_data.reattempts_date[strlen(mip_data.reattempts_date)-1] = ';';
						memcpy(mip_data.first_reattempt_date, reattemptsDate[deleteCount], strlen(reattemptsDate[deleteCount]));
					}//0701
				}
				break;
			}
			else if((i+1 == count))
			{
				mip_db_update = true;
				deleteCount = count;
				memset(mip_data.first_reattempt_date, 0, sizeof(mip_data.first_reattempt_date));
				memset(mip_data.reattempts_date,      0, sizeof(mip_data.reattempts_date));
			}
		}
    }

    if(mip_db_update == true)
    {
    	if(strlen(mip_data.reattempts_date) == 0 && strlen(mip_data.first_reattempt_date) == 0)
    	{
    		ResultCode = db_delete_merchant_integrity_param_mc(&mip_data, &errorMsg);
    		retVal = DELETED;
    	}
    	else
    	{
        	updates_count_mc(&mip_data, deleteCount);
    		ResultCode = db_update_merchant_integrity_param_mc(&mip_data, &errorMsg);
    		retVal = UPDATED;
    	}

    	if(ResultCode != PTEMSG_OK)
    	{
    		sprintf(buffer, "%s Result Code:[%d] %s [%s:%s]",
    						errorMsg,
							ResultCode,
    						(retVal==UPDATED)?" on UPDATE":" on DELETE",
    						mip_data.primary_key.merchant_id,
							mask_card_number(&mip_data.primary_key.card_nbr));
    		mparam_log_message(2, 3, buffer, " verify_and_update_reattempt_date_mc");
    		retVal = ERROR;
    	}
    	else
    	{
			//dbcommon_commit();//cannot commit inside cursor
    		if(retVal == UPDATED)
    		{
    			sprintf(errorMsg, "Updated the MC record in MIP table [%s:%s]",
	    			          	  mip_data.primary_key.merchant_id,
								  mask_card_number(&mip_data.primary_key.card_nbr));
    		}
    		else
    		{
    			sprintf(errorMsg, "Deleted the MC record from MIP table [%s:%s]",
    	    			          mip_data.primary_key.merchant_id,
    							  mask_card_number(&mip_data.primary_key.card_nbr));
    		}
			mparam_log_message(2, 1, errorMsg, " verify_and_update_reattempt_date_mc");
    	}
    }

    return retVal;
}

void updates_count_mc(pMERCH_INT_PARAMS mipData, int delCount)
{
	int c2Count;

	c2Count = atoi(mipData->c2_count);
	c2Count -= delCount;

	if(c2Count > 0)
		sprintf(mipData->c2_count, "%02d", c2Count);
	else
	{
		//memset(mipData->c2_block, 0, sizeof(mipData->c2_block));
		memset(mipData->c2_count, 0, sizeof(mipData->c2_count));
	}

	return;
}

void read_scheme_integrity_category_mc()
{
	int  ResultCode    = 0;
	char errorMsg[256] = {0};

	mparam_log_message(2, 1, "----------------MC : SCHEME_INTEGRITY_CATEGORY----------------", "");
	ResultCode = db_get_scheme_integrity_category_mc(errorMsg);
	if(ResultCode != PTEMSG_OK && ResultCode != PTEMSG_NOT_FOUND)
	{
		//printf("\n%s\n", errorMsg);
		mparam_log_message(2, 3, strcat(errorMsg, "MC"), " read_scheme_integrity_category_mc");
	}
	else if(ResultCode == PTEMSG_NOT_FOUND)
	{
		sprintf(errorMsg, "No records found in SCHEME_INTEGRITY_CATEGORY table for MC");
		mparam_log_message(2, 1, errorMsg, " read_scheme_integrity_category_mc");
		//printf("\n%s\n", errorMsg);
	}

	return;
}
/****************************************MC FUNCTIONS - END***************************************/


/****************************************MC MAC FUNCTIONS - START***************************************/
void mc_mac_checks()
{
	int  ResultCode    = 0;
	int  ResultCode1   = 0;
	char errorMsg[256] = {0};

	mparam_log_message(2, 1, "----------------MC : MERCHANT_ADVICE_CODE_CHECKS----------------", "");
	get_end_date(&macEndDate, 1);
	ResultCode1 = db_delete_merchant_advice_code_records(errorMsg);
	if(ResultCode1 == PTEMSG_OK)
	{
		dbcommon_commit();
		mparam_log_message(2, 1, "Removed the MC Merchant_Advice_Code records from MERCHANT_INTEGRITY_PARAM table",
								 " mc_mac_checks");
	}
}

/****************************************MC MAC FUNCTIONS - END***************************************/


/****************************************COMMON FUNCTIONS - START***************************************/
int merchant_status_update()//common-call once
{
	int  ResultCode    = 0;
	char errorMsg[256] = {0};

	//check whether merchants are unblocked in MIP and then update the MCF division_nbr as '000'
	mparam_log_message(2, 1, "----------------MCF01----------------", "");

	ResultCode = db_verify_and_update_merchant(errorMsg);
	if(ResultCode == PTEMSG_NOT_FOUND)
	{
		mparam_log_message(2, 1, "MCF01 : No merchants to update", " merchant_status_update");
	}
	else if(ResultCode != PTEMSG_OK)
	{
		mparam_log_message(2, 3, errorMsg, " merchant_status_update");
		return PTEMSG_DATABASE_ERROR;
	}
	else
	{
		dbcommon_commit();
	}

	return PTEMSG_OK;
}

char* get_complete_reattempts_date(char *reattsDate, int len)
{
	int i, j;
	char tempDate[10] = {0};
	char currentMonth[3] = {0}, currentYear[5] = {0};
	char temp[5] = {0}, mon[3] = {0};

	memcpy(temp, reattsDate, len);
	memcpy(mon,  temp, 2);
    memcpy(currentYear,  sys_current_date,   4);
    memcpy(currentMonth, sys_current_date+4, 2);

	if(atoi(mon) <= atoi(currentMonth))
	{
		strncpy(&tempDate, currentYear, strlen(currentYear));
		strncat(&tempDate, temp, 		strlen(temp));
	}
	else
	{
		sprintf(&tempDate, "%d", atoi(currentYear)-1);
		strncat(&tempDate, temp, strlen(temp));
	}
	memcpy(&reattsDate, &tempDate, strlen(tempDate));

	return &reattsDate;
}

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

void get_end_date(char* end_date, int days)
{
	char buf_date[9] = {0};
	long julian_date = 0;

	strcpy(buf_date, sys_current_date);
	//convert to julian, subtract and convert back to normal
    julian_date = Txutils_Calculate_Julian_Date ((BYTE*)buf_date);
    julian_date = julian_date - days;
    Txutils_Calculate_Gregorian_Date( julian_date, buf_date);

    memcpy(end_date, buf_date, strlen(buf_date));
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
    CHAR  mparam_error_warning_file_size[5] = {0};

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
								   mparam_error_warning_file_size,
								   sizeof(mparam_error_warning_file_size)-1,
								   filename
								 );

	/* File size size conversion */
	Max_File_Size_Defined = atoi(mparam_error_warning_file_size);
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
 *  NAME:         mparam_log_message
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
void mparam_log_message( INT dest, INT sev, pCHAR msg, pCHAR func)
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

   /* Make sure text message is not too long. */
   memcpy( text_message, msg, sizeof(text_message)-1 );
   mparam_Log_error_warning_to_File(text_message,sev,func);
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

INT mparam_Log_error_warning_to_File(pCHAR Error_Buf, int sev, pCHAR func)
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
   mparam_log_message(2, 1, Buffer, " EndProcess ");
   //printf("%s\n", Buffer );
}
