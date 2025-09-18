/*********************************************************************************

                MERCHBATCH - EFTPOS - UTILITY - THOUGHTFOCUS 


  REVISION HISTORY
   ----------------

    Rev 1.2 Jun 25 2009 Ajay S.T. TF
   Some of the tags was missing in parsing the emv raw data
   Updated version 1.0.0.27

	 Rev 1.1 Jun 22 2009 Ajay S.T. TF
   Issue. MCC field was missing from the segment 10.
   Updated version 1.0.0.26 

   

/********************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <math.h>
#include <limits.h>
#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "ntutils.h"
#include "diners.h"
#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "settle.h"
#include "txutils.h"
#include "dc_database.h"
#include "memmnger.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "dbcommon.h"
#include "timings.h"
#include "string.h"



/***** External variables */

extern volatile INT  EndProcessSignalled;
extern volatile INT  MainProcessDone;
extern int           Start();
extern void          Stop ();

/***** Global variables *****/
UINT Max_File_Size_Defined = 0 ;
static BCH10_LIST    bch10_list;
static BCH11_LIST    bch11_list;

static BCH01_LIST    bch01_list;
static TLOG01_LIST   tlog01_list; /* ThoughtFocus - Chandra */
static EMV_LIST      Emv_list;    /* ThoughtFocus - Chandra */
CHAR	temp_filler[50]={0};
static CHAR          c_record [277];
static CHAR          c_record_cup [277];
static CHAR          c_record_cup_exception[256];
static INT 	         Cup_Settlement_amount_percentage = 0;
static INT 			 CUP_EPOS_Exception_Flag =0;
CHAR Incremental_Auth_flag_CUP[1]={0};

static CHAR          d_record [245];
static CHAR          e_record [122]; 
static CHAR			 dump_record[256];
static CHAR			 r_record[256];

static CHAR  merchbatch_Erro_warning_Filename[256] = {0};
static CHAR  merchbatch_module_error_warning_file_name[256] = {0};
static CHAR  merchbatch_error_warning_file_path[256] = {0};


static TC_TOTALS     tc_totals;
DCF01                terminal;
BCH10				 auth_tx_detail;         
BCH11				 auth_tx_detail_arc;
BCH01			  	 auth_tx_total; 
EMV_RAW              emv_record;  /* ThoughtFocus - Chandra */         
CHAR				 test_temp_msg[15];	

static CHAR          cutoff_date[9]={0};
static CHAR          cutoff_hour[3];
static CHAR          day[4];
static CHAR          day_m[3];
static CHAR          extract_date[7] = "";
static CHAR          hour_offset[3];
static CHAR          minute[3];
static CHAR          month[3];
static CHAR          year[5];
static CHAR          jul_day[4];
static CHAR          offset[3];
static CHAR          process_code[2];
static CHAR          time_tag[20];
static CHAR          yyyymmdd_finish[9];
static CHAR          yyyymmdd_start[9];
static CHAR          path[30] = "";  /*hold root directory path*/
static INT           cre_inc_value=1,deb_inc_value=1,cseg10_records=0,dseg10_records=0,crec_count=0,drec_count=0,E_crec_count=0; /* ThoughtFocus - Chandra - Increment value */
#ifdef WIN32 // TF Phani to build in winodws
static unsigned __int64  ctotal_amt=0,dtotal_amt=0;
#else
static unsigned long long int  ctotal_amt=0,dtotal_amt=0;
#endif

static FILE          *c_file_write = 0;
static FILE          *d_file_write = 0;
static FILE          *e_file_write = 0;
static FILE          *r_file_write = 0,*err_file_write = 0;
static FILE 		   *E_file_write = 0;

static CHAR          base[17];
static CHAR          c_filename[256] = "";       /* credit file name */
static CHAR          d_filename[256] = "";       /* debit  file name */
static CHAR          r_filename[256] = "";       /* report  file name */
static CHAR          e_filename[256] = "";       /* error  file name */
static CHAR 			 E_filename[256] = "";      /* Exception file name*/


static CHAR          dump_filename[256] = "";    /* dump file name */
static FILE          *dumpfile_ptr = 0;	         /* dump file ptr */


static CHAR          dir_str  [128];
static INT           max_tran = 0,len=0;
static double        lenf=0.0;
static INT           credit_txns_exist=0;
static INT           debit_txns_exist=0;
static CHAR          curr_code[4];
static CHAR          device_type[3];
static CHAR 		 cupMerchantID[16];
static BYTE          max_reward[13],emv_data[576];
static CHAR          industry_code[3];			 
extern CHAR          ExeName[100];


       CHAR          EndProcessFlag;
       CHAR          AppName[12];
       CHAR          lError_Msg[300];
       CHAR          Version[] = "ATP_11.12.0";  /* ThoughtFocus - Phani -MC/VISA/JCB POS entry mode changes*/
											 

	   CHAR  emv_seg13[192],emv_seg14[192],emv_seg15[192],temp_filler_emv[500];


	   CHAR  CurrentDate[9],clen[2],elen[2],err_msg[100];

#define EMV_TAG_MAX_LEN 36

/* Database Timing Measurements */
TIMINGS_LIST  TimingStats;
INT           DB_Timing_Flag; /* True = Do DB Timing Stats, False = don't do */
INT           ForceReport;
CHAR          ReportTime[5];

/* Falg for Dulicate Extraction restrict - TF Phani
	DUPLICATE_RESTRICT flag from tf.ini file
	# 1 - Don't allow duplicates in EFTPOS file
	# 0 - Allow dulicates in EFTPOS file	*/
static INT Duplicate_Restrict;

void bin_to_ascii (pCHAR src, pCHAR dst, INT len)
{
   INT  i, j;

   for (i = 0; i < (len*2); i+=2)
   {
       j = (src[i/2] < 0) ? (src[i/2] + 256) : src[i/2];

      if ((j / 16) <= 9) 
        dst[i] = (j / 16) + '0';
      else
        dst[i] = (j/16 - 0x0a)+'A';

      if ((j & 0x0f) <= 9)
        dst[i+1] = (j & 0x0F) + '0'; 
      else
        dst[i+1] = (j & 0x0f) - 0x0a + 'A';    
   }
} 

void strTrim (char* strSource, char* strDestination)
{
	int index1 = 0, index2 = 0, CurrentChar = 0, firstValidCharacter = 0 ;

	while ((CurrentChar = strSource[index1++]) != 0)
	{
		if (CurrentChar > 0x20) // valid character
		{
			firstValidCharacter = 1 ;
			strDestination[index2++] = CurrentChar ;
		}
		else // control character or a space
		{
			if (!firstValidCharacter)
				// skip leading control characters or spaces
				continue ;
			break ;
		}
	}
	strDestination[index2] = 0 ;
} /* strTrim */

INT Check_If_CUP_Transaction_epos (void)
{
	if ((0 == strncmp(auth_tx_detail.product_codes[9].quantity,CUP_CARD_INDICATOR_FLAG,2))||
		(0 == strncmp(auth_tx_detail.product_code,CUP_CARD_INDICATOR_FLAG,2)))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}
INT Check_If_VISA_Transaction (void)
{
	if (auth_tx_detail.card_nbr[0]=='4')
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}
INT Check_If_MC_Transaction (void)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,auth_tx_detail.card_nbr,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	auth_tx_detail.card_nbr[0]=='5')
	{
		return (true) ;
	}
	else if( (MC_bin_value >= MC_Bin_series_low) && (MC_bin_value <= MC_Bin_series_high))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT Check_If_JCB_Transaction (void)
{
	if((auth_tx_detail.card_nbr[0]=='3') &&
	   (auth_tx_detail.card_nbr[1] == '5'))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}
INT Check_If_AMEX_Transaction (void)
{
	if((auth_tx_detail.card_nbr[0]=='3') &&
	   ((auth_tx_detail.card_nbr[1] == '4') ||
	   (auth_tx_detail.card_nbr[1] == '7')))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}
INT Check_If_Diners_Transaction (void)
{
	if((auth_tx_detail.card_nbr[0]=='3') &&
	   ((auth_tx_detail.card_nbr[1] == '0') ||
		(auth_tx_detail.card_nbr[1] == '6') ||
		(auth_tx_detail.card_nbr[1] == '8') ||
		(auth_tx_detail.card_nbr[1] == '9') ))
	{
		return (true) ;
	}
	else if ((auth_tx_detail.card_nbr[0]=='6') &&
			 ((auth_tx_detail.card_nbr[1] == '0') ||
			 (auth_tx_detail.card_nbr[1] == '4') ||
			 (auth_tx_detail.card_nbr[1] == '5')))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}

INT Check_If_VISA_RPS_Transaction (void)
{
	if (auth_tx_detail.card_nbr[0]=='4' &&
		0 == strncmp(auth_tx_detail.product_codes[6].amount, "08", 2))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}

