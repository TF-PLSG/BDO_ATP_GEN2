/********************************************************************************
*  
* MODULE:      Txauthorize.C
*  
* TITLE:       
*  
* DESCRIPTION:  
*
* APPLICATION: TXCNTL
*
* AUTHOR:  Sanjoy Dasgupta
*
* REVISION HISTORY:
*

*******************************************************************************/
#include <stdlib.h>				// for atoi function prototype
#include <math.h>				   // for HUGE_VAL definition
#include "txauthorize.h"
#include "genutil.h"
#include "txservice.h"
#include "txfleet.h"
#include "txutils.h"
#include "ntutils.h"
#include "pteipc.h"
#include "memmnger.h"
#include "myutils.h"
#include "txdataserver.h"
#include "cup.h"

extern AUTH_TX Auth_Tx ;
extern MCC_LIMITS  Mcc_Limits;
extern PRIVATE_CCF03P_STRUCT private_ccf03p_struct ;
extern CHAR crypto_que_name[] ;
extern CHAR tran_dataserver_que_name[] ;
extern CHAR	pos_rc[3];
extern CHAR pos_res_txt[40];
extern AMEX3DS2 amex_3ds2;
extern DCI3DS2 diners_3ds2;

/* Indicator for Authorization Number for ON-US txns.
 * When it changes, UAG01 in the database is updated.
 */
extern INT  AuthNumberChanged;
// MCC codes
extern MCC_CHECK mcc_check_list[MCC_MAX_CODES];
extern INT No_of_MCC_codes;
double nCCF02P_Incremented_Outstanding_Auths ;
double nACF01_Incremented_Outstanding ;
double nACF01_Incremented_Cash_Outstanding ;
double nACF01_Decremented_Available ;
double nACF01_Decremented_Cash_Available  ;
LONG   nCashBonusTransactionCounter = 0L ;
CHAR  errbuf_authid[256]; // TF PHANI

extern INT Falcon_RT_Flag;

extern INT SecondaryExpiryDateValidationFlag;

extern char cav2_validation_flag;
extern char cavv_verification_rc[3];

CRF01  crf01;
CR01   sCR01;
extern CUP3DS2 cup_3ds2;
extern char mc_3ds2_delcine_rc[3];

extern INT keyblock_enable_flag_for_MC ;
extern INT keyblock_enable_flag_for_VISA ;
extern INT keyblock_enable_flag_for_AMEX ;
extern INT keyblock_enable_flag_for_JCB ;
extern INT keyblock_enable_flag_for_UPI ;
extern INT keyblock_enable_flag_for_DCI ;
/*************************************************************************
NAME:			doubletostr
DESCRIPTION:	Contains logic for converting double to string with 12 length with 2 decimals	
INPUTS:			None
OUTPUTS:		None
RETURNS:             ""  if convertion failed
                           converted string   if conversion success
AUTHOR:              Praneeth
MODIFIED BY:	
**************************************************************************/

char *doubletostr(double f)
{
    int i, j = 0;
    static char s[13] = "";
    sprintf(s,"%012.2lf",f);
    for ( i = 0; s[i] != '\0'; i++ )
    {
        if ( s[i] != '.')
        {
            s[j++] = s[i];
        }
    }
    s[j] = '\0';
    return s;
} 

/*************************************************************************
NAME:			TxAuthorize_Authorize_Transaction
DESCRIPTION:	Contains logic for authorizing various transactions	
INPUTS:			None
OUTPUTS:		None
RETURNS:        false  if both transaction was not authorized
                true   if transaction was authorized
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
**************************************************************************/
BYTE TxAuthorize_Authorize_Transaction (void) 
{
   int nIndex = 0 ;

   BOOLEAN bTypeFound = false ;
   BYTE ReturnCode = 0 ;
   BYTE TransactionOrigin = 0 ;
   char strTemp[256] = {0} ;

   double nAuthTxTotalAmount = 0.0;
   double nAuthTxRevAmount   = 0.0;
   double nAuthTxOrigAmount  = 0.0;
	
   double nConvertedTxAmount = 0.0 ;
   double nConversionRate = 0.0 ;
   double nTotalAmount = 0.0 ;


   /* This is for Equitable only.  For JCB  and AMEX cards (ON-US), if the
    * currency is in US Dollars, it must be converted to Phillippino Peso
    * prior to processing it.  Earlier in local processing, if card family
    * was JCB or AMEX and currency code was USD, the currency code was set
    * to PESO and host start time was set to 0.00.  This was to allow txcntl
    * to hit the correct balance account.  So the check here is for JCB or AMEX
    * family, PESO, and host start time = 0.000.
    */
   if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000")     ) &&
        ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) &&
        ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)  )   ||
         (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )     ))
   {
      if ( !TxAuthorize_Convert_USD_to_Peso(&nAuthTxTotalAmount,
                                            &nAuthTxRevAmount,
                                            &nAuthTxOrigAmount) )
         return SYSTEM_ERROR ;
   }/* In case of STIP, Txn currency is Peso, if Peso account not found convert the amount into USD
	     and debit from the doller account. If txn currency USD and Doller account not found debit from the 
		Peso account Rel-09-57-  08-Sep-2009 Ajay ThoughtFocus*/
	 
   else if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, USD_COMP)     ) &&
        ( strEqual(Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY) ) && 
        (strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN)==0)
        )
   	{
   		if ( !TxAuthorize_Convert_Peso_to_USD(&nAuthTxTotalAmount,
                                            &nAuthTxRevAmount,
                                            &nAuthTxOrigAmount) )
         return SYSTEM_ERROR ;
   	}
   else if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, PESO_COMP) ) &&
        ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) && 
        (strcmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN)==0)
        )
   	{
   		if ( !TxAuthorize_Convert_USD_to_Peso(&nAuthTxTotalAmount,
                                            &nAuthTxRevAmount,
                                            &nAuthTxOrigAmount) )
         return SYSTEM_ERROR ;
   	}
   else
   {
      nAuthTxTotalAmount = (strtod (Auth_Tx.TLF01_details.total_amount,    0) / 100.0);
      nAuthTxRevAmount   = (strtod (Auth_Tx.TLF01_details.reversal_amount, 0) / 100.0);
      nAuthTxOrigAmount  = (strtod (Auth_Tx.TLF01_details.orig_amount,     0) / 100.0);
   }

   /* Visa 5 Oct mandt 2022, billing amt is used for card balance
    * if F28 and F54 is present*/
   if (true == txservice_Check_If_VISA_Transaction() &&
	   0 == strncmp(Auth_Tx.TLF01_details.handler_queue, "ncvis3", 6) &&
	   0 == strncmp(Auth_Tx.TLF01_details.message_type, "0100", 4) &&
	   0 != strlen(Auth_Tx.TLF01_details.tran_fee_amount) &&
	   ('1' == Auth_Tx.TLF01_details.product_codes[13].code[0]))
   {
	   nAuthTxTotalAmount = (strtod (Auth_Tx.TLF01_details.sales_amount,    0) / 100.0);
   }

   /* Is this an RSB transaction? */
   if ( Auth_Tx.BIN01_details.credit_subtype[0] == RSB_CARD )
   {
      /* Yes, calculate deferred gross amount based on transaction type. */
      if (txservice_If_Tx_Is_Treated_Like_A_Reversal ())
      {
         nTotalAmount = TxAuthorize_Calculate_RSB_Amounts(&nAuthTxRevAmount);
      }
      else if ( Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_SALE ||
                Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_REFUND)
      {
         nTotalAmount = TxAuthorize_Calculate_RSB_Amounts(&nAuthTxOrigAmount);
      }
      else if ( txservice_If_Tx_Is_Treated_Like_A_Adjustment () )
      {  
         nAuthTxOrigAmount  = TxAuthorize_Calculate_RSB_Amounts(&nAuthTxOrigAmount);
         nAuthTxTotalAmount = TxAuthorize_Calculate_RSB_Amounts(&nAuthTxTotalAmount);
         nTotalAmount       = nAuthTxOrigAmount - nAuthTxTotalAmount;
      }
      else if ( (strcmp ( Auth_Tx.TLF01_details.response_code, "00" ) != 0) &&
                (strcmp ( Auth_Tx.TLF01_details.message_type,"0130" ) == 0)  )
      {
         nTotalAmount = TxAuthorize_Calculate_RSB_Amounts(&nAuthTxOrigAmount);
      }
      else
      {
         nTotalAmount = TxAuthorize_Calculate_RSB_Amounts(&nAuthTxTotalAmount);
      }
   }
   else
   {
      /* Non-RSB Transactions */
      if (txservice_If_Tx_Is_Treated_Like_A_Reversal ())
      {
    	  if (true == txservice_Check_If_JCB_Transaction())
    	  {
    		  nTotalAmount = nAuthTxTotalAmount;
    	  }
    	  else
    	  {
         nTotalAmount = nAuthTxRevAmount;
    	  }
      }

      else if ( Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_SALE ||
                Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_REFUND)
         nTotalAmount = nAuthTxOrigAmount;

      else if ( txservice_If_Tx_Is_Treated_Like_A_Adjustment () )
         nTotalAmount = nAuthTxOrigAmount -  nAuthTxTotalAmount;

      else if ( (strcmp ( Auth_Tx.TLF01_details.response_code, "00" ) != 0) &&
                (strcmp ( Auth_Tx.TLF01_details.message_type, "0130" ) == 0 ))
         nTotalAmount = nAuthTxOrigAmount;

      else
         nTotalAmount = nAuthTxTotalAmount;
   }

   // convert amount to card holder currency, if they are different
   if (!strEqual (Auth_Tx.TLF01_details.conversion_rate, "1"))
   {
      nConversionRate = strtod (Auth_Tx.TLF01_details.conversion_rate, 0) ;
      nConvertedTxAmount = nTotalAmount * nConversionRate ;
   }
   else 
      nConvertedTxAmount = nTotalAmount ;
/*BDO ARPIL 2011 mandate to support account verfication tx- TF Phani */	
   if(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"51",2) == 0 && nConvertedTxAmount== 0.0)//Check if trnaction is Acc verification tx
	{
		//Log meesage for ACC verfication
		sprintf (strTemp, "Account Verfification Message with NULL amount") ;

		 txcntl_log_message( 0, 1, strTemp, "TxAuthorize_Authorize_Transaction", 1 );
	}
   /*BDO ARPIL 2011 MC mandate to support RPS tx with zero amount- TF Phani */	
   else if(nConvertedTxAmount== 0.0 && (0 == strcmp(Auth_Tx.industry_code,"800") ))
   	{
   	//Log meesage for RPS verfication
		sprintf (strTemp, "RPS tx with NULL amount") ;

		txcntl_log_message( 0, 1, strTemp, "TxAuthorize_Authorize_Transaction", 1 );
   	}
    else if ( !TxAuthorize_Validate_TransactionAmount ( nConvertedTxAmount ))
   {
      sprintf (strTemp, "Invalid amount %-9.2f in Tx  ", nConvertedTxAmount) ;
      ReturnCode = txservice_Generate_Usage_Error_Message( 2,
                                              strTemp,
                                             "Authorize_Transaction",
                                              GS_TXN_INVALID_TRANSACTION_AMOUNT,
                                              INVALID_AMOUNT) ;
      return ReturnCode ;
   }

   // Check if the transaction type is allowed for the terminal
   ReturnCode = TxAuthorize_Validate_TransactionType () ;
   if (ReturnCode != NO_AUTH_ERROR)
      return ReturnCode ;

   //praneeth added to store converted amount while dual currency
   if(strEqual(Auth_Tx.TLF01_details.host_start_time, USD_COMP) || 
   	  strEqual(Auth_Tx.TLF01_details.host_start_time, PESO_COMP))
   {
   	   strcpy(Auth_Tx.TLF01_details.product_codes[9].amount,doubletostr(nConvertedTxAmount));
   }

   if (txservice_If_Tx_Qualifies_For_Authorization_Checks ())
   {
      ReturnCode = TxAuthorize_Perform_Authorization (nConvertedTxAmount) ;
      if (ReturnCode != NO_AUTH_ERROR)
         return ReturnCode ;
   }
   return NO_AUTH_ERROR ;
} /* BOOLEAN TxAuthorize_Authorize_Transaction () */

BYTE TxAuthorize_Validate_TransactionAmount ( double nConvertedTxAmount )
{ 

	int MC_DE48_len = 0;

   if ((nConvertedTxAmount == 0) && ( Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_SALE	||
                                      Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_REFUND ||
                                      Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE ||
									  Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION ||
									  Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY))
      return true;
   else if ((nConvertedTxAmount <= 0) && ( Auth_Tx.TLF01_details.tx_key == AUTH_SALE_ADJUSTMENT	||
                                      Auth_Tx.TLF01_details.tx_key == AUTH_REFUND_ADJUSTMENT ||
									  Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION) )
      return true;
   else 	if (nConvertedTxAmount <= 0 || +HUGE_VAL == nConvertedTxAmount) 
   	{

		/*Master Card*/
	   if(true == txservice_Check_If_MC_Transaction())
		{
			if((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100", 4)))
			{
	
		   	    /*Check if its Recurring Payment Test transaction for MasterCard
		   	       Message Type : Authorization Request/0100
			       DE 61 (Point-of-Service [POS] Data), subfield 4 (POS Cardholder Presence) = 4 (Standing order/recurring transactions)
			       DE 61, subfield 7 (POS Transaction Status) = 4 (Preauthorized Request) or 8 (Account Status Inquiry Service)
			      */
			 	if( (Auth_Tx.acceptor_term_name[3] == '4') && 
					(Auth_Tx.acceptor_term_name[6] == '4'|| Auth_Tx.acceptor_term_name[6] == '8'))
				{
					return true;
				}
				
			    /*Check if it Account Status Inquiry Service Transactions for MasterCard
					Message Type : 0100 Authorization message
					Processing Code = 00
			       	DE 61 (Point-of-Service [POS] Data) subfield 7 (POS Transaction Status) = 8 (Account Status Inquiry Service)
			      */
			    else if( (0 == strncmp (Auth_Tx.TLF01_details.processing_code, "00",2))&&
			  		   ( Auth_Tx.acceptor_term_name[6] == '8'))
				{
					strncpy(Auth_Tx.TLF01_details.visa_merchant_id,"01",2);
					return true;
				}

			 	//For Zero Amount in Preauthorization Request, Lets check processing code , it should be 00XXXX
			    else if(( 0 == strncmp(Auth_Tx.TLF01_details.processing_code,"00",2)) &&
			    		(Auth_Tx.acceptor_term_name[3] != '4') &&
						(Auth_Tx.acceptor_term_name[6] == '4'))
				{
					 MC_DE48_len = strlen(Auth_Tx.EMV_details.issuer_script_2);
					 if(true == txservice_Check_If_MC_DE_48_Tag_63_Trace_ID_Present(MC_DE48_len,
								   Auth_Tx.EMV_details.issuer_script_2))
					   {
						   return true;
					   }
					   else
					   {
						   return false;
					   }

				}
				else
				{
					return false;
				}
			}

			else
			{
					return false;
			}
		}

		/*Visa*/
		else if(true == txservice_Check_If_VISA_Transaction())
		{
			if((0 == strncmp(Auth_Tx.TLF01_details.message_type,"0100", 4)))
			{

				/*
				Verification-Only Requests
				Message type = 0100
			       Field 25 = 51
				Field 4 = zeroes
				*/
				if ( 0 == strncmp(Auth_Tx.TLF01_details.pos_condition_code, "51",2))
				{
					strncpy(Auth_Tx.TLF01_details.product_codes[0].quantity,"01",2);
					return true;
				}

				/*
				0120 File Update Advices
				Message Type : 0100

				*/
				/*Need to know how to find 0120 in 0100 message
				else if()
				{

				}*/

				/*
				Prepaid Transactions 
				Messages  type =0100
				Field 3 Positions 1-2 = 72
				Field 54 Positions 1-2 = 72
				*/
				else if( (0 == strncmp(Auth_Tx.TLF01_details.processing_code,"72",2))&& 
						 (0 == strncmp (Auth_Tx.accptor_bus_code, "72", 2)))
				{
					return true;
				}
				else
				{
					return false;
				}

			}
			else
			{
				return false;
			}

		}
	  else
	  {
      	return false;
	  }
   	}
   
   return true;
}

BYTE TxAuthorize_Perform_Authorization (double nConvertedTxAmount) 
{
   BYTE  TransactionOrigin = 0 ;
   CHAR  strTemp[256] = {0} ;
   BYTE  ReturnCode = NO_AUTH_ERROR ;
   BYTE  UpdateCCF03PStruct = 0, UpdateCashBonusStruct = 0, UpdateACF01Struct = 0 ;

   if (txservice_If_Tx_Qualifies_For_Expiry_Date_Checking ())
   {
      ReturnCode = TxAuthorize_Check_Expiry_Date () ;
      if (ReturnCode != NO_AUTH_ERROR)
         return ReturnCode ;
   }

   if (txservice_If_Tx_Qualifies_For_Card_Velocity_Checking ())
   {
      ReturnCode = TxAuthorize_Perform_Card_Velocity_Checks(nConvertedTxAmount, sCR01);
      if (ReturnCode != NO_AUTH_ERROR)
      {
         if (!txservice_If_Tx_Cannot_Be_Declined ())
            return ReturnCode ;
      }
      else
      {
         UpdateCCF03PStruct = 1 ;
      }
   }

   if (txservice_If_Tx_Qualifies_For_Fleet_Card_Checks ())
   {
      ReturnCode = TxFleet_Validate_Fleet_Cards () ;
      if (ReturnCode != NO_AUTH_ERROR)
         return ReturnCode ;
   }

   if (txservice_If_Tx_Qualifies_For_Balance_Account_Checks ())
   {
      ReturnCode = TxAuthorize_Perform_Balance_Account_Checks (nConvertedTxAmount) ;
      if (ReturnCode != NO_AUTH_ERROR)
         return ReturnCode ;
      UpdateACF01Struct = 1 ;
   }

   if (txservice_If_Tx_Qualifies_For_Cash_Bonus_Processing ())
   {
      /* The actual processing is done in updatds.  This is so the Bonus
       * record can be fetched and updated immediately so there are not
       * multiple winners.  This also ensures the award is only given on
       * approvals.
       */
      UpdateCashBonusStruct = 1 ;
   }

   ReturnCode = TxAuthorize_Update_AuthTx_Structures (UpdateCCF03PStruct,
                                                      UpdateCashBonusStruct,
                                                      UpdateACF01Struct) ;
   if (ReturnCode != NO_AUTH_ERROR)
      return ReturnCode ;
   return NO_AUTH_ERROR ;
}
BYTE TxDataServer_Continue_With_Authorize_After_Override(double nConvertedTxAmount)
{
	BYTE  TransactionOrigin = 0 ;
	CHAR  strTemp[256] = {0} ;
	BYTE  ReturnCode = NO_AUTH_ERROR,uag_rcode ;
	BYTE  UpdateCCF03PStruct = 1, UpdateCashBonusStruct = 0, UpdateACF01Struct = 0 ;
	

		if (txservice_If_Tx_Qualifies_For_Balance_Account_Checks ())
		{
		   ReturnCode = TxAuthorize_Perform_Balance_Account_Checks (nConvertedTxAmount) ;
		   if (ReturnCode != NO_AUTH_ERROR)
			  return ReturnCode ;
		   UpdateACF01Struct = 1 ;
		}

		if (txservice_If_Tx_Qualifies_For_Cash_Bonus_Processing ())
		{
		   /* The actual processing is done in updatds.  This is so the Bonus
			* record can be fetched and updated immediately so there are not
			* multiple winners.  This also ensures the award is only given on
			* approvals.
			*/
		   UpdateCashBonusStruct = 1 ;
		}

		ReturnCode = TxAuthorize_Update_AuthTx_Structures (UpdateCCF03PStruct,
														   UpdateCashBonusStruct,
														   UpdateACF01Struct);
			if (ReturnCode == NO_AUTH_ERROR)
			{
			   /* All checks satisfied at this point */
			   txservice_Prepare_AuthTx_For_Forwarding( GS_TXN_COMPLETED,
														TRANSACTION_APPROVED );
			
			   /* Retrieve the current alphanumeric authorization_id from UAG01 */
			   if ( strlen (Auth_Tx.TLF01_details.auth_number) > 0
					&&
				   (
					  txservice_If_Tx_Is_Treated_Like_A_Voice_Auth_Override () ||
					  txservice_If_Tx_Is_Treated_Like_A_Offline_Sale () 	   ||
					  txservice_If_Tx_Is_Treated_Like_A_Adjustment ()		   ||
					  strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX)
					)
				  )
			   {
				  /* All done, so send response back to originator.
				   * Do not calculate a auth_id if the auth_id exists AND
				   * (the transaction is a voice overridden transaction OR
				   * the transaction is an offline sale OR card is Amex).
				   */
				  txservice_Forward_Auth_Tx_To_Originator(Auth_Tx.TLF01_details.handler_queue,
														  MT_AUTH_RESPONSE, 0, 0);
			
				  /* There is a path through CCF03P that may try to resend to the
				   * originator.  Set tran_id to '1' as a flag so it knows not to
				   * resend the response.
				   */
				  strcpy( Auth_Tx.TLF01_details.primary_key.transaction_id, "1" );
			   }
			   else
			   {
				  uag_rcode = TxAuthorize_GetAuthId( ST2_NONE );
				  if ( PTEMSG_OK != uag_rcode )
				  {
					 /* XIPC Error */
					 ReturnCode = SYSTEM_ERROR;
				  }
				  else
				  {
					 TxDataServer_DBReply_Continue();
			
					 /* There is a path through CCF03P that may try to send to the
					  * originator again.  Set tran_id to '1' as a flag so it knows
					  * not to send it.  It was sent by the above statement.
					  */
					 strcpy( Auth_Tx.TLF01_details.primary_key.transaction_id, "1" );
					 ReturnCode = NO_AUTH_ERROR;
				  }
			   }
			}
			return ReturnCode ;
		return NO_AUTH_ERROR ;

}
/***********************************************************************
NAME:			TxAuthorize_Validate_TransactionType
DESCRIPTION:	Determines if a transaction is allowed for the cardholder	
INPUTS:			None				
OUTPUTS:		None
RETURNS:        false  if the transaction is not allowed
                true   if the transaction is allowed
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
************************************************************************/
BYTE TxAuthorize_Validate_TransactionType (void) 
{	
	BYTE ReturnCode = 0 ;
    char strTemp[256] = {0} ;
	switch (Auth_Tx.TLF01_details.tx_key)
	{
		
		case AUTH_BALANCE_INQUIRY:
			if (Auth_Tx.CCF02P_details.allowed[CREDIT_INQUIRY] != '1')
			{
				if( true == txservice_Check_If_CUP_Transaction() &&
					Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)
				{
					return NO_AUTH_ERROR ;
				}
				else
				{
                strcpy (strTemp, "Credit Inquiry Tx not allowed for card") ;
                
				ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																	"TxAuthorize_Validate_TransactionType",
																	GS_TXN_TRANSACTION_NOT_ALLOWED,
																	INVALID_TRANSACTION) ;
            txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_TRANSACTION_NOT_ALLOWED,
																REFER_TO_CARD_ISSUER) ;

				return ReturnCode ;
			}
			}
		break ;
		case AUTH_ADVICE:
		case AUTH_AUTHORIZATION:
        case AUTH_CARD_VERIFICATION:
		case AUTH_QUASI_CASH:
		case AUTH_PRE_AUTHORIZATION:
		case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:
		case AUTH_SALE:
		case AUTH_OFFLINE_SALE:
					
			if (Auth_Tx.CCF02P_details.allowed[CREDIT_PURCHASE] != '1')
			{
                strcpy (strTemp, "Credit Purchase Tx not allowed for card") ;
                
				ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																	"TxAuthorize_Validate_TransactionType",
																	GS_TXN_TRANSACTION_NOT_ALLOWED,
																	INVALID_TRANSACTION) ;
            txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_TRANSACTION_NOT_ALLOWED,
																REFER_TO_CARD_ISSUER) ;
				
				return ReturnCode ;
				
			}
		break ;
		
		case AUTH_CASH_ADVANCE:
		case AUTH_CASH_ADVANCE_AUTHORIZATION:
			strcpy (strTemp, "Credit CashAdv Tx not allowed for card") ;
            
			if (Auth_Tx.CCF02P_details.allowed[CREDIT_CASH_ADVANCE] != '1')
			{
				ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																	"TxAuthorize_Validate_TransactionType",
																	GS_TXN_TRANSACTION_NOT_ALLOWED,
																	INVALID_TRANSACTION) ;
                txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_TRANSACTION_NOT_ALLOWED,
																REFER_TO_CARD_ISSUER) ;
				
				return ReturnCode ;

			}
		break ;

		case AUTH_INVALID:
		    strcpy (strTemp, "Inval Tx in TxAuthorize_Validate_TxType") ;
            
			ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
																"TxAuthorize_Validate_TransactionType",
																GS_TXN_TRANSACTION_NOT_ALLOWED,
																INVALID_TRANSACTION) ;	
            txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_TRANSACTION_NOT_ALLOWED,
												  REFER_TO_CARD_ISSUER) ;
				
			return ReturnCode ;

		break ;
	} // switch
	return NO_AUTH_ERROR ;
}  /* TxAuthorize_Validate_TransactionType */

/*************************************************************************************
NAME:			TxAuthorize_Perform_Merchant_Max_Credit_Amt
DESCRIPTION:	Determines if the merchant is allowed to handle the purchase amount 	
INPUTS:			None				
OUTPUTS:		None
RETURNS:        false  if not allowed
                true   if allowed
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BYTE TxAuthorize_Perform_Merchant_Max_Credit_Amt (double nTotalAmount) 
{
	double nCreditPurchMax = 0.0 ;
	char strTemp[256] = {0} ;
	BYTE ReturnCode = 0 ;
	// Verify that the tx amount does not exceed the max allowed for the merchant
	if (Auth_Tx.MCF01_details.credit_purch_max[0] != 0)
	{
		nCreditPurchMax = (strtod (Auth_Tx.MCF01_details.credit_purch_max, 0) / 100.0) ;

		if (nTotalAmount > nCreditPurchMax)
		{
			//strcpy (strTemp,"Max once Tx dollar limit exceed for Mid") ;
			strcpy(strTemp, pos_res_txt);
            
			ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																"Perform_Merchant_Max_Credit_Amt",
																GS_TXN_EXCEEDED_MAX_TRANSACTION_LIMIT_FOR_MERCHANT,
																pos_rc) ;
			return ReturnCode ;
		}
	}
	return NO_AUTH_ERROR ;
} /* TxAuthorize_Perform_Merchant_Max_Credit_Amt */

BYTE TxAuthorize_Perform_Merchant_Max_Auth_Amt (double nTotalAmount)
{
	double nCreditAuthMax = 0.0 ;
	char strTemp[256] = {0} ;
	BYTE ReturnCode = 0 ;
	// Verify that the tx amount does not exceed the max allowed for the merchant
	if (Auth_Tx.MCF01_details.credit_auth_max[0] != 0)
	{
		nCreditAuthMax = (strtod (Auth_Tx.MCF01_details.credit_auth_max, 0) / 100.0) ;

		if (nTotalAmount > nCreditAuthMax)
		{
			if (Auth_Tx.TLF01_details.card_num[0] == '7' &&
							Auth_Tx.TLF01_details.acquirer_id[0] == '\0')
			{
				 strncpy(Auth_Tx.TLF01_details.acquirer_id, "nciftr",6);
			}
			//strcpy (strTemp,"Max once Tx dollar limit exceed for Mid") ;
			strcpy (strTemp, pos_res_txt) ;

			ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																"Perform_Merchant_Max_Auth_Amt",
																GS_TXN_EXCEEDED_MAX_TRANSACTION_LIMIT_FOR_MERCHANT,
																pos_rc) ;
			return ReturnCode ;
		}
	}
	return NO_AUTH_ERROR ;
} /*

/******************************************************************************

NAME:			TxAuthorize_Perform_Merchant_Max_Return_Amt
DESCRIPTION:	Determines if the merchant is allowed to handle the return amount 	
INPUTS:			None				
OUTPUTS:		None
RETURNS:        false  if not allowed
                true   if allowed
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*********************************************************************************/
BYTE TxAuthorize_Perform_Merchant_Max_Return_Amt (double nTotalAmount) 
{
	double nReturnMax = 0.0 ;
	char strTemp[256] = {0} ;
	BYTE ReturnCode = 0 ;

   	// Verify that the tx amount does not exceed the max allowed for the merchant
	if (Auth_Tx.MCF01_details.credit_return_max[0] != 0)
	{
		nReturnMax = (strtod (Auth_Tx.MCF01_details.credit_return_max, 0) / 100.0) ;

		if (nTotalAmount > nReturnMax)
		{
			strcpy (strTemp,"Max once return dollar limit exceed Mid") ;
            
			ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																"Perform_Merchant_Max_Credit_Amt",
																GS_TXN_EXCEEDED_MAX_REFUND_LIMIT_FOR_MERCHANT,
																RCI_SPECIAL_CONDITION) ;
			return ReturnCode ;
		}
	}
	return NO_AUTH_ERROR ;
} /* TxAuthorize_Perform_Merchant_Max_Credit_Amt */



/******************************************************************************

NAME:			TxAuthorize_Perform_Merchant_Max_CashAdvance_Amt
DESCRIPTION:	Determines if the merchant is allowed to handle the cash advance amount 	
INPUTS:			None				
OUTPUTS:		None
RETURNS:        false  if not allowed
                true   if allowed
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*********************************************************************************/
BYTE TxAuthorize_Perform_Merchant_Max_CashAdvance_Amt (double nTotalAmount) 
{
	double nReturnMax = 0.0 ;
	char strTemp[256] = {0} ;
	BYTE ReturnCode = 0 ;

   

	// Verify that the tx amount does not exceed the max allowed for the merchant
	if (Auth_Tx.MCF01_details.credit_cash_adv_max[0] != 0)
	{
		nReturnMax = (strtod (Auth_Tx.MCF01_details.credit_cash_adv_max, 0) / 100.0) ;

		if (nTotalAmount > nReturnMax)
		{
			strcpy (strTemp,"Max once cash adv limit exceed for Mid") ;
            
			ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																"Perform_Merchant_Max_CashAdvance_Amt",
																GS_TXN_EXCEEDED_MAX_REFUND_LIMIT_FOR_MERCHANT,
																EXCEEDS_AMOUNT_LIMIT) ;
			return ReturnCode ;
		}
	}
	return NO_AUTH_ERROR ;
} /* TxAuthorize_Perform_Merchant_Max_Credit_Amt */

BYTE TxAuthorize_Perform_Cash_Bonus_Processing (double nConvertedTxAmount) 
{
	BOOLEAN bReturnCode = false ;
	double  nCashBonusMinimumAmount = 0.0 ;
	
	LONG	nCashBonusStartDateInJulian = 0L ; 
	LONG	nCashBonusEndDateInJulian = 0L ;
	LONG  nTranDateInJulian = 0L ;
	
	LONG	nCashBonusTransactionThreshold = 0L ;
	
	char strSystemDateTime[50] = {0} ;
	char strTemp[256] = {0} ;

	// Reset global variable
	nCashBonusTransactionCounter = 0L ;

	nCashBonusMinimumAmount = (strtod (Auth_Tx.BON01_details.minimum_amount, 0)) / 100.0 ;
	nCashBonusStartDateInJulian = atol (Auth_Tx.BON01_details.program_start_date) ;
	nCashBonusEndDateInJulian = atol (Auth_Tx.BON01_details.program_end_date) ;
	// nCashBonusTransactionCount is a global variable
	nCashBonusTransactionThreshold = atol (Auth_Tx.BON01_details.transaction_threshold) ;
	nCashBonusTransactionCounter = atol (Auth_Tx.BON01_details.transaction_counter) ;

	genutil_format_date (strSystemDateTime) ;
	memcpy (strTemp, strSystemDateTime, 8) ; // date in yyyymmdd format
	nTranDateInJulian = Txutils_Calculate_Julian_Date (strTemp) ;
	
	if (nConvertedTxAmount >= nCashBonusMinimumAmount)

	{
		if (nTranDateInJulian >= nCashBonusStartDateInJulian &&
			 nTranDateInJulian <= nCashBonusEndDateInJulian)
		{
			if ((nCashBonusTransactionCounter + 1) == nCashBonusTransactionThreshold)
			{
				strcpy (Auth_Tx.TLF01_details.cash_bonus, "Y") ;
                strcpy (Auth_Tx.TLF01_details.response_text, Auth_Tx.BON01_details.response_message) ;
				nCashBonusTransactionCounter = 0 ;
			}
			else if ((nCashBonusTransactionCounter + 1) < nCashBonusTransactionThreshold)
            {
                /* Cap nCashBonusTransactionCounter to 9999999 */
                if ((nCashBonusTransactionCounter + 1) > 9999999)
                        nCashBonusTransactionCounter = 0 ;
				nCashBonusTransactionCounter++ ;
            }
		}
	}
	return NO_AUTH_ERROR ;
} /* TxAuthorize_Perform_Cash_Bonus_Processing */

