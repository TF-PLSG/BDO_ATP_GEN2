<?php

/**
 * @package		MVC
 * @author		Gopal Sikhawal
 */
define('AUTH_SALE', 1);   /*    0200        00 */

define('AUTH_REFUND', 2);   /*    0200        20 */

define('AUTH_CASH_ADVANCE', 3);   /*    0200        01 */

define('AUTH_PAYMENT', 4);   /*    0200        40 */

define('AUTH_OFFLINE_SALE', 5);   /*    0220        00 */

define('AUTH_OFFLINE_REFUND', 6);   /*    0220        20 */

define('AUTH_SALE_ADJUSTMENT', 7);   /*    0220        02 */

define('AUTH_REFUND_ADJUSTMENT', 8);  /*    0220        22 */

define('AUTH_PAYMENT_ADJUSTMENT', 9);   /*    0220        42 */

define('AUTH_PRE_AUTHORIZATION', 10);  /*    0100        30 */

define('AUTH_AUTHORIZATION', 11);  /*    0100        00 */

define('AUTH_BALANCE_INQUIRY', 12);  /*    0100        31 */

define('AUTH_VOID_SALE', 13);  /*    0200        02 */

define('AUTH_VOID_REFUND', 14);  /*    0200        22 */

define('AUTH_SETTLE', 15);  /*    0500        92 */

define('AUTH_BATCH_UPLOAD', 16);  /*    0320        xx */

define('AUTH_SETTLE_TRAILER', 17);  /*    0500        96 */

define('AUTH_STATISTICS', 18);  /*    0800        91 */

define('AUTH_REVERSAL', 19);  /*    0400        xx */

define('AUTH_LOGON', 20);  /*    0800        92 */

define('AUTH_TEST', 21);  /*    0800        99 */

define('AUTH_PIN_CHANGE', 22);  /*    0800        90 */

define('AUTH_VOID_CASH_ADVANCE', 23);  /*    0200        20 */

define('AUTH_SALE_CASH', 24);  /*    0200        09 */

define('AUTH_CHECK_VERIFICATION', 25);  /*    0100        04 */

define('AUTH_CARD_VERIFICATION', 26);  /*    0100        38 */

define('AUTH_RE_AUTH', 27);  /*    0100        02 */

define('AUTH_SALES_COMPLETION', 28);  /*    0200        20 */

define('AUTH_GUARANTEED_LATE_ARRIVAL', 29);  /*    0220        90 */

define('AUTH_BATCH_DOWN_LINE_LOAD', 30);  /*    0500        95 */

define('AUTH_SIGNATURE_DATA', 31);  /*    0320        90 */

define('AUTH_INITIALIZATION', 32);  /*    0800        93 */

define('AUTH_PLEASE_WAIT_ADVICE', 33);  /*    0830        90*/

define('AUTH_NEGATIVE_DOWNLOAD', 34);  /*    0200        20 */

define('AUTH_ADMIN_ADVICE', 35);

define('AUTH_VOID_PAYMENT', 36);

define('AUTH_REGISTRATION', 37);

define('AUTH_CHECK_DL', 38);

define('AUTH_BRANCH_PMT', 39);

define('AUTH_ATM_CONFIRM', 40);

define('AUTH_QUASI_CASH', 41);

define('AUTH_ADVICE', 42);

define('AUTH_REVERSAL_ADVICE', 43);  /*   0420                                       */

define('AUTH_CARD_UPDATE', 44);

define('AUTH_RECONCILE_ADVICE', 45);

define('AUTH_NETWORK_ADVICE', 46);

define('AUTH_ADVICE_REPEAT', 47);

define('AUTH_REVERSAL_REPEAT', 48);

define('AUTH_CASH_ADVANCE_AUTHORIZATION', 49);

define('AUTH_DEFERRED_PURCHASE_AUTHORIZATION', 50);

define('AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE', 51);

define('AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE', 52);

define('AUTH_AUTHORIZATION_OVERRIDE', 53);

define('AUTH_OFFLINE_VOID_SALE', 54);

define('AUTH_OFFLINE_VOID_REFUND', 55);

define('AUTH_ISSUER_REVERSAL_ADVICE', 56);  /* 0422/0432 - Cirrus */

define('AUTH_ISSUER_RECONCILE_ADVICE', 57);  /* 0522/0532 - Cirrus */

/*  VISA SMS messages */

define('AUTH_BACKOFFICE_ADJUSTMENT', 58);

define('AUTH_CHARGEBACK', 59);

define('AUTH_CHARGEBACK_REVERSAL', 60);

define('AUTH_REPRESENTMENT', 61);

define('AUTH_RECONCILIATION', 62);

define('AUTH_ADMINISTRATIVE_TEXT', 63);

define('AUTH_KEY_CHANGE', 64);

define('AUTH_CASH_DISBURSEMENT_ADJUSTMENT', 65);

define('AUTH_FUND_TRANSFER_TOTAL', 66);

define('AUTH_FEE_COLLECTION_ACQUIRER', 67);  /* 0220, 19 */

define('AUTH_FEE_COLLECTION_ISSUER', 68);  /* 0422, 19 */

define('AUTH_FUNDS_DISBURSEMENT_ACQUIRER', 69);  /* 0220, 29 */

define('AUTH_FUNDS_DISBURSEMENT_ISSUER', 70);  /* 0422, 29 */

/* Reload */

define('AUTH_RELOAD', 71);  /* 0200, 21 */

define('AUTH_RELOAD_REPEAT', 72);  /* 0201, 21 */

define('AUTH_RELOAD_CONFIRM', 73);  /* 0202, 21 */

define('AUTH_VOID_RELOAD', 74);  /* 0200, 22 */

define('AUTH_EMV_ADVICE', 75);  /* 0340,    */

define('AUTH_EMV_UPLOAD', 76);  /* 0320, 94 */

define('AUTH_REVERSAL_ADVICE_REPEAT', 77); /* 421, 00 TF-Ajay 14 Dec,2009*/

define('AUTH_OCT_TX', 78); /* 0200, 260000 */

define('AUTH_FAST_FUND', 79); /* 0200, 100000 */

define('AUTH_MONEY_SEND', 80); /* 0100 28*/

define('AUTH_INVALID', 99);

define('AUTH_FAST_FUND', 79);

define('AUTH_MONEY_SEND', 80);
 

/* used for message responses... tx_key */

define('AUTH_SALE_RESPONSE', AUTH_SALE + 100);              

define('AUTH_REFUND_RESPONSE', AUTH_REFUND + 100);

define('AUTH_CASH_ADVANCE_RESPONSE', AUTH_CASH_ADVANCE + 100);

define('AUTH_PAYMENT_RESPONSE', AUTH_PAYMENT + 100);

define('AUTH_OFFLINE_SALE_RESPONSE', AUTH_OFFLINE_SALE + 100);

define('AUTH_OFFLINE_REFUND_RESPONSE', AUTH_OFFLINE_REFUND + 100);

define('AUTH_SALE_ADJUSTMENT_RESPONSE', AUTH_SALE_ADJUSTMENT + 100);  

define('AUTH_REFUND_ADJUSTMENT_RESPONSE', AUTH_REFUND_ADJUSTMENT  + 100);

define('AUTH_PAYMENT_ADJUSTMENT_RESPONSE', AUTH_PAYMENT_ADJUSTMENT + 100);

define('AUTH_PRE_AUTHORIZATION_RESPONSE', AUTH_PRE_AUTHORIZATION  + 100);

define('AUTH_AUTHORIZATION_RESPONSE', AUTH_AUTHORIZATION + 100);

define('AUTH_BALANCE_INQUIRY_RESPONSE', AUTH_BALANCE_INQUIRY + 100);

define('AUTH_VOID_SALE_RESPONSE', AUTH_VOID_SALE + 100);

define('AUTH_VOID_REFUND_RESPONSE', AUTH_VOID_REFUND + 100);

define('AUTH_SETTLE_RESPONSE', AUTH_SETTLE + 100);

define('AUTH_BATCH_UPLOAD_RESPONSE', AUTH_BATCH_UPLOAD + 100);

define('AUTH_SETTLE_TRAILER_RESPONSE', AUTH_SETTLE_TRAILER + 100);

define('AUTH_STATISTICS_RESPONSE', AUTH_STATISTICS + 100);

define('AUTH_REVERSAL_RESPONSE',  AUTH_REVERSAL + 100);

define('AUTH_LOGON_RESPONSE', AUTH_LOGON + 100);

define('AUTH_TEST_RESPONSE', AUTH_TEST + 100);