INT Check_If_MC_RPS_Transaction(void)
{
	if (true == Check_If_MC_Transaction() &&
		   0 == strncmp(auth_tx_detail.product_codes[6].amount,"08",2))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT Check_If_VISA_RPS_Offus_Transaction (void)
{
	if(true == Check_If_VISA_RPS_Transaction() &&
		  0 == strncmp(auth_tx_detail.product_codes[5].code, "OFFUS", 5))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}

INT Check_If_MC_RPS_Offus_Transaction(void)
{
	if(true == Check_If_MC_RPS_Transaction() &&
		  0 == strncmp(auth_tx_detail.product_codes[5].code, "OFFUS", 5))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT Check_If_CUP_Transaction_arc_epos (void)
{
	if ((0 == strncmp(auth_tx_detail_arc.product_codes[9].quantity,CUP_CARD_INDICATOR_FLAG,2))||
		(0 == strncmp(auth_tx_detail_arc.product_code,CUP_CARD_INDICATOR_FLAG,2)))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}
INT Check_If_VISA_Transaction_arc (void)
{
	if (auth_tx_detail_arc.card_nbr[0]=='4')
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}
INT Check_If_MC_Transaction_arc (void)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,auth_tx_detail_arc.card_nbr,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	auth_tx_detail_arc.card_nbr[0]=='5')
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
INT Check_If_JCB_Transaction_arc (void)
{
	if((auth_tx_detail_arc.card_nbr[0]=='3') &&
	   (auth_tx_detail_arc.card_nbr[1] == '5'))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT Check_If_AMEX_Transaction_arc (void)
{
	if((auth_tx_detail_arc.card_nbr[0]=='3') &&
	   ((auth_tx_detail_arc.card_nbr[1] == '4') ||
	   (auth_tx_detail_arc.card_nbr[1] == '7')))
	{

		return true ;
	}
	else
	{
		return false;
	}
}

INT Check_If_Diners_Transaction_arc (void)
{
	if((auth_tx_detail_arc.card_nbr[0]=='3') &&
	   ((auth_tx_detail_arc.card_nbr[1] == '0') ||
		(auth_tx_detail_arc.card_nbr[1] == '6') ||
		(auth_tx_detail_arc.card_nbr[1] == '8') ||
		(auth_tx_detail_arc.card_nbr[1] == '9') ))
	{
		return true ;
	}
	else if ((auth_tx_detail_arc.card_nbr[0]=='6') &&
			 ((auth_tx_detail_arc.card_nbr[1] == '0') ||
			 (auth_tx_detail_arc.card_nbr[1] == '4') ||
			 (auth_tx_detail_arc.card_nbr[1] == '5')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}



INT Check_If_VISA_RPS_Transaction_arc (void)
{
	if (auth_tx_detail_arc.card_nbr[0]=='4' &&
		0 == strncmp(auth_tx_detail_arc.product_codes[6].amount, "08", 2))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT Check_If_MC_RPS_Transaction_arc(void)
{

	if (true == Check_If_MC_Transaction_arc() &&
		   0 == strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT Check_If_VISA_RPS_Offus_Transaction_arc (void)
{
	if(true == Check_If_VISA_RPS_Transaction_arc() &&
		  0 == strncmp(auth_tx_detail_arc.product_codes[5].code, "OFFUS", 5))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT Check_If_MC_RPS_Offus_Transaction_arc(void)
{

	if(true == Check_If_MC_RPS_Transaction_arc() &&
		  0 == strncmp(auth_tx_detail_arc.product_codes[5].code, "OFFUS", 5))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*****************************************************************************/
INT Check_If_DCI_pin_block_exists()
{
   INT  ret_val = false;

   if ( auth_tx_detail.pos_entry_mode[3]=='1')
   {
      ret_val = true;
   }

   return( ret_val );
}
/*****************************************************************************/
INT Check_DINERS_emv_capability_device(void)
{
	/*BDO have discarded this functionality so always return fasle*/
	return false;
	if( 0 == strncmp(terminal.current_bus_date + DEVICE_EMV_CAPABILITY_POS_FOR_DINERS,
					 DINERS_EMV_CAPABILITY_INDICATOR,DINERS_DEVICE_EMV_CAPABILITY_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT Check_is_Diners_Magstrip_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx_detail.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP1,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

INT Check_is_Diners_Chip_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx_detail.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_ICC_EMV,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

INT Check_is_Diners_Fallback_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx_detail.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_FALLBACK,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

INT Check_is_Diners_Contactless_Mastripe_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx_detail.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_MAGSTRIPE,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

INT Check_is_Diners_Contactless_Chip_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx_detail.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_EMV,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

INT Check_is_Diners_Manual_pos_transaction(void)
{
	if( 0 ==  strncmp( auth_tx_detail.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_MANUAL,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT Check_is_Diners_Asiapay_Ecommerce_transaction (void)
{
	if( 0 == strncmp( auth_tx_detail.product_codes[6].amount,
					  DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION,
					  DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT Check_is_Diners_Asiapay_Recurring (void)
{
	if( 0 == strncmp(auth_tx_detail.product_codes[6].amount,
					 DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION,
					 DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT Check_is_Diners_BDO_PAY (void)
{
	if( 0 == strncmp(auth_tx_detail.product_codes[6].amount,
					 DINERS_DE25_BDO_PAY_TRANSACTION,
					 DINERS_DE25_BDO_PAY_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

/*******************************************************************************
 * NAME:        Diners_Prepare_posdata_for_DINERS
 *
 * DESCRIPTION: This function prepare POS data for DINERS for
 * 				EPOS if we are not able to retrive from Original
 *
 * INPUTS:      None
 *
 * OUTPUTS:     None
 *
 * RETURNS:     None
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/
Diners_Prepare_posdata_for_DINERS ()
{

	if(true == Check_DINERS_emv_capability_device ())
	{
		if(true == Check_is_Diners_Magstrip_pos_transaction())
		{
			if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Chip_pos_transaction())
		{
			if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Fallback_pos_transaction())
		{
			if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_FALLBACK_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_FALLBACK_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Contactless_Mastripe_pos_transaction())
		{
			if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Contactless_Chip_pos_transaction())
		{
			if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}

#if BDOR_68_REQ_2_DCI

		else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		{
		   strncpy( auth_tx_detail.product_codes[16].amount,
					DCI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
		else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		{
		   strncpy( auth_tx_detail.product_codes[16].amount,
					DCI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
#if BDOR_68_REQ_4_DCI

		else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin())
		{
		   strncpy( auth_tx_detail.product_codes[16].amount,
					DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
		else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin())
		{
		   strncpy( auth_tx_detail.product_codes[16].amount,
					DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
#if BDOR_68_REQ_6_DCI
		else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trxn())
		{
		   strncpy( auth_tx_detail.product_codes[16].amount,
					DCI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
		else if(true == Check_is_Diners_Manual_pos_transaction())
		{
			if(true == Check_is_Diners_Asiapay_Ecommerce_transaction())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_ASIAPAY_ECOM,
						DINERS_DE22_SIZE);
			}
			else if (true == Check_is_Diners_Asiapay_Recurring())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_RPS,
						DINERS_DE22_SIZE);
			}
			else if (true == Check_is_Diners_BDO_PAY())
			{
				// POS data for RPS and Asiapay is same.
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_FOR_BDO_PAY,
						DINERS_DE22_SIZE);
			}
			else if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MANUAL_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MANUAL_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else
		{
			strncpy(auth_tx_detail.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_UNKNOWN,
					DINERS_DE22_SIZE);
		}
	}
	else
	{
		if(true == Check_is_Diners_Magstrip_pos_transaction())
		{
			if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Chip_pos_transaction())
		{
			if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITHOUT_PIN ,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Fallback_pos_transaction())
		{
			if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Contactless_Mastripe_pos_transaction())
		{
			if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Contactless_Chip_pos_transaction())
		{
			if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
#if BDOR_68_REQ_2_DCI

		else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		{
		   strncpy( auth_tx_detail.product_codes[16].amount,
					DCI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
		else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		{
		   strncpy( auth_tx_detail.product_codes[16].amount,
					DCI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
#if BDOR_68_REQ_4_DCI

		else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin())
		{
		   strncpy( auth_tx_detail.product_codes[16].amount,
					DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
		else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin())
		{
		   strncpy( auth_tx_detail.product_codes[16].amount,
					DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
#if BDOR_68_REQ_6_DCI
		else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trxn())
		{
		   strncpy( auth_tx_detail.product_codes[16].amount,
					DCI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
		else if(true == Check_is_Diners_Manual_pos_transaction())
		{
			if(true == Check_is_Diners_Asiapay_Ecommerce_transaction())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_ECOM,
						DINERS_DE22_SIZE);
			}
			else if (true == Check_is_Diners_Asiapay_Recurring())
			{
				// POS data for RPS and Asiapay is same.
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_RPS,
						DINERS_DE22_SIZE);
			}
			else if (true == Check_is_Diners_BDO_PAY())
			{
				// POS data for RPS and Asiapay is same.
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_FOR_BDO_PAY,
						DINERS_DE22_SIZE);
			}
			else if(true == Check_If_DCI_pin_block_exists())
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else
		{
			strncpy(auth_tx_detail.product_codes[16].amount,
					DCI_POS_DATA_DC_NOT_PRESENT_UNKNOWN,
					DINERS_DE22_SIZE);
		}
	}
}


/*********************************************************/

INT Check_is_Diners_Magstrip_pos_transaction_arc(void)
{
	if( 0 ==  strncmp( auth_tx_detail_arc.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_MAGSTRIP1,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

INT Check_is_Diners_Chip_pos_transaction_arc(void)
{
	if( 0 ==  strncmp( auth_tx_detail_arc.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_ICC_EMV,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

INT Check_is_Diners_Fallback_pos_transaction_arc(void)
{
	if( 0 ==  strncmp( auth_tx_detail_arc.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_FALLBACK,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

INT Check_is_Diners_Contactless_Mastripe_pos_transaction_arc(void)
{
	if( 0 ==  strncmp( auth_tx_detail_arc.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_MAGSTRIPE,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

INT Check_is_Diners_Contactless_Chip_pos_transaction_arc(void)
{
	if( 0 ==  strncmp( auth_tx_detail_arc.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_CONTACTLESS_EMV,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	return false;
}

INT Check_is_Diners_Manual_pos_transaction_arc(void)
{
	if( 0 ==  strncmp( auth_tx_detail_arc.pos_entry_mode,
					   DINERS_BDO_POS_ENTRY_MODE_MANUAL,
					   DINERS_BDO_POS_ENTRY_MODE_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT Check_is_Diners_Asiapay_Ecommerce_transaction_arc (void)
{
	if( 0 == strncmp( auth_tx_detail_arc.product_codes[6].amount,
					  DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION,
					  DINERS_DE25_ASIAPAY_E_COMMERCE_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT Check_is_Diners_Asiapay_Recurring_arc (void)
{
	if( 0 == strncmp(auth_tx_detail_arc.product_codes[6].amount,
					 DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION,
					 DINERS_DE25_ASIAPAY_RECURRING_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

INT Check_is_Diners_BDO_PAY_arc (void)
{
	if( 0 == strncmp(auth_tx_detail_arc.product_codes[6].amount,
					 DINERS_DE25_BDO_PAY_TRANSACTION,
					 DINERS_DE25_BDO_PAY_TRANSACTION_INDICATOR_LEN))
	{
		return true;
	}
	else
	{
		return false;
	}

}

/*****************************************************************************/
INT Check_If_DCI_pin_block_exists_arc()
{
   INT  ret_val = false;

   if ( auth_tx_detail_arc.pos_entry_mode[3]=='1')
   {
      ret_val = true;
   }

   return( ret_val );
}
/*****************************************************************************/

/*******************************************************************************
 * NAME:        Diners_Prepare_posdata_for_DINERS_arc
 *
 * DESCRIPTION: This function prepare POS data for DINERS for
 * 				EPOS if we are not able to retrive from Original
 *
 * INPUTS:      None
 *
 * OUTPUTS:     None
 *
 * RETURNS:     None
 *
 * AUTHOR:      Abhishek Verma
 ******************************************************************************/
Diners_Prepare_posdata_for_DINERS_arc ()
{

	if(true == Check_DINERS_emv_capability_device ())
	{
		if(true == Check_is_Diners_Magstrip_pos_transaction_arc())
		{
			if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Chip_pos_transaction_arc())
		{
			if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CHIP_DIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Fallback_pos_transaction_arc())
		{
			if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_FALLBACK_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_FALLBACK_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Contactless_Mastripe_pos_transaction_arc())
		{
			if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Contactless_Chip_pos_transaction_arc())
		{
			if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
#if BDOR_68_REQ_2_DCI

		else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin_arc())
		{
		   strncpy( auth_tx_detail_arc.product_codes[16].amount,
				   	DCI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
		else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin_arc())
		{
		   strncpy( auth_tx_detail_arc.product_codes[16].amount,
				    DCI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
#if BDOR_68_REQ_4_DCI

		else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin_arc())
		{
		   strncpy( auth_tx_detail_arc.product_codes[16].amount,
				    DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
		else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin_arc())
		{
		   strncpy( auth_tx_detail_arc.product_codes[16].amount,
				    DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
#if BDOR_68_REQ_6_DCI
		else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trx_arc())
		{
			strncpy( auth_tx_detail_arc.product_codes[16].amount,
					DCI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
		else if(true == Check_is_Diners_Manual_pos_transaction_arc())
		{
			if(true == Check_is_Diners_Asiapay_Ecommerce_transaction_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_ASIAPAY_ECOM,
						DINERS_DE22_SIZE);
			}
			else if (true == Check_is_Diners_Asiapay_Recurring_arc())
			{
				// POS data for RPS and Asiapay is same.
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_RPS,
						DINERS_DE22_SIZE);
			}
			else if (true == Check_is_Diners_BDO_PAY_arc())
			{
				// POS data for RPS and Asiapay is same.
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_FOR_BDO_PAY,
						DINERS_DE22_SIZE);
			}
			else if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MANUAL_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_PRESENT_MANUAL_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else
		{
			strncpy(auth_tx_detail_arc.product_codes[16].amount,
					DCI_POS_DATA_DC_PRESENT_UNKNOWN,
					DINERS_DE22_SIZE);
		}
	}
	else
	{
		if(true == Check_is_Diners_Magstrip_pos_transaction_arc())
		{
			if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Chip_pos_transaction_arc())
		{
			if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CHIP_DIP_WITHOUT_PIN ,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Fallback_pos_transaction_arc())
		{
			if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_FALLBACK_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Contactless_Mastripe_pos_transaction_arc())
		{
			if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_MAGSTRIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else if(true == Check_is_Diners_Contactless_Chip_pos_transaction_arc())
		{
			if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_CONTACTLESS_CHIP_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
#if BDOR_68_REQ_2_DCI

		else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin_arc())
		{
		   strncpy( auth_tx_detail_arc.product_codes[16].amount,
				   	DCI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
		else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin_arc())
		{
		   strncpy( auth_tx_detail_arc.product_codes[16].amount,
				    DCI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
#if BDOR_68_REQ_4_DCI

		else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin_arc())
		{
		   strncpy( auth_tx_detail_arc.product_codes[16].amount,
				    DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
		else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin_arc())
		{
		   strncpy( auth_tx_detail_arc.product_codes[16].amount,
				    DCI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
#if BDOR_68_REQ_6_DCI
		else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trx_arc())
		{
			strncpy( auth_tx_detail_arc.product_codes[16].amount,
					DCI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					DCI_POS_DATA_LEN_FOR_EPOS );
		}
#endif
		else if(true == Check_is_Diners_Manual_pos_transaction_arc())
		{
			if(true == Check_is_Diners_Asiapay_Ecommerce_transaction_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_ASIAPAY_ECOM,
						DINERS_DE22_SIZE);
			}
			else if (true == Check_is_Diners_Asiapay_Recurring_arc())
			{
				// POS data for RPS and Asiapay is same.
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_RPS,
						DINERS_DE22_SIZE);
			}
			else if (true == Check_is_Diners_BDO_PAY_arc())
			{
				// POS data for RPS and Asiapay is same.
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_FOR_BDO_PAY,
						DINERS_DE22_SIZE);
			}
			else if(true == Check_If_DCI_pin_block_exists_arc())
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITH_PIN,
						DINERS_DE22_SIZE);
			}
			else
			{
				strncpy(auth_tx_detail_arc.product_codes[16].amount,
						DCI_POS_DATA_DC_NOT_PRESENT_MANUAL_WITHOUT_PIN,
						DINERS_DE22_SIZE);
			}
		}
		else
		{
			strncpy(auth_tx_detail_arc.product_codes[16].amount,
					DCI_POS_DATA_DC_NOT_PRESENT_UNKNOWN,
					DINERS_DE22_SIZE);
		}
	}
}


/*****************************************************************************
NAME:			Convert_Amt_To_Auth_Tx_String_Format
DESCRIPTION:	Converts from a double value to a string value
				
INPUTS:			
				nAmount:	the value to be converted in double
OUTPUTS:		strAmount	the pointer to the string that contains the converted amount
RETURNS:        false  if error
                true   if no error
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*****************************************************************************/
int Convert_Amt_To_Auth_Tx_String_Format 
                              (double nAmount, char* strAmount)
{
	char strTemp[256] = {0} ;
	int  nStringLength = 0 ;
	int  nIndex = 0 ;

	gcvt (nAmount*100, 15, strTemp) ; // choice of 15 is arbitrary

	nStringLength = strlen (strTemp) ;
	// Discard all characters after the decimal point, if present
	for (nIndex = 0; nIndex < nStringLength ; nIndex++)
	{
		if ('.' == strTemp[nIndex])
		{
			strTemp[nIndex] = 0 ;
			break ;
		}
	}

	strcpy (strAmount, strTemp) ;
	return 1 ;
} /* Convert_Amt_To_Auth_Tx_String_Format */


void LeftFill_String_With_Character (unsigned int destination_array_size,
                                     char* strAmount,
                                     unsigned char fill_character) 
{
   unsigned char strTemp[256] = {0} ;
   int len = 0 ;
   memset (strTemp, fill_character, sizeof strTemp) ;
   
   len = strlen (strAmount) ;
   strTemp [destination_array_size - len] = 0 ;
   strcat (strTemp, strAmount) ;
   strcpy (strAmount, strTemp) ;
}


/*******************************************************************************
*
*  FUNCTION:         main
*
*  DESCRIPTION:      This function gets the initial parameters.
*
*  INPUTS:           INT argc - number of parameters
*                    pCHAR argv[1] - path and filename 
*                  
*  OUTPUTS:          None
*
*  RETURN VALUE:     None 
*   
*******************************************************************************/
void main(int argc, char *argv[])
{

   CHAR correct_hours [24] [3] = {"00", "01", "02", "03", "04", "05",
                                  "06", "07", "08", "09", "10", "11",
                                  "12", "13", "14", "15", "16", "17",
                                  "18", "19", "20", "21", "22", "23"};

   CHAR correct_files [16] [7] = {"ALL", "all", "CREDIT", "credit", 
                                  "DEBIT", "debit", "RAFFLE", "raffle", 
                                  "A", "a", "C", "c", "D", "d", "R", "r"};

   CHAR correct_codes [16] [2] = {"A", "A", "C", "C", "D", "D", "R", "R",
                                  "A", "A", "C", "C", "D", "D", "R", "R"};

   int               hour_index;
   int               file_index;

   /*** Initialize global variables ***/
   memset(base,         0, sizeof(base));
   memset(cutoff_date,  0, sizeof(cutoff_date));
   memset(cutoff_hour,  0, sizeof(cutoff_hour));
   memset(offset,       0, sizeof(offset));

   EndProcessFlag = false;
	#ifndef WIN32
		strcpy(ExeName, argv[0]);
	#endif
   /*********************************************************
   *
   *  There are 5 required command-line parameters:
   *     Parameter 0    -  executable program name
   *     Parameter 1    -  settle file base file name 
   *     Parameter 2    -  cutoff date (yyyymmdd) or comma (',')
   *     Parameter 3    -  cutoff hour 
   *     Parameter 4    -  number of hours to look back
   *
   *  There is 1 optional command-line parameter:
   *     Parameter 5    -  file to process
   *
   **********************************************************/

   if (argc != 7) 
   {
      display_settle_info();
      return;
   }

   if (strcmp(argv[1], "?") == 0)
   {
      display_settle_info();
      return;
   }

   /*** 1st Parameter - base file name ***/
   if (strlen(argv[1]) > 16)
	{
      printf("\n\n");
	   printf("Error in 1st parameter:\n");
      printf("Base of filename must be less than 17 characters.\n");
      printf("\n");
      return;
	}
   else
      strcpy(base, argv[1]);  

   /*** 2nd Parameter - cutoff date ***/
   if (strcmp(argv[2], ",") != 0)
   {
      if (strlen(argv[2]) != 8)
      {
         printf("\n\n");
         printf("Error in 2nd parameter.\n");
         printf("Date must be in YYYYMMDD format.\n");
         printf("\n");
         return;
      }
   }
   strcpy(cutoff_date, argv[2]);

   /*** 3rd Parameter - cutoff hour ***/
   if (strlen(argv[3]) > 2)
   {
      printf("\n\n");
      printf("Error in 3rd parameter.\n");
      printf("Hour must be in HH format.\n");
      printf("\n");
      return;
   }

   if (strlen(argv[3]) == 1)
   {
      strcpy(cutoff_hour, "00");
      strncpy(cutoff_hour + 1, argv[3], 1);
   }
   else
      strcpy(cutoff_hour, argv[3]);

   for (hour_index = 0; hour_index < 24; hour_index++)
   {
      if (strcmp(cutoff_hour, correct_hours[hour_index]) == 0)
         break;
   }

   if (hour_index == 24)
   {
      printf("\n\n");
      printf("Error in 3rd parameter.\n");
      printf("Invalid hour.\n");
      printf("\n");
      return;
   }

   /*** 4th Parameter - hours back from cutoff hour (/t) ***/
   if (strlen(argv[4]) > 2)
   {
      printf("\n\n");
      printf("Error in 4th parameter.\n");
      printf("Hours back from cutoff time must be between 00 and 99.\n");
      printf("\n");
      return;
   }

   if (strlen(argv[4]) == 1)
   {
      strcpy(offset, "00");
      strncpy(offset + 1, argv[4], 1);
   }
   else
      strcpy(offset, argv[4]);

   /*** 5th Parameter - files to be generated (mandatory) ***/
   if (strcmp(argv[5], " ") != 0)
   {
      for (file_index = 0; file_index < 16; file_index++)
      {
         if (strcmp(argv[5], correct_files[file_index]) == 0)
            break;
      }

      if (file_index == 16)
      {
         printf("\n\n");
         printf("Error in 5th parameter.\n");
         printf("Invalid file to process.\n");
         printf("\n");
         return;
      }
      else
      {
         strcpy(process_code, correct_codes[file_index]);
      }
   }
   else
      strcpy(process_code, "A");

   /*** 6th Parameter - number of runs  (mandatory) ***/

   if (argc==7)
   {
      cre_inc_value=atoi(argv[6]);
	  deb_inc_value=atoi(argv[6]);
   }

   if(!Start())
	   printf("Error starting ServiceManager\n\n" );
   	 
	Stop();

} /* main */

void create_reports();

/*******************************************************************************
*
*  FUNCTION:         MainProcessor
*
*  DESCRIPTION:      This function performs initial application setup.
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     None
* 
*******************************************************************************/
void MainProcessor(void)
{
   CHAR              buffer[500];
   CHAR              err_msg[100];
   CHAR              time_date[25] = "";
   INT               count_tran = 0;
   INT               next_portion = 0; 
   INT               num_returned = 0;
   INT               ret_code = 0;   
   BYTE              bch01_buffer[sizeof(AUTH_TX) + sizeof(BCH01)];
   INT               num_sql;
   INT               err_ctr = 0;
   

   /* Clear buffer (set to null) */
   memset(&bch01_buffer,0, sizeof(bch01_buffer));
   memset (&lError_Msg, 0x00, sizeof (lError_Msg));
   memset (&buffer, 0x00, sizeof(buffer));
  
   GetAppName (AppName);
   merchbatch_get_error_warning_file_name_path();
   sprintf( buffer,
           "Starting EFTPOS: %s,  version %s", AppName, Version );
   merchbatch_log_message( 2, 1, buffer, "MainProcessor", 0);

   /* Send message to DOS window in debug mode */
   strcat(buffer,"\n");
   PRINT(buffer);

   /* Get root directory path */
   memset (path, 0x00, sizeof(path));
   GetAscendentRootDirectory(path);

   /* Get Database Timing Parameters to gather statistics about each query. */
   memset( ReportTime, 0x00,   sizeof(ReportTime) );
   memset( buffer,     0x00, sizeof(buffer)     );
   ret_code = get_timing_parameters( &DB_Timing_Flag, ReportTime, &ForceReport, buffer );
   merchbatch_log_message( 2, 1, buffer, "MainProcessor", 0);
	/* Read Duplicate Extraction flag from  TF.ini . 
    If flag is 0 - duplicates are allowed 1- Duplicates are not allowed*/
	Get_duplicate_restrict_flag_from_ini();
	Get_Incremental_Auth_flag_CUP_from_ini();
	/* Get Epos Exception flag for CUP	*/
	Get_Epos_Exception_CUP_flag_from_ini();
	if(CUP_EPOS_Exception_Flag == 1)
	{
		/*Epos Exception flag for CUP is enable , let get EPOS Settle percentage Value */
		Get_EPOS_Settle_percentage_Value_CUP_from_ini();
	}

   /* Initialize the DB Timing Statistics Data Structure. */
   if ( DB_Timing_Flag == true )
   {
      reset_timing_stats( &TimingStats );
   }

   /* Try to connect to the database */
#ifdef _DEBUG
   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", err_msg) != PTEMSG_OK)
#else
   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", err_msg) != PTEMSG_OK)
#endif
   {
      memset(&buffer, 0x00, sizeof(buffer));
	  strcpy(buffer, "Database Connect Error:  ");
	  strcat(buffer, err_msg);
      merchbatch_log_message( 1, 3, buffer, "MainProcessor", 0 );
	  write_to_dump_file(buffer);
	   /* pteipc_shutdown_single_instance_app(); */
	  MainProcessDone = 1;

       return;
   }

   /* Send message to system monitor */
   strcpy(buffer,"Connected to ORACLE ");
   merchbatch_log_message( 2, 1, buffer, "MainProcessor", 0 );
   /* Send message to DOS window in debug mode */
   strcat(buffer,"\n");
   PRINT(buffer);

   /* Set cutoff date and time */
   set_date();

   /* Set dates for Exception processing. */
   ptetime_get_timestamp( time_date );  /* YYYY-MM-DD-hh.mm.ss.jjj */
   get_date( time_date, CurrentDate );  /* YYYYMMDD */   

   strcpy(CurrentDate,CurrentDate+2);     

    

   /* If we are going to generate all files or just credit file */
   if ((process_code[0] == 'A') || (process_code[0] == 'C'))
   {
      if (open_credit_file() == false)
      {
         /* Send message to system monitor */
         strcpy(buffer,"Stopping settlement ");
         merchbatch_log_message( 2, 1, buffer, "MainProcessor", 0 );
         /* Send message to DOS window in debug mode */
         strcat(buffer,"\n");
         PRINT(buffer);

         /* pteipc_shutdown_single_instance_app(); */
         MainProcessDone = 1;
         return;
      }
	  /*Check if expection flag is enable then open exception file	  */
	  if(CUP_EPOS_Exception_Flag == 1)
	  {
		/*Epos Exception flag for CUP is enable , let open/create exception file */
		if (open_exception_file() == false)
	  	{
	  	      		return (false);
	  	}
		else
		{
			if (create_write_exception_headers() == false)
	  		{
      			return (false);
	  		}
		}
	 }

   }

   /* If we are going to generate all files or just debit file */
   if ((process_code[0] == 'A') || (process_code[0] == 'D'))
   {
      if (open_debit_file() == false)
      {
         /* Send message to system monitor */
         strcpy(buffer,"Stopping settlement ");
         merchbatch_log_message( 2, 1, buffer, "MainProcessor", 0 );
         /* Send message to DOS window in debug mode */
         strcat(buffer,"\n");
         PRINT(buffer);
   
         /* pteipc_shutdown_single_instance_app(); */
         MainProcessDone = 1;
         return;
      }	
   }  
   
   memset(c_record, 0, sizeof(c_record));
   memset(d_record, 0, sizeof(d_record));

   create_headers();

   /*** Write header records to credit and debit file ***/
   if (((process_code[0] == 'A') || (process_code[0] == 'C')))     
   {
     write_to_credit_settle_file();    
   }
   if (((process_code[0] == 'A') || (process_code[0] == 'D'))) 
   {
	  write_to_debit_settle_file();         
   } 

   memset(c_record, 0, sizeof(c_record));
   memset(d_record, 0, sizeof(d_record));

   
      
   /*** Set initial data for BCH01 table ***/
   memset(&auth_tx_total, 0, sizeof(auth_tx_total));
   memset(&tc_totals, 0, sizeof(tc_totals));
   strcpy(auth_tx_total.primary_key.device_id, " ");
   strcpy(auth_tx_total.primary_key.batch_nbr, " ");
   next_portion = 0;

   strcpy(auth_tx_total.open_date, yyyymmdd_start);
   strcpy(auth_tx_total.release_date, yyyymmdd_finish);
   strcpy(auth_tx_total.open_time, offset);
   strcat(auth_tx_total.open_time, "0000");
   strcpy(auth_tx_total.release_time, cutoff_hour);
   strcat(auth_tx_total.release_time, "0000");  


   /* Process all batches identified in the bch01 database table */
   if (strcmp(auth_tx_total.release_date,auth_tx_total.open_date)==0)  /* If open_date and release_date both are same */
   {

	   if (ret_code =  (db_get_bch01_service_list(&auth_tx_total, &bch01_list, err_msg)) != PTEMSG_OK)
	   {
		   sprintf (buffer, "Error processing bch01 batches.");
		   sprintf(lError_Msg,"db_get_bch01_service_list: %s",buffer);
		   merchbatch_log_message( 2, 3, lError_Msg, "MainProcessor", 0);

	   }
   }
   else /* If open_date and release_date both are different */
   {

		if (ret_code =  (db_get_bch01_service_list1(&auth_tx_total, &bch01_list, err_msg)) != PTEMSG_OK)
	   {
		   sprintf (buffer, "Error processing bch01 batches.");
		   sprintf(lError_Msg,"db_get_bch01_service_list: %s",buffer);
		   merchbatch_log_message( 2, 3, lError_Msg, "MainProcessor", 0);

	   }
   }
   memset(c_record, 0, sizeof(c_record));
   memset(d_record, 0, sizeof(d_record));
    
   create_trailers();

   if (((process_code[0] == 'A') || (process_code[0] == 'C')))     
   {
     write_to_credit_settle_file();    
   }
   if (((process_code[0] == 'A') || (process_code[0] == 'D'))) 
   {
	  write_to_debit_settle_file();         
   } 

   if ((process_code[0] == 'A') || (process_code[0] == 'C'))
      fclose (c_file_write);
   if ((process_code[0] == 'A') || (process_code[0] == 'D'))
      fclose (d_file_write);

   create_reports();
  if(CUP_EPOS_Exception_Flag == 1)
  {
  	if (create_trailer_exception_headers() == false)
	{
		return (false);
	}
  }
 
   dbcommon_disconnect (err_msg);

   /* Report the DB timing measurement statistics before exiting. */
   num_sql = log_timing_statistics_report( &TimingStats );

   memset( buffer, 0x00, sizeof(buffer) );
   sprintf( buffer,
           "%s logged stats for %d SQLs",
            AppName, num_sql );
   merchbatch_log_message( 2, 1, buffer, "MainProcessor", 0);
   /* Send message to system monitor */
   strcpy(buffer,"Stopping settlement ");
   merchbatch_log_message( 2, 1, buffer, "MainProcessor", 0 );

   /* Send message to DOS window in debug mode */
   strcat(buffer,"\n");
   PRINT(buffer);

   /* pteipc_shutdown_single_instance_app(); */
   MainProcessDone = 1;
           
   return;  
   
} /* MainProcessor */
//#if 0
/******************************************************************************
 *
 *  NAME:         Get_duplicate_restrict_flag_from_ini
 *
 *  DESCRIPTION:  This function reads information whether dpulicate restrict flag is enabled.
 *
 *  INPUTS:       None
 *
 *  RTRN VALUE:   1 for enabled; 0 for disabled
 *
 *  AUTHOR:       TF PHANI - ThoughtFocus 
 *
 ******************************************************************************/
void Get_duplicate_restrict_flag_from_ini( void )
{
	#define MAX_INI_INFO_SIZE   20
   #define MAX_FILE_NAME_SZIE 128

	CHAR filename     [MAX_FILE_NAME_SZIE]={0};
	CHAR tmpstr       [MAX_FILE_NAME_SZIE]={0};

	CHAR tmpstring    [MAX_INI_INFO_SIZE]={0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]={0};

   INT nARState_temp   = 0;
   INT nARState_retval = 1;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   /* See if duplicate flag is turned on */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
#ifndef WIN32
   GetPrivateProfileString(
             "EFTPOS",                   /* points to section name       */
             "DUPLICATE_RESTRICT",                         /* points to key name           */
             "1",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );
#endif
   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   Duplicate_Restrict = atoi(tmpstring);

   return;
}
//#endif

/******************************************************************************
 *
 *  NAME:         Get_Epos_Exception_CUP_flag_from_ini
 *
 *  DESCRIPTION:  This function reads information whether Epos Exception flag is Enable for CUP or Not restrict flag is enabled.
 *
 *  INPUTS:       None
 *
 *  RTRN VALUE:   1 for enabled; 0 for disabled
 *
 *  AUTHOR:       TF Abhishek - ThoughtFocus 
 *
 ******************************************************************************/
void Get_Epos_Exception_CUP_flag_from_ini( void )
{
	#define MAX_INI_INFO_SIZE   20
	#define MAX_FILE_NAME_SZIE 128

	CHAR filename     [MAX_FILE_NAME_SZIE]={0};
	CHAR tmpstr       [MAX_FILE_NAME_SZIE]={0};

	CHAR tmpstring    [MAX_INI_INFO_SIZE]={0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]={0};

	INT nARState_temp   = 0;
	INT nARState_retval = 1;

	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

	/* See if duplicate flag is turned on */
	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
#ifndef WIN32
	GetPrivateProfileString(
					"EFTPOS",         /* points to section name       */
					"CUP_EXP_FLAG",            /* points to key name           */
					"0",                            /* points to default string     */
					ini_info,                     /* points to destination buffer */
					MAX_INI_INFO_SIZE - 1,      /* size of destination buffer   */
					filename                      /* points to ini filename       */
								);
#endif
	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

	CUP_EPOS_Exception_Flag = atoi(tmpstring);

	return;
}


void Get_Incremental_Auth_flag_CUP_from_ini( void )
{
	#define MAX_INI_INFO_SIZE   20
	#define MAX_FILE_NAME_SZIE 128

	CHAR filename     [MAX_FILE_NAME_SZIE]={0};
	CHAR tmpstr       [MAX_FILE_NAME_SZIE]={0};

	CHAR tmpstring    [MAX_INI_INFO_SIZE]={0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]={0};

	INT nARState_temp   = 0;
	INT nARState_retval = 1;

	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

	/* See if duplicate flag is turned on */
	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
#ifndef WIN32
	GetPrivateProfileString(
					"VALIDATION_INCREMENTAL_PREAUTH_FLAG_CUP",         /* points to section name       */
					"INCREMENTAL_PREAUTH_FLAG_CUP",            /* points to key name           */
					"",                            /* points to default string     */
					ini_info,                     /* points to destination buffer */
					MAX_INI_INFO_SIZE - 1,      /* size of destination buffer   */
					filename                      /* points to ini filename       */
								);
#endif
	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( Incremental_Auth_flag_CUP, tmpstring, 1 );

	return;
}
/******************************************************************************
 *
 *  NAME:         Get_EPOS_Settle_percentage_Value_CUP_from_ini
 *
 *  DESCRIPTION:  This function reads information of EPOS SETTLE PERCENTAGE VALUE.
 *
 *  INPUTS:       None
 *
 *  RTRN VALUE:   1 for enabled; 0 for disabled
 *
 *  AUTHOR:       TF Abhishek - ThoughtFocus 
 *
 ******************************************************************************/
void Get_EPOS_Settle_percentage_Value_CUP_from_ini( void )
{
	#define MAX_INI_INFO_SIZE   20
	#define MAX_FILE_NAME_SZIE 128

	CHAR filename     [MAX_FILE_NAME_SZIE]={0};
	CHAR tmpstr       [MAX_FILE_NAME_SZIE]={0};

	CHAR tmpstring    [MAX_INI_INFO_SIZE]={0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]={0};

	INT nARState_temp   = 0;
	INT nARState_retval = 1;

	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

	/* See if duplicate flag is turned on */
	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
#ifndef WIN32
	GetPrivateProfileString(
					"EFTPOS",                   /* points to section name       */
					"CUP_SETL_PER_VALUE",                         /* points to key name           */
					"0",                            /* points to default string     */
					ini_info,                       /* points to destination buffer */
					MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
					filename                        /* points to ini filename       */
							);
#endif
	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

	Cup_Settlement_amount_percentage = atoi(tmpstring);

	return;
}

/*****************************************************************************
*
*  FUNCTION:         calc_julian_day
*
*  DESCRIPTION:      This function will calculate the julian day
*
*  INPUTS:           date_str -  input string format (YYYYMMDD)
*
*  OUTPUTS:          julian_str - output string format (DDD)
*
*  RETURN VALUE:     None  
*
******************************************************************************/
void calc_julian_day(char date_str[], char julian_str[])
{
   char              year[5];
   char              month[3];
   char              day[3];
   int               iYear;
   int               iMonth;
   int               iDay;
   int               julian_day;
   int               i;
   int               DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

   memset (&year,  0, sizeof(year));
   memset (&month, 0, sizeof(month));
   memset (&day,   0, sizeof(day));
   memcpy (&year,  &date_str[0], 4);
   memcpy (&month, &date_str[4], 2);
   memcpy (&day,   &date_str[6], 2);

   iYear  = atoi(year);
   iMonth = atoi(month);
   iDay   = atoi(day);

   for(i=0,julian_day=0; i<iMonth-1 && i<12; i++)
   {
		julian_day += DaysOfMonth[i];
   }
   julian_day += iDay;

   if (iMonth > 2)
      if (0 == (iYear % 4))
          julian_day++;
   
   itoa(julian_day, julian_str, 10);

   Rj_with_lead_zeros (julian_str, 3);

} /* calc_julian_day */

/************************* BIN 2 ASCII ****************************************/

void genutil_bin_to_ascii (BYTE bstring[], INT blen, pCHAR astr, INT astr_len)
{
   INT  i = 0;

   memset (astr, 0x00, astr_len);

   for (i = 0; i < (blen * 2); i+=2)
   {
      if ((bstring[i/2] / 16) <= 9) 
        astr[i] = (bstring[i/2] / 16) + '0';
      else
        astr[i] = (bstring[i/2] / 16 - 0x0a)+'A';

      if ((bstring[i/2] & 0x0f) <= 9)
        astr[i+1] = (bstring[i/2] & 0x0F) + '0'; 
      else
        astr[i+1] = (bstring[i/2] & 0x0F) - 0x0a + 'A';    
   }
} /* genutil_bin_to_ascii */

/*****************************************************************************/


/*******************************************************************************
*
*  FUNCTION:         create_credit_detail
*
*  DESCRIPTION:      This function creates detail structure for 
*                    the credit settlement file. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  detail structure 
*
*  RETURN VALUE:     True if record should be written to file, else false
*
*   
*******************************************************************************/
INT create_credit_detail()
{
   INT   max_etran = 0;
   INT   i = 0;
   INT   elength = 0;
   BYTE  term_id[16] = {0};
   CHAR  cad_merchant_id[12] = {0};
   CHAR  tran_dt[7]={0} ;
   BYTE  pos_data[13] = {0};
   INT   iIndustry=0;
   BYTE  eci[4]={0};
   BYTE  tran_id[16] = {0};
   BYTE  settle_date[5] = {0};
   BYTE	 temp_entry_mode[4] = {0};
   BYTE  card_nbr[20] = {0};
   BYTE  pan_sequence_number[4]={0};
   BYTE  perch_id[PERCH_ID_LENGTH]={0}; /* ID to store De63 table id -12 of messages 27 charcters- TF Phani*/
   CHAR  temp_message_type[5]={0};
   CHAR  CUP_Pos_input_mode[4]={0};
   CHAR  CUP_trace_ID[7]={0};
   CHAR  temp_DCI_POS_DATA[13] = {0};
   CHAR  temp_Amex_POS_DATA[13] = {0};
   BYTE  temp_DCI_tran_id[16]={0};
   CHAR  temp_service_code[4]= {0};
   CHAR  service_code [4]= {0};
   CHAR  temp_processing_code[7] = {0};
   char  buff[512] = {0};
   INT   cardlen = 0;
   CHAR  tempcard[21] = {0};
   INT   nIndex = 0;
   CHAR  ErrorMsg[512] = {0};
   INT   returncode = 0;
   INT   db_retrive_flag = 0;
   BYTE  trsn_intn_mode[2]	= {0};
   BYTE	 cit_mit_indr[5] 	= {0};
   BYTE  format_code [3]	= {0};
   BYTE	 rpymt_type	 [2]	= {0};
   BYTE  rpymt_amt_indr[2]	= {0};
   BYTE  rpymt_nbr[3]		= {0};
   BYTE  rpymt_freq[3]		= {0};
   BYTE  rr_nbr[36]			= {0};
   BYTE  max_rpymt_amt[13]	= {0};
   BYTE	 valdn_indr[2]		= {0};
   CHAR  add_token_resp[2]  = {0};
   CHAR  tran_link_id[25] 	= {0};
   BYTE  plan_reg_sys_idntfr[11]= {0};
   BYTE	 authn_char_indcr[2] = {0};
   BYTE  authn_resp_code[3]  = {0};
   BYTE  card_hldr_idntfn_method [2] = {0};
   BYTE  cash_bk_amt [10] = {0};
   BYTE  valdn_code	 [5] =	{0};
   BYTE  prdouct_id	 [3] =  {0};
   BYTE  spend_qualfn_indcr[2] = {0};
   BYTE  accnt_funding_src[2] = {0};
   BYTE  authrzd_amount	[13]  = {0};
   BYTE  authrzn_curr_code[4] = {0};
      
   memset(tran_dt,0x00,7);   
   memset(c_record, 0x00, sizeof(c_record));
   memset(c_record_cup, 0x00, sizeof(c_record_cup));
   memset(term_id, 0x00, sizeof(term_id));
   memset(card_nbr, 0x00, sizeof(card_nbr));
   memset(cad_merchant_id, 0x00, sizeof(cad_merchant_id));
   memset(eci, 0x00, sizeof(eci));
   memset(tran_id,0x00, sizeof(tran_id));
   memset(pos_data, 0x00, sizeof(pos_data));
   memset(tran_link_id, 0x00, sizeof(tran_link_id));
   memset(plan_reg_sys_idntfr, 0x00, sizeof(plan_reg_sys_idntfr));
   memset(authn_char_indcr, 0x00, sizeof(authn_char_indcr));
   memset(authn_resp_code, 0x00, sizeof(authn_resp_code));
   memset(card_hldr_idntfn_method, 0x00, sizeof(card_hldr_idntfn_method));
   memset(cash_bk_amt, 0x00, sizeof(cash_bk_amt));
   memset(valdn_code, 0x00, sizeof(valdn_code));
   memset(prdouct_id, 0x00, sizeof(prdouct_id));
   memset(spend_qualfn_indcr, 0x00, sizeof(spend_qualfn_indcr));
   memset(accnt_funding_src, 0x00, sizeof(accnt_funding_src));
   memset(authrzd_amount, 0x00, sizeof(authrzd_amount));
   memset(authrzn_curr_code, 0x00, sizeof(authrzn_curr_code));

    /* Fill 6 byte with spaces for handling RPS traction.*/
   memset(perch_id,0x20,CUSTOM_IDENTIFIER_TYPE_LENGTH);

   /* Message Type */
    /* store message type for CUP OFFLINE transaction to generate exception file*/
	strncpy(temp_message_type,auth_tx_detail.message_type,4);
	strncpy(temp_processing_code,auth_tx_detail.processing_code,6);
    if (auth_tx_detail.tx_key == AUTH_REFUND)
	   {
		  strcpy(auth_tx_detail.message_type,"0400"); /* For Reversal Transaction */
	   }
	else
	      strcpy(auth_tx_detail.message_type,"0000"); /* For Original Transaction */

	/* Terminal ID - Left Justified ; Space Filled */

    if(true == Check_If_AMEX_Transaction()) /* AMEX Terminal Id - 15 chars + 1 Space Filled */
	 {
		memcpy(&term_id,auth_tx_detail.primary_key.device_id,15);
		
	}
	else   /* VISA,JCB,MCARD Terminal ID */
	{
		memcpy(&term_id,auth_tx_detail.primary_key.device_id,8);
		memset(term_id+8,' ',7);	
		
	}

    if(true == Check_If_AMEX_Transaction())/* AMEX Card Number - 15 numeric + 4 Space Filled */
	{
		memcpy(&card_nbr,auth_tx_detail.card_nbr,15);
		memset(&card_nbr[15],0x20,4);		
	}
    else if(true == Check_If_CUP_Transaction_epos())
    {
    	memset(card_nbr, 0x20,(sizeof(card_nbr)-1));
    	memcpy(&card_nbr,auth_tx_detail.card_nbr,strlen(auth_tx_detail.card_nbr));
    }
	else                                          /* VISA,JCB,MC Card Number - 16 numeric + 3 Space Filled */
	{
		memcpy(&card_nbr,auth_tx_detail.card_nbr,16);
		memset(&card_nbr[16],0x20,3);
	}

	  if(auth_tx_detail.tx_key == AUTH_REVERSAL)  /* Processing Code for REVERSAL */
	  {
		  memcpy(&auth_tx_detail.processing_code, "20", 2 );
	  }
	  if((0 == strcmp("0320",auth_tx_detail.message_type)) || (auth_tx_detail.processing_code[5] == '1'))  /* Batch Upload */
	  {
		  memcpy(&auth_tx_detail.processing_code, "000000", 6 );
	  }
	  if((auth_tx_detail.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION) || (auth_tx_detail.tx_key == AUTH_CASH_ADVANCE) || (strcmp(auth_tx_detail.processing_code,"010000") == 0)) /* Cash Advance */
	  {
		  if(true == Check_If_AMEX_Transaction())
		  {
			  memcpy(&auth_tx_detail.processing_code, "014008", 6 );
		  }
		  else
		  {
			  memcpy(&auth_tx_detail.processing_code, "010000", 6 );
		  }
	  }
    


	  if(true == Check_If_VISA_Transaction())
	  {
		  /* Its Quasi cash for Visa*/
		  if(0 == strncmp(temp_processing_code,"11",2))
		  {
			  memcpy(&auth_tx_detail.processing_code, "110000", 6 );
		  }
	  }
	  else if(true == Check_If_MC_Transaction() ||
			  true == Check_If_JCB_Transaction()||
			  true == Check_If_AMEX_Transaction())
	  {
		  /* Its Quasi cash for MC*/
		  if(0 == strncmp(temp_processing_code,"11",2))
		  {
			  memcpy(&auth_tx_detail.processing_code, "000000", 6 );
		  }
	  }


	 /* POS Data Logic */
	if(true == Check_If_CUP_Transaction_epos())
	{
		if(0==strncmp(Incremental_Auth_flag_CUP,"1",1))
		{

			if(strncmp(auth_tx_detail.product_codes[4].code,"procsd",6)==0)
			{
				/* USE the merchant ID not CUP Acceptor ID.*/
				if(strlen(cupMerchantID)>0)
				{
					strcpy(auth_tx_detail.merchant_id,cupMerchantID);
				}

				strncpy(pos_data,auth_tx_detail.product_codes[16].amount,CUP_EPOS_POS_DATA_LEN);
				/* RPS, Asiapay Recurring */
				if (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0)
				{

					strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
				}
				/* BDO Pay*/
				else if(strncmp(auth_tx_detail.product_codes[6].amount,"05",2) == 0)
				{

					strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
				}
				/* card swipe */
				else if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
				{

					strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
				}
				/* manual entry */
				else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
				{

					strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
				}

				/* CHIP */

				else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
				{

					strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
				}
								/* Fallback */
				else if(strncmp(auth_tx_detail.pos_entry_mode,"08",2) == 0)
				{

									/* CUP POS Input Mode (Segment 10, Pos 262-264) for Fallback transaction as "022"*/
					strncpy(CUP_Pos_input_mode,"022",3);
				}
				/* Contact less */
				else if(strncmp(auth_tx_detail.pos_entry_mode,"007",3) == 0)
				{

					strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
				}
				iIndustry = atoi(terminal.industry_code );
				if(iIndustry == 500)
				{
					/* CUP MOTO EPOS changes*/
					memset (CUP_Pos_input_mode, 0x00, sizeof(CUP_Pos_input_mode));
					strncpy(CUP_Pos_input_mode,"012",3);
				}
			}
			else
			{
				/* USE the merchant ID not CUP Acceptor ID.*/
				if(strlen(cupMerchantID)>0)
				{
					strcpy(auth_tx_detail.merchant_id,cupMerchantID);
				}

				/* RPS, Asiapay Recurring */
				if (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0)
				{
					strncpy(pos_data,CUP_RPS_ASIAPAY_RECURRING,CUP_EPOS_POS_DATA_LEN);
					strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
				}

				/* BDO Pay*/
				else if(strncmp(auth_tx_detail.product_codes[6].amount,"05",2) == 0)
				{
					strncpy(pos_data,CUP_BDO_PAY,CUP_EPOS_POS_DATA_LEN);
					strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
				}
			   /* card swipe */

				else if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
			   {
				   strncpy(pos_data,CUP_MAGAGSTRIPE_SWIPE,CUP_EPOS_POS_DATA_LEN);
				   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
			   }
			   /* manual entry */

			   else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
			   {
				   strncpy(pos_data,CUP_POS_MANUAL_KEYIN,CUP_EPOS_POS_DATA_LEN);
				   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
			   }

			   /* CHIP */

			   else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
			   {
				   strncpy(pos_data,CUP_CHIP_CONTACT,CUP_EPOS_POS_DATA_LEN);
				   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
			   }

			   /* Fallback */

			   else if(strncmp(auth_tx_detail.pos_entry_mode,"08",2) == 0)
			   {
				   strncpy(pos_data,CUP_CHIP_FALLBACK,CUP_EPOS_POS_DATA_LEN);
				   /* CUP POS Input Mode (Segment 10, Pos 262-264) for Fallback transaction as "022"*/
				   strncpy(CUP_Pos_input_mode,"022",3);
			   }

			   /* Contact less */
			   else if(strncmp(auth_tx_detail.pos_entry_mode,"007",3) == 0)
			   {
				  strncpy(pos_data,CUP_CHIP_CONTACTLESS,CUP_EPOS_POS_DATA_LEN);
				  strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
			   }

			   iIndustry = atoi(terminal.industry_code );
			   if(iIndustry == 500)
			   {
				   /* CUP MOTO EPOS changes*/
				   memset (CUP_Pos_input_mode, 0x00, sizeof(CUP_Pos_input_mode));
				   strncpy(CUP_Pos_input_mode,"012",3);
			   }
			}
		}
		else
		{
			if(strlen(cupMerchantID)>0)
			{
				strcpy(auth_tx_detail.merchant_id,cupMerchantID);
			}

			/* RPS, Asiapay Recurring */
			if (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0)
			{
				strncpy(pos_data,CUP_RPS_ASIAPAY_RECURRING,CUP_EPOS_POS_DATA_LEN);
				strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
			}

			/* BDO Pay*/
			else if(strncmp(auth_tx_detail.product_codes[6].amount,"05",2) == 0)
			{
				strncpy(pos_data,CUP_BDO_PAY,CUP_EPOS_POS_DATA_LEN);
				strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
			}
		   /* card swipe */

			else if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
		   {
			   strncpy(pos_data,CUP_MAGAGSTRIPE_SWIPE,CUP_EPOS_POS_DATA_LEN);
			   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		   }
		   /* manual entry */

		   else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
		   {
			   strncpy(pos_data,CUP_POS_MANUAL_KEYIN,CUP_EPOS_POS_DATA_LEN);
			   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		   }

		   /* CHIP */

		   else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
		   {
			   strncpy(pos_data,CUP_CHIP_CONTACT,CUP_EPOS_POS_DATA_LEN);
			   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		   }

		   /* Fallback */

		   else if(strncmp(auth_tx_detail.pos_entry_mode,"08",2) == 0)
		   {
			   strncpy(pos_data,CUP_CHIP_FALLBACK,CUP_EPOS_POS_DATA_LEN);
			   /* CUP POS Input Mode (Segment 10, Pos 262-264) for Fallback transaction as "022"*/
			   strncpy(CUP_Pos_input_mode,"022",3);
		   }

		   /* Contact less */
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"007",3) == 0)
		   {
			  strncpy(pos_data,CUP_CHIP_CONTACTLESS,CUP_EPOS_POS_DATA_LEN);
			  strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		   }

		   iIndustry = atoi(terminal.industry_code );
		   if(iIndustry == 500)
		   {
			   /* CUP MOTO EPOS changes*/
			   memset (CUP_Pos_input_mode, 0x00, sizeof(CUP_Pos_input_mode));
			   strncpy(CUP_Pos_input_mode,"012",3);
		   }
		}
#if BDOR_68_REQ_2_UPI

    	if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		{
		   strncpy( pos_data,
				   	UPI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
					UPI_POS_DATA_LEN_FOR_EPOS );
		   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		}
    	else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		{
		   strncpy( pos_data,
				    UPI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
					UPI_POS_DATA_LEN_FOR_EPOS );
		   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		}
#endif
#if BDOR_68_REQ_4_UPI

    	else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin())
		{
		   strncpy( pos_data,
				    UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
					UPI_POS_DATA_LEN_FOR_EPOS );
		   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		}
    	else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin())
		{
		   strncpy( pos_data,
				    UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
					UPI_POS_DATA_LEN_FOR_EPOS );
		   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		}
#endif
#if BDOR_68_REQ_6_UPI
    	else if (true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trxn())
		{
		   strncpy( pos_data,
				    UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					UPI_POS_DATA_LEN_FOR_EPOS );
		   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		}
#endif
	}
	else if(true == Check_If_JCB_Transaction()) /* JCB Card POS Data */
	{
	  /*
	  *  Let us check if we have already pos data generated during authorization
	  */

	  /*if(strlen(auth_tx_detail.product_codes[16].amount) > 0 )
	  {
		  strncpy( pos_data, auth_tx_detail.product_codes[16].amount,
				   EPOS_POS_DATA_SIZE);
	  }
	  else
	  {*/
		   /* card swipe */

		   if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
		   {
               strncpy(pos_data,"022000022",9);
		   }

		   /*Contactless magstripe */
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"091",3) == 0)
		   {
               strncpy(pos_data,"910000022",9);
		   }

		   /* manual entry */

           else if((strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0) &&
        		   (0==strncmp(auth_tx_detail.product_codes[6].amount,"00",2)))
           {
               strncpy(pos_data,"011000022",9);
           }
         
		   /* CHIP */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
           {
               strncpy(pos_data,"051000022",9);
           }

		   /*Contactless EMV*/
           else if(strncmp(auth_tx_detail.pos_entry_mode,"007",3) == 0)
           {
                   strncpy(pos_data,"071000022",9);
           }

		   /* Fallback */

		   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
		   {
               strncpy(pos_data,"971000022",9);
		   }
		   
		   /* New POS entry mode for BDO - TF Phani*/
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
		   {
			   strncpy(pos_data,"971000022",9);
		   }
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
		   {
			   strncpy(pos_data,"971000022",9);
		   }
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
		   {
			   strncpy(pos_data,"971000022",9);
		   }
		   else if((0==strncmp(auth_tx_detail.product_codes[6].amount,"05",2)))
		   {
			   strncpy(pos_data,"012050012   ",12);
		   }

		   /* E-Commerce */

		   if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) ||
				   (strncmp(auth_tx_detail.pos_entry_mode,"0081",4) == 0))
		   {
			   strncpy(pos_data,"812000012",9);
		   }

		   /* RPS  */

		   iIndustry = atoi(terminal.industry_code );

		   if((strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0))
		   {
			   strncpy(pos_data,"102000021   ",12);
		   }

#if BDOR_68_REQ_2_JCB

		   else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		   {
			   strncpy( pos_data,
					    JCB_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
						JCB_POS_DATA_LEN_FOR_EPOS );
		   }
		   else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		   {
			   strncpy( pos_data,
					    JCB_MOBILE_NFC_TAP2PHONE_TXN_TXN_WITHOUT_PIN_POS_DATA,
						JCB_POS_DATA_LEN_FOR_EPOS );
		   }
#endif
#if BDOR_68_REQ_4_JCB

		   else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin())
		   {
			   strncpy( pos_data,
					    JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
						JCB_POS_DATA_LEN_FOR_EPOS );
		   }
		   else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin())
		   {
			   strncpy( pos_data,
					    JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
						JCB_POS_DATA_LEN_FOR_EPOS );
		   }
#endif
#if BDOR_68_REQ_6_JCB
		   else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trxn())
			{
			   strncpy( pos_data,
						JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
						JCB_POS_DATA_LEN_FOR_EPOS );
			}
#endif
		   if(auth_tx_detail.pos_entry_mode[3]=='1')
		   {
			   memset(&pos_data[2],0x31,1);
		   }
		   else
		   {
			   memset(&pos_data[2],0x32,1);
		   }
	}

	else if(true == Check_If_VISA_Transaction()) /* VISA POS Data */
	{
	  /*
	  *  Let us check if we have already pos data generated during authorization
	  */

	  /*if(strlen(auth_tx_detail.product_codes[16].amount) > 0 )
	  {
		  strncpy( pos_data, auth_tx_detail.product_codes[16].amount,
				   EPOS_POS_DATA_SIZE);
	  }
	  else
	  {*/
		  /* card swipe */
           if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
           {
               strncpy(pos_data,"900050000",9);
           }
		   /* manual entry */
           else if((strncmp(auth_tx_detail.pos_entry_mode,"0012",4) == 0) &&
                   	(0==strncmp(auth_tx_detail.product_codes[6].amount,"00",2)))
           {
               strncpy(pos_data,"012050000",9);
           }
           else if((strncmp(auth_tx_detail.pos_entry_mode,"0011",4) == 0) &&
                    (0==strncmp(auth_tx_detail.product_codes[6].amount,"00",2)))
           {
                     strncpy(pos_data,"011052000",9);
           }
		   /* CHIP */
           else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
           {
               strncpy(pos_data,"050050000",9);
           }
		    else if(strncmp(auth_tx_detail.pos_entry_mode,"007",3) == 0)
		    {
               strncpy(pos_data,"072050000",9);
		    }

		   /* FALLBACK */
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
		   {
               strncpy(pos_data,"902052000",9);
		   }
          /* New POS entry mode for BDO - TF Phani*/
			else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
			{
               strncpy(pos_data,"902052000",9);
			}
            else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
            {
               strncpy(pos_data,"901052000",9);
            }
            else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
            {
               strncpy(pos_data,"902052000",9);
            }
            else if(strncmp(auth_tx_detail.pos_entry_mode,"091",3) == 0)
            {
                strncpy(pos_data,"912052000",9);
            }
            else if((0==strncmp(auth_tx_detail.product_codes[6].amount,"05",2)))
            {
                  strncpy(pos_data,"012010000   ",12);
            }
		   /* E-Commerce */
		    if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail.pos_entry_mode,"0081",4) == 0))
            {   
				strncpy(pos_data,"010590000",9);
					/*ECI value for VISA - ECI value should be copied to 88-89 position*/
				strncpy(eci,"07 ",3);
				if(strcmp(auth_tx_detail.profile,"")!=0)
				{
					strncpy(eci,auth_tx_detail.profile,2);
				}
				strncpy(pos_data+7,eci,2);
			}
		   /* RPS */
		    else if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) 
		   {
				strncpy(pos_data,"102010000",9);
				memset(&pos_data[9],0x52,1);
		   }

		   if(0 != strncmp(auth_tx_detail.product_codes[6].amount,"08",2) ) /* Rec Txn - R ,Non-Rec Txn - Space */
		   {
			   memset(&pos_data[9],0x20,1);
		   }

#if BDOR_68_REQ_2_VISA

		   if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		   {
			   strncpy( pos_data,
					    VISA_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
						VISA_POS_DATA_LEN_FOR_EPOS );
		   }
		   else  if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		   {
			   strncpy( pos_data,
					    VISA_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
						VISA_POS_DATA_LEN_FOR_EPOS );
		   }
#endif
#if BDOR_68_REQ_4_VISA

		   else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin())
		   {
			   strncpy( pos_data,
					    VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
						VISA_POS_DATA_LEN_FOR_EPOS );
		   }
		   else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin())
		   {
			   strncpy( pos_data,
					    VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
						VISA_POS_DATA_LEN_FOR_EPOS );
		   }
#endif
#if BDOR_68_REQ_6_VISA
		   else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trxn())
			{
			   strncpy( pos_data,
						VISA_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
						VISA_POS_DATA_LEN_FOR_EPOS );
			}
#endif
		   /* ATM - For checking pin exists or not */ 
		   if(auth_tx_detail.pos_entry_mode[3]=='1')
		   {
               memset(&pos_data[2],0x31,1);
		   }
		   else
		   {
			   memset(&pos_data[2],0x32,1); 
		   }
		    iIndustry = atoi(terminal.industry_code );
			if (iIndustry == MAIL_PHONE) /* MOTO */
			{

				if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0 ||
				   strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0 ||
				   strncmp(auth_tx_detail.pos_entry_mode,"007",3) == 0 ||
				   strncmp(auth_tx_detail.pos_entry_mode,"091",3) == 0	||
				   strncmp(auth_tx_detail.pos_entry_mode,"090",3) == 0	||
				   strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0 ||
				   strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
				{
					strncpy(pos_data+7,"00",2);
				}
				else if((strncmp(auth_tx_detail.pos_entry_mode,"0012",4) == 0 ||
						strncmp(auth_tx_detail.pos_entry_mode,"0011",4) == 0) &&
						strncmp(auth_tx_detail.product_codes[6].amount,"59",2) != 0)
				{
					strncpy(pos_data+7,"01",2);
				}

			}
	 }

     else if(true == Check_If_AMEX_Transaction()) /* AMEX POS Data */
	 {

    	 /*  Let us check if we have already pos data generated during authorization  */

    	 if(strlen(auth_tx_detail.product_codes[16].amount) > 0 )
          {
        	  strncpy( temp_Amex_POS_DATA,
        			   auth_tx_detail.product_codes[16].amount,
        			   EPOS_POS_DATA_SIZE);
    		  strTrim (temp_Amex_POS_DATA, pos_data) ;
          }

		  if(EPOS_POS_DATA_SIZE == strlen(pos_data))
		  {
			/* We have populated Pos data correctly*/
		  }
          else
          {
				/* We need only last 3 characters of POS entry mode
				* coming from terminal for processing further.  */

			if((strncmp(auth_tx_detail.processing_code,"0040",4) == 0) && (strncmp(auth_tx_detail.message_type,"0220",4) == 0) )
			{
				strncpy(auth_tx_detail.processing_code,"0000",4);
			}

			if(strlen(auth_tx_detail.pos_entry_mode) > 3)
			{
				strncpy(temp_entry_mode,&auth_tx_detail.pos_entry_mode[1],3);
			}
			else
			{
				strcpy(temp_entry_mode,auth_tx_detail.pos_entry_mode);
			}

#if BDOR_68_REQ_2_AMEX

    	  if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin())
		  {
			   strncpy( pos_data,
					    AMEX_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
						AMEX_POS_DATA_LEN_FOR_EPOS );
		  }
    	  else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin())
		  {
			   strncpy( pos_data,
					    AMEX_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PINPOS_DATA,
						AMEX_POS_DATA_LEN_FOR_EPOS );
		  }
#endif
#if BDOR_68_REQ_4_AMEX

    	  else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin())
		  {
			   strncpy( pos_data,
					    AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
						AMEX_POS_DATA_LEN_FOR_EPOS );
		  }
    	  else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin())
		  {
			   strncpy( pos_data,
					    AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PINPOS_DATA,
						AMEX_POS_DATA_LEN_FOR_EPOS );
		  }
#endif
#if BDOR_68_REQ_6_AMEX
		   else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trxn())
			{
			   strncpy( pos_data,
						AMEX_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
						AMEX_POS_DATA_LEN_FOR_EPOS );
			}
#endif
			   /* CHIP */

			else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
			 {
				   strncpy(pos_data,
						   AMEX_CHIP_POS_DATA_WITH_PIN,
						   EPOS_POS_DATA_SIZE);
			 }
			 else if(strncmp(auth_tx_detail.pos_entry_mode,"007",3) == 0)
			 {
				 strncpy(pos_data,
						 AMEX_CONTACTLESS_CHIP_POS_DATA_WITH_PIN,
						 EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"02",2) == 0) &&
					 (strncmp(temp_entry_mode,"021",3) != 0)) /* card swip without 4DBC */
			 {
				strncpy(pos_data,
						AMEX_MASGTRIP_POS_DATA_WITHOUT_PIN,
						EPOS_POS_DATA_SIZE);
			 }

			 else if(strncmp(temp_entry_mode,"91",2) == 0)
			 {
				strncpy(pos_data,
						AMEX_CONTACTLESS_MAGSTRIP_POS_DATA_WITHOUT_PIN,
						EPOS_POS_DATA_SIZE);
			 }

			 else if(strncmp(temp_entry_mode,"06",2) == 0) /* card swip with 4DBC */
			 {
				strncpy(pos_data,"260101W54140",EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"01",2) == 0)	 &&
					 (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) != 0) &&
					 (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) != 0)) /* manual key in without 4DBC */
			 {
				strncpy(pos_data,
						AMEX_POS_MANUAL_KEYIN_POS_DATA_WITH_PIN,
						EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"07",2) == 0) &&
					 (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) != 0) &&
					 (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) != 0)) /* manual key in with 4DBC */
			 {
				strncpy(pos_data,
						AMEX_CONTACTLESS_CHIP_POS_DATA_WITH_PIN,
						EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"012",3) == 0) &&
					 (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) &&
					  ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */
				 	 	 	 	 	 	 	 	 	 	 	 	 	 /* Changed the code for POS entry mode of position 5 from'2' to '9' */											  /* Girija Y TF, March 02, 2009 */ /* RPS Transactions without 4DBC */
			 {
				strncpy(pos_data,
						AMEX_RPS_ASIAPAY_RPS_POS_DATA,
						EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"07",2) == 0) &&
					 (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) &&
					  ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
																										  /* Girija Y TF, March 02, 2009 */ /* RPS Transactions with 4DBC */
			 {
				strncpy(pos_data,"660390S64130",EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"01",2) == 0) &&
					 (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) == 0)) /* Ecommerce without 4DBC */
			 {
				strncpy(pos_data,
						AMEX_ECOMM_ASIAPAY_ECOM_POS_DATA,
						EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"07",2) == 0) &&
					 (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) == 0)) /* Ecommerce with 4DBC */
			 {
				strncpy(pos_data,"6609S0SS4140",EPOS_POS_DATA_SIZE);
			 }

			 else if(strncmp(temp_entry_mode,"021",3) == 0) /* ATM */
			 {
				strncpy(pos_data,
						AMEX_MAGSTRIP_POS_DATA_WITH_PIN,
						EPOS_POS_DATA_SIZE);
			 }

			 else if(strncmp(temp_entry_mode,"80",2) == 0 ||
					  strncmp(temp_entry_mode,"802",3) == 0) /*Fall Back */
			 {
					strncpy(pos_data,
							AMEX_FALLBACK_POS_DATA_WITH_PIN,
							EPOS_POS_DATA_SIZE);
			 }

			 if(strlen(pos_data) == 0 &&
				strncmp(auth_tx_detail.pos_entry_mode,"080",3) == 0)
			 {
				 strncpy(pos_data,
						 AMEX_FALLBACK_POS_DATA_WITH_PIN,
						 EPOS_POS_DATA_SIZE);
			 }
		}
	}
	else  if(true == Check_If_MC_Transaction()) /* Master Card POS Data */
	 {
	   /* e-commerce  */
	   if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) ||
		  (0==strncmp(auth_tx_detail.pos_entry_mode,"0081",4)))
	   {
		   strncpy(pos_data,"810125106600",EPOS_POS_DATA_SIZE);
	   } /* RPS */
	   else if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0)
	   {
		   strncpy(pos_data,"100234100100",EPOS_POS_DATA_SIZE);
	   }
	   /* manual entry */
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0012",4) == 0 && (0==strncmp(auth_tx_detail.product_codes[6].amount,"00",2)))
	   {
			strncpy(pos_data,"012001110300",EPOS_POS_DATA_SIZE);
	   }
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0011",4) == 0 && (0==strncmp(auth_tx_detail.product_codes[6].amount,"00",2)))
	   {
			strncpy(pos_data,"011231100100",EPOS_POS_DATA_SIZE);
	   }
	   /* card swipe */
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
	   {
		   strncpy(pos_data,"900000010300",EPOS_POS_DATA_SIZE);
	   }
	   /* CHIP */
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
	   {
		   strncpy(pos_data,"050000010300",EPOS_POS_DATA_SIZE);
	   }
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"007",3) == 0) /*Pay pass CHip Contactless*/
	   {
		   strncpy(pos_data,"070000010300",EPOS_POS_DATA_SIZE);
	   }
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"091",3) == 0) /*MC Pay pass -Contactless Mag stripe*/
	   {
		   strncpy(pos_data,"910000010300",EPOS_POS_DATA_SIZE);
	   }
	   /* FALLBACK */
	   /* New POS entry mode added for MC */
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
	   {
		   strncpy(pos_data,"800000010300",EPOS_POS_DATA_SIZE);
	   }
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
	   {
		   strncpy(pos_data,"801000010300",EPOS_POS_DATA_SIZE);
	   }
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
	   {
		   strncpy(pos_data,"802000010300",EPOS_POS_DATA_SIZE);
	   }
	   else if(strncmp(auth_tx_detail.product_codes[6].amount,"05",2) == 0)
	   {   /*BDO Pay*/
		   strncpy(pos_data,"012231100100",EPOS_POS_DATA_SIZE);
	   }
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"009",3) == 0) /*MC Pay pass -Contactless Mag stripe*/
	   {
		   strncpy(pos_data,"091000000400",EPOS_POS_DATA_SIZE);
	   }
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
	   {
		   strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);
	   }

		iIndustry = atoi(terminal.industry_code );
		if (iIndustry == MAIL_PHONE) /* MOTO */
		{
			strncpy(pos_data,&auth_tx_detail.pos_entry_mode[1],3);
			strncpy(pos_data+3,"232100000",9);
		}

#if BDOR_68_REQ_2_MC

	    if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin())
	    {
		   strncpy( pos_data,
				   	MC_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
					MC_POS_DATA_LEN_FOR_EPOS );
	    }
	    else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin())
	    {
		   strncpy( pos_data,
				    MC_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
					MC_POS_DATA_LEN_FOR_EPOS );
	    }
#endif

#if BDOR_68_REQ_4_MC

	    else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin())
	    {
		   strncpy( pos_data,
				    MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
					MC_POS_DATA_LEN_FOR_EPOS );
	    }
	    else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin())
	    {
		   strncpy( pos_data,
				    MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
					MC_POS_DATA_LEN_FOR_EPOS );
	    }
#endif
#if BDOR_68_REQ_6_MC
	   else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trxn())
		{
		   strncpy( pos_data,
					MC_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					MC_POS_DATA_LEN_FOR_EPOS );
		}
#endif

		/* ATM - For checking pin exists or not */
	    else if(auth_tx_detail.pos_entry_mode[3]=='1')
		{
		   memset(&pos_data[2],0x31,1);
		}
		else if(auth_tx_detail.pos_entry_mode[3]=='0')
		{
			memset(&pos_data[2],0x30,1);
		}
		else
		{
		   memset(&pos_data[2],0x32,1);
		}
	}
	 /* Prepare pos data for Diners */
	else if(true == Check_If_Diners_Transaction())
	{
		/*  Let us check if we have already pos data generated during authorization */
		if(strlen(auth_tx_detail.product_codes[16].amount) > 0 )
		{
			strncpy( temp_DCI_POS_DATA, auth_tx_detail.product_codes[16].amount,
				     EPOS_POS_DATA_SIZE);
			strTrim (temp_DCI_POS_DATA, pos_data) ;
		}

		if(EPOS_POS_DATA_SIZE == strlen(pos_data))
		{
			/* We have populated Pos data correctly*/
		}
		else
		{
			memset(pos_data, 0x00, sizeof(pos_data));
			Diners_Prepare_posdata_for_DINERS();
			strncpy( pos_data, auth_tx_detail.product_codes[16].amount,
							   EPOS_POS_DATA_SIZE);
		}
	}

	
	/* Ecommerce Indicator Logic */

	  if(true == Check_If_AMEX_Transaction())
	  {
		  if((0 == strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) &&
			 (0 == strncmp(temp_message_type,"0200",4)) &&
			 ( (0 == strncmp(auth_tx_detail.pos_entry_mode,"001",3)) ||
			   (0 == strncmp(auth_tx_detail.pos_entry_mode,"01",2))))
		  {
			if(strcmp(auth_tx_detail.profile,"")!=0)
			{
				strncpy(eci,auth_tx_detail.profile,2);
				eci[2] = ' ';
			}
		   else
			  strncpy(eci,"07 ",3);
		  }
		  else
		  {
			  strncpy(eci," ",3);
		  }
	  }

	 else if(true == Check_If_MC_Transaction()) /* Master Card ECOM Indicator */
	
	 {
           if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2) || (0==strncmp(auth_tx_detail.pos_entry_mode,"0081",4)))) /* If E-commerce txn */
		   {
			   if(auth_tx_detail.product_codes[13].quantity[0]!='\0')
			   {
			     strncpy(eci,"21",2);
			     eci[2] = auth_tx_detail.product_codes[13].quantity[1];
			   }
			   else
			   {
				   strcpy(eci,"210");
			   }
		   }
		   else
		   {
			   strcpy(eci,"000");			   
		   }
     }

	 else if(true == Check_If_JCB_Transaction()) /* JCB Card ECOM Indicator */
	 {
		

	    if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2))) /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail.profile,"")!=0)
			   {
				 strcpy(eci,auth_tx_detail.profile);
				 strncpy(eci+2," ",1);
				 
			   }
			   else strncpy(eci,"07 ",3);
		   }
		   else
		   {
			  strncpy(eci,"   ",3);			   
		   }
	 }
	 else if(true == Check_If_CUP_Transaction_epos())
	 {
	 	strncpy(eci,"   ",3);
	 }

	 /* PAN Sequence Number */

	  /* if(strcmp(emv_record.tag,"5F34")==0)
	 {
		 strcpy(pan_sequence_number,emv_record.emv_data);
	 }
	 else */
	 strncpy(pan_sequence_number,"000",3);

	 if(true == Check_If_AMEX_Transaction()) /* For AMEX Card Sequence Number is Zero */
	 {
		 strncpy(pan_sequence_number,"000",3);
	 }
	 else if (true == Check_If_Diners_Transaction())
	 {
		 if(strlen(auth_tx_detail.product_codes[11].code)> 0)
		 {
			 strncpy(pan_sequence_number,auth_tx_detail.product_codes[11].code,3);
		 }
		 else
		 {
			 strncpy(pan_sequence_number,"001",3);
		 }
	 }

	  strncpy(tran_dt,auth_tx_detail.tran_date+2,6);

	  if(strncmp(auth_tx_detail.product_codes[6].amount+2,"",3)==0)
	  {
		  memcpy(auth_tx_detail.product_codes[6].amount+2,"000",3);
	  }
	  else if(strncmp(auth_tx_detail.product_codes[6].amount+2,"   ",3)==0)
	  {
	  	  memcpy(auth_tx_detail.product_codes[6].amount+2,"000",3);
	  }
	  strncpy(temp_service_code,auth_tx_detail.product_codes[6].amount+2,3);
	  strTrim(temp_service_code,service_code);

	  if(3 == strlen(service_code))
	  {
		  // We are able to retive service code from DB.
	  }
	  else
	  {
		  // Service code detaisl look incorrect.
		  memcpy(auth_tx_detail.product_codes[6].amount+2,"000",3);
	  }

	 /* Form 27 byte message for DE 63- table Id 12, TF Phani-
	  First 15 bytes in - BCh10 -> ticket_nbr
	  Next  10 bytes in - BCh10 -> product_codes[14].amount*/
	/* TF-- Abhishek
	1) If Field 63 is present in the authorisation message and starts with "PRCHID", place the value of Field 63 in EPOS at segment 11 starting at
	    column 76 until 106.
	2) If Field 63 is present in the authorisation message and doesn't start with "PRCHID", place the values of Field 63 at segment 11 starting at column
	   82 until 106.
	Note :Item 1 is to address for RPS transactions. Item 2 is to address for PolicyNumber coming from POS terminal.
	*/
	if(strncmp(auth_tx_detail.ticket_nbr,"PRCHID",CUSTOM_IDENTIFIER_TYPE_LENGTH)== 0)
	{
		// Print 31 byte starting from 76 until 106
		strncpy(perch_id,auth_tx_detail.ticket_nbr,strlen(auth_tx_detail.ticket_nbr));
		strncat(perch_id,auth_tx_detail.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail.ticket_nbr) - CUSTOM_IDENTIFIER_TYPE_LENGTH +1));
	    perch_id[PERCH_ID_LENGTH -1]='\0';
	}
	else
	{
		// 1st 6 byte should be space as feild 63 is not starts whith PRCHID
		// copy data after 6 byte which will be store from 82nd byte location
		//Print 25 byte starting from 82 until 106
		strncpy(perch_id + CUSTOM_IDENTIFIER_TYPE_LENGTH ,auth_tx_detail.ticket_nbr,strlen(auth_tx_detail.ticket_nbr));
		strncat(perch_id,auth_tx_detail.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail.ticket_nbr)- CUSTOM_IDENTIFIER_TYPE_LENGTH +1 ));
		perch_id[PERCH_ID_LENGTH -1]='\0';
	}

	memcpy(cad_merchant_id,auth_tx_detail.merchant_id+5, 10 ); /* Cadence Merchant ID */

    /* Transaction ID */

	 if(true == Check_If_MC_Transaction()) /* Transaction ID for MASTER CARDS */
	
	 {    
		 // Settlement date MMDD
			strncpy(settle_date,auth_tx_detail.product_codes[19].code,4);//MMDD
			/*If Settle date is NUll, copy tran date as settle date*/
			if(strlen(settle_date) == 0 || settle_date[0] ==  ' ' || settle_date[1] ==  ' ')
			memcpy(settle_date,&auth_tx_detail.tran_date[4],4);	 
			
			if(strlen(settle_date) == 0 || settle_date[0] ==  ' ' || settle_date[1] ==  ' ')
			memcpy(settle_date,CurrentDate+2,4);

		 if(strlen(auth_tx_detail.mcard_banknet)>0)
		 {
			 if(strlen(auth_tx_detail.mcard_banknet)>12)
			 {
			   strncpy(tran_id,auth_tx_detail.mcard_banknet,12);
			   strncat(tran_id,"   ",3);
			   strncpy(&tran_id[9],settle_date,4);
			 }
			 else 
			 {
                 memset(tran_id,' ',15);
				 strncpy(tran_id,auth_tx_detail.mcard_banknet,
						 strlen(auth_tx_detail.mcard_banknet));
				 strncpy(&tran_id[9],settle_date,4);
			   
			 }
		 }
		 else
		 {
			 memset(tran_id,' ',15);
			 //strcat(tran_id,"000");
			 
			 if(strlen(settle_date) == 0)
			 memcpy(settle_date,CurrentDate+2,4);
			 
			 strncpy(&tran_id[9],settle_date,4);
		 } 

	 }

	 else if(true == Check_If_VISA_Transaction()) /* Transaction ID for VISA */
	 {
	      memset(tran_id,0x30,15);	
		  strncpy(tran_id,&auth_tx_detail.mcard_banknet[1],15);
	 }

	 else if(true == Check_If_AMEX_Transaction()) /* Transaction ID for AMEX */
	 {
		 strncpy(tran_id,auth_tx_detail.mcard_banknet,15);
	 }
	 else if(true == Check_If_JCB_Transaction()) /* Transaction ID for JCB - Spaces */
	 {
		 memset(tran_id,' ',15); 
	 }
	 else if(true == Check_If_CUP_Transaction_epos()) /* Transaction ID for JCB - Spaces */
	 {
		 memset(tran_id,' ',15); 
	 }
	 else if(true == Check_If_Diners_Transaction()) /* Transaction ID for DCI - Spaces */
	 {
		 len = strlen (auth_tx_detail.product_codes[13].amount);
		 if(len > 0 )
		 {
			 memcpy(temp_DCI_tran_id,auth_tx_detail.product_codes[13].code,6);
			 memcpy(temp_DCI_tran_id +6,auth_tx_detail.product_codes[13].amount,len);
		}
		else
		{
		   len = strlen (auth_tx_detail.product_codes[13].code);
		   memcpy(temp_DCI_tran_id,auth_tx_detail.product_codes[13].code,len);
		}
		strTrim (temp_DCI_tran_id, tran_id) ;
		if(strlen(tran_id) > 0)
		{
			// We have populated TID data correctly
		}
		else
		{
			memcpy(tran_id,auth_tx_detail.transaction_id,15);
		}

	 }
	 /* writing details to file */

	 /* Segment 10 */
  if(true == Check_If_CUP_Transaction_epos())
  {
	// check if we have original Satn store in product10_code filed which we have stored for offline transaction.
	if(strlen(auth_tx_detail.product_codes[9].code) > 0)
	{
		memcpy(CUP_trace_ID,auth_tx_detail.product_codes[9].code,(sizeof(CUP_trace_ID) -1));
	}
	else
	{
		memcpy(CUP_trace_ID,auth_tx_detail.sys_trace_audit_num,(sizeof(CUP_trace_ID) -1));
	}
	sprintf(c_record_cup, "%1s%4s%-15s%-019s%6s%6s%2s%6s%012s%06s%4s%-12s%04s%11s%12s%06s%03s%40s%012s%4s%29s%11s%03s%3s%2s%-3s%3s%6s%6s%10s%3s%4s%8s",
			"1",
			auth_tx_detail.message_type,
			term_id,		 
			auth_tx_detail.card_nbr,
			tran_dt,
			auth_tx_detail.tran_time,
			"10",  /* Segment 10 Data Follows */
			auth_tx_detail.processing_code,
			auth_tx_detail.tran_amount,
			"0",
			terminal.category_code,
			pos_data, 
			"0",
			" ",
			auth_tx_detail.primary_key.retrieval_ref_num,
			auth_tx_detail.auth_number, 		
			"0",
			" ",
			"0",
			" ",
			" ",
			" ",
			"0",
			curr_code,
			" ",
			eci, /* E-Commerce Logic for MCRD,JCB ; VISA,AMEX - Spaces */
			" ",
			auth_tx_detail.primary_key.batch_nbr,//240-245
			CUP_trace_ID, //246-251
			auth_tx_detail.transmission_timestamp,//252-261
			CUP_Pos_input_mode,//262-264
			auth_tx_detail.product_codes[19].code,//265-268
			auth_tx_detail.primary_key.device_id //269-276
			);
		
	
		 strncpy(c_record_cup + 276, "\n\0", 2);
		 crec_count++;

	  if (write_to_CUP_credit_settle_file() == false)
      return (false);
	  /*
			check if Exception is enable then create the excpetion file
	  */
	  if(CUP_EPOS_Exception_Flag == 1)
	  {
		/* We need to generate exception file only for OFF line transaction*/
		if( 0 == strncmp(temp_message_type,"0220",4))
		{
			if (write_to_CUP_Exception_file() == false)
			{
				 return (false);
			}
			E_crec_count++;
		}
	}
  }
  else
  {     sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%6s%012s%06s%4s%-12s%04s%11s%12s%06s%03s%40s%012s%4s%29s%11s%03s%3s%2s%-3s%3s%6s",
         "1",
		 auth_tx_detail.message_type,
         term_id,         
         auth_tx_detail.card_nbr,
         tran_dt,
         auth_tx_detail.tran_time,
         "10",  /* Segment 10 Data Follows */
         auth_tx_detail.processing_code,
         auth_tx_detail.tran_amount,
		 "0",
		 terminal.category_code,
		 pos_data, 
		 "0",
		 " ",
		 auth_tx_detail.primary_key.retrieval_ref_num,
		 auth_tx_detail.auth_number,         
		 "0",
		 " ",
		 "0",
		 " ",
		 " ",
		 " ",
		 "0",
		 curr_code,
		 " ",
		 eci, /* E-Commerce Logic for MCRD,JCB ; VISA,AMEX - Spaces */
		 " ",
		 auth_tx_detail.primary_key.batch_nbr		 
         );
	 

      strncpy(c_record + 245, "\n\0", 2);
	   crec_count++;

	  if (write_to_credit_settle_file() == false)
      return (false);	
  }
	  cseg10_records++;

	  /* Segment 11 */
	  /* TF Abhishek
	   * copy for CUP or CUP branded card shall	  contain "CC"on Segment 11 Pos 159-173
	   * Pad the value with spaces to occupy the whole 159 to 173 position. */
	  if (true == Check_If_CUP_Transaction_epos())
	  {
		    memcpy(tran_id," ",(sizeof(tran_id)-1));
		    memcpy(tran_id, CUP_CARD_INDICATOR_FLAG,2 );
	  }

	  if(auth_tx_detail.product_codes[0].code[0] == '\0')
	  {
		  memcpy(auth_tx_detail.product_codes[0].code," ",1);  //This is done to avoid the wrong formatting.
	  }