/**************************************************************************
NAME:			TxAuthorize_Perform_Card_Velocity_Checks
DESCRIPTION:	Performs all card velocity checks for the cardholder for 
					all transaction types	
INPUTS:			None				
OUTPUTS:		None
RETURNS:        false  if checks are violated
                true   if allowed
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:
************************************************************************/
BYTE TxAuthorize_Perform_Card_Velocity_Checks (double nConvertedTxAmount,CR01 sCR01)
{
	char		strSystemDateTime[50] = {0} ;
	BYTE		strCurrentDate[9] = {0} ;
	BYTE		strTemp[256] = {0} ;
	BYTE		TransactionOrigin = 0 ;
	BYTE		ReturnCode = 0 ;

	// Note that all dates referenced in this function are in Julian

	LONG	CurrentDate = 0L ;
	LONG	DateWhenAPesoTransactionLastOccurred = 0L ;
	LONG	DateWhenADollarTransactionLastOccurred = 0L ;
	LONG	DateWhenATransactionLastOccurred = 0L ;

	int	DaysElapsedSinceATransactionLastOccurred = 0 ;
	int	DaysElapsedSinceAPesoTransactionLastOccurred ;
	int	DaysElapsedSinceADollarTransactionLastOccurred = 0 ;
	
	int	CurrencyType = 0 ;
	int	nIndex = 0 ;
	int	nIndex1 = 0 ;
	
	int CR01_Txn_Counter_Value;
	double CR01_Amt_Counter_Value=0.0;
	double CR01_Amt_Counter_Incremented=0.0;
	int CR01_Txn_Acc_limit_Value;
	double CR01_Amt_Acc_Limit_Value=0.0;
	BYTE  strAmount[50] = {0} ;
	BYTE  strAmount_limit[50] = {0} ;
	CARD_VELOCITY_FROM_PROFILE card_velocity[3] ;


	memset (&card_velocity, 0, sizeof card_velocity) ;
	memset (strTemp, 0, sizeof (strTemp)) ;

	genutil_format_date ((char*)strSystemDateTime) ;
	memcpy (strCurrentDate, strSystemDateTime, 8) ; 
	// date in yyyymmdd format

	// Current Date is in julian
	CurrentDate = Txutils_Calculate_Julian_Date (strCurrentDate) ;
	
	if (strEqual (Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY))
		CurrencyType = PESO ;
	else if (strEqual (Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY))
		CurrencyType = DOLLAR ;

	// Retrieve the peso and dollar day period & velocity limits (nbr_usage and 
	// dollar_usage) from Auth_Tx.CAF01_details for both the peso and dollar for the 3 periods
	ReturnCode = TxAuthorize_Get_Velocity_Limits_From_Profile (card_velocity) ;

	// peso_julian_date is the date when a peso tx last occurred
	DateWhenAPesoTransactionLastOccurred = atol (Auth_Tx.CCF03P_details.peso_julian_date) ;
	// dollar_julian_date is the date when a dollar tx last occurred
	DateWhenADollarTransactionLastOccurred = atol (Auth_Tx.CCF03P_details.dollar_julian_date) ;

	/* The following line of code is written to overcome the problem of the julian date 
	   being set to zero by the card import process
	*/
	   
	if (DateWhenAPesoTransactionLastOccurred == 0L ||
		DateWhenADollarTransactionLastOccurred == 0L)

		return NO_AUTH_ERROR ;

	// Get diff between the current date and when a peso tx was last occurred (could be 0)
	DaysElapsedSinceAPesoTransactionLastOccurred = CurrentDate - DateWhenAPesoTransactionLastOccurred ;
	// Get diff between the current date and when a dollar tx was last occurred (could be 0)
	DaysElapsedSinceADollarTransactionLastOccurred = CurrentDate-DateWhenADollarTransactionLastOccurred ;

	// Regardless of peso or dollar, determine when the latest transaction date for the cardholder
	if (DateWhenAPesoTransactionLastOccurred > DateWhenADollarTransactionLastOccurred)
		DateWhenATransactionLastOccurred = DateWhenAPesoTransactionLastOccurred ;
	else 
		DateWhenATransactionLastOccurred = DateWhenADollarTransactionLastOccurred ;

	// Regardless of peso or dollar, get diff between the current date and when 
	// a transaction was last occurred for the cardholder (could be 0)
	DaysElapsedSinceATransactionLastOccurred = CurrentDate - DateWhenATransactionLastOccurred ;

	if (CurrencyType == PESO)
	{
		// This is done to populate the private structure because this will be used
		// to update the Auth_Tx.CCF03P_details
		private_ccf03p_struct.peso_julian_date  = CurrentDate ;
		private_ccf03p_struct.dollar_julian_date = DateWhenADollarTransactionLastOccurred ;
	}
	else if (CurrencyType == DOLLAR)
	{
		// This is done to populate the private structure because this will be used
		// to update the Auth_Tx.CCF03P_details
		private_ccf03p_struct.dollar_julian_date = CurrentDate ;
		private_ccf03p_struct.peso_julian_date  = DateWhenAPesoTransactionLastOccurred ;
	}

	if (DaysElapsedSinceAPesoTransactionLastOccurred >= 14)
		// Clear the peso bucket because values are irrelevant
		TxAuthorize_Clear_Usage_Bucket (PESO) ;
	else  // Populate the private structure 14 day peso bucket with values from 
			// Auth_Tx.CCF03P_details.peso_usage[]
		TxAuthorize_Populate_Usage_Bucket (PESO) ;
	
	if (DaysElapsedSinceADollarTransactionLastOccurred >= 14)
		// Clear the dollar bucket because values are irrelevant
		TxAuthorize_Clear_Usage_Bucket (DOLLAR) ;
	else // Populate the private structure 14 day dollar bucket with values from 
			//Auth_Tx.CCF03P_details.dollar_usage[]
		TxAuthorize_Populate_Usage_Bucket (DOLLAR) ;

	if (DaysElapsedSinceATransactionLastOccurred != 0)
		// The merchant usage bucket has to be cleared on a new transaction day
		TxAxthorize_Clear_Merchant_Usage_Bucket () ;
	else
		// Populate the private structure with values from Auth_Tx.CCF03P_details.repeat[]
		TxAuthorize_Populate_Merchant_Usage_Bucket () ;

	if (DaysElapsedSinceATransactionLastOccurred != 0)
		// The merchant usage bucket has to be cleared on a new transaction day
		TxAxthorize_Clear_Category_Usage_Bucket () ;
	else
		// Populate the private structure with values from Auth_Tx.CCF03P_details.mcc[]
		TxAuthorize_Populate_Category_Usage_Bucket () ;

	// Populate the private structure failed pin attempts with 
	// Auth_Tx.CCF03P_details.failed_pin_attempts
	TxAuthorize_Populate_Failed_Pin_Attempts () ;

	/* On a reversal/void the transaction number and amount has to be
    * deducted from the zeroth element of the usage bucket and then
    * we skip the rest of the authorization process.
    */
   if (txservice_If_Tx_Is_Treated_Like_A_Reversal()           || 
       Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE         ||          
       Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE ||          
       txservice_If_Tx_Is_Treated_Like_A_Offline_Void() )
	{
		ReturnCode = TxAuthorize_Deduct_Amount_From_Usage_Bucket (CurrencyType,nConvertedTxAmount) ;
		return ReturnCode ;
	}

	if (CurrencyType == PESO)
	{	// Note that if transactions occur on the same day,no shifting downwards happen
		/* No need check it again as its done already in TxAuthorize_Perform_Offus_Card_Velocity_Checks TF- Abhishek
		// For ONUS and OFFUS we do this check in function TxAuthorize_Perform_Offus_Card_Velocity_Checks TF- Abhishek
		ReturnCode = TxAuthorize_Shift_Peso_Usage_Bucket_Down_And_Verify_Limits 
															(card_velocity,
															 DaysElapsedSinceAPesoTransactionLastOccurred,
															 nConvertedTxAmount) ;
		if (ReturnCode != NO_AUTH_ERROR)
			return ReturnCode ;*/
		
		// Since there were no violations, the counts are incremented
		private_ccf03p_struct.peso_usage[0].nbr_usage += 1 ;

        /* Cap private_ccf03p_struct.peso_usage[0].nbr_usage to 999 */
        if (private_ccf03p_struct.peso_usage[0].nbr_usage > 999)
            private_ccf03p_struct.peso_usage[0].nbr_usage = 999 ;

        private_ccf03p_struct.peso_usage[0].amt_usage += nConvertedTxAmount ;

        if (private_ccf03p_struct.peso_usage[0].amt_usage > 9999999999.99) 
            private_ccf03p_struct.peso_usage[0].amt_usage = 9999999999.99 ;

        if(sCR01.acc_amtlimit[0]!='\0')
                {
                	if(TxAuthorize_Check_TxnType_is_ChipFallBack()==true)
                	{

                		if(0!=strncmp(sCR01.acc_amtlimit,"000000000000",12) || sCR01.acc_transactionlimit!="0")
                		{
                			//transaction counter increment
                			CR01_Txn_Counter_Value = atoi(sCR01.acc_transactionlimit_counter);
                			CR01_Txn_Acc_limit_Value = atoi(sCR01.acc_transactionlimit);
                			CR01_Txn_Counter_Value += 1;

                			memset(strTemp, 0, sizeof(strTemp));
                			itoa(CR01_Txn_Counter_Value, strTemp, 10);
                			strcpy(sCR01.acc_transactionlimit_counter, strTemp);

                			// Accumulated amount counter increment
                			 CR01_Amt_Counter_Value = (strtod(sCR01.acc_amtlimit_counter, 0)/ 100.0);
                			 CR01_Amt_Acc_Limit_Value = (strtod(sCR01.acc_amtlimit, 0) / 100.0);

                			 CR01_Amt_Counter_Incremented =CR01_Amt_Counter_Value+ nConvertedTxAmount;

                			 //The accumulated amount field should always be greater than the counter value.
                			if (CR01_Amt_Counter_Incremented > CR01_Amt_Acc_Limit_Value)
                			{
                					sprintf(strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id);
                					ReturnCode = txservice_Generate_Usage_Error_Message(
                												2, strTemp, "TxAuthorize_Perform_Card_Velocity_Checks",
                												GS_TXN_TRANSACTION_NOT_ALLOWED,
                												sCR01.response_code);
                					return ReturnCode;
                			}
                			//The accumulated txn field should always be greater than the counter value.
                			if (CR01_Txn_Counter_Value > CR01_Txn_Acc_limit_Value)
                			{
                					sprintf(strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id);
                					ReturnCode = txservice_Generate_Usage_Error_Message(
                												2, strTemp, "TxAuthorize_Perform_Card_Velocity_Checks",
                												GS_TXN_TRANSACTION_NOT_ALLOWED,
                												sCR01.response_code);
                										return ReturnCode;
                			}

                			//convert the amount counter value into 12 characters.
                			Convert_Amt_To_Auth_Tx_String_Format(CR01_Amt_Counter_Incremented,strAmount);
                    	    LeftFill_String_With_Character (sizeof sCR01.acc_amtlimit_counter-1, strAmount,'0') ;
                    		strcpy (sCR01.acc_amtlimit_counter, strAmount) ;

                    		//Insert the updated values into the db
        					ReturnCode=TxDataServer_Update_CR01_Details(sCR01);
        					memset(&sCR01,0x00,sizeof(sCR01));
                		}
                		else
                		{
                			sprintf (strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id) ;
                			ReturnCode = txservice_Generate_Usage_Error_Message( 2,
        		                                                      strTemp,
        		                                                     "TxAuthorize_Perform_Card_Velocity_Checks",
        		                                                      GS_TXN_TRANSACTION_NOT_ALLOWED,
        															  sCR01.response_code) ;
                			return ReturnCode;

                		}
                	}
                }
	}
	else if (CurrencyType == DOLLAR)
	{
		// Note that if transactions occur on the same day,no shifting downwards happen
		/* No need check it again as its done already in TxAuthorize_Perform_Offus_Card_Velocity_Checks TF- Abhishek
		// For ONUS and OFFUS we do this check in function TxAuthorize_Perform_Offus_Card_Velocity_Checks TF- Abhishek
		ReturnCode = TxAuthorize_Shift_Dollar_Usage_Bucket_Down_And_Verify_Limits 
															(card_velocity,
															 DaysElapsedSinceADollarTransactionLastOccurred,
															 nConvertedTxAmount) ;
		if (ReturnCode != NO_AUTH_ERROR)
			return ReturnCode ;*/

		// Since there were no violations, the counts are incremented
		private_ccf03p_struct.dollar_usage[0].nbr_usage += 1 ;

        /* Cap private_ccf03p_struct.dollar_usage[0].nbr_usage to 999 */
        if (private_ccf03p_struct.dollar_usage[0].nbr_usage > 999)
            private_ccf03p_struct.dollar_usage[0].nbr_usage = 999 ;

		private_ccf03p_struct.dollar_usage[0].amt_usage += nConvertedTxAmount ;

        if (private_ccf03p_struct.dollar_usage[0].amt_usage > 9999999999.99) 
            private_ccf03p_struct.dollar_usage[0].amt_usage = 9999999999.99 ;

        if(sCR01.acc_amtlimit[0]!='\0')
        {
             if(TxAuthorize_Check_TxnType_is_ChipFallBack()==true)
             {

            	 if(0!=strncmp(sCR01.acc_amtlimit,"000000000000",12) || sCR01.acc_transactionlimit!="0")
                	{
            		 	 //transaction counter increment
            		    CR01_Txn_Counter_Value = atoi(sCR01.acc_transactionlimit_counter);
            		    CR01_Txn_Acc_limit_Value = atoi(sCR01.acc_transactionlimit);
            		    CR01_Txn_Counter_Value += 1;

            		    memset(strTemp, 0, sizeof(strTemp));
            		    itoa(CR01_Txn_Counter_Value, strTemp, 10);
            		    strcpy(sCR01.acc_transactionlimit_counter, strTemp);

            		    // Accumulated amount counter increment
            		     CR01_Amt_Counter_Value = (strtod(sCR01.acc_amtlimit_counter, 0)/ 100.0);
            		     CR01_Amt_Acc_Limit_Value = (strtod(sCR01.acc_amtlimit, 0) / 100.0);

            		     CR01_Amt_Counter_Incremented =CR01_Amt_Counter_Value+ nConvertedTxAmount;

            		      //The accumulated amount field should always be greater than the counter value.
            		       if (CR01_Amt_Counter_Incremented > CR01_Amt_Acc_Limit_Value)
            		       {
            		               sprintf(strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id);
            		                ReturnCode = txservice_Generate_Usage_Error_Message(
            		                 								2, strTemp, "TxAuthorize_Perform_Card_Velocity_Checks",
            		                 							    GS_TXN_TRANSACTION_NOT_ALLOWED,
            		                 							   sCR01.response_code);
            		                 return ReturnCode;
            		        }
            		        //The accumulated txn field should always be greater than the counter value.
            		       if (CR01_Txn_Counter_Value > CR01_Txn_Acc_limit_Value)
            		        {
            		                 sprintf(strTemp, "ChipFallback rule decline %s \n",sCR01.rule_id);
            		                 ReturnCode = txservice_Generate_Usage_Error_Message(
            		                 						2, strTemp, "TxAuthorize_Perform_Card_Velocity_Checks",
            		                 						GS_TXN_TRANSACTION_NOT_ALLOWED,
            		                 						sCR01.response_code);
            		                 return ReturnCode;
            		       }

            		        //convert the amount counter value into 12 characters.
            		        Convert_Amt_To_Auth_Tx_String_Format(CR01_Amt_Counter_Incremented,strAmount);
            		        LeftFill_String_With_Character (sizeof sCR01.acc_amtlimit_counter-1, strAmount,'0') ;
            		        strcpy (sCR01.acc_amtlimit_counter, strAmount) ;

            		          //Insert the updated values into the db
            		         ReturnCode=TxDataServer_Update_CR01_Details(sCR01);
            		         memset(&sCR01,0x00,sizeof(sCR01));
					}
                	else
                	{
                			sprintf(strTemp, "ChipFallback rule decline %s \n",
								sCR01.rule_id);
                			ReturnCode = txservice_Generate_Usage_Error_Message(2,
                					strTemp, "TxAuthorize_Perform_Card_Velocity_Checks",
									GS_TXN_TRANSACTION_NOT_ALLOWED,
									sCR01.response_code);
                			return ReturnCode;

                	}


            }
        }
															
	}




	// *********************************************************************
	// Check if the card has been used at the same merchant more than 
	// <MCF01.pan_same> times in a 24 hour period
	// The merchant usage bucket has to be cleared on a new transaction day

   if (Auth_Tx.CAF01_details.merchant_same[0] != 0)
   {
      ReturnCode = TxAuthorize_Validate_Merchant_Repeat_Checks () ;
      if (ReturnCode != NO_AUTH_ERROR)
         return ReturnCode ;
   }

	/* No need check it again as its done already in TxAuthorize_Perform_Offus_Card_Velocity_Checks TF- Abhishek
	For ONUS and OFFUS we do this check in function TxAuthorize_Perform_Offus_Card_Velocity_Checks TF- Abhishek */

/*	if (Auth_Tx.TLF01_details.category_code[0] != 0)
	{
	  if ( LOCAL_ORIGIN == txservice_GetTransactionOrigin() )
	  {
	     ReturnCode = TxAuthorize_Validate_Category_Count_Checks(CurrencyType);
         if (ReturnCode != NO_AUTH_ERROR && Auth_Tx.TLF01_details.tx_key != AUTH_AUTHORIZATION_OVERRIDE)
          {
          // MCC validation failed, we need to look in to OCM01 , if card is overriden
			 ReturnCode = TxDataServer_Get_OMC01_Details(ONUS);
          return ReturnCode ;
          }
			return ReturnCode ;
	  }
   	}*/
	return NO_AUTH_ERROR ;
} /* TxAuthorize_Perform_Card_Velocity_Checks */

/**************************************************************************
NAME:			TxAuthorize_Perform_offus_Card_Velocity_Checks
DESCRIPTION:	Performs all card velocity checks for the cardholder for 
					all transaction types	
INPUTS:			None				
OUTPUTS:		None
RETURNS:        false  if checks are violated
                true   if allowed
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:
************************************************************************/
BYTE TxAuthorize_Perform_Offus_Card_Velocity_Checks () 
{
	char		strSystemDateTime[50] = {0} ;
	BYTE		strCurrentDate[9] = {0} ;
	BYTE		strTemp[256] = {0} ;
	BYTE		TransactionOrigin = 0 ;
	BYTE		ReturnCode = 0 ;

	// Note that all dates referenced in this function are in Julian

	LONG	CurrentDate = 0L ;
	LONG	DateWhenAPesoTransactionLastOccurred = 0L ;
	LONG	DateWhenADollarTransactionLastOccurred = 0L ;
	LONG	DateWhenATransactionLastOccurred = 0L ;

	int	DaysElapsedSinceATransactionLastOccurred = 0 ;
	int	DaysElapsedSinceAPesoTransactionLastOccurred ;
	int	DaysElapsedSinceADollarTransactionLastOccurred = 0 ;
	
	int	CurrencyType = 0 ;
	int	nIndex = 0 ;
	int	nIndex1 = 0 ;

   double nConvertedTxAmount = 0.0 ;
	double nConversionRate = 0.0 ;
	double nTotalAmount = 0.0 ;
	
	CARD_VELOCITY_FROM_PROFILE card_velocity[3] ;



	if (txservice_If_Tx_Is_Treated_Like_A_Reversal ())
		nTotalAmount = (strtod (Auth_Tx.TLF01_details.reversal_amount, 0) / 100.0) ;
   else if ( Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_SALE ||
             Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_VOID_REFUND)
      nTotalAmount = (strtod (Auth_Tx.TLF01_details.orig_amount, 0) / 100.0) ;
   else if ( txservice_If_Tx_Is_Treated_Like_A_Adjustment () )
   {  
      nTotalAmount = (strtod (Auth_Tx.TLF01_details.orig_amount, 0) / 100.0) - 
                     (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0) ;
   }
	
	else if ( (strcmp ( Auth_Tx.TLF01_details.response_code, "00" ) != 0) && (strcmp ( Auth_Tx.TLF01_details.message_type, "0130" ) == 0 ))
      nTotalAmount = (strtod (Auth_Tx.TLF01_details.orig_amount, 0) / 100.0) ;
   else
		nTotalAmount = (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0) ;

	// convert amount to card holder currency, if they are different
/*	if (!strEqual (Auth_Tx.TLF01_details.conversion_rate, "1"))
	{
		nConversionRate = strtod (Auth_Tx.TLF01_details.conversion_rate, 0) ;
		nConvertedTxAmount = nTotalAmount * nConversionRate ;
	}
	else */
		nConvertedTxAmount = nTotalAmount ;

    /*BDO ARPIL 2011 mandate to support account verfication tx- TF Phani */	
   if(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"51",2) == 0 && nConvertedTxAmount== 0.0)//Check if trnaction is Acc verification tx
	{
		//Log meesage for ACC verfication
		sprintf (strTemp, "Account Verfification Message with NULL amount") ;
		txcntl_log_message( 0, 1, strTemp, "TxAuthorize_Authorize_Transaction", 1 );
	}
   /*BDO ARPIL 2011 MC mandate to support RPS tx with zero amount- TF Phani */	
   else if(nConvertedTxAmount== 0.0 && (0 == strcmp(Auth_Tx.industry_code,"800") ))
   	{
   	//Log meesage for RPS verfication
		sprintf (strTemp, "RPS tx with NULL amount") ;
		txcntl_log_message( 0, 1, strTemp, "TxAuthorize_Authorize_Transaction", 1 );
   	}
    else if ( !TxAuthorize_Validate_TransactionAmount ( nConvertedTxAmount ))
	{
		sprintf (strTemp, "Invalid amount %-9.2f in Tx  ", nConvertedTxAmount) ;
        ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp, "Authorize_Transaction",
															GS_TXN_INVALID_TRANSACTION_AMOUNT,
															INVALID_AMOUNT) ;
		return ReturnCode ;
	}

	memset (&card_velocity, 0, sizeof card_velocity) ;
	memset (strTemp, 0, sizeof (strTemp)) ;

	genutil_format_date ((char*)strSystemDateTime) ;
	memcpy (strCurrentDate, strSystemDateTime, 8) ; 
	// date in yyyymmdd format

	// Current Date is in julian
	CurrentDate = Txutils_Calculate_Julian_Date (strCurrentDate) ;
	
	if (strEqual (Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY))
		CurrencyType = PESO ;
	else if (strEqual (Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY))
		CurrencyType = DOLLAR ;

	// Retrieve the peso and dollar day period & velocity limits (nbr_usage and 
	// dollar_usage) from Auth_Tx.CAF01_details for both the peso and dollar for the 3 periods
	ReturnCode = TxAuthorize_Get_Velocity_Limits_From_Profile (card_velocity) ;

	// peso_julian_date is the date when a peso tx last occurred
	DateWhenAPesoTransactionLastOccurred = atol (Auth_Tx.offus_velocity_details.peso_julian_date) ;
	// dollar_julian_date is the date when a dollar tx last occurred
	DateWhenADollarTransactionLastOccurred = atol (Auth_Tx.offus_velocity_details.dollar_julian_date) ;

	/* The following line of code is written to overcome the problem of the julian date 
	   being set to zero by the card import process
	*/
	   
	if (DateWhenAPesoTransactionLastOccurred == 0L ||
		DateWhenADollarTransactionLastOccurred == 0L)

		return NO_AUTH_ERROR ;

	// Get diff between the current date and when a peso tx was last occurred (could be 0)
	DaysElapsedSinceAPesoTransactionLastOccurred = CurrentDate - DateWhenAPesoTransactionLastOccurred ;
	// Get diff between the current date and when a dollar tx was last occurred (could be 0)
	DaysElapsedSinceADollarTransactionLastOccurred = CurrentDate-DateWhenADollarTransactionLastOccurred ;

	// Regardless of peso or dollar, determine when the latest transaction date for the cardholder
	if (DateWhenAPesoTransactionLastOccurred > DateWhenADollarTransactionLastOccurred)
		DateWhenATransactionLastOccurred = DateWhenAPesoTransactionLastOccurred ;
	else 
		DateWhenATransactionLastOccurred = DateWhenADollarTransactionLastOccurred ;

	// Regardless of peso or dollar, get diff between the current date and when 
	// a transaction was last occurred for the cardholder (could be 0)
	DaysElapsedSinceATransactionLastOccurred = CurrentDate - DateWhenATransactionLastOccurred ;

	if (CurrencyType == PESO)
	{
		// This is done to populate the private structure because this will be used
		// to update the Auth_Tx.CCF03P_details
		private_ccf03p_struct.peso_julian_date  = CurrentDate ;
		private_ccf03p_struct.dollar_julian_date = DateWhenADollarTransactionLastOccurred ;
	}
	else if (CurrencyType == DOLLAR)
	{
		// This is done to populate the private structure because this will be used
		// to update the Auth_Tx.CCF03P_details
		private_ccf03p_struct.dollar_julian_date = CurrentDate ;
		private_ccf03p_struct.peso_julian_date  = DateWhenAPesoTransactionLastOccurred ;
	}

	if (DaysElapsedSinceAPesoTransactionLastOccurred >= 14)
		// Clear the peso bucket because values are irrelevant
		TxAuthorize_Clear_Usage_Bucket (PESO) ;
	else  // Populate the private structure 14 day peso bucket with values from 
			// Auth_Tx.CCF03P_details.peso_usage[]
		TxAuthorize_Populate_Offus_Velocity_Usage_Bucket (PESO) ;
	
	if (DaysElapsedSinceADollarTransactionLastOccurred >= 14)
		// Clear the dollar bucket because values are irrelevant
		TxAuthorize_Clear_Usage_Bucket (DOLLAR) ;
	else // Populate the private structure 14 day dollar bucket with values from 
			//Auth_Tx.CCF03P_details.dollar_usage[]
		TxAuthorize_Populate_Offus_Velocity_Usage_Bucket (DOLLAR) ;

	if (DaysElapsedSinceATransactionLastOccurred != 0)
		// The merchant usage bucket has to be cleared on a new transaction day
		TxAxthorize_Clear_Merchant_Usage_Bucket () ;
	else
		// Populate the private structure with values from Auth_Tx.CCF03P_details.repeat[]
		TxAuthorize_Populate_Offus_Velocity_Merchant_Usage_Bucket () ;

	if (DaysElapsedSinceATransactionLastOccurred != 0)
		// The merchant usage bucket has to be cleared on a new transaction day
		TxAxthorize_Clear_Category_Usage_Bucket () ;
	else
		// Populate the private structure with values from Auth_Tx.CCF03P_details.mcc[]
		TxAuthorize_Populate_Offus_Velocity_Category_Usage_Bucket () ;

	// Populate the private structure failed pin attempts with 
	// Auth_Tx.CCF03P_details.failed_pin_attempts
//	TxAuthorize_Populate_Failed_Pin_Attempts () ;

	// On a reversal the transaction number and amount has to be deducted from the 
	// zeroth element of the usage bucket and then we skip the rest of the process
   if (txservice_If_Tx_Is_Treated_Like_A_Reversal()           || 
       Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE         ||          
       Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE ||          
       txservice_If_Tx_Is_Treated_Like_A_Offline_Void() )
	{
		ReturnCode = TxAuthorize_Deduct_Amount_From_Usage_Bucket (CurrencyType,nConvertedTxAmount) ;
      ReturnCode = TxAuthorize_Update_Offus_Velocity_Structure ();

  		ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host 
						             (strcat(Auth_Tx.CAF01_details.destination_name, "A"), 
										 MT_AUTH_REQUEST, 
										 0, 
										 0) ;
	   if (ReturnCode == false)
	   {
		   sprintf(strTemp,"%s down,Tx sent to %s\n",
				   Auth_Tx.BIN01_details.destination_name,Auth_Tx.TLF01_details.handler_queue) ;
               if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
                   strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;

		   txcntl_log_message( 1, 3, strTemp, "TxAuthorize_Perform_Offus_Card_Velocity_Checks", 1 );
               /* On error, clear the auth_number field - as per Musab Saleh (April 28 '99) */
               memset (Auth_Tx.TLF01_details.auth_number, 0, sizeof Auth_Tx.TLF01_details.auth_number) ;	
		   
               txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
															     MT_AUTH_RESPONSE, 
															     0, 
															     0) ;
	   }
		return ReturnCode ;
	}

	if (CurrencyType == PESO)
	{	// Note that if transactions occur on the same day,no shifting downwards happen
	
		ReturnCode = TxAuthorize_Shift_Peso_Usage_Bucket_Down_And_Verify_Limits 
															(card_velocity,
															 DaysElapsedSinceAPesoTransactionLastOccurred,
															 nConvertedTxAmount) ;
		//Abhishek bypasic Gloab parameter capping fo Merchant over ride denail
		if (ReturnCode != NO_AUTH_ERROR && Auth_Tx.TLF01_details.tx_key != AUTH_AUTHORIZATION_OVERRIDE)
		{
			/* Global Parameter Capping validation failed, we need to look in to OCM01 , if Merchant  is overriden*/
			ReturnCode = TxDataServer_Get_OMC01_Details(OFFUS);
			return ReturnCode ;
		}
		else if (ReturnCode != NO_AUTH_ERROR)
		{
			return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
		}
	
		// Since there were no violations, the counts are incremented
		private_ccf03p_struct.peso_usage[0].nbr_usage += 1 ;

        /* Cap private_ccf03p_struct.peso_usage[0].nbr_usage to 999 */
        if (private_ccf03p_struct.peso_usage[0].nbr_usage > 999)
            private_ccf03p_struct.peso_usage[0].nbr_usage = 999 ;

        private_ccf03p_struct.peso_usage[0].amt_usage += nConvertedTxAmount ;

        if (private_ccf03p_struct.peso_usage[0].amt_usage > 9999999999.99) 
            private_ccf03p_struct.peso_usage[0].amt_usage = 9999999999.99 ;
	}
	else if (CurrencyType == DOLLAR)
	{
	// Note that if transactions occur on the same day,no shifting downwards happen
	
		ReturnCode = TxAuthorize_Shift_Dollar_Usage_Bucket_Down_And_Verify_Limits 
															(card_velocity,
															 DaysElapsedSinceADollarTransactionLastOccurred,
															 nConvertedTxAmount) ;
		//Abhishek bypasic Gloab parameter capping fo Merchant over ride denail
		if (ReturnCode != NO_AUTH_ERROR && Auth_Tx.TLF01_details.tx_key != AUTH_AUTHORIZATION_OVERRIDE)
		{
			/* Global Parameter Capping validation failed, we need to look in to OCM01 , if Merchant  is overriden*/
			ReturnCode = TxDataServer_Get_OMC01_Details(OFFUS);
			return ReturnCode ;
		}
		else if (ReturnCode != NO_AUTH_ERROR)
		{
			return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
		}
		
		// Since there were no violations, the counts are incremented
		private_ccf03p_struct.dollar_usage[0].nbr_usage += 1 ;

        /* Cap private_ccf03p_struct.dollar_usage[0].nbr_usage to 999 */
        if (private_ccf03p_struct.dollar_usage[0].nbr_usage > 999)
            private_ccf03p_struct.dollar_usage[0].nbr_usage = 999 ;

		private_ccf03p_struct.dollar_usage[0].amt_usage += nConvertedTxAmount ;

        if (private_ccf03p_struct.dollar_usage[0].amt_usage > 9999999999.99) 
            private_ccf03p_struct.dollar_usage[0].amt_usage = 9999999999.99 ;
	}
															


	// *********************************************************************
	// Check if the card has been used at the same merchant more than 
	// <MCF01.pan_same> times in a 24 hour period
	// The merchant usage bucket has to be cleared on a new transaction day

   if (Auth_Tx.CAF01_details.merchant_same[0] != 0)
   {
      ReturnCode = TxAuthorize_Validate_Merchant_Repeat_Checks () ;
      if (ReturnCode != NO_AUTH_ERROR)
         return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
   }

	if (Auth_Tx.TLF01_details.category_code[0] != 0)
	{
	   if ( LOCAL_ORIGIN == txservice_GetTransactionOrigin() )
  	   {
  	    	 ReturnCode = TxAuthorize_Validate_Category_Count_Checks(CurrencyType);
    		 //Praneeth modified to support OMC while online txn
		 	if (ReturnCode != NO_AUTH_ERROR)
	 		{
				if (Auth_Tx.TLF01_details.tx_key != AUTH_AUTHORIZATION_OVERRIDE)
				{
					return TxDataServer_Get_OMC01_Details(OFFUS);
				}
				else
				{
					return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
				}
	 		}
  		}
  	}
   

   /* Determine if transaction qualifies for Cash Bonus
    * Program prior to sending it out to a remote host.
    */
   strcpy( Auth_Tx.TLF01_details.dispensation.auth_2, "RA" );
   if (txservice_If_Tx_Qualifies_For_Cash_Bonus_Processing ())
   {
      /* It does qualify; set the flag for updatds. */
      ReturnCode = TxAuthorize_Update_AuthTx_Structures( 0, 1, 0 );
   }

   ReturnCode = TxAuthorize_Update_Offus_Velocity_Structure ();
   if (ReturnCode != NO_AUTH_ERROR)
      return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;

   if (0 == strcmp(Auth_Tx.CAF01_details.authorization_location,"S") &&
	   true == Falcon_RT_Flag  &&
	   true == txservice_If_Tx_Qualifies_For_RealTime() &&
	   false == txservice_Check_If_IST_Message ())
   {
	   /*Lets Get CCF02P details */
	   ReturnCode = TxDataServer_Get_CCF02_Details_For_RT ();
	   if (ReturnCode == NO_AUTH_ERROR)
	   {
	   		return NO_AUTH_ERROR;
	   }
	   else
	   {
		   txcntl_log_message( 2, 3, "Failed to retrieve CCF02P details for FRT transaction", "TxAuthorize_Perform_Offus_Card_Velocity_Checks", 1 );
		   ReturnCode = txservice_Send_Request_To_RT_Falcon_Service();
		   if(false == ReturnCode)
		   {
			   txcntl_log_message( 1, 3, "Failed to send FRT transaction to Falcon", "TxAuthorize_Perform_Offus_Card_Velocity_Checks", 1 );
			   ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
		                                  (strcat(Auth_Tx.CAF01_details.destination_name, "A"),
		                                   MT_AUTH_REQUEST,
		                                   0,
		                                   0) ;
		   }
		   else
		   {
			   return ReturnCode ;
		   }
	   }
   }

   else
   {
	   ReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host
                               (strcat(Auth_Tx.CAF01_details.destination_name, "A"), 
                               MT_AUTH_REQUEST, 
                               0, 
                               0) ;
   }
   if (ReturnCode == false)
   {
      sprintf( strTemp,
              "%s down,Tx sent to %s\n",
               Auth_Tx.BIN01_details.destination_name,
               Auth_Tx.TLF01_details.handler_queue) ;

      if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
      strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;

      txcntl_log_message( 1, 3, strTemp, "TxAuthorize_Perform_Offus_Card_Velocity_Checks", 1 );
      /* On error, clear the auth_number field - as per Musab Saleh (April 28 '99) */
      memset (Auth_Tx.TLF01_details.auth_number, 0, sizeof Auth_Tx.TLF01_details.auth_number) ;	

      txservice_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
                                               MT_AUTH_RESPONSE, 
                                               0, 
                                               0) ;
   }
   return ReturnCode ;
	
} /* TxAuthorize_Perform_Offus_Card_Velocity_Checks */