define('AUTH_PIN_CHANGE_RESPONSE', AUTH_PIN_CHANGE + 100);

define('AUTH_NEGATIVE_DOWNLOAD_RESPONSE', AUTH_NEGATIVE_DOWNLOAD + 100);

define('AUTH_SALE_CASH_RESPONSE', AUTH_SALE_CASH + 100);

define('AUTH_CHECK_VERIFICATION_RESPONSE', AUTH_CHECK_VERIFICATION + 100);

define('AUTH_CARD_VERIFICATION_RESPONSE', AUTH_CARD_VERIFICATION  + 100);

define('AUTH_RE_AUTH_RESPONSE',  AUTH_RE_AUTH + 100);

define('AUTH_SALES_COMPLETION_RESPONSE', AUTH_SALES_COMPLETION + 100);

//define('AUTH_GUAREANTEED_LATE_ARRIVAL_RESPONSE', AUTH_GUAREANTEED_LATE_ARRIVAL + 100);

define('AUTH_BATCH_DOWN_LINE_LOAD_RESPONSE', AUTH_BATCH_DOWN_LINE_LOAD + 100);

define('AUTH_SIGNATURE_DATA_RESPONSE', AUTH_SIGNATURE_DATA + 100);

define('AUTH_INITIALIZATION_RESPONSE', AUTH_INITIALIZATION + 100);

define('AUTH_PLEASE_WAIT_ADVICE_RESPONSE', AUTH_PLEASE_WAIT_ADVICE + 100);

define('AUTH_VOID_CASH_ADVANCE_RESPONSE', AUTH_VOID_CASH_ADVANCE + 100);

define('AUTH_ADMIN_ADVICE_RESPONSE', AUTH_ADMIN_ADVICE + 100);

define('AUTH_VOID_PAYMENT_RESPONSE', AUTH_VOID_PAYMENT + 100);

define('AUTH_REGISTRATION_RESPONSE', AUTH_REGISTRATION + 100);

define('AUTH_CHECK_DL_RESPONSE', AUTH_CHECK_DL + 100);

define('AUTH_ATM_CONFIRM_RESPONSE', AUTH_ATM_CONFIRM + 100);

define('AUTH_QUASI_CASH_RESPONSE',  AUTH_QUASI_CASH + 100);

define('AUTH_ADVICE_RESPONSE', AUTH_ADVICE + 100);

define('AUTH_REVERSAL_ADVICE_RESPONSE', AUTH_REVERSAL_ADVICE + 100);

define('AUTH_CARD_UPDATE_RESPONSE', AUTH_CARD_UPDATE + 100);

define('AUTH_RECONCILE_ADVICE_RESPONSE', AUTH_RECONCILE_ADVICE  + 100);

define('AUTH_NETWORK_ADVICE_RESPONSE', AUTH_NETWORK_ADVICE + 100);

define('AUTH_ADVICE_REPEAT_RESPONSE', AUTH_ADVICE_REPEAT + 100);

define('AUTH_REVERSAL_REPEAT_RESPONSE', AUTH_REVERSAL_REPEAT + 100);

define('AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE', AUTH_CASH_ADVANCE_AUTHORIZATION + 100);

define('AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE', AUTH_DEFERRED_PURCHASE_AUTHORIZATION + 100);

define('AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE', AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE + 100);

define('AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE', AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE + 100);

define('AUTH_AUTHORIZATION_OVERRIDE_RESPONSE', AUTH_AUTHORIZATION_OVERRIDE + 100);

define('AUTH_OFFLINE_VOID_SALE_RESPONSE', AUTH_OFFLINE_VOID_SALE  + 100);

define('AUTH_OFFLINE_VOID_REFUND_RESPONSE', AUTH_OFFLINE_VOID_REFUND + 100);

define('AUTH_ISSUER_REVERSAL_ADVICE_RESPONSE', AUTH_ISSUER_REVERSAL_ADVICE  + 100);

define('AUTH_ISSUER_RECONCILE_ADVICE_RESPONSE', AUTH_ISSUER_RECONCILE_ADVICE + 100);

/* visa sms messages */

define('AUTH_BACKOFFICE_ADJUSTMENT_RESPONSE', AUTH_BACKOFFICE_ADJUSTMENT + 100);

define('AUTH_CHARGEBACK_RESPONSE', AUTH_CHARGEBACK + 100);

define('AUTH_CHARGEBACK_REVERSAL_RESPONSE', AUTH_CHARGEBACK_REVERSAL + 100);

define('AUTH_REPRESENTMENT_RESPONSE', AUTH_REPRESENTMENT + 100);

define('AUTH_RECONCILIATION_RESPONSE', AUTH_RECONCILIATION + 100);

define('AUTH_ADMINISTRATIVE_TEXT_RESPONSE', AUTH_ADMINISTRATIVE_TEXT + 100);

define('AUTH_KEY_CHANGE_RESPONSE', AUTH_KEY_CHANGE + 100);

define('AUTH_CASH_DISBURSEMENT_ADJUSTMENT_RESPONSE', AUTH_CASH_DISBURSEMENT_ADJUSTMENT + 100);

define('AUTH_FUND_TRANSFER_TOTAL_RESPONSE', AUTH_FUND_TRANSFER_TOTAL + 100);

define('AUTH_FEE_COLLECTION_ACQUIRER_RESPONSE', AUTH_FEE_COLLECTION_ACQUIRER + 100);

define('AUTH_FEE_COLLECTION_ISSUER_RESPONSE', AUTH_FEE_COLLECTION_ISSUER + 100);

define('AUTH_FUNDS_DISBURSEMENT_ACQUIRER_RESPONSE', AUTH_FUNDS_DISBURSEMENT_ACQUIRER + 100);

define('AUTH_FUNDS_DISBURSEMENT_ISSUER_RESPONSE', AUTH_FUNDS_DISBURSEMENT_ISSUER  +100);

/* Reload Responses */

define('AUTH_RELOAD_RESPONSE', AUTH_RELOAD + 100);

define('AUTH_RELOAD_REPEAT_RESPONSE', AUTH_RELOAD_REPEAT  + 100);

define('AUTH_RELOAD_CONFIRM_RESPONSE', AUTH_RELOAD_CONFIRM + 100);

define('AUTH_VOID_RELOAD_RESPONSE', AUTH_VOID_RELOAD + 100);

define('AUTH_EMV_ADVICE_RESPONSE', AUTH_EMV_ADVICE + 100);

define('AUTH_EMV_UPLOAD_RESPONSE', AUTH_EMV_UPLOAD + 100);

define('AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE', AUTH_REVERSAL_ADVICE_REPEAT +100); /* 421, 00 TF-Ajay 14 Dec,2009*/

define('AUTH_OCT_TX_RESPONSE', AUTH_OCT_TX + 100);

define('AUTH_FAST_FUND_RESPONSE', AUTH_FAST_FUND  + 100);

define('AUTH_MONEY_SEND_RESPONSE', AUTH_MONEY_SEND + 100);

define(' AUTH_TX_KEY_MAX', 199); 


class Transaction_Model extends MVC_Model
{
	/* used for message requests... tx_key *//* .... stay between 1 and 99 .... */
                                           /*   msg_type    procode[0] */

	     