#ifdef MC02_APR2024
	  memcpy(add_token_resp, " ", 1);
	  if(true == Check_If_MC_Transaction())
	  {
		  if(auth_tx_detail.product_codes[10].amount[0] != '\0')
		  {
			  memcpy(tran_link_id, auth_tx_detail.product_codes[10].amount, 10);
			  strncat(tran_link_id, auth_tx_detail.product_codes[1].amount, 12);
		  }
		  memset(plan_reg_sys_idntfr, 0x00, sizeof(plan_reg_sys_idntfr) -1);
	  }
#endif

	  else if (true == Check_If_VISA_Transaction())
	  {
		  if (auth_tx_detail.product_codes[1].amount[0] != '\0')
		  {
			  memcpy(plan_reg_sys_idntfr, auth_tx_detail.product_codes[1].amount, 10);

		  }
		  memset(tran_link_id, 0x00, sizeof(tran_link_id) -1);
		  }
	  else
	  {
		  memset(tran_link_id, 0x00, sizeof(tran_link_id) -1);
		  memset(plan_reg_sys_idntfr, 0x00, sizeof(plan_reg_sys_idntfr) -1);
	  }
	  memset(c_record, 0, sizeof(c_record));

	  sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%4s%012s%03s%03s%-031s%4s%10s%1s%11s%15s%03s%8s%-15s%48s%1c%1c%-40s%-10s%3s",
         "1",
		 auth_tx_detail.message_type,
         term_id,         
         auth_tx_detail.card_nbr,
         tran_dt,
         auth_tx_detail.tran_time,         
		 "11", /* Segment 11 Data Follows */
         auth_tx_detail.exp_date,
		 "0",
		 auth_tx_detail.product_codes[6].amount+2,
	     pan_sequence_number,
	     perch_id,
		 " ",
         cad_merchant_id,
		 " ",
		 " ",
		 " ",
		 "0",
		 " ",
         tran_id, 
		 " ",
		 auth_tx_detail.product_codes[0].code[0],
		 add_token_resp[0],
		 tran_link_id,
		 plan_reg_sys_idntfr,
		 " "	 	          	 
         );

      strncpy(c_record + 276, "\n\0", 2);

	  crec_count++;

	  if (write_to_credit_settle_file() == false)
      return (false);

	  //Segment 12



	  if (true ==  Check_If_CUP_Transaction_epos())
	  {
		  iIndustry = atoi(terminal.industry_code );
		  if (iIndustry == 500 ||
			  strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0)
		  {
			  // MOTO/RPS Iniatiation Mode
			  strncpy(trsn_intn_mode, "3", 1);
		  }
		  else
		  {
			  // Attended,POS Iniatiation Mode
			  strncpy(trsn_intn_mode, "0", 1);
		  }

		  memset(c_record, 0, sizeof(c_record));
			sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%11s%15s%011s%40s%1s%40s%2s%3s%2s%1s%1s%2s%2s%-35s%12s%1s%4s%1s%8s",
			   "1",
			   auth_tx_detail.message_type,
			   term_id,
			   auth_tx_detail.card_nbr,
			   tran_dt,
			   auth_tx_detail.tran_time,
			   "12",
			   " ",
			   " ",
			"",
			" ",
			" ",
			" ",
			" ",
			" ",
			format_code,
			rpymt_type,
			rpymt_amt_indr,
			rpymt_nbr,
			rpymt_freq,
			perch_id,
			max_rpymt_amt,
			valdn_indr,
			cit_mit_indr,
			trsn_intn_mode,
			" "
			);

		strncpy(c_record + 245, "\n\0", 2);
		crec_count++;

		if (write_to_credit_settle_file() == false)
			return (false);

	  }

	  else if ((true == Check_If_VISA_RPS_Offus_Transaction() ||
	  	   true == Check_If_MC_RPS_Offus_Transaction()) )
	  {
		  if (true == Check_If_VISA_RPS_Offus_Transaction())
		  {
			  strncpy(format_code, "RP", 2);
			  strncpy(rpymt_type, "2", 1);
			  strncpy(rpymt_amt_indr, "2" , 1);
			  strncpy(rpymt_nbr, "99", 2);
			  strncpy(rpymt_freq, "12",2);
			  strncpy(max_rpymt_amt, "999999999999", 12);
			  strncpy(valdn_indr, "1", 1);
			  strncpy(authn_char_indcr, auth_tx_detail.mcard_banknet , 1);
			  strncpy(authn_resp_code, auth_tx_detail.response_code, 2);
			  strncpy(card_hldr_idntfn_method, auth_tx_detail.product_codes[5].quantity, 1);
			  strncpy(cash_bk_amt, auth_tx_detail.down_payment, 9);
			  strncpy(valdn_code, auth_tx_detail.interest_rate, 4);
			  strncpy(prdouct_id, auth_tx_detail.product_codes[7].code, 2);
			  strncpy(spend_qualfn_indcr, &auth_tx_detail.product_codes[7].code[2], 1);
			  strncpy(accnt_funding_src, &auth_tx_detail.product_codes[5].quantity[1], 1);
			  strncpy(authrzd_amount, auth_tx_detail.tran_amount, 12);
			  strncpy(authrzn_curr_code, auth_tx_detail.term, 3);
		  }

		  if (true == Check_If_MC_RPS_Offus_Transaction())
		  {
			  strncpy(cit_mit_indr, "M101", sizeof(cit_mit_indr)-1);
			  memset(perch_id, 0x00, sizeof(perch_id));
		  }

			  memset(c_record, 0, sizeof(c_record));
				sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%11s%15s%11s%40s%1s%40s%2s%3s%2s%1s%1s%2s%2s%-35s%12s%1s%4s%1s%1s%2s%1s%9s%-4s%2s%1s%1s%12s%3s%3s",
				   "1",
				   auth_tx_detail.message_type,
				   term_id,
				   auth_tx_detail.card_nbr,
				   tran_dt,
				   auth_tx_detail.tran_time,
				   "12",
				   " ",
				   " ",
				" ",
				" ",
				" ",
				" ",
				" ",
				" ",
				format_code,
				rpymt_type,
				rpymt_amt_indr,
				rpymt_nbr,
				rpymt_freq,
				perch_id,
				max_rpymt_amt,
				valdn_indr,
				cit_mit_indr,
				trsn_intn_mode,
				authn_char_indcr,
				authn_resp_code,
				card_hldr_idntfn_method,
				cash_bk_amt,
				valdn_code,
				prdouct_id,
				spend_qualfn_indcr,
				accnt_funding_src,
				authrzd_amount,
				authrzn_curr_code,
				" "
				);

			strncpy(c_record + 276, "\n\0", 2);
			crec_count++;

		  if (write_to_credit_settle_file() == false)
			return (false);
	  }
	  else if ((true == Check_If_VISA_Transaction()) &&
			   0 == strncmp(auth_tx_detail.product_codes[5].code, "OFFUS", 5)  )
	  {

		  if (true == Check_If_VISA_Transaction())
		  {
				  /*strncpy(format_code, "RP", 2);
				  strncpy(rpymt_type, "2", 1);
				  strncpy(rpymt_amt_indr, "2" , 1);
				  strncpy(rpymt_nbr, "99", 2);
				  strncpy(rpymt_freq, "12",2);
				  strncpy(max_rpymt_amt, "999999999999", 12);
				  strncpy(valdn_indr, "1", 1);*/
			  	  memset(format_code, 0x00, sizeof(format_code));
			  	  memset(rpymt_type, 0x00, sizeof(rpymt_type));
			  	  memset(rpymt_amt_indr, 0x00, sizeof(rpymt_amt_indr));
			  	  memset(rpymt_nbr, 0x00, sizeof(rpymt_nbr));
			  	  memset(rpymt_freq, 0x00, sizeof(rpymt_freq));
			  	  memset(max_rpymt_amt, 0x00, sizeof(max_rpymt_amt));
			  	  memset(valdn_indr, 0x00, sizeof(valdn_indr));
			  	  memset(cit_mit_indr, 0x00, sizeof(cit_mit_indr));

				  strncpy(authn_char_indcr, auth_tx_detail.mcard_banknet , 1);
				  strncpy(authn_resp_code, auth_tx_detail.response_code, 2);
				  strncpy(card_hldr_idntfn_method, auth_tx_detail.product_codes[5].quantity, 1);
				  strncpy(cash_bk_amt, auth_tx_detail.down_payment, 9);
				  strncpy(valdn_code, auth_tx_detail.interest_rate, 4);
				  strncpy(prdouct_id, auth_tx_detail.product_codes[7].code, 2);
				  strncpy(spend_qualfn_indcr, &auth_tx_detail.product_codes[7].code[2], 1);
				  strncpy(accnt_funding_src, &auth_tx_detail.product_codes[5].quantity[1], 1);
				  strncpy(authrzd_amount, auth_tx_detail.tran_amount, 12);
				  strncpy(authrzn_curr_code, auth_tx_detail.term, 3);
		  }


				  memset(c_record, 0, sizeof(c_record));
					sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%11s%15s%11s%40s%1s%40s%2s%3s%2s%1s%1s%2s%2s%-35s%12s%1s%4s%1s%1s%2s%1s%9s%-4s%2s%1s%1s%12s%3s%3s",
							   "1",
							   auth_tx_detail.message_type,
							   term_id,
							   auth_tx_detail.card_nbr,
							   tran_dt,
							   auth_tx_detail.tran_time,
							   "12",
							   " ",
							   " ",
							" ",
							" ",
							" ",
							" ",
							" ",
							" ",
							format_code,
							rpymt_type,
							rpymt_amt_indr,
							rpymt_nbr,
							rpymt_freq,
							perch_id,
							max_rpymt_amt,
							valdn_indr,
							cit_mit_indr,
							trsn_intn_mode,
							authn_char_indcr,
							authn_resp_code,
							card_hldr_idntfn_method,
							cash_bk_amt,
							valdn_code,
							prdouct_id,
							spend_qualfn_indcr,
							accnt_funding_src,
							authrzd_amount,
							authrzn_curr_code,
							" "
							);

						strncpy(c_record + 276, "\n\0", 2);
						crec_count++;

						if (write_to_credit_settle_file() == false)
						return (false);
	  }

	  /* Segment 13 - EMV Data */


     memset(emv_seg13, 0x00, sizeof(emv_seg13));
	 memset(emv_seg14, 0x00, sizeof(emv_seg14));
	 memset(emv_seg15, 0x00, sizeof(emv_seg15));
	     
     memset(c_record, 0x00, sizeof(c_record));

	// if((strncmp(auth_tx_detail.card_nbr,"34",2)!=0) && (strncmp(auth_tx_detail.card_nbr,"37",2)!=0) && ((strcmp(auth_tx_detail.pos_entry_mode,"0051")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0052")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0072")==0))) /* EMV Data is not present in AMEX */
	 if((strcmp(auth_tx_detail.pos_entry_mode,"0051")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0052")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0072")==0)) /* EMV Data is not present in AMEX */
	 {	  

        max_etran = atoi(Emv_list.num_returned);		
		
		if(max_etran==0) return false;

		for (i = 0; i < max_etran; i++)
		{
			memset(emv_data, 0, sizeof(emv_data));
			memset(&emv_record, 0, sizeof(emv_data));
			memcpy(&emv_record,&Emv_list.batch_list[i],sizeof(emv_record));			
         
			elength=2*atoi(emv_record.DATA_LENGTH);					

		//	bin_to_ascii(emv_data,emv_record.RAW_DATA,elength/2);	
		//	bin_to_ascii(emv_record.RAW_DATA,emv_data,elength/2);

			/* Parse the EMV raw data to wrtie it in  segments - Ajay-TF, 18-Jun-2009 */
        	parseEmvDataToSegment(elength, emv_seg13, emv_seg14, emv_seg15);	 

		}		
	 		
		 memset(&auth_tx_detail.tran_time[6],'\0',1);
		 memset(&term_id[15],'\0',1);
         memset(c_record, 0x00, sizeof(c_record));

		 if((strlen(emv_seg13)!=0))
		 {

		    sprintf(c_record, "%1s%04s%-15s%-019s%06s%06s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,         
			 "13", /* Segment 13 Data Follows */		
			 emv_seg13			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;

			 if((strlen(emv_seg14)==0)) return(true);
			 
			 if (write_to_credit_settle_file() == false)
             return (false);

		 }

		  memset(c_record, 0x00, sizeof(c_record));
		   memset(&auth_tx_detail.tran_time[6],'\0',1);
		 memset(&term_id[15],'\0',1);	
		  if((strlen(emv_seg14)!=0))
		 {

			 sprintf(c_record, "%1s%04s%-15s%-019s%06s%06s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,         
			 "14", /* Segment 14 Data Follows */		
			 emv_seg14			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;	

			 if((strlen(emv_seg15)==0)) return(true);
			 
			 if (write_to_credit_settle_file() == false)
             return (false);
		 }
		  memset(&auth_tx_detail.tran_time[6],'\0',1);
  	      memset(&term_id[15],'\0',1);
		  memset(c_record, 0x00, sizeof(c_record));
		 
		    if((strlen(emv_seg15)!=0))
		 {

			 sprintf(c_record, "%1s%04s%-15s%-019s%06s%06s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,         
			 "15", /* Segment 15 Data Follows */		
			 emv_seg15			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;				   		 

		 }
	}
	       
	 
         return(true);

} /* create_credit_detail */

/*****************************************************************************
  Function : parseEmvDataToSegment

  Description: This function parse the emv raw data and writes in segment. 
               if length less than 192 it writes in segment 13
			   else if length is gretaer than 192 adn less than 384 
			   first 192 length data writes in segment 13 and remainiang data in segment 14
			   else 
			   first 192 length data writes in a segment 13, 
			   next 192 length of data writes in segment 14 and
			   remaining in segment 15
  Author: 
             Ajay @ ThoughtFocus, Bangaluru
  Inputs:
             elength- Data length
  Outputs:
             emv_seg13, emv_seg14 and emv_seg15
  Return values:
			 None
  
*******************************************************************************/
void parseEmvDataToSegment(INT elength, CHAR emv_seg13[], CHAR emv_seg14[], CHAR emv_seg15[])
{
   INT    fieldLength;
   INT    dataIndex;
   INT    bTwoBytesTag;
   BYTE   byteOneOfTag;
   BYTE   byteTwoOfTag;
   CHAR   tempstr[500] = {0};
   INT	  dataLen13=0;
   INT	  dataLen14=0;
   INT	  dataLen15=0;
   INT	  totLen=0;
  
	
	if(elength<192)
	{
		bin_to_ascii(emv_record.RAW_DATA,emv_data,elength/2);
		memcpy(&emv_seg13,emv_data,elength);
	}
	else 
	{
		 dataIndex=0;
		 dataLen13=0;
		 dataLen14=0;
		 dataLen15=0;
		 while (dataIndex<= elength/2)
		 {
			 bTwoBytesTag = 0;
			 byteOneOfTag = emv_record.RAW_DATA[dataIndex];
			 dataIndex++;
			 if ((byteOneOfTag & 0x1F) == 0x1F)
			 {
				bTwoBytesTag = 1;
				byteTwoOfTag = emv_record.RAW_DATA[dataIndex];
				dataIndex++;
			 }
			 fieldLength = emv_record.RAW_DATA[dataIndex];
			 // Field length validation ..considering worst case length 36
			 if(fieldLength > EMV_TAG_MAX_LEN)
			 	return; // Some thing wrong with length ..return
			 dataIndex++;
             if(fieldLength!=0)
			 {
				 if (bTwoBytesTag)
				 {
					bin_to_ascii (&emv_record.RAW_DATA[dataIndex-3],  &tempstr[0], fieldLength+3 );
					dataIndex += fieldLength;
					totLen+=fieldLength*2+6;
					if(totLen<=192)
					{ 
						 memcpy(emv_seg13+dataLen13,&tempstr,fieldLength*2+6);
						 dataLen13+=fieldLength*2+6;
					}
					else if(totLen>192 && totLen<384)
					{ 
						 memcpy(emv_seg14+dataLen14,&tempstr,fieldLength*2+6); 
						 dataLen14+=fieldLength*2+6;
					}
					else if(totLen>=384 && totLen<512)
					{
						 memcpy(emv_seg15+dataLen15,&tempstr,fieldLength*2+6); 
						 dataLen15+=fieldLength*2+6;
					}
					memset(&tempstr,0,sizeof(tempstr));
				}
				else
				{
					bin_to_ascii (&emv_record.RAW_DATA[dataIndex-2],  &tempstr[0], fieldLength+2 );
					dataIndex += fieldLength;
					totLen+=fieldLength*2+4;
					if(totLen<=192)
					{ 
						memcpy(emv_seg13+dataLen13,&tempstr,fieldLength*2+4);
						dataLen13+=fieldLength*2+4;
					}
					else if(totLen>192 && totLen<384)
					{ 
						memcpy(emv_seg14+dataLen14,&tempstr,fieldLength*2+4);
						dataLen14+=fieldLength*2+4;
					}
					else if(totLen>=384 && totLen<512)
					{
						memcpy(emv_seg15+dataLen15,&tempstr,fieldLength*2+4);
						dataLen15+=fieldLength*2+4;
					}
					memset(tempstr,0,sizeof(tempstr));
				}
			}
		}
	}	
}



/*******************************************************************************
*
*  FUNCTION:         create_credit_detail_arc
*
*  DESCRIPTION:      This function creates detail structure for the credit
*                    settlement file from the batch archive file (BCH11).
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  detail structure 
*
*  RETURN VALUE:     True if record should be written to file, else false

*******************************************************************************/
INT create_credit_detail_arc()
{
   INT   max_etran = 0;
   INT   i = 0;
   INT   elength = 0;
   BYTE  term_id[16] = {0};
   BYTE  pos_data[13] = {0};
   CHAR  cad_merchant_id[12]={0};
   CHAR  tran_dt[7]={0};
   INT   iIndustry=0;
   BYTE  eci[4]={0};
   BYTE  tran_id[16] = {0};
   BYTE  settle_date[5] = {0};
   BYTE	 temp_entry_mode[4] = {0};
   BYTE  card_nbr[20] = {0};
   BYTE  pan_sequence_number[4]={0};
   BYTE  perch_id[PERCH_ID_LENGTH]={0}; // ID to store De63 table id -12 of messages 27 charcters- TF Phani
   CHAR  temp_message_type[5]={0};
   CHAR  CUP_Pos_input_mode[4]={0};
   CHAR  CUP_trace_ID[7]={0};
   CHAR  temp_DCI_POS_DATA[13] = {0};
   CHAR  temp_Amex_POS_DATA[13] = {0};
   BYTE  temp_DCI_tran_id[16]={0};
   CHAR  temp_service_code[4] = {0};
   CHAR  service_code[4]     = {0};
   CHAR  temp_processing_code[7] = {0};
   char  buff[512] = {0};
   INT   cardlen = 0;
   CHAR  tempcard[21] = {0};
   INT   nIndex = 0;
   CHAR  ErrorMsg[512] = {0};
   INT   returncode = 0;
   INT   db_retrive_flag = 0;
   BYTE  trsn_intn_mode[2]	= {0};
   BYTE	 cit_mit_indr[5] 	= {0};
   BYTE  format_code [3]	= {0};
   BYTE	 rpymt_type	 [2]	= {0};
   BYTE  rpymt_amt_indr[2]	= {0};
   BYTE  rpymt_nbr[3]		= {0};
   BYTE  rpymt_freq[3]		= {0};
   BYTE  rr_nbr[36]			= {0};
   BYTE  max_rpymt_amt[13]	= {0};
   BYTE	 valdn_indr[2]		= {0};
   CHAR  add_token_resp[2]  = {0};
   CHAR  tran_link_id[25] 	= {0};
   BYTE  plan_reg_sys_idntfr[11]= {0};
   BYTE	 authn_char_indcr[2] = {0};
   BYTE  authn_resp_code[3]  = {0};
   BYTE  card_hldr_idntfn_method [2] = {0};
   BYTE  cash_bk_amt [10] = {0};
   BYTE  valdn_code	 [5] =	{0};
   BYTE  prdouct_id	 [3] =  {0};
   BYTE  spend_qualfn_indcr[2] = {0};
   BYTE  accnt_funding_src[2] = {0};
   BYTE  authrzd_amount	[13]  = {0};
   BYTE  authrzn_curr_code[4] = {0};

   memset(tran_dt,0x00,7);   
   memset(c_record, 0x00, sizeof(c_record));
   memset(c_record_cup, 0x00, sizeof(c_record_cup));
   memset(term_id, 0x00, sizeof(term_id));
   memset(card_nbr, 0x00, sizeof(card_nbr));
   memset(cad_merchant_id, 0x00, sizeof(cad_merchant_id));
   memset(eci, 0x00, sizeof(eci));
   memset(tran_id, 0x00, sizeof(tran_id));
   memset(pos_data, 0x00, sizeof(pos_data));
   memset(tran_link_id, 0x00, sizeof(tran_link_id));
   memset(plan_reg_sys_idntfr, 0x00, sizeof(plan_reg_sys_idntfr));
   memset(authn_char_indcr, 0x00, sizeof(authn_char_indcr));
   memset(authn_resp_code, 0x00, sizeof(authn_resp_code));
   memset(card_hldr_idntfn_method, 0x00, sizeof(card_hldr_idntfn_method));
   memset(cash_bk_amt, 0x00, sizeof(cash_bk_amt));
   memset(valdn_code, 0x00, sizeof(valdn_code));
   memset(prdouct_id, 0x00, sizeof(prdouct_id));
   memset(spend_qualfn_indcr, 0x00, sizeof(spend_qualfn_indcr));
   memset(accnt_funding_src, 0x00, sizeof(accnt_funding_src));
   memset(authrzd_amount, 0x00, sizeof(authrzd_amount));
   memset(authrzn_curr_code, 0x00, sizeof(authrzn_curr_code));

   // Fill 6 byte with spaces for handling RPS traction.
   memset(perch_id,0x20,CUSTOM_IDENTIFIER_TYPE_LENGTH);

   /* Message Type */
   strcpy(temp_message_type,auth_tx_detail_arc.message_type);
   strncpy(temp_processing_code,auth_tx_detail_arc.processing_code,6);
   if (auth_tx_detail_arc.tx_key == AUTH_REFUND)
   {
	  strcpy(auth_tx_detail_arc.message_type,"0400"); /* For Reversal Transaction */
   }
   else
	      strcpy(auth_tx_detail_arc.message_type,"0000"); /* For Original Transaction */

	/* Terminal ID - Left Justified ; Space Filled */

    if(true == Check_If_AMEX_Transaction_arc()) /* AMEX Terminal Id - 15 chars + 1 Space Filled */
	 {
		memcpy(&term_id,auth_tx_detail_arc.primary_key.device_id,15);
		
	}
	else    /* VISA,JCB,MCARD Terminal ID */
	{
		memcpy(&term_id,auth_tx_detail_arc.primary_key.device_id,8);
		memset(term_id+8,' ',7);	
		
	}

    if(true == Check_If_AMEX_Transaction_arc()) /* AMEX Card Number - 15 numeric + 4 Space Filled */
	 {
		memcpy(&card_nbr,auth_tx_detail_arc.card_nbr,15);
		memset(&card_nbr[15],0x20,4);		
	}
    else if(true == Check_If_CUP_Transaction_arc_epos())
	{
    	memset(card_nbr, 0x20,(sizeof(card_nbr)-1));
		memcpy(&card_nbr,auth_tx_detail_arc.card_nbr,strlen(auth_tx_detail_arc.card_nbr));
	}
	else                                          /* VISA,JCB,MC Card Number - 16 numeric + 3 Space Filled */
	{
		memcpy(&card_nbr,auth_tx_detail_arc.card_nbr,16);
		memset(&card_nbr[16],0x20,3);
		
	}

	  if(auth_tx_detail_arc.tx_key == AUTH_REVERSAL)  /* Processing Code */
		{
		  memcpy(&auth_tx_detail_arc.processing_code, "20", 2 );
		}
	  if((0 == strcmp("0320",auth_tx_detail_arc.message_type)) || (auth_tx_detail_arc.processing_code[5] == '1'))  /* Batch Upload */
	  {
		  memcpy(&auth_tx_detail_arc.processing_code, "000000", 6 );
	  }
	  if((auth_tx_detail_arc.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION) || (auth_tx_detail_arc.tx_key == AUTH_CASH_ADVANCE) || (strcmp(auth_tx_detail_arc.processing_code,"010000") == 0)) /* Cash Advance */

	  {
		  if(true == Check_If_AMEX_Transaction_arc())
		  {
			  memcpy(&auth_tx_detail_arc.processing_code, "014008", 6 );
		  }
		  else
		  {
		  memcpy(&auth_tx_detail_arc.processing_code, "010000", 6 );
	  }
	  }
	  if( true == Check_If_VISA_Transaction_arc() ||
		  true == Check_If_JCB_Transaction_arc()  ||
		  true == Check_If_AMEX_Transaction_arc())
	  {
		  // Its Quasi cash for Visa
		  if(0 == strncmp(temp_processing_code,"11",2))
		  {
			  memcpy(&auth_tx_detail_arc.processing_code, "110000", 6 );
		  }
	  }
	  else if(true == Check_If_MC_Transaction_arc())
	  {
		  // Its Quasi cash for MC
		  if(0 == strncmp(temp_processing_code,"11",2))
		  {
			  memcpy(&auth_tx_detail_arc.processing_code, "000000", 6 );
		  }
	  }
   
	 /* POS Data Logic */
	 
	   if(true == Check_If_JCB_Transaction_arc()) /* JCB Card POS Data */
	{
	       /* card swipe */

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
		   {
               strncpy(pos_data,"022000022",9);
		   }

		   /*Contactless magstripe */
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"091",3) == 0)
		   {
               strncpy(pos_data,"910000022",9);
		   }

		   /* manual entry */

           else if((strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0) &&
        		   (0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"00",2)))
           {
               strncpy(pos_data,"011000022",9);
           }
         
		   /* CHIP */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
           {
               strncpy(pos_data,"051000022",9);
           }

		   /*Contactless EMV*/
           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"007",3) == 0)
           {
                   strncpy(pos_data,"071000022",9);
           }

		   /* Fallback */

		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
		   {
               strncpy(pos_data,"971000022",9);
		   }

		   /* New POS entry mode for BDO - TF Phani*/
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
			 {
				strncpy(pos_data,"971000022",9);
			 }
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
			 {
				strncpy(pos_data,"971000022",9);
			 }
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
			 {
				strncpy(pos_data,"971000022",9);
			 }
			 else if(0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"05",2)) //BDO PAY QR
			 {
			 	strncpy(pos_data,"012050012   ",12);
			 }


		    /* E-Commerce */

		   if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) ||
			   (strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4) == 0))
		   {
               strncpy(pos_data,"812000012",9);
		   }

		   /* RPS  */

		    iIndustry = atoi(terminal.industry_code );

		   if((strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0))
		   {
			   strncpy(pos_data,"102000021   ",12);
		   }			   