/*************************************************************************************
NAME:			TxAuthorize_Perform_Card_Limit_Checks
DESCRIPTION:	Determines if the individual card level limits have been exceeded	
INPUTS:			None				
OUTPUTS:		None
RETURNS:        false  if limit is exceeded
                true   if limit is not exceeded
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
BYTE TxAuthorize_Perform_Card_Limit_Checks (double nConvertedTxAmount)
{
	BYTE ReturnCode = 0 ;

	double nCCF02P_Credit_Line = 0.0 ;
	double nCCF02P_Outstanding_Auths = 0.0 ;
	char strTemp[256] = {0} ;

	// Only if there are valid values in credit_line and outstanding_auths, do the check

	if (Auth_Tx.CCF02P_details.credit_line[0] != 0 && 
		 Auth_Tx.CCF02P_details.outstanding_auths[0] != 0)
	{
		nCCF02P_Credit_Line = 
			(strtod (Auth_Tx.CCF02P_details.credit_line, 0) / 100.0) ;
		nCCF02P_Outstanding_Auths = 
			(strtod (Auth_Tx.CCF02P_details.outstanding_auths, 0) / 100.0) ;

		if (AUTH_REVERSAL == Auth_Tx.TLF01_details.tx_key)
		{
			nCCF02P_Incremented_Outstanding_Auths = 
			     nCCF02P_Outstanding_Auths - nConvertedTxAmount ;
		}
		else
		{
			nCCF02P_Incremented_Outstanding_Auths = 
			     nCCF02P_Outstanding_Auths + nConvertedTxAmount ;
		}
		if (nCCF02P_Incremented_Outstanding_Auths > nCCF02P_Credit_Line)
		{
			strcpy (strTemp,"Card(CCF02P) level credit line exceeded") ;
			ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																 "Perform_Merchant_Max_Credit_Amt",
																GS_TXN_CARD_LEVEL_CREDIT_LINE_LIMIT_FAILED,
																INVALID_TRANSACTION) ;
			return ReturnCode ;

			
		}
	}

	return NO_AUTH_ERROR ;
} /* TxAuthorize_Perform_Card_Limit_Checks */

/**************************************************************************
NAME:			TxAuthorize_Perform_Balance_Account_Checks
DESCRIPTION:	Determines if the balance account limits have been exceeded	
INPUTS:			None				
OUTPUTS:		None
RETURNS:        false  if limit is exceeded
                true   if limit is not exceeded
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
**************************************************************************/
BYTE TxAuthorize_Perform_Balance_Account_Checks (double nConvertedTxAmount) 
{
   double nACF01_Available        = 0.0 ;
   double nACF01_Cash_Available   = 0.0 ;
   double nACF01_Outstanding      = 0.0 ;
   double nACF01_Cash_Outstanding = 0.0 ;
   double nACF01_Credit_Limit     = 0.0 ;
   double nACF01_Cash_Limit       = 0.0 ;
   double nTLF01_orig_amount      = 0.0 ;

   BYTE strTemp[256] = {0} ;
   BYTE strTxType[7] = {0} ;
   BYTE strProcessingCode[20] = {0} ;
   BYTE ReturnCode = 0 ;


   /* Reset the global variables */
   nCCF02P_Incremented_Outstanding_Auths = 0.0 ;
   nACF01_Incremented_Outstanding        = 0.0 ;
   nACF01_Incremented_Cash_Outstanding   = 0.0 ;
   nACF01_Decremented_Available          = 0.0 ;
   nACF01_Decremented_Cash_Available     = 0.0 ;

   /* Populate with values from the ACF01 record */
   nACF01_Available        = (strtod (Auth_Tx.ACF01_details.available, 0) / 100.0) ;
   nACF01_Outstanding      = (strtod (Auth_Tx.ACF01_details.outstanding, 0) / 100.0) ;
   nACF01_Cash_Available   = (strtod (Auth_Tx.ACF01_details.cash_available, 0) / 100.0) ;
   nACF01_Cash_Outstanding = (strtod (Auth_Tx.ACF01_details.cash_outstanding, 0) / 100.0) ;
   nACF01_Credit_Limit     = (strtod (Auth_Tx.ACF01_details.credit_limit, 0) / 100.0) ;
   nACF01_Cash_Limit       = (strtod (Auth_Tx.ACF01_details.cash_limit, 0) / 100.0) ;

   if(nACF01_Available == 0)
   {
	   // We are seeing avaiable as Zero, lets check if we have retrive details correctly or not
	   //Cash
	   if ( Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE               ||
	        Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION ||
	        Auth_Tx.TLF01_details.tx_key == AUTH_QUASI_CASH )
		{
		   if(nACF01_Cash_Limit <= 0)
		   {
			   txcntl_log_message( 2, 3,"Failed to read ACF01 details for Cash",
					   	   	   	   "TxAuthorize_Perform_Balance_Account_Checks", 1 );
		   }
		   else
		   {
			   nACF01_Available = nACF01_Cash_Limit - nACF01_Cash_Outstanding;
		   }
		}
	    // Credit
	    else if(nACF01_Credit_Limit <= 0)
	    {
		   txcntl_log_message( 2, 3,"Failed to read ACF01 details",
				   	   	   	   "TxAuthorize_Perform_Balance_Account_Checks", 1 );
	    }
	    else
	    {
		   nACF01_Available = nACF01_Credit_Limit - nACF01_Outstanding;
	    }
   }

   /* If tx is CASH_ADVANCE or QUASI_CASH,
    * then follow the algorithm in the specs.
    */
   if ( Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE               ||
        Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION ||
        Auth_Tx.TLF01_details.tx_key == AUTH_QUASI_CASH )
   {
      if (nConvertedTxAmount <= nACF01_Cash_Available)
      {
         if (nConvertedTxAmount <= nACF01_Available)
         {
            // transaction is approved
            nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding + nConvertedTxAmount ;
            nACF01_Decremented_Cash_Available   = nACF01_Cash_Available - nConvertedTxAmount ;
            nACF01_Incremented_Outstanding      = nACF01_Outstanding  + nConvertedTxAmount ;
            nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ; 
         }
         else
         {
            sprintf (strTemp,"Bal_Chk fail-Avail:%-9.2fReq:%-9.2f ", 
                     nACF01_Available, nConvertedTxAmount) ;

            ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                 "TxAuthorize_Perform_Balance_Account_Checks",
                                  GS_TXN_ACF01_BALANCE_ACCOUNT_LIMIT_FAILED, 
                                  NO_SUFFICIENT_FUNDS) ;
            if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
            {
               strcpy(Auth_Tx.TLF01_details.response_text,O2_INSUFFIENT_FUNDS);
            }
            return ReturnCode ;

         }
      }
      else
      {
         sprintf (strTemp,"CashAdv fail-Avail:%-9.2fReq:%-9.2f", 
                  nACF01_Cash_Available, nConvertedTxAmount) ;

         ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                 "TxAuthorize_Perform_Balance_Account_Checks",
                                  GS_TXN_ACF01_BALANCE_ACCOUNT_LIMIT_FAILED, 
                                  NO_SUFFICIENT_FUNDS) ;
         if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
         {
            strcpy(Auth_Tx.TLF01_details.response_text,O2_INSUFFIENT_FUNDS);
         }
         return ReturnCode ;
      }
   }
   else if ( (strcmp ( Auth_Tx.TLF01_details.response_code, "00" ) != 0) &&
             (strcmp ( Auth_Tx.TLF01_details.message_type, "0130" )== 0 ) )
   {
      nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
      nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
      nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
      nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
   }
   else if ( Auth_Tx.TLF01_details.tx_key == AUTH_REFUND )
   {
      nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
      nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
      nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
      nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
   }
   else if (txservice_If_Tx_Is_Treated_Like_A_Reversal()       || 
            Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE     ||          
            Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND   ||          
            txservice_If_Tx_Is_Treated_Like_A_Offline_Void() )
   {
      /* Transaction is AUTH_REVERSAL, or ATM_CONFIRM, or REVERSAL_ADVICE */
      strncpy (strProcessingCode, Auth_Tx.TLF01_details.processing_code, 2) ;

      /* If the previous tx was a SALE, OFFLINE_SALE, AUTHORIZATION or a
       * PRE_AUTHORIZATION, we have to undo the previous transaction from
       * the Oustanding and the Available.
       */
	  /* SCR 12502 - ThoughtFocus 
	   * REVERSAL of an AUTH_VOID_REFUND, AUTH_OFFLINE_VOID_REFUND, AUTH_REFUND_ADJUSTMENT, AUTH_VOID_RELOAD
	   */
	  if  ( txservice_If_Tx_Is_Treated_Like_A_Reversal() &&
		    (strEqual (strProcessingCode, "22") || 
		    (0 == strncmp(Auth_Tx.BIN01_details.card_type,"CUP",3)))
		  )
	  {
		 nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
	  }
	  /* SCR 12502 - ThoughtFocus */ 
   /* if  ( strEqual (strProcessingCode, "20") ) */ /* AUTH_REFUND */
	  else if  ( strEqual (strProcessingCode, "20") )  /* REVERSAL of AUTH_REFUND/AUTH_VOID_CASH_ADVANCE */
      { /*partial*/
         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
      else if  ( strEqual (strProcessingCode, "22") )  /* AUTH_VOID_REFUND/AUTH_OFFLINE_REFUND*/
      {
         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }

      /* Deferred Purchase including Deferred Voids
       * ------------------------------------------
       * This IF statement looks for a deferred purchase OR void of deferred,
       * AND not an RSB card.  Need this 'and' because RSB is very similar to
       * a deferred purchase, but there is no deferred record. So an RSB card
       * will fall through to the next 'else if'.
       */
      else if ( ( (strEqual(strProcessingCode, "70"))  ||
                  (Auth_Tx.TLF01_details.deferred_term_length[0] != 0x00) ) &&
                (  Auth_Tx.BIN01_details.credit_subtype[0] != RSB_CARD)      )
      {
         if (strEqual (Auth_Tx.ACF01_details.primary_key.account_type, PESO_ACCOUNT_TYPE))
         {
            /* SECOND PASS THROUGH HERE
             * ------------------------
             * This is the PESO account for the deferred customer.
             * Calculate the monthly amortized amount.  It is compared
             * to the Peso balance account.
             *
             * The amortized amount is nConvertedTxAmount
             * The available and outstanding values correspond to
             * the PESO account.
             */
            TxAuthorize_Calculate_Deferred_Monthly_Amortized_Amount (&nConvertedTxAmount) ;
         }
         else if (strEqual (Auth_Tx.ACF01_details.primary_key.account_type, DEFERRED_ACCOUNT_TYPE))
         {
            /* FIRST PASS THROUGH HERE
             * -----------------------
             * This is the DEFERRED account for the deferred customer.
             * Calculate the gross deferred amount.  It will be compared to
             * the deferred balance account.  Later, 2nd pass, the monthly
             * amortization amount will be compared to the Peso balance.
             *
             * The gross amount is nConvertedTxAmount
             * The available and outstanding values correspond to
             * the DEFERRED account
             */
            TxAuthorize_Calculate_Deferred_Gross_Amount (&nConvertedTxAmount) ;
         }
         sprintf ( strTemp, "%.2f", nConvertedTxAmount ) ;
         nConvertedTxAmount = strtod ( strTemp, 0 );

         nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Available        = nACF01_Credit_Limit - nACF01_Incremented_Outstanding;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Limit - nACF01_Incremented_Cash_Outstanding;
      }
#ifdef AMEX03A
      else if(true == txservice_Check_If_AMEX_issuing_transaction() &&
    		  true == txservice_check_for_AMEX_partial_reversal_transaction())
      {
    	  nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
    	  nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
    	  /* Not done any changes to the cash part, just copy back the data read from ACF01 back to ACF01*/
          nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
          nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
#endif
	  /* */
	  /* SCR 12502 -ThoughtFocus
	   * REVERSAL of a AUTH_VOID_SALE or AUTH_OFFLINE_VOID_SALE or AUTH_SALE_ADJUSTMENT
	   */
	  else if ( txservice_If_Tx_Is_Treated_Like_A_Reversal() &&
		  strEqual (strProcessingCode, "02")) 
	  {
		 nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Available        = nACF01_Credit_Limit - nACF01_Incremented_Outstanding;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Limit - nACF01_Incremented_Cash_Outstanding;
	  }

      else if  (strEqual (strProcessingCode, "00") || /* AUTH_SALE              */ 
                                                      /* or AUTH_OFFLINE        */
                                                      /* or AUTH_AUTHORIZATION  */
           strEqual (strProcessingCode, "30") ||      /* AUTH_PRE_AUTHORIZATION */
           strEqual (strProcessingCode, "02"))        /* AUTH_VOID_SALE         */
      {
         nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Available        = nACF01_Credit_Limit - nACF01_Incremented_Outstanding;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Limit - nACF01_Incremented_Cash_Outstanding;
      }
      else if (strEqual (strProcessingCode, "01") || /* AUTH_CASH_ADVANCE */
               strEqual (strProcessingCode, "17") ||
               strEqual (strProcessingCode, "11") || /* AUTH_QUASI_CASH */
               strEqual (strProcessingCode, "18")  ) /* AUTH_QUASI_CASH */
      {
         /* If the previous tx was a QUASI_CASH or CASH_ADVANCE,
          * we need to undo it from purchase and cash accounts.
          */

         /* Undo Purchase Account */
         nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Credit_Limit - nACF01_Incremented_Outstanding;

         /* Undo Cash Account */
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding - nConvertedTxAmount ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Limit - nACF01_Incremented_Cash_Outstanding;
      }
   }

   else if (txservice_If_Tx_Is_Treated_Like_A_Voice_Auth_Override ())
   {
      /* AUTH_AUTHORIZATION_OVERRIDE, AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE,
         AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE */
      if (Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE) 
      {
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available   - nConvertedTxAmount ;
         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ; 
         nACF01_Decremented_Available        = nACF01_Available  - nConvertedTxAmount ;
      }
      else if (Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_OVERRIDE ||
               Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE) 
      {
         if (Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE)
         {
            TxAuthorize_Calculate_Deferred_Monthly_Amortized_Amount (&nConvertedTxAmount) ;
            sprintf ( strTemp, "%.2f", nConvertedTxAmount ) ;
            nConvertedTxAmount = strtod ( strTemp, 0 );
         }

         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
   }
   else if ( txservice_If_Tx_Is_Treated_Like_A_Adjustment () )
   {
      strncpy (strProcessingCode, Auth_Tx.TLF01_details.processing_code, 2) ;
      if  ( strEqual (strProcessingCode, "22") )  /* AUTH_REFUND */
      {
         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
      else
      {
         nACF01_Incremented_Outstanding      = nACF01_Outstanding - nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
   }

   else if (txservice_If_Tx_Is_Treated_Like_A_Non_Reversal_Advice ())
   {
      /* AUTH_OFFLINE_SALE or AUTH_ADVICE or AUTH_ADVICE_REPEAT
       * No need to validate amounts, as this transaction cannot be rejected
       */
      nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
      nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
      nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
      nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
   }
   else if ( Auth_Tx.TLF01_details.tx_key == AUTH_CARD_VERIFICATION )
   {
      nACF01_Incremented_Outstanding      = nACF01_Outstanding ;
      nACF01_Decremented_Available        = nACF01_Available ;
      nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
      nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;

      if (nConvertedTxAmount > nACF01_Available)
      {
         sprintf (strTemp,"Bal_Acc_Chk fail-Avail:%9.2fReq:%9.2f", 
                  nACF01_Available, nConvertedTxAmount) ;

         ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                             "TxAuthorize_Perform_Balance_Account_Checks",
                              GS_TXN_ACF01_BALANCE_ACCOUNT_LIMIT_FAILED,
                              NO_SUFFICIENT_FUNDS) ;
         if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
         {
            strcpy(Auth_Tx.TLF01_details.response_text,O2_INSUFFIENT_FUNDS);
         }
         return ReturnCode ;
      }
   }
   else if (Auth_Tx.TLF01_details.tx_key == AUTH_SALE ||
            Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION ||
            Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
            Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION||
			Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE)
            
   {
      if (Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION)
      {
         if (strEqual (Auth_Tx.ACF01_details.primary_key.account_type, PESO_ACCOUNT_TYPE))
         {
            /* SECOND PASS THROUGH HERE
             * ------------------------
             * This is the PESO account for the deferred customer.
             * Calculate the monthly amortized amount.  It is compared
             * to the Peso balance account.
             *
             * The amortized amount is nConvertedTxAmount
             * The available and outstanding values correspond to
             * the PESO account
             */
            TxAuthorize_Calculate_Deferred_Monthly_Amortized_Amount (&nConvertedTxAmount) ;
         }
         else if (strEqual (Auth_Tx.ACF01_details.primary_key.account_type, DEFERRED_ACCOUNT_TYPE))
         {
            /* FIRST PASS THROUGH HERE
             * -----------------------
             * This is the DEFERRED account for the deferred customer.
             * Calculate the gross deferred amount.  It will be compared to
             * the deferred balance account.  Later, 2nd pass, the monthly
             * amortization amount will be compared to the Peso balance.
             *
             * The gross amount is nConvertedTxAmount
             * The available and outstanding values correspond to
             * the DEFERRED account
             */
            TxAuthorize_Calculate_Deferred_Gross_Amount (&nConvertedTxAmount) ;
         }
         sprintf ( strTemp, "%.2f", nConvertedTxAmount ) ;
         nConvertedTxAmount = strtod ( strTemp, 0 );
      }

      if (nConvertedTxAmount <= nACF01_Available)
      { /*estimated and incremental*/
         nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      }
      else
      {
		if ( strEqual(Auth_Tx.TLF01_details.host_finish_time, USD_COMP) )
		{
			strcpy( Auth_Tx.TLF01_details.currency_code,   USDOLLAR_CURRENCY );
		}
		else if ( strEqual(Auth_Tx.TLF01_details.host_finish_time, PESO_COMP) )
		{
			strcpy( Auth_Tx.TLF01_details.currency_code,   PESO_CURRENCY );
		}
		if(Auth_Tx.TLF01_details.host_start_time[0] == NULL)
			strcpy( Auth_Tx.TLF01_details.host_start_time,"" );
		if(Auth_Tx.TLF01_details.host_finish_time[0] == NULL)
			strcpy( Auth_Tx.TLF01_details.host_finish_time,"");
		strcpy(Auth_Tx.CCF02P_details.pin_verification_override,"");
         sprintf (strTemp,
                 "BalChk fail-Avail:%-9.2fReq:%-9.2f", 
                  nACF01_Available, nConvertedTxAmount) ;

         ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                                  "TxAuthorize_Perform_Balance_Account_Checks",
                                                   GS_TXN_ACF01_BALANCE_ACCOUNT_LIMIT_FAILED,
                                                   NO_SUFFICIENT_FUNDS) ;
         if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
         {
            strcpy(Auth_Tx.TLF01_details.response_text,O2_INSUFFIENT_FUNDS);
         }
         return ReturnCode ;
      }
   }
   else if (Auth_Tx.TLF01_details.tx_key == AUTH_RELOAD_CONFIRM)
   {
	     nACF01_Incremented_Outstanding      = nACF01_Outstanding ;
         nACF01_Decremented_Available        = nACF01_Available + nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      
   }
   else if (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_RELOAD)
   {
	     nACF01_Incremented_Outstanding      = nACF01_Outstanding ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
      
   }

   /* Make sure Available is not greater than credit limit. */
   if ( nACF01_Decremented_Available > nACF01_Credit_Limit )
      nACF01_Decremented_Available = nACF01_Credit_Limit;

   if ( nACF01_Decremented_Cash_Available > nACF01_Cash_Limit )
      nACF01_Decremented_Cash_Available = nACF01_Cash_Limit;

   return NO_AUTH_ERROR ;
} /* TxAuthorize_Perform_Balance_Account_Checks */

/******************************************************************************

NAME:          TxAuthorize_Update_AuthTx_Structures

DESCRIPTION:   Updates the Auth_Tx structure with the results of validation
               converting from numeric to string if necessary.  Also sets
               the mask for what tables to update

INPUTS:        Flags indicating which structures to update.
OUTPUTS:       None

RETURNS:       Always returns NO_AUTH_ERROR

AUTHOR:        Sanjoy Dasgupta
*******************************************************************************/
BYTE TxAuthorize_Update_AuthTx_Structures( BYTE UpdateCCF03PStruct,
                                           BYTE UpdateCashBonusStruct,
                                           BYTE UpdateACF01Struct )
{
	if ( strEqual(Auth_Tx.TLF01_details.host_finish_time, USD_COMP) )
	{
		strcpy( Auth_Tx.TLF01_details.currency_code,   USDOLLAR_CURRENCY );
	}
	else if ( strEqual(Auth_Tx.TLF01_details.host_finish_time, PESO_COMP) )
	{
		strcpy( Auth_Tx.TLF01_details.currency_code,   PESO_CURRENCY );
	}
	if ( strEqual(Auth_Tx.TLF01_details.host_start_time, PESO_COMP) )
	{
		strcpy( Auth_Tx.TLF01_details.currency_code,   USDOLLAR_CURRENCY );
	}
	if(Auth_Tx.TLF01_details.host_start_time[0] == NULL)
	{
		strcpy( Auth_Tx.TLF01_details.host_start_time,"" );
	}
	strcpy( Auth_Tx.TLF01_details.host_finish_time,"" );
	strcpy(Auth_Tx.CCF02P_details.pin_verification_override,"");
   /* Clear the update mask */
   Auth_Tx.TLF01_details.update_mask[0] = 0x00;

   /* Update On-Us Velocity. */
   if (UpdateCCF03PStruct)
      (void)TxAuthorize_Update_CCF03_Structure( true );

   /* Update Balances */
   if (UpdateACF01Struct)
      (void)TxAuthorize_Update_ACF01_Structure();

   /* Now update the cash bonus table BON01 */
   if (UpdateCashBonusStruct)
      (void)TxAuthorize_Update_BON01_Structure();

	return NO_AUTH_ERROR;
} /* TxAuthorize_Update_AuthTx_Structures */



/*******************************************************************************
NAME:          TxAuthorize_Process_Security
DESCRIPTION:   Validates the pin in the card, if any

INPUTS:        None
OUTPUTS:       None
RETURNS:       NO_PIN_ERROR (if there was no pin)
               PIN_ERROR (if track pin indicator was invalid or pin 
                          retries exceeded limit or XIPC error)
               PIN_SENT_FOR_VALIDATION (if there was a pin and was sent for 
                                        validation to the crypto service)
AUTHOR:        Sanjoy Dasgupta
*******************************************************************************/
BYTE TxAuthorize_Process_Security (BYTE verification_type)
{
   INT         nCAF01_max_pin_attempts = 0 ;
   BYTE        ReturnCode = 0 ;
   BOOLEAN     bReturnCode = true ;
   CHAR        strTemp[256] = {0} ;
   LONG        CurrentDate = 0L ;
   LONG        PINFailedDate = 0L;
   CHAR        strSystemDateTime[50] = {0} ;
   BYTE        strCurrentDate[9] = {0} ;

   if (verification_type == PIN_VERIFICATION)
   {
      if (Auth_Tx.CAF01_details.max_pin_retries[0] != 0)
      {
         nCAF01_max_pin_attempts = atoi (Auth_Tx.CAF01_details.max_pin_retries);
         if ((private_ccf03p_struct.failed_pin_attempts + 1) > nCAF01_max_pin_attempts)
         {
            /* Increment failed_pin_attempts because this
             * has to be set in the table.
             */
            private_ccf03p_struct.failed_pin_attempts += 1;
            strcpy( strTemp, "Max failed PIN attempt exceed-see Admin" );

            /* Set parameter 'false' so the date is not updated to 'today'.
             * That gets updated at the end of authorization.  Right now,
             * need to keep the date set to the date of the last activity
             * until velocity is checked.
             */
            TxAuthorize_Update_CCF03_Structure( false );

            ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                  "TxAuthorize_Validate_Pin",
                                                   GS_TXN_INVALID_FAILED_PIN_LIMIT_EXCEEDED,
                                                   PIN_TRIES_EXCEEDED) ;
            return (PIN_ERROR) ;
         }
      }

      /* Send the transaction to the hardware box for pin verification */
      if( (keyblock_enable_flag_for_MC == KEYBLOCK_ENABLE) &&
    	  (true == txservice_Check_If_MC_Transaction()))
      {
    	  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
    			  	  	  	  	  	  	  	  	  	  	  	  	  	  	   MT_NSP_VERIFYPIN_KEYBLOCK,
																		   0, 0) ;
      }
      else if( (keyblock_enable_flag_for_VISA == KEYBLOCK_ENABLE)&&
        	   (true == txservice_Check_If_VISA_Transaction()))
      {
    	  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
																		   MT_NSP_VERIFYPIN_KEYBLOCK,
																		   0, 0) ;
      }
      else if( (keyblock_enable_flag_for_AMEX == KEYBLOCK_ENABLE)&&
       	       (true == txservice_Check_If_AMEX_Transaction()))
      {
    	  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
																		   MT_NSP_VERIFYPIN_KEYBLOCK,
																		   0, 0) ;
      }
      else if( (keyblock_enable_flag_for_JCB == KEYBLOCK_ENABLE)&&
       	       (true == txservice_Check_If_JCB_Transaction()))
      {
    	  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
    			  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFYPIN_KEYBLOCK,
																  0, 0) ;
      }
      else if( (keyblock_enable_flag_for_UPI == KEYBLOCK_ENABLE)&&
    		   (true == txservice_Check_If_CUP_Transaction()))
      {
    	  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
    			  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFYPIN_KEYBLOCK,
																  0, 0) ;
      }
      else if( (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)&&
    		   (true == txservice_Check_If_Diners_Transaction()))
      {
    	  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
    			  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFYPIN_KEYBLOCK,
																  0, 0) ;
      }
      else
      {
    	  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host( crypto_que_name,
                                                              	  MT_NSP_VERIFYPIN,
																  0, 0) ;
      }
      if (false == bReturnCode)
      {
         strcpy (Auth_Tx.TLF01_details.response_text, "System Error") ;
         return (SYSTEM_ERROR );
      }
   }
   else if (verification_type == MAG_STRIP_VERIFICATION)
   {
	   if( (keyblock_enable_flag_for_MC == KEYBLOCK_ENABLE) &&
		  (true == txservice_Check_If_MC_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_VISA == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_VISA_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_AMEX == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_AMEX_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_JCB == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_JCB_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_UPI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_CUP_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_Diners_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else
	   {
		   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host (crypto_que_name,
                                                      MT_NSP_VERIFY_MAGSTRIPE, 
                                                      0, 0) ;
	   }

      if (false == bReturnCode)
      {
         return (SYSTEM_ERROR) ;
      }
   }

   else if (verification_type == NSP_VERIFY_ARQC)
   {
	   if( (keyblock_enable_flag_for_MC == KEYBLOCK_ENABLE) &&
		   (true == txservice_Check_If_MC_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_ARQC_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_VISA == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_VISA_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_ARQC_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_AMEX == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_AMEX_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_ARQC_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_JCB == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_JCB_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_ARQC_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_UPI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_CUP_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_ARQC_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_Diners_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_ARQC_KEYBLOCK,
																  0, 0) ;
	   }
	   else
	   {

		   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host (crypto_que_name,
				   	   	   	   	   	   	   	   	   	   	   	   	   MT_NSP_VERIFY_ARQC,
																   0, 0) ;
	   }

	   if (false == bReturnCode)
	   {
		   return (SYSTEM_ERROR) ;
	   }
   }
   else if (verification_type == NSP_VERIFY_3D_SECURE)
   {
	   if( (keyblock_enable_flag_for_MC == KEYBLOCK_ENABLE) &&
		   (true == txservice_Check_If_MC_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_VISA == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_VISA_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_AMEX == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_AMEX_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_JCB == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_JCB_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_UPI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_CUP_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_Diners_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_KEYBLOCK,
																  0, 0) ;
	   }
	   else
	   {
		   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host (crypto_que_name,
				   	   	   	   	   	   	   	   	   	   	   	   	   MT_NSP_VERIFY_3D_SECURE,
																   0, 0) ;
	   }

	   if (false == bReturnCode)
	   {
		   return (SYSTEM_ERROR) ;
	   }
   }
   else if (verification_type == NSP_CUP_ECOM_CVN2)
   {
	   if( (keyblock_enable_flag_for_MC == KEYBLOCK_ENABLE) &&
		   (true == txservice_Check_If_MC_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_CUP_ECOM_CVN2_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_VISA == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_VISA_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_CUP_ECOM_CVN2_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_AMEX == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_AMEX_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_CUP_ECOM_CVN2_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_JCB == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_JCB_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_CUP_ECOM_CVN2_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_UPI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_CUP_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_CUP_ECOM_CVN2_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_Diners_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_CUP_ECOM_CVN2_KEYBLOCK,
																  0, 0) ;
	   }
	   else
	   {
		   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host (crypto_que_name,
														 	 	   MT_NSP_VERIFY_CUP_ECOM_CVN2,
																   0, 0) ;
	   }

	   if (false == bReturnCode)
	   {
		   return (SYSTEM_ERROR) ;
	   }
   }
   else if (verification_type == MT_NSP_VERIFY_3D_SECURE_2)
   {
	   if( (keyblock_enable_flag_for_MC == KEYBLOCK_ENABLE) &&
		   (true == txservice_Check_If_MC_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_2_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_VISA == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_VISA_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_2_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_AMEX == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_AMEX_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_2_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_JCB == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_JCB_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_2_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_UPI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_CUP_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_2_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_Diners_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY_3D_SECURE_2_KEYBLOCK,
																  0, 0) ;
	   }
	   else
	   {
		   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host (crypto_que_name,
				   	   	   	   	   	   	   	   	   	   	   	   	   MT_NSP_VERIFY_3D_SECURE_2,
																   0, 0) ;
	   }

	   if (false == bReturnCode)
	   {
		   return (SYSTEM_ERROR) ;
	   }
   }
   else if (verification_type == MT_NSP_VERIFY3DS2_MAGSTRIPE)
   {
	   if( (keyblock_enable_flag_for_MC == KEYBLOCK_ENABLE) &&
		   (true == txservice_Check_If_MC_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY3DS2_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_VISA == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_VISA_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY3DS2_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_AMEX == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_AMEX_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY3DS2_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_JCB == KEYBLOCK_ENABLE)&&
				   (true == txservice_Check_If_JCB_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY3DS2_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_UPI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_CUP_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY3DS2_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else if( (keyblock_enable_flag_for_DCI == KEYBLOCK_ENABLE)&&
			   (true == txservice_Check_If_Diners_Transaction()))
	   {
		  bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host_keyblock( crypto_que_name,
				  	  	  	  	  	  	  	  	  	  	  	  	  MT_NSP_VERIFY3DS2_MAGSTRIPE_KEYBLOCK,
																  0, 0) ;
	   }
	   else
	   {
		   bReturnCode = txservice_Forward_Auth_Tx_To_Remote_Host (crypto_que_name,
    		  	  	  	  	  	  	  	  	  	  	  	  	  	   MT_NSP_VERIFY3DS2_MAGSTRIPE,
																   0, 0) ;
	   }

	   if (false == bReturnCode)
	   {
		   return (SYSTEM_ERROR) ;
	   }
   }

	return (NO_AUTH_ERROR) ;
} /* TxAuthorize_Validate_Pin_Indicator */
/******************************************************/
BYTE TxAuthorize_Process_3D_Secure_Response( pPTE_MSG p_msg_in )
{
   BYTE    ResultCode = 0 ;
   BYTE    ReturnCode = 0 ;
   BOOLEAN bReturnCode = true ;
   CHAR    strTemp[256] = {0} ;

   pBYTE          pATEMessageDataData = 0 ;
   pPTE_MSG_DATA  pATEMessageData     = 0 ;

   pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
   pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;

   ResultCode = ptemsg_get_result_code (p_msg_in) ;

   /***********************************************************************/
   memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
   memcpy (&Auth_Tx, (AUTH_TX *) pATEMessageDataData, sizeof Auth_Tx) ;
   /***********************************************************************/

   ReturnCode = txservice_Perform_PTE_Message_Checks (p_msg_in) ;
   if (ReturnCode == NO_AUTH_ERROR)
   {
      /* Perform semantic checks on the transaction */
      ReturnCode = txservice_Perform_Auth_Tx_Checks () ;
      if (ReturnCode == NO_AUTH_ERROR)
      {
         /* Repopulate the private ccf03p structure. */
         TxAuthorize_Update_Private_Ccf03p_Structure () ;

         if (!strEqual (Auth_Tx.TLF01_details.security_response_code, "00"))
         {



        	if(strcmp(Auth_Tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 &&
   			   Auth_Tx.TLF01_details.product_codes[16].amount[3] == '5')
        	{
        		sprintf( strTemp, "3DS 1 validation Failed & DE61.4(%c)", Auth_Tx.TLF01_details.product_codes[16].amount[3]);
				ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
															  "TxAuthorize_Process_3D_Secure_Response",
															   GS_MAGSTRIPE_FAILED,
															   mc_3ds2_delcine_rc) ;
        	}
        	else if(strcmp(Auth_Tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 &&
        			strcmp(Auth_Tx.industry_code,"700")== 0 &&
					strcmp(Auth_Tx.TLF01_details.nfi_end_date,"1") == 0)
        	{
        		sprintf( strTemp, "3D secure validation Failed ");
        		ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
        																	  "TxAuthorize_Process_3D_Secure_Response",
        																	   GS_MAGSTRIPE_FAILED,
        																	   mc_3ds2_delcine_rc) ;
        	}
        	else
        	{
        		sprintf( strTemp, "3D secure or CVV/CAVV validation Failed" );
				ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
															  "TxAuthorize_Process_3D_Secure_Response",
															   GS_MAGSTRIPE_FAILED,
															   DO_NOT_HONOR) ;
        	}

            ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
         }
     
         if ( ReturnCode == NO_AUTH_ERROR )
            /* Get the MCC Limits, then authorize the transaction. */
  		 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
      }
   }
   return ReturnCode ;
	
} /* TxAuthorize_Process_3D_Secure_Response */

BYTE TxAuthorize_Process_3D_Secure_Response_2( pPTE_MSG p_msg_in )
{
   BYTE    ResultCode = 0 ;
   BYTE    ReturnCode = 0 ;
   BOOLEAN bReturnCode = true ;
   CHAR    strTemp[256] = {0} ;
   CHAR 	txn_cavv[4] = {0};
   CHAR 	txn_avop[4] = {0};

   pBYTE          pATEMessageDataData = 0 ;
   pPTE_MSG_DATA  pATEMessageData     = 0 ;


   INT de48_retcode= 0;
   	INT ucaf_data_64_len =0;
   	BYTE UCAF_data[33]={0};
   	INT ucaf_data_len,astr_len = 0;
   	char cvv_value[9] = {0};
   	BYTE UCAF_data_ascii[41]={0},UCAF_data_bin[41]={0};
   	unsigned char   mac[9] = {0};

   pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
   pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;

   ResultCode = ptemsg_get_result_code (p_msg_in) ;

   /***********************************************************************/
   memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
   memcpy (&Auth_Tx, (AUTH_TX *) pATEMessageDataData, sizeof Auth_Tx) ;
   /***********************************************************************/

   ReturnCode = txservice_Perform_PTE_Message_Checks (p_msg_in) ;
   if (ReturnCode == NO_AUTH_ERROR)
   {
      /* Perform semantic checks on the transaction */
      ReturnCode = txservice_Perform_Auth_Tx_Checks () ;
      if (ReturnCode == NO_AUTH_ERROR)
      {
         /* Repopulate the private ccf03p structure. */
         TxAuthorize_Update_Private_Ccf03p_Structure () ;


         if((0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_VISA)))
         {
             strncpy(txn_cavv, &Auth_Tx.TLF01_details.dcf01_retired_cwk[7], 3);

             /* Flag to build DE44 in 0110 if transaction qualifies for VISA 3DS2 */
             Auth_Tx.TLF01_details.product_codes[14].quantity[0] = '1';

             if (!strEqual (Auth_Tx.TLF01_details.security_response_code, "00"))
             {

            	sprintf( strTemp, "3D secure or CVV/CAVV validation Failed" );

                ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                              "TxAuthorize_Process_3D_Secure_Response_2",
															  GS_TXN_3DS2_FAILED,
                                                               DO_NOT_HONOR) ;

                ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
             }
             else if(strncmp(txn_cavv, Auth_Tx.conv_date, 3) == 0)
             {
            	sprintf(strTemp, "3DS2 CAVV matches, CAVV & cvv is : %s", txn_cavv, Auth_Tx.conv_date);
            	txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );
            	ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );

             }
             else
             {
            	 sprintf(strTemp, "3DS2 CAVV mismatch, CAVV & cvv is : %s",txn_cavv, Auth_Tx.conv_date);
            	 txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );
            	 sprintf(strTemp, "3DS2 CAVV/cvv mismatch");
            	 ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
            	                                                           "TxAuthorize_Process_3D_Secure_Response_2",
																		   GS_TXN_3DS2_FAILED,
            	                                                            DO_NOT_HONOR) ;
             }
         }
         else if((0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_MASTERCARD)))
         {
        	 if(0 == strncmp(Auth_Tx.TLF01_details.security_response_code, "00",2) ||
        	    0 == strncmp(Auth_Tx.TLF01_details.security_response_code, "03",2))
        	 {
        		 txcntl_log_message( 3, 1,"3DS Txn verified", "TxAuthorize_Process_3D_Secure_Response_2", 1 );
        		}
        	 else if(0 == strncmp(Auth_Tx.TLF01_details.security_response_code, "05",2))
        	 {
				// strncpy(Auth_Tx.TLF01_details.response_code,DO_NOT_HONOR,2);
        		 strncpy(Auth_Tx.TLF01_details.security_response_code, Auth_Tx.pos_pin_capt_code,2);
				strcpy(strTemp, "CAVV validation fail & CVV2 not verified");
				ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,	"TxDataServer_Continue_With_Pin_Processing",
							 GS_TXN_TRANSACTION_NOT_ALLOWED,mc_3ds2_delcine_rc) ;
				return ReturnCode;
        	 }
        	 else
        	 {
        		 strcpy(strTemp, "MC 3DS transaction failed");
        		 ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
        		                                                   "TxDataServer_Continue_With_Pin_Processing",
        		                                                    GS_MAGSTRIPE_FAILED_REPORTED,
																	mc_3ds2_delcine_rc );
        		 return ReturnCode;
        	 }
         }
         else if((0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_JCB)))
         {
				strncpy(txn_cavv, &Auth_Tx.TLF01_details.product_codes[3].amount[7],3);
				if (!strEqual (Auth_Tx.TLF01_details.security_response_code, "00"))
				{

					sprintf( strTemp, "3DS2 JCB failed to generate CAVV from CW command" );
					ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																				"TxAuthorize_Process_3D_Secure_Response_2",
																				GS_TXN_3DS2_FAILED,
																				cavv_verification_rc) ;

					ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
				}
				else if(strncmp(txn_cavv, Auth_Tx.conv_date, 3) == 0)
				{
					sprintf(strTemp, "3DS2 CAVV matches,CAVV Validation Successful. CAVV:%s,Gen CAVV:%s", txn_cavv, Auth_Tx.conv_date);
					txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );
					if( cav2_validation_flag == '1' && Auth_Tx.TLF01_details.cvc_data[0] != '\0' )
					{
							ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
							return ReturnCode ;
					}
					ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );

				}
				else
				{
					 sprintf(strTemp, "3DS2 CAVV mismatch,CAVV Validation Failed.CAVV:%s,Gen CAVV:%s",txn_cavv, Auth_Tx.conv_date);
					  txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );
					  sprintf(strTemp, "3DS2 CAVV/cvv mismatch");
					  strcpy(Auth_Tx.TLF01_details.response_code,cavv_verification_rc);
					  ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																		   "TxAuthorize_Process_3D_Secure_Response_2",
																		   GS_TXN_3DS2_FAILED,
																			Auth_Tx.TLF01_details.response_code) ;
				}
          }
		  else if((0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_CUP)))
		  {
			  strncpy(txn_cavv, &Auth_Tx.TLF01_details.product_codes[1].amount[5],3);
			   if (!strEqual (Auth_Tx.TLF01_details.security_response_code, "00"))
			   {

					   sprintf( strTemp, "3DS2 CUP failed to generate AV from CW command" );

					   ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																			"TxAuthorize_Process_3D_Secure_Response_2",
																			GS_TXN_3DS2_FAILED,
																			cup_3ds2.av_verification_rc) ;

					   ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
				 }
				 else if(strncmp(txn_cavv, Auth_Tx.conv_date, 3) == 0)
				 {
					   sprintf(strTemp, "3DS2 AV matches, AV Validation Successfull. AV:%s & Gen AV:%s", txn_cavv, Auth_Tx.conv_date);
					   txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );

					  if((cup_3ds2.cvn2_validation_flag[0] == 'Y') &&
						 (Auth_Tx.TLF01_details.resp_source[CVN2_INDICATOR_POS] == '1'))
					  {
						  memset(strTemp, 0x00, sizeof(strTemp));
						  sprintf(strTemp, "CUP: CVN2 verification indicator is set to 1");
						  txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );

						  /* Do CVN2 validation if the CVN2 verfn indicator is set to '1' */
							ReturnCode = TxAuthorize_Process_Security (NSP_CUP_ECOM_CVN2) ;
							return ReturnCode ;
					  }

					   ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );

				  }
				  else
				  {
						sprintf(strTemp, "3DS2 AV mismatch, AV Validation Failed.  AV:%s & Gen AV:%s",txn_cavv, Auth_Tx.conv_date);
						txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );
						sprintf(strTemp, "3DS2 CAVV/cvv mismatch");
						ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																			"TxAuthorize_Process_3D_Secure_Response_2",
																			GS_TXN_3DS2_FAILED,
																			 cup_3ds2.av_verification_rc) ;
				  }

		  }
         else if ((0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_AMEX)))
         {
        	 strncpy(txn_avop, &Auth_Tx.TLF01_details.dcf01_retired_cwk[7], 3);

			 if (!strEqual (Auth_Tx.TLF01_details.security_response_code, "00"))
			 {

				strcpy(Auth_Tx.TLF01_details.response_text,
														"3DS2 AMEX AEVV validation Failed");
				strcpy(strTemp, Auth_Tx.TLF01_details.response_text);
				strncpy(Auth_Tx.TLF01_details.product_codes[12].code,
															amex_3ds2.self_validation_ac, 3);

				ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
															  "TxAuthorize_Process_3D_Secure_Response_2",
															  GS_TXN_3DS2_FAILED,
															  REFER_TO_CARD_ISSUER) ;

				Auth_Tx.current_balance_sign = 0;
				ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;

			 }
			 else if(strncmp(txn_avop, Auth_Tx.conv_date, 3) == 0)
			 {
				sprintf(strTemp, "3DS2 AMEX AEVV matches, AVop_T & AVop_H is : %s", txn_avop, Auth_Tx.conv_date);
				txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );
				Auth_Tx.current_balance_sign = 1;
				if(amex_3ds2.csc_validation_flag[0] == 'Y' && (strlen(Auth_Tx.TLF01_details.interest_rate) > 0))
				{
					memset(strTemp, 0x00, sizeof(strTemp));
					sprintf(strTemp, "AMEX 3DS2: CSC data is present, perform CSC validation");
					txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );

					ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
					return ReturnCode;
				}

			 }
			 else
			 {
				 sprintf(strTemp, "3DS2 AMEX AEVV mismatch, AVop_T & AVop_H is : %s",txn_avop, Auth_Tx.conv_date);
				 txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );
				 sprintf(strTemp, "3DS2 AMEX AEVV mismatch");
				 strncpy(Auth_Tx.TLF01_details.product_codes[12].code,
				 											amex_3ds2.self_validation_ac, 3);

				 ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																		   "TxAuthorize_Process_3D_Secure_Response_2",
																		   GS_TXN_3DS2_FAILED,
																			DO_NOT_HONOR) ;
				 Auth_Tx.current_balance_sign = 0;
			 }

         }
         else if ((0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_DINERS)))
         {

        	 strncpy(txn_avop, &Auth_Tx.TLF01_details.dcf01_retired_cwk[9], 3);

			 if (!strEqual (Auth_Tx.TLF01_details.security_response_code, "00"))
			 {

				strcpy(Auth_Tx.TLF01_details.response_text,
														"3DS2 DINERS CAVV validation Failed");
				strcpy(strTemp, Auth_Tx.TLF01_details.response_text);

				ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
															  "TxAuthorize_Process_3D_Secure_Response_2",
															  GS_TXN_3DS2_FAILED,
															   DO_NOT_HONOR) ;
				strncpy(Auth_Tx.TLF01_details.product_codes[12].code,
									diners_3ds2.authentication_result_code_ac, 3);

				ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;

			 }
			 else if(strncmp(txn_avop, Auth_Tx.conv_date, 3) == 0)
			 {
				sprintf(strTemp, "3DS2 DINERS CAVV matches, AVop_T & AVop_H is : %s", txn_avop, Auth_Tx.conv_date);
				txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );

				if(diners_3ds2.cvv2_validation_flag[0] == 'Y' &&  (strlen(Auth_Tx.TLF01_details.cvc_data) > 0))
				{
					memset(strTemp, 0x00, sizeof(strTemp));
					sprintf(strTemp, "DCI 3DS2: CVV2 data is present, perform CVV2 validation");
					txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );

					ReturnCode = TxAuthorize_Process_Security (MAG_STRIP_VERIFICATION) ;
					return ReturnCode;
				}

			 }
			 else
			 {
				 sprintf(strTemp, "3DS2 DINERS CAVV mismatch, AVop_T & AVop_H is : %s",txn_avop, Auth_Tx.conv_date);
				 txcntl_log_message( 2, 1,strTemp, "TxAuthorize_Process_3D_Secure_Response_2", 1 );
				 sprintf(strTemp, "3DS2 DINERS CAVV mismatch");
				 ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																		   "TxAuthorize_Process_3D_Secure_Response_2",
																		   GS_TXN_3DS2_FAILED,
																			DO_NOT_HONOR) ;
				 strncpy(Auth_Tx.TLF01_details.product_codes[12].code,
						 	 	 	 	 	 diners_3ds2.authentication_result_code_ac, 3);
				 Auth_Tx.current_balance_sign = 0;
			 }

         }


         if ( ReturnCode == NO_AUTH_ERROR )
            /* Get the MCC Limits, then authorize the transaction. */
  		 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
      }
   }
   return ReturnCode ;

} /* TxAuthorize_Process_3D_Secure_2_Response */
/******************************************************/
BYTE TxAuthorize_Process_CNV2_For_CUP_Ecom_Response( pPTE_MSG p_msg_in )
{
   BYTE    ResultCode = 0 ;
   BYTE    ReturnCode = 0 ;
   BOOLEAN bReturnCode = true ;
   CHAR    strTemp[256] = {0} ;

   pBYTE          pATEMessageDataData = 0 ;
   pPTE_MSG_DATA  pATEMessageData     = 0 ;

   pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
   pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;

   ResultCode = ptemsg_get_result_code (p_msg_in) ;

   /***********************************************************************/
   memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
   memcpy (&Auth_Tx, (AUTH_TX *) pATEMessageDataData, sizeof Auth_Tx) ;
   /***********************************************************************/

   ReturnCode = txservice_Perform_PTE_Message_Checks (p_msg_in) ;
   if (ReturnCode == NO_AUTH_ERROR)
   {
      /* Perform semantic checks on the transaction */
      ReturnCode = txservice_Perform_Auth_Tx_Checks () ;
      if (ReturnCode == NO_AUTH_ERROR)
      {
         /* Repopulate the private ccf03p structure. */
    	 TxAuthorize_Update_Private_Ccf03p_Structure () ;

         if (!strEqual (Auth_Tx.TLF01_details.security_response_code, "00"))
         {

       		sprintf( strTemp, "CVN2 validation Failed" );
            ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                          	  	 "TxAuthorize_Process_CNV2_For_CUP_Ecom_Response",
																 GS_MAGSTRIPE_FAILED,
																 DO_NOT_HONOR) ;
            ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
         }

         if ( ReturnCode == NO_AUTH_ERROR )
         {
			 /*let us check if we have received Balance inquiry with 3rd currency and CVN2*/
			 if((Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY) &&
				((0 != strncmp( Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY, CURRENCY_LEN ))&&
				 (0 != strncmp ( Auth_Tx.TLF01_details.currency_code, USDOLLAR_CURRENCY, CURRENCY_LEN ))))
			  {
				 // We �an't get card details so we have to approve this transaction
				 strncpy(Auth_Tx.TLF01_details.response_code,CUP_ISO_APPROVED,CUP_RESPONSEP_CODE_LEN);
				 if ( Auth_Tx.TLF01_details.auth_number[0] == 0x00 )
				 {
					 /* No. Generate a numeric authorization id */
					 (void)TxAuthorize_GetAuthId( ST2_DB_SELECT_NUMERIC_AUTH_ID );
				 }
				 strcpy(Auth_Tx.TLF01_details.response_text,"APPROVED_STAND_IN");
				return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR;
			  }
			  else
			  {
				 /* Get the MCC Limits, then authorize the transaction. */
				 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
			  }
         }

      }
   }
   return ReturnCode ;

} /* TxAuthorize_Process_CNV2_For_CUP_Ecom_Response */