    public static function getTransactionType($data) {
	
		$tx_key = ord($data['TX_KEY']);
		if($tx_key > 100 ) 	{
			$strResponse = " RESPONSE";
		} else {
			$strResponse = "";
		}
		
		switch ($tx_key) {
			case AUTH_INVALID:
				$TransactionType = "INVALID" . $strResponse;
				break;
		
			case AUTH_SALE:
			case AUTH_SALE_RESPONSE:
				$TransactionType = "SALE" . $strResponse;
				break;
		
			case  AUTH_REFUND:
			case  AUTH_REFUND_RESPONSE:
				$TransactionType = "REFUND" . $strResponse; 
				break;
		
			case  AUTH_CASH_ADVANCE:
			case  AUTH_CASH_ADVANCE_RESPONSE:
				$TransactionType = "CASH ADVANCE" . $strResponse; 
				break;
		
			case AUTH_PAYMENT:
			case AUTH_PAYMENT_RESPONSE:
				$TransactionType = "PAYMENT" . $strResponse; 
				break;
		
			case AUTH_OFFLINE_SALE:
			case AUTH_OFFLINE_SALE_RESPONSE:
				$TransactionType = "OFFLINE SALE" . $strResponse; 
				break;
		
			case AUTH_OFFLINE_REFUND:
			case AUTH_OFFLINE_REFUND_RESPONSE:
				$TransactionType = "OFFLINE REFUND" . $strResponse; 
				break;
		
			case AUTH_SALE_ADJUSTMENT:
			case AUTH_SALE_ADJUSTMENT_RESPONSE:
				$TransactionType = "SALE ADJUSTMENT" . $strResponse; 
				break;
		
			case AUTH_REFUND_ADJUSTMENT:
			case AUTH_REFUND_ADJUSTMENT_RESPONSE:
				$TransactionType = "REFUND ADJUSTMENT" . $strResponse; 
				break;
		
			case AUTH_PAYMENT_ADJUSTMENT:
			case AUTH_PAYMENT_ADJUSTMENT_RESPONSE:
				$TransactionType = "PAYMENT ADJUSTMENT" . $strResponse; 
				break;
		
			case AUTH_PRE_AUTHORIZATION:
			case AUTH_PRE_AUTHORIZATION_RESPONSE:
				$TransactionType = "PRE AUTHORIZATION" . $strResponse; 
				break;
		
			case AUTH_AUTHORIZATION:
			case AUTH_AUTHORIZATION_RESPONSE:
			
			case AUTH_MONEY_SEND:
			case AUTH_MONEY_SEND_RESPONSE:
			
			case AUTH_FAST_FUND:
			case AUTH_FAST_FUND_RESPONSE:
			{
				if (strcmp($data['PROCESSING_CODE'], "180000" ) == 0)
				{
					$TransactionType = "QUASI CASH" . $strResponse;
				}
				else
				{
					if ((strcmp($data['VOICE_AUTH'], "Y") == 0) || (strcmp($data['VOICE_AUTH'], "y") == 0))
					{
						$TransactionType = "VOICE AUTHORIZATION" . $strResponse; 
					}
					else
					{
						$TransactionType = "AUTHORIZATION" . $strResponse; 
					}
				}
			}
				break;
		
			case AUTH_BALANCE_INQUIRY:
			case AUTH_BALANCE_INQUIRY_RESPONSE:
				$TransactionType = "BALANCE INQUIRY" . $strResponse; 
				break;
		
			case AUTH_VOID_SALE:
			case AUTH_VOID_SALE_RESPONSE:
				$TransactionType = "VOID SALE" . $strResponse; 
				break;
		
			case AUTH_VOID_REFUND:
			case AUTH_VOID_REFUND_RESPONSE:
				$TransactionType = "VOID REFUND" . $strResponse; 
				break;
		
			case AUTH_SETTLE:
			case AUTH_SETTLE_RESPONSE:
				$TransactionType = "SETTLE" . $strResponse; 
				break;
		
			case AUTH_BATCH_UPLOAD:
			case AUTH_BATCH_UPLOAD_RESPONSE:
				$TransactionType = "BATCH UPLOAD" . $strResponse; 
				break;
		
			case AUTH_SETTLE_TRAILER:
			case AUTH_SETTLE_TRAILER_RESPONSE:
				$TransactionType = "SETTLE TRAILER" . $strResponse; 
				break;
		
			case AUTH_STATISTICS:
			case AUTH_STATISTICS_RESPONSE:
				$TransactionType = "STATISTICS" . $strResponse; 
				break;
		
			case AUTH_REVERSAL:
			case AUTH_REVERSAL_RESPONSE:
		
				$TransactionType = "REVERSAL - " ;
		
				if ( strcmp ($data['PROCESSING_CODE'], "700000" ) == 0)
					$TransactionType .="DEFERRED";
		
				else if( strcmp ($data['PROCESSING_CODE'], "180000" ) == 0)
					$TransactionType .="QUASI CASH";
		
				else if((strcmp($data['PROCESSING_CODE'],"000000") == 0) && (strcmp($data['ORIG_MESSAGE'], "0100" ) == 0))
					$TransactionType .="AUTHORIZATION";
		
				else if((strcmp($data['PROCESSING_CODE'],"000000") == 0) && (strcmp($data['ORIG_MESSAGE'], "0200" ) == 0))
					$TransactionType .="PURCHASE";
		
				else if((strcmp($data['PROCESSING_CODE'],"000000") == 0) && (strcmp($data['ORIG_MESSAGE'], "0220" ) == 0))
					$TransactionType .="SALE COMPLETION";
		
				else if( strcmp ($data['PROCESSING_CODE'], "200000" ) == 0) 
					$TransactionType .="REFUND";
		
				else if( strcmp ($data['PROCESSING_CODE'], "010000" ) == 0) 
					$TransactionType .="CASH";
		
				else if((strcmp($data['PROCESSING_CODE'],"020000") == 0) && (strcmp($data['ORIG_MESSAGE'], "0200" ) == 0))
					$TransactionType .="VOID SALE";
		
				else if((strcmp($data['PROCESSING_CODE'],"220000") == 0) && (strcmp($data['ORIG_MESSAGE'], "0200" ) == 0))
					$TransactionType .="VOID REFUND";
		
				else if((strcmp($data['PROCESSING_CODE'],"020000") == 0) && (strcmp($data['ORIG_MESSAGE'], "0220" ) == 0))
					$TransactionType .="SALE ADJUSTMENT";
		
				else if((strcmp($data['PROCESSING_CODE'],"220000") == 0) && (strcmp($data['ORIG_MESSAGE'], "0220" ) == 0))
					$TransactionType .="REFUND";
		
				if (empty($data['ORIG_MESSAGE'])) {
					/* No original transaction for this reversal. */
					$TransactionType .=" (without updates)";
				} else {
					$TransactionType .=" (with updates)";
				}
		
				break;
		
			case AUTH_LOGON:
			case AUTH_LOGON_RESPONSE:
				$TransactionType = "LOGON" . $strResponse; 
				break;
		
			case AUTH_TEST:
			case AUTH_TEST_RESPONSE:
				$TransactionType = "TEST" . $strResponse; 
				break;
		
			case AUTH_PIN_CHANGE:
			case AUTH_PIN_CHANGE_RESPONSE:
				$TransactionType = "PIN CHANGE" . $strResponse; 
				break;
		
			case AUTH_VOID_CASH_ADVANCE:
			case AUTH_VOID_CASH_ADVANCE_RESPONSE:
				$TransactionType = "VOID CASH ADVANCE" . $strResponse; 
				break;
		
			case AUTH_SALE_CASH:
			case AUTH_SALE_CASH_RESPONSE:
				$TransactionType = "SALE CASH" . $strResponse; 
				break;
		
			case AUTH_CHECK_VERIFICATION:
			case AUTH_CHECK_VERIFICATION_RESPONSE:
				$TransactionType = "CHECK VERIFICATION" . $strResponse; 
				break;
		
			case AUTH_CARD_VERIFICATION:
			case AUTH_CARD_VERIFICATION_RESPONSE:
				$TransactionType = "CARD VERIFICATION" . $strResponse; 
				break;
		
			case AUTH_RE_AUTH:
			case AUTH_RE_AUTH_RESPONSE:
				$TransactionType = "RE AUTH" . $strResponse; 
				break;
		
			case AUTH_SALES_COMPLETION:
			case AUTH_SALES_COMPLETION_RESPONSE:
				$TransactionType = "SALES COMPLETION" . $strResponse; 
				break;
				
			case AUTH_GUARANTEED_LATE_ARRIVAL:
				$TransactionType = "GUARANTEED LATE ARRIVAL" . $strResponse; 
				break;
				
			case AUTH_BATCH_DOWN_LINE_LOAD:
			case AUTH_BATCH_DOWN_LINE_LOAD_RESPONSE:
				$TransactionType = "BATCH DOWN LINE LOAD" . $strResponse; 
				break;
				
			case AUTH_SIGNATURE_DATA:
			case AUTH_SIGNATURE_DATA_RESPONSE:
				$TransactionType = "SIGNATURE DATA" . $strResponse; 
				break;
				
			case AUTH_INITIALIZATION:
			case AUTH_INITIALIZATION_RESPONSE:
				$TransactionType = "INITIALIZATION" . $strResponse; 
				break;
				
			case AUTH_PLEASE_WAIT_ADVICE:
			case AUTH_PLEASE_WAIT_ADVICE_RESPONSE:
				$TransactionType = "PLEASE WAIT ADVICE" . $strResponse; 
				break;
				
			case AUTH_NEGATIVE_DOWNLOAD:
			case AUTH_NEGATIVE_DOWNLOAD_RESPONSE:
				$TransactionType = "NEGATIVE DOWNLOAD" . $strResponse; 
				break;
				
			case AUTH_ADMIN_ADVICE:
			case AUTH_ADMIN_ADVICE_RESPONSE:
				$TransactionType = "ADMIN ADVICE" . $strResponse; 
				break;
				
			case AUTH_VOID_PAYMENT:
			case AUTH_VOID_PAYMENT_RESPONSE:
				$TransactionType = "VOID PAYMENT" . $strResponse; 
				break;
				
			case AUTH_REGISTRATION:
			case AUTH_REGISTRATION_RESPONSE:
				$TransactionType = "REGISTRATION" . $strResponse; 
				break;
				
			case AUTH_CHECK_DL:
			case AUTH_CHECK_DL_RESPONSE:
				$TransactionType = "CHECK DL" . $strResponse; 
				break;
				
			case AUTH_BRANCH_PMT: 
				$TransactionType = "BRANCH PMT" . $strResponse; 
				break;
				
			case AUTH_QUASI_CASH:
			case AUTH_QUASI_CASH_RESPONSE: 
				$TransactionType = "QUASI CASH" . $strResponse; 
				break;
				
			case AUTH_CASH_ADVANCE_AUTHORIZATION:
			case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE: 
				$TransactionType = "CASH ADVANCE AUTHORIZATION" . $strResponse; 
				break;
				
			case AUTH_DEFERRED_PURCHASE_AUTHORIZATION: 
			case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE: 
				$TransactionType = "DEFERRED PURCHASE" . $strResponse; 
				break;
				
			case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
			case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE:
				$TransactionType = "DEFERRED PURCHASE OVERRIDE" . $strResponse; 
				break;
				
			case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
			case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE:
				$TransactionType = "CASH ADVANCE AUTHORIZATION OVERRIDE" . $strResponse;
				break;
				
			case AUTH_AUTHORIZATION_OVERRIDE:
			case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE:
				$TransactionType = "AUTHORIZATION OVERRIDE" . $strResponse; 
				break;
				
			case AUTH_ADVICE:
			case AUTH_ADVICE_RESPONSE:
				$TransactionType = "ADVICE" . $strResponse; 
				break;
				
			case AUTH_REVERSAL_ADVICE:
			case AUTH_REVERSAL_ADVICE_RESPONSE:
				$TransactionType = "REVERSAL ADVICE" . $strResponse; 
				break;
				
			case AUTH_CARD_UPDATE:
			case AUTH_CARD_UPDATE_RESPONSE:
				$TransactionType = "CARD UPDATE" . $strResponse; 
				break;
				
			case AUTH_RELOAD:
			case AUTH_RELOAD_RESPONSE:
				$TransactionType = "CASH LOAD" . $strResponse;
				break;
				
			case AUTH_RELOAD_REPEAT:
			case AUTH_RELOAD_REPEAT_RESPONSE:
				$TransactionType = "CASH LOAD REPEAT" . $strResponse;
				break;
				
			case AUTH_RELOAD_CONFIRM:
			case AUTH_RELOAD_CONFIRM_RESPONSE:
				$TransactionType = "CASH LOAD CONFIRM" . $strResponse;
				break;
				
			case AUTH_VOID_RELOAD:
			case AUTH_VOID_RELOAD_RESPONSE:
				$TransactionType = "CASH LOAD VOID" . $strResponse;
				break;
				
			case AUTH_EMV_ADVICE:
			case AUTH_EMV_ADVICE_RESPONSE:
				$TransactionType = "EMV SCRIPT RESULTS" . $strResponse;
				break;
				
			case AUTH_EMV_UPLOAD:
			case AUTH_EMV_UPLOAD_RESPONSE:
				$TransactionType = "TC UPLOAD" . $strResponse;
				break;
		}
		return $TransactionType;
	}
	function process_data($data) {
		
		 
		
		if(!empty($data)) {
			if(isset($_SESSION['search']['UNMASK']) && $_SESSION['search']['UNMASK'] == 1) {
			} elseif(isset($data['CARD_NUM']) && !empty($data['CARD_NUM'])) {
				$data['CARD_NUM'] = substr($data['CARD_NUM'], 0, 6) . str_repeat("X", strlen($data['CARD_NUM']) - 10) . substr($data['CARD_NUM'], -4);
			}
		}
		if(!empty($data["TOTAL_AMOUNT"])) {
			$data["TOTAL_AMOUNT"] = ltrim($data["TOTAL_AMOUNT"], 0);
			$data["TOTAL_AMOUNT"] = substr_replace($data["TOTAL_AMOUNT"], '.', -2, 0);
		}
		if(!empty($data["ORIG_AMOUNT"])) {
			$data["ORIG_AMOUNT"] = ltrim($data["ORIG_AMOUNT"], 0);
			$data["ORIG_AMOUNT"] = substr_replace($data["ORIG_AMOUNT"], '.', -2, 0);
		}
		if(!empty($data["CREDIT_LIMIT"])) {
			$data["CREDIT_LIMIT"] = ltrim($data["CREDIT_LIMIT"], 0);
			$data["CREDIT_LIMIT"] = substr_replace($data["CREDIT_LIMIT"], '.', -2, 0);
		}
		if(!empty($data["OUTSTANDING_BALANCE"])) {
			$data["OUTSTANDING_BALANCE"] = ltrim($data["OUTSTANDING_BALANCE"], 0);
			$data["OUTSTANDING_BALANCE"] = substr_replace($data["OUTSTANDING_BALANCE"], '.', -2, 0);
		}
		if(!empty($data["PRODUCT10_AMOUNT"])) {
			$data["PRODUCT10_AMOUNT"] = ltrim($data["PRODUCT10_AMOUNT"], 0);
			$data["PRODUCT10_AMOUNT"] = substr_replace($data["PRODUCT10_AMOUNT"], '.', -2, 0);
		}
		if(!empty($data['DATE_YYYYMMDD'])) {
			$data['DATE_YYYYMMDD'] = date('Y-m-d', strtotime($data['DATE_YYYYMMDD']));
		}
		if(!empty($data['TIME_HHMMSS'])) {
			$data['TIME_HHMMSS'] = substr($data['TIME_HHMMSS'], 0, 2).':'.substr($data['TIME_HHMMSS'], 2, 2).':'.substr($data['TIME_HHMMSS'], 4, 2);
		}
		$host_start_time = $data['HOST_START_TIME'];
		

		if($host_start_time == '5.000')
		{
			$data['EarMarkedCurrency'] = 'PHP';
			$data['EarMarkedAmt'] = $data['PRODUCT10_AMOUNT'];
		}
		else if($host_start_time == '7.000')
		{
			$data['EarMarkedCurrency'] = 'USD';
			$data['EarMarkedAmt'] = $data['PRODUCT10_AMOUNT'];
		}
		else
		{
			$data['EarMarkedCurrency'] = $data["CURRENCY_CODE"];
			$data['EarMarkedAmt'] = $data["TOTAL_AMOUNT"];
		}
		
		if((substr($data['HANDLER_QUEUE'],0,3) == 'dcp') ||
	    (substr($data['HANDLER_QUEUE'],0,3) == 'DCP'))
		{
			$data['NII'] =  $data["NII"] ;
		}
		else
		{
			$data['NII'] = ' ';
		}
		
		
		
		if ($data["PRODUCT4_QUANTITY"] == "RP")
		{
			//C Start
			if((true == self::Check_If_VISA_Transaction($data['CARD_NUM'])) ||
			   (true ==	self::Check_If_MC_Transaction($data['CARD_NUM'])))
			{
				$data['ORG_TXN_AMT'] = $data["ORIG_AMOUNT"];
				$data['REVERSAL_AMOUNT'] = $data["TOTAL_AMOUNT"];
				$data['TOTAL_AMOUNT'] = $data["ORIG_AMOUNT"];
			}
			else
			{
				$data['ORG_TXN_AMT'] = $data["TOTAL_AMOUNT"];
			}
			$data['ORG_TXN_CUR'] = $data["CURRENCY_CODE"];
			
		//end 
		}
		
		elseif ($data['HANDLER_QUEUE'] == 'dcpiso')
		{
			$data['ORG_TXN_AMT'] = $data["TOTAL_AMOUNT"];
			$data['ORG_TXN_CUR'] = $data["CURRENCY_CODE"];
		}
		
		
		else {
			if ($data["PRODUCT5_CODE"] == "ISSUE") {
				$data['ORG_TXN_AMT'] = $data["SALES_AMOUNT"];
				$data['ORG_TXN_CUR'] = $data["NFI_SEQ_NBR"];
			} else {
				$data['ORG_TXN_AMT'] = $data["TOTAL_AMOUNT"];
				$data['ORG_TXN_CUR'] = $data["CURRENCY_CODE"];
			}
		}
		if(!empty($data['DEFERRED_TERM_LENGTH'])) {
			if(!empty($data['DEF_GROSS_AMT'])) {
				$data['DEF_GROSS_AMT'] = ltrim($data['DEF_GROSS_AMT'], 0);
				$data['DEF_GROSS_AMT'] = substr_replace($data['DEF_GROSS_AMT'], '.', -2, 0);
			}
			if(!empty($data['MONTHLY_AMORT_AMT'])) {
				$data['MONTHLY_AMORT_AMT'] = ltrim($data['MONTHLY_AMORT_AMT'], 0);
				$data['MONTHLY_AMORT_AMT'] = substr_replace($data['MONTHLY_AMORT_AMT'], '.', -2, 0);
			}
			$data['DEFERRED_FACTOR'] = $data['DEFERRED_FACTOR']/10000;
	
			$data['DEFERRED_FACTOR'] = sprintf("%8.4f", $data['DEFERRED_FACTOR']);
	
		}
		
		
		if((true ==	self::Check_If_MC_Transaction($data['CARD_NUM'])) &&
			!(empty($data['PROFILE'])))
		{
			$data['PROFILE'] = '2'.$data['PROFILE'];
		}
		elseif(strcmp($data['POS_CONDITION_CODE'], "59" ) == 0) 
		{
			$data['PROFILE'] = '2'.$data['PROFILE'];
		}
		
		if((true == self::Check_If_MC_Transaction($data['CARD_NUM'])) && 
	    ((substr($data['HANDLER_QUEUE'],0,6) == "DCPISO")||
		(substr($data['HANDLER_QUEUE'],0,6) == "dcpiso")))
		{
			$data['PROFILE_1'] = '2'.$data['PRODUCT13_QUANTITY'];
		}
		else
		{
			$data['PROFILE_1'] = $data['PROFILE'];
		}
		
		
		$temp_OriginalECI_Reason = $data['PRODUCT15_CODE'];
		$m_StrOriginalECI_Reason = substr($temp_OriginalECI_Reason, 0, 3) ;
 
		$OriginalECI_Reason_len = strlen($m_StrOriginalECI_Reason);
		if($OriginalECI_Reason_len > 0) {
			$m_StrOriginalECI_Reason = $m_StrOriginalECI_Reason .'/'. $data['PRODUCT15_QUANTITY'];
		}
		$data['ORG_ECI_REASON'] = $m_StrOriginalECI_Reason;
		$data['TransactionType'] = self::getTransactionType($data);
		
		if((substr($data['MESSAGE_TYPE'],0,4) == "0120")||
		   (substr($data['MESSAGE_TYPE'],0,4) == "1120")||
		   (substr($data['MESSAGE_TYPE'],0,4) == "0420")||
		   (substr($data['MESSAGE_TYPE'],0,4) == "1420"))
		{
			//echo "inside";
			//return "inside";
					/*echo "HI";
					exit;*/
			if(	self::Check_If_AMEX_Transaction($data['CARD_NUM']) == true)
			{
					//error_log(print_r($data['PRODUCT14_CODE'], TRUE)); 
					//error_log ($data['PRODUCT14_CODE'], 3, 'D:\php-5.3.5_errors_test.log');
					 $amex = self::match_ISO_to_AMEX_response_code($data['PRODUCT14_QUANTITY'],$data['TX_KEY']);
					 $data['RESPONSE_CODE_BY_SCHEME'] = $amex;
								
			}
			else if( self::Check_If_Diners_Transaction($data['CARD_NUM']) == true)
			{
					$dci_rescode = self::match_ISO_to_Diners_response_code($data['PRODUCT14_QUANTITY']);
					$data['RESPONSE_CODE_BY_SCHEME'] = $dci_rescode;		
			}
			else 
			{
				    $data['RESPONSE_CODE_BY_SCHEME'] = $data['PRODUCT14_QUANTITY'];
			}
		}
		
		
		
		
		//$data['TRAN_ID'] = self::getTranID($data['CARD_NUM'], $data);
		$data['TRAN_ID'] = $tranid = self::getTranID($data['CARD_NUM'], $data);
		$data['TRAN_ID1'] = $tranid[0];
		$data['TRAN_ID2'] = $tranid[1];
		
		$data['ACTION_CODE'] = self::getActionCode($data['CARD_NUM'], $data);
		
		$data['CUP_INDICATOR'] = $data['PRODUCT_CODE'];
		
		$data['VISA_MERCHANT_IDENTIFIER'] = $data['PRODUCT5_CODE'];	

		$data['DIGITAL_COMM_PROGRAM_INDICATOR'] = $data['PRODUCT9_CODE'];
		
		$data['DIGITAL_ENTRY_IDENTIFIER'] = $data['PRODUCT6_CODE'];
		
		$data['POS_DATA_EXTENDED_COND_CODE'] = self::getPosExtCondCode($data['CARD_NUM'], $data);
		
		$data['POS_ENTRY_MODE_1'] = self::getPosEntryMode1($data['CARD_NUM'], $data);
		
		$data['VOICE_AUTH_TEXT'] = /*self::getCAVVfield($data['CARD_NUM'], $data['MESSAGE_TYPE'],$data)*/
		$temp = self::getMP_Txn_Details($data['CARD_NUM'], $data['MESSAGE_TYPE'],$data['ACQUIRER_ID'],$data['AUTHORIZING_HOST_QUEUE'], $data);
		$data['VOICE_AUTH_TEXT1'] = $temp[0];
		$data['VOICE_AUTH_TEXT2'] = $temp[1];
			
									//self::get126Field_Details(&$data);
								
									

								
		return $data;
	}		
	

	
	/*public static function get126Field_Details(&$data1) 
	{
			$Text_Field = " ";
			if(!(empty($data1['PRODUCT1_CODE'])))
			{
				$Text_Field =  $Text_Field."<br> De126.9(Position 2) Authentication Method = ".substr($data1['PRODUCT1_CODE'],0,2);
				//var_dump($Text_Field);die;
				$Text_Field = $Text_Field."<br> De126.20 3D-Secure Indicator = ".substr($data1['PRODUCT1_CODE'],2,1);
				return $Text_Field;
			}
	}*/

	
	public static function getActionCode($card_num, &$data) 
	{
		if(substr($card_num, 0, 1) == '3' &&  (substr($card_num, 1, 1) == '4' || substr($card_num, 1, 1) == '7'))
		{
			return $data['PRODUCT13_CODE'];
		}
		 elseif((substr($card_num, 0, 1) == '3')&&  (substr($card_num, 1, 1) == '6' || substr($card_num, 1, 1) == '8'
			||substr($card_num, 1, 1) == '0' || substr($card_num, 1, 1) == '9'))
		{
			return $data['PRODUCT13_CODE'];
		}	
		elseif((substr($card_num, 0, 1) == '6')&&  (substr($card_num, 1, 1) == '0' || substr($card_num, 1, 1) == '4'
			||substr($card_num, 1, 1) == '5' ))
		{
			return $data['PRODUCT13_CODE'];
		}
	}
	