#if BDOR_68_REQ_2_JCB

		    else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin_arc())
		    {
			   strncpy( pos_data,
					    JCB_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
						JCB_POS_DATA_LEN_FOR_EPOS );
		   }
		    else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin_arc())
			{
			   strncpy( pos_data,
					    JCB_MOBILE_NFC_TAP2PHONE_TXN_TXN_WITHOUT_PIN_POS_DATA,
						JCB_POS_DATA_LEN_FOR_EPOS );
		   }
#endif
#if BDOR_68_REQ_4_JCB

		   else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin_arc())
		   {
			   strncpy( pos_data,
					    JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
						JCB_POS_DATA_LEN_FOR_EPOS );
		   }
		   else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin_arc())
		   {
			   strncpy( pos_data,
					    JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
						JCB_POS_DATA_LEN_FOR_EPOS );
		   }
#endif
#if BDOR_68_REQ_6_JCB
	   else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trx_arc())
		{
		   strncpy( pos_data,
					JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					JCB_POS_DATA_LEN_FOR_EPOS );
		}
#endif



		   /* ATM - For checking pin exists or not */
		   if(auth_tx_detail_arc.pos_entry_mode[3]=='1')
		   {
               memset(&pos_data[2],0x31,1);
		   }
		   else
		   {
			   memset(&pos_data[2],0x32,1);
		   }
		   
	  //}
	}
	else if(true == Check_If_CUP_Transaction_arc_epos()) /* CUP Card POS Data */
	{
		if(0==strncmp(Incremental_Auth_flag_CUP,"1",1))
		{

			if(strncmp(auth_tx_detail_arc.product_codes[4].code,"procsd",6)==0)
			{

				if(strlen(cupMerchantID)>0)
				{
					strcpy(auth_tx_detail_arc.merchant_id,cupMerchantID);
				}
				strncpy(pos_data, auth_tx_detail_arc.product_codes[16].amount,
						CUP_EPOS_POS_DATA_LEN);
				if (strncmp(auth_tx_detail_arc.product_codes[6].amount, "08", 2)== 0)
				{

					strncpy(CUP_Pos_input_mode,
							auth_tx_detail_arc.pos_entry_mode + 1, 3);
				}

				/* BDO Pay*/
				else if (strncmp(auth_tx_detail_arc.product_codes[6].amount,"05", 2) == 0)
				{

					strncpy(CUP_Pos_input_mode,
							auth_tx_detail_arc.pos_entry_mode + 1, 3);
				}
				/* card swipe */

				else if (strncmp(auth_tx_detail_arc.pos_entry_mode, "002", 3)== 0)
				{

					strncpy(CUP_Pos_input_mode,
							auth_tx_detail_arc.pos_entry_mode + 1, 3);
				}
				/* manual entry */

				else if (strncmp(auth_tx_detail_arc.pos_entry_mode, "001", 3)== 0)
				{

					strncpy(CUP_Pos_input_mode,
							auth_tx_detail_arc.pos_entry_mode + 1, 3);
				}

				/* CHIP */

				else if (strncmp(auth_tx_detail_arc.pos_entry_mode, "005", 3)== 0)
				{

					strncpy(CUP_Pos_input_mode,
							auth_tx_detail_arc.pos_entry_mode + 1, 3);
				}

				/* Fallback */

				else if (strncmp(auth_tx_detail_arc.pos_entry_mode, "08", 2)== 0)
				{

					/* CUP POS Input Mode (Segment 10, Pos 262-264) for Fallback transaction as "022"*/
					strncpy(CUP_Pos_input_mode, "022", 3);
				}

				/* Contact less */
				else if (strncmp(auth_tx_detail_arc.pos_entry_mode, "007", 3)== 0)
				{

					strncpy(CUP_Pos_input_mode,
							auth_tx_detail_arc.pos_entry_mode + 1, 3);
				}

				iIndustry = atoi(terminal.industry_code);
				if (iIndustry == 500)
				{
					/* CUP MOTO EPOS changes*/
					memset(CUP_Pos_input_mode, 0x00,
							sizeof(CUP_Pos_input_mode));
					strncpy(CUP_Pos_input_mode, "012", 3);
				}
			}
			else
			{
				/* USE the merchant ID not CUP Acceptor ID.*/
				if(strlen(cupMerchantID)>0)
				{
					strcpy(auth_tx_detail_arc.merchant_id,cupMerchantID);
				}

				/* RPS, Asiapay Recurring */
				if (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0)
				{
					strncpy(pos_data,CUP_RPS_ASIAPAY_RECURRING,CUP_EPOS_POS_DATA_LEN);
					strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
				}

				/* BDO Pay*/
				else if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"05",2) == 0)
				{
					strncpy(pos_data,CUP_BDO_PAY,CUP_EPOS_POS_DATA_LEN);
					strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
				}
			   /* card swipe */

				else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
			   {
				   strncpy(pos_data,CUP_MAGAGSTRIPE_SWIPE,CUP_EPOS_POS_DATA_LEN);
				   strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
			   }
			   /* manual entry */

			   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0)
			   {
				   strncpy(pos_data,CUP_POS_MANUAL_KEYIN,CUP_EPOS_POS_DATA_LEN);
				   strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
			   }

			   /* CHIP */

			   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
			   {
				   strncpy(pos_data,CUP_CHIP_CONTACT,CUP_EPOS_POS_DATA_LEN);
				   strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
			   }

			   /* Fallback */

			   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"08",2) == 0)
			   {
				   strncpy(pos_data,CUP_CHIP_FALLBACK,CUP_EPOS_POS_DATA_LEN);
				   /* CUP POS Input Mode (Segment 10, Pos 262-264) for Fallback transaction as "022"*/
				   strncpy(CUP_Pos_input_mode,"022",3);
			   }

			   /* Contact less */
			   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"007",3) == 0)
			   {
				  strncpy(pos_data,CUP_CHIP_CONTACTLESS,CUP_EPOS_POS_DATA_LEN);
				  strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
			   }

			   iIndustry = atoi(terminal.industry_code );
			   if(iIndustry == 500)
			   {
				   /* CUP MOTO EPOS changes*/
				   memset (CUP_Pos_input_mode, 0x00, sizeof(CUP_Pos_input_mode));
				   strncpy(CUP_Pos_input_mode,"012",3);
			   }
			}
		}
		else
		{
			if(strlen(cupMerchantID)>0)
			{
				strcpy(auth_tx_detail_arc.merchant_id,cupMerchantID);
			}

			/* RPS, Asiapay Recurring */
			if (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0)
			{
				strncpy(pos_data,CUP_RPS_ASIAPAY_RECURRING,CUP_EPOS_POS_DATA_LEN);
				strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
			}

			/* BDO Pay*/
			else if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"05",2) == 0)
			{
				strncpy(pos_data,CUP_BDO_PAY,CUP_EPOS_POS_DATA_LEN);
				strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
			}
		   /* card swipe */

			else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
		   {
			   strncpy(pos_data,CUP_MAGAGSTRIPE_SWIPE,CUP_EPOS_POS_DATA_LEN);
			   strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
		   }
		   /* manual entry */

		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0)
		   {
			   strncpy(pos_data,CUP_POS_MANUAL_KEYIN,CUP_EPOS_POS_DATA_LEN);
			   strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
		   }

		   /* CHIP */

		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
		   {
			   strncpy(pos_data,CUP_CHIP_CONTACT,CUP_EPOS_POS_DATA_LEN);
			   strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
		   }

		   /* Fallback */

		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"08",2) == 0)
		   {
			   strncpy(pos_data,CUP_CHIP_FALLBACK,CUP_EPOS_POS_DATA_LEN);
			   /* CUP POS Input Mode (Segment 10, Pos 262-264) for Fallback transaction as "022"*/
			   strncpy(CUP_Pos_input_mode,"022",3);
		   }

		   /* Contact less */
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"007",3) == 0)
		   {
			  strncpy(pos_data,CUP_CHIP_CONTACTLESS,CUP_EPOS_POS_DATA_LEN);
			  strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
		   }

		   iIndustry = atoi(terminal.industry_code );
		   if(iIndustry == 500)
		   {
			   /* CUP MOTO EPOS changes */
			   memset (CUP_Pos_input_mode, 0x00, sizeof(CUP_Pos_input_mode));
			   strncpy(CUP_Pos_input_mode,"012",3);
		   }

		}

#if BDOR_68_REQ_2_UPI

    	 if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin_arc())
		 {
			   strncpy( pos_data,
					    UPI_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
						UPI_POS_DATA_LEN_FOR_EPOS );
			   strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
		 }
    	 else  if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin_arc())
		 {
			   strncpy( pos_data,
					    UPI_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
						UPI_POS_DATA_LEN_FOR_EPOS );
			   strncpy(CUP_Pos_input_mode,auth_tx_detail_arc.pos_entry_mode +1,3);
		 }
#endif
#if BDOR_68_REQ_4_UPI

    	else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin_arc())
		{
		   strncpy( pos_data,
				    UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
					UPI_POS_DATA_LEN_FOR_EPOS );
		   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		}
    	else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin_arc())
		{
		   strncpy( pos_data,
				    UPI_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
					UPI_POS_DATA_LEN_FOR_EPOS );
		   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		}
#endif
#if BDOR_68_REQ_6_UPI

    	if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trx_arc())
		{
		   strncpy( pos_data,
				    UPI_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					UPI_POS_DATA_LEN_FOR_EPOS );
		   strncpy(CUP_Pos_input_mode,auth_tx_detail.pos_entry_mode +1,3);
		}
#endif
	}

	else if(true == Check_If_VISA_Transaction_arc()) /* VISA POS Data */
	{
		  /*
		 *  Let us check if we have already pos data generated during authorization
		 */

		/*if(strlen(auth_tx_detail_arc.product_codes[16].amount) > 0 )
		{
			strncpy( pos_data, auth_tx_detail_arc.product_codes[16].amount,
				   EPOS_POS_DATA_SIZE);
		}
		else
		{*/
		  		  
		  /* card swipe */

           if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
           {
               strncpy(pos_data,"900050000",9);
           }
		   /* manual entry */

           else if((strncmp(auth_tx_detail_arc.pos_entry_mode,"0012",4) == 0) &&
        		   (0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"00",2)))
           {
               strncpy(pos_data,"012050000",9);
           }
           else if((strncmp(auth_tx_detail_arc.pos_entry_mode,"0011",4) == 0) &&
                  (0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"00",2)))
           {
                                strncpy(pos_data,"011052000",9);
           }
         
		   /* CHIP */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
           {
               strncpy(pos_data,"050050000",9);
           }
	       else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"007",3) == 0)
	       {
               strncpy(pos_data,"072050000",9);
	       }
	       else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"091",3) == 0)
	       {
	                       strncpy(pos_data,"912052000",9);
	       }
		   /* FALLBACK */

		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
		   {
               strncpy(pos_data,"902052000",9);
		   }
			        /* New POS entry mode for BDO - TF Phani*/
			else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
			{
               strncpy(pos_data,"902052000",9);
			}
            else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
            {
               strncpy(pos_data,"902052000",9);
            }
            else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
            {
               strncpy(pos_data,"902052000",9);
            }
            else if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"05",2)))
            {
               strncpy(pos_data,"012010000   ",12);
            }

		   /* E-Commerce */

		    if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4) == 0))
			{  	
				strncpy(pos_data,"010590000",9);
					/*ECI value for VISA - ECI value should be copied to 88-89 position*/
				strncpy(eci,"07 ",3);
				if(strcmp(auth_tx_detail_arc.profile,"")!=0)
				{
					strncpy(eci,auth_tx_detail_arc.profile,2);
				}
				strncpy(pos_data+7,eci,2);
	 
			}
		     /* RPS */
		    else if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) 
		    {
		    	strncpy(pos_data,"102010000",9);
		    	memset(&pos_data[9],0x52,1);
		    }
#if BDOR_68_REQ_2_VISA

		    else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin_arc())
		    {
			   strncpy( pos_data,
					    VISA_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
						VISA_POS_DATA_LEN_FOR_EPOS );
		   }
		    else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin_arc())
			{
			   strncpy( pos_data,
					    VISA_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
						VISA_POS_DATA_LEN_FOR_EPOS );
		   }
#endif
#if BDOR_68_REQ_4_VISA

		    else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin_arc())
		    {
			   strncpy( pos_data,
					    VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
						VISA_POS_DATA_LEN_FOR_EPOS );
		   }
		    else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin_arc())
			{
			   strncpy( pos_data,
					    VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
						VISA_POS_DATA_LEN_FOR_EPOS );
		   }
#endif
#if BDOR_68_REQ_6_VISA
		   else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trx_arc())
			{
			   strncpy( pos_data,
						VISA_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
						VISA_POS_DATA_LEN_FOR_EPOS );
			}
#endif

		   if(0 != strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) ) /* Rec Txn - R ,Non-Rec Txn - Space */
		   {
			   memset(&pos_data[9],0x20,1);
		   }

		   /* ATM - For checking pin exists or not */ 

		   if(auth_tx_detail_arc.pos_entry_mode[3]=='1')
		   {
               memset(&pos_data[2],0x31,1);
		   }
		   else
		   {
			   memset(&pos_data[2],0x32,1); 
		   }

		   iIndustry = atoi(terminal.industry_code );

		   if (iIndustry == MAIL_PHONE) /* MOTO */
		   {
				if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0 ||
				   strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0 ||
				   strncmp(auth_tx_detail_arc.pos_entry_mode,"007",3) == 0 ||
				   strncmp(auth_tx_detail_arc.pos_entry_mode,"091",3) == 0	||
				   strncmp(auth_tx_detail_arc.pos_entry_mode,"090",3) == 0	||
				   strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0 ||
				   strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
				{
					strncpy(pos_data+7,"00",2);
				}
				else if((strncmp(auth_tx_detail_arc.pos_entry_mode,"0012",4) == 0 ||
						strncmp(auth_tx_detail_arc.pos_entry_mode,"0011",4) == 0) &&
						strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) != 0)
				{
					strncpy(pos_data+7,"01",2);
				}
			}
	 }

     else if(true == Check_If_AMEX_Transaction_arc()) /* AMEX POS Data */
	 {
    	 /*
		  *  Let us check if we have already pos data generated during authorization
		  */
    	 if(strlen(auth_tx_detail_arc.product_codes[16].amount) > 0 )
		 {
			 strncpy(temp_Amex_POS_DATA, auth_tx_detail_arc.product_codes[16].amount,
					 EPOS_POS_DATA_SIZE);
			 strTrim (temp_Amex_POS_DATA, pos_data) ;
		  }

		  if(EPOS_POS_DATA_SIZE == strlen(pos_data))
		  {
			/* We have populated Pos data correctly */
		  }
		  else
		  {
        
			 /* We need only last 3 characters of POS entry mode
			  * coming from terminal for processing further.
			  */
			 if((strncmp(auth_tx_detail_arc.processing_code,"0040",4) == 0) && (strncmp(auth_tx_detail.message_type,"0220",4) == 0) )
			 {
				strncpy(auth_tx_detail_arc.processing_code,"0000",4); // Req from BDO for AMEX proc code
			 }

			 if(strlen(auth_tx_detail_arc.pos_entry_mode) > 3)
			 {
				strncpy(temp_entry_mode,&auth_tx_detail_arc.pos_entry_mode[1],3);
			 }
			 else
			 {
				strcpy(temp_entry_mode,auth_tx_detail_arc.pos_entry_mode);
			 }
#if BDOR_68_REQ_2_AMEX

    	 if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin_arc())
		 {
			   strncpy( pos_data,
					    AMEX_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
						AMEX_POS_DATA_LEN_FOR_EPOS );
		 }
    	 else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin_arc())
		 {
			   strncpy( pos_data,
					    AMEX_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PINPOS_DATA,
						AMEX_POS_DATA_LEN_FOR_EPOS );
		 }
#endif
#if BDOR_68_REQ_4_AMEX
    	  else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin_arc())
		  {
			   strncpy( pos_data,
					    AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
						AMEX_POS_DATA_LEN_FOR_EPOS );
		  }
    	  else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin_arc())
		  {
			   strncpy( pos_data,
					    AMEX_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PINPOS_DATA,
						AMEX_POS_DATA_LEN_FOR_EPOS );
		  }
#endif
#if BDOR_68_REQ_6_AMEX
	     else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trx_arc())
		 {
		   strncpy( pos_data,
					AMEX_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					AMEX_POS_DATA_LEN_FOR_EPOS );
		}