/******************************************************/

BYTE TxAuthorize_Process_ARQC_Response( pPTE_MSG p_msg_in )
{
   BYTE    ResultCode = 0 ;
   BYTE    ReturnCode = 0 ;
   BOOLEAN bReturnCode = true ;
   CHAR    strTemp[256] = {0} ;
   pBYTE          pATEMessageDataData = 0 ;
   pPTE_MSG_DATA  pATEMessageData     = 0 ;

   pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
   pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;

   ResultCode = ptemsg_get_result_code (p_msg_in) ;

   /***********************************************************************/
   memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
   memcpy (&Auth_Tx, (AUTH_TX *) pATEMessageDataData, sizeof Auth_Tx) ;
   /***********************************************************************/
   ReturnCode = txservice_Perform_PTE_Message_Checks (p_msg_in) ;
   if (ReturnCode == NO_AUTH_ERROR)
   {
      /* Perform semantic checks on the transaction */
      ReturnCode = txservice_Perform_Auth_Tx_Checks () ;
      if (ReturnCode == NO_AUTH_ERROR)
      {
         /* Repopulate the private ccf03p structure. */
         TxAuthorize_Update_Private_Ccf03p_Structure () ;

         if (!strEqual (Auth_Tx.TLF01_details.security_response_code, "00"))
         {
            sprintf( strTemp, "ARQC validation or ARPC generation Failed" );

            if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6)||
               0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6))
            {/* will include all ncmcrd and ncmcr3*/
            	ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                              "TxAuthorize_Process_ARQC_Response",
                                                               GS_MAGSTRIPE_FAILED,
															   MC_INC_ARQC_FAILURE) ;
            }
            else
            {
            ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                          "TxAuthorize_Process_ARQC_Response",
                                                           GS_MAGSTRIPE_FAILED,
                                                           DO_NOT_HONOR) ;
            }

			if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000")     ) &&
			   ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) &&
			   ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)  )   ||
			   (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )     ))
			 {
			   strncpy(Auth_Tx.TLF01_details.currency_code,
					   USDOLLAR_CURRENCY,
					   CURRENCY_LEN);
			 }
            ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
         }
     
         if ( ReturnCode == NO_AUTH_ERROR )
            /* Get the MCC Limits, then authorize the transaction. */
  		 ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
      }
   }
   return ReturnCode ;
	
} /* TxAuthorize_Process_ARQC_Response */

/******************************************************************************
NAME:          TxAuthorize_Process_Pin_Reply 
DESCRIPTION:   Processes response from the pin validation service

INPUTS:        p_msg_in:  pointer to a PTE_MSG structure 
OUTPUTS:       None

RETURNS:       ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ; if there was a error that
                                                       needs forwarding
               NO_AUTH_ERROR: if there was no error

AUTHOR:        Sanjoy Dasgupta
*******************************************************************************/
BYTE TxAuthorize_Process_VerifyPin_Response( pPTE_MSG p_msg_in )
{
   BYTE    ResultCode = 0 ;
   BYTE    ReturnCode = 0 ;
   BOOLEAN bReturnCode = true ;
   CHAR    strTemp[256] = {0} ;

   pBYTE          pATEMessageDataData = 0 ;
   pPTE_MSG_DATA  pATEMessageData     = 0 ;

   pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
   pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;

   ResultCode = ptemsg_get_result_code (p_msg_in) ;

   /***********************************************************************/
   memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
   memcpy (&Auth_Tx, (AUTH_TX *) pATEMessageDataData, sizeof Auth_Tx) ;
   /***********************************************************************/

   ReturnCode = txservice_Perform_PTE_Message_Checks (p_msg_in) ;
   if (ReturnCode == NO_AUTH_ERROR)
   {
      /* Perform semantic checks on the transaction */
      ReturnCode = txservice_Perform_Auth_Tx_Checks () ;
      if (ReturnCode == NO_AUTH_ERROR)
      {
         /* Repopulate the private ccf03p structure. */
         TxAuthorize_Update_Private_Ccf03p_Structure () ;

         if (!strEqual (Auth_Tx.TLF01_details.security_response_code, "00"))
         {
            private_ccf03p_struct.failed_pin_attempts += 1;

            /* Update date, else count keeps getting reset to zero.
             * Disregard comment below concerning updating the date.
             * That really only pertains to successful pins.
             */
            txservice_Set_Last_Txn_Date_To_Today();

            sprintf( strTemp,
                    "PIN mismatch #%d",
                     private_ccf03p_struct.failed_pin_attempts );

            ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                          "TxAuthorize_Process_VerifyPin_Response",
                                                           GS_TXN_INVALID_PIN,
                                                           INCORRECT_PIN) ;
            ReturnCode = PIN_ERROR ;
         }
         else
         {
            private_ccf03p_struct.failed_pin_attempts = 0 ;
         }

         /* Update CCF03P for the failed pin attempts.  However, set
          * parameter to 'false' so the date is not updated to 'today'.
          * That gets updated at the end of authorization.  Right now,
          * need to keep the date set to the date of the last activity
          * until velocity is checked.
          */
         TxAuthorize_Update_CCF03_Structure( false );

         if ( ReturnCode == NO_AUTH_ERROR )
            ReturnCode = TxDataServer_Continue_With_Pin_Processing () ;
      }
   }
   return ReturnCode ;
	
} /* TxAuthorize_Process_VerifyPin_Response */

BYTE TxAuthorize_Process_VerifyMagstripe_Response (pPTE_MSG p_msg_in) 
{
   BYTE    ResultCode        = 0 ;
   BOOLEAN bReturnCode       = true ;
   BYTE    ReturnCode        = 0 ;
   CHAR    strTemp[256]      = {0} ;
	BYTE    TransactionOrigin = 0 ;

   pBYTE	pATEMessageDataData = 0 ;
   pPTE_MSG_DATA	pATEMessageData = 0 ;

   pATEMessageData = ptemsg_get_pte_msg_data (p_msg_in) ;
   pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;

   ResultCode = ptemsg_get_result_code (p_msg_in) ;

   /***********************************************************************/
   memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
   memcpy (&Auth_Tx, (AUTH_TX *) pATEMessageDataData, sizeof Auth_Tx) ;
   /***********************************************************************/

   ReturnCode = txservice_Perform_PTE_Message_Checks (p_msg_in) ;
   if (ReturnCode != NO_AUTH_ERROR)
      return ReturnCode ;

   /* Perform semantic checks on the transaction */
   ReturnCode = txservice_Perform_Auth_Tx_Checks () ;
   if (ReturnCode != NO_AUTH_ERROR)
      return ReturnCode ;

   /* Repopulate the private ccf03p structure including memsetting it to 0 */
   TxAuthorize_Update_Private_Ccf03p_Structure () ;

	/* 071301LM - scr258 set tlf01_details.cvc according to
    * security_response_code result set in racal.
    */
   if(strEqual(Auth_Tx.TLF01_details.security_response_code, "01"))
      Auth_Tx.TLF01_details.cvc = 'Y';  /*Invalid CVC1 value*/
   else
   if(strEqual(Auth_Tx.TLF01_details.security_response_code, "02"))
      Auth_Tx.TLF01_details.cvc = 'N';  /*Invalid CVC2 value*/
   else
   if(strEqual(Auth_Tx.TLF01_details.security_response_code, "03"))
      Auth_Tx.TLF01_details.cvc = 'M';  /*Valid or matched CVC2 value*/
   else
      Auth_Tx.TLF01_details.cvc = 0x00;  /*blank */

   if(strEqual (Auth_Tx.TLF01_details.security_response_code, "55"))
   {

	   strcpy(strTemp,"Invalid PIN");
	   ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
	                                                     "Process_Magstripe_Response",
														 GS_TXN_INVALID_PIN,
	                                                      DO_NOT_HONOR );


   }
   else if ( (!strEqual (Auth_Tx.TLF01_details.security_response_code, "00")) &&
        (!strEqual (Auth_Tx.TLF01_details.security_response_code, "03")) )  /*071301LM*/
   {
      // CVV CVC CVC2 validation fails, deny transaction
      if ( (Auth_Tx.CAF01_details.cvv_processing[0] == '1') ||
           (Auth_Tx.BIN01_details.credit_subtype[0] == 'E') ||
           (Auth_Tx.BIN01_details.credit_subtype[0] == 'P')  )
      {
         if ( Auth_Tx.TLF01_details.security_response_code[1] == '2' )
         {
            strcpy( strTemp,"Decline CVV2" );
            // Check if we have converted the Currency code during Local Processing
		    if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000")     ) &&
			   ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) &&
			   ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)  )   ||
			   (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )     ))
			 {
			   strncpy(Auth_Tx.TLF01_details.currency_code,
					   USDOLLAR_CURRENCY,
					   CURRENCY_LEN);
			 }

            if (strEqual(Auth_Tx.TLF01_details.dispensation.origin, "RO"))
            {
               if ( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncv",3))
               {
                  /* Send different response code to Visa. */
                  ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED,
                                                    CVV2_FAILURE );
               }
               else if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) ||
            		   0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6))
               {
                   ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
                                                    "Process_Magstripe_Response",
                                                     GS_MAGSTRIPE_FAILED,
													 mc_3ds2_delcine_rc );
               }
               else
               {
                  ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED,
                                                    DO_NOT_HONOR );
               }
            }
            else
            {
               ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED,
                                                    CVV2_FAILURE );

               /* Make dcpiso think text came from CSF01. */
               Auth_Tx.FAF01_details.velocity[9].to_time[0] = '1';
            }
         }
         else
         {
            strcpy( strTemp, "Mag stripe verification failed" );

            if (strEqual(Auth_Tx.TLF01_details.dispensation.origin, "RO"))
            {
            	 if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) ||
            	     0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6))
			   {
				   ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
													"Process_Magstripe_Response",
													 GS_MAGSTRIPE_FAILED,
													 mc_3ds2_delcine_rc );
			   }
            	else
            	{
                    ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                        "Process_Magstripe_Response",
                                                         GS_MAGSTRIPE_FAILED,
                                                         DO_NOT_HONOR) ;
            	}
            }
            else
            {
               ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED,
                                                    RCI_SPECIAL_CONDITION) ;
            }
            // Check if we have converted the Currency code during Local Processing
			if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000")     ) &&
			   ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) &&
			   ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)  )   ||
			   (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )     ))
			 {
			   strncpy(Auth_Tx.TLF01_details.currency_code,
					   USDOLLAR_CURRENCY,
					   CURRENCY_LEN);
			 }
         }
         return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
      }

      // CVV CVC CVC2 validation fails, deny transaction and report
      else if (strEqual(Auth_Tx.CAF01_details.cvv_processing, "2"))
      {
         if ( Auth_Tx.TLF01_details.security_response_code[1] == '2' )
         {
            strcpy( strTemp,"Decline CVV2" );
            // Check if we have converted the Currency code during Local Processing
		   if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000")     ) &&
			   ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) &&
			   ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)  )   ||
			   (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )     ))
			 {
			   strncpy(Auth_Tx.TLF01_details.currency_code,
					   USDOLLAR_CURRENCY,
					   CURRENCY_LEN);
			 }

            if (strEqual(Auth_Tx.TLF01_details.dispensation.origin, "RO"))
            {
               if ( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncv",3))
               {
                  /* Send different response code to Visa. */
                  ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED_REPORTED,
                                                    CVV2_FAILURE );
               }
               else if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) ||
            		   0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6))
               {
                  ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED_REPORTED,
													mc_3ds2_delcine_rc );
               }
               else
               {
                  ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED_REPORTED,
            	                                          DO_NOT_HONOR );

               }
            }
            else
            {
               ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED_REPORTED,
                                                    CVV2_FAILURE );

               /* Make dcpiso think text came from CSF01. */
               Auth_Tx.FAF01_details.velocity[9].to_time[0] = '1';
            }
         }
         else
         {
            if(Auth_Tx.TLF01_details.response_text[0] != NULL)
				strcpy( strTemp, Auth_Tx.TLF01_details.response_text );
			else
				strcpy( strTemp, "Mag stripe failed and reported" );

            if (strEqual(Auth_Tx.TLF01_details.dispensation.origin, "RO") || Auth_Tx.TLF01_details.response_text[0] != NULL)
            {
            	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) ||
            	   0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6))
			    {
            		 ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
												   "Process_Magstripe_Response",
													GS_MAGSTRIPE_FAILED_REPORTED,
													mc_3ds2_delcine_rc );
			    }
            	else
            	{
                    ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                        "Process_Magstripe_Response",
                                                         GS_MAGSTRIPE_FAILED_REPORTED,
                                                         DO_NOT_HONOR) ;
            	}
            }
            else
            {
                ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                    "Process_Magstripe_Response",
                                                     GS_MAGSTRIPE_FAILED_REPORTED,
                                                     RCI_SPECIAL_CONDITION) ;
            }

			if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000")     ) &&
			   ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) &&
			   ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)  )   ||
			   (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )     ))
			 {
			   strncpy(Auth_Tx.TLF01_details.currency_code,
					   USDOLLAR_CURRENCY,
					   CURRENCY_LEN);
			 }
         }
         return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
      }

      // CVV CVC CVC2 validation fails, APPROVE transaction and report
      else if (strEqual(Auth_Tx.CAF01_details.cvv_processing, "3"))
      {
         if ( Auth_Tx.TLF01_details.security_response_code[1] == '2' )
            strcpy (strTemp,"CVV2 failed and reported APPROVED") ;
         else
            strcpy (strTemp,"Mag stripe failed and reported APPROVED") ;

         ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                         "Process_Magstripe_Response",
                                          GS_MAGSTRIPE_FAILED_REPORTED_APPROVED,
                                          TRANSACTION_APPROVED) ;


         /* DO NOT RETURN ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ; */
      }

      // CVV CVC CVC2 validation fails and .cvv_processing is not valid
      else
      {
         if ( Auth_Tx.TLF01_details.security_response_code[1] == '2' )
         {
            strcpy( strTemp,"Decline CVV2" );
            // Check if we have converted the Currency code during Local Processing
		   if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000")     ) &&
			   ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) &&
			   ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)  )   ||
			   (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )     ))
			 {
			   strncpy(Auth_Tx.TLF01_details.currency_code,
					   USDOLLAR_CURRENCY,
					   CURRENCY_LEN);
			 }

            if (strEqual(Auth_Tx.TLF01_details.dispensation.origin, "RO"))
            {
               if ( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncv",3))
               {
                  /* Send different response code to Visa. */
                  ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED,
                                                    CVV2_FAILURE );
               }
               else  if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) ||
            		   0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6))
               {
            	   ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
            			   	   	   	   	   	   	   	   	   	   	   	   "Process_Magstripe_Response",
																	   GS_MAGSTRIPE_FAILED_REPORTED,
																	   mc_3ds2_delcine_rc) ;
               }
               else
               {
            	   ReturnCode = txservice_Generate_Usage_Error_Message(2,strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED,
                                                    DO_NOT_HONOR );
               }
            }
            else
            {
               ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED,
                                                    CVV2_FAILURE );

               /* Make dcpiso think text came from CSF01. */
               Auth_Tx.FAF01_details.velocity[9].to_time[0] = '1';
            }
         }
         else
         {
            strcpy( strTemp, "MagStripe fail: CVV Processing is BLANK" );

            if (strEqual(Auth_Tx.TLF01_details.dispensation.origin, "RO"))
            {
            	if(0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) ||
            	   0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6))
			   {
				   ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
																	   "Process_Magstripe_Response",
																	   GS_MAGSTRIPE_FAILED_REPORTED,
																	   mc_3ds2_delcine_rc) ;
			   }
               else
               {
            	   ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                   "Process_Magstripe_Response",
                                                    GS_MAGSTRIPE_FAILED,
                                                    DO_NOT_HONOR) ;
            	}
            }
            else
            {
                ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                                    "Process_Magstripe_Response",
                                                     GS_MAGSTRIPE_FAILED,
                                                     RCI_SPECIAL_CONDITION) ;
            }

			if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000")     ) &&
			   ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) &&
			   ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)  )   ||
			   (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )     ))
			 {
			   strncpy(Auth_Tx.TLF01_details.currency_code,
					   USDOLLAR_CURRENCY,
					   CURRENCY_LEN);
			 }
         }
         return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
      }
   }

   /* Check for a Visa Electron card. */
   if ( (Auth_Tx.BIN01_details.credit_subtype[0] == 'E') ||
        (Auth_Tx.BIN01_details.credit_subtype[0] == 'P')  )
   {
      Auth_Tx.effective_date[0] = 1;  /* CVV has been checked. */

      /* Perform velocity checks. */
      ReturnCode = TxAuthorize_Perform_Merchant_Velocity_Checks () ;
      if (ReturnCode != NO_AUTH_ERROR)
	     return ReturnCode ;

      if ( Auth_Tx.CAF01_details.destination_name[0] != 0x00 )
      {
         ReturnCode = 
               txservice_Process_Bin_Range_Switch_Out (BIN01_DATA, 
                                   Auth_Tx.CAF01_details.destination_name,
                                   Auth_Tx.BIN01_details.alt_destination_name) ;
      }
      else
      {
         ReturnCode = 
            txservice_Process_Bin_Range_Switch_Out(BIN01_DATA, 
                              Auth_Tx.BIN01_details.destination_name,
                              Auth_Tx.BIN01_details.alt_destination_name);
      }
      return( ReturnCode );
   }

   if (Auth_Tx.TLF01_details.tx_key == AUTH_BALANCE_INQUIRY)
   {
      txservice_Prepare_AuthTx_For_Forwarding (GS_TXN_COMPLETED, TRANSACTION_APPROVED) ;
      if ( Auth_Tx.TLF01_details.auth_number[0] == 0x00 )
      {
         (void)TxAuthorize_GetAuthId( ST2_NONE );
         TxDataServer_DBReply_Continue();
      }
      else
      {
         /* All done, so send response back to originator. */
         txservice_Forward_Auth_Tx_To_Originator(Auth_Tx.TLF01_details.handler_queue,
                                                 MT_AUTH_RESPONSE, 0, 0);
      }

      return NO_AUTH_ERROR ;
   }

   /* Get the MCC Limits, then authorize the transaction. */
   ReturnCode = TxDataServer_Get_MCC_Limit_Details( ONUS );
   return ReturnCode ;

} /* TxAuthorize_Process_Magstripe_Response */