	public static function getTranID($card_num, &$data) 	
	{
		$Bin_Low = 222100; $Bin_High = 272099;
		$MC_bin_value = substr($card_num, 0, 6);
		$Tran_ID1 = " ";
		$Tran_ID2 = " ";
		
		
		if(substr($card_num, 0, 1) == '3' &&  (substr($card_num, 1, 1) == '4' || substr($card_num, 1, 1) == '7')) 
    {
			$Tran_ID1 = $data['VISA_TRAN_ID'];
		} 
    elseif(substr($card_num, 0, 1) == '4' ) 
    {
			$Tran_ID1 = $data['VISA_TRAN_ID'];	
		}
    elseif((substr($card_num, 0, 1) == '5') || (($MC_bin_value <= $Bin_High) && ($MC_bin_value >= $Bin_Low)) ) 
    {
			$Tran_ID1 = $data['MCARD_BANKNET'];
		}
    elseif((substr($card_num, 0, 1) == '3')&&  (substr($card_num, 1, 1) == '6' || substr($card_num, 1, 1) == '8'
			||substr($card_num, 1, 1) == '0' || substr($card_num, 1, 1) == '9'))
		{
			$Tran_ID1 = $data['RESP_SOURCE'];
		}
		elseif((substr($card_num, 0, 1) == '6')&&  (substr($card_num, 1, 1) == '0' || substr($card_num, 1, 1) == '4'
			||substr($card_num, 1, 1) == '5' ))
		{
			$Tran_ID1 = $data['RESP_SOURCE'];
		}
		
	/*if(!(empty($data['PRODUCT18_CODE'])))
		{
			$Tran_ID2 = substr($data['PRODUCT18_CODE'],0,6)."/";
		}
		if(!(empty($data['PRODUCT18_AMOUNT'])))
		{
			$Tran_ID2 = $Tran_ID2 . substr($data['PRODUCT18_AMOUNT'],0,12);
		}
		if(!(empty($data['PRODUCT19_AMOUNT'])))
		{
			$Tran_ID2 = $Tran_ID2 . substr($data['PRODUCT19_AMOUNT'],0,12);
		}
		if(!(empty($data['PRODUCT19_CODE'])))
		{
			$Tran_ID2 = $Tran_ID2 . substr($data['PRODUCT19_CODE'],0,1);
		}*/
		$TranID_Field = array($Tran_ID1);
		//var_dump($Tran_ID1).var_dump($Tran_ID2);die;
		
		return $TranID_Field;
	}
	