#endif
			   /* CHIP */

			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
			 {
				   strncpy(pos_data,
						   AMEX_CHIP_POS_DATA_WITH_PIN,
						   EPOS_POS_DATA_SIZE);
			 }
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"007",3) == 0)
			 {
				 strncpy(pos_data,
						 AMEX_CONTACTLESS_CHIP_POS_DATA_WITH_PIN,
						 EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"02",2) == 0) &&
					 (strncmp(temp_entry_mode,"021",3) != 0)) /* card swip without 4DBC */
			 {
				strncpy(pos_data,
						AMEX_MASGTRIP_POS_DATA_WITHOUT_PIN,
						EPOS_POS_DATA_SIZE);
			 }

			 else if(strncmp(temp_entry_mode,"91",2) == 0)
			 {
				strncpy(pos_data,
						AMEX_CONTACTLESS_MAGSTRIP_POS_DATA_WITHOUT_PIN,
						EPOS_POS_DATA_SIZE);
			 }

			 else if(strncmp(temp_entry_mode,"06",2) == 0) /* card swip with 4DBC */
			 {
				strncpy(pos_data,"260101W54140",EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"01",2) == 0)	 &&
					 (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) != 0) &&
					 (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) != 0)) /* manual key in without 4DBC */
			 {
				strncpy(pos_data,
						AMEX_POS_MANUAL_KEYIN_POS_DATA_WITH_PIN,
						EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"07",2) == 0) &&
					 (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) != 0) &&
					 (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) != 0)) /* manual key in with 4DBC */
			 {
				strncpy(pos_data,
						AMEX_CONTACTLESS_CHIP_POS_DATA_WITH_PIN,
						EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"012",3) == 0) &&
					 (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) &&
					  ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */
																	 /* Changed the code for POS entry mode of position 5 from'2' to '9' */											  /* Girija Y TF, March 02, 2009 */ /* RPS Transactions without 4DBC */
			 {
				strncpy(pos_data,
						AMEX_RPS_ASIAPAY_RPS_POS_DATA,
						EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"07",2) == 0) &&
					 (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) &&
					  ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
																										  /* Girija Y TF, March 02, 2009 */ /* RPS Transactions with 4DBC */
			 {
				strncpy(pos_data,"660390S64130",EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"01",2) == 0) &&
					 (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) == 0)) /* Ecommerce without 4DBC */
			 {
				strncpy(pos_data,
						AMEX_ECOMM_ASIAPAY_ECOM_POS_DATA,
						EPOS_POS_DATA_SIZE);
			 }

			 else if((strncmp(temp_entry_mode,"07",2) == 0) &&
					 (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) == 0)) /* Ecommerce with 4DBC */
			 {
				strncpy(pos_data,"6609S0SS4140",EPOS_POS_DATA_SIZE);
			 }

			 else if(strncmp(temp_entry_mode,"021",3) == 0) /* ATM */
			 {
				strncpy(pos_data,
						AMEX_MAGSTRIP_POS_DATA_WITH_PIN,
						EPOS_POS_DATA_SIZE);
			 }

			 else if(strncmp(temp_entry_mode,"80",2) == 0 ||
					  strncmp(temp_entry_mode,"802",3) == 0) /*Fall Back */
			 {
					strncpy(pos_data,
							AMEX_FALLBACK_POS_DATA_WITH_PIN,
							EPOS_POS_DATA_SIZE);
			 }

			 if(strlen(pos_data) == 0 &&
				strncmp(auth_tx_detail_arc.pos_entry_mode,"080",3) == 0)
			 {
				 strncpy(pos_data,
						 AMEX_FALLBACK_POS_DATA_WITH_PIN,
						 EPOS_POS_DATA_SIZE);
			 }
		}
	}

	else if(true == Check_If_MC_Transaction_arc()) /* Master Card POS Data */
	 {		
		   /* e-commerce  */
		   if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) ||
			 (0==strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4)))
		   {
			   strncpy(pos_data,"810125106600",EPOS_POS_DATA_SIZE);
		   }
			/* RPS */
		   else if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0)
		   {
			   strncpy(pos_data,"100234100100",EPOS_POS_DATA_SIZE);
		   }
		   /* manual entry */
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0012",4) == 0 && (0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"00",2)))
		   {
			   strncpy(pos_data,"012001110300",EPOS_POS_DATA_SIZE);
		   }
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0011",4) == 0 && (0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"00",2)))
		   {
		   		strncpy(pos_data,"011231100100",EPOS_POS_DATA_SIZE);
		   }
		   /* card swipe */
		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
		   {
			   strncpy(pos_data,"900000010300",EPOS_POS_DATA_SIZE);
		   }
		   /* CHIP */
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
		   {
			   strncpy(pos_data,"050000010300",EPOS_POS_DATA_SIZE);
		   }
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"007",3) == 0) /*Pay pass CHip Contactless*/
		   {
			   strncpy(pos_data,"070000010300",EPOS_POS_DATA_SIZE);
		   }
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"091",3) == 0) /*MC Pay pass -Contactless Mag stripe*/
		   {
			   strncpy(pos_data,"910000010300",EPOS_POS_DATA_SIZE);
		   }
		   /* FALLBACK */
		   /* New POS entry mode added for MC */
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
		   {
			   strncpy(pos_data,"800000010300",EPOS_POS_DATA_SIZE);
		   }
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
		   {
			   strncpy(pos_data,"801000010300",EPOS_POS_DATA_SIZE);
		   }
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
		   {
			   strncpy(pos_data,"802000010300",EPOS_POS_DATA_SIZE);
		   }
		   else if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"05",2) == 0)
		   {   /*BDO Pay*/
			   strncpy(pos_data,"012231100100",EPOS_POS_DATA_SIZE);
		   }
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"009",3) == 0) /*MC Pay pass -Contactless Mag stripe*/
		   {
			   strncpy(pos_data,"091000000400",EPOS_POS_DATA_SIZE);
		   }
		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
		   {
			   strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);
		   }
#if BDOR_68_REQ_2_MC

			if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin_arc())
			{
			   strncpy( pos_data,
					    MC_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_POS_DATA,
						MC_POS_DATA_LEN_FOR_EPOS );
			}
			else if(true == merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin_arc())
			{
			   strncpy( pos_data,
					    MC_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_POS_DATA,
						MC_POS_DATA_LEN_FOR_EPOS );
			}
#endif
#if BDOR_68_REQ_4_MC

			else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin_arc())
			{
			   strncpy( pos_data,
					   	MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_POS_DATA,
						MC_POS_DATA_LEN_FOR_EPOS );
			}
			else if(true == merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin_arc())
			{
			   strncpy( pos_data,
					    MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_POS_DATA,
						MC_POS_DATA_LEN_FOR_EPOS );
			}
#endif
#if BDOR_68_REQ_6_MC
		   else if(true == merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trx_arc())
			{
			   strncpy( pos_data,
						MC_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
						MC_POS_DATA_LEN_FOR_EPOS );
			}
#endif
		   iIndustry = atoi(terminal.industry_code );
			 /* ATM - For checking pin exists or not */
			if (iIndustry == MAIL_PHONE) /* MOTO */
			{
				strncpy(pos_data,&auth_tx_detail_arc.pos_entry_mode[1],3);
				strncpy(pos_data+3,"232100000",9);
			}

		    if(auth_tx_detail_arc.pos_entry_mode[3]=='1')
		   {
				   memset(&pos_data[2],0x31,1);
		   }
		    else if(auth_tx_detail_arc.pos_entry_mode[3]=='0')
		    {
		    	memset(&pos_data[2],0x30,1);
		    }
		   else
		   {
				   memset(&pos_data[2],0x32,1);
		   }
	 } 

	else if (true == Check_If_Diners_Transaction_arc())
	{
		/*  Let us check if we have already pos data generated during authorization */
		if(strlen(auth_tx_detail_arc.product_codes[16].amount) > 0 )
		{
			strncpy( temp_DCI_POS_DATA, auth_tx_detail_arc.product_codes[16].amount,
					 EPOS_POS_DATA_SIZE);
			strTrim (temp_DCI_POS_DATA, pos_data);
		}
		 ;
		if(EPOS_POS_DATA_SIZE == strlen(pos_data))
		{
			/* We have populated Pos data correctly*/
		}
		else
		{
			memset( pos_data, 0x00, sizeof(pos_data));
			Diners_Prepare_posdata_for_DINERS_arc();
			strncpy( pos_data, auth_tx_detail_arc.product_codes[16].amount,
					 EPOS_POS_DATA_SIZE);
		}
	}
	
	/* Ecommerce Indicator Logic */

	   if(true == Check_If_AMEX_Transaction_arc())
	   {
		  if((0 == strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) &&
			 (0 == strncmp(temp_message_type,"0200",4)) &&
			 ( (0 == strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3)) ||
			   (0 == strncmp(auth_tx_detail_arc.pos_entry_mode,"01",2))))
		  {
				if(strcmp(auth_tx_detail_arc.profile,"")!=0)
				{
					strncpy(eci,auth_tx_detail_arc.profile,2);
					eci[2] = ' ';
				}
			    else
				  strncpy(eci,"07 ",3);
		  }
		  else
		  {
			  strncpy(eci," ",3);
		  }
   }

	 else if(true == Check_If_MC_Transaction_arc()) /* Master Card ECOM Indicator */
	
	 {
           if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4) == 0)) /* If E-commerce txn */
		   {
			   if(auth_tx_detail_arc.product_codes[13].quantity[0]!='\0')
			   {
			     strncpy(eci,"21",2);
			     eci[2] = auth_tx_detail_arc.product_codes[13].quantity[1];
			   }
			   else
			   {
				   strcpy(eci,"210");
			   }
		   }
		   else
		   {
			   strcpy(eci,"000");			   
		   }
     }

	 else if(true == Check_If_JCB_Transaction_arc()) /* JCB Card ECOM Indicator */
	 {

	    if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) || (0==strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4)))) /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail_arc.profile,"")!=0)
			   {
				 strcpy(eci,auth_tx_detail_arc.profile);
				 strncpy(eci+2," ",1);				 
			   }
			   else strncpy(eci,"07 ",3);

		   }
		   else
		   {
			  strncpy(eci,"   ",3);
			   
		   }
	 }
	 else if(true == Check_If_CUP_Transaction_arc_epos())
	 	{
	 	strncpy(eci,"   ",3);
	 	}




	 /* PAN Sequence Number */

	/* if(strcmp(emv_record.tag,"5F34")==0)
	 {
		 strcpy(pan_sequence_number,emv_record.emv_data);
	 }
	 else */
	strncpy(pan_sequence_number,"000",3);

	 if(true == Check_If_AMEX_Transaction_arc()) /* For AMEX Card Sequence Number is Zero */
	 {
		 strncpy(pan_sequence_number,"000",3);
	 }
	 else if (true == Check_If_Diners_Transaction_arc())
	 {
		 if(strlen(auth_tx_detail_arc.product_codes[11].code)> 0)
		 {
			 strncpy(pan_sequence_number,auth_tx_detail_arc.product_codes[11].code,3);
		 }
		 else
		 {
			 strncpy(pan_sequence_number,"001",3);
		 }
	 }

	  strcpy(tran_dt,auth_tx_detail_arc.tran_date + 2);

	  if(strncmp(auth_tx_detail_arc.product_codes[6].amount+2,"",3)==0)
	  {
		  memcpy(auth_tx_detail_arc.product_codes[6].amount+2,"000",3);
	  }
	  else if(strncmp(auth_tx_detail_arc.product_codes[6].amount+2,"  ",2)==0)
	  {
		  memcpy(auth_tx_detail_arc.product_codes[6].amount+2,"000",3);
	  }
	  strncpy(temp_service_code,auth_tx_detail_arc.product_codes[6].amount+2,3);
	  strTrim(temp_service_code,service_code);

	  if(3 == strlen(service_code))
	  {
		  // We are able to retive service code from DB.
	  }
	  else
	  {
		  // Service code detaisl look incorrect.
		  memcpy(auth_tx_detail_arc.product_codes[6].amount+2,"000",3);
	  }
	 /* Form 27 byte message for DE 63- table Id 12, TF Phani- 
	  First 15 bytes in - BCh10 -> ticket_nbr 
	  Next  10 bytes in - BCh10 -> product_codes[14].amount*/
	/* TF-- Abhishek
	1) If Field 63 is present in the authorisation message and starts with "PRCHID", place the value of Field 63 in EPOS at segment 11 starting at
	    column 76 until 106.
	2) If Field 63 is present in the authorisation message and doesn't start with "PRCHID", place the values of Field 63 at segment 11 starting at column
	   82 until 106.
	Note :Item 1 is to address for RPS transactions. Item 2 is to address for PolicyNumber coming from POS terminal.
	*/
	if(strncmp(auth_tx_detail_arc.ticket_nbr,"PRCHID",CUSTOM_IDENTIFIER_TYPE_LENGTH)== 0)
	{
		// Print 31 byte starting from 76 until 106
		strncpy(perch_id,auth_tx_detail_arc.ticket_nbr,strlen(auth_tx_detail_arc.ticket_nbr));
		strncat(perch_id,auth_tx_detail_arc.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail_arc.ticket_nbr) - CUSTOM_IDENTIFIER_TYPE_LENGTH +1));
	    perch_id[PERCH_ID_LENGTH -1]='\0';
	}
	else
	{
		// 1st 6 byte should be space as feild 63 is not starts whith PRCHID 
		// copy data after 6 byte which will be store from 82nd byte location
		//Print 25 byte starting from 82 until 106
		strncpy(perch_id + CUSTOM_IDENTIFIER_TYPE_LENGTH ,auth_tx_detail_arc.ticket_nbr,strlen(auth_tx_detail_arc.ticket_nbr));
		strncat(perch_id,auth_tx_detail_arc.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail_arc.ticket_nbr)- CUSTOM_IDENTIFIER_TYPE_LENGTH +1 ));
		perch_id[PERCH_ID_LENGTH -1]='\0';
	}

     memcpy(cad_merchant_id,auth_tx_detail_arc.merchant_id+5, 10 ); /* Cadence Merchant ID */

    /* Transaction ID */

	   if(true == Check_If_MC_Transaction_arc()) /* Transaction ID for MASTER CARDS */
	
	 {    
		 // Settlement date MMDD
			strncpy(settle_date,auth_tx_detail_arc.product_codes[19].code,4);//MMDD
			/*If Settle date is NUll, copy tran date as settle date*/
			if(strlen(settle_date) == 0 || settle_date[0] ==  ' ' || settle_date[1] ==  ' ')
			memcpy(settle_date,&auth_tx_detail_arc.tran_date[4],4); 
			
			if(strlen(settle_date) == 0 || settle_date[0] ==  ' ' || settle_date[1] ==  ' ')
			memcpy(settle_date,CurrentDate+2,4);
			
		  if(strlen(auth_tx_detail_arc.mcard_banknet)>0)
		 {
			 if(strlen(auth_tx_detail_arc.mcard_banknet)>12)
			 {
			   strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,12);
			   strncat(tran_id,"   ",3);
			   strncpy(&tran_id[9],settle_date,4);
			 }
			 else 
			 {
				  memset(tran_id,' ',15);
				 strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,strlen(auth_tx_detail_arc.mcard_banknet));
			     strncpy(&tran_id[9],settle_date,4);
			 }
		 }
		 else
		 {
			 memset(tran_id,' ',15);
 			if(strlen(settle_date) == 0)
			memcpy(settle_date,CurrentDate+2,4);  

			 strncpy(&tran_id[9],settle_date,4);
		 } 
	 }

	 else if(true == Check_If_VISA_Transaction_arc()) /* Transaction ID for VISA */
	 {
		memset(tran_id,0x30,15);	
		strncpy(tran_id,&auth_tx_detail_arc.mcard_banknet[1],15);
	 }

	 else if(true == Check_If_AMEX_Transaction_arc()) /* Transaction ID for AMEX */
	 {
		 strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,15); 
	 }
	 else if(true == Check_If_JCB_Transaction_arc()) /* Transaction ID for JCB - Spaces */
	 {
		 memset(&tran_id,' ',15); 
	 }
	 else if(true == Check_If_CUP_Transaction_arc_epos()) /* Transaction ID for CUP - Spaces */
	 {
		 memset(&tran_id,' ',15); 
	 }
	 else if(true == Check_If_Diners_Transaction_arc()) /* Transaction ID for JCB - Spaces */
	 {
		 len = strlen (auth_tx_detail_arc.product_codes[13].amount);
		 if(len > 0 )
		 {
			 memcpy(temp_DCI_tran_id,auth_tx_detail_arc.product_codes[13].code,6);
			 memcpy(temp_DCI_tran_id +6,auth_tx_detail_arc.product_codes[13].amount,len);
		}
		else
		{
		   len = strlen (auth_tx_detail_arc.product_codes[13].code);
		   memcpy(temp_DCI_tran_id,auth_tx_detail_arc.product_codes[13].code,len);
		}
		strTrim (temp_DCI_tran_id, tran_id) ;
		if(strlen(tran_id) > 0)
		{
			// We have populated TID data correctly
		}
		else
		{
			memcpy(tran_id,auth_tx_detail_arc.transaction_id,15);
		}
	}
	  memset(c_record, 0, sizeof(c_record));
	

	 /* writing details to file */

	 /* Segment 10 */
	  if(true == Check_If_CUP_Transaction_arc_epos())
	 {
		 // check if we have original Satn store in product10_code filed which we have stored for offline transaction.
		 if(strlen(auth_tx_detail_arc.product_codes[9].code) > 0)
		 {
			 memcpy(CUP_trace_ID,auth_tx_detail_arc.product_codes[9].code,(sizeof(CUP_trace_ID) -1));
		 }
		 else
		 {
			 memcpy(CUP_trace_ID,auth_tx_detail_arc.sys_trace_audit_num,(sizeof(CUP_trace_ID) -1));
		 }

		 sprintf(c_record_cup, "%1s%4s%-15s%-019s%6s%6s%2s%6s%012s%06s%4s%-12s%04s%11s%12s%06s%03s%40s%012s%4s%29s%11s%03s%3s%2s%-3s%3s%6s%6s%10s%3s%4s%8s",
				 "1",
				 auth_tx_detail_arc.message_type,
				 term_id,		  
				 auth_tx_detail_arc.card_nbr,
				 tran_dt,
				 auth_tx_detail_arc.tran_time,
				 "10",	/* Segment 10 Data Follows */
				 auth_tx_detail_arc.processing_code,
				 auth_tx_detail_arc.tran_amount,
				 "0",
				 terminal.category_code,
				 pos_data, 
				 "0",
				 " ",
				 auth_tx_detail_arc.primary_key.retrieval_ref_num,
				 auth_tx_detail_arc.auth_number,		 
				 "0",
				 " ",
				 "0",
				 " ",
				 " ",
				 " ",
				 "0",
				 curr_code,
				 " ",
				 eci, /* E-Commerce Logic for MCRD,JCB ; VISA,AMEX - Spaces */
				 " ",
				 auth_tx_detail_arc.primary_key.batch_nbr,//240-245
				 CUP_trace_ID, //246-251
				 auth_tx_detail_arc.transmission_timestamp,//252-261
				 CUP_Pos_input_mode,//261-264
				 auth_tx_detail_arc.product_codes[19].code,//265-268
				 auth_tx_detail_arc.primary_key.device_id //269-276
				 );
			 
		 
			  strncpy(c_record_cup + 276, "\n\0", 2);
			   crec_count++;
	   
			if (write_to_CUP_credit_settle_file() == false)
			return (false); 	

			 /*Check if Exception is enable then create the excpetion file	 */
	 	 	if(CUP_EPOS_Exception_Flag == 1)
	  		{
				/* We need to generate exception file only for OFF line transaction*/
				if( 0 == strncmp(temp_message_type,"0220",4))
				{
					if (write_to_CUP_Exception_file_arc() == false)
					{
						 return (false);
					}
					E_crec_count++;
				}
			}
	 
	 }
	 else
	 {
	
      sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%6s%012s%06s%4s%-12s%04s%11s%12s%06s%03s%40s%012s%4s%29s%11s%03s%3s%2s%-3s%3s%6s",
         "1",
		 auth_tx_detail_arc.message_type,
         term_id,         
         auth_tx_detail_arc.card_nbr,
         tran_dt,
         auth_tx_detail_arc.tran_time,
         "10",  /* Segment 10 Data Follows */
         auth_tx_detail_arc.processing_code,
         auth_tx_detail_arc.tran_amount,
		 "0",
		 terminal.category_code,
		 pos_data, 
		 "0",
		 " ",
		 auth_tx_detail_arc.primary_key.retrieval_ref_num,
		 auth_tx_detail_arc.auth_number,         
		 "0",
		 " ",
		 "0",
		 " ",
		 " ",
		 " ",
		 "0",
		 curr_code,
		 " ",
		 eci, /* E-Commerce Logic for MCRD,JCB ; VISA,AMEX - Spaces */
		 " ",
		 auth_tx_detail_arc.primary_key.batch_nbr		 
         );

       strncpy(c_record + 245, "\n\0", 2);

	   
	   crec_count++;
	   
			if (write_to_credit_settle_file() == false)
			return (false); 	
	 	}
	 
	  cseg10_records++;

	  /* Segment 11 */

	  memset(c_record, 0, sizeof(c_record));
	  /* TF Abhishek
	   * copy for CUP or CUP branded card shall	  contain "CC"on Segment 11 Pos 159-173
	   * Pad the value with spaces to occupy the whole 159 to 173 position. */
	  if (true ==  Check_If_CUP_Transaction_arc_epos())
		{
			memcpy(tran_id," ",(sizeof(tran_id)-1));
			memcpy(tran_id, CUP_CARD_INDICATOR_FLAG,2 );
		}
	  if(auth_tx_detail_arc.product_codes[0].code[0] == '\0')
	  {
		  memcpy(auth_tx_detail_arc.product_codes[0].code," ",1);  //This is done to avoid the wrong formatting.
	  }

#ifdef MC02_APR2024
	  memcpy(add_token_resp, " ", 1);
	  if(true == Check_If_MC_Transaction_arc())
	  {
		  if(auth_tx_detail_arc.product_codes[10].amount[0] != '\0')
		  {
			  memcpy(tran_link_id, auth_tx_detail_arc.product_codes[10].amount, 10);
			  strncat(tran_link_id, auth_tx_detail_arc.product_codes[1].amount, 12);
		  }
			memset(plan_reg_sys_idntfr, 0x00, sizeof(plan_reg_sys_idntfr) -1);
	  }
#endif
	  else if (true == Check_If_VISA_Transaction_arc())
	  {
		  if (auth_tx_detail_arc.product_codes[1].amount[0] != '\0')
		  {
			  memcpy(plan_reg_sys_idntfr, auth_tx_detail_arc.product_codes[1].amount, 10);
		  }
			memset(tran_link_id, 0x00, sizeof(tran_link_id) -1);
	  }
	  else
	  {
				memset(tran_link_id, 0x00, sizeof(tran_link_id) -1);
				memset(plan_reg_sys_idntfr, 0x00, sizeof(plan_reg_sys_idntfr) -1);
	  }

	  /* TF PHANI- New DE63 message adititon*/
	  sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%4s%012s%03s%03s%-031s%4s%10s%1s%11s%15s%03s%8s%-15s%48s%1c%1c%-40s%-10s%3s",
         "1",
		 auth_tx_detail_arc.message_type,
         term_id,         
         auth_tx_detail_arc.card_nbr,
         tran_dt,
         auth_tx_detail_arc.tran_time,         
		 "11", /* Segment 11 Data Follows */
         auth_tx_detail_arc.exp_date,
		 "0",
		 auth_tx_detail_arc.product_codes[6].amount+2,
	     pan_sequence_number,
	     perch_id,
		 " ",
         cad_merchant_id,
		 " ",
		 " ",
		 " ",
		 "0",
		 " ",
         tran_id, 
		 " ",
		 auth_tx_detail_arc.product_codes[0].code[0],
		 add_token_resp[0],
		 tran_link_id,
		 plan_reg_sys_idntfr,
		 " "	 	          	 
         );

      strncpy(c_record + 276, "\n\0", 2);

	  crec_count++;	 

	  if (write_to_credit_settle_file() == false)
      return (false);

		 //Segment 12

	  if (true ==  Check_If_CUP_Transaction_arc_epos())
	 	  {
	 		  iIndustry = atoi(terminal.industry_code );
	 		  if (iIndustry == 500 ||
	 			  strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0)
	 		  {
	 			  // MOTO/RPS Iniatiation Mode
	 			  strncpy(trsn_intn_mode, "3", 1);
	 		  }
	 		  else
	 		  {
	 			  // Attended,POS Iniatiation Mode
	 			  strncpy(trsn_intn_mode, "0", 1);
	 		  }

	 		  memset(c_record, 0, sizeof(c_record));
	 			sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%11s%15s%011s%40s%1s%40s%2s%3s%2s%1s%1s%2s%2s%-35s%12s%1s%4s%1s%8s",
	 			   "1",
	 			   auth_tx_detail_arc.message_type,
	 			   term_id,
	 			   auth_tx_detail_arc.card_nbr,
	 			   tran_dt,
	 			   auth_tx_detail_arc.tran_time,
	 			   "12",
	 			   " ",
	 			   " ",
	 			"",
	 			" ",
	 			" ",
	 			" ",
	 			" ",
	 			" ",
	 			format_code,
	 			rpymt_type,
	 			rpymt_amt_indr,
	 			rpymt_nbr,
	 			rpymt_freq,
	 			perch_id,
	 			max_rpymt_amt,
	 			valdn_indr,
	 			cit_mit_indr,
	 			trsn_intn_mode,
	 			" "
	 			);

	 		strncpy(c_record + 245, "\n\0", 2);
	 		crec_count++;

	 	  if (write_to_credit_settle_file() == false)
	 		return (false);

	 	   }

	  else if ((true == Check_If_VISA_RPS_Transaction_arc() ||
		  true == Check_If_MC_RPS_Transaction_arc()) &&
		  0 == strncmp(auth_tx_detail_arc.product_codes[5].code, "OFFUS", 5)  )
	  {
		  if (true == Check_If_VISA_RPS_Transaction_arc())
		  {
			  strncpy(format_code, "RP", 2);
			  strncpy(rpymt_type, "2", 1);
			  strncpy(rpymt_amt_indr, "2" , 1);
			  strncpy(rpymt_nbr, "99", 2);
			  strncpy(rpymt_freq, "12",2);
			  strncpy(max_rpymt_amt, "999999999999", 12);
			  strncpy(valdn_indr, "1", 1);
			  strncpy(authn_char_indcr, auth_tx_detail_arc.mcard_banknet , 1);
			  strncpy(authn_resp_code, auth_tx_detail_arc.response_code, 2);
			  strncpy(card_hldr_idntfn_method, auth_tx_detail_arc.product_codes[5].quantity, 1);
			  strncpy(cash_bk_amt, auth_tx_detail_arc.down_payment, 9);
			  strncpy(valdn_code, auth_tx_detail_arc.interest_rate, 4);
			  strncpy(prdouct_id, auth_tx_detail_arc.product_codes[7].code, 2);
			  strncpy(spend_qualfn_indcr, &auth_tx_detail_arc.product_codes[7].code[2], 1);
			  strncpy(accnt_funding_src, &auth_tx_detail_arc.product_codes[5].quantity[1], 1);
			  strncpy(authrzd_amount, auth_tx_detail_arc.tran_amount, 12);
			  strncpy(authrzn_curr_code, auth_tx_detail_arc.term, 3);
		  }

		  if (true == Check_If_MC_RPS_Transaction_arc())
		  {
			  strncpy(cit_mit_indr, "M101", sizeof(cit_mit_indr)-1);
			  memset(perch_id, 0x00, sizeof(perch_id));
		  }

			  memset(c_record, 0, sizeof(c_record));
				sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%11s%15s%11s%40s%1s%40s%2s%3s%2s%1s%1s%2s%2s%-35s%12s%1s%4s%1s%1s%2s%1s%9s%-4s%2s%1s%1s%12s%3s%3s",
				   "1",
				   auth_tx_detail_arc.message_type,
				   term_id,
				   auth_tx_detail_arc.card_nbr,
				   tran_dt,
				   auth_tx_detail_arc.tran_time,
				   "12",
				   " ",
				   " ",
				" ",
				" ",
				" ",
				" ",
				" ",
				" ",
				format_code,
				rpymt_type,
				rpymt_amt_indr,
				rpymt_nbr,
				rpymt_freq,
				perch_id,
				max_rpymt_amt,
				valdn_indr,
				cit_mit_indr,
				trsn_intn_mode,
				authn_char_indcr,
				authn_resp_code,
				card_hldr_idntfn_method,
				cash_bk_amt,
				valdn_code,
				prdouct_id,
				spend_qualfn_indcr,
				accnt_funding_src,
				authrzd_amount,
				authrzn_curr_code,
				" "
				);

			strncpy(c_record + 276, "\n\0", 2);
			crec_count++;

		  if (write_to_credit_settle_file() == false)
			return (false);
	  }

	  else if ((true == Check_If_VISA_Transaction_arc()) &&
				0 == strncmp(auth_tx_detail_arc.product_codes[5].code, "OFFUS", 5)  )
	  {
		  if (true == Check_If_VISA_Transaction_arc())
		  {
			  /*strncpy(format_code, "RP", 2);
			  strncpy(rpymt_type, "2", 1);
			  strncpy(rpymt_amt_indr, "2" , 1);
			  strncpy(rpymt_nbr, "99", 2);
			  strncpy(rpymt_freq, "12",2);
			  strncpy(max_rpymt_amt, "999999999999", 12);
			  strncpy(valdn_indr, "1", 1); */

		  	  memset(format_code, 0x00, sizeof(format_code));
		  	  memset(rpymt_type, 0x00, sizeof(rpymt_type));
		  	  memset(rpymt_amt_indr, 0x00, sizeof(rpymt_amt_indr));
		  	  memset(rpymt_nbr, 0x00, sizeof(rpymt_nbr));
		  	  memset(rpymt_freq, 0x00, sizeof(rpymt_freq));
		  	  memset(max_rpymt_amt, 0x00, sizeof(max_rpymt_amt));
		  	  memset(valdn_indr, 0x00, sizeof(valdn_indr));
		  	  memset(cit_mit_indr, 0x00, sizeof(cit_mit_indr));

			  strncpy(authn_char_indcr, auth_tx_detail_arc.mcard_banknet , 1);
			  strncpy(authn_resp_code, auth_tx_detail_arc.response_code, 2);
			  strncpy(card_hldr_idntfn_method, auth_tx_detail_arc.product_codes[5].quantity, 1);
			  strncpy(cash_bk_amt, auth_tx_detail_arc.down_payment, 9);
			  strncpy(valdn_code, auth_tx_detail_arc.interest_rate, 4);
			  strncpy(prdouct_id, auth_tx_detail_arc.product_codes[7].code, 2);
			  strncpy(spend_qualfn_indcr, &auth_tx_detail_arc.product_codes[7].code[2], 1);
			  strncpy(accnt_funding_src, &auth_tx_detail_arc.product_codes[5].quantity[1], 1);
			  strncpy(authrzd_amount, auth_tx_detail_arc.tran_amount, 12);
			  strncpy(authrzn_curr_code, auth_tx_detail_arc.term, 3);
		  }


			  memset(c_record, 0, sizeof(c_record));
				sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%11s%15s%11s%40s%1s%40s%2s%3s%2s%1s%1s%2s%2s%-35s%12s%1s%4s%1s%1s%2s%1s%9s%-4s%2s%1s%1s%12s%3s%3s",
						   "1",
						   auth_tx_detail_arc.message_type,
						   term_id,
						   auth_tx_detail_arc.card_nbr,
						   tran_dt,
						   auth_tx_detail_arc.tran_time,
						   "12",
						   " ",
						   " ",
						" ",
						" ",
						" ",
						" ",
						" ",
						" ",
						format_code,
						rpymt_type,
						rpymt_amt_indr,
						rpymt_nbr,
						rpymt_freq,
						perch_id,
						max_rpymt_amt,
						valdn_indr,
						cit_mit_indr,
						trsn_intn_mode,
						authn_char_indcr,
						authn_resp_code,
						card_hldr_idntfn_method,
						cash_bk_amt,
						valdn_code,
						prdouct_id,
						spend_qualfn_indcr,
						accnt_funding_src,
						authrzd_amount,
						authrzn_curr_code,
						" "
						);

			strncpy(c_record + 276, "\n\0", 2);
			crec_count++;

		  if (write_to_credit_settle_file() == false)
			return (false);
	  }




	  /* Segment 13 - EMV Data */

	     
     memset(emv_seg13, 0, sizeof(emv_seg13));
	 memset(emv_seg14, 0, sizeof(emv_seg14));
	 memset(emv_seg15, 0, sizeof(emv_seg15));
	     
     memset(c_record, 0, sizeof(c_record));

	 if((strcmp(auth_tx_detail_arc.pos_entry_mode,"0051")==0) ||
		(strcmp(auth_tx_detail_arc.pos_entry_mode,"0052")==0) ||
		(strcmp(auth_tx_detail_arc.pos_entry_mode,"0072")==0)) /* EMV Data is not present in AMEX */
	 {	  

		max_etran = atoi(Emv_list.num_returned);
		
		if(max_etran==0) return false;

	    for (i = 0; i < max_etran; i++)
		{
			memset(emv_data, 0, sizeof(emv_data));

			memcpy(&emv_record,&Emv_list.batch_list[i],sizeof(emv_record));			
			
			elength= atoi(emv_record.DATA_LENGTH);

			elength = elength * 2;

			/* Parse the EMV raw data to wrtie it in  segments - Ajay-TF, 18-Jun-2009 */

			parseEmvDataToSegment(elength, emv_seg13, emv_seg14, emv_seg15);	 
	/*		bin_to_ascii(emv_record.RAW_DATA,emv_data,elength/2);	

				if(elength<192)
					{						
						memcpy(emv_seg13,emv_data,elength);												
						
					}

				else if((elength>192) && (elength<384))

					{					     
						 memcpy(emv_seg13,emv_data,192);
						 memcpy(emv_seg14,emv_data+192,192);
					}

				else
					{
				 		memcpy(emv_seg13,emv_data,192);
						memcpy(emv_seg14,emv_data+192,192);
						memcpy(emv_seg15,emv_data+384,192);
					}
			*/

		}	
		  memset(&auth_tx_detail_arc.tran_time[6],'\0',1);
		 memset(&term_id[15],'\0',1);
         memset(c_record, 0x00, sizeof(c_record));

		 if((strlen(emv_seg13)!=0))
		 {

		 sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "13",  /*Segment 13 Data Follows 	*/	
			 emv_seg13			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;

			 if((strlen(emv_seg14)==0)) return(true);
			 
			 if (write_to_credit_settle_file() == false)
             return (false);

		 }
		 memset(&auth_tx_detail_arc.tran_time[6],'\0',1);
		 memset(&term_id[15],'\0',1);
		  memset(c_record, 0, sizeof(c_record));

		  if((strlen(emv_seg14)!=0))
		 {

			 sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "14", /* Segment 14 Data Follows */		
			 emv_seg14			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;	

			 if((strlen(emv_seg15)==0)) return(true);
			 
			 if (write_to_credit_settle_file() == false)
             return (false);
		 }
		 memset(&auth_tx_detail_arc.tran_time[6],'\0',1);
		 memset(&term_id[15],'\0',1);
		  memset(c_record, 0x00, sizeof(c_record));
		 
		    if((strlen(emv_seg15)!=0))
		 {

			 sprintf(c_record, "%1s%04s%-15s%-019s%06s%06s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "15", /* Segment 15 Data Follows */		
			 emv_seg15			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;				   		 

		 }
	}
   
         return(true);

} /* create_credit_detail_arc */