BYTE TxAuthorize_Perform_Merchant_Velocity_Checks (void) 
{
   BYTE ReturnCode = NO_AUTH_ERROR ;
   double nTotalAmount = 0.0 ;

   nTotalAmount = 
      (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0) ;

   switch (Auth_Tx.TLF01_details.tx_key)
   {
      case AUTH_SALE:
      case AUTH_QUASI_CASH:
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION:
      case AUTH_RELOAD:
      case AUTH_RELOAD_REPEAT:
      case AUTH_RELOAD_CONFIRM:
         ReturnCode = 
            TxAuthorize_Perform_Merchant_Max_Credit_Amt (nTotalAmount);
         return ReturnCode ;
      break ;
      case AUTH_PRE_AUTHORIZATION:
      case AUTH_AUTHORIZATION:
    	//Check Auth amt for fleet card transactions and voice trans as well

    		  ReturnCode =
    					TxAuthorize_Perform_Merchant_Max_Auth_Amt (nTotalAmount);
    		  return ReturnCode;


		  break;

      case AUTH_CASH_ADVANCE:
      case AUTH_CASH_ADVANCE_AUTHORIZATION:
         ReturnCode = 
            TxAuthorize_Perform_Merchant_Max_CashAdvance_Amt (nTotalAmount) ;
         return ReturnCode ;
      break ;

      case AUTH_REFUND:
         ReturnCode = 
            TxAuthorize_Perform_Merchant_Max_Return_Amt (nTotalAmount) ;
         return ReturnCode ;
      break ;
   } // switch
   return NO_AUTH_ERROR ;
} /* TxAuthorize_Perform_Merchant_Velocity_Checks */

BYTE TxAuthorize_Check_Expiry_Date (void)
{
   CHAR strTemp[256]        = {0};
   CHAR strReversedDate[10] = {0};
   BYTE ReturnCode          = NO_AUTH_ERROR;
   BYTE date_check;

 if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
  {
    return NO_AUTH_ERROR ; /* Skip exp date checks for RPS tx's*/
  }
	 
   date_check = Auth_Tx.CAF01_details.expiration_date_check[0];
   if ((CHECK_EXPIRY_YES == date_check) || (CHECK_EXPIRY_EXPANDED == date_check))
   {
      if (EXPIRATION_DATE_FROM_CARDHOLDER == Auth_Tx.CAF01_details.expiration_date_location[0])
      {
         //ReturnCode = TxAuthorize_Validate_Expiry_Date(Auth_Tx.CCF02P_details.expire_date);
		 /*  TF-Ajay additional expiry date checking  changes 24 Oct, 2010
				 if CCF02P_details.expire_date fails then check the additional expiry date which is present 
				 in the CCF02P_details.maint_Date feild*/
		ReturnCode =TxAuthorize_Validate_Primary_Expiry_Date_And_Secondary_Expiry_Date (Auth_Tx.CCF02P_details.expire_date,Auth_Tx.CCF02P_details.maint_date);
		
			
      }
      else if (EXPIRATION_DATE_FROM_CARD == Auth_Tx.CAF01_details.expiration_date_location[0])
      {
         ReturnCode = TxAuthorize_Validate_Expiry_Date (Auth_Tx.TLF01_details.exp_date);
      }
      else
      {
         ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
         if (EXPIRATION_DATE_FROM_BOTH == Auth_Tx.CAF01_details.expiration_date_location[0])
         {
            /* Normal Expiry Check - Track2 same as CCF02P and >= system date. */
            if((strEqual(Auth_Tx.TLF01_details.exp_date, Auth_Tx.CCF02P_details.expire_date)) ||
			   (strEqual(Auth_Tx.TLF01_details.exp_date, Auth_Tx.CCF02P_details.maint_date)))
            {
               //ReturnCode = TxAuthorize_Validate_Expiry_Date(Auth_Tx.CCF02P_details.expire_date);
			   /*  TF-Ajay additional expiry date checking  changes 24 Oct, 2010
				 if CCF02P_details.expire_date fails then check the additional expiry date which is present 
				 in the CCF02P_details.maint_Date feild*/
				ReturnCode =TxAuthorize_Validate_Primary_Expiry_Date_And_Secondary_Expiry_Date (Auth_Tx.CCF02P_details.expire_date,Auth_Tx.CCF02P_details.maint_date);
			
            }
            else if ( date_check == CHECK_EXPIRY_EXPANDED )
            {
               /* Expanded Expiry Check:
                *    Track2 >= system date.
                *    Track2 Month same as CCF02P Month
                *    Track2 Year <= CCF02P Year
                */
               ReturnCode = TxAuthorize_Validate_Expiry_Date(Auth_Tx.TLF01_details.exp_date);
               if ( ReturnCode == NO_AUTH_ERROR )
               {
                  if ( 0 != strcmp(Auth_Tx.CCF02P_details.expire_date+2,
                                   Auth_Tx.TLF01_details.exp_date+2) )
                  {
                     /* Months are the same. */
                     ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
                     sprintf( strTemp,
                             "Invalid Expiry Month:Card(%s) DB(%s)",
                              Auth_Tx.TLF01_details.exp_date,
                              Auth_Tx.CCF02P_details.expire_date );
                  }
                  else if ( 0 < strncmp(Auth_Tx.TLF01_details.exp_date,
                                        Auth_Tx.CCF02P_details.expire_date, 2) )
                  {
                     /* Year on card is greater than year on CCF02P. */
                     ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
                     sprintf( strTemp,
                             "Invalid Expiry Year: Card(%s) DB(%s)",
                              Auth_Tx.TLF01_details.exp_date,
                              Auth_Tx.CCF02P_details.expire_date );
                  }
               }
            }
            else
            {
               sprintf( strTemp,
                       "Dif Expiry Dates: Card(%s) Primary Exp. Date(%s) Sec. Exp. Date(%s)",
                        Auth_Tx.TLF01_details.exp_date,
                        Auth_Tx.CCF02P_details.expire_date,
						 Auth_Tx.CCF02P_details.maint_date);
               // Check if we have converted the Currency code during Local Processing
			   if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000")     ) &&
				   ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) &&
				   ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)  )   ||
				   (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )     ))
				 {
				   strncpy(Auth_Tx.TLF01_details.currency_code,
						   USDOLLAR_CURRENCY,
						   CURRENCY_LEN);
				 }

            }

            if ( ReturnCode != NO_AUTH_ERROR )
            {
               if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
                  strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;

               txcntl_log_message( 1, 3, strTemp, "TxAuthorize_Check_Expiry_Date", 1 );
               if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
               {
                  strcpy(Auth_Tx.TLF01_details.response_text,O3_INV_EXP_DATE);
               }

               txservice_Prepare_AuthTx_For_Forwarding (
                                    GS_TXN_INVALID_EXPIRY_DATE_LOCATION,
                                    EXPIRED_CARD) ;
            }
         }
         else if (EXPIRATION_DATE_FROM_BOTH_REVERSED == Auth_Tx.CAF01_details.expiration_date_location[0])
         {
            strncpy( strReversedDate, Auth_Tx.TLF01_details.exp_date + 2, 2 );
            strncat( strReversedDate, Auth_Tx.TLF01_details.exp_date,     2 );

            if (strEqual(Auth_Tx.TLF01_details.exp_date, Auth_Tx.CCF02P_details.expire_date) ||
                strEqual(strReversedDate, Auth_Tx.CCF02P_details.expire_date) ||
				strEqual(Auth_Tx.TLF01_details.exp_date, Auth_Tx.CCF02P_details.maint_date)||
				strEqual(strReversedDate, Auth_Tx.CCF02P_details.maint_date))
            {
              // ReturnCode = TxAuthorize_Validate_Expiry_Date(Auth_Tx.CCF02P_details.expire_date);
			   /*  TF-Ajay additional expiry date checking  changes 24 Oct, 2010
				 if CCF02P_details.expire_date fails then check the additional expiry date which is present 
				 in the CCF02P_details.maint_Date feild*/
					ReturnCode =TxAuthorize_Validate_Primary_Expiry_Date_And_Secondary_Expiry_Date (Auth_Tx.CCF02P_details.expire_date,Auth_Tx.CCF02P_details.maint_date);
			
            }
            else if ( date_check == CHECK_EXPIRY_EXPANDED )
            {
               /* Expanded Expiry Check:
                *    Track2 >= system date.
                *    Track2 Month same as CCF02P Month
                *    Track2 Year <= CCF02P Year
                */
               ReturnCode = TxAuthorize_Validate_Expiry_Date(strReversedDate);
               if ( ReturnCode == NO_AUTH_ERROR )
               {
                  if ( 0 != strcmp(Auth_Tx.CCF02P_details.expire_date+2,
                                   strReversedDate+2) )
                  {
                     /* Months are the same. */
                     ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
                     sprintf( strTemp,
                             "Invalid Expiry Month:Card(%s) DB(%s)",
                              Auth_Tx.TLF01_details.exp_date,
                              Auth_Tx.CCF02P_details.expire_date );
                  }
                  else if ( 0 < strncmp(strReversedDate,
                                        Auth_Tx.CCF02P_details.expire_date, 2) )
                  {
                     /* Year on card is greater than year on CCF02P. */
                     ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
                     sprintf( strTemp,
                             "Invalid Expiry Year: Card(%s) DB(%s)",
                              Auth_Tx.TLF01_details.exp_date,
                              Auth_Tx.CCF02P_details.expire_date );
                  }
               }
            }
            else
            {
               sprintf( strTemp,
                       "Dif Expiry Dates: Card(%s) Primary Exp. Date(%s) Sec. Exp. Date(%s)",
                        Auth_Tx.TLF01_details.exp_date,
                        Auth_Tx.CCF02P_details.expire_date,
						Auth_Tx.CCF02P_details.maint_date);
               // Check if we have converted the Currency code during Local Processing
               if ( ( strEqual(Auth_Tx.TLF01_details.host_start_time, "0.000")     ) &&
                    ( strEqual(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY) ) &&
                    ((strEqual(Auth_Tx.BIN01_details.card_family, CARD_JCB)  )   ||
                    (strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )     ))
                  {
            	   	   strncpy(Auth_Tx.TLF01_details.currency_code,
            	   			   USDOLLAR_CURRENCY,
							   CURRENCY_LEN);
                  }

            }

            if ( ReturnCode != NO_AUTH_ERROR )
            {
               if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
                   strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;

               txcntl_log_message( 1, 3, strTemp, "TxAuthorize_Check_Expiry_Date", 1 );
               if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
               {
                  strcpy(Auth_Tx.TLF01_details.response_text,O3_INV_EXP_DATE);
               }

               txservice_Prepare_AuthTx_For_Forwarding (
                                    GS_TXN_INVALID_EXPIRY_DATE_LOCATION,
                                    EXPIRED_CARD) ;
               ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
            }
         }
         else
         {
            strcpy (strTemp,"Invalid Expiration Date on Card Track\n") ; 
            if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
               strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;

            txcntl_log_message( 1, 3, strTemp, "TxAuthorize_Check_Expiry_Date", 1 );
            txservice_Prepare_AuthTx_For_Forwarding (
                                    GS_TXN_INVALID_EXPIRY_DATE_LOCATION,
                                    REFER_TO_CARD_ISSUER) ;
            ReturnCode = ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
         }
      }
   }

   return( ReturnCode );
} /* TxAuthorize_Check_Expiry_Date */

/***********************************************************************

NAME:         TxAuthorize_Validate_Expiry_Date

DESCRIPTION:  Determines if a date is EXPIRED, VALID or INVALID (20/20 format)
              The algorithm uses a +/- 20 factor
              Any date less than 20 years before the system date or 
              more than 20 years from the system date is treated as INVALID

              The maximum allowed card date is (system year + 20) and
              card month = system month

INPUTS:       strExpiryDate - Pointer to the date string in the format "YYMM"

OUTPUTS:      None

RETURNS:      false  if the date is EXPIRED or INVALID
              true   if date is VALID

AUTHOR:       Sanjoy Dasgupta

*************************************************************************/
BYTE TxAuthorize_Validate_Expiry_Date (BYTE* strExpiryDate)
{
   /* The Expiry date has to be in the format YYMM */
   BYTE strSystemDateTime[50] = {0} ;
   BYTE strTemp[256] = {0} ;
   BYTE ValidExpiryDate = true ;
   BYTE ReturnCode = 0 ;

   INT nSystemYY = 0 ;        // 00 to 99
   INT nSystemMM = 0 ;        // 00 to 12
   INT nCardYY   = 0 ;        // 00 to 99
   INT nCardMM   = 0 ;        // 00 to 12

   INT nCardRangeLowYY  = 0 ;
   INT nCardRangeHighYY = 0 ;
	
	 if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
      {
        return NO_AUTH_ERROR ; /* Skip exp date checks for RPS tx's*/
      }
   if (strlen(strExpiryDate) == 0)
   {
      strcpy (strTemp, "Invalid Expiry Date-Date cannot be NULL") ;

      ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                        "TxAuthorize_Validate_Expiry_Date",
                                         GS_TXN_INVALID_EXPIRY_DATE,
                                         EXPIRED_CARD );

      if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
      {
         strcpy(Auth_Tx.TLF01_details.response_text,O3_NO_EXP_DATE);
      }
      return ReturnCode ;
   }

   /* Get the System Date in the format yyyymmdd */
   genutil_format_date (strSystemDateTime) ;

   /* Get the month portion of the expiry date (yymm) */
   nCardMM = atoi(strExpiryDate + 2) ;

   /* Get the year portion of the expiry date (yymm) */
   memcpy (strTemp, strExpiryDate, 2) ; 
   nCardYY = atoi(strTemp) ;

   /* Get the year portion of the expiry date (yyyymmdd) */
   /* If the current year is 1999, nSystemYY will be 99 and if 2000, nSystemYY will be 0 */
   memcpy (strTemp, strSystemDateTime, 4) ;
   nSystemYY = atoi(strTemp) % 100 ;

   /* Get the month portion of the expiry date (yyyymmdd) */
   memset (strTemp, 0, sizeof strTemp) ;
   memcpy (strTemp, strSystemDateTime+4, 2) ;
   nSystemMM = atoi(strTemp) ;

   /* System Date between 2000 and 2020 */
   if (nSystemYY >= 0 && nSystemYY <= 20)
      nSystemYY += 100 ;

   if (nSystemYY >= 80 && nSystemYY <= 120 && nCardYY >= 0 && nCardYY <= 40)
      nCardYY += 100 ;

   /* Define the boundary for the +/- 20 year check */
   nCardRangeLowYY = nSystemYY - 20 ;
   nCardRangeHighYY = nSystemYY + 20 ;

   /* Date is invalid if date on card is less than 20 years from system date
      or more than 20 years from the system date */
   if (nCardYY < nCardRangeLowYY || nCardYY > nCardRangeHighYY)
   {
      ValidExpiryDate = false ; /* Outside the window - INVALID CARD */
   }

   /* If the card year is same as the boundary year, check the month */
   else if (nCardYY == nCardRangeLowYY && (nCardMM < nSystemMM))
   {
      ValidExpiryDate = false ; /* INVALID CARD */
   }

   /* If the card year is same as the boundary year, check the month */
   else if (nCardYY == nCardRangeHighYY && (nCardMM > nSystemMM))
   {
      /* card month is greater than system month */
      ValidExpiryDate = false ; /* INVALID CARD */
   }

   /* All cards within the +/- 20 year boundary and with years less than the system
   date are termed EXPIRED (these are not INVALID cards). For our purposes all INVALID and
   EXPIRED cards are treated as INVALID */

   if (ValidExpiryDate) /* At this time, all cards are within the +/- 20 year boundary */
   {
      if (nCardYY < nSystemYY)
      {
         ValidExpiryDate = false ; /* EXPIRED CARD */
      }
      else if (nCardYY == nSystemYY && (nCardMM < nSystemMM))
      {
         ValidExpiryDate = false ; /* EXPIRED CARD */
      }
   }

   if (ValidExpiryDate == false)
   {
      sprintf( strTemp, "Invalid Expiry Date (%s)", strExpiryDate );
      ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                            "Validate_Expiry_Date",
                                             GS_TXN_INVALID_EXPIRY_DATE,
                                             EXPIRED_CARD) ;

      if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
      {
         strcpy(Auth_Tx.TLF01_details.response_text,O3_EXPD_CARD);
      }
      return ReturnCode ;
   }
   /* At this stage, all cards are valid */
   return NO_AUTH_ERROR ;
} /* TxAuthorize_Validate_Expiry_Date */

/***********************************************************************

NAME:         TxAuthorize_Validate_Primary_Expiry_Date_And_Secondary_Expiry_Date

DESCRIPTION:  Determines if a date is EXPIRED, VALID or INVALID (20/20 format)
              The algorithm uses a +/- 20 factor
              Any date less than 20 years before the system date or 
              more than 20 years from the system date is treated as INVALID

              The maximum allowed card date is (system year + 20) and
              card month = system month

INPUTS:       strPrimaryExpiryDate - Pointer to the date string in the format "YYMM"
			  strSecondaryExpiryDate - Pointer to the date string in the format "YYMM"

OUTPUTS:      None

RETURNS:      false  if the both date is EXPIRED or INVALID
              true   if any one date is VALID

AUTHOR:       Ajay ThoughtFocus

*************************************************************************/
BYTE TxAuthorize_Validate_Primary_Expiry_Date_And_Secondary_Expiry_Date (BYTE* strPrimaryExpiryDate, BYTE* strSecondaryExpiryDate)
{
   /* The Expiry date has to be in the format YYMM */
   BYTE strSystemDateTime[50] = {0} ;
   BYTE strTemp[256] = {0} ;
   BYTE PrimaryValidExpiryDate = true ;
   BYTE SecondaryValidExpiryDate=true;

   BYTE ReturnCode = 0 ;

   INT nSystemYY = 0 ;        // 00 to 99
   INT nSystemMM = 0 ;        // 00 to 12
   INT nCardYY   = 0 ;        // 00 to 99
   INT nCardMM   = 0 ;        // 00 to 12

   INT nCardRangeLowYY  = 0 ;
   INT nCardRangeHighYY = 0 ;

   if(SecondaryExpiryDateValidationFlag==1)
   {
	   if ((strlen(strPrimaryExpiryDate) == 0)&&(strlen(strSecondaryExpiryDate)==0))
	   {
		   strcpy (strTemp, "Invalid Expiry Date-Both Dates cannot be NULL") ;

		   ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                        "TxAuthorize_Validate_Primary_Expiry_Date_And_Secondary_Expiry_Date",
                                         GS_TXN_INVALID_EXPIRY_DATE,
                                         EXPIRED_CARD );

		   if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
		   {
			   strcpy(Auth_Tx.TLF01_details.response_text,O3_NO_EXP_DATE);
		   }
		   return ReturnCode ;
	   }
   }
   else
   {
	   if ((strlen(strPrimaryExpiryDate) == 0))
	   {
		   strcpy (strTemp, "Primary expiry Date cannot be NULL") ;

		   ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                        "TxAuthorize_Validate_Primary_Expiry_Date_And_Secondary_Expiry_Date",
                                         GS_TXN_INVALID_EXPIRY_DATE,
                                         EXPIRED_CARD );

		   if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
		   {
			   strcpy(Auth_Tx.TLF01_details.response_text,O3_NO_EXP_DATE);
		   }
		   return ReturnCode ;
	   }

   }

   /* Get the System Date in the format yyyymmdd */
   genutil_format_date (strSystemDateTime) ;

   /* Get the month portion of the expiry date (yymm) */
   nCardMM = atoi(strPrimaryExpiryDate + 2) ;

   /* Get the year portion of the expiry date (yymm) */
   memcpy (strTemp, strPrimaryExpiryDate, 2) ; 
   nCardYY = atoi(strTemp) ;

   /* Get the year portion of the expiry date (yyyymmdd) */
   /* If the current year is 1999, nSystemYY will be 99 and if 2000, nSystemYY will be 0 */
   memcpy (strTemp, strSystemDateTime, 4) ;
   nSystemYY = atoi(strTemp) % 100 ;

   /* Get the month portion of the expiry date (yyyymmdd) */
   memset (strTemp, 0, sizeof strTemp) ;
   memcpy (strTemp, strSystemDateTime+4, 2) ;
   nSystemMM = atoi(strTemp) ;

   /* System Date between 2000 and 2020 */
   if (nSystemYY >= 0 && nSystemYY <= 20)
      nSystemYY += 100 ;

   if (nSystemYY >= 80 && nSystemYY <= 120 && nCardYY >= 0 && nCardYY <= 40)
      nCardYY += 100 ;

   /* Define the boundary for the +/- 20 year check */
   nCardRangeLowYY = nSystemYY - 20 ;
   nCardRangeHighYY = nSystemYY + 20 ;

   /* Date is invalid if date on card is less than 20 years from system date
      or more than 20 years from the system date */
   if (nCardYY < nCardRangeLowYY || nCardYY > nCardRangeHighYY)
   {
      PrimaryValidExpiryDate = false ; /* Outside the window - INVALID CARD */
   }

   /* If the card year is same as the boundary year, check the month */
   else if (nCardYY == nCardRangeLowYY && (nCardMM < nSystemMM))
   {
      PrimaryValidExpiryDate = false ; /* INVALID CARD */
   }

   /* If the card year is same as the boundary year, check the month */
   else if (nCardYY == nCardRangeHighYY && (nCardMM > nSystemMM))
   {
      /* card month is greater than system month */
      PrimaryValidExpiryDate = false ; /* INVALID CARD */
   }

   /* All cards within the +/- 20 year boundary and with years less than the system
   date are termed EXPIRED (these are not INVALID cards). For our purposes all INVALID and
   EXPIRED cards are treated as INVALID */

   if (PrimaryValidExpiryDate) /* At this time, all cards are within the +/- 20 year boundary */
   {
      if (nCardYY < nSystemYY)
      {
         PrimaryValidExpiryDate = false ; /* EXPIRED CARD */
      }
      else if (nCardYY == nSystemYY && (nCardMM < nSystemMM))
      {
         PrimaryValidExpiryDate = false ; /* EXPIRED CARD */
      }
   }

   if(PrimaryValidExpiryDate==false && SecondaryExpiryDateValidationFlag==1)
   {
	      /* Get the month portion of the expiry date (yymm) */
	nCardMM=0;
   nCardMM = atoi(strSecondaryExpiryDate + 2) ;

   /* Get the year portion of the expiry date (yymm) */
   memcpy (strTemp, strSecondaryExpiryDate, 2) ; 
   nCardYY = atoi(strTemp) ;

   /* Get the year portion of the expiry date (yyyymmdd) */
   /* If the current year is 1999, nSystemYY will be 99 and if 2000, nSystemYY will be 0 */
   memcpy (strTemp, strSystemDateTime, 4) ;
   nSystemYY = atoi(strTemp) % 100 ;

   /* Get the month portion of the expiry date (yyyymmdd) */
   memset (strTemp, 0, sizeof strTemp) ;
   memcpy (strTemp, strSystemDateTime+4, 2) ;
   nSystemMM = atoi(strTemp) ;

   /* System Date between 2000 and 2020 */
   if (nSystemYY >= 0 && nSystemYY <= 20)
      nSystemYY += 100 ;

   if (nSystemYY >= 80 && nSystemYY <= 120 && nCardYY >= 0 && nCardYY <= 40)
      nCardYY += 100 ;

   /* Define the boundary for the +/- 20 year check */
   nCardRangeLowYY = nSystemYY - 20 ;
   nCardRangeHighYY = nSystemYY + 20 ;

   /* Date is invalid if date on card is less than 20 years from system date
      or more than 20 years from the system date */
   if (nCardYY < nCardRangeLowYY || nCardYY > nCardRangeHighYY)
   {
      SecondaryValidExpiryDate = false ; /* Outside the window - INVALID CARD */
   }

   /* If the card year is same as the boundary year, check the month */
   else if (nCardYY == nCardRangeLowYY && (nCardMM < nSystemMM))
   {
      SecondaryValidExpiryDate = false ; /* INVALID CARD */
   }

   /* If the card year is same as the boundary year, check the month */
   else if (nCardYY == nCardRangeHighYY && (nCardMM > nSystemMM))
   {
      /* card month is greater than system month */
      SecondaryValidExpiryDate = false ; /* INVALID CARD */
   }

   /* All cards within the +/- 20 year boundary and with years less than the system
   date are termed EXPIRED (these are not INVALID cards). For our purposes all INVALID and
   EXPIRED cards are treated as INVALID */

   if (SecondaryValidExpiryDate) /* At this time, all cards are within the +/- 20 year boundary */
   {
      if (nCardYY < nSystemYY)
      {
         SecondaryValidExpiryDate = false ; /* EXPIRED CARD */
      }
      else if (nCardYY == nSystemYY && (nCardMM < nSystemMM))
      {
         SecondaryValidExpiryDate = false ; /* EXPIRED CARD */
      }
   }


   }

   if(SecondaryExpiryDateValidationFlag==1)
   {
	   if((PrimaryValidExpiryDate == false)&&(SecondaryValidExpiryDate==false))
	   {
		   sprintf( strTemp, "Invalid Primary Expiry Date (%s) and Secondary Expiry Date (%s)", strPrimaryExpiryDate,strSecondaryExpiryDate );
		   ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                            "Validate_Expiry_Date",
                                             GS_TXN_INVALID_EXPIRY_DATE,
                                             EXPIRED_CARD) ;

		   if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
		   {
			   strcpy(Auth_Tx.TLF01_details.response_text,O3_EXPD_CARD);
		   }
		   return ReturnCode ;
	   }
   }
   else
   {
	   if((PrimaryValidExpiryDate == false))
	   {
		   sprintf( strTemp, "Invalid Primary Expiry Date (%s)", strPrimaryExpiryDate);
		   ReturnCode = txservice_Generate_Usage_Error_Message (2, strTemp,
                                            "Validate_Expiry_Date",
                                             GS_TXN_INVALID_EXPIRY_DATE,
                                             EXPIRED_CARD) ;

		   if(Auth_Tx.MCF01_details.rps_merchant[0] =='Y')
		   {
			   strcpy(Auth_Tx.TLF01_details.response_text,O3_EXPD_CARD);
		   }
		   return ReturnCode ;
	   }


   }
   /* At this stage, all cards are valid */
   return NO_AUTH_ERROR ;
} /* TxAuthorize_Validate_Primary_Expiry_Date_And_Secondary_Expiry_Date */