	public static function getPosExtCondCode($card_num, &$data) 
	{
		$Bin_Low = 222100; $Bin_High = 272099;
		$MC_bin_value = substr($card_num, 0, 6);
		
		if((substr($card_num, 0, 1) == '5') || (($MC_bin_value <= $Bin_High) && ($MC_bin_value >= $Bin_Low)) ) 
		{
			return substr($data['PRODUCT14_AMOUNT'],4,8);
		}
	}
	
	public static function getPosEntryMode1($card_num, &$data) 
	{
		
		if(substr($card_num, 0, 1) == '4')
		{
			return $data['PRODUCT18_CODE'];
		}
		else
		{
			return $data['PRODUCT10_CODE'];
		}
	}
	
	public static function Check_If_VISA_Transaction($card_num)
	{
		if(substr($card_num, 0, 1) == '4' ) 
		{
			return true;
		}
		else
		{
			return false;
		}
	}  
	public static function Check_If_JCB_Transaction($card_num)
	{
		if((substr($card_num, 0, 1) == '3' )||
		(substr($card_num, 0, 1) == '5' ))
		{
			return true;
		}
		else
		{
			return false;
		}
	}  
	
	public static function Check_If_MC_Transaction($card_num)
	{
		$Bin_Low = 222100; $Bin_High = 272099;
		$MC_bin_value = substr($card_num, 0, 6);
		
		if(substr($card_num, 0, 1) == '5' || (($MC_bin_value <= $Bin_High) && ($MC_bin_value >= $Bin_Low))) 
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
		public static function Check_If_AMEX_Transaction($card_num)
		{	
			//echo $card_num;
			//exit;
			
			if((substr($card_num, 0, 1) == '3')&&
			   ((substr($card_num, 1, 1) == '4')||
			   (substr($card_num, 1, 1) == '7'))) 
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		
		public static function Check_If_Diners_Transaction($card_num)
		{		
			if((substr($card_num, 0, 1) == '3')&& 
			   ((substr($card_num, 1, 1) == '0')||
			   (substr($card_num, 1, 1) == '6')||
			   (substr($card_num, 1, 1) == '8')||
			   (substr($card_num, 1, 1) == '9'))) 
			{
				return true;
			}
			elseif((substr($card_num, 0, 1) == '6')&& 
			       ((substr($card_num, 1, 1) == '0')||
			       (substr($card_num, 1, 1) == '4')||
			       (substr($card_num, 1, 1) == '5')))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	
	// public static function getCAVVfield($card_num, $MessageType, &$data) 
	// {
		// if(substr($card_num, 0, 1) == '4' ) 
		// {
			// if((substr($MessageType,0,4) == "0100")||(substr($MessageType,0,4) == "0200")||(substr($MessageType,0,4) == "0220"))
			// {
				// if(!(empty($data['CVC'])))
				// {
						// return "CAVV Results Code:".$data['CVC'];
				// }
			// }
		// }
		
	// }
	
	public static function getMP_Txn_Details($card_num, $MessageType, $Acquirer_ID, $Authorizing_host_queue, &$data) 
	{
			$Voice_Auth_Text = " ";
			$Text_Field = " ";
			$CAVV_Components = " ";
			
			 //$object = new stdClass();
			 
			if(true ==	self::Check_If_MC_Transaction($data['CARD_NUM']))
			{
				if(!(empty($data['PRODUCT12_QUANTITY'])))
				{
					$Voice_Auth_Text = $Voice_Auth_Text."DE48 SF66 SF1 = "." ".substr($data['PRODUCT12_QUANTITY'],0,1);
				}
				if(!(empty($data['WORKSTATION'])))
				{
					$Voice_Auth_Text = $Voice_Auth_Text."<br> DE48 SF66 SF2 = "." ".($data['WORKSTATION'])." ".($data['OPERATOR_ID']);
				}
				if(!(empty($data['PRODUCT6_QUANTITY'])))
				{
					$Voice_Auth_Text = $Voice_Auth_Text."<br> DE48 SF43:".$data['PRODUCT6_QUANTITY'];
				}
				if(!(empty($data['ORIG_AUTH_NUMBER'])))
				{
					$Voice_Auth_Text = $Voice_Auth_Text."<br> Merchant Country of Origin value = ".$data['ORIG_AUTH_NUMBER'];
				}   
				
				if(!(empty($data['DOWN_PAYMENT']))&& !(empty($data['TOTAL_INTEREST']))&& !(empty($data['ACTUAL_AMOUNT'])))
				{
					$Voice_Auth_Text = $Voice_Auth_Text."<br> DE56 SE01 SF01 = ".($data['DOWN_PAYMENT'])." ".($data['TOTAL_INTEREST']) ." ".($data['ACTUAL_AMOUNT']);

				}	
			}
		
			if(true ==	self::Check_If_Diners_Transaction($data['CARD_NUM']))
			{
				
				if((!(empty($data['PRODUCT13_CODE']))))
				{
					$Voice_Auth_Text = $Voice_Auth_Text."<br> DE24 =".$data['PRODUCT13_CODE'];

				}
			}				
		

			
				
			if( ($Acquirer_ID == 'nciftr') || ($Authorizing_host_queue == 'nciftr') )
					
			{
					$Voice_Auth_Text =  "F48 <"." ".$data['CVC_DATA']. " "."<br>F60 <";
					
					if(substr($MessageType,0,4) == "0320")
					{
							$Voice_Auth_Text = $Voice_Auth_Text." ".$data['ORIG_MESSAGE']." ".$data['ORIG_SYS_TRACE_NUM'] ;
					}
					else if(substr($MessageType,0,4) == "0500")	
					{
							$Voice_Auth_Text = $Voice_Auth_Text." ".$data['BATCH_NUMBER'];
					}
					else if(substr($MessageType,0,4) == "0200")
					{
							$Voice_Auth_Text = $Voice_Auth_Text." ".$data['ORIG_AMOUNT'];
					}
					else if(substr($MessageType,0,4) == "0200")
					{
							$Voice_Auth_Text = $Voice_Auth_Text." ".$data['ORIG_AMOUNT'];
					}
					
				$Voice_Auth_Text = $Voice_Auth_Text. "<br> F61 <"." ".$data['PRODUCT_CODE'];
				$Voice_Auth_Text = $Voice_Auth_Text. "<br> F62 <". " ".$data['INVOICE_NUMBER'];
				$Voice_Auth_Text = $Voice_Auth_Text. "<br> F63 <". " ".$data['NUM_SALES']." ".$data['SALES_AMOUNT']." ".
				$data['NUM_REFUNDS']." ".$data['REFUND_AMOUNT']." ".$data['NUM_DBSALES']." ".$data['DBSALES_AMOUNT']." ".
				$data['NUM_DBREFUNDS']." ".$data['DBREFUND_AMOUNT'];
				//var_dump($Voice_Auth_Text);die;
				//return $Voice_Auth_Text;	
				
				
			}
			else
			{
				if(substr($MessageType,0,4) == "0500")
				{
					$Voice_Auth_Text = $Voice_Auth_Text."F48 <"." ".$data['CVC_DATA'];
					$Voice_Auth_Text = $Voice_Auth_Text. "<br> F60 <"." ".$data['BATCH_NUMBER'];
					$Voice_Auth_Text = $Voice_Auth_Text. "<br> F61 <"." ".$data['PRODUCT_CODE'];
					$Voice_Auth_Text = $Voice_Auth_Text. "<br> F63 <". " ".$data['NUM_SALES']." ".$data['SALES_AMOUNT']." ".
					$data['NUM_REFUNDS']." ".$data['REFUND_AMOUNT']." ".$data['NUM_DBSALES']." ".$data['DBSALES_AMOUNT']." ".
					$data['NUM_DBREFUNDS']." ".$data['DBREFUND_AMOUNT'];
					//var_dump($Voice_Auth_Text);die;
					//return $Voice_Auth_Text;
					
				}
				

				else if(substr($card_num, 0, 1) == '4' ) 
				{
					if((substr($MessageType,0,4) == "0100")||(substr($MessageType,0,4) == "0200")||
					(substr($MessageType,0,4) == "0220"))
					{
						if(!(empty($data['CVC'])))
						{
								$Text_Field = "CAVV Results Code:".$data['CVC'];
						}
              
					}
					if(!(empty($data['RESP_SOURCE'])))
					{
						   $Text_Field = $Text_Field."<br> De44.1 Response source/reason code =".substr($data['RESP_SOURCE'],0,1);				
						   //var_dump($Text_Field);
						   //die("123");
						   //return $Text_Field;
					}
          
					if(substr($data['PRODUCT8_QUANTITY'],0,1) == 'C')
					{
						$Text_Field = $Text_Field."<br> POS Environment Field:".$data['PRODUCT8_QUANTITY'];
				
					}
          
					if(!(empty($data['RESP_SOURCE'])))
					{
          
						$Text_Field = $Text_Field."<br> De44.4 Value=".substr($data['RESP_SOURCE'],3,1);
           
					}
					if(!(empty($data['PRODUCT15_AMOUNT'])))
					{
						$Text_Field = $Text_Field."<br> Visa MVV Value=".$data['PRODUCT15_AMOUNT'];
					     
					}
				}
				else if( $data['HANDLER_QUEUE']=='nccup' || $data['HANDLER_QUEUE']=='nccup3' )
				{
					
					if((!(empty($data['PRODUCT2_AMOUNT']))) && (!(empty($data['PRODUCT1_CODE']))) && (!(empty($data['PRODUCT3_AMOUNT']))) && (!(empty($data['PRODUCT9_AMOUNT']))))
					{
						$Text_Field = $Text_Field."<br> DE61.6 AVN Value =".$data['PRODUCT2_AMOUNT'].$data['PRODUCT3_AMOUNT'].$data['PRODUCT9_AMOUNT'].$data['PRODUCT1_CODE'];
						die(123);

					}
					if((!(empty($data['PRODUCT14_AMOUNT']))) && (!(empty($data['PRODUCT19_AMOUNT']))) && (!(empty($data['PRODUCT20_AMOUNT']))))
					{
						$Text_Field = $Text_Field."<br> DE125 DSTransaction =".$data['PRODUCT14_AMOUNT'].$data['PRODUCT19_AMOUNT'].$data['PRODUCT20_AMOUNT'];

					}
					if((!(empty($data['PRODUCT1_CODE']))) 
					{
						$Text_Field = $Text_Field."<br> 3D Secure Version =".$data['PRODUCT1_CODE'];

					}
						if((!(empty($data['PRODUCT6_CODE']))) 
					{
						$Text_Field = $Text_Field."<br> ECI(DE61.4) =".$data['PRODUCT6_CODE'];

					}
				}
				
				else if(true ==	self::Check_If_JCB_Transaction($data['CARD_NUM']))
				{
				
					if((!(empty($data['PRODUCT4_AMOUNT']))))
					{
						$Text_Field = $Text_Field."<br> 3D Secure Version =".substr($data['PRODUCT4_AMOUNT'],2,2);

					}
					if((!(empty($data['profile']))))
					{
						$Text_Field = $Text_Field."<br> ECI =".$data['profile'];

					}
					if((!(empty($data['PRODUCT4_AMOUNT']))))
					{
						$Text_Field = $Text_Field."<br> 3D Secure Authentication Results Code =".substr($data['PRODUCT4_AMOUNT'],0,2);
						$Text_Field = $Text_Field."<br> Second Factor Authentication =".substr($data['PRODUCT4_AMOUNT'],2,2);
						$Text_Field = $Text_Field."<br> CAVV Key Indicator, =".substr($data['PRODUCT4_AMOUNT'],4,2);

					}
					if((!(empty($data['PRODUCT7_AMOUNT']))))
					{
						$Text_Field = $Text_Field."<br> ATN =".substr($data['PRODUCT7_AMOUNT'],2,10).substr($data['PRODUCT7_AMOUNT'],2,10).substr($data['PRODUCT9_AMOUNT'],0,6);

					}
					
				}
				else if(true ==	self::Check_If_Diners_Transaction($data['CARD_NUM']))
				{
					if((!(empty($data['PRODUCT1_CODE']))))
					{
						$Text_Field = $Text_Field."<br> DE24 =".$data['PRODUCT1_CODE'];

					}
					if((!(empty($data['operator_id']))))
					{
						$Text_Field = $Text_Field."<br> DE24 =".substr($data['operator_id'],0,2);

					}
					if((!(empty($data['dcf01_retired_cwk']))))
					{
						$Text_Field = $Text_Field."<br> DE122 value =".$data['dcf01_retired_cwk'];

					}
				}
				
				else if(true ==	self::Check_If_AMEX_Transaction($data['CARD_NUM']))
				{
					if((!(empty($data['profile']))))
					{
						$Text_Field = $Text_Field."<br> B61 SF4 =".$data['profile'];
						var_dump($Text_Field);die;
						return $Text_Field;
					}
					if((!(empty($data['dcf01_retired_cwk']))))
					{
						$Text_Field = $Text_Field."<br> B61 SF6 =".$data['dcf01_retired_cwk'];

					}
					if((!(empty($data['operator_id']))))
					{
						$Text_Field = $Text_Field."<br> B61 SF8 =".$data['operator_id'];

					}
					if((!(empty($data['workstation']))))
					{
						$Text_Field = $Text_Field."<br> 3DS version =".$data['workstation'];

					}
				
				}
				if(!(empty($data['PRODUCT1_CODE'])))
				{
					$Text_Field =  $Text_Field."<br> De126.9(Position 2) Authentication Method = ".substr($data['PRODUCT1_CODE'],0,2);
					//var_dump($Text_Field);die;
					$Text_Field = $Text_Field."<br> De126.20 3D-Secure Indicator = ".substr($data['PRODUCT1_CODE'],2,1);
					//return $Text_Field;
				}	
							
			//return $Voice_Auth_Text;
			
			$Text = array($Voice_Auth_Text,$Text_Field);
      //var_dump($Text_Field);
      //var_dump($Text);
      //var_dump($Voice_Auth_Text);
      
			return $Text;
			}
	}
	
public static function match_ISO_to_AMEX_response_code($data1,$data2)
{
		$tx_key = ord($data2);
		//return $tx_key;
		$amex = " ";
		$iso_resp_codes = array("00",  "01",  "02",  "03",  "04",  "05",  "06",  "07",
                                 "12",  "13",  "14",  "15",  "19",  "21",  "25",  "28",
                                 "39",  "41",  "43",  "51",  "52",  "53",  "54",  "55",
                                 "57",  "58",  "61",  "62",  "63",  "65",  "75",  "76",
                                 "77",  "80",  "81",  "82",  "85",  "86",  "91",  "92",
                                 "93",  "96");
							   
	$auth_advice_amex = array("900", "904", "904", "904", "904", "904", "904", "904",
                              "904", "904", "904", "904", "904", "904", "904", "904",
                              "904", "904", "904", "904", "904", "904", "904", "904",
                              "904", "904", "904", "904", "904", "904", "904", "904",
                              "904", "904", "904", "904", "900", "904", "904", "904",
                              "904", "904");
									   
	$auth_reversal_amex = array("400", "400", "400", "400", "400", "400", "400", "400",
                                 "400", "400", "400", "400", "400", "400", "400", "400",
                                 "400", "400", "400", "400", "400", "400", "400", "400",
                                 "400", "400", "400", "400", "400", "400", "400", "400",
                                 "400", "400", "400", "400", "400", "400", "400", "400",
                                 "400", "400");
								 
	//define('NUMBER_OF_ELEMENTS', count($iso_resp_codes));
	
	$numberofelements = count($iso_resp_codes);
	
	//error_log(print_r($numberofelements, TRUE));
	//return $numberofelements;
	//echo $numberofelements;
	//exit;
	//return $iso_resp_codes[3];
	
	//return "hi";
	if($tx_key == AUTH_ADVICE || $tx_key == AUTH_ADVICE_RESPONSE) 
	{
		
		for( $i=0; $i<$numberofelements; $i++)
		{
			//return $iso_resp_codes[$numberofelements];
			if(0 == strncmp($data1,$iso_resp_codes[$i],2))
			{
		
				$amex = $auth_advice_amex[$i];
				break;
			}
		}
	}
	
	else if($tx_key == AUTH_REVERSAL_ADVICE || $tx_key == AUTH_REVERSAL_ADVICE_RESPONSE) 
	{
		
		
		for( $i=0; $i<$numberofelements; $i++)
		{
			if(0 == strncmp($data1,$iso_resp_codes[$i],2))
			{
				//$amex =strncpy($amex,$auth_reversal_amex[$numberofelements],3);
				$amex = $auth_reversal_amex[$i];
				break;
			}
		}
	}
	return $amex;
}
	
	
	
	public static function match_ISO_to_Diners_response_code($data1)
	{
		$dci_rescode = " ";
	
		$iso_response_codes  =  array("00",  "01",  "02",  "03",  "04",  "05",  "06",  "07",
                                       "08",  "09",  "10",  "11",  "12",  "13",  "14",  "15",
                                       "16",  "17",  "18",  "19",  "20",  "21",  "22",  "23",
                                       "24",  "25",  "26",  "27",  "28",  "29",  "30",  "31",
                                       "32",  "33",  "34",  "35",  "36",  "37",  "38",  "39",
                                       "40",  "41",  "42",  "43",  "44",  "51",  "52",  "53",
									   "54",  "55",  "58",  "59",  "60",  "61",  "62",  "63",
									   "64",  "65",  "66",  "68",  "75",  "76",  "77",  "78",
									   "79",  "80",  "81",  "82",  "84",  "85",  "86",  "87",
									   "88",  "89",  "90",  "91",  "92",  "93",  "94",  "95",
									   "96",  "97",  "98",  "NO",  "N7",  "00");

		$DCI_response_code  =    array("081", "107", "107", "109", "200", "182", "182", "200",
                                        "001", "182", "081", "081", "182", "110", "182", "182",
                                        "081", "182", "909", "182", "909", "182", "909", "182",
                                        "   ", "182", "   ", "   ", "182", "   ", "904", "905",
                                        "182", "101", "102", "182", "204", "203", "206", "182",
                                        "115", "208", "182", "209", "182", "182", "182", "182",
									    "101", "117", "182", "210", "203", "182", "104", "182",
									    "182", "182", "205", "182", "106", "182", "182", "182",
									    "182", "307", "909", "182", "182", "081", "909", "182",
									    "909", "182", "910", "182", "909", "124", "909", "909",
									    "909", "182", "182", "909", "182", "400");
										
		$numberofarrayelements = count($iso_response_codes);
		
		for($i=0; $i<$numberofarrayelements;$i++)
		{
			if (0 == strncmp($data1, $iso_response_codes[$i], 2))
			{
				//strncpy(diners_response_code, DCI_response_code[i], 3);
				$dci_rescode = $DCI_response_code[$i];
				break;
			}
		}
		
		return $dci_rescode;
	}
	
	public static function processed_data($data) {
		
		if(!empty($data["TOTAL_AMOUNT"])) {
			$data["TOTAL_AMOUNT"] = ltrim($data["TOTAL_AMOUNT"], 0);
			$data["TOTAL_AMOUNT"] = substr_replace($data["TOTAL_AMOUNT"], '.', -2, 0);
		}
		if(!empty($data["CREDIT_LIMIT"])) {
			$data["CREDIT_LIMIT"] = ltrim($data["CREDIT_LIMIT"], 0);
			$data["CREDIT_LIMIT"] = substr_replace($data["CREDIT_LIMIT"], '.', -2, 0);
		}
		if(!empty($data["OUTSTANDING_BALANCE"])) {
			$data["OUTSTANDING_BALANCE"] = ltrim($data["OUTSTANDING_BALANCE"], 0);
			$data["OUTSTANDING_BALANCE"] = substr_replace($data["OUTSTANDING_BALANCE"], '.', -2, 0);
		}
		if(!empty($data["PRODUCT10_AMOUNT"])) {
			$data["PRODUCT10_AMOUNT"] = ltrim($data["PRODUCT10_AMOUNT"], 0);
			$data["PRODUCT10_AMOUNT"] = substr_replace($data["PRODUCT10_AMOUNT"], '.', -2, 0);
		}
		if(!empty($data['DATE_YYYYMMDD'])) {
			$data['DATE_YYYYMMDD'] = date('Y-m-d', strtotime($data['DATE_YYYYMMDD']));
		}
		if(!empty($data['TIME_HHMMSS'])) {
			$data['TIME_HHMMSS'] = substr($data['TIME_HHMMSS'], 0, 2).':'.substr($data['TIME_HHMMSS'], 2, 2).':'.substr($data['TIME_HHMMSS'], 4, 2);
		}
		$host_start_time = $data['HOST_START_TIME'];
		$data['EarMarkedCurrency'] = '';
		$data['EarMarkedAmt'] = '';
		if($host_start_time == '5.000')
		{
			$data['EarMarkedCurrency'] = 'PHP';
			$data['EarMarkedAmt'] = $data['PRODUCT10_AMOUNT'];
		}
		else if($host_start_time == '7.000')
		{
			$data['EarMarkedCurrency'] = 'USD';
			$data['EarMarkedAmt'] = $data['PRODUCT10_AMOUNT'];
		}
		else
		{
			$data['EarMarkedCurrency'] = $data["CURRENCY_CODE"];
			$data['EarMarkedAmt'] = $data["TOTAL_AMOUNT"];
		}
		$data['ORG_TXN_AMT'] = '';
		$data['ORG_TXN_CUR'] = '';
		
		if ($data["PRODUCT4_QUANTITY"] == "RP")
		{
			//C Start
			if((true == self::Check_If_VISA_Transaction($data['CARD_NUM'])) ||
			   (true ==	self::Check_If_MC_Transaction($data['CARD_NUM'])))
			{
				$data['ORG_TXN_AMT'] = $data["ORIG_AMOUNT"];
				$data['REVERSAL_AMOUNT'] = $data["TOTAL_AMOUNT"];
				$data['TOTAL_AMOUNT'] = $data["ORIG_AMOUNT"];
								
			}
			else
			{
				$data['ORG_TXN_AMT'] = $data["TOTAL_AMOUNT"];
			}
			$data['ORG_TXN_CUR'] = $data["CURRENCY_CODE"];
		//end 
		}
		
		elseif ($data['HANDLER_QUEUE'] == 'dcpiso')
		{
			$data['ORG_TXN_AMT'] = $data["TOTAL_AMOUNT"];
			$data['ORG_TXN_CUR'] = $data["CURRENCY_CODE"];
		} 
	
		else {
			if ($data["PRODUCT5_CODE"] == "ISSUE") {
				$data['ORG_TXN_AMT'] = $data["SALES_AMOUNT"];
				$data['ORG_TXN_CUR'] = $data["NFI_SEQ_NBR"];
			} else {
				$data['ORG_TXN_AMT'] = $data["TOTAL_AMOUNT"];
				$data['ORG_TXN_CUR'] = $data["CURRENCY_CODE"];
			}
		}
		if(!empty($data['DEFERRED_TERM_LENGTH'])) {
			if(!empty($data['DEF_GROSS_AMT'])) {
				$data['DEF_GROSS_AMT'] = ltrim($data['DEF_GROSS_AMT'], 0);
				$data['DEF_GROSS_AMT'] = substr_replace($data['DEF_GROSS_AMT'], '.', -2, 0);
			}
			if(!empty($data['MONTHLY_AMORT_AMT'])) {
				$data['MONTHLY_AMORT_AMT'] = ltrim($data['MONTHLY_AMORT_AMT'], 0);
				$data['MONTHLY_AMORT_AMT'] = substr_replace($data['MONTHLY_AMORT_AMT'], '.', -2, 0);
			}
			$data['DEFERRED_FACTOR'] = $data['DEFERRED_FACTOR']/10000;
	
			$data['DEFERRED_FACTOR'] = sprintf("%8.4f", $data['DEFERRED_FACTOR']);
	
		}
		$data['ACQUIRING_COUNTRY'] = $data['CURRENCY_CODE'];
		$data['TransactionType'] = self::getTransactionType($data);
		return $data;
	}


}
?>