/*******************************************************************************
*
*  FUNCTION:         create_debit_detail
*
*  DESCRIPTION:      This function creates detail structure for 
*                    the debit settlement file.
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR d_record -  detail structure 
*
*  RETURN VALUE:     True if record should be written to file, else false
 
*******************************************************************************/
INT create_debit_detail()
{

   INT   retval = true,max_etran,i,elength;
   BYTE  term_id[16]={0};
   CHAR  rrn[13] = "";
//   CHAR  emv_seg13[192],emv_seg14[192],emv_seg15[192];
   CHAR  descriptor_code[3] = "",cad_merchant_id[12]={0}; 
   CHAR  tran_dt[7],entry_mode[5] ={0};	
   BYTE  pos_data[12]="";
   BYTE  temp_string[2]="";
   INT   iIndustry=0;
   BYTE  eci[4]={0};
   BYTE  tran_id[16]={0},temp_id[2]="",settle_date[5]="",temp_date[2]="";
   BYTE	 temp_entry_mode[4] = {0},card_nbr[20]={0};   
   BYTE  returncode=0;   
   BYTE  pan_sequence_number[4]={0};  
   BYTE  perch_id[PERCH_ID_LENGTH]={0}; // ID to store De63 table id -12 of messages 27 charcters- TF Phani	
   memset(tran_dt,0,7);   

   memset(d_record, 0, sizeof(d_record));
   memset(term_id, 0x00, sizeof(term_id));
   memset(card_nbr, 0x00, sizeof(card_nbr));
   memset(cad_merchant_id, 0x00, sizeof(cad_merchant_id));
   memset(eci, 0x00, sizeof(eci));
   memset(tran_id, 0x00, sizeof(tran_id));
   memset(pos_data,0x00,sizeof(pos_data));
    // Fill 6 byte with spaces for handling RPS traction.
   memset(perch_id,0x20,CUSTOM_IDENTIFIER_TYPE_LENGTH);

   /* Message Type */
    if (auth_tx_detail.tx_key == AUTH_REFUND)
	   {
		  strcpy(auth_tx_detail.message_type,"0400"); /* For Reversal Transaction */
	   }
	else
	      strcpy(auth_tx_detail.message_type,"0000"); /* For Original Transaction */

	/* Terminal ID - Left Justified ; Space Filled */

    if(true == Check_If_AMEX_Transaction())/* AMEX Terminal Id - 15 chars + 1 Space Filled */
	 {
		memcpy(&term_id,auth_tx_detail.primary_key.device_id,15);
		
	}
	else                                           /* VISA,JCB,MCARD Terminal ID */
	{
		memcpy(&term_id,auth_tx_detail.primary_key.device_id,8);
		memset(term_id+8,' ',7);	
		
	}

    if(true == Check_If_AMEX_Transaction()) /* AMEX Card Number - 15 numeric + 4 Space Filled */
	{
		memcpy(&card_nbr,auth_tx_detail.card_nbr,15);
		memset(&card_nbr[15],0x20,4);		
	}
	else                                          /* VISA,JCB,MC Card Number - 16 numeric + 3 Space Filled */
	{
		memcpy(&card_nbr,auth_tx_detail.card_nbr,16);
		memset(&card_nbr[16],0x20,3);
		
	}

	  if(auth_tx_detail.tx_key == AUTH_REVERSAL)  /* Processing Code */
		{
		  memcpy(&auth_tx_detail.processing_code, "20", 2 );
		}


	   // TF PHANI,Chandra - Fix fo processing code for Batch Upload and CA Txns

	  if((strcmp(auth_tx_detail.processing_code,"000001") == 0) || (auth_tx_detail.tx_key == AUTH_BATCH_UPLOAD) || (0 == strcmp("0320",auth_tx_detail_arc.message_type))) /* Batch Upload */
	  {
		  memcpy(&auth_tx_detail.processing_code, "000000", 6 ); 
	  }

	   if((auth_tx_detail.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION) || (auth_tx_detail.tx_key == AUTH_CASH_ADVANCE) || (strcmp(auth_tx_detail.processing_code,"010000") == 0)) /* Cash Advance */

	  {
		  memcpy(&auth_tx_detail.processing_code, "010000", 6 );
	  }
    

    memcpy(cad_merchant_id,auth_tx_detail.merchant_id+5, 10 ); /* Cadence Merchant ID */ 

	 /* POS Data Logic */
	 
    if(true == Check_If_JCB_Transaction()) /* JCB Card POS Data */
	{
		  	   
		   
		   /* card swipe */

		   if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"0220000",7);

		   /* manual entry */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"0100000",7);
         
		   /* CHIP */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"0510000",7);

		   /* Fallback */

		   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"9700000",7);
		   /* New POS entry mode for BDO - TF Phani*/
			 else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
				strncpy(pos_data,"9700000",7);

		    /* E-Commerce */

		   if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail.pos_entry_mode,"0081",4) == 0))
               strncpy(pos_data,"8120000",7);		   


		  /* RPS  */

		    iIndustry = atoi(terminal.industry_code );

		   if((strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0))
		   {
			   /*strncpy(pos_data,"0100800",7); TF PHANI - Do not copy this */
			   strncpy(pos_data+7,"11",2);			  
			   strncpy(pos_data+3,"08",2);			 
		   }


            else if (iIndustry == MAIL_PHONE)
			  {
				  strncpy(pos_data+7,"1",1);
				  strncpy(pos_data+3,"08",2);
				  strncpy(pos_data+8,"1",1);
			  } 
			  else
			  {
				  strncpy(pos_data+7,"2",1);				 
				  strncpy(pos_data+8,"2",1);
			  } 

			 if((strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) && (iIndustry != MAIL_PHONE)) /* For RPS and Non-MOTO */ 
			 {
				  strncpy(pos_data+7,"21",2);			  
			 }
		
			  
			  /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   
		   
	}

	  else if(true == Check_If_VISA_Transaction()) /* VISA POS Data */
	{
          	  
		  
		  /* card swipe */

           if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"900050000",9);

		   /* manual entry */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"012050000",9);

         
		   /* CHIP */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"050050000",9);
		   
		   /* FALLBACK */

		   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"900052000",9);
		   
				   /* New POS entry mode for BDO - TF Phani*/
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
			  strncpy(pos_data,"950000000",9);
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
			  strncpy(pos_data,"950000000",9);
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
			  strncpy(pos_data,"950000000",9);  

		   /* E-Commerce */

		  if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail.pos_entry_mode,"0081",4) == 0))
		  {
				strncpy(pos_data,"010500007",9);	  
					/*ECI value for VISA - ECI value should be copied to 88-89 position*/
				strncpy(eci,"07 ",3);
				if(strcmp(auth_tx_detail.profile,"")!=0)
				{
				 strncpy(eci,auth_tx_detail.profile,2);
				 strncpy(&pos_data[7],auth_tx_detail.profile,2);
				}
	 	  }
		  /* RPS */
		  else if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) 
		  {
			   strncpy(pos_data,"0100000",7);			   
			   strncpy(pos_data+7,"08",2);
			   memset(&pos_data[9],0x52,1);
		   }   
				 

		   if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) /* Rec Txn - R ,Non-Rec Txn - Space */
		   {
				memset(&pos_data[9],0x52,1);
		   }
		   else
                memset(&pos_data[9],0x20,1);

		   /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1); 

		   iIndustry = atoi(terminal.industry_code );


		     if (iIndustry == MAIL_PHONE) /* MOTO */
			  {
				  strncpy(pos_data+7,"08",2);
			  }
		/* else
			 {
				  strncpy(pos_data+7,"00",2);
			  }*/
				 
	 }

     else if(true == Check_If_AMEX_Transaction()) /* AMEX POS Data */
	 {

		 /* We need only last 3 characters of POS entry mode
          * coming from terminal for processing further.
          */

         if(strlen(auth_tx_detail.pos_entry_mode) > 3)
            strncpy(temp_entry_mode,&auth_tx_detail.pos_entry_mode[1],3);
         else
            strcpy(temp_entry_mode,auth_tx_detail.pos_entry_mode);
        	

         if((strncmp(temp_entry_mode,"02",2) == 0) && (strncmp(temp_entry_mode,"021",3) != 0)) /* card swip without 4DBC */
         {                           
            strncpy(pos_data,"260101254140",EPOS_POS_DATA_SIZE);
         }

         else if(strncmp(temp_entry_mode,"06",2) == 0) /* card swip with 4DBC */
		 {
            strncpy(pos_data,"260101W54140",EPOS_POS_DATA_SIZE);
         }

		 
         else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) != 0)) /* manual key in without 4DBC */
         {
            strncpy(pos_data,"660101654140",EPOS_POS_DATA_SIZE);
         }
		 else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) != 0)) /* manual key in with 4DBC */ 
		 {
            strncpy(pos_data,"660101S54140",EPOS_POS_DATA_SIZE);
         }
		 
		 		 
		 else if((strncmp(temp_entry_mode,"012",3) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */ /* RPS Transactions without 4DBC */
         {
            strncpy(pos_data,"660390664130",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */  /* RPS Transactions with 4DBC */
		 {
            strncpy(pos_data,"660390S64130",EPOS_POS_DATA_SIZE);
         }

		  	 

		 else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) == 0)) /* Ecommerce without 4DBC */
         {
            strncpy(pos_data,"6609S06S4140",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) == 0)) /* Ecommerce with 4DBC */
		 {
            strncpy(pos_data,"6609S0SS4140",EPOS_POS_DATA_SIZE);
         }

		 else if(strncmp(temp_entry_mode,"021",3) == 0) /* ATM */
		 {
			strncpy(pos_data,"210111210141",EPOS_POS_DATA_SIZE);
		 }	 



	}

	else if(true == Check_If_MC_Transaction()) /* Master Card POS Data */
	 {	 		

		   
		   /* card swipe */

       if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
       strncpy(pos_data,"902000000800",EPOS_POS_DATA_SIZE);

	   /* manual entry */

	   else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
       strncpy(pos_data,"010000000700",EPOS_POS_DATA_SIZE);
         
	   /* CHIP */

       else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
       strncpy(pos_data,"051000000800",EPOS_POS_DATA_SIZE);
	   
	   /* FALLBACK */
   	   
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);   
	   /* New POS entry mode added for MC */
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
       strncpy(pos_data,"800000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
       strncpy(pos_data,"801000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);   

	   /* e-commerce  */
	   if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (0==strncmp(auth_tx_detail.pos_entry_mode,"0081",4)))
	   {
	       strncpy(pos_data,"810125106000",EPOS_POS_DATA_SIZE);
	   }      
		
	    /* RPS */

		 else if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0)
		   {
			   strncpy(pos_data,"010004100000",EPOS_POS_DATA_SIZE);
		   }
		 
		 /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1);

		      if ((auth_tx_detail.pos_entry_mode[1] == '9') && (strncmp(auth_tx_detail.product_codes[6].amount,"00",2) == 0)) /* Magnetic stripe read capability */
			 {
				strncpy(pos_data,"91",2); // TF PHANI- contactless mag-stripe
			 }
           else if (terminal.device_type[1] == '7' ) /* Terminal has Chip Read capability. */
			 {
			     strncpy(pos_data,"07",2);
			 }
	   

	 } 
	  


	/* Ecommerce Indicator Logic */

	  if(true == Check_If_AMEX_Transaction()) /* Space Filling for AMEX Cards ECOM Indicator */
	/* Removed VISA from this catageory- TF Phani*/
	 {
		 strncpy(eci," ",3);
	 }

	 else if(true == Check_If_MC_Transaction()) /* Master Card ECOM Indicator */
	
	 {
           if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail.pos_entry_mode,"0081",4) == 0)) /* If E-commerce txn */ /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail.profile,"")!=0)
			   {
			     strncpy(eci,"21",2);
			     strncpy(eci+2,auth_tx_detail.profile+1,1);
			   }
			   else strcpy(eci,"210");
		   }
		   else
		   {
			   strcpy(eci,"000");			   
		   }
     }

	 else if(true == Check_If_JCB_Transaction()) /* JCB Card ECOM Indicator */
	 {

	    if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2) || (0==strncmp(auth_tx_detail.pos_entry_mode,"0081",4)))) /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail.profile,"")!=0)
			   {
			     strcpy(eci,auth_tx_detail.profile);
				 strncpy(eci+2," ",1);
				 
			   }
			   else strncpy(eci,"07 ",3);

		   }
		   else
		   {
			  strncpy(eci,"   ",3);			   
		   }
	 }



    /* Transaction ID */


	 if(true == Check_If_MC_Transaction()) /* Transaction ID for MASTER CARDS */
	
	 {    
		 // Settlement date MMDD
			strncpy(settle_date,auth_tx_detail.product_codes[19].code,4);//MMDD
			/*If Settle date is NUll, copy tran date as settle date*/
			if(strlen(settle_date) == 0)
			memcpy(settle_date,&auth_tx_detail.tran_date[4],4);  
			
		  if(strlen(auth_tx_detail.mcard_banknet)>0)
		 {
			 if(strlen(auth_tx_detail.mcard_banknet)>12)
			 {
			   strncpy(tran_id,auth_tx_detail.mcard_banknet,12);
			   strcat(tran_id,"   ");
			   strncpy(&tran_id[9],settle_date,4);
			 }
			 else 
			 {
                 memset(tran_id,' ',12);  
				 memset(tran_id+12,' ',3); 
				 strncpy(tran_id,auth_tx_detail.mcard_banknet,strlen(auth_tx_detail.mcard_banknet));
			   
				 strncpy(&tran_id[9],settle_date,4);
			   
			 }
		 }
		 else
		 {
			 memset(tran_id,' ',15);
			 //strcat(tran_id,"000");
			 
			 strncpy(&tran_id[9],settle_date,4);
		 } 
	 }


	 else if(true == Check_If_VISA_Transaction()) /* Transaction ID for VISA */
	 {
		 memset(tran_id,0x30,15);
		 memcpy(tran_id,auth_tx_detail.mcard_banknet,15);
	 }

	 else if(true == Check_If_AMEX_Transaction()) /* Transaction ID for AMEX */
	 {
		  strncpy(tran_id,auth_tx_detail.mcard_banknet,15);
	 }
	 else if(true == Check_If_JCB_Transaction()) /* Transaction ID for JCB - Spaces */
	 {
		 memset(tran_id,' ',15); 
	 }


	 /* PAN Sequence Number */

/*	 if(strcmp(emv_record.tag,"5F34")==0)
	 {
		 strcpy(pan_sequence_number,emv_record.emv_data);
	 }
	 else */
		strcpy(pan_sequence_number,"000");

	if(true == Check_If_AMEX_Transaction())/* For AMEX Card Sequence Number is Zero */
	 {
		 strcpy(pan_sequence_number,"000");
	 }

	  strcpy(tran_dt,auth_tx_detail.tran_date + 2);

	  if(strncmp(auth_tx_detail.product_codes[6].amount+2,"",3)==0)
	  {
		  strncpy(auth_tx_detail.product_codes[6].amount+2,"000",3);
	  }

      /* Form 27 byte message for DE 63- table Id 12, TF Phani- 
	  First 15 bytes in - BCh10 -> ticket_nbr 
	  Next  10 bytes in - BCh10 -> product_codes[14].amount*/
	/* TF-- Abhishek
	1) If Field 63 is present in the authorisation message and starts with "PRCHID", place the value of Field 63 in EPOS at segment 11 starting at
	    column 76 until 106.
	2) If Field 63 is present in the authorisation message and doesn't start with "PRCHID", place the values of Field 63 at segment 11 starting at column
	   82 until 106.
	Note :Item 1 is to address for RPS transactions. Item 2 is to address for PolicyNumber coming from POS terminal.
	*/
	if(strncmp(auth_tx_detail.ticket_nbr,"PRCHID",CUSTOM_IDENTIFIER_TYPE_LENGTH)== 0)
	{
		// Print 31 byte starting from 76 until 106
		strncpy(perch_id,auth_tx_detail.ticket_nbr,strlen(auth_tx_detail.ticket_nbr));
		strncat(perch_id,auth_tx_detail.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail.ticket_nbr) - CUSTOM_IDENTIFIER_TYPE_LENGTH +1));
	    perch_id[PERCH_ID_LENGTH -1]='\0';
	}
	else
	{
		// 1st 6 byte should be space as feild 63 is not starts whith PRCHID 
		// copy data after 6 byte which will be store from 82nd byte location
		//Print 25 byte starting from 82 until 106
		strncpy(perch_id + CUSTOM_IDENTIFIER_TYPE_LENGTH ,auth_tx_detail.ticket_nbr,strlen(auth_tx_detail.ticket_nbr));
		strncat(perch_id,auth_tx_detail.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail.ticket_nbr)- CUSTOM_IDENTIFIER_TYPE_LENGTH +1 ));
		perch_id[PERCH_ID_LENGTH -1]='\0';
	}

	 /* writing details to file */

	 /* Segment 10 */

      sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%6s%012s%06s%4s%-12s%04s%11s%12s%06s%03s%40s%012s%4s%29s%11s%03s%3s%2s%3s%3s%6s",
         "1",
		 auth_tx_detail.message_type,
         term_id,         
         card_nbr,
         tran_dt,
         auth_tx_detail.tran_time,
         "10",  /* Segment 10 Data Follows */
         auth_tx_detail.processing_code,
         auth_tx_detail.tran_amount,
		 "0",
		 terminal.category_code,
		 pos_data, 
		 "0",
		 " ",
		 auth_tx_detail.primary_key.retrieval_ref_num,
		 auth_tx_detail.auth_number,         
		 "0",
		 " ",
		 "0",
		 " ",
		 " ",
		 " ",
		 "0",
		 curr_code,
		 " ",
		 eci, /* E-Commerce Logic for MCRD,JCB ; VISA,AMEX - Spaces */
		 " ",
		 auth_tx_detail.primary_key.batch_nbr		 
         );

	  drec_count++;
   

      strncpy(d_record + 245, "\n\0", 2);

	  if (write_to_debit_settle_file() == false)
      return (false);	  

	  dseg10_records++;

	  /* Segment 11 */

	  memset(d_record, 0, sizeof(d_record));

	  sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%4s%012s%3s%3s%-027s%8s%10s%1s%11s%15s%03s%8s%-15s%72s",
         "1",
		 auth_tx_detail.message_type,
         term_id,         
         card_nbr,
         tran_dt,
         auth_tx_detail.tran_time,         
		 "11", /* Segment 11 Data Follows */
         auth_tx_detail.exp_date,
		 "0",
		 auth_tx_detail.product_codes[6].amount+2,
	     pan_sequence_number,
		 perch_id,
		 " ",
         cad_merchant_id,
		 " ",
		 " ",
		 " ",
		 "0",
		 " ",
         tran_id, 
		 " "	 	          	 
         );

	  drec_count++;		 

      strncpy(d_record + 245, "\n\0", 2);

	  if (write_to_debit_settle_file() == false)
      return (false);


	 /* Segment 13 - EMV Data */

	     
     memset(emv_seg13, 0, sizeof(emv_seg13));
	 memset(emv_seg14, 0, sizeof(emv_seg14));
	 memset(emv_seg15, 0, sizeof(emv_seg15));
	     
     memset(d_record, 0, sizeof(c_record));

	// if((strncmp(auth_tx_detail.card_nbr,"34",2)!=0) && (strncmp(auth_tx_detail.card_nbr,"37",2)!=0) && ((strcmp(auth_tx_detail.pos_entry_mode,"0051")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0052")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0072")==0))) /* EMV Data is not present in AMEX */

     if(true != Check_If_AMEX_Transaction())
     {
       if ((strcmp(auth_tx_detail.pos_entry_mode,"0051")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0052")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0072")==0)) /* EMV Data is not present in AMEX */
	   {

		max_etran = atoi(Emv_list.num_returned);

		if(max_etran==0) return false;

	    for (i = 0; i < max_etran; i++)
		{
			memset(emv_data, 0, sizeof(emv_data));

			memcpy(&emv_record,&Emv_list.batch_list[i],sizeof(emv_record));

			elength=2*atoi(emv_record.DATA_LENGTH);

			/* Parse the EMV raw data to wrtie it in  segments - Ajay-TF, 18-Jun-2009 */

			parseEmvDataToSegment(elength, emv_seg13, emv_seg14, emv_seg15);

		/*	bin_to_ascii(emv_data,emv_record.RAW_DATA,elength/2);

				if(elength<192)
					{
						memcpy(emv_seg13,emv_data,elength);
					}

				else if((elength>192) && (elength<384))

					{
						 memcpy(emv_seg13,emv_data,192);
						 memcpy(emv_seg14,emv_data+192,192);
					}

				else
					{
				 		memcpy(emv_seg13,emv_data,192);
						memcpy(emv_seg14,emv_data+192,192);
						memcpy(emv_seg15,emv_data+384,192);
					}
		*/

		}
		
         memset(d_record, 0, sizeof(d_record));

		 if((strlen(emv_seg13)!=0))
		 {

		 sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,
			 "13", /* Segment 13 Data Follows */
			 emv_seg13
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;

			 if((strlen(emv_seg14)==0)) return(true);

			 if (write_to_debit_settle_file() == false)
             return (false);

		 }

		  memset(d_record, 0, sizeof(d_record));

		  if((strlen(emv_seg14)!=0))
		 {

			 sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,
			 "14", /* Segment 14 Data Follows */
			 emv_seg14
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;

			 if((strlen(emv_seg15)==0)) return(true);

			 if (write_to_debit_settle_file() == false)
             return (false);
		 }

		  memset(d_record, 0, sizeof(d_record));

		    if((strlen(emv_seg15)!=0))
		 {

			 sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,
			 "15", /* Segment 15 Data Follows */
			 emv_seg15
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;

		 }
     }
	}
   
         return(true);
 
} /* create_debit_detail */


/*******************************************************************************
*
*  FUNCTION:         create_debit_detail_arc
*
*  DESCRIPTION:      This function creates detail structure for the debit
*                    settlement file from the batch archive file (BCH11).
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR d_record -  detail structure 
*
*  RETURN VALUE:     True if record should be written to file, else false

*******************************************************************************/
INT create_debit_detail_arc()
{
   INT   retval = true,max_etran,i,elength;
   BYTE  term_id[16]={0};
   CHAR  rrn[13] = "";
//   CHAR  emv_seg13[192],emv_seg14[192],emv_seg15[192];
   CHAR  descriptor_code[3] = "",cad_merchant_id[12]={0}; 
   CHAR  tran_dt[7],entry_mode[5] ={0};	
   BYTE  pos_data[12]="";
   BYTE  temp_string[2]="";
   INT   iIndustry=0;
   BYTE  eci[4]={0};
   BYTE  tran_id[16]={0},settle_date[5]={0};
   BYTE	 temp_entry_mode[4] = {0},card_nbr[20]={0};   
   BYTE  returncode=0;   
   BYTE  pan_sequence_number[4]={0};
   BYTE  perch_id[PERCH_ID_LENGTH]={0}; // ID to store De63 table id -12 of messages 27 charcters- TF Phani
   memset(tran_dt,0,7);   

   memset(d_record, 0, sizeof(d_record));
   memset(term_id, 0x00, sizeof(term_id));
   memset(card_nbr, 0x00, sizeof(card_nbr));
   memset(cad_merchant_id, 0x00, sizeof(cad_merchant_id));
   memset(eci, 0x00, sizeof(eci));
   memset(tran_id, 0x00, sizeof(tran_id));
    // Fill 6 byte with spaces for handling RPS traction.
   memset(perch_id,0x20,CUSTOM_IDENTIFIER_TYPE_LENGTH);

   /* Message Type */
    if (auth_tx_detail_arc.tx_key == AUTH_REFUND)
	   {
		  strcpy(auth_tx_detail_arc.message_type,"0400"); /* For Reversal Transaction */
	   }
	else
	      strcpy(auth_tx_detail_arc.message_type,"0000"); /* For Original Transaction */

	/* Terminal ID - Left Justified ; Space Filled */

		if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* AMEX Terminal Id - 15 chars + 1 Space Filled */
	 {
		memcpy(&term_id,auth_tx_detail_arc.primary_key.device_id,15);
		
	}
	else                                           /* VISA,JCB,MCARD Terminal ID */
	{
		memcpy(&term_id,auth_tx_detail_arc.primary_key.device_id,8);
		memset(term_id+8,' ',7);	
		
	}

		if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* AMEX Card Number - 15 numeric + 4 Space Filled */
	 {
		memcpy(&card_nbr,auth_tx_detail_arc.card_nbr,15);
		memset(&card_nbr[15],0x20,4);		
	}
	else                                          /* VISA,JCB,MC Card Number - 16 numeric + 3 Space Filled */
	{
		memcpy(&card_nbr,auth_tx_detail_arc.card_nbr,16);
		memset(&card_nbr[16],0x20,3);
		
	}

	  if(auth_tx_detail_arc.tx_key == AUTH_REVERSAL)  /* Processing Code */
		{
		  memcpy(&auth_tx_detail_arc.processing_code, "20", 2 );
		}


	  // TF PHANI,Chandra - Fix fo processing code for Batch Upload and CA Txns

	 if((0 == strcmp("0320",auth_tx_detail_arc.message_type)) || (auth_tx_detail_arc.processing_code[5] == '1'))  /* Batch Upload */
	  {
		  memcpy(&auth_tx_detail_arc.processing_code, "000000", 6 ); 
	  }

	 if((auth_tx_detail_arc.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION) || (auth_tx_detail_arc.tx_key == AUTH_CASH_ADVANCE) || (strcmp(auth_tx_detail_arc.processing_code,"010000") == 0)) /* Cash Advance */

	  {
		  memcpy(&auth_tx_detail_arc.processing_code, "010000", 6 );
	  }

  

    memcpy(cad_merchant_id,auth_tx_detail_arc.merchant_id+5, 10 ); /* Cadence Merchant ID */ 

	 /* POS Data Logic */
	 
	  if((strncmp(auth_tx_detail_arc.card_nbr,"35",2)==0)) /* JCB Card POS Data */
	{
		  	              
		   
		   /* card swipe */

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"0220000",7);

		   /* manual entry */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"0100000",7);
         
		   /* CHIP */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"0510000",7);

		   
		   /* Fallback */

		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"9700000",7);
		   /* New POS entry mode for BDO - TF Phani*/
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
				strncpy(pos_data,"9700000",7);

		    /* E-Commerce */

		    if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4) == 0)
               strncpy(pos_data,"8120000",7);


		  /* RPS  */

		    iIndustry = atoi(terminal.industry_code );

		   if((strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0))
		   {
			   /*strncpy(pos_data,"0100800",7); TF PHANI - Do not copy this */
			   strncpy(pos_data+7,"11",2);			  
			   strncpy(pos_data+3,"08",2);			 
		   }else if (iIndustry == MAIL_PHONE) /* MOTO */
		   {
			  strncpy(pos_data+7,"1",1);
			  strncpy(pos_data+3,"08",2);
			  strncpy(pos_data+8,"1",1);
		   }else
		   {
			  strncpy(pos_data+7,"2",1);				 
			  strncpy(pos_data+8,"2",1);
		   } 

			 if((strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) && (iIndustry != MAIL_PHONE)) /* For RPS and Non-MOTO */ 
			 {
				  strncpy(pos_data+7,"21",2);			  
			 }
		
			  
			  /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   
		   
	}

	  else if(auth_tx_detail_arc.card_nbr[0]=='4') /* VISA POS Data */
	{
          	  
		  		  
		  /* card swipe */

           if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"900050000",9);

		   /* manual entry */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"012050000",9);

         
		   /* CHIP */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"050050000",9);
		   
		   /* FALLBACK */

		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"900052000",9);
		     
					 /* New POS entry mode for BDO - TF Phani*/
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
				strncpy(pos_data,"950000000",9);
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
				strncpy(pos_data,"950000000",9);
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
				strncpy(pos_data,"950000000",9) ; 

		   /* E-Commerce */

		   if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4) == 0))
           {    
				strncpy(pos_data,"010500007",9);	  
					/*ECI value for VISA - ECI value should be copied to 89-90 position-TF Phani*/
				strncpy(eci,"07 ",3);
				if(strcmp(auth_tx_detail_arc.profile,"")!=0)
				{
				 strncpy(eci,auth_tx_detail_arc.profile,2);
				 strncpy(&pos_data[7],auth_tx_detail_arc.profile,2);
				}
	 		}
		   /* RPS */
		   else if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) 
		   {
			   strncpy(pos_data,"0100000",7);			   
			   strncpy(pos_data+7,"08",2);
			   memset(&pos_data[9],0x52,1);
		   }		 
			  
				 

		   if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) /* Rec Txn - R ,Non-Rec Txn - Space */
		   {
				memset(&pos_data[9],0x52,1);
		   }
		   else
                memset(&pos_data[9],0x20,1);

		   /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1); 

		   iIndustry = atoi(terminal.industry_code );

			  if (iIndustry == MAIL_PHONE) /* MOTO */
			  {
				  strncpy(pos_data+7,"08",2);
			  }
			 /* else
			  {
				  strncpy(pos_data+7,"00",2);
			  }*/


	 }

     else if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* AMEX POS Data */
	 {
       
		 /* We need only last 3 characters of POS entry mode
          * coming from terminal for processing further.
          */

         if(strlen(auth_tx_detail_arc.pos_entry_mode) > 3)
            strncpy(temp_entry_mode,&auth_tx_detail_arc.pos_entry_mode[1],3);
         else
            strcpy(temp_entry_mode,auth_tx_detail_arc.pos_entry_mode);


        if((strncmp(temp_entry_mode,"02",2) == 0) && (strncmp(temp_entry_mode,"021",3) != 0)) /* card swip without 4DBC */
         {                           
            strncpy(pos_data,"260101254140",EPOS_POS_DATA_SIZE);
         }

         else if(strncmp(temp_entry_mode,"06",2) == 0) /* card swip with 4DBC */
		 {
            strncpy(pos_data,"260101W54140",EPOS_POS_DATA_SIZE);
         }

		 
         else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) != 0)) /* manual key in without 4DBC */
         {
            strncpy(pos_data,"660101654140",EPOS_POS_DATA_SIZE);
         }
		 else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) != 0)) /* manual key in with 4DBC */ 
		 {
            strncpy(pos_data,"660101S54140",EPOS_POS_DATA_SIZE);
         }
		 
		 		 
		 else if((strncmp(temp_entry_mode,"012",3) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */
         {
            strncpy(pos_data,"660390664130",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions with 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */
		 {
            strncpy(pos_data,"660390S64130",EPOS_POS_DATA_SIZE);
         }
		  	 

		 else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) == 0)) /* Ecommerce without 4DBC */
         {
            strncpy(pos_data,"6609S06S4140",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) == 0)) /* Ecommerce with 4DBC */
		 {
            strncpy(pos_data,"6609S0SS4140",EPOS_POS_DATA_SIZE);
         }

		 else if(strncmp(temp_entry_mode,"021",3) == 0) /* ATM */
		 {
			strncpy(pos_data,"210111210141",EPOS_POS_DATA_SIZE);
		 }	 

	}

	else  if(auth_tx_detail_arc.card_nbr[0]=='5') /* Master Card POS Data */
	 {
	  					   
		   /* card swipe */

       if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
       strncpy(pos_data,"902000000800",EPOS_POS_DATA_SIZE);

	   /* manual entry */

	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0)
       strncpy(pos_data,"010000000700",EPOS_POS_DATA_SIZE);
         
	   /* CHIP */

       else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
       strncpy(pos_data,"051000000800",EPOS_POS_DATA_SIZE); 
   	   
	   /* FALLBACK */
   	   
	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);   
	   /* New POS entry mode added for MC */
	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
       strncpy(pos_data,"800000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
       strncpy(pos_data,"801000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);   


	   /* e-commerce  */
	   if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) || (0==strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4)))
	   {
	       strncpy(pos_data,"810125106000",EPOS_POS_DATA_SIZE);
	   }      
		
	    /* RPS */

		 else if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0)
		   {
			   strncpy(pos_data,"010004100000",EPOS_POS_DATA_SIZE);
		   }
		 
		 /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);

          if ((auth_tx_detail_arc.pos_entry_mode[1] == '9') && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"00",2) == 0)) /* Magnetic stripe read capability */
			 {
				strncpy(pos_data,"91",2);// TF PHANI- contactless mag-stripe
			 }
           else if (terminal.device_type[1] == '7' ) /* Terminal has Chip Read capability. */
			 {
			     strncpy(pos_data,"07",2);
			 }
	   

	 }     
	
	/* Ecommerce Indicator Logic */

	 if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0) ) /* Space Filling for AMEX Cards ECOM Indicator */
	/* Removed VISA from this catageory- TF Phani*/
	 {
		 strncpy(eci," ",3);
	 }

	 else if(auth_tx_detail_arc.card_nbr[0]=='5') /* Master Card ECOM Indicator */
	
	 {
           if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) || (0==strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4)))) /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail_arc.profile,"")!=0)
			   {
			     strncpy(eci,"21",2);
			     strncpy(eci+2,auth_tx_detail_arc.profile+1,1);
			   }
			   else strcpy(eci,"210");
		   }
		   else
		   {
			   strcpy(eci,"000");			   
		   }
     }

	 else if(strncmp(auth_tx_detail_arc.card_nbr,"35",2)==0) /* JCB Card ECOM Indicator */
	 {

	    if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2))) /* If E-commerce txn */
		{
		   if(strcmp(auth_tx_detail_arc.profile,"")!=0)
		   {
			 strcpy(eci,auth_tx_detail_arc.profile);
			 strncpy(eci+2," ",1);

		   }
		   else strncpy(eci,"07 ",3);
		}
		else
		{
		  strncpy(eci,"   ",3);
		}
	 }

    /* Transaction ID */

	if(auth_tx_detail_arc.card_nbr[0]=='5') /* Transaction ID for MASTER CARDS */
	
	 {    
		// Settlement date MMDD
		   strncpy(settle_date,auth_tx_detail_arc.product_codes[19].code,4);//MMDD
		   /*If Settle date is NUll, copy tran date as settle date*/
		   if(strlen(settle_date) == 0)
		   memcpy(settle_date,&auth_tx_detail_arc.tran_date[4],4);	
		  if(strlen(auth_tx_detail_arc.mcard_banknet)>0)
		 {
			 if(strlen(auth_tx_detail_arc.mcard_banknet)>12)
			 {
			   strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,12);
			  strcat(tran_id,"	 ");
			  strncpy(&tran_id[9],settle_date,4);
			 }
			 else 
			 {
                 memset(tran_id,' ',12);  
				memset(tran_id+12,' ',3); 
				 strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,strlen(auth_tx_detail_arc.mcard_banknet));
				strncpy(&tran_id[9],settle_date,4);
			 }
		 }
		 else
		 {
			 memset(tran_id,' ',12);
			strncpy(&tran_id[9],settle_date,4);
		 } 
	 }

	 else if(auth_tx_detail_arc.card_nbr[0]=='4') /* Transaction ID for VISA */
	 {
		 memset(tran_id,0x30,15);
		 memcpy(tran_id,auth_tx_detail_arc.mcard_banknet,strlen(auth_tx_detail_arc.mcard_banknet));
	 }

	 else	if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* Transaction ID for AMEX */
	 {
		  strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,15);
	 }
	 else if((strncmp(auth_tx_detail_arc.card_nbr,"35",2)==0)) /* Transaction ID for JCB - Spaces */
	 {
		 memset(tran_id,' ',15); 
	 }


	 /* PAN Sequence Number */