BYTE TxAuthorize_Validate_Deferred_Amounts 
               (char* deferred_min_amount, char* deferred_max_amount)
{
	double nConvertedTxAmount = 0.0 ;
	double nDeferredMaxAmount = 0.0 ;
	double nDeferredMinAmount = 0.0 ;
	char strTemp[256] = {0} ;
	
	if (txservice_If_Tx_Is_Treated_Like_A_Reversal ())
	    nConvertedTxAmount = (strtod (Auth_Tx.TLF01_details.reversal_amount, 0) / 100.0) ;
    else
        nConvertedTxAmount = (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0) ;
	/* If amount is > than the deferred_max_amount, then it is an error */
	nDeferredMaxAmount = strtod (deferred_max_amount, 0) /100.0 ;
	nDeferredMinAmount = strtod (deferred_min_amount, 0) /100.0;

	if (nConvertedTxAmount > nDeferredMaxAmount)
	{
		sprintf (strTemp,"DefAmt:%-9.2f exceeds MaxAmt:%-9.2f", 
					nConvertedTxAmount, nDeferredMaxAmount) ;
        if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
            strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;

		 txcntl_log_message( 1, 3, strTemp, "TxAuthorize_Validate_Deferred_Amounts", 1 );
		txservice_Prepare_AuthTx_For_Forwarding 
			(GS_TXN_ACF01_BALANCE_ACCOUNT_LIMIT_FAILED,REFER_TO_CARD_ISSUER) ;	

		
		return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
	}

	if (nConvertedTxAmount < nDeferredMinAmount)
	{
		sprintf (strTemp,"DefAmt:%-9.2f lessthan MinAmt:%-9.2f", 
				nConvertedTxAmount, nDeferredMinAmount) ;
        if (strEqual (Auth_Tx.TLF01_details.handler_queue, "VOICE"))
            strcpy (Auth_Tx.TLF01_details.voice_auth_text, strTemp) ;

		 txcntl_log_message( 1, 3, strTemp, "TxAuthorize_Validate_Deferred_Amounts", 1 );
		txservice_Prepare_AuthTx_For_Forwarding 
			(GS_TXN_ACF01_BALANCE_ACCOUNT_LIMIT_FAILED,REFER_TO_CARD_ISSUER) ;	
		return ERROR_BUT_FORWARD_TO_UPDATE_PROCESSOR ;
	}
	return NO_AUTH_ERROR ;
	
} /* TxAuthorize_Validate_Deferred_Amounts */

BYTE TxAuthorize_Update_CCF03_Structure( INT update_date )
{
   BYTE  strTemp[50];
   BYTE  strAmount[50] = {0} ;
   INT   nIndex = 0 ;

   /* Update the date only if the input indicates it. */
   if ( update_date == true )
   {
      memset( strTemp, 0x00, sizeof(strTemp) );
      ltoa( private_ccf03p_struct.peso_julian_date, strTemp, 10 );
      strcpy( Auth_Tx.CCF03P_details.peso_julian_date, strTemp );

      memset( strTemp, 0x00, sizeof(strTemp) );
      ltoa( private_ccf03p_struct.dollar_julian_date, strTemp, 10 );
      strcpy( Auth_Tx.CCF03P_details.dollar_julian_date, strTemp );
   }

   memset (strTemp, 0, sizeof (strTemp)) ;
   itoa (private_ccf03p_struct.failed_pin_attempts, strTemp, 10) ;
   strcpy (Auth_Tx.CCF03P_details.failed_pin_attempts, strTemp) ;

   Convert_Amt_To_Auth_Tx_String_Format( private_ccf03p_struct.daily_amt_usage,
                                         strAmount) ;
   strcpy (Auth_Tx.CCF03P_details.daily_amt_usage, strAmount) ;

   for (nIndex = 0 ; nIndex <= 13 ; nIndex++)
   {
      memset (strTemp, 0, sizeof (strTemp)) ;
      itoa (private_ccf03p_struct.peso_usage[nIndex].nbr_usage, strTemp, 10) ;
      strcpy (Auth_Tx.CCF03P_details.peso_usage[nIndex].nbr_usage, strTemp) ;

      // Multiply the number by 100 and get rid of the decimal point
      memset (strAmount, 0, sizeof (strAmount)) ;
      Convert_Amt_To_Auth_Tx_String_Format 
             (private_ccf03p_struct.peso_usage[nIndex].amt_usage, strAmount) ;

      strcpy (Auth_Tx.CCF03P_details.peso_usage[nIndex].amt_usage, strAmount) ;

      memset (strTemp, 0, sizeof (strTemp)) ;
      itoa (private_ccf03p_struct.dollar_usage[nIndex].nbr_usage, strTemp, 10) ;
      strcpy (Auth_Tx.CCF03P_details.dollar_usage[nIndex].nbr_usage, strTemp) ;

      // Multiply the number by 100 and get rid of the decimal point
      memset (strAmount, 0, sizeof (strAmount)) ;
      Convert_Amt_To_Auth_Tx_String_Format 
              (private_ccf03p_struct.dollar_usage[nIndex].amt_usage, strAmount);

      strcpy (Auth_Tx.CCF03P_details.dollar_usage[nIndex].amt_usage, strAmount);
   }

   for (nIndex = 0 ; nIndex <= 19 ; nIndex++)
   {
      strcpy (Auth_Tx.CCF03P_details.mcc[nIndex].category_code, 
              private_ccf03p_struct.mcc[nIndex].category_code) ;

      memset (strTemp, 0, sizeof (strTemp)) ;
      itoa (private_ccf03p_struct.mcc[nIndex].usage, strTemp, 10) ;
      strcpy (Auth_Tx.CCF03P_details.mcc[nIndex].usage, strTemp) ;
   }

   for (nIndex = 0 ; nIndex <= 19 ; nIndex++)
   {
      strcpy (Auth_Tx.CCF03P_details.repeat[nIndex].merchant_id, 
              private_ccf03p_struct.repeat[nIndex].merchant_id) ;

      memset (strTemp, 0, sizeof (strTemp)) ;
      itoa (private_ccf03p_struct.repeat[nIndex].nbr_uses, strTemp, 10) ;
      strcpy (Auth_Tx.CCF03P_details.repeat[nIndex].nbr_uses, strTemp) ;
   }

   // Set CCF03P_MASK in the update_mask field of TLF01
   Auth_Tx.TLF01_details.update_mask[0] |= CCF03P_MASK ;
   return NO_AUTH_ERROR ;
} /* TxAuthorize_Update_CCF03_Structure */

BYTE TxAuthorize_Update_Offus_Velocity_Structure (void)
{
	BYTE strTemp[50] = {0} ;
	BYTE strAmount[50] = {0} ;
	int nIndex = 0 ;

	ltoa (private_ccf03p_struct.peso_julian_date, strTemp, 10) ;
	strcpy (Auth_Tx.offus_velocity_details.peso_julian_date, strTemp) ;

	memset (strTemp, 0, sizeof (strTemp)) ;
	ltoa (private_ccf03p_struct.dollar_julian_date, strTemp, 10) ;
	strcpy (Auth_Tx.offus_velocity_details.dollar_julian_date, strTemp) ;

	memset (strTemp, 0, sizeof (strTemp)) ;
	itoa (private_ccf03p_struct.failed_pin_attempts, strTemp, 10) ;
	strcpy (Auth_Tx.offus_velocity_details.failed_pin_attempts, strTemp) ;


	Convert_Amt_To_Auth_Tx_String_Format (private_ccf03p_struct.daily_amt_usage, strAmount) ;
	strcpy (Auth_Tx.offus_velocity_details.daily_amt_usage, strAmount) ;

	for (nIndex = 0 ; nIndex <= 13 ; nIndex++)
	{
		memset (strTemp, 0, sizeof (strTemp)) ;
		itoa (private_ccf03p_struct.peso_usage[nIndex].nbr_usage, strTemp, 10) ;
		strcpy (Auth_Tx.offus_velocity_details.peso_usage[nIndex].nbr_usage, strTemp) ;
		
		memset (strAmount, 0, sizeof (strAmount)) ;
		// Multiply the number by 100 and get rid of the decimal point
		Convert_Amt_To_Auth_Tx_String_Format 
			(private_ccf03p_struct.peso_usage[nIndex].amt_usage, strAmount) ;
		
		strcpy (Auth_Tx.offus_velocity_details.peso_usage[nIndex].amt_usage, strAmount) ;

		memset (strTemp, 0, sizeof (strTemp)) ;
		itoa (private_ccf03p_struct.dollar_usage[nIndex].nbr_usage, strTemp, 10) ;
		strcpy (Auth_Tx.offus_velocity_details.dollar_usage[nIndex].nbr_usage, strTemp) ;
		
		memset (strAmount, 0, sizeof (strAmount)) ;
		// Multiply the number by 100 and get rid of the decimal point
		Convert_Amt_To_Auth_Tx_String_Format 
			(private_ccf03p_struct.dollar_usage[nIndex].amt_usage, strAmount) ;
		
		strcpy (Auth_Tx.offus_velocity_details.dollar_usage[nIndex].amt_usage, strAmount) ;
	}

	for (nIndex = 0 ; nIndex <= 19 ; nIndex++)
	{
		strcpy (Auth_Tx.offus_velocity_details.mcc[nIndex].category_code, 
			     private_ccf03p_struct.mcc[nIndex].category_code) ;

		memset (strTemp, 0, sizeof (strTemp)) ;
		itoa (private_ccf03p_struct.mcc[nIndex].usage, strTemp, 10) ;
		strcpy (Auth_Tx.offus_velocity_details.mcc[nIndex].usage, strTemp) ;	
	}

	for (nIndex = 0 ; nIndex <= 19 ; nIndex++)
	{
		strcpy (Auth_Tx.offus_velocity_details.repeat[nIndex].merchant_id, 
			     private_ccf03p_struct.repeat[nIndex].merchant_id) ;

		memset (strTemp, 0, sizeof (strTemp)) ;
		itoa (private_ccf03p_struct.repeat[nIndex].nbr_uses, strTemp, 10) ;
		strcpy (Auth_Tx.offus_velocity_details.repeat[nIndex].nbr_uses, strTemp) ;	
	}

	// Set CCF03P_MASK in the update_mask field of TLF01
	Auth_Tx.TLF01_details.update_mask[0] |= OFFUS_VELOCITY_MASK ;
	return NO_AUTH_ERROR ;
} /* TxAuthorize_Update_Offus_Velocity_Structure */

  /* Function For update Account details, when Local processing rejected transction But host approved transction.
     This can happen in case of STIP and card holder accounts are not synchronized properly with issuer.
     TF PHANI	
*/
BYTE TxAuthorize_STIP_Update_ACF01_Structure (void)
{
	BYTE strTemp[50] = {0} ;
	BYTE strAmount[50] = {0} ;
	int nIndex = 0 ;
   double nACF01_Available        = 0.0 ;
   double nACF01_Cash_Available   = 0.0 ;
   double nACF01_Outstanding      = 0.0 ;
   double nACF01_Cash_Outstanding = 0.0 ;
   double nACF01_Credit_Limit     = 0.0 ;
   double nACF01_Cash_Limit       = 0.0 ;
   double nTLF01_orig_amount      = 0.0 ;
   double nConvertedTxAmount      = 0.0 ;

    nACF01_Incremented_Outstanding = 0.0 ;
    nACF01_Incremented_Cash_Outstanding = 0.0 ;
    nACF01_Decremented_Available = 0.0 ;
    nACF01_Decremented_Cash_Available = 0.0 ;
   nConvertedTxAmount	   = (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0) ;	
   nACF01_Available        = (strtod (Auth_Tx.ACF01_details.available, 0) / 100.0) ;
   nACF01_Outstanding      = (strtod (Auth_Tx.ACF01_details.outstanding, 0) / 100.0) ;
   nACF01_Cash_Available   = (strtod (Auth_Tx.ACF01_details.cash_available, 0) / 100.0) ;
   nACF01_Cash_Outstanding = (strtod (Auth_Tx.ACF01_details.cash_outstanding, 0) / 100.0) ;
   nACF01_Credit_Limit     = (strtod (Auth_Tx.ACF01_details.credit_limit, 0) / 100.0) ;
   nACF01_Cash_Limit       = (strtod (Auth_Tx.ACF01_details.cash_limit, 0) / 100.0) ;
   
	if (nConvertedTxAmount <= nACF01_Available)
    {
		 nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = nACF01_Available - nConvertedTxAmount ;
         nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
    }else
    {
		 nACF01_Incremented_Outstanding      = nACF01_Outstanding + nConvertedTxAmount ;
         nACF01_Decremented_Available        = 0.0 ;
		 nACF01_Incremented_Cash_Outstanding = nACF01_Cash_Outstanding  ;
         nACF01_Decremented_Cash_Available   = nACF01_Cash_Available  ;
	}
	/* We have a limit of 10 digits and 2 decimals for amount fields */
    if (nACF01_Incremented_Outstanding > 9999999999.99) nACF01_Incremented_Outstanding = 9999999999.99 ;
    if (nACF01_Incremented_Cash_Outstanding > 9999999999.99) nACF01_Incremented_Cash_Outstanding = 9999999999.99 ;

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format (nACF01_Decremented_Available, strAmount) ;
	LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.available-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.available, strAmount) ;

    memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format(nACF01_Incremented_Outstanding, strAmount) ;
    LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.outstanding-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.outstanding, strAmount) ;

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format (nACF01_Incremented_Cash_Outstanding, strAmount) ;
    LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.cash_outstanding-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.cash_outstanding, strAmount) ;

	
    /* special case - this is because TLF01 contains only <outstanding_balance> */
    strcpy (Auth_Tx.TLF01_details.outstanding_balance, Auth_Tx.ACF01_details.available) ;
    /* end special case */

    strcpy (Auth_Tx.TLF01_details.credit_limit, Auth_Tx.ACF01_details.credit_limit) ;

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format (nACF01_Decremented_Cash_Available, strAmount) ;
    LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.cash_available-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.cash_available, strAmount) ;

	/* Set ACF01_MASK in the update_mask field of TLF01 */
	Auth_Tx.TLF01_details.update_mask[0] |= ACF01_MASK ;
	return NO_AUTH_ERROR ;
} /* TxAuthorize_STIP_Update_ACF01_Structure */

BYTE TxAuthorize_Update_ACF01_Structure (void)
{
	BYTE strTemp[50] = {0} ;
	BYTE strAmount[50] = {0} ;
	int nIndex = 0 ;

    if (nACF01_Incremented_Outstanding <= 0.0) nACF01_Incremented_Outstanding = 0.0 ;
    if (nACF01_Incremented_Cash_Outstanding <= 0.0) nACF01_Incremented_Cash_Outstanding = 0.0 ;
    if (nACF01_Decremented_Available <= 0.0) nACF01_Decremented_Available = 0.0 ;
    if (nACF01_Decremented_Cash_Available <= 0.0) nACF01_Decremented_Cash_Available = 0.0 ;

    /* We have a limit of 10 digits and 2 decimals for amount fields */
    if (nACF01_Incremented_Outstanding > 9999999999.99) nACF01_Incremented_Outstanding = 9999999999.99 ;
    if (nACF01_Incremented_Cash_Outstanding > 9999999999.99) nACF01_Incremented_Cash_Outstanding = 9999999999.99 ;

    memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format(nACF01_Incremented_Outstanding, strAmount) ;
    LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.outstanding-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.outstanding, strAmount) ;

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format (nACF01_Incremented_Cash_Outstanding, strAmount) ;
    LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.cash_outstanding-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.cash_outstanding, strAmount) ;

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format (nACF01_Decremented_Available, strAmount) ;
    LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.available-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.available, strAmount) ;

    /* special case - this is because TLF01 contains only <outstanding_balance> */
    strcpy (Auth_Tx.TLF01_details.outstanding_balance, Auth_Tx.ACF01_details.available) ;
    /* end special case */

    strcpy (Auth_Tx.TLF01_details.credit_limit, Auth_Tx.ACF01_details.credit_limit) ;

	memset (strAmount, 0, sizeof (strAmount)) ;
	Convert_Amt_To_Auth_Tx_String_Format (nACF01_Decremented_Cash_Available, strAmount) ;
    LeftFill_String_With_Character (sizeof Auth_Tx.ACF01_details.cash_available-1, strAmount,'0') ;
	strcpy (Auth_Tx.ACF01_details.cash_available, strAmount) ;

	/* Set ACF01_MASK in the update_mask field of TLF01 */
	Auth_Tx.TLF01_details.update_mask[0] |= ACF01_MASK ;
	return NO_AUTH_ERROR ;
} /* TxAuthorize_Update_ACF01_Structure */

BYTE TxAuthorize_Update_BON01_Structure (void)
{
	BYTE strTemp[50] = {0} ;
	BYTE strAmount[50] = {0} ;
	int nIndex = 0 ;

	memset (strTemp, 0, sizeof (strTemp)) ;
	ltoa (nCashBonusTransactionCounter, strTemp, 10) ;
	strcpy (Auth_Tx.BON01_details.transaction_counter, strTemp) ;
	// Set BON01_MASK in the update_mask field of TLF01
	Auth_Tx.TLF01_details.update_mask[0] |= BON01_MASK ;
	return NO_AUTH_ERROR ;
} /* TxAuthorize_Update_BON01_Structure */

BYTE TxAuthorize_Get_Velocity_Limits_From_Profile (CARD_VELOCITY_FROM_PROFILE card_velocity[]) 
{
	int nIndex = 0 ;

	for (nIndex = 0 ; nIndex <= 2 ; nIndex ++)
	{
		// convert velocity values to numeric
		card_velocity[nIndex].day_period 
			= atoi (Auth_Tx.CAF01_details.velocity[nIndex].day_period) ;
		if (card_velocity[nIndex].day_period > 14)
			card_velocity[nIndex].day_period = 14 ;

		card_velocity[nIndex].nbr_usage_peso = 
			atoi (Auth_Tx.CAF01_details.velocity[nIndex].nbr_usage[0]) ;
		card_velocity[nIndex].nbr_usage_dollar = 
			atoi (Auth_Tx.CAF01_details.velocity[nIndex].nbr_usage[1]) ;

		card_velocity[nIndex].amt_usage_peso = 
			(strtod (Auth_Tx.CAF01_details.velocity[nIndex].amt_usage[0],0) / 100.0);
		card_velocity[nIndex].amt_usage_dollar = 
			(strtod (Auth_Tx.CAF01_details.velocity[nIndex].amt_usage[1],0) / 100.0);
	} // for	
	return NO_AUTH_ERROR ;
} /* TxAuthorize_Get_Velocity_Limits_From_Profile */

BYTE TxAuthorize_Clear_Usage_Bucket (int Context) 
{
	int index = 0 ;

   	if (Context == PESO)
	{
		for (index = 0 ; index <= 13 ; index++)
		{
			private_ccf03p_struct.peso_usage[index].nbr_usage = 0 ;
			private_ccf03p_struct.peso_usage[index].amt_usage = 0.0 ;
		} // for
	}
	else if (Context == DOLLAR)
	{
		for (index = 0 ; index <= 13 ; index++)
		{
			private_ccf03p_struct.dollar_usage[index].nbr_usage = 0 ;
			private_ccf03p_struct.dollar_usage[index].amt_usage = 0.0 ;
		} // for
	}
	return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_Populate_Usage_Bucket (int context)
{
	int index = 0 ;

   if (context == PESO)
	{
		// Copy the array from CCF03P into card_usage converting 
		// the string to numeric
		for (index = 0 ; index <= 13 ; index++)
		{
			private_ccf03p_struct.peso_usage[index].nbr_usage = 
				atoi (Auth_Tx.CCF03P_details.peso_usage[index].nbr_usage) ;
			private_ccf03p_struct.peso_usage[index].amt_usage = 
				(strtod (Auth_Tx.CCF03P_details.peso_usage[index].amt_usage,0) / 100.0) ;
		} // for
	}
	else if (context == DOLLAR)
	{
		// Copy the array from CCF03P into card_usage converting the string to numeric
		for (index = 0 ; index <= 13 ; index++)
		{
			private_ccf03p_struct.dollar_usage[index].nbr_usage = 
				atoi (Auth_Tx.CCF03P_details.dollar_usage[index].nbr_usage) ;
			private_ccf03p_struct.dollar_usage[index].amt_usage = 
				(strtod (Auth_Tx.CCF03P_details.dollar_usage[index].amt_usage,0) / 100.0) ;
		} // for
	}
	return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_Populate_Offus_Velocity_Usage_Bucket (int context)
{
	int index = 0 ;

   if (context == PESO)
	{
		// Copy the array from CCF03P into card_usage converting 
		// the string to numeric
		for (index = 0 ; index <= 13 ; index++)
		{
			private_ccf03p_struct.peso_usage[index].nbr_usage = 
				atoi (Auth_Tx.offus_velocity_details.peso_usage[index].nbr_usage) ;
			private_ccf03p_struct.peso_usage[index].amt_usage = 
				(strtod (Auth_Tx.offus_velocity_details.peso_usage[index].amt_usage,0) / 100.0) ;
		} // for
	}
	else if (context == DOLLAR)
	{
		// Copy the array from CCF03P into card_usage converting the string to numeric
		for (index = 0 ; index <= 13 ; index++)
		{
			private_ccf03p_struct.dollar_usage[index].nbr_usage = 
				atoi (Auth_Tx.offus_velocity_details.dollar_usage[index].nbr_usage) ;
			private_ccf03p_struct.dollar_usage[index].amt_usage = 
				(strtod (Auth_Tx.offus_velocity_details.dollar_usage[index].amt_usage,0) / 100.0) ;
		} // for
	}
	return NO_AUTH_ERROR ;
}


BYTE TxAuthorize_Populate_Merchant_Usage_Bucket (void)
{
	int index = 0 ;
   
   	for (index = 0 ; index <= 19 ; index++)
	{
		strcpy (private_ccf03p_struct.repeat[index].merchant_id, 
			    Auth_Tx.CCF03P_details.repeat[index].merchant_id) ;
		private_ccf03p_struct.repeat[index].nbr_uses = 
			atoi (Auth_Tx.CCF03P_details.repeat[index].nbr_uses) ;
	} // for
	return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_Populate_Category_Usage_Bucket (void)
{
	int index = 0 ;

   	for (index = 0 ; index <= 19 ; index++)
	{
		strcpy (private_ccf03p_struct.mcc[index].category_code, 
				  Auth_Tx.CCF03P_details.mcc[index].category_code) ;
		private_ccf03p_struct.mcc[index].usage = 
			atoi (Auth_Tx.CCF03P_details.mcc[index].usage) ;
	} // for
	return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_Populate_Offus_Velocity_Merchant_Usage_Bucket (void)
{
	int index = 0 ;
   
  	for (index = 0 ; index <= 19 ; index++)
	{
		strcpy (private_ccf03p_struct.repeat[index].merchant_id, 
			    Auth_Tx.offus_velocity_details.repeat[index].merchant_id) ;
		private_ccf03p_struct.repeat[index].nbr_uses = 
			atoi (Auth_Tx.offus_velocity_details.repeat[index].nbr_uses) ;
	} // for
	return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_Populate_Offus_Velocity_Category_Usage_Bucket (void)
{
	int index = 0 ;

   	for (index = 0 ; index <= 19 ; index++)
	{
		strcpy (private_ccf03p_struct.mcc[index].category_code, 
				  Auth_Tx.offus_velocity_details.mcc[index].category_code) ;
		private_ccf03p_struct.mcc[index].usage = 
			atoi (Auth_Tx.offus_velocity_details.mcc[index].usage) ;
	} // for
	return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_Populate_Failed_Pin_Attempts (void) 
{
	private_ccf03p_struct.failed_pin_attempts = 
		atoi (Auth_Tx.CCF03P_details.failed_pin_attempts) ;
	return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_Deduct_Amount_From_Usage_Bucket( INT    CurrencyType,
                                                  double nConvertedTxAmount )
{
   INT  index;
   BYTE CategoryFoundInBucket = 0;
   BYTE MerchantFoundInBucket = 0;

   /* On a reversal, deduct the amounts that were
    * added during the previous transaction.
    */
   if (CurrencyType == PESO)
   {
      private_ccf03p_struct.peso_usage[0].nbr_usage -= 1 ;
      private_ccf03p_struct.peso_usage[0].amt_usage -= nConvertedTxAmount ;

      if (private_ccf03p_struct.peso_usage[0].nbr_usage < 0)
         private_ccf03p_struct.peso_usage[0].nbr_usage = 0 ;

      if (private_ccf03p_struct.peso_usage[0].amt_usage < 0.0)
         private_ccf03p_struct.peso_usage[0].amt_usage  = 0.0 ;
   }
   else if (CurrencyType == DOLLAR)
   {
      private_ccf03p_struct.dollar_usage[0].nbr_usage -= 1 ;
      private_ccf03p_struct.dollar_usage[0].amt_usage -= nConvertedTxAmount ;

      if (private_ccf03p_struct.dollar_usage[0].nbr_usage < 0)
         private_ccf03p_struct.dollar_usage[0].nbr_usage = 0 ;

      if (private_ccf03p_struct.dollar_usage[0].amt_usage < 0.0)
         private_ccf03p_struct.dollar_usage[0].amt_usage  = 0.0 ;
   }

   /* Decrement the Merchant Repeat count.
    * ------------------------------------
    * This is the Merchant Same Count on the Auth Profile (CAF01) for
    * a 24 hour period.  Count is kept in CCF03P.
    */
   if (Auth_Tx.CAF01_details.merchant_same[0] != 0)
   {
      for( index = 0; index <= 19 && !MerchantFoundInBucket; index++ )
      {
         if (strEqual (private_ccf03p_struct.repeat[index].merchant_id, 
             Auth_Tx.TLF01_details.merchant_id))
         {
            /* Merchant_id has been found in the repeat bucket. */
            private_ccf03p_struct.repeat[index].nbr_uses -- ;

            if ( private_ccf03p_struct.repeat[index].nbr_uses < 0 )
               private_ccf03p_struct.repeat[index].nbr_uses = 0 ;

            MerchantFoundInBucket = 1; /* Exit Loop */
         }
      }
   }

   /* Decrement the Merchant Type Limits (MCC) count.
    * -----------------------------------------------
    * This is the Merchant Type Limits tab on the Auth Profile (CAF01).
    * Count cannot exceed configured limit within a 24 hour period.
    * Count is kept in CCF03P.
    */
   if (Auth_Tx.TLF01_details.category_code[0] != 0)
   {
      if ( LOCAL_ORIGIN == txservice_GetTransactionOrigin() )
      {
         /* We do not check MCC limits for deferred transactions. */
         if ( !txservice_If_Tx_Is_A_Deferred_Purchase_Type() )
         {
            if ( Mcc_Limits.primary_key.caf_profile_id[0] != 0x00 )
            {
               /* We have MCC limits. */
               for (index = 0 ; index <= 19 && !CategoryFoundInBucket ; index++)
               {
                  if (strEqual (private_ccf03p_struct.mcc[index].category_code,
                                Auth_Tx.TLF01_details.category_code) )
                  {
                     /* Found the MCC in the bucket. */
                     private_ccf03p_struct.mcc[index].usage -- ;

                     if ( private_ccf03p_struct.mcc[index].usage < 0 )
                        private_ccf03p_struct.mcc[index].usage = 0;

                     CategoryFoundInBucket = true;
                  }
               }
            }
         }
      }
   }

   /* Skip the rest of the velocity checking */
   return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_Shift_Peso_Usage_Bucket_Down_And_Verify_Limits 
												(CARD_VELOCITY_FROM_PROFILE card_velocity[],
												int DaysElapsedSinceAPesoTransactionLastOccurred,
												double nConvertedTxAmount) 
{
	int shiftFactor = 0 ;
	int index = 0 ;
	int index1 = 0 ;
	int sumOfNumberUsage = 0 ;
	double sumOfAmountUsage = 0.0 ;
	char strTemp[256] = {0} ;
	BYTE ReturnCode ;

	BYTE tempamount[13]="";

	// Save nConvertedTxAmount so that it can be use in MCC Bypass if MCC Bypass flag is enable for OMD
	memset (tempamount, 0, sizeof (tempamount)) ;
	Convert_Amt_To_Auth_Tx_String_Format(nConvertedTxAmount,tempamount);
	LeftFill_String_With_Character(12,tempamount , '0');
	strncpy(Auth_Tx.TLF01_details.product_codes[15].amount,tempamount,12);

   	shiftFactor = DaysElapsedSinceAPesoTransactionLastOccurred ;

    /* If the system date is changed and transactions are carried out, subsequently if
    the system date is changed and new transactions are carried out, there is a potential that
    the shift factor can become negative */

    if (shiftFactor < 0)
        shiftFactor = 0 ;
												  
	// Note that Shift Factor could be 0, if transactions occur on the same day
	// Shift the Usage Bucket down ShiftFactor times, replacing the zeroth elements
	// of the Usage Bucket with 0
	while (shiftFactor--)
	{
		for (index = 13 ; index >= 1 ; index--)
		{	
			private_ccf03p_struct.peso_usage[index].nbr_usage = 
				private_ccf03p_struct.peso_usage[index-1].nbr_usage ;
			private_ccf03p_struct.peso_usage[index].amt_usage = 
				private_ccf03p_struct.peso_usage[index-1].amt_usage ;
		} // for
		private_ccf03p_struct.peso_usage[0].nbr_usage = 0 ;
		private_ccf03p_struct.peso_usage[0].amt_usage = 0.0 ;
	} // while

	for (index = 2 ; index >= 0 ; index--)
	{
		sumOfNumberUsage = 0 ;
		sumOfAmountUsage = 0.0 ;
		
		if (card_velocity[index].day_period != 0) 
			// turn off velocity checks for the timeframe
		{	
			for (index1 = card_velocity[index].day_period ; index1 >= 1 ; index1--)
			{
				sumOfNumberUsage += private_ccf03p_struct.peso_usage[index1-1].nbr_usage ;
				sumOfAmountUsage += private_ccf03p_struct.peso_usage[index1-1].amt_usage ;
			}

			// Check nbr_usage velocity

			if ((sumOfNumberUsage + 1) > card_velocity[index].nbr_usage_peso)
			{
				sprintf (strTemp,"Fail PesoUsage(Nbr)chk for period %d\n", 
					     index + 1) ;
                
				ReturnCode = txservice_Generate_Usage_Error_Message 
																	(2,strTemp, "Shift_Peso_Usage_Bucket_Down_And_Verify_Limits",
																	GS_TXN_VELOCITY_NBR_USAGE_CHECK_FAILED, REFER_TO_CARD_ISSUER) ;

            /*+-------------------------------------------------+
              | These log messages are diagnostics for SCR 939. |
              | These can be removed when SCR 939 is closed.    |
              +-------------------------------------------------+*/

            /* Debug message 1 */
            memset( strTemp, 0x00, sizeof(strTemp) );
            sprintf( strTemp,
                    "Debug: sumOfNumberUsage = %d, card_velocity.nbr_usage_peso = %d",
                     sumOfNumberUsage, card_velocity[index].nbr_usage_peso );

            txcntl_log_message( 2, 1, strTemp, "TxAuthorize_Shift_Peso_Usage_Bucket_Down_And_Verify_Limits", 1 );
            /* Debug message 2 */
            memset( strTemp, 0x00, sizeof(strTemp) );
            sprintf( strTemp,
                    "Debug: 14 peso usages: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                     private_ccf03p_struct.peso_usage[0].nbr_usage,
                     private_ccf03p_struct.peso_usage[1].nbr_usage,
                     private_ccf03p_struct.peso_usage[2].nbr_usage,
                     private_ccf03p_struct.peso_usage[3].nbr_usage,
                     private_ccf03p_struct.peso_usage[4].nbr_usage,
                     private_ccf03p_struct.peso_usage[5].nbr_usage,
                     private_ccf03p_struct.peso_usage[6].nbr_usage,
                     private_ccf03p_struct.peso_usage[7].nbr_usage,
                     private_ccf03p_struct.peso_usage[8].nbr_usage,
                     private_ccf03p_struct.peso_usage[9].nbr_usage,
                     private_ccf03p_struct.peso_usage[10].nbr_usage,
                     private_ccf03p_struct.peso_usage[11].nbr_usage,
                     private_ccf03p_struct.peso_usage[12].nbr_usage,
                     private_ccf03p_struct.peso_usage[13].nbr_usage );

               txcntl_log_message( 2, 1, strTemp, "TxAuthorize_Shift_Peso_Usage_Bucket_Down_And_Verify_Limits", 1 );
            /* Debug message 3 */
            memset( strTemp, 0x00, sizeof(strTemp) );
            sprintf( strTemp,
                    "Debug: CCF03P: Card %s, Type %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
                     Auth_Tx.CCF03P_details.primary_key.card_nbr,
                     Auth_Tx.CCF03P_details.primary_key.card_type,
                     Auth_Tx.CCF03P_details.peso_usage[0].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[1].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[2].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[3].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[4].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[5].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[6].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[7].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[8].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[9].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[10].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[11].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[12].nbr_usage,
                     Auth_Tx.CCF03P_details.peso_usage[13].nbr_usage );

               txcntl_log_message( 2, 1, strTemp, "TxAuthorize_Shift_Peso_Usage_Bucket_Down_And_Verify_Limits", 1 );
            /* Debug message 4 */
            memset( strTemp, 0x00, sizeof(strTemp) );
            sprintf( strTemp,
                    "Debug: CAF01: %s, %s, Day %s,%s,%s, Nbr %s,%s,%s,%s,%s,%s",
                     Auth_Tx.CAF01_details.primary_key.caf_profile_id,
                     Auth_Tx.CAF01_details.primary_key.type,
                     Auth_Tx.CAF01_details.velocity[0].day_period,
                     Auth_Tx.CAF01_details.velocity[1].day_period,
                     Auth_Tx.CAF01_details.velocity[2].day_period,
                     Auth_Tx.CAF01_details.velocity[0].nbr_usage[0],
                     Auth_Tx.CAF01_details.velocity[0].nbr_usage[1],
                     Auth_Tx.CAF01_details.velocity[1].nbr_usage[0],
                     Auth_Tx.CAF01_details.velocity[1].nbr_usage[1],
                     Auth_Tx.CAF01_details.velocity[2].nbr_usage[0],
                     Auth_Tx.CAF01_details.velocity[2].nbr_usage[1] );


            txcntl_log_message( 2, 1, strTemp, "TxAuthorize_Shift_Peso_Usage_Bucket_Down_And_Verify_Limits", 1 );
            /* Debug message 5 */
            memset( strTemp, 0x00, sizeof(strTemp) );
            sprintf( strTemp,
                    "Debug: index=%d, shiftFactor=%d, dayPeriod=%d",
                     index, shiftFactor, card_velocity[index].day_period );

            txcntl_log_message( 2, 1, strTemp, "TxAuthorize_Shift_Peso_Usage_Bucket_Down_And_Verify_Limits", 1 );
				return ReturnCode ;
			}

			// Check amt_usage velocity

			if ((sumOfAmountUsage + nConvertedTxAmount) > card_velocity[index].amt_usage_peso)
			{
				sprintf (strTemp, 
						  "Fail PesoUsage(Amt)chk for period %d\n", 
						  index + 1) ;
                
				ReturnCode = txservice_Generate_Usage_Error_Message 
																	(2,strTemp, "Shift_Peso_Usage_Bucket_Down_And_Verify_Limits",
																	GS_TXN_VELOCITY_AMT_USAGE_CHECK_FAILED, REFER_TO_CARD_ISSUER) ;
				return ReturnCode ;
			}
		} // if
	} // for
	return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_Shift_Dollar_Usage_Bucket_Down_And_Verify_Limits 
															(CARD_VELOCITY_FROM_PROFILE card_velocity[],
															 int DaysElapsedSinceADollarTransactionLastOccurred,
															 double nConvertedTxAmount) 
{
	int shiftFactor = 0 ;
	int index = 0 ;
	int index1 = 0 ;
	int sumOfNumberUsage = 0 ;
	double sumOfAmountUsage = 0.0 ;
	char strTemp[256] = {0} ;
	BYTE ReturnCode ;

	BYTE tempamount[13]="";

	// Save nConvertedTxAmount so that it can be use in MCC Bypass if MCC Bypass flag is enable for OMD
	memset (tempamount, 0, sizeof (tempamount)) ;
	Convert_Amt_To_Auth_Tx_String_Format(nConvertedTxAmount,tempamount);
	LeftFill_String_With_Character(12,tempamount , '0');
	strncpy(Auth_Tx.TLF01_details.product_codes[15].amount,tempamount,12);

	shiftFactor = DaysElapsedSinceADollarTransactionLastOccurred ;
    
    if (shiftFactor < 0)
        shiftFactor = 0 ;
												  
	// Note that Shift Factor could be 0, if transactions occur on the same day
	// Shift the Usage Bucket down ShiftFactor times, replacing the zeroth elements
	// of the Usage Bucket with 0
	while (shiftFactor--)
	{
		for (index = 13 ; index >= 1 ; index--)
		{	
			private_ccf03p_struct.dollar_usage[index].nbr_usage = 
				private_ccf03p_struct.dollar_usage[index-1].nbr_usage ;

			private_ccf03p_struct.dollar_usage[index].amt_usage = 
				private_ccf03p_struct.dollar_usage[index-1].amt_usage ;
		} // for
		private_ccf03p_struct.dollar_usage[0].nbr_usage = 0 ;
		private_ccf03p_struct.dollar_usage[0].amt_usage = 0.0 ;
	} // while

	for (index = 2 ; index >= 0 ; index--)
	{
		sumOfNumberUsage = 0 ;
		sumOfAmountUsage = 0.0 ;
		
		if (card_velocity[index].day_period != 0) 
			// turn off velocity checks for the timeframe
		{	
			for (index1 = card_velocity[index].day_period ; index1 >= 1 ; index1--)
			{
				sumOfNumberUsage += private_ccf03p_struct.dollar_usage[index1-1].nbr_usage ;
				sumOfAmountUsage += private_ccf03p_struct.dollar_usage[index1-1].amt_usage ;
			}

			// Check nbr_usage velocity

			if ((sumOfNumberUsage + 1) > card_velocity[index].nbr_usage_dollar)
			{
				sprintf (strTemp,"Fail dollarUsage(Nbr)chk for period%d\n", 
					     index + 1) ;
                
				ReturnCode = txservice_Generate_Usage_Error_Message 
																	(2,strTemp, "Shift_Dollar_Usage_Bucket_Down_And_Verify_Limits",
																	GS_TXN_VELOCITY_NBR_USAGE_CHECK_FAILED, REFER_TO_CARD_ISSUER) ;
				return ReturnCode ;
			}

			// Check amt_usage velocity

			if ((sumOfAmountUsage + nConvertedTxAmount) > card_velocity[index].amt_usage_dollar)
			{
				sprintf (strTemp, 
						  "Fail dollarUsage(Amt)chk for period%d\n", 
						  index + 1) ;
                
				ReturnCode = txservice_Generate_Usage_Error_Message 
																	(2,strTemp, "Shift_Dollar_Usage_Bucket_Down_And_Verify_Limits",
																	GS_TXN_VELOCITY_AMT_USAGE_CHECK_FAILED, REFER_TO_CARD_ISSUER) ;
				return ReturnCode ;
			}
		} // if
	} // for
	return NO_AUTH_ERROR ;
}


BYTE TxAuthorize_Validate_Merchant_Repeat_Checks (void)
{
   /* First check to see if the transaction merchant is found in the merchant
    * usage bucket (repeat[]). If the merchant is found, a check is made to
    * see if the (number of uses + 1) exceeds the merchant_same value in the
    * profile. If it exceeds an error is returned. If not, the number of uses
    * for that merchant is incremented.
    *
    * New merchants are always added to the top of the bucket.
    * If the merchant is not found, the merchant has to be added to the bucket.
    * If the value of bucket[0] is 0, it indicates that the bucket is empty
    * and the merchant is added to position 0. If the value of bucket[0] is not
    * 0, the bucket is always shifted down by 1 position and the new merchant
    * is added to the top of the bucket.
    */

   BYTE MerchantFoundInBucket = 0 ;
   INT  index = 0 ;
   CHAR strTemp[256] = {0} ;
   BYTE ReturnCode = 0 ;

   for (index = 0 ; index <= 19 && !MerchantFoundInBucket ; index++)
   {
      if (strEqual (private_ccf03p_struct.repeat[index].merchant_id, 
          Auth_Tx.TLF01_details.merchant_id))
      {
         // merchant_id has been found in the repeat bucket
         MerchantFoundInBucket = 1 ;
         if ((private_ccf03p_struct.repeat[index].nbr_uses + 1) >  
              atoi(Auth_Tx.CAF01_details.merchant_same))
         {
            /* Decline the transaction - over the limit. */
            strcpy (strTemp, "No Of Tx in 24hr period exceeds for Mid") ;

            ReturnCode = txservice_Generate_Usage_Error_Message(
                                2,strTemp, "Validate_Merchant_Repeat_Checks",
                                GS_TXN_MERCHANT_CARD_USAGE_CHECK_FAILED,
                                REFER_TO_CARD_ISSUER) ;
            return SYSTEM_ERROR ;
         }
         else
         {
            private_ccf03p_struct.repeat[index].nbr_uses ++ ;

            if (private_ccf03p_struct.repeat[index].nbr_uses > 999)
               private_ccf03p_struct.repeat[index].nbr_uses = 999 ;
         }
      }
   }

   if (!MerchantFoundInBucket)
   {
      if (!strEqual (private_ccf03p_struct.repeat[0].merchant_id, "0"))
      {	
         /* Shift all the bucket values down by 1 position. */
         for (index = 19 ; index >=1 ; index--)
         {
            strcpy (private_ccf03p_struct.repeat[index].merchant_id, 
                    private_ccf03p_struct.repeat[index-1].merchant_id) ;

            private_ccf03p_struct.repeat[index].nbr_uses = 
                         private_ccf03p_struct.repeat[index-1].nbr_uses ;
         }
      }

      strcpy (private_ccf03p_struct.repeat[0].merchant_id, 
              Auth_Tx.TLF01_details.merchant_id) ;
      private_ccf03p_struct.repeat[0].nbr_uses = 1 ;
   }
   return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_Validate_Category_Count_Checks( INT currency )
{
   // Check if the category code limits have been exceeded
   BYTE CategoryFoundInBucket = 0 ;
   BYTE deferred;
   INT  index = 0 ;
   INT  index1 = 0 ;
   INT  mcc_usage_limit = 0;
   INT  prev_mcc_uses = 0;
   LONG txn_amount = 0;
   LONG mcc_amount_limit = 0;
   CHAR strTemp[256] = {0} ;
   BYTE ReturnCode = 0 ;


   /* Determine if this is a deferred transaction.
    * Because we do not want to include deferred
    * transactions in the MCC Limit testing that follows.
    */
   deferred = txservice_If_Tx_Is_A_Deferred_Purchase_Type();

   if ( (Mcc_Limits.primary_key.caf_profile_id[0] != 0x00) && !deferred )
   {
      /* We have MCC limits.
       * Initialize the variables used for limit checking.
       */
      mcc_usage_limit = atoi( Mcc_Limits.usage );
      txn_amount      = atol( Auth_Tx.TLF01_details.total_amount );

      if ( currency == PESO )
         mcc_amount_limit = atol( Mcc_Limits.peso_amt_limit );
      else if ( currency == DOLLAR )
         mcc_amount_limit = atol( Mcc_Limits.usd_amt_limit  );

      /* Before checking the usage limits, check the one-time txn amount. */
      if ( txn_amount > mcc_amount_limit )
      {
         /* Decline the transaction.
          * Transaction amount is exceeded for this MCC.
          */
         if ( currency == PESO )
         {
            sprintf( strTemp,
                    "Amount %ld over MCC Peso limit:%ld",
                     txn_amount, mcc_amount_limit );
         }
         else if ( currency == DOLLAR )
         {
            sprintf( strTemp,
                    "Amount %ld over MCC USD limit:%ld",
                     txn_amount, mcc_amount_limit );
         }
         else
         {
            strcpy( strTemp, "Fail MCC Amt Limit, unknown currency." );
         }

         ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                             "TxAuthorize_Validate_Category_Count_Checks",
                              GS_TXN_CATEGORY_USAGE_CHECK_FAILED,
                              REFER_TO_CARD_ISSUER );
         return SYSTEM_ERROR;
      }
   }



   // If the Auth_Tx category code is found in the CCF03P mcc[20] 
   // structure, check if there is an MCC record for this CAF Profile
   // in the MCC_Limits structure.  If there is, check the limits.
   // If there is not, do not check the limits, but still increment
   // the usage in the CCF03P mcc[20] structure.

   for (index = 0 ; index <= 19 && !CategoryFoundInBucket ; index++)
   {
      if (strEqual (private_ccf03p_struct.mcc[index].category_code, 
                    Auth_Tx.TLF01_details.category_code) )
      {
         // Found the category_code in the bucket, now compare againt MCC limits
         CategoryFoundInBucket = true ;

         if ( (Mcc_Limits.primary_key.caf_profile_id[0] != 0x00) && !deferred )
         {
            /* CAF Profile does have limits for this MCC. */

            /* Check number of Uses. */
            prev_mcc_uses = private_ccf03p_struct.mcc[index].usage;
            if( (mcc_usage_limit == 0) || ((prev_mcc_uses+1) > mcc_usage_limit))
            {
               /* Decline the transaction.
                * Number of uses has been exceeded for this MCC.
                */
               sprintf( strTemp,
                       "MCC %s exceeds nbr 24hr uses %d > %s",
                        Auth_Tx.TLF01_details.category_code,
                        private_ccf03p_struct.mcc[index].usage + 1,
                        Mcc_Limits.usage );

               ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                   "TxAuthorize_Validate_Category_Count_Checks" ,
                                    GS_TXN_CATEGORY_USAGE_CHECK_FAILED,
                                    REFER_TO_CARD_ISSUER ) ;
               return SYSTEM_ERROR ;
            }
         }

         /* Number of uses and amount is not exceeded. Increment usage count. */
         private_ccf03p_struct.mcc[index].usage ++ ;

         if ( private_ccf03p_struct.mcc[index].usage > 999 )
             private_ccf03p_struct.mcc[index].usage = 999 ;
      }
   } // for

   if (!CategoryFoundInBucket)
   {
      /* If we get here, then this MCC was NOT found in CCF03P.
       * So now, look in CAF01 to see if there is any limit on
       * the number of uses.  Since there was not one found in
       * CCF03P, then here we only need to check for number of
       * allowed uses equal to zero.  If MCC is found in CAF01
       * and number of uses is zero, decline the transaction.
       * Else, break the loop and update the velocity table.
       *
       * Note: Mcc_Limits is linked to CAF01 via CAF Profile ID.
       */
      if ( (Mcc_Limits.primary_key.caf_profile_id[0] != 0x00) && !deferred )
      {
         /* MCC is listed in the CAF profile. Is usage zero? */
         if ( mcc_usage_limit == 0 )
         {
            /* Yes */
            sprintf( strTemp,
                    "MCC %s exceeds number uses. Allowed=0",
                     Auth_Tx.TLF01_details.category_code );

            ReturnCode = txservice_Generate_Usage_Error_Message (2,strTemp,
                                   "TxAuthorize_Validate_Category_Count_Checks",
                                    GS_TXN_CATEGORY_USAGE_CHECK_FAILED,
                                    REFER_TO_CARD_ISSUER );
            return SYSTEM_ERROR ;                  
         }
      }

      /*+---------------------------------+
        | Update velocity usage - CCF03P. |
        +---------------------------------+*/

      /* Shift velocity MCC Uses, if any exist, in CCF03P. */
      if (strcmp (private_ccf03p_struct.mcc[0].category_code, "0"))
      {
         /* Some do exist, so go ahead and shift them. */
         for (index = 19 ; index >=1 ; index--)
         {
            strcpy( private_ccf03p_struct.mcc[index].category_code, 
                    private_ccf03p_struct.mcc[index-1].category_code );

            private_ccf03p_struct.mcc[index].usage = 
                                      private_ccf03p_struct.mcc[index-1].usage;
         }
      }

      /* Now set the first value in CCF03P to this MCC and set usage to 1.
       * Do this whether we needed to shift or not.
       */
      strcpy( private_ccf03p_struct.mcc[0].category_code, 
              Auth_Tx.TLF01_details.category_code) ;

      private_ccf03p_struct.mcc[0].usage = 1 ;
   }				

   return NO_AUTH_ERROR ;
}

BYTE TxAxthorize_Clear_Merchant_Usage_Bucket (void)
{
	int index = 0 ;
	// brand new day, so zap the repeat array elements
	for (index = 0 ; index <= 19 ; index++)
	{
		strcpy (private_ccf03p_struct.repeat[index].merchant_id, "0") ;
		private_ccf03p_struct.repeat[index].nbr_uses = 0 ;
	} // for
	return NO_AUTH_ERROR ;
}

BYTE TxAxthorize_Clear_Category_Usage_Bucket (void)
{
	int index = 0 ;
	// brand new day, so zap the mcc array elements
	for (index = 0 ; index <= 19 ; index++)
	{
		strcpy (private_ccf03p_struct.mcc[index].category_code, "0") ;
		private_ccf03p_struct.mcc[index].usage = 0 ;
	} // for
	return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_Calculate_Deferred_Gross_Amount (double* pConvertedTxAmount) 
{
   double  nDeferredFactor = 0.0 ;
   double  nAmount         = 0.0 ;
   int     nDeferredMonths = 0 ;

   nDeferredFactor = (strtod(Auth_Tx.TLF01_details.deferred_factor, 0) / 1000000.0);
   nDeferredMonths = atoi(Auth_Tx.TLF01_details.deferred_term_length);

   if (txservice_If_Tx_Is_Treated_Like_A_Reversal ())
      nAmount = (strtod (Auth_Tx.TLF01_details.reversal_amount, 0) / 100.0) ;
   else
      nAmount = (strtod (Auth_Tx.TLF01_details.total_amount, 0) / 100.0) ;

   /* *pConvertedTxAmount = nAmount + (nAmount * nDeferredFactor) ;*/
   *pConvertedTxAmount = nAmount * nDeferredFactor * nDeferredMonths ;

	return NO_AUTH_ERROR ;
}

/******************************************************************************
 *
 *  NAME:         TxAuthorize_Calculate_RSB_Amounts
 *
 *  DESCRIPTION:  This procedure calculates deferred gross amount and monthly
 *                amortization amount for an RSB card.  Here is the algorithm:
 *
 *                DGA = Deferred Gross Amount
 *                P   = Principal amount, this is the transaction amount
 *                AR  = Add-On Rate from card record
 *                AT  = Add-On Term from card record
 *                MA  = Monthly amortization
 *
 *                DGA = P + ( (P * AR)/100 )
 *                MA  = DGA / AT
 *
 *  INPUTS:       pConvertedTxAmount = Principal/transaction amount
 *                Auth_Tx - (Global) Transaction record with transaction amount,
 *                                   add-on rate, and add-on term
 *
 *  OUTPUTS:      Auth_Tx - (Global) Deferred gross and monthly amort amounts
 *
 *  RTRN VALUE:   Returns Deferred Gross Amount, with decimals
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
double TxAuthorize_Calculate_RSB_Amounts(double* pConvertedTxAmount)
{
   double  DGA = 0.0;
   double  MA  = 0.0;
   double  P   = 0.0;
   double  AR  = 0.0;
   INT     AT  =   0;
   INT     digit2;
   LONG    lDGA = 0;
   LONG    lMA  = 0;
   LONG    lAR  = 0;
   CHAR    temp_digit[2];
   CHAR    temp_with_decimal[13];
   CHAR    temp_without_decimal[13];

   /* Convert all strings to numeric. */
   AR = strtod( Auth_Tx.CCF02P_details.add_on_rate, 0 ) / 100.0; // Two decimals
   AT = atoi( Auth_Tx.CCF02P_details.add_on_term );
   P  = *pConvertedTxAmount;

   /* Calculate deferred gross amount. */
   DGA = P + (P * AR);  /* Division by 100 is done above, where AR gets set. */

   /* Calculate monthly amortization amount. */
   if ( AT <= 0 )
      AT = 1;

   MA = DGA / (double)AT;

   /*+-------------------------------------------------------------+
     |  B U S I N E S S   R U L E S   F O R   R O U N D I N G      |
     | -------------------------------------------------------     |
     | The following Equitable proprietary rules are used for      |
     | rounding purposes on the RSG Guarantor Card calculations:   |
     |                                                             |
     | 1.  Round to 2 decimals                                     |
     | 2.  If 3rd decimal is from 0 to 4, round 2nd decimal down   |
     | 3.  If 3rd decimal is from 6 to 9, round 2nd decimal up     |
     | 4.  If 3rd decimal is 5, then                               |
     |                                                             |
     |     4.1  If 2nd decimal is even, round 2nd decimal down     |
     |     4.2  If 2nd decimal is odd,  round 2nd decimal up       |
     |                                                             |
     | Examples:                                                   |
     |                                                             |
     |       Amount      Rounded Amount                            |
     |       --------    --------------                            |
     |       150.0025    150.00                                    |
     |       150.0050    150.00                                    |
     |       150.0075    150.01                                    |
     |       150.0150    150.02                                    |
     |       150.0175    150.02                                    |
     |       150.0250    150.02                                    |
     |       150.0350    150.04                                    |
     |       150.0450    150.04                                    |
     |       150.0550    150.06                                    |
     |       150.0825    150.08                                    |
     |       150.0890    150.09                                    |
     |                                                             |
     +-------------------------------------------------------------+*/

   /* First do Deferred Gross Amount */
   memset( temp_digit, 0x00, sizeof(temp_digit) );
   memset( temp_with_decimal, 0x00, sizeof(temp_with_decimal) );
   memset( temp_without_decimal, 0x00, sizeof(temp_without_decimal) );
   sprintf( temp_with_decimal, "%012.2f", DGA * 100.0 );
   temp_digit[0] = temp_with_decimal[10];

   /* Create an integer without the decimal = truncate. */
   memcpy( temp_without_decimal, temp_with_decimal, 9 );
   if ( (temp_digit[0] >= 0x30) && (temp_digit[0] <= 0x34) )
   {
      /* Round down, value is ok as is. */
      lDGA = atol( temp_without_decimal );
   }
   else if ( (temp_digit[0] >= 0x36) && (temp_digit[0] <= 0x39) )
   {
      /* Round up */
      lDGA = atol( temp_without_decimal ) + 1;
   }
   else
   {
      /* Third decimal is a 5.  Check 2nd decimal for odd or even. */
      temp_digit[0] = temp_with_decimal[8]; // 2nd decimal
      digit2 = atoi( temp_digit );
      if ( 0 == (digit2 % 2) )
      {
         /* Even - round down. */
         lDGA = atol( temp_without_decimal );
      }
      else
      {
         /* Odd - round up. */
         lDGA = atol( temp_without_decimal ) + 1;
      }
   }

   /* Now do Monthly Amortization Amount */
   memset( temp_digit, 0x00, sizeof(temp_digit) );
   memset( temp_with_decimal, 0x00, sizeof(temp_with_decimal) );
   memset( temp_without_decimal, 0x00, sizeof(temp_without_decimal) );
   sprintf( temp_with_decimal, "%012.2f", MA * 100.0 );
   temp_digit[0] = temp_with_decimal[10];

   /* Create an integer without the decimal = truncate. */
   memcpy( temp_without_decimal, temp_with_decimal, 9 );
   if ( (temp_digit[0] >= 0x30) && (temp_digit[0] <= 0x34) )
   {
      /* Round down, value is ok as is. */
      lMA = atol( temp_without_decimal );
   }
   else if ( (temp_digit[0] >= 0x36) && (temp_digit[0] <= 0x39) )
   {
      /* Round up */
      lMA = atol( temp_without_decimal ) + 1;
   }
   else
   {
      /* Third decimal is a 5.  Check 2nd decimal for odd or even. */
      temp_digit[0] = temp_with_decimal[8]; // 2nd decimal
      digit2 = atoi( temp_digit );
      if ( 0 == (digit2 % 2) )
      {
         /* Even - round down. */
         lMA = atol( temp_without_decimal );
      }
      else
      {
         /* Odd - round up. */
         lMA = atol( temp_without_decimal ) + 1;
      }
   }

   /* Round the deferred rate. */
   lAR = (LONG)( AR * 100.0 );

   /* Update values in TLF01 record. */
   sprintf( Auth_Tx.TLF01_details.def_gross_amt,        "%012ld", lDGA );
   sprintf( Auth_Tx.TLF01_details.monthly_amort_amt,    "%012ld", lMA  );
   sprintf( Auth_Tx.TLF01_details.deferred_factor,      "%07ld",  lAR  );
   sprintf( Auth_Tx.TLF01_details.deferred_term_length, "%03d",   AT   );

   return( DGA );
}


BYTE TxAuthorize_Calculate_Deferred_Monthly_Amortized_Amount (double* pConvertedTxAmount) 
{
   double  nGrossDeferredAmount = 0.0 ;
   int     nDeferredMonths      = 0 ;

   TxAuthorize_Calculate_Deferred_Gross_Amount (&nGrossDeferredAmount)  ;

   nDeferredMonths = atoi (Auth_Tx.TLF01_details.deferred_term_length) ;

   if (nDeferredMonths == 0) 
      nDeferredMonths = 1 ;

   *pConvertedTxAmount = nGrossDeferredAmount / ((double)nDeferredMonths) ;

   /* Update values in TLF01 record. */
   sprintf( Auth_Tx.TLF01_details.def_gross_amt,
           "%012.0f",
            nGrossDeferredAmount*100.0 );

   sprintf( Auth_Tx.TLF01_details.monthly_amort_amt,
           "%012.0f",
            *pConvertedTxAmount*100.0 );

	return NO_AUTH_ERROR ;
}

BYTE TxAuthorize_If_Transaction_Occurred_On_Same_Day (void)
{
    char strSystemDateTime[50] = {0} ;
    char strCurrentDate[50] = {0} ;
    LONG CurrentDate = 0L ;
    LONG DateWhenAPesoTransactionLastOccurred = 0L ;
    LONG DateWhenADollarTransactionLastOccurred = 0L ;
    LONG DateWhenATransactionLastOccurred = 0L ;
    LONG DaysElapsedSinceATransactionLastOccurred = 0L ;

    /* Get date in yyyymmdd format */
    genutil_format_date ((char*)strSystemDateTime) ;
	memcpy (strCurrentDate, strSystemDateTime, 8) ; 
	
	/* Get Current Date in Julian */
	CurrentDate = Txutils_Calculate_Julian_Date (strCurrentDate) ;

    /* peso_julian_date is the date when a peso tx last occurred */
	DateWhenAPesoTransactionLastOccurred = atol (Auth_Tx.CCF03P_details.peso_julian_date) ;
	/* dollar_julian_date is the date when a dollar tx last occurred */
	DateWhenADollarTransactionLastOccurred = atol (Auth_Tx.CCF03P_details.dollar_julian_date) ;

	/* Regardless of peso or dollar, determine when the latest transaction date for the cardholder */
	if (DateWhenAPesoTransactionLastOccurred > DateWhenADollarTransactionLastOccurred)
		DateWhenATransactionLastOccurred = DateWhenAPesoTransactionLastOccurred ;
	else 
		DateWhenATransactionLastOccurred = DateWhenADollarTransactionLastOccurred ;

	// Regardless of peso or dollar, get diff between the current date and when 
	// a transaction was last occurred for the cardholder (could be 0)
	DaysElapsedSinceATransactionLastOccurred = CurrentDate - DateWhenATransactionLastOccurred ;
    if (DaysElapsedSinceATransactionLastOccurred == 0)
        return 1 ;
    else 
        return 0 ;
} /* TxAuthorize_If_Transaction_Occurred_On_Same_Day */

BYTE TxAuthorize_Update_Private_Ccf03p_Structure ()
{
    memset (&private_ccf03p_struct, 0, sizeof private_ccf03p_struct) ;

	 private_ccf03p_struct.peso_julian_date = atol (Auth_Tx.CCF03P_details.peso_julian_date) ;
    private_ccf03p_struct.dollar_julian_date = atol (Auth_Tx.CCF03P_details.dollar_julian_date) ;

    TxAuthorize_Populate_Failed_Pin_Attempts () ;
    TxAuthorize_Populate_Usage_Bucket (PESO) ;
    TxAuthorize_Populate_Usage_Bucket (DOLLAR) ;
    TxAuthorize_Populate_Merchant_Usage_Bucket () ;
    TxAuthorize_Populate_Category_Usage_Bucket () ;

    return NO_AUTH_ERROR ;
} /* TxAuthorize_Update_private_ccf03p_Structure */

BYTE TxAuthorize_Update_Private_Offus_Velocity_Structure ()
{
    memset (&private_ccf03p_struct, 0, sizeof private_ccf03p_struct) ;

	 private_ccf03p_struct.peso_julian_date = atol (Auth_Tx.offus_velocity_details.peso_julian_date) ;
    private_ccf03p_struct.dollar_julian_date = atol (Auth_Tx.offus_velocity_details.dollar_julian_date) ;

    TxAuthorize_Populate_Offus_Velocity_Usage_Bucket (PESO) ;
    TxAuthorize_Populate_Offus_Velocity_Usage_Bucket (DOLLAR) ;
    TxAuthorize_Populate_Offus_Velocity_Merchant_Usage_Bucket () ;
    TxAuthorize_Populate_Offus_Velocity_Category_Usage_Bucket () ;

    return NO_AUTH_ERROR ;
} /* TxAuthorize_Update_private_ccf03p_Structure */


BYTE TxAuthorize_Is_Transaction_Manually_Entered (void)
{
    char strPosEntryMode[10] = {0} ;

    /* Request from Kenneth..should not check if it comes from the interchange or NC.. TF PHANI*/
	if(0 != strcmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA"))
		return false;
	/* If the first two bytes of the pos_entry_mode is "01", then it indicates
       a manual entry */
     
    strncpy (strPosEntryMode, Auth_Tx.TLF01_details.pos_entry_mode, 3) ;
    if (strEqual (strPosEntryMode, "001"))
        return true ;
    return false ;
} /* TxAuthorize_Is_Transaction_Manually_Entered */
BYTE TxCheck_Allowable_MCC_From_Ini()
{
   CHAR strCardCategoryCode[256] = {0} ;
   INT  CardCategoryCode = 0 , i=0,j=0;
   BYTE ret_val=false;

   /* Check Merchant Category Code to see if merchant is MOTO or T&E. */
   strcpy (strCardCategoryCode, Auth_Tx.TLF01_details.category_code) ;
   CardCategoryCode = atoi (strCardCategoryCode) ;

	
	//cehck with CardCategoryCode
	for (j=0;j <= No_of_MCC_codes ;j++)	
	{
	   if ( (CardCategoryCode >= mcc_check_list[j].low  &&  
			 CardCategoryCode <= mcc_check_list[j].high)
		  ) 
        
		{
		   ret_val = true;
		   return ret_val;
		}
	}


return ret_val;
}
/******************************************************************************
 *
 *  NAME:         TxAuthorize_Is_Merchant_A_MOTO_TE_Merchant
 *
 *  DESCRIPTION:  This function determines if a transaction is from a Mail
 *                Order/Telephone Order (MOTO) or Travel/Entertainment (T&E)
 *                merchant.  This is based on Merchant Category Code (MCC).
 *                If it is a MOTO / T&E merchant, that means the transaction
 *                can be manually entered.  If not a MOTO / T&E merchant, the
 *                transaction must be a card swipe.  However, there are certain
 *                conditions where it does not matter if the transaction is
 *                manually entered or a card swipe. Those checks are made here.
 *                If the transaction falls under one of these conditions, then
 *                the return code is set as if the merchant were actually a
 *                MOTO/T&E merchant.  This is to tell the rest of the code that
 *                it does not matter if the card was swiped ora manual entry.
 *                The conditions are commented in the code below.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if it does not matter if txn entry is swipe or manual,
 *                False if it does matter.
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
BYTE TxAuthorize_Is_Merchant_A_MOTO_TE_Merchant (void)
{
   CHAR strCardCategoryCode[256] = {0} ;
   INT  CardCategoryCode = 0 ;
   BYTE ret_val;
	 ret_val = true;
	 return( ret_val );// NO MCC code validation , Req from Marian
   /* Check Merchant Category Code to see if merchant is MOTO or T&E. */
   strcpy (strCardCategoryCode, Auth_Tx.TLF01_details.category_code) ;
   CardCategoryCode = atoi (strCardCategoryCode) ;

 /*  if ( (CardCategoryCode >= 3000  &&  CardCategoryCode <= 3768) ||
        (CardCategoryCode >= 5960  &&  CardCategoryCode <= 5969) ||
        (CardCategoryCode == 7011) || (CardCategoryCode == 6010) ||
        (CardCategoryCode == 6051) // MCC for Cash Reload       )(erlier logic- TF Phani)*/ 
   if(TxCheck_Allowable_MCC_From_Ini() == true)
   {
      /* Yes it is. */
      ret_val = true;
   }
   else
   {
      /* No, merchant is not MOTO or T&E.
       *
       * Perform other checks. We do not care about MCC if:
       *   Card is a JCB card
       *   Transaction came from Visa
       *   Transaction came from MasterCard
       *   Transaction came from JCB
       *   Transaction is an RPS transaction
       *   Transaction is an e-commerce transaction
       */
      if ( 0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_JCB) )
      {
         /* This is a JCB card.  Do not care about MCC. */
         ret_val = true;
      }
      else if( REMOTE_ORIGIN == txservice_GetTransactionOrigin() )
      {
         if ( NULL != strstr(Auth_Tx.TLF01_details.handler_queue, "visa") )
         {
            /* Txn is from Visa host.  Do not care about MCC. */
            ret_val = true;
         }
         else if ( NULL != strstr(Auth_Tx.TLF01_details.handler_queue, "mcrd") ||
        		   NULL != strstr(Auth_Tx.TLF01_details.handler_queue, "mcr3"))
         {
            /* Txn is from MasterCard host.  Do not care about MCC. */
            ret_val = true;
         }
         else if ( NULL != strstr(Auth_Tx.TLF01_details.handler_queue, "jcb") )
         {
            /* Txn is from JCB host.  Do not care about MCC. */
            ret_val = true;
         }
      }

      /******************
       *  LOCAL ORIGIN  *
       ******************/

      /* For RPS Transactions, we do not care to make this check. */
      else if ( NULL != strstr( Auth_Tx.TLF01_details.handler_queue, "rps" ) )
         ret_val = true;
      else if ( 0 == strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") )
         ret_val = true;

      /* For E-Commerce, we do not care to make this check. */
      else if ( Auth_Tx.industry_code[0] == '7' )
         ret_val = true;
      else if ( 0 == strcmp(Auth_Tx.TLF01_details.pos_condition_code,"59") )
         ret_val = true;


      /************************************************************************
      ----                                                                 ----
      ----  This is commented out for now. Need to make 4722 configurable. ----
      ----                                                                 ----
      ----                                                                 ----

      /* Allow Amex POS Acquiring with MCC = 4722 (EB Requirement).
       * This is a different business practice for Amex from Visa/JCB/MC.
       * 4722 is Travel Agency (T&E).
       *
       * If 4722 AND (card family is null or Amex), then return true.
       *
      else if ( (CardCategoryCode == 4722) &&
                ( (Auth_Tx.BIN01_details.card_family[0] == 0x00)  ||
                  (0 == strcmp(Auth_Tx.BIN01_details.card_family,CARD_AMEX))
                )
              )
         ret_val = true;

      ----                                                                 ----
      ----                                                                 ----
      ----  This is commented out for now. Need to make 4722 configurable. ----
      ----                                                                 ----
      *************************************************************************/

      else
      {
         /* Return false to show that this is not a MOTO or T&E merchant,
          * based on MCC, and that none of the conditions are met which
          * would make us not care.
          *
          * If we reach this line of code, it means the transaction is
          * one of the following:
          *
          *    A non-JCB card from a local device  -or-
          *    A Voice Transaction                 -or-
          *    A transaction that came from Amex or Equitable host
          *
          * Voice transactions should really return true, but I did not
          * put in a special check for them here, because they are checked
          * in a separate function.
          */
         ret_val = false;
      }
   }
   return( ret_val );

} /* TxAuthorize_Is_Merchant_A_MOTO_TE_Merchant */


/******************************************************************************
 *
 *  NAME:         TxAuthorize_Convert_USD_to_Peso
 *
 *  DESCRIPTION:  This function converts the total amount in AuthTx from USD
 *                into Peso.  It will also do the same for the reversal amount
 *                in AuthTx.  The amounts in AuthTx are not overwritten.  The
 *                converted amounts are output parameters.
 *
 *  INPUTS:       AuthTx - (Global) Transaction information
 *
 *  OUTPUTS:      nAmount     - Total Amount in Pesos
 *                nRevAmount  - Reversal amount in Pesos
 *                nOrigAmount - Amount of original txn, in Pesos
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
BOOLEAN TxAuthorize_Convert_USD_to_Peso( double* nAmount,
                                         double* nRevAmount,
                                         double* nOrigAmount )
{
   BOOLEAN  ret_val = true;
   LONG     lReturnCode;
	BYTE     ReturnCode  = 0 ;
   CHAR     Buffer[100] = "";
   double   conv_rate   = 0.0;

   /* Convert from US Dollars to Peso */
   memset( &crf01, 0x00, sizeof(CRF01) );
   strcpy( crf01.primary_key.currency_code, PESO_CURRENCY);

   /* Get conversion value to convert from USD to PESOs */
   lReturnCode = FindMemRecord((pBYTE)&crf01,
                               sizeof(crf01.primary_key.currency_code),
                               sizeof(CRF01), "Crf01Table");

   if ( lReturnCode < 0 )
   {
      /* There is either a problem with shared memory
       * or the record is missing.  Go to the database.
       */
      lReturnCode = get_conv_rate_from_database( &crf01 );
   }

   if (lReturnCode >= 0)
   {
      /* Currency code found in CRF01 table */
      strcpy(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
      conv_rate = strtod( (char *)crf01.conversion_rate, (char **)NULL );
	  if(strcmp(Auth_Tx.TLF01_details.conversion_rate,"") == 0)
		strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );
	  if(strcmp(Auth_Tx.TLF01_details.conversion_rate,"") == 0)
		strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );
      if (conv_rate > 0.0)
      {
         *nAmount = (atof(Auth_Tx.TLF01_details.total_amount)*conv_rate) /100.0;

         /* Also convert reversal amount. */
         if (txservice_If_Tx_Is_Treated_Like_A_Reversal ())
         {
            *nRevAmount = (atof(Auth_Tx.TLF01_details.reversal_amount)*conv_rate) / 100.0;
         }

         /* Convert amount of original transaction. */
         if ( Auth_Tx.TLF01_details.orig_amount[0] != 0x00 )
         {
            *nOrigAmount = (atof(Auth_Tx.TLF01_details.orig_amount)*conv_rate) / 100.0;
         }
      }
      else
      {
         sprintf(Buffer, "Convert rate:%f for currencycode:%s",
                 conv_rate, Auth_Tx.TLF01_details.currency_code );
         ReturnCode = txservice_Generate_Usage_Error_Message 
                           (2,Buffer, "TxAuthorize_Convert_USD_to_Peso",
                           GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION) ;
         ret_val = false;
      }
   }
   else if (-1L == lReturnCode)
   {
      /* Currency code not found in CRF01 table */
      sprintf( Buffer, "Currency conversion notfound for %s",
               PESO_CURRENCY );
      ReturnCode = txservice_Generate_Usage_Error_Message 
                           (2,Buffer, "TxAuthorize_Convert_USD_to_Peso",
                           GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION) ;
      ret_val = false;
   }
   return( ret_val );
}
/* Balance update convet functions, when tx is approved by host, ATP is doing currency conversion
*   and balance update - 19 oct -2010  -TF Phani*/
BOOLEAN TxAuthorizeCadience_Convert_USD_to_Peso( double* nAmount,
                                         double* nRevAmount,
                                         double* nOrigAmount )
{
   BOOLEAN  ret_val = true;
   LONG     lReturnCode;
	BYTE     ReturnCode  = 0 ;
   CHAR     Buffer[100] = "";
   double   conv_rate   = 0.0;

   /* Convert from US Dollars to Peso */
   memset( &crf01, 0x00, sizeof(CRF01) );
   strcpy( crf01.primary_key.currency_code, PESO_CURRENCY);

   /* Get conversion value to convert from USD to PESOs */
   lReturnCode = FindMemRecord((pBYTE)&crf01,
                               sizeof(crf01.primary_key.currency_code),
                               sizeof(CRF01), "Crf01Table");

   if ( lReturnCode < 0 )
   {
      /* There is either a problem with shared memory
       * or the record is missing.  Don not go to the database.
       */
           // Ignore error, since already tx is approved by host
            ret_val = false;
			return( ret_val );
           
   }

   if (lReturnCode >= 0)
   {
      /* Currency code found in CRF01 table */
      strcpy(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
      conv_rate = strtod( (char *)crf01.conversion_rate, (char **)NULL );
  	  if(strcmp(Auth_Tx.TLF01_details.conversion_rate,"") == 0)
		strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );

  	  if(strcmp(Auth_Tx.TLF01_details.conversion_rate,"") == 0)
		strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );

      if (conv_rate > 0.0)
      {
         *nAmount = (atof(Auth_Tx.TLF01_details.total_amount)*conv_rate) /100.0;

         /* Also convert reversal amount. */
         if (txservice_If_Tx_Is_Treated_Like_A_Reversal ())
         {
         	// We need to add a falg so that tranlookup will decide EmarMarkedCurrency
         	if(0 == strncmp(Auth_Tx.BIN01_details.card_family, CARD_JCB, 2))
         	{
				strncpy(Auth_Tx.TLF01_details.product_codes[1].quantity,"01",2);
         	}
            *nRevAmount = (atof(Auth_Tx.TLF01_details.reversal_amount)*conv_rate) / 100.0;
         }

         /* Convert amount of original transaction. */
         if ( Auth_Tx.TLF01_details.orig_amount[0] != 0x00 )
         {
            *nOrigAmount = (atof(Auth_Tx.TLF01_details.orig_amount)*conv_rate) / 100.0;
         }
      }
      else
      {
         // Ignore error, since already tx is approved by host
         ret_val = false;
      }
   }
   else if (-1L == lReturnCode)
   {
      /* Currency code not found in CRF01 table */
       // Ignore error, since already tx is approved by host
      ret_val = false;
   }
   return( ret_val );
}


/* Function for balance update - TF Phani */
BOOLEAN TxAuthorizeCadience_Convert_Peso_to_USD( double* nAmount,
                                         double* nRevAmount,
                                         double* nOrigAmount )

{
   BOOLEAN  ret_val = true;
   LONG     lReturnCode;
	BYTE     ReturnCode  = 0 ;
   CHAR     Buffer[100] = "";
   double   conv_rate   = 0.0;

   /* Convert from Peso to USD */
   memset( &crf01, 0x00, sizeof(CRF01) );
   strcpy( crf01.primary_key.currency_code, PESO_CURRENCY);

   /* Get conversion value to convert from USD to PESOs */
   lReturnCode = FindMemRecord((pBYTE)&crf01,
                               sizeof(crf01.primary_key.currency_code),
                               sizeof(CRF01), "Crf01Table");

   if ( lReturnCode < 0 )
   {
      /* There is either a problem with shared memory
       * or the record is missing. Do not  Go to the database.
       */
       // Ignore error, since already tx is approved by host
      ret_val = false;
   }

   if (lReturnCode >= 0)
   {
      /* Currency code found in CRF01 table */
      strcpy(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
      conv_rate = strtod( (char *)crf01.conversion_rate, (char **)NULL );
  	  if(strcmp(Auth_Tx.TLF01_details.conversion_rate,"") == 0)
		strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );

  	  if(strcmp(Auth_Tx.TLF01_details.conversion_rate,"") == 0)
		strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );

      if (conv_rate > 0.0)
      {
         *nAmount = (atof(Auth_Tx.TLF01_details.total_amount)/conv_rate)/100.0;

         /* Also convert reversal amount. */
         if (txservice_If_Tx_Is_Treated_Like_A_Reversal ())
         {
            *nRevAmount = (atof(Auth_Tx.TLF01_details.reversal_amount)/conv_rate)/100.0;
         }

         /* Convert amount of original transaction. */
         if ( Auth_Tx.TLF01_details.orig_amount[0] != 0x00 )
         {
            *nOrigAmount = (atof(Auth_Tx.TLF01_details.orig_amount)/conv_rate)/100.0;
         }
      }
      else
      {
		  // Ignore error, since already tx is approved by host
			  ret_val = false;
      }
   }
   else if (-1L == lReturnCode)
   {
	   // Ignore error, since already tx is approved by host
		   ret_val = false;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         TxAuthorize_Convert_Peso_to_USD
 *
 *  DESCRIPTION:  This function converts the total amount in AuthTx from Peso
 *                into USD.  It will also do the same for the reversal amount
 *                in AuthTx.  The amounts in AuthTx are not overwritten.  The
 *                converted amounts are output parameters.
 *
 *  INPUTS:       AuthTx - (Global) Transaction information
 *
 *  OUTPUTS:      nAmount     - Total Amount in Pesos
 *                nRevAmount  - Reversal amount in Pesos
 *                nOrigAmount - Amount of original txn, in Pesos
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Ajay . ST /MS MD
 *
 ******************************************************************************/
BOOLEAN TxAuthorize_Convert_Peso_to_USD( double* nAmount,
                                         double* nRevAmount,
                                         double* nOrigAmount )
{
   BOOLEAN  ret_val = true;
   LONG     lReturnCode;
	BYTE     ReturnCode  = 0 ;
   CHAR     Buffer[100] = "";
   double   conv_rate   = 0.0;

   /* Convert from Peso to USD */
   memset( &crf01, 0x00, sizeof(CRF01) );
   strcpy( crf01.primary_key.currency_code, PESO_CURRENCY);

   /* Get conversion value to convert from USD to PESOs */
   lReturnCode = FindMemRecord((pBYTE)&crf01,
                               sizeof(crf01.primary_key.currency_code),
                               sizeof(CRF01), "Crf01Table");

   if ( lReturnCode < 0 )
   {
      /* There is either a problem with shared memory
       * or the record is missing.  Go to the database.
       */
      lReturnCode = get_conv_rate_from_database( &crf01 );
   }

   if (lReturnCode >= 0)
   {
      /* Currency code found in CRF01 table */
      strcpy(Auth_Tx.TLF01_details.currency_code, PESO_CURRENCY );
      conv_rate = strtod( (char *)crf01.conversion_rate, (char **)NULL );
  	  if(strcmp(Auth_Tx.TLF01_details.conversion_rate,"") == 0)
		strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );

  	  if(strcmp(Auth_Tx.TLF01_details.conversion_rate,"") == 0)
		strcpy( Auth_Tx.TLF01_details.conversion_rate, crf01.conversion_rate );

      if (conv_rate > 0.0)
      {
         *nAmount = (atof(Auth_Tx.TLF01_details.total_amount)/conv_rate)/100.0;

         /* Also convert reversal amount. */
         if (txservice_If_Tx_Is_Treated_Like_A_Reversal ())
         {
            *nRevAmount = (atof(Auth_Tx.TLF01_details.reversal_amount)/conv_rate)/100.0;
         }

         /* Convert amount of original transaction. */
         if ( Auth_Tx.TLF01_details.orig_amount[0] != 0x00 )
         {
            *nOrigAmount = (atof(Auth_Tx.TLF01_details.orig_amount)/conv_rate)/100.0;
         }
      }
      else
      {
         sprintf(Buffer, "Convert rate:%f for currencycode:%s",
                 conv_rate, Auth_Tx.TLF01_details.currency_code );
         ReturnCode = txservice_Generate_Usage_Error_Message 
                           (2,Buffer, "TxAuthorize_Convert_Peso_to_USD",
                           GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION) ;
         ret_val = false;
      }
   }
   else if (-1L == lReturnCode)
   {
      /* Currency code not found in CRF01 table */
      sprintf( Buffer, "Currency conversion notfound for %s",
               PESO_CURRENCY );
      ReturnCode = txservice_Generate_Usage_Error_Message 
                           (2,Buffer, "TxAuthorize_Convert_Peso_to_USD",
                           GS_TXN_SYSTEM_ERROR, SYSTEM_MALFUNCTION) ;
      ret_val = false;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         GET_CONV_RATE_FROM_DATABASE
 *
 *  DESCRIPTION:  This function makes a call to the data server to select 
 *                a record for currency conversion.  Use the interactive
 *                queue since this function is not expected to be called
 *                often, and it beats breaking up the code to allow for
 *                re-entrant code.
 *                Reference SCR 1293.
 *
 *  INPUTS:       p_crf01 - Currency conversion data structure -> Currency Code
 *                Auth_Tx - (Global) Transaction Info
 *
 *  OUTPUTS:      p_crf01 - Currency conversion data structure -> Currency Rate
 *
 *  RTRN VALUE:    0 if successful
 *                -1 if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *  
 ******************************************************************************/
LONG get_conv_rate_from_database( pCRF01 p_crf01 )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(CRF01)];
   BYTE           dberror;
   CHAR           temp_str[100];
   CHAR           error_msg[200] = "";
   LONG           retcode;
   LONG           ret_val;

   memset( buffer, 0x00, sizeof(buffer) );
   memcpy( buffer+sizeof(AUTH_TX), p_crf01, sizeof(CRF01) );

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CRF01_DATA );

   if( NULL_PTR == p_msg_out )
   {
      strcpy( error_msg, "Insufficient memory to build CRF01 request message" );
      ret_val = -1;
   }
   else
   {
      p_msg_in = pteipc_send_receive( p_msg_out,
                                      tran_dataserver_que_name,
                                      interactive_que_name,
                                     &retcode );
      if( NULL_PTR == p_msg_in )
      {
         pteipc_get_errormsg( retcode, temp_str );
         sprintf( error_msg,
                 "Communication Error during CRF01 request to DataServer: %s",
                  temp_str );
         ret_val = -1;
      }
      else
      {
         if( ( dberror = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
         {
            p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
            p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
            data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

            strcpy( temp_str, p_data+sizeof(AUTH_TX) );
            sprintf( error_msg, "DB Error: %s", temp_str );
            ret_val = -1;
         }
         else
         {
            p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
            p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
            data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

            memset( p_crf01, 0x00, sizeof(CRF01) );
            memcpy( p_crf01, p_data+(sizeof(AUTH_TX)), sizeof(CRF01));
            ret_val = 0;
         }
         free(p_msg_in);
      }
   }

   if ( ret_val < 0 )
   {
      txcntl_log_message( 1, 2, error_msg, "get_conv_rate", 1 );
   }
   return ( ret_val );
}

/******************************************************************************
 *
 *  NAME:         isalphanum
 *
 *  DESCRIPTION:  This function checks for a string to be all numbers and / or
 *                alphabetical characters.
 *
 *  INPUTS:       in_str - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if string is all alphanumeric, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT isalphanum(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (!isalnum(string[i]))
        return(false);
   }
   return(true);
}

/******************************************************************************
 *
 *  NAME:         TxAuthorize_GetAuthId
 *
 *  DESCRIPTION:  This function generates an authorization number
 *                based on the value in UAG01.  The format is Annnnn, where
 *                A is alphanumeric.  Make sure 'A' is numeric if the
 *                subtype2 input parameter asks for numeric only.
 *
 *  INPUTS:       subtype2 - Requests numeric or alphanumeric auth id
 *
 *  OUTPUTS:      Auth_Tx  - (Global) Txn structure with authorization number
 *
 *  RTRN VALUE:   PTEMSG_OK if successful, else a different value
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
BYTE TxAuthorize_GetAuthId( BYTE subtype2 )
{
   #define  AUTH_TABLE_LOCKED_MAX_RETRIES  5

   BYTE  retval = PTEMSG_OK;
   CHAR  AuthNumber[7] = "";
   INT   rcode = TABLE_ALREADY_LOCKED;
   INT   lockctr = 0,nullRet;

   if(txservice_Is_Incoming_stip_reversal_txn() == true)
   {
	   return retval;
   }

   /* Do not overwrite an existing Auth Number. */
   if ( Auth_Tx.TLF01_details.auth_number[0] == 0x00 )
   {
      /* No auth number. Generate a new one. */
      while( (rcode == TABLE_ALREADY_LOCKED) &&
             (lockctr < AUTH_TABLE_LOCKED_MAX_RETRIES) )
      {
         lockctr++;
         rcode = ReadAuthTable( AuthNumber, LOCK );
         if ( MEM_OK == rcode )
         {
            if (!isalphanum(AuthNumber))
			{								// TF PHANI DEBUG
			sprintf( errbuf_authid,
                       "Auth Number is null %s Porblem with Tx or ReadAuthTable has returned NULL.",
                        Auth_Tx.TLF01_details.sys_trace_audit_num );

			txcntl_log_message( 1, 2, errbuf_authid, "TxAuthorize_GetAuthId", 1 );
			}
			 /* We have the old Auth Number; now create a new one. */
            nullRet=TxAuthorize_GenerateNewAuthId( AuthNumber, AuthNumber );
			/* New logic for Auth number validation and Re_Initlization 
			If nullRet value is False means, AUTH_NUM is invalid we have already called
			Init_AUTH_number function, again wecan generate new AUTH_num... TF PHANI
			*/ 
			if(nullRet == false)
			{   rcode = TABLE_ALREADY_LOCKED;
				 continue;
			}

			if (!isalphanum(AuthNumber))
			{								// TF PHANI DEBUG
				sprintf( errbuf_authid,
                       "Auth Number is null %s Porblem with Tx or TxAuthorize_GenerateNewAuthId made NULL",
                        Auth_Tx.TLF01_details.sys_trace_audit_num );

				txcntl_log_message( 1, 2, errbuf_authid, "TxAuthorize_GetAuthId", 1 );
			}

            rcode = WriteAuthTable( AuthNumber, UNLOCK );
            if ( MEM_OK == rcode )
            {
               /* A new auth number has been put into the Auth Table. */
               AuthNumberChanged = true;

               if ( subtype2 == ST2_DB_SELECT_NUMERIC_AUTH_ID )
               {
                  /* Change the format from 'Annnnn' to 'nnnnnn'. */
                  if ( !isdigit(AuthNumber[0]) )
                  {
                     switch( AuthNumber[0] )
                     {
                        /* Dont set to 0. Do not want all zeroes. */
                        case 'A':
                        case 'B':
                        case 'C':  AuthNumber[0] = '1'; break;
                        case 'D':
                        case 'E':
                        case 'F':  AuthNumber[0] = '2'; break;
                        case 'G':
                        case 'H':
                        case 'I':  AuthNumber[0] = '3'; break;
                        case 'J':
                        case 'K':
                        case 'L':  AuthNumber[0] = '4'; break;
                        case 'M':
                        case 'N':
                        case 'O':  AuthNumber[0] = '5'; break;
                        case 'P':
                        case 'Q':
                        case 'R':  AuthNumber[0] = '6'; break;
                        case 'S':
                        case 'T':
                        case 'U':  AuthNumber[0] = '7'; break;
                        case 'V':
                        case 'W':
                        case 'X':  AuthNumber[0] = '8'; break;
                        case 'Y':
                        case 'Z':  AuthNumber[0] = '9'; break;
                     }
                  }
               }

               /* RSB and deferred transactions must have
                * an auth number format of 'Knnnnn'.
                */
               if ( (Auth_Tx.BIN01_details.credit_subtype[0] == RSB_CARD) ||
                    (txservice_If_Tx_Is_A_Deferred_Purchase_Type())        )
               {
                  AuthNumber[0] = 'K';
               }

               /* Store new auth number into the transaction data structure. */
               strcpy( Auth_Tx.TLF01_details.auth_number, AuthNumber );
            }
            else
            {
               retval = SYSTEM_ERROR;
               memset( errbuf_authid, 0x00, sizeof(errbuf_authid) );
               sprintf( errbuf_authid,
                       "Xipc error %d writing AuthNumber into shared memory.",
                        rcode );

               txcntl_log_message( 1, 2, errbuf_authid, "TxAuthorize_GetAuthId", 1 );
            }
         }
         else if ( rcode == TABLE_ALREADY_LOCKED )
         {
            if ( lockctr >= AUTH_TABLE_LOCKED_MAX_RETRIES )
            {
                txcntl_log_message( 1, 2, "Timed out waiting for AuthNumber table to unlock", "TxAuthorize_GetAuthId", 1 );
                retval = SYSTEM_ERROR;
            }
            else
            {
               /* Wait a bit before trying again (1/10 sec). */
               #ifdef WIN32
                  Sleep( 100 );
               #else
                  usleep( 100000 );
               #endif
            }
         }
         else
         {
            retval = SYSTEM_ERROR;
            memset( errbuf_authid, 0x00, sizeof(errbuf_authid) );
            sprintf( errbuf_authid,
                    "Xipc error %d reading AuthNumber shared memory table.",
                     rcode );

            txcntl_log_message( 1, 2, errbuf_authid, "TxAuthorize_GetAuthId", 1 );
         }
      }
   }
   return( retval );	
}

/******************************************************************************
 *
 *  NAME:         TxAuthorize_GenerateNewAuthId
 *
 *  DESCRIPTION:  This function generates an authorization number.  It uses
 *                an existing authorization number as a starting point.  It
 *                increments the first character only after the last 5 have
 *                reached 99999.  The first character is alphanumeric
 *                ( 0, 1,..., 9, A, B,..., Y, Z ).  The last 5 characters
 *                are numeric.  These also get incremented, as a whole.
 *
 *                   Auth Number:  Annnnn
 *                      where
 *                                 A = 0 - Z (alphanumeric)
 *                                 n = 0 - 9 (numeric)
 *
 *                Never let it start with 'K'.  K is reserved for deferred
 *                and RSB transactions.  The authorizer takes care of that
 *                logic.
 *
 *  INPUTS:       pstrCurrentAuthId - An existing authorization number
 *
 *  OUTPUTS:      pstrNewAuthId - A new authorization number
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
BYTE TxAuthorize_GenerateNewAuthId(pCHAR pstrCurrentAuthId, pCHAR pstrNewAuthId)
{
   INT  nIndex;
   CHAR five_zeroes[] = "00000";
   CHAR temp_AuthId[7] ;
   CHAR strTemp[200];
   INT	nullFlag=0;

   /*+------------------------------------------------------------+
     | Verify the existing auth number contains valid characters. |
     | Replace with zeroes if not.                                |
     +------------------------------------------------------------+*/
	//copy old one
   strcpy(temp_AuthId,pstrCurrentAuthId);
   /* Verify first character to be alphanumeric ( upper case alpha ). */
   if ( isalnum(pstrCurrentAuthId[0]) )
   {
      /* Convert to UPPER CASE if it is a lower case letter. */
	 if ( islower(pstrCurrentAuthId[0]) )
         pstrCurrentAuthId[0] = toupper( pstrCurrentAuthId[0] );
	 /* TF Phani.. AUTH NUM should not have first one as '0' Zero*/
	 if ( strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) && pstrCurrentAuthId[0] == '0' ) /* TF Phani.. AUTH NUM should not have first one as 0 Zero*/
      pstrCurrentAuthId[0] = '1';
     
   }
   else if ( strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) ) /* TF Phani.. AUTH NUM should not have first one as 0 Zero*/
      pstrCurrentAuthId[0] = '1';
   else
	 pstrCurrentAuthId[0] = '0';
   /* Verify next five characters to be numeric. */
   for( nIndex=1; nIndex<=5; nIndex++ )
   {
      if ( !isdigit( pstrCurrentAuthId[nIndex] )  )
	  {/* TF PHANI .. Ressting AUTH_ID.. */
		sprintf (strTemp,"Restting to new,old AUTH_ID %s position %d RRN %s STAN %s  \n",temp_AuthId,nIndex,Auth_Tx.TLF01_details.retrieval_ref_num,Auth_Tx.TLF01_details.sys_trace_audit_num ) ;
		txcntl_log_message( 0, 2, strTemp, "TxAuthorize_GenerateNewAuthId", 0 );
        //pstrCurrentAuthId[nIndex] = '0';
		nullFlag=1; // Set flag for intilization
	  }
   }
	/* In above loop ,we are verfifying the last 5 digits of the AUTH_ID,
	   If invlid digits, Instead of Resetting to '0', call init AUTH_ID  */
	if(nullFlag == 1)
	{
		   if(TxDataServer_initAuthNumber() == false)
		  {
			sprintf(strTemp,"Not able to init Auth Number:TxDataServer_initAuthNumber failed");
			txcntl_log_message( 0, 2, strTemp, "TxAuthorize_GenerateNewAuthId", 0 );
		  }
		
		return false;
	}

   /*+----------------------------+
     | Generate a new Auth Number |
     +----------------------------+*/

   /* Increment last five characters, which are numeric. */
   for (nIndex = 5; nIndex > 0; nIndex--)
   {
      if ( pstrCurrentAuthId[nIndex] >= '9' )
         pstrCurrentAuthId[nIndex] = '0';

      else
      {
         pstrCurrentAuthId[nIndex] += 1;
         nIndex = 0;  /* Number has been incremented, so break the loop. */
      }
   }

   /* If the last five rolled over to '00000', increment the first character. */
   if ( 0 == strcmp(pstrCurrentAuthId+1, five_zeroes) )
   {
      /* Increment first character; it is alphanumeric. */
      if ( pstrCurrentAuthId[0] == '9' )
         pstrCurrentAuthId[0] = 'A';

      else if ( pstrCurrentAuthId[0] == 'Z' )
	  {
		  if ( strEqual(Auth_Tx.BIN01_details.card_family, CARD_AMEX) )
         pstrCurrentAuthId[0] = '1';
		  else
		 pstrCurrentAuthId[0] = '0';

	  }

      else
         pstrCurrentAuthId[0] += 1;
   }

   /* Never let it start with K, which is reserved for deferred and RSB txns.
    * Client wants to skip approval codes	starting with D,G,I,J,O,Q,S, and Y.
    */
   if ( pstrCurrentAuthId[0] == 'K' )
      pstrCurrentAuthId[0] = 'L';
	else if (pstrCurrentAuthId[0] == 'D')
		pstrCurrentAuthId[0] = 'E';
	else if (pstrCurrentAuthId[0] == 'G')
		pstrCurrentAuthId[0] = 'H';
	else if (pstrCurrentAuthId[0] == 'I')
		pstrCurrentAuthId[0] = 'L';
	else if (pstrCurrentAuthId[0] == 'J')
		pstrCurrentAuthId[0] = 'L';
	else if (pstrCurrentAuthId[0] == 'O')
		pstrCurrentAuthId[0] = 'P';
	else if (pstrCurrentAuthId[0] == 'Q')
		pstrCurrentAuthId[0] = 'R';
	else if (pstrCurrentAuthId[0] == 'S')
		pstrCurrentAuthId[0] = 'T';
	else if (pstrCurrentAuthId[0] == 'Y')
		pstrCurrentAuthId[0] = 'Z';
   strcpy (pstrNewAuthId, pstrCurrentAuthId) ;

   return true;
}

INT TxAuthorize_Check_TxnType_is_ChipFallBack()
{

	if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0)
	{
		if((strncmp(Auth_Tx.TLF01_details.message_type,"0200",4) == 0 || strncmp(Auth_Tx.TLF01_details.message_type,"0100",4) == 0) &&
		strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"08",2) == 0 )
		{
			return true;
		}
		else
		{
		 return false;
		}

	}
	else if((txservice_Check_If_VISA_issuing_transaction()==true) && txservice_Check_If_VISA_Transaction()==true)
	{

		if((strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"90",2) == 0 || strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"02",2) == 0 ) &&
		(strncmp(Auth_Tx.TLF01_details.product_codes[16].amount[2],'1',1) == 0 || strncmp(Auth_Tx.TLF01_details.product_codes[16].amount[2],'2',1) == 0))

		{
			return true;
		}
		else
		{

			return false;
		}
	}
	else if((txservice_Check_If_MC_issuing_transaction()==true) && txservice_Check_If_MC_Transaction()==true)
	{
		if(((strncmp(Auth_Tx.TLF01_details.pos_entry_mode+1,"80",2) == 0) && strncmp(Auth_Tx.TLF01_details.message_type,"0100",4) == 0))
		{
			return true;
		}
		else
		{
			return false;
		}


	}
	else if((txservice_Check_If_JCB_issuing_transaction()==true) && txservice_Check_If_JCB_Transaction()==true)
	{
		if(((0==strncmp(Auth_Tx.TLF01_details.pos_entry_mode+1,"97",2)  && strncmp(Auth_Tx.TLF01_details.message_type,"0100",4) == 0)))
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	else if((txservice_Check_If_AMEX_issuing_transaction()==true) && txservice_Check_If_AMEX_Transaction()==true)
	{
		if((strncmp(Auth_Tx.TLF01_details.pos_entry_mode+1,"80",2)==0 && strncmp(Auth_Tx.TLF01_details.message_type,"0100",4) == 0))
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	else if((txservice_Check_If_DCI_issuing_transaction()==true) && txservice_Check_If_DCI_Transaction()==true)
	{
		if((Auth_Tx.TLF01_details.product_codes[16].amount[6] == '9'  && (strncmp(Auth_Tx.TLF01_details.message_type,"0100",4) == 0)))
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	else if((txservice_Check_If_CUP_Transaction()==true) && txservice_Check_If_CUP_Transaction()==true)
	{
		if((strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"90",2) == 0 || strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"02",2) == 0 ) &&
		strncmp(Auth_Tx.TLF01_details.product_codes[16].amount[2],'2',1) == 0 )

		{
			return true;
		}
		else
		{

		return false;
		}

	}
	else
	{
		return false;
	}
}