/*	 if(strcmp(emv_record.tag,"5F34")==0)
	 {
		 strcpy(pan_sequence_number,emv_record.emv_data);
	 }
	 else */
		strcpy(pan_sequence_number,"000");

	 if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* For AMEX Card Sequence Number is Zero */
	 {
		 strcpy(pan_sequence_number,"000");
	 }


	  strcpy(tran_dt,auth_tx_detail_arc.tran_date + 2);

	  if(strncmp(auth_tx_detail_arc.product_codes[6].amount+2,"",3)==0)
	  {
		  strncpy(auth_tx_detail_arc.product_codes[6].amount+2,"000",3);
	  }

		 /* Form 27 byte message for DE 63- table Id 12, TF Phani- 
	  First 15 bytes in - BCh10 -> ticket_nbr 
	  Next  10 bytes in - BCh10 -> product_codes[14].amount*/
	/* TF-- Abhishek
	1) If Field 63 is present in the authorisation message and starts with "PRCHID", place the value of Field 63 in EPOS at segment 11 starting at
	    column 76 until 106.
	2) If Field 63 is present in the authorisation message and doesn't start with "PRCHID", place the values of Field 63 at segment 11 starting at column
	   82 until 106.
	Note :Item 1 is to address for RPS transactions. Item 2 is to address for PolicyNumber coming from POS terminal.
	*/
	if(strncmp(auth_tx_detail_arc.ticket_nbr,"PRCHID",CUSTOM_IDENTIFIER_TYPE_LENGTH)== 0)
	{
		// Print 31 byte starting from 76 until 106
		strncpy(perch_id,auth_tx_detail_arc.ticket_nbr,strlen(auth_tx_detail_arc.ticket_nbr));
		strncat(perch_id,auth_tx_detail_arc.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail_arc.ticket_nbr) - CUSTOM_IDENTIFIER_TYPE_LENGTH +1));
	    perch_id[PERCH_ID_LENGTH -1]='\0';
	}
	else
	{
		// 1st 6 byte should be space as feild 63 is not starts whith PRCHID 
		// copy data after 6 byte which will be store from 82nd byte location
		//Print 25 byte starting from 82 until 106
		strncpy(perch_id + CUSTOM_IDENTIFIER_TYPE_LENGTH ,auth_tx_detail_arc.ticket_nbr,strlen(auth_tx_detail_arc.ticket_nbr));
		strncat(perch_id,auth_tx_detail_arc.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail_arc.ticket_nbr)- CUSTOM_IDENTIFIER_TYPE_LENGTH +1 ));
		perch_id[PERCH_ID_LENGTH -1]='\0';
	}

	 /* writing details to file */

	 /* Segment 10 */


      sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%6s%012s%06s%4s%-12s%04s%11s%12s%06s%03s%40s%012s%4s%29s%11s%03s%3s%2s%3s%3s%6s",
         "1",
		 auth_tx_detail_arc.message_type,
         term_id,         
         card_nbr,
         tran_dt,
         auth_tx_detail_arc.tran_time,
         "10",  /* Segment 10 Data Follows */
         auth_tx_detail_arc.processing_code,
         auth_tx_detail_arc.tran_amount,
		 "0",
		 terminal.category_code,
		 pos_data, 
		 "0",
		 " ",
		 auth_tx_detail_arc.primary_key.retrieval_ref_num,
		 auth_tx_detail_arc.auth_number,         
		 "0",
		 " ",
		 "0",
		 " ",
		 " ",
		 " ",
		 "0",
		 curr_code,
		 " ",
		 eci, /* E-Commerce Logic for MCRD,JCB ; VISA,AMEX - Spaces */
		 " ",
		 auth_tx_detail_arc.primary_key.batch_nbr		 
         );

      strncpy(d_record + 245, "\n\0", 2);

	  drec_count++;

	  if (write_to_debit_settle_file() == false)
      return (false);	  

	  dseg10_records++;

	  /* Segment 11 */

	  memset(d_record, 0, sizeof(d_record));

	  sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%4s%012s%3s%3s%-027s%8s%10s%1s%11s%15s%03s%8s%15s%72s",
         "1",
		 auth_tx_detail_arc.message_type,
         term_id,         
         card_nbr,
         tran_dt,
         auth_tx_detail_arc.tran_time,         
		 "11", /* Segment 11 Data Follows */
         auth_tx_detail_arc.exp_date,
		 "0",
		 auth_tx_detail_arc.product_codes[6].amount,
	     pan_sequence_number,
		 perch_id,
		 " ",
         cad_merchant_id,
		 " ",
		 " ",
		 " ",
		 "0",
		 " ",
         tran_id, 
		 " "	 	          	 
         );

      strncpy(d_record + 245, "\n\0", 2);

	  drec_count++;
	 

	  if (write_to_debit_settle_file() == false)
      return (false);


      /* Segment 13 - EMV Data */

	     
     memset(emv_seg13, 0, sizeof(emv_seg13));
	 memset(emv_seg14, 0, sizeof(emv_seg14));
	 memset(emv_seg15, 0, sizeof(emv_seg15));
	     
     memset(d_record, 0, sizeof(d_record));

	 if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)!=0) && (strncmp(auth_tx_detail_arc.card_nbr,"37",2)!=0) && ((strcmp(auth_tx_detail_arc.pos_entry_mode,"0051")==0) || (strcmp(auth_tx_detail_arc.pos_entry_mode,"0052")==0) || (strcmp(auth_tx_detail_arc.pos_entry_mode,"0072")==0))) /* EMV Data is not present in AMEX */
	 {	  

		max_etran = atoi(Emv_list.num_returned);
		
		if(max_etran==0) return false;

		for (i = 0; i < max_etran; i++)
		{
			memset(emv_data, 0, sizeof(emv_data));
			memcpy(&emv_record,&Emv_list.batch_list[i],sizeof(emv_record));			
			elength=2*atoi(emv_record.DATA_LENGTH);					

            /* Parse the EMV raw data to wrtie it in  segments - Ajay-TF, 08-Jun-2009 */
	       	parseEmvDataToSegment(elength, emv_seg13, emv_seg14, emv_seg15);

			/*	bin_to_ascii(emv_data,emv_record.RAW_DATA,elength/2);						

						if(elength<192)
							{						
								memcpy(emv_seg13,emv_data,elength);												
							}

						else if((elength>192) && (elength<384))

							{					     
								 memcpy(emv_seg13,emv_data,192);
								 memcpy(emv_seg14,emv_data+192,192);
							}

						else
							{
				 				memcpy(emv_seg13,emv_data,192);
								memcpy(emv_seg14,emv_data+192,192);
								memcpy(emv_seg15,emv_data+384,192);
							}	
					*/

				}	

	      memset(d_record, 0, sizeof(d_record));

		 if((strlen(emv_seg13)!=0))
		 {

		 sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "13", /* Segment 13 Data Follows */		
			 emv_seg13			 	         
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;

			 if((strlen(emv_seg14)==0)) return(true);
			 
			 if (write_to_debit_settle_file() == false)
             return (false);

		 }

		  memset(d_record, 0, sizeof(d_record));

		  if((strlen(emv_seg14)!=0))
		 {

			 sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "14", /* Segment 14 Data Follows */		
			 emv_seg14			 	         
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;	

			 if((strlen(emv_seg15)==0)) return(true);
			 
			 if (write_to_debit_settle_file() == false)
             return (false);
		 }

		  memset(d_record, 0, sizeof(d_record));
		 
		    if((strlen(emv_seg15)!=0))
		 {

			 sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "15", /* Segment 15 Data Follows */		
			 emv_seg15			 	         
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;				   		 

		 }
	}	   
   
         return(true);

} /* create_debit_detail_arc */


/*******************************************************************************
*
*  FUNCTION:         create_headers
*
*  DESCRIPTION:      This function creates header structures for 
*                    the credit and debit settlement files.
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  header structure 
*
*  RETURN VALUE:     None  
*   
*******************************************************************************/
void create_headers()
{
  
   /* ThoughtFocus - Chandra - Credit Header Record Implementation */

	char currentTime[7]=" ";

	returnCurrentTime(currentTime);

   if (((process_code[0] == 'A') || (process_code[0] == 'C')))
        
       
   {
      memset(c_record, 0, sizeof(c_record)); 
      sprintf(c_record, "%1s%6s%6s%06s%3s%06s%06s%011s%-11s%10s%10s%1d%1d%167s",
         "0",         
         extract_date,
		 CurrentDate,
		 currentTime,
		 "300",
		 "0",
		 "0",
		 "0",
		 "500051",
		 " ",
		 " ",
		 cre_inc_value,
		 cre_inc_value++,         
         " ");
      strncpy(c_record + 245, "\n\0", 2);
   }
   
   /* ThoughtFocus - Chandra - Debit Header Record Implementation */

  if (((process_code[0] == 'A') || (process_code[0] == 'D')))        
       
   {
      memset(d_record, 0, sizeof(d_record)); 
      sprintf(d_record, "%1s%6s%6s%06s%3s%06s%06s%011s%-11s%10s%10s%1d%1d%167s",
         "0",         
         extract_date,
		 CurrentDate,
		 currentTime,
		 "300",
		 "0",
		 "0",
		 "0",
		 "500051",
		 " ",
		 " ",
		 deb_inc_value,
		 deb_inc_value++,         
         " ");
      strncpy(d_record + 245, "\n\0", 2);
   }

} /* create_headers */


/*******************************************************************************
*
*  FUNCTION:         create_exception_headers
*
*  DESCRIPTION:      This function creates header structures for 
*                    the credit and debit settlement files.
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  header structure 
*
*  RETURN VALUE:     None  
*  AUTHOR:          ABHISHEK KUMAR VERMA 
*   
*******************************************************************************/
BOOLEAN create_write_exception_headers(void)
{
	CHAR              tempstr[16]="";
	CHAR              buffer[100]=""; 
 	CHAR              header[512]=" ";
	
    memset(header, 0, sizeof(header)); 
	strcpy(header,"Card Number        | Tran date | Settlement date | Auth Tran Amount | Settlement Amount | Auth Number | Terminal ID | Merchant ID     | % OVERLIMIT  ");
	strncpy(header + 149, "\n\0", 2);
	
	/*** Write to file ***/
   if (fputs(header,E_file_write) < 0)
   {
	  itoa (ferror(E_file_write), tempstr, 10);
	  fclose (E_file_write);
	  
	  strcpy(buffer, "Cannot write to credit settlement file: ");
	  strcat(buffer, tempstr);
	  merchbatch_log_message( 2, 3, buffer, "create_write_exception_headers", 0 );
	  write_to_dump_file(buffer);

	  return(false);
   }   

   return (true);
} /* create_exception_headers */
/*******************************************************************************/

/*******************************************************************************
*
*  FUNCTION:         create_trailer_exception_headers
*
*  DESCRIPTION:      This function creates trailer structures for 
*                    the credit and debit settlement files.
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  header structure 
*
*  RETURN VALUE:     None  
*  AUTHOR:          ABHISHEK KUMAR VERMA 
*   
*******************************************************************************/
BOOLEAN create_trailer_exception_headers(void)
{
	CHAR              tempstr[16]="";
	CHAR              buffer[100]=""; 
 	CHAR              total_reocrds[256]=" ";
	CHAR              total_line[256]=" ";
	CHAR              trailer[256]="";
	INT					line_count=0;
	INT 				len=0;
	
    memset(total_reocrds, 0, sizeof(total_reocrds)); 
	memset(total_line, 0, sizeof(total_line)); 
	memset(trailer, 0, sizeof(trailer));
	line_count=E_crec_count + 1;
	sprintf(total_reocrds,"Total # of records: %d",E_crec_count);
	len = strlen(total_reocrds);
	strncpy(trailer,total_reocrds, len);
	strncpy(trailer + len, "\n\0", 2);
	
	/*** Write to file ***/
   if (fputs(trailer,E_file_write) < 0)
   {
	  itoa (ferror(E_file_write), tempstr, 10);
	  fclose (E_file_write);
	  
	  strcpy(buffer, "Cannot write to credit settlement file: ");
	  strcat(buffer, tempstr);
	  merchbatch_log_message( 2, 3, buffer, "create_trailer_exception_headers", 0 );
	  write_to_dump_file(buffer);

	  return(false);
   }   
   memset(trailer, 0, sizeof(trailer));
   sprintf(total_line,"Total # of line: %d",line_count);
   strncpy(trailer + strlen(total_line), "\n\0", 2);
   len = strlen(total_line);
	strncpy(trailer,total_line, len);
	strncpy(trailer + len, "\n\0", 2);
   /*** Write to file ***/
  if (fputs(trailer,E_file_write) < 0)
  {
	 itoa (ferror(E_file_write), tempstr, 10);
	 fclose (E_file_write);
	 
	 strcpy(buffer, "Cannot write to credit settlement file: ");
	 strcat(buffer, tempstr);
	 merchbatch_log_message( 2, 3, buffer, "create_trailer_exception_headers", 0 );
	 write_to_dump_file(buffer);

	 return(false);
  }

   return (true);
} /* create_exception_headers */
/*******************************************************************************/


//
/*******************************************************************************
*
*  FUNCTION:         create_reports
*
*  DESCRIPTION:      This function creates reports for EPOS 
*                    
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  trailer structure 
*
*  RETURN VALUE:     None  
*   
*******************************************************************************/
void create_reports()
{
     

   CHAR time_date[25] = "";

  
  strcpy(r_filename, path);
#ifndef WIN32
	  strcat(r_filename, "eftpos/");
#else
	  strcat(r_filename, "eftpos\\");
#endif
  strcat(r_filename, "epos_report_");
  strcat(r_filename, cutoff_date);
  strcat(r_filename,"_");
  strcat(r_filename,cutoff_hour);
  strcat(r_filename, ".rpt"); 
  
  r_file_write = fopen (r_filename, "w+");
   memset(r_record, 0, sizeof(r_record)); 
    ptetime_get_timestamp( time_date );	
	  sprintf(r_record,"TIME and DATE			  :	 %s\n",time_date);
	  fputs(r_record, r_file_write);
	  sprintf(r_record,"FILE NAME		       : 	epos_report_%s_%s.rpt \n",cutoff_date,cutoff_hour);
	  fputs(r_record, r_file_write);
	  sprintf(r_record,"No of Credit Transactions      : 	%09d \n",crec_count);
	  fputs(r_record, r_file_write);
	  sprintf(r_record,"No of Debit Transactions       : 	%09d \n",drec_count);
	  fputs(r_record, r_file_write);
#ifdef WIN32 
	  sprintf(r_record,"Total Credit Tx Amount		   :	  %016I64u \n",ctotal_amt);
#else
	  sprintf(r_record,"Total Credit Tx Amount         : 	%016llu \n",ctotal_amt);
#endif
	  fputs(r_record, r_file_write);
#ifdef WIN32 
	  sprintf(r_record,"Total Debit Tx Amount         : 	%016I64u \n",dtotal_amt);
#else
 
	  sprintf(r_record,"Total Debit Tx Amount         : 	%016llu \n",dtotal_amt);
#endif
	  fputs(r_record, r_file_write);

#ifdef WIN32 
	  sprintf(r_record,"Grand Total Amount	           : 	%016I64u \n",ctotal_amt+dtotal_amt);
#else
	  
	  sprintf(r_record,"Grand Total Amount	        : 	%016llu \n",ctotal_amt+dtotal_amt);
#endif
	  fputs(r_record, r_file_write);
	  sprintf(r_record,"Total Transactions  		: 	%09d \n",crec_count+drec_count);
	  fputs(r_record, r_file_write);
	  fclose(r_file_write);		  
   
} /* create_reprots */


//
/*******************************************************************************
*
*  FUNCTION:         create_trailers
*
*  DESCRIPTION:      This function creates trailer structures for 
*                    the credit and debit settlement files.
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  trailer structure 
*
*  RETURN VALUE:     None  
*   
*******************************************************************************/
void create_trailers()
{
     
   /* ThoughtFocus - Chandra - Credit Trailer Record Implementation */
	
  if ( ((process_code[0] == 'A') || (process_code[0] == 'C')))
        
       
   {
      memset(c_record, 0, sizeof(c_record)); 
#ifdef WIN32      
	  sprintf(c_record, "%1s%09d%016I64u%1s%016s%016d%1s%015I64u%170s",
#else
	  sprintf(c_record, "%1s%09d%016llu%1s%016s%016d%1s%015llu%170s",
#endif
         "9",         
         crec_count,
		 ctotal_amt,
		 " ",
		 "0",
		 cseg10_records,
		 "+",
		 ctotal_amt,         
         " ");
	 
      strncpy(c_record + 245, "\n\0", 2);
   } 

   
   
   /* ThoughtFocus - Chandra - Debit Trailer Record Implementation */

   if ( ((process_code[0] == 'A') || (process_code[0] == 'D')))
       
        
   {
      memset(d_record, 0, sizeof(d_record)); 
#ifdef WIN32
      sprintf(d_record, "%1s%09d%016I64u%1s%016s%016d%1s%015I64u%170s",
#else
	  sprintf(d_record, "%1s%09d%016llu%1s%016s%016d%1s%015llu%170s",
#endif
         "9",         
         drec_count,
		 dtotal_amt,
		 " ",
		 "0",
		 dseg10_records,
		 "+",
		 dtotal_amt,         
         " ");
      strncpy(d_record + 245, "\n\0", 2);
   } 
   
} /* create_trailers */


/*******************************************************************************
*
*  FUNCTION:         create_settle_files
*
*  DESCRIPTION:      This function creates records and 
*                    writes to the settlement files. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.*
  
*******************************************************************************/

 BOOLEAN create_settle_files(pCHAR merchant_id)

 {
 
   CHAR              bonus_char[2] = "Y";
   CHAR              err_msg[100], buffer[200]; 
   INT               count_tran;
   INT               max_tran;
   INT               next_portion = 0;
   INT               ret_code = 0,i=0,ctemp_amt=0,dtemp_amt=0;
   
 
  
   /*** Set initial detail data for BCH10 ***/
   memset(&auth_tx_detail, 0, sizeof(auth_tx_detail));
   strcpy (auth_tx_detail.primary_key.batch_nbr, tc_totals.batch_number) ;
   strncpy (auth_tx_detail.primary_key.device_id, tc_totals.terminal_id, 8) ;
   strcpy (auth_tx_detail.primary_key.retrieval_ref_num, " ") ;
   strcpy (auth_tx_detail.open_date, tc_totals.open_date);
   strcpy (auth_tx_detail.open_time, tc_totals.open_time);
   strcpy (auth_tx_detail.merchant_id,merchant_id);
   
   /*** Set initial detail data for BCH11 ***/
   memset(&auth_tx_detail_arc, 0, sizeof(auth_tx_detail_arc));
   strcpy (auth_tx_detail_arc.primary_key.batch_nbr, tc_totals.batch_number) ;
   strncpy (auth_tx_detail_arc.primary_key.device_id, tc_totals.terminal_id, 8) ;
   strcpy (auth_tx_detail_arc.primary_key.retrieval_ref_num, " ") ;
   strcpy (auth_tx_detail_arc.open_date, tc_totals.open_date);
   strcpy (auth_tx_detail_arc.open_time, tc_totals.open_time);
   strcpy (auth_tx_detail_arc.merchant_id, merchant_id);

   
   memset(&emv_record, 0, sizeof(emv_record));
   
   /*** ThoughtFocus - Chandra  ***/

   while (next_portion == 0)
   {

	  /* If batch status is "C" then data is in BCH10 */
      if (	strcmp(auth_tx_total.batch_status, "C") == 0 || 
			(strcmp(auth_tx_total.batch_status, "P") == 0 && Duplicate_Restrict == ALLOW_DUPLICATES)
		 )
      {
		 		  
		 ret_code = db_get_bch10_service_list(&auth_tx_detail, &bch10_list, err_msg);
         if (ret_code != PTEMSG_OK)
		 {
            merchbatch_log_message( 2, 3, err_msg, "create_settle_files", 0 );
			write_to_dump_file(err_msg);

            return (false);
         }
      
         max_tran = atoi(bch10_list.num_returned);
         if (max_tran < BATCH_SERVICE_LIST_SIZE) //if less than 30 then 1. Apinto
            next_portion = 1; 
		 
		          
         /***  ***/ 
         for (count_tran = 0; count_tran < max_tran; count_tran++)
         {
            memset( &auth_tx_detail, 0x00, sizeof(auth_tx_detail) );
            memcpy( &auth_tx_detail,
                    &bch10_list.batch_list[count_tran], 
                    sizeof(auth_tx_detail));		
			

            /* Void Sale transactions should not be included in this file */
            if ((auth_tx_detail.tx_key == AUTH_VOID_SALE  ||
                 auth_tx_detail.tx_key == AUTH_VOID_SALE_RESPONSE))
               continue; 	

		 	 memset(&emv_record, 0, sizeof(emv_record));		  

			 memcpy(emv_record.transaction_id,auth_tx_detail.transaction_id,sizeof(emv_record));

			 ret_code = db_get_emv_service_list(&auth_tx_detail, &Emv_list, err_msg);			 
			 

			 if (ret_code != PTEMSG_OK)
		 {
               merchbatch_log_message( 2, 3, err_msg, "create_settle_files", 0 );
			   write_to_dump_file(err_msg);

            return (false);
         }
 			
            
               if (auth_tx_detail.processing_code[2] == '0' || 
                   auth_tx_detail.processing_code[2] == '3' ||
                   auth_tx_detail.processing_code[2] == '4' )
               {
                   if ( ((process_code[0] == 'A') || (process_code[0] == 'C')) &&
                       ( credit_txns_exist == true ) )
                  {
					  					   
					    ctemp_amt=atoi(auth_tx_detail.tran_amount);  /* To Check whether amount is proper or not */

					    if((ctemp_amt==INT_MAX) || (ctemp_amt==INT_MIN))
					   {						   

						   memset(&buffer, 0x00, sizeof(buffer));
#ifdef WIN32
	                       sprintf(buffer, "Credit Txn Amount is not proper ! Txn Amount=%d,Total Amount=%I64u",ctemp_amt,ctotal_amt);
#else
						    sprintf(buffer, "Credit Txn Amount is not proper ! Txn Amount=%d,Total Amount=%llu",ctemp_amt,ctotal_amt);
#endif
						   
                          merchbatch_log_message( 2, 1, buffer, "create_settle_files", 0 );
						  return true;
					
					   }


					   ctotal_amt= ctotal_amt+ctemp_amt;
					


                     /*** Write to credit file ***/
                     
                     if ( true == create_credit_detail() )
                     {
                        if (write_to_credit_settle_file() == false)
                           return (false);
                     } 
                       
                  }
               }
               else
               {                  			   
				   
				   if ( ((process_code[0] == 'A') || (process_code[0] == 'D')) &&
                       ( debit_txns_exist == true ) )
                  {
					    					   
					   dtemp_amt=atoi(auth_tx_detail.tran_amount); /* To Check whether amount is proper or not */

					   if((dtemp_amt==INT_MAX) || (dtemp_amt==INT_MIN))
					   {

						   memset(&buffer, 0x00, sizeof(buffer));
#ifdef WIN32
	                       sprintf(buffer, "Debit Txn Amount is not proper ! Txn Amount=%d,Total Amount=%I64u",dtemp_amt,dtotal_amt);
#else
						   sprintf(buffer, "Debit Txn Amount is not proper ! Txn Amount=%d,Total Amount=%llu",dtemp_amt,dtotal_amt);
#endif
						   
                           merchbatch_log_message( 2, 1, buffer, "create_settle_files", 0 );
						   return true;
					
					   }
					  
					  
					  dtotal_amt= dtotal_amt+dtemp_amt;				
					  


                     /*** Write to debit file ***/

                     if ( true == create_debit_detail() )
                     {
                        if (write_to_debit_settle_file() == false)
                           return (false);
                     }
                  }
               }

         } /*** for end ***/
      } /*** if end ***/
      else
      /* If batch status is "R" then data is in BCH11 */	
	  // Or if batch is already writen to file then status would be W, Check ,if Duplicate_Restrict flag is  ALLOW_DUPLICATES and staus is  W
      if (strcmp(auth_tx_total.batch_status, "R") == 0 || 
			(strcmp(auth_tx_total.batch_status, "W") == 0 && Duplicate_Restrict == ALLOW_DUPLICATES)
		 )

	   {
         if (db_get_bch11_service_list(&auth_tx_detail_arc, &bch11_list, err_msg) != PTEMSG_OK)
         {
            merchbatch_log_message( 2, 3, err_msg, "create_settle_files", 0 );
			write_to_dump_file(err_msg);

            return (false);
         }

         max_tran = atoi(bch11_list.num_returned);
         if (max_tran < BATCH_SERVICE_LIST_SIZE) //if less than 30 then 1. Apinto
            next_portion = 1; 
		
         /*** Loop for all transactions in the list ***/ 
         for (count_tran = 0; count_tran < max_tran; count_tran++)
         {
            memset(&auth_tx_detail_arc, 0x00, sizeof(auth_tx_detail_arc));
            memcpy(&auth_tx_detail_arc, &bch11_list.batch_list[count_tran], 
               sizeof(auth_tx_detail_arc));

            /* Void Sale transactions should not be included in this file */
            if ((auth_tx_detail_arc.tx_key == AUTH_VOID_SALE  ||
                 auth_tx_detail_arc.tx_key == AUTH_VOID_SALE_RESPONSE))
               continue;

			 memset(&emv_record, 0, sizeof(emv_record));			    
			
			 memcpy(emv_record.transaction_id,auth_tx_detail_arc.transaction_id,sizeof(emv_record));

			 ret_code = db_get_emv_arc_service_list(&auth_tx_detail_arc, &Emv_list, err_msg);

			 if (ret_code != PTEMSG_OK)
		 {
               merchbatch_log_message( 2, 3, err_msg, "create_settle_files", 0 );
			   write_to_dump_file(err_msg);

            return (false);
         }		 

            if (auth_tx_detail_arc.processing_code[2] == '0' || 
                auth_tx_detail_arc.processing_code[2] == '3' ||
                auth_tx_detail_arc.processing_code[2] == '4' )
			  
            {  
				
               if ( ((process_code[0] == 'A') || (process_code[0] == 'C')) &&
                    ( credit_txns_exist == true ) )
               {
				    
			          
				      ctemp_amt=atoi(auth_tx_detail_arc.tran_amount); /* To Check whether amount is proper or not */

					   if((ctemp_amt==INT_MAX) || (ctemp_amt==INT_MIN))
					   {

						   memset(&buffer, 0x00, sizeof(buffer));
#ifdef WIN32
	                       sprintf(buffer, "Credit Txn Amount is not proper ! Txn Amount=%d,Total Amount=%I64u",ctemp_amt,ctotal_amt);
#else
						   sprintf(buffer, "Credit Txn Amount is not proper ! Txn Amount=%d,Total Amount=%llu",ctemp_amt,ctotal_amt);
#endif
                           merchbatch_log_message( 2, 1, buffer, "create_settle_files", 0 );
						   return true;
					
					   }
					   
				      ctotal_amt= ctotal_amt+ctemp_amt;

				
                  /*** Write to credit file ***/

                  if ( true == create_credit_detail_arc() )
                  {
                     if (write_to_credit_settle_file() == false)
                        return (false);
                  }
               }
            }
            else
            {
               if ( ((process_code[0] == 'A') || (process_code[0] == 'D')) &&
                    ( debit_txns_exist == true ) )
               {
				  				   
				   dtemp_amt=atoi(auth_tx_detail_arc.tran_amount); /* To Check whether amount is proper or not */

					   if((dtemp_amt==INT_MAX) || (dtemp_amt==INT_MIN))
					   {

						   memset(&buffer, 0x00, sizeof(buffer));
#ifdef WIN32
	                       sprintf(buffer, "Debit Txn Amount is not proper ! Txn Amount=%d,Total Amount=%I64u",dtemp_amt,dtotal_amt);
#else
						   sprintf(buffer, "Debit Txn Amount is not proper ! Txn Amount=%d,Total Amount=%llu",dtemp_amt,dtotal_amt);
#endif
                           merchbatch_log_message( 2, 1, buffer, "create_settle_files", 0 );
						   return true;
					
					   }
					   
					   dtotal_amt= dtotal_amt+dtemp_amt;
			

                  /*** Write to debit file ***/

                  if ( true == create_debit_detail_arc() )
                  {
                     if (write_to_debit_settle_file() == false)
                        return (false);
                  }
               }
            }

         } /*** for end ***/

      } /*** else if end ***/   
	  else if ((strcmp(auth_tx_total.batch_status, "W") == 0) || (strcmp(auth_tx_total.batch_status, "P") == 0 ))
	  {// TF Phani ..New staus code for 'EFTPOS file writen W' and P
	   // Check , Duplicate_Restrict flag is  ALLOW_DUPLICATES is 		
	  		return (true);
	  }
      else
      /*** Error if batch status is not "C" or "R" ***/
      {
         strcpy(err_msg, "Batch status not closed (C) or released (R) or (W) or (P).");
         merchbatch_log_message( 2, 3, buffer, "create_settle_files", 0 );
		 write_to_dump_file(err_msg);

         return (false);
      } /*** else end ***/

	  	  

   } /*** while end ****/   
   

   

   return (true);

} /* create_settle_files */


/*****************************************************************************

  FUNCTION:       display_settle_info   

  DESCRIPTION:    This function displays an informational message
                  on the console.
  
  AUTHOR:         Darcy Person

  INPUTS:         None

  OUTPUTS:        None

  RETURN VALUE:   None

******************************************************************************/
void display_settle_info(void)
{
   printf("MerchBatch(EFTPOS) requires four command line parameters to work correctly:        \n");
   printf("(1) the base file name                                                 \n");
   printf("(2) the cutoff date in YYYYMMDD format,                                \n");
   printf("(3) the cutoff hour in HH format in the range of 00 to 23, and         \n");
   printf("(4) the number of hours to look back in the range of 01 to 99.         \n");
   printf("(5) the files to be generated (C)redit,(D)ebit,(A)ll                   \n");
   printf("(6) EX_RUN                                                             \n");
   printf("                                                                       \n");
   printf("In addition, EFTPOS has an optional 5th parameter to indicate which    \n");
   printf("settlement file to generate: (A)ll, (C)redit, (D)ebit.                 \n");
   printf("The default is to generate all two files for the specified period.     \n");
   printf("                                                                       \n");
   printf("For example, to process closed and released batches from July 9, 1999  \n");
   printf("at 2:00 p.m. through July 10, 1999 at 1:00 p.m., use the parameters:   \n");
   printf("                                                                       \n");
   printf("          eftpos 19990710 13 23 A 1                                    \n");
   printf("                                                                       \n");
   printf("To process only debit transactions from yesterday at midnight through  \n");
   printf("today at midnight, use the parameters:                                 \n");
   printf("                                                                       \n");
   printf("          eftpos , 00 24 debit     (where the comma indicates today)   \n");
   printf("                                                                       \n");   
   printf("Output is placed in the eftpos subdirectory under the configured       \n");
   printf("Ascendent system directory.                                            \n");
   printf("                                                                       \n");   
   printf("                                                                       \n");
   printf("Credit/Debit EFTPOS Version: %s\n\n", Version                         );
 
} /* display_settle_info */


/*******************************************************************************
*
*  FUNCTION:         EndProcess
*
*  DESCRIPTION:      This function does shutdown and clean up functionalities.
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*  AUTHOR:           Irene Goldfild
*   
*******************************************************************************/
void EndProcess(void)
{
   CHAR Buffer[100] = "";

   if ( EndProcessFlag == true )
   {
      sprintf( Buffer, "Shutting down the %s Service, version %s",
               AppName, Version );
      merchbatch_log_message( 2, 1, Buffer, "EndProcess", 0);
      strcat( Buffer, "\n" );
      PRINT( Buffer );
   }
   else
   {
      EndProcessFlag = true;
   }
   return;

} /* EndProcess */


/*******************************************************************************
*
*  FUNCTION:         get_currency_code
*
*  DESCRIPTION:      This function gets currency code using dcf01 (terminal) 
*                    and mcf01 (merchant) tables.    
*
*  INPUTS:           None
*         
*  OUTPUTS:          DCF01 terminal, MCF01 merchant.
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*  AUTHOR:           Irene Goldfild
*   
*******************************************************************************/
BOOLEAN get_currency_code()
{
   BYTE    rc;
   CHAR    err_msg[100] = "";
   CHAR    buffer[500];
   BOOLEAN retval = true;
   double  start_time;
   double  end_time;
   double  duration;

   memset( &terminal, 0x00, sizeof(terminal) );
   memset(&cupMerchantID,0x00,sizeof(cupMerchantID));
   memcpy( terminal.primary_key.device_id, tc_totals.terminal_id, 8 );
   memcpy( terminal.merchant_id, tc_totals.merchant_id, 15);

   /*** Get terminal configuration table ***/
   memset( device_type, 0x00, sizeof(device_type) );

   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   rc = db_select_curr_code( curr_code,device_type,&terminal,&cupMerchantID,err_msg );

   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, ST2_NONE, MCF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( rc != PTEMSG_OK )
   {
      memset( buffer, 0x00,sizeof(buffer) );
      sprintf( buffer,
              "DB Err getting Currency Code from Merchant record: %s",
               err_msg );
      merchbatch_log_message( 2, 2, buffer, "get_currency_code", 0 );
      retval = false;
	  return( retval );
   }
   /*TF- Phani Check for INDUSRY CODE and category_code value. At this point Quiry is success*/
   if(strlen(terminal.industry_code) == 0)
   	{
		memset( buffer, 0x00,sizeof(buffer) );
		sprintf( buffer,
				"Industry Code is NULL for Merchant record: %s",
				 err_msg );
		merchbatch_log_message( 2, 2, buffer, "get_currency_code", 0 );
		retval = false;
		return( retval );
   	}
   return( retval );
}


/*******************************************************************************
*
*  FUNCTION:         open_credit_file
*
*  DESCRIPTION:      This function opens the credit settlement file 
*                    or creates it if it doesn't exist. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*  AUTHOR:           Darcy Person
*   
*******************************************************************************/
BOOLEAN open_credit_file()
{   
   CHAR              buffer[500];

   memset(buffer, 0, sizeof(buffer));   

   /* Send message to system monitor */
   strcpy(buffer,"Open credit settlement file ");
   merchbatch_log_message( 2, 1, buffer, "open_credit_file", 0 );
   /* Send message to DOS window in debug mode */
   strcat(buffer,"\n");
   PRINT(buffer);

   /*** Open file ***/
   strcpy(c_filename, path);
   #ifndef WIN32
	   strcat(c_filename, "eftpos/");
   #else
	   strcat(c_filename, "eftpos\\");
   #endif
   strcat(c_filename, "credit_eftpos");
   strcat(c_filename, cutoff_date);
   strcat(c_filename, ".upl"); 

   c_file_write = fopen (c_filename, "w");

   if (c_file_write == NULL_PTR)
   {  
      strcpy(buffer, "Cannot open credit settlement file: ");
      strcat(buffer, c_filename); 
      merchbatch_log_message( 2, 3, buffer, "open_credit_file", 0 );
	  write_to_dump_file(buffer);

      return(false);
   }

   return (true);

} /* open_credit_file */


/*******************************************************************************
*
*  FUNCTION:         open_debit_file
*
*  DESCRIPTION:      This function opens the debit settlement file 
*                    or creates it if it doesn't exist. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*  AUTHOR:           Darcy Person
*   
*******************************************************************************/
BOOLEAN open_debit_file()
{   
   CHAR              buffer[500];

   memset(buffer, 0, sizeof(buffer));

   /* Send message to system monitor */
   strcpy(buffer,"Open debit settlement file ");
   merchbatch_log_message( 2, 1, buffer, "open_debit_file", 0 );
   /* Send message to DOS window in debug mode */
   strcat(buffer,"\n");
   PRINT(buffer);

   /*** Open file ***/  
   strcpy(d_filename, path);
   #ifndef WIN32
	   strcat(d_filename, "eftpos/");
   #else
	   strcat(d_filename, "eftpos\\");
   #endif
   strcat(d_filename, "debit_eftpos"); 
   strcat(d_filename, cutoff_date);
   strcat(d_filename, ".upl"); 
   d_file_write = fopen (d_filename, "w");

   if (d_file_write == NULL_PTR)
   {  
      strcpy(buffer, "Cannot open debit settlement file: ");
      strcat(buffer, d_filename); 
      merchbatch_log_message( 2, 3, buffer, "open_debit_settle_file", 0 );
	  write_to_dump_file(buffer);
      
      return(false);
   }

   return (true);

} /* open_debit_file */

/*******************************************************************************
*
*  FUNCTION:         open_exception_file
*
*  DESCRIPTION:      This function opens the exception EPOS file 
*                    or creates it if it doesn't exist. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*  AUTHOR:           Abhishek Kumar Verma
*   
*******************************************************************************/
BOOLEAN open_exception_file(void)
{   
   CHAR              buffer[500];
   INT                file_name_len=0;

   memset(buffer, 0, sizeof(buffer));   

   /* Send message to system monitor */
   strcpy(buffer,"Open exception EPOS file ");
   merchbatch_log_message( 2, 1, buffer, "open_exception_file", 0 );
   /* Send message to DOS window in debug mode */
   strcat(buffer,"\n");
   PRINT(buffer);

   /*** Open file ***/
   strcpy(E_filename, path);
   #ifndef WIN32
	   strcat(E_filename, "eftpos/");
   #else
	   strcat(E_filename, "eftpos\\");
   #endif
   file_name_len=strlen(E_filename);
   strcat(E_filename,"exception_");
   strcat(E_filename, c_filename +file_name_len);
   file_name_len=strlen(E_filename);
   strncpy(E_filename +(file_name_len -3),"txt",3);

   E_file_write = fopen (E_filename, "w");

   if (E_file_write == NULL_PTR)
   {  
      strcpy(buffer, "Cannot open credit EPOS file: ");
      strcat(buffer, E_filename); 
      merchbatch_log_message( 2, 3, buffer, "open_exception_file", 0 );
	  write_to_dump_file(buffer);

      return(false);
   }

   return (true);

} /* open_credit_file */


/*************************************************************************************
*
*  NAME:           returnCurrentTime
*
*  DESCRIPTION:    This function return the current time in HHMMSS frormat.
*
*  INPUTS:         None
*         
*  OUTPUTS:        NONE
*
*  RETURNS:        None
*
*  AUTHOR:         Farad 10-28-1999  
*   
*************************************************************************************/
void returnCurrentTime(char *currentTime)
{

   time_t tme;
   struct tm *tm1;
   CHAR timeString[20]={0};

   /*Get the time in seconds since 0 hrs from 1/1/70*/
   tme = time(NULL_PTR);
   tm1 = localtime(&tme);
  
   sprintf(timeString  ,"%2.2d",tm1->tm_hour);
   sprintf(timeString+2,"%2.2d",tm1->tm_min);
   sprintf(timeString+4,"%2.2d",tm1->tm_sec);

   strcpy(currentTime,timeString);
}


/*****************************************************************************
*
*  FUNCTION:         Rj_with_lead_zeros
*
*  DESCRIPTION:      This function will right justify a string and 
*                    add leading zeros
*
*  INPUTS:           str - the ascii string to be right justified 
*                    FieldWidth
*
*  OUTPUTS:          str - the right justified ascii string
*
*  RETURN VALUE:     None
*
*  AUTHOR:           Unknown
*
******************************************************************************/
void Rj_with_lead_zeros( pCHAR str, INT FieldWidth )
{
   INT               i;
   INT               dif;
   INT               strlength;

   strlength = strlen(str);

   /* Convert any leading blanks to zero. */
   for (i = 0; i < strlength; i++)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x30;
      else
         break;
   }

   /* Remove any trailing blanks. */
   for (i = strlength-1; i >= 0; i--)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x00;
      else
         break;
   }

   strlength = strlen(str);
   dif       = FieldWidth - strlength;

   if (dif > 0)
   {
      /* Right justify, then prepend leading zeros. */
      memmove (&str [dif], &str[0], strlength+1);
      memset  (&str[0], 0x30, dif);
   }

} /* Rj_with_lead_zeros */


/*************************************************************************************
*
*  FUNCTION:         set_date
*
*  DESCRIPTION:      This function uses the cutoff date and time 
*                    along with the number of hours to look back 
*                    to compute the date/time values needed for 
*                    the retrieval of information from the database 
*                    table bch01.
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     None
*
*  AUTHOR:           Darcy Person   
*
*************************************************************************************/
void set_date()
{
   CHAR              year_yyyy[5];

   INT               day_m_int;
   INT               month_int;
	INT               offset_tmp = 0;
	INT               temp_div = 0;
	INT               temp_rem = 0;
	INT               temp_var = 0;
   INT               year_int;

   LONG              julian;

   time_t            tme;
   struct            tm *tm1;

   PRINT ("Set date\n");

   /* Initialize variables */
   memset(time_tag,        0, sizeof(time_tag));
   memset(yyyymmdd_start,  0, sizeof(yyyymmdd_start));
   memset(yyyymmdd_finish, 0, sizeof(yyyymmdd_finish));

   /* Get time tag */
   returnCurrentTime(time_tag);

   /* Calculate today's date as the finish date   */
   /* if the cutoff date uses the comma shorthand */
   if (strcmp(cutoff_date, ",") == 0)
   {
      tme = time(NULL_PTR);
      tm1 = localtime(&tme);

      /* Put today's date in YYYYMMDD format */
      month_int = tm1->tm_mon + 1;
      day_m_int = tm1->tm_mday;
      year_int  = tm1->tm_year;
      year_int  = year_int + 1900;

      memset(day_m,           0, sizeof(day_m));
      memset(month,           0, sizeof(month));
      memset(year_yyyy,       0, sizeof(year_yyyy));

      sprintf(month, "%02d", month_int);
      sprintf(day_m, "%02d", day_m_int);
      itoa(year_int, year_yyyy, 10);
      strncpy(yyyymmdd_finish, year_yyyy, 4);
      strncpy(yyyymmdd_finish + 4, month, 2);
      strncpy(yyyymmdd_finish + 6, day_m, 2);
      strcpy(cutoff_date, yyyymmdd_finish);
   }
   else
   {
      strncpy(month, cutoff_date + 4, 2);
      strncpy(day_m, cutoff_date + 6, 2);
      strcpy(yyyymmdd_finish, cutoff_date);
   }

   /* Set values for extraction date */
   strcpy(extract_date, yyyymmdd_finish + 2); //+4
   //strncpy(extract_date + 4, yyyymmdd_finish + 2, 2);

   /* Determine start date */
   julian = Txutils_Calculate_Julian_Date(yyyymmdd_finish);

   temp_rem = atoi(offset) % 24;
	temp_div = (atoi(offset) / 24 );
	

	julian = julian - temp_div;

	if( temp_rem > atoi(cutoff_hour) )
	{
		julian = julian - 1;
		offset_tmp = 24 - (temp_rem - atoi(cutoff_hour));
	}
	else 
      offset_tmp = atoi(cutoff_hour) - temp_rem;

	Txutils_Calculate_Gregorian_Date(julian, yyyymmdd_start);

   /* Determine start time */
	if( (offset_tmp / 10 ) == 0)
			sprintf(offset,"0%d",offset_tmp);
		else 
			sprintf(offset,"%d",offset_tmp);

} /* set_date */
/*******************************************************************************
*
*  FUNCTION:         write_to_CUP_credit_settle_file
*
*  DESCRIPTION:      This function writes to credit settlement file. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*   
*******************************************************************************/
BOOLEAN write_to_CUP_credit_settle_file()
{
   CHAR              tempstr[16];
   CHAR              buffer[100];   


   /*** Write to file ***/
   if (fputs(c_record_cup,c_file_write) < 0)
   {
      itoa (ferror(c_file_write), tempstr, 10);
      fclose (c_file_write);
      
      strcpy(buffer, "Cannot write to credit settlement file: ");
      strcat(buffer, tempstr);
      merchbatch_log_message( 2, 3, buffer, "write_to_CUP_credit_settle_file", 0 );
	  write_to_dump_file(buffer);

      return(false);
   }   

   return (true);

} /* write_to_CUP_credit_settle_file */

/*******************************************************************************
*
*  FUNCTION:         write_to_CUP_Exception_file
*
*  DESCRIPTION:      This function writes to credit settlement file. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*   
*******************************************************************************/
BOOLEAN write_to_CUP_Exception_file()
{
	CHAR      tempstr[16]="";
	CHAR      buffer[100]="";   
	BYTE  	  temp_year[9]="";

	//Varibales used for Over limit percentage calculation
	double  CUP_Overlimit_per = 0.0;
	BYTE  	CUP_OVERLIMIT[20]="0";
	BYTE   	CUP_Auth_amt[20]="";
	BYTE    CUP_Overlimit[20]="";
	BYTE 	temp_Cup_Overlimit[13]="";
	BYTE    temp_CUP_Over_limit[13]="";
	BYTE   	OVER_rem[3]="00";
	
	//Variable used to Settlement Amount (BCH10)
	double 	Cup_Settlement_amount=0.0;
	BYTE   	settle_rem[3]="";
	BYTE 	temp_Setl_amount[20] ="";
	BYTE	temp_Cup_Settlement_amount[13] ="";
	BYTE 	Setl_AMT[20] ="";
			
	//varibale used for AUTH Amount and CUP expection Amount
	double  CUP_Auth_excp_amount=0.0;
	double  CUP_Sale_Amount=0.0;
	double 	CUP_Auth_amount=0.0;
	BYTE 	temp_Cup_exp_Auth_amount[13]="";
	BYTE 	temp_AUTH_amount[20] ="";
	BYTE 	AUTH_AMT[20] ="";
	BYTE   	auth_rem[3]="";
	char    Auth_Amount[13]="";
	
	BYTE   	temp_card_num[20]="";
	BYTE   	temp_card_len=0;
	char    ErrorMsg[100]="";
	char    remarks[41]="";
	BYTE    ret_type;
	INT 		i =0;
	strncpy(temp_year,auth_tx_detail.tran_date,4);
	strncat(temp_year,auth_tx_detail.product_codes[19].code,4);

	/*Let us get CUP Auth Amount from bch20 table*/
	ret_type = db_get_CUP_Auth_Amount_bch20(&auth_tx_detail,Auth_Amount,ErrorMsg);
	if(ret_type != PTEMSG_OK)
	{
		// No corresponding CARD VER/PRE-AUTH is not present
		strcpy(remarks,"No corresponding CARD VER/PRE-AUTH");
		// No Need to preapre Auth Amount and overlimit
	}
	else
	{
		// Let us use the Auth Amount received from BCH20 and find the exception amount.
		/*Calculate CUP Settlement Amount */
		CUP_Auth_amount=(strtod (Auth_Amount, 0) / 100 );
		CUP_Auth_excp_amount = (Cup_Settlement_amount_percentage * CUP_Auth_amount)/100;
	
		// CUP Settlement Amount
		CUP_Sale_Amount=(strtod (auth_tx_detail.tran_amount, 0) / 100 ); // We are getting it from BCH10
	
		/*Let us find if Settlement Amount is more than Excpetion Amount*/
		if(CUP_Auth_excp_amount>=CUP_Sale_Amount)
		{
			// No need to creat CUP excpetion file
			return true;
		}
		else
		{
			sprintf(remarks,"Above settlement value flag - <%d> -",Cup_Settlement_amount_percentage);
			//Lets Prepare AUTH Amount that will store in exception files
			strcpy (CUP_Auth_amt, Auth_Amount) ;
			strncpy(auth_rem,CUP_Auth_amt+10,2); // Auth Settle  BCH20
			trimleadingZero(CUP_Auth_amt,temp_AUTH_amount);
			formatAmount(temp_AUTH_amount,AUTH_AMT);
		
			//Lets Prepare overlimit that will store in exception files
			//Let us find the overlimit.
			CUP_Overlimit_per = (CUP_Sale_Amount/CUP_Auth_amount)*100;
			Convert_Amt_To_Auth_Tx_String_Format(CUP_Overlimit_per, temp_Cup_Overlimit) ;
			LeftFill_String_With_Character ((sizeof(temp_Cup_Overlimit)-1), temp_Cup_Overlimit,'0') ;
			strcpy (CUP_Overlimit, temp_Cup_Overlimit) ;
			strncpy(OVER_rem,CUP_Overlimit+10,2); // Auth Settle  BCH20
			trimleadingZero(CUP_Overlimit,temp_CUP_Over_limit);
			formatAmount(temp_CUP_Over_limit,CUP_OVERLIMIT);
		}
	}
	
	//Lets Prepare Settlement Amount that will store in exception files
	trimleadingZero(auth_tx_detail.tran_amount,temp_Setl_amount);
	formatAmount(temp_Setl_amount,Setl_AMT);
	strncpy(settle_rem,auth_tx_detail.tran_amount+10,2); // Settlement BCH10
	
	temp_card_len=strlen(auth_tx_detail.card_nbr);
	strcat(temp_card_num,auth_tx_detail.card_nbr);
	if(temp_card_len <19)
	{
		for( i = temp_card_len; i <19;i++)
		{
			strcat(temp_card_num ," " );
		}
	}

	sprintf(c_record_cup_exception, "%19s%11s%12s%23s.%02s%17s.%02s%9s%16s%21s%8s.%02s",
			temp_card_num,
			auth_tx_detail.tran_date,
			temp_year,
			AUTH_AMT,
			auth_rem,
			Setl_AMT,
			settle_rem,
			auth_tx_detail.auth_number,
			auth_tx_detail.primary_key.device_id,
			auth_tx_detail.merchant_id,
			CUP_OVERLIMIT,
			OVER_rem
			);
	
   strncpy(c_record_cup_exception + 145, "\n\0", 2);

   /*** Write to file ***/
   if (fputs(c_record_cup_exception,E_file_write) < 0)
   {
      itoa (ferror(E_file_write), tempstr, 10);
      fclose (E_file_write);
      
      strcpy(buffer, "Cannot write to credit settlement file: ");
      strcat(buffer, tempstr);
      merchbatch_log_message( 2, 3, buffer, "write_to_CUP_Exception_file", 0 );
	  write_to_dump_file(buffer);

      return(false);
   }   

   return (true);

} /* write_to_CUP_Exception_file */


/*******************************************************************************
*
*  FUNCTION:         write_to_CUP_Exception_file_arc
*
*  DESCRIPTION:      This function writes to credit settlement file. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*   
*******************************************************************************/
BOOLEAN write_to_CUP_Exception_file_arc()
{
	CHAR      tempstr[16]="";
	CHAR      buffer[100]="";   
	BYTE  	  temp_year[9]="";

	//Varibales used for Over limit percentage calculation
	double  CUP_Overlimit_per = 0.0;
	BYTE  	CUP_OVERLIMIT[20]="0";
	BYTE   	CUP_Auth_amt[20]="";
	BYTE    CUP_Overlimit[20]="";
	BYTE 	temp_Cup_Overlimit[13]="";
	BYTE    temp_CUP_Over_limit[13]="";
	BYTE   	OVER_rem[3]="00";
	BYTE    OVER_LIM_PER[20]="";
	
	//Variable used to Settlement Amount (BCH10)
	double 	Cup_Settlement_amount=0.0;
	BYTE   	settle_rem[3]="";
	BYTE 	temp_Setl_amount[20] ="";
	BYTE	temp_Cup_Settlement_amount[13] ="";
	BYTE 	Setl_AMT[20] ="";
			
	//varibale used for AUTH Amount and CUP expection Amount
	double  CUP_Auth_excp_amount=0.0;
	double  CUP_Sale_Amount=0.0;
	double 	CUP_Auth_amount=0.0;
	BYTE 	temp_Cup_exp_Auth_amount[13]="";
	BYTE 	temp_AUTH_amount[20] ="";
	BYTE 	AUTH_AMT[20] ="";
	BYTE   	auth_rem[3]="";
	char    Auth_Amount[13]="";
	
	BYTE   	temp_card_num[20]="";
	BYTE   	temp_card_len=0;
	char    ErrorMsg[100]="";
	char    remarks[41]="";
	BYTE    ret_type;
	INT 		i =0;

	strncpy(temp_year,auth_tx_detail_arc.tran_date,4);
	strncat(temp_year,auth_tx_detail_arc.product_codes[19].code,4);

	/*Let us get CUP Auth Amount from bch20 table*/
	ret_type = db_get_CUP_Auth_Amount_bch20(&auth_tx_detail_arc,Auth_Amount,ErrorMsg);
	if(ret_type != PTEMSG_OK)
	{
		// No corresponding CARD VER/PRE-AUTH is not present
		strcpy(remarks,"No corresponding CARD VER/PRE-AUTH");
		
		// No Need to preapre Auth Amount and overlimit
	}
	else
	{
		// Let us use the Auth Amount received from BCH20 and find the exception amount.
		/*Calculate CUP Settlement Amount */
		CUP_Auth_amount=(strtod (Auth_Amount, 0) / 100 );
		CUP_Auth_excp_amount = (Cup_Settlement_amount_percentage * CUP_Auth_amount)/100;
	
		// CUP Settlement Amount
		CUP_Sale_Amount=(strtod (auth_tx_detail_arc.tran_amount, 0) / 100 ); // We are getting it from BCH10
	
		/*Let us find if Settlement Amount is more than Excpetion Amount*/
		if(CUP_Auth_excp_amount>=CUP_Sale_Amount)
		{
			// No need to creat CUP excpetion file
			return true;
		}
		else
		{
			sprintf(remarks,"Above settlement value flag - <%d> -",Cup_Settlement_amount_percentage);
			//Lets Prepare AUTH Amount that will store in exception files
			strcpy (CUP_Auth_amt, Auth_Amount) ;
			strncpy(auth_rem,CUP_Auth_amt+10,2); // Auth Settle  BCH20
			trimleadingZero(CUP_Auth_amt,temp_AUTH_amount);
			formatAmount(temp_AUTH_amount,AUTH_AMT);
		
			//Lets Prepare overlimit that will store in exception files
			//Let us find the overlimit.
			CUP_Overlimit_per = (CUP_Sale_Amount/CUP_Auth_amount)*100;
			Convert_Amt_To_Auth_Tx_String_Format(CUP_Overlimit_per, temp_Cup_Overlimit) ;
			LeftFill_String_With_Character ((sizeof(temp_Cup_Overlimit)-1), temp_Cup_Overlimit,'0') ;
			strcpy (CUP_Overlimit, temp_Cup_Overlimit) ;
			strncpy(OVER_rem,CUP_Overlimit+10,2); // Auth Settle  BCH20
			trimleadingZero(CUP_Overlimit,temp_CUP_Over_limit);
			formatAmount(temp_CUP_Over_limit,CUP_OVERLIMIT);
		}
	}
	
	//Lets Prepare Settlement Amount that will store in exception files
	trimleadingZero(auth_tx_detail_arc.tran_amount,temp_Setl_amount);
	formatAmount(temp_Setl_amount,Setl_AMT);
	strncpy(settle_rem,auth_tx_detail_arc.tran_amount+10,2); // Settlement BCH10
	
	temp_card_len=strlen(auth_tx_detail_arc.card_nbr);
	strcat(temp_card_num,auth_tx_detail_arc.card_nbr);
	if(temp_card_len <19)
	{
		for( i = temp_card_len; i <19;i++)
		{
			strcat(temp_card_num ," " );
		}
	}

	sprintf(c_record_cup_exception, "%19s%11s%12s%23s.%02s%17s.%02s%9s%16s%21s%8s.%02s",
			temp_card_num,
			auth_tx_detail_arc.tran_date,
			temp_year,
			AUTH_AMT,
			auth_rem,
			Setl_AMT,
			settle_rem,
			auth_tx_detail_arc.auth_number,
			auth_tx_detail_arc.primary_key.device_id,
			auth_tx_detail_arc.merchant_id,
			CUP_OVERLIMIT,
			OVER_rem
			);
	
   strncpy(c_record_cup_exception + 145, "\n\0", 2);

   /*** Write to file ***/
   if (fputs(c_record_cup_exception,E_file_write) < 0)
   {
      itoa (ferror(E_file_write), tempstr, 10);
      fclose (E_file_write);
      
      strcpy(buffer, "Cannot write to credit settlement file: ");
      strcat(buffer, tempstr);
      merchbatch_log_message( 2, 3, buffer, "write_to_CUP_Exception_file_arc", 0 );
	  write_to_dump_file(buffer);

      return(false);
   }   

   return (true);


} /* write_to_CUP_Exception_file_arc */


/*******************************************************************************
*
*  FUNCTION:         write_to_credit_settle_file
*
*  DESCRIPTION:      This function writes to credit settlement file. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*   
*******************************************************************************/
BOOLEAN write_to_credit_settle_file()
{
   CHAR              tempstr[16];
   CHAR              buffer[100];   

   /* Send message to system monitor */
 /*  strcpy(buffer,"Writing to credit settlement file ");
   	 merchbatch_log_message( 2, 2, buffer, "write_to_credit_settle_file", 0 );*/

   /*** Write to file ***/
   if (fputs(c_record,c_file_write) < 0)
   {
      itoa (ferror(c_file_write), tempstr, 10);
      fclose (c_file_write);
      
      strcpy(buffer, "Cannot write to credit settlement file: ");
      strcat(buffer, tempstr);
      merchbatch_log_message( 2, 3, buffer, "write_to_credit_settle_file", 0 );
	  write_to_dump_file(buffer);

      return(false);
   }   

   return (true);

} /* write_to_credit_settle_file */


/*******************************************************************************
*
*  FUNCTION:         write_to_dump_file() 
*
*  DESCRIPTION:      This function writes an error record to the credit dump file. 
*					 The file will be created if it does not already exist.
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
**********************************************************************************/
void write_to_dump_file(CHAR* dump_record)
{
   CHAR              tempstr[16] = "";
   CHAR              buffer[500] = ""; 
   CHAR				 time_hhmmss[7] = "";
   CHAR				 time_str[10] = "";


   memset (&buffer, 0x00, sizeof(buffer));

   returnCurrentTime(time_hhmmss);
   sprintf(time_str, "%2.2s:%2.2s:%2.2s:\n", time_hhmmss, time_hhmmss+2, time_hhmmss+4);

   /* Check if file exists*/
   if (dumpfile_ptr == NULL_PTR)
   {
       /*** Open dump file ***/
	   memset (&dump_filename, 0x00, sizeof(dump_filename));
       strcpy(dump_filename, path);
       #ifndef WIN32
	       strcat(dump_filename, "eftpos/settle_");
       #else
	       strcat(dump_filename, "eftpos\\settle_");
       #endif

	   strcat(dump_filename, cutoff_date);
	   strcat(dump_filename, "_dump.txt");


	   /* Send message to system monitor */
       sprintf(buffer,"Opening dump file %s.", dump_filename);
       merchbatch_log_message( 2, 1, buffer, "write_to_dump_file", 0 );
       /* Send message to DOS window in debug mode */
       strcat(buffer,"\n");
       PRINT(buffer);
	   
	   dumpfile_ptr = fopen (dump_filename, "w");

	   if (dumpfile_ptr == NULL_PTR)
	   {  
		  memset (&buffer, 0x00, sizeof(buffer));
		  sprintf(buffer, "Cannot open dump file %s.", dump_filename);
		  merchbatch_log_message( 2, 3, buffer, "write_to_dump_file", 0 );
		  return;					      /*will try to continue settlement process*/
	   }
	   else
	   {
	   	   sprintf(buffer, "Settlement Dump File: %s\n", dump_filename); 
		   fputs (buffer, dumpfile_ptr);
	   }
   }

   /*else file exists or was opened successfully*/
   /*** Write to file ***/
   fputs(time_str, dumpfile_ptr);
   if (fputs (dump_record, dumpfile_ptr) < 0)
   {
      itoa (ferror(dumpfile_ptr), tempstr, 10);
      fclose (dumpfile_ptr);
      
      sprintf(buffer, "Cannot write to dump file %s: %s", dump_filename, tempstr);
      merchbatch_log_message( 2, 3, buffer, "write_to_dump_file", 0 );
   }
   return;

} /* write_to_dump_file */



/*******************************************************************************
*
*  FUNCTION:         write_to_debit_settle_file
*
*  DESCRIPTION:      This function writes to debit settlement file. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN: true for success or false for failure.
     
*************************************************************************************/
BOOLEAN write_to_debit_settle_file()
{
   CHAR              tempstr[16];
   CHAR              buffer[100];  

   /* Send message to system monitor 
   strcpy(buffer,"Writing to debit settlement file ");
   merchbatch_log_message( 2, 1, buffer, "write_to_debit_settle_file", 0 );
   /* Send message to DOS window in debug mode 
   strcat(buffer,"\n");
   PRINT(buffer); */
   
   /*** Write to file ***/
   if (fputs (d_record, d_file_write) < 0)
   {
      itoa (ferror(d_file_write), tempstr, 10);
      fclose (d_file_write);
      
      strcpy(buffer, "Cannot write to debit settlement file: ");
      strcat(buffer, tempstr);
      merchbatch_log_message( 2, 3, buffer, "write_to_debit_settle_file", 0 );
	  write_to_dump_file(buffer);

      return(false);
   }
   return (true);

} /* write_to_debit_settle_file */

 
/*******************************************************************************
*
*  FUNCTION:         bch01_handler
*
*  DESCRIPTION:      This function processes each batch to the settlement files. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN : true successful, false unsuccessful
*
*             
*   
*******************************************************************************/
BOOLEAN bch01_handler()
{
   CHAR     err_msg[100];
   CHAR     buffer[500];
   CHAR     merchant_id[16]="";
   BOOLEAN  ret_val = true;
   int cre_amt=0,deb_amt=0;

   memset( &err_msg,   0x00, sizeof(err_msg)  );
   memset( &buffer,    0x00, sizeof(buffer)   );
   memset( &tc_totals, 0x00, sizeof(tc_totals));
   memset( &merchant_id,    0x00, sizeof(merchant_id)   );

   
   /*** Set batch totals ***/
   strcpy(tc_totals.batch_number, auth_tx_total.primary_key.batch_nbr);
   strcpy(tc_totals.terminal_id, auth_tx_total.primary_key.device_id);
   strcpy(tc_totals.merchant_id, auth_tx_total.merchant_id);
   strcpy(tc_totals.open_date, auth_tx_total.open_date);
   strcpy(tc_totals.open_time, auth_tx_total.open_time);
   strcpy(merchant_id, auth_tx_total.merchant_id); // We will pass this merchangt ID for creating settle files.
   itoa(atoi(auth_tx_total.total_credit_cnt) + 
        atoi(auth_tx_total.total_credit_refund_cnt), 
		tc_totals.number_credit_tran_for_batch, 9);
   itoa(atoi(auth_tx_total.total_debit_cnt) + 
        atoi(auth_tx_total.total_debit_refund_cnt), 
        tc_totals.number_debit_tran_for_batch, 9);
   itoa(atoi(auth_tx_total.total_credit_amt) + 
        atoi(auth_tx_total.total_credit_refund_amt), 
        tc_totals.totals_credit_for_batch, 10);
   itoa(atoi(auth_tx_total.total_debit_amt) + 
        atoi(auth_tx_total.total_debit_refund_amt), 
         tc_totals.totals_debit_for_batch, 10);   
   


   /* Set flags to show if there are credit and/or debit txns. */
   if ( atoi(tc_totals.number_credit_tran_for_batch) > 0 )
      credit_txns_exist = true;
   else
      credit_txns_exist = false;

   if ( atoi(tc_totals.number_debit_tran_for_batch) > 0 )
      debit_txns_exist = true;
   else
      debit_txns_exist = false;

/* Fixed the issue for missing Currency code 
 Girija Y, TF Feb 19, 2009 */
	if (get_currency_code() == false)
   {
	  memset(&buffer, 0x00, sizeof(buffer));
	  sprintf(buffer, "Batch %s Not Settled. Error retrieving currency code.", tc_totals.batch_number);
      merchbatch_log_message( 2, 1, buffer, "bch01_handler", 0 );

	  memset(&dump_record, 0x00, sizeof(dump_record));
	  sprintf(dump_record,"%s  Merchant: %s  Terminal: %s\n",buffer, tc_totals.merchant_id, tc_totals.terminal_id);
	  write_to_dump_file(dump_record );

    return true; 
	}  

   if (create_settle_files(&merchant_id) == false)
      ret_val = false; 
	
   else
   {		// Update Batch Status with "W"- TF Phani
      /*** update batch status with "W" ***/
      if (strcmp(auth_tx_total.batch_status, "C") == 0 ||  strcmp(auth_tx_total.batch_status, "R") == 0)
      {
     	 if(strcmp(auth_tx_total.batch_status, "C") == 0)
			 strcpy(auth_tx_total.batch_status, "P");
      	 else if(strcmp(auth_tx_total.batch_status, "R") == 0) 
			 strcpy(auth_tx_total.batch_status, "W");

			 if (db_update_bch01_batch_Status_After_Writing_File(&auth_tx_total,err_msg) != PTEMSG_OK)
	         {
	            dbcommon_rollback ();

	            sprintf( buffer,
	                    "Error updating bch10 batch status , batch(%s):Terminal ID(%s):Merchant ID(%s): %s",
	                     auth_tx_total.primary_key.batch_nbr,auth_tx_total.primary_key.device_id,auth_tx_total.merchant_id,err_msg );
	           merchbatch_log_message( 2, 2, buffer, "bch01_handler", 0 );
		       /*Write Batch Information to dump file*/
		       memset(&dump_record, 0x00, sizeof(dump_record));
		       sprintf(dump_record,"%s  %s  %s\n", buffer, tc_totals.merchant_id, tc_totals.terminal_id);
		       write_to_dump_file(dump_record );

	            /* Do not return 'false' here.  If you do, the other batches
	                      will not get processed.  */
	         }
	         else
	         {
	            /*** commit the database update(s) ***/
	            dbcommon_commit ();
	         }
       }/* end of if batch status is "C" or "R" */
		
  } // end of Else case
   
   
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         GET_DATE
 *
 *  DESCRIPTION:  This procedure takes an input string that contains a date and
 *                time.  It copies the date portion into an output string,
 *                formatted slightly different.
 *
 *  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *
 *  OUTPUTS:      date_str  : Format = "YYYY0M0D"
 *
 *  RTRN VALUE:   None
 *
 *********************************************************************************/
void get_date( pCHAR time_date, pCHAR date_str )
{
   memset ( date_str,  0x00,         9 );
   strncpy( date_str,  time_date,    4 );
   strncat( date_str, &time_date[5], 2 );
   strncat( date_str, &time_date[8], 2 );
}
   

/****************************************************************************************/


void Strrev( char * string)
{
	int len = 0;
	int i = 0;
	int j =0 ;
    BYTE  temp_str[20]="";
	while (string[i++]!='\0')
	{
		len++;
	}

	 for(i=len-1 ;i>=0;i--)
	 {
	   temp_str[j]=string[i];
	   j++;
	 }
	  temp_str[j]='\0';
	  strcpy(string,temp_str);
}


void trimleadingZero(char * string1, char * string2)
{
	int i;
	int len=0;
	len = strlen(string1);
	
		for(i =0; i < len; i++)
		{
			if(0!=strncmp(string1+i,"0",1))
			{
				strcat(string2,string1+i);
				i = len;
			}
		}
}

void formatAmount(char * string1, char * string2)
{
	INT temp_amount_len = 0;
	BYTE amount[20]="";
	BYTE TEMP_AMT[20]="";
	INT i=0;
	
	temp_amount_len = strlen(string1);
		if(temp_amount_len>2)
		{
			strncpy(amount,string1,(temp_amount_len - 2));
			temp_amount_len= strlen(amount);
			strncpy(TEMP_AMT,amount,temp_amount_len);
			Strrev(TEMP_AMT);
			if(temp_amount_len > 3)
			{
				memset(amount,0x00,20);
				for(i =0; i < temp_amount_len; i++)
				{
					strncat(amount,TEMP_AMT+i,3);
					strcat (amount, ",");
					i=i+2;
				}
				Strrev(amount);
				if(0==strncmp(amount,",",1))
				{
					amount[0]=' ';
				}
			}
		}
		else
		{
			strncpy(amount,"0",1);
		}
		temp_amount_len = strlen(amount);
	    strncpy(string2,amount,temp_amount_len);

}



/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void merchbatch_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  merchbatch_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   merchbatch_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(merchbatch_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  merchbatch_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(merchbatch_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
								  "500",  					/* Default string                 */
								  merchbatch_error_warning_file_size,              	/* points to destination buffer   */
								  sizeof(merchbatch_error_warning_file_size)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(merchbatch_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/
   if((strlen(merchbatch_error_warning_file_path) > 0) &&
	  (strlen(merchbatch_module_error_warning_file_name)> 0))
   {
	   merchbatch_create_Error_Filename();
   }
}

/******************************************************************************
 *
 *  NAME:         merchbatch_log_message
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void merchbatch_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [512] = {0};
   INT  Appname_len = 0;
   CHAR print_buffer[256] = {0} ;
   INT text_message_len = 0;

   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   /* Set Event Log flag */
   /*if ( dest > 1 )
      eventlog_flag = 1;
   else
      eventlog_flag = 0;*/

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
   sprintf(text_message,"%s ,",AppName);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   /* Call function to post the message. */
   merchbatch_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	   text_message_len = strlen(text_message);
	   if(text_message_len > 255)
	   {
		   strncpy(print_buffer,text_message,254); // Maximum msg sent to moniotr is 256
		   TxUtils_Send_Msg_To_Operator( monitor_flag, print_buffer,
									 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
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
INT merchbatch_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func, int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	CHAR card_tid_mid_details [256] = {0};
	CHAR tempcard[21] = {0};
	CHAR masked_cardnum[21] = {0};
	char *cardIndex = NULL ;
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;


	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(merchbatch_Erro_warning_Filename);
    path_len = strlen(merchbatch_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&merchbatch_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	merchbatch_create_Error_Filename();
    }
	if((fp = fopen(merchbatch_Erro_warning_Filename,"a+b"))==NULL)
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

void merchbatch_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( merchbatch_Erro_warning_Filename, merchbatch_error_warning_file_path );
    strcat( merchbatch_Erro_warning_Filename, merchbatch_module_error_warning_file_name );
	strcat( merchbatch_Erro_warning_Filename, ".EW." );
    strncat(merchbatch_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(merchbatch_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(merchbatch_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

/*MobilePOS NFC Tap2Phone Transaction*/

INT merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( )
{
	 if(0 == strncmp(auth_tx_detail.pos_entry_mode,
			 	 	 POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


INT merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_with_pin_arc( )
{
	if(0 == strncmp(auth_tx_detail_arc.pos_entry_mode,
					POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( )
{
	 if(0 == strncmp(auth_tx_detail.pos_entry_mode,
			 	 	 POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


INT merchbatch_check_if_trx_is_MobilePOS_NFC_Tap2Phone_trx_without_pin_arc( )
{
	if(0 == strncmp(auth_tx_detail_arc.pos_entry_mode,
					POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

/***********************************************/

/*Swiftpass BDOPay Credit Transaction*/

INT merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin( )
{
	 if(0 == strncmp(auth_tx_detail.pos_entry_mode,
			 	 	 POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


INT merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_with_pin_arc( )
{
	if(0 == strncmp(auth_tx_detail_arc.pos_entry_mode,
					POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin( )
{
	 if(0 == strncmp(auth_tx_detail.pos_entry_mode,
			 	 	 POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


INT merchbatch_check_if_trx_is_Swiftpass_BDO_Pay_credit_trx_without_pin_arc( )
{
	if(0 == strncmp(auth_tx_detail_arc.pos_entry_mode,
					POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

/***********************************************/

/*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

INT merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trxn( )
{
	 if((0 == strncmp(auth_tx_detail.pos_entry_mode,
			 	 	  POS_ENTRY_MODE_FOR_ECOM_TXN,
					  POS_ENTRY_MODE_LEN)) &&
	    (0 == strncmp(auth_tx_detail.product_codes[6].amount,
	    			  POS_ECOM_POS_CONDITION_CODE,
					  POS_CONDITION_CODE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT merchbatch_check_if_trx_is_Ecommerce_Swiftpass_BDO_Pay_credit_trx_arc( )
{
	if((0 == strncmp(auth_tx_detail_arc.pos_entry_mode,
					 POS_ENTRY_MODE_FOR_ECOM_TXN,
					POS_ENTRY_MODE_LEN)) &&
	   (0 == strncmp(auth_tx_detail_arc.product_codes[6].amount,
			   	     POS_ECOM_POS_CONDITION_CODE,
					 POS_CONDITION_CODE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

/**********************************************/
