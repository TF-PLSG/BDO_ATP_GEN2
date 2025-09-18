/*********************************************************************************

************************************************************************************/
// TransactionDetails.cpp : implementation file
//

#include "stdafx.h"
#include "TranSearch.h"
#include "Visa_EDQP_Txn_Details.h"
#include "transactiondetails.h"
#include "MyUtils.h"
#include "GenUtil.h"
#include "TranInquiry.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BYTE Transactionid[21];
extern BYTE Handler_queue[7];

extern BOOL m_bTran;
extern BOOL m_bTran_stip;

//CTransactionDetails *pTransactionDetails;

/////////////////////////////////////////////////////////////////////////////
// CVisaEDQPTransactionDetails dialog

IMPLEMENT_DYNCREATE(CVisaEDQPTransactionDetails, CPropertyPage)
CVisaEDQPTransactionDetails::CVisaEDQPTransactionDetails(): CPropertyPage(CVisaEDQPTransactionDetails::IDD)
{
	CString m_Strcard_num               =_T("");  
	CString m_Strtran_amt				=_T("");  
	CString m_Strauth_number            =_T("");  
	CString m_Strappl_auth_char_indcr   =_T("");
	CString m_Strresponse_code			=_T("");  
	CString m_Strcurrency_code			=_T("");  
	CString m_Straccnt_fund_source		=_T("");  
	CString m_Strcard_hldr_id_mthd		=_T("");  
	CString m_Strcash_back_amt			=_T("");  
	CString m_Strmrkt_spcfc_data_indcr	=_T("");  
	CString m_Strmerch_ctgry_code		=_T("");  
	CString m_Strpos_entry_mode			=_T("");  
	CString m_Strproduct_id				=_T("");  
	CString m_Strtranscn_identfr		=_T("");  
	CString m_Strvaldn_code				=_T("");  
	CString m_StrECI_value				=_T("");  
	CString m_Strspend_qualfn_indcr		=_T("");  
	CString m_StrPOS_env_field			=_T("");  
	
}


void CVisaEDQPTransactionDetails::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransactionDetails)
	DDX_Control(pDX, IDC_CARD_NUMBER_FLD			,m_Cntlcard_num					);
	DDX_Control(pDX, IDC_TRAN_AMOUNT_FLD			,m_Cntltran_amt					);	
	DDX_Control(pDX, IDC_AUTH_ID_RESPONSE_FLD		,m_Cntlauth_number				);
	DDX_Control(pDX, IDC_APPLIED_AUTH_CHAR_INDCR	,m_Cntlappl_auth_char_indcr		);
	DDX_Control(pDX, IDC_RESPONSE_CODE_FLD			,m_Cntlresponse_code			);
	DDX_Control(pDX, IDC_CURRENCY_CODE_FLD			,m_Cntlcurrency_code			);
	DDX_Control(pDX, IDC_ACCNT_FUNDING_SOURCE		,m_Cntlaccnt_fund_source		);
	DDX_Control(pDX, IDC_CARD_HOLDER_ID_METHOD		,m_Cntlcard_hldr_id_mthd		);
	DDX_Control(pDX, IDC_CASHBACK_AMOUNT			,m_Cntlcash_back_amt			);
	DDX_Control(pDX, IDC_MARKET_SPECIFIC_AUTH_INDCR	,m_Cntlmrkt_spcfc_data_indcr	);
	DDX_Control(pDX, IDC_MCC						,m_Cntlmerch_ctgry_code			);
	DDX_Control(pDX, IDC_POS_ENTRY_MODE_FLD			,m_Cntlpos_entry_mode			);
	DDX_Control(pDX, IDC_PRODUCT_ID					,m_Cntlproduct_id				);
	DDX_Control(pDX, IDC_TRANSACTION_IDENTIFIER		,m_Cntltranscn_identfr			);
	DDX_Control(pDX, IDC_VALIDATION_CODE			,m_Cntlvaldn_code				);
	DDX_Control(pDX, IDC_ECI						,m_CntlECI_value				);
	DDX_Control(pDX, IDC_SPEND_QUALFICATION_INDCR	,m_Cntlspend_qualfn_indcr		);
	DDX_Control(pDX, IDC_POS_ENVIRONMENT_FIELD	    ,m_CntlPOS_env_field			);
		DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CARD_NUMBER_FLD 				,m_Strcard_num					);
	DDX_Text(pDX, IDC_TRAN_AMOUNT_FLD 				,m_Strtran_amt					);
	DDX_Text(pDX, IDC_AUTH_ID_RESPONSE_FLD			,m_Strauth_number				);
	DDX_Text(pDX, IDC_APPLIED_AUTH_CHAR_INDCR		,m_Strappl_auth_char_indcr		);
	DDX_Text(pDX, IDC_RESPONSE_CODE_FLD				,m_Strresponse_code				);
	DDX_Text(pDX, IDC_CURRENCY_CODE_FLD				,m_Strcurrency_code				);
	DDX_Text(pDX, IDC_ACCNT_FUNDING_SOURCE			,m_Straccnt_fund_source			);
	DDX_Text(pDX, IDC_CARD_HOLDER_ID_METHOD			,m_Strcard_hldr_id_mthd			);
	DDX_Text(pDX, IDC_CASHBACK_AMOUNT				,m_Strcash_back_amt				);
	DDX_Text(pDX, IDC_MARKET_SPECIFIC_AUTH_INDCR	,m_Strmrkt_spcfc_data_indcr		);
	DDX_Text(pDX, IDC_MCC							,m_Strmerch_ctgry_code			);
	DDX_Text(pDX, IDC_POS_ENTRY_MODE_FLD			,m_Strpos_entry_mode			);
	DDX_Text(pDX, IDC_PRODUCT_ID					,m_Strproduct_id				);
	DDX_Text(pDX, IDC_TRANSACTION_IDENTIFIER		,m_Strtranscn_identfr			);
	DDX_Text(pDX, IDC_VALIDATION_CODE				,m_Strvaldn_code				);
	DDX_Text(pDX, IDC_ECI 							,m_StrECI_value					);
	DDX_Text(pDX, IDC_SPEND_QUALFICATION_INDCR 	    ,m_Strspend_qualfn_indcr		);
	DDX_Text(pDX, IDC_POS_ENVIRONMENT_FIELD 	    ,m_StrPOS_env_field				);
		//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVisaEDQPTransactionDetails, CPropertyPage)
//{{AFX_MSG_MAP(CVisaEDQPTransactionDetails)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisaEDQPTransactionDetails message handlers

BOOL CVisaEDQPTransactionDetails::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CFont m_font;
	LOGFONT lf;
	CHAR strErrorMessage[256]= {0};
	/*strncpy((char *) TranRecord.tlf01.primary_key.transaction_id, (char *)Transactionid ,sizeof(Transactionid));
	int rcode = txdsapi_get_record ( (pBYTE)&TranRecord, sizeof (TranRecord), TLF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
            m_StrMessageLbl = "The TLF01 record for this Transaction was not found.";
			break;
		}
		UpdateData(FALSE);
		return FALSE;
	}*/

	if(m_bTran)
	{
			PopulateVisaEDQPTranDialogFromDatabase ( TranRecord );
	}
	/*else if(m_bFGuard)
			PopulateFGuardDialogFromDatabase(sFGuardRecord);*/
	if(m_bTran_stip)
	{
		PopulateVisaEDQPSTIPTranDialogFromDatabase( TranRecordS );
	}
		UpdateData(FALSE);	
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CVisaEDQPTransactionDetails::PopulateVisaEDQPTranDialogFromDatabase ( VEDQP_RECORD vTranRecord  )
{
	CMyUtils myUtilsObject;
	CString  temp  = _T("");
	CHAR strErrorMessage[256]= {0};
	char	 cavv_buff[1024] = {0};
	int rcode = txdsapi_get_record ( (pBYTE)&vTranRecord, sizeof (vTranRecord), TLF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
            m_StrMessageLbl = "The VISA EDQP details for this Transaction were not found.";
			break;
		}
		UpdateData(FALSE);
		return FALSE;
	}

	if ((0 == strncmp((char *)vTranRecord.tlf01.handler_queue,"dcpiso" ,5 ) ||
		0 == strncmp((char *)vTranRecord.tlf01.handler_queue,"VOICE", 5 )) &&
		0 == strncmp((char *)vTranRecord.tlf01.acquirer_id, "ncvisa", 6))
	{
		/*m_Strcard_num    			= vTranRecord.tlf01.card_num;
		m_Strtran_amt   			= vTranRecord.tlf01.total_amount;
		m_Strauth_number    		= vTranRecord.tlf01.auth_number;
		m_Strappl_auth_char_indcr	= vTranRecord.tlf01.visa_char_indicator;
		m_Strresponse_code         	= vTranRecord.tlf01.response_code;
		m_Strcurrency_code 			= vTranRecord.tlf01.currency_code;*/

		/*m_Straccnt_fund_source 		= vTranRecord.tlf01.kek[0];
		m_Strcard_hldr_id_mthd 		+= (char *)vTranRecord.tlf01.product_codes[5].quantity[0];
		m_Strcash_back_amt			= vTranRecord.tlf01.add_amounts;
		m_Strmrkt_spcfc_data_indcr 	+= (char *)vTranRecord.tlf01.product_codes[15].quantity[0];
		m_Strmerch_ctgry_code 		= vTranRecord.tlf01.category_code;
		m_Strpos_entry_mode 		= vTranRecord.tlf01.pos_entry_mode;
		m_Strproduct_id 			+= (char *)vTranRecord.tlf01.product_codes[7].code[0];
		m_Strproduct_id				+= (char *)vTranRecord.tlf01.product_codes[7].code[1];
		m_Strtranscn_identfr		= vTranRecord.tlf01.visa_tran_id;
		m_Strvaldn_code 			= vTranRecord.tlf01.visa_validation_code;
		m_StrECI_value				= vTranRecord.tlf01.profile;
		m_Strspend_qualfn_indcr		= vTranRecord.tlf01.product_codes[7].code[2];
		m_StrPOS_env_field			= vTranRecord.tlf01.product_codes[7].quantity[0]; */

				if (vTranRecord.tlf01.card_num[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.card_num);
					m_Strcard_num += cavv_buff;
				}

				if (vTranRecord.tlf01.total_amount[0] != '\0')
				{					
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					/*sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.total_amount);
					m_Strtran_amt += cavv_buff;*/
					temp = _T("");
					temp = myUtilsObject.DisplayAmount(vTranRecord.tlf01.total_amount);
					sprintf (cavv_buff," %s " ,temp);
					m_Strtran_amt +=  cavv_buff;
				}

				if (vTranRecord.tlf01.auth_number[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.auth_number);
					m_Strauth_number += cavv_buff;
				}

				if (vTranRecord.tlf01.visa_char_indicator != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.visa_char_indicator);
					m_Strappl_auth_char_indcr += cavv_buff;
				}

				if (vTranRecord.tlf01.visa_char_indicator == '\0' &&
					vTranRecord.tlf01.kek[3] != '\0')
				{					/* F111 DS56 TG82 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.kek[3]);
					m_Strappl_auth_char_indcr += cavv_buff;
				}

				if (vTranRecord.tlf01.response_code[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.response_code);
					m_Strresponse_code += cavv_buff;
				}

				if (vTranRecord.tlf01.currency_code[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.currency_code);
					m_Strcurrency_code += cavv_buff;
				}

				if (vTranRecord.tlf01.kek[0] != '\0')
				{				/* F111 DS56 TG80 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.kek[0]);
					m_Straccnt_fund_source += cavv_buff;
				}

				if (vTranRecord.tlf01.product_codes[5].quantity[0] != '\0')
				{				/* F60.9*/
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.product_codes[5].quantity[0]);
					m_Strcard_hldr_id_mthd += cavv_buff;

				}

				if (vTranRecord.tlf01.product_codes[5].quantity[0] == '\0' &&
					vTranRecord.tlf01.kek[4] != '\0')
				{				/* F111 DS56 TG85 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.kek[4]);
					m_Strcard_hldr_id_mthd += cavv_buff;

				}

				if (vTranRecord.tlf01.add_amounts[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.add_amounts);
					m_Strcash_back_amt += cavv_buff;

				}

				if (vTranRecord.tlf01.product_codes[15].quantity[0] != '\0')
				{				/* F62.4 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.product_codes[15].quantity[0]);
					m_Strmrkt_spcfc_data_indcr += cavv_buff;
				}

				if (vTranRecord.tlf01.category_code[0] != '\0')
				{				/* F18 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.category_code);
					m_Strmerch_ctgry_code += cavv_buff;
				}

				if (vTranRecord.tlf01.pos_entry_mode[0] != '\0')
				{				/* F22 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.pos_entry_mode);
					m_Strpos_entry_mode += cavv_buff;
				}

				if (vTranRecord.tlf01.product_codes[7].code[0] != '\0')
				{				/* F62.23 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c%c" ,vTranRecord.tlf01.product_codes[7].code[0],vTranRecord.tlf01.product_codes[7].code[1]);
					m_Strproduct_id += cavv_buff;
				}

				if (vTranRecord.tlf01.visa_tran_id[0] != '\0')
				{				/* F62.2 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.visa_tran_id);
					m_Strtranscn_identfr += cavv_buff;
				}

				if (vTranRecord.tlf01.visa_validation_code[0] != '\0')
				{				/* F62.3 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.visa_validation_code);
					m_Strvaldn_code += cavv_buff;
				}

				if (vTranRecord.tlf01.profile[0] != '\0')
				{				/* F60.8 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.profile);
					m_StrECI_value += cavv_buff;
				}

				if (vTranRecord.tlf01.product_codes[7].code[2] != '\0')
				{				/* F62.25 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.product_codes[7].code[2]);
					m_Strspend_qualfn_indcr += cavv_buff;
				}

				if (vTranRecord.tlf01.product_codes[7].quantity[0] != '\0')
				{				/* F126.13 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.product_codes[7].quantity[0]);
					m_StrPOS_env_field += cavv_buff;
				}


	}
	else
	{
		 m_StrMessageLbl = "The Visa EDQP details for this Transaction was not found.";
	}
	UpdateData(FALSE);
   
   return TRUE;
}

BOOL CVisaEDQPTransactionDetails::PopulateVisaEDQPSTIPTranDialogFromDatabase ( VEDQP_STIP_RECORD vTranRecord  )
{
	CHAR strErrorMessage[256]= {0};
	char	 cavv_buff[1024] = {0};
	int rcode = txdsapi_get_record ( (pBYTE)&vTranRecord, sizeof (vTranRecord), TLF01_STIP_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
            m_StrMessageLbl = "The VISA EDQP details for this Transaction were not found.";
			break;
		}
		UpdateData(FALSE);
		return FALSE;
	}

	if (0 == strncmp((char *)vTranRecord.tlf01.handler_queue,"dcpiso" ,5 ) &&
		0 == strncmp((char *)vTranRecord.tlf01.acquirer_id, "ncvisa", 6))
	{
		/*m_Strcard_num    			= vTranRecord.tlf01.card_num;
		m_Strtran_amt   			= vTranRecord.tlf01.total_amount;
		m_Strauth_number    		= vTranRecord.tlf01.auth_number;
		m_Strappl_auth_char_indcr	= vTranRecord.tlf01.visa_char_indicator;
		m_Strresponse_code         	= vTranRecord.tlf01.response_code;
		m_Strcurrency_code 			= vTranRecord.tlf01.currency_code;*/

		/*m_Straccnt_fund_source 		= vTranRecord.tlf01.kek[0];
		m_Strcard_hldr_id_mthd 		+= (char *)vTranRecord.tlf01.product_codes[5].quantity[0];
		m_Strcash_back_amt			= vTranRecord.tlf01.add_amounts;
		m_Strmrkt_spcfc_data_indcr 	+= (char *)vTranRecord.tlf01.product_codes[15].quantity[0];
		m_Strmerch_ctgry_code 		= vTranRecord.tlf01.category_code;
		m_Strpos_entry_mode 		= vTranRecord.tlf01.pos_entry_mode;
		m_Strproduct_id 			+= (char *)vTranRecord.tlf01.product_codes[7].code[0];
		m_Strproduct_id				+= (char *)vTranRecord.tlf01.product_codes[7].code[1];
		m_Strtranscn_identfr		= vTranRecord.tlf01.visa_tran_id;
		m_Strvaldn_code 			= vTranRecord.tlf01.visa_validation_code;
		m_StrECI_value				= vTranRecord.tlf01.profile;
		m_Strspend_qualfn_indcr		= vTranRecord.tlf01.product_codes[7].code[2];
		m_StrPOS_env_field			= vTranRecord.tlf01.product_codes[7].quantity[0]; */

				if (vTranRecord.tlf01.card_num[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.card_num);
					m_Strcard_num += cavv_buff;
				}

				if (vTranRecord.tlf01.total_amount[0] != '\0')
				{					
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.total_amount);
					m_Strtran_amt += cavv_buff;
				}

				if (vTranRecord.tlf01.auth_number[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.auth_number);
					m_Strauth_number += cavv_buff;
				}

				if (vTranRecord.tlf01.visa_char_indicator != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.visa_char_indicator);
					m_Strappl_auth_char_indcr += cavv_buff;
				}

				if (vTranRecord.tlf01.visa_char_indicator == '\0' &&
					vTranRecord.tlf01.kek[3] != '\0')
				{					/* F111 DS56 TG82 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.kek[3]);
					m_Strappl_auth_char_indcr += cavv_buff;
				}

				if (vTranRecord.tlf01.response_code[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.response_code);
					m_Strresponse_code += cavv_buff;
				}

				if (vTranRecord.tlf01.currency_code[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.currency_code);
					m_Strcurrency_code += cavv_buff;
				}

				if (vTranRecord.tlf01.kek[0] != '\0')
				{				/* F111 DS56 TG80 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.kek[0]);
					m_Straccnt_fund_source += cavv_buff;
				}

				if (vTranRecord.tlf01.product_codes[5].quantity[0] != '\0')
				{				/* F60.9*/
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.product_codes[5].quantity[0]);
					m_Strcard_hldr_id_mthd += cavv_buff;

				}

				if (vTranRecord.tlf01.product_codes[5].quantity[0] == '\0' &&
					vTranRecord.tlf01.kek[4] != '\0')
				{				/* F111 DS56 TG85 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.kek[4]);
					m_Strcard_hldr_id_mthd += cavv_buff;

				}

				if (vTranRecord.tlf01.add_amounts[0] != '\0')
				{
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.add_amounts);
					m_Strcash_back_amt += cavv_buff;

				}

				if (vTranRecord.tlf01.product_codes[15].quantity[0] != '\0')
				{				/* F62.4 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.product_codes[15].quantity[0]);
					m_Strmrkt_spcfc_data_indcr += cavv_buff;
				}

				if (vTranRecord.tlf01.category_code[0] != '\0')
				{				/* F18 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.category_code);
					m_Strmerch_ctgry_code += cavv_buff;
				}

				if (vTranRecord.tlf01.pos_entry_mode[0] != '\0')
				{				/* F22 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.pos_entry_mode);
					m_Strpos_entry_mode += cavv_buff;
				}

				if (vTranRecord.tlf01.product_codes[7].code[0] != '\0')
				{				/* F62.23 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c%c" ,vTranRecord.tlf01.product_codes[7].code[0],vTranRecord.tlf01.product_codes[7].code[1]);
					m_Strproduct_id += cavv_buff;
				}

				if (vTranRecord.tlf01.visa_tran_id[0] != '\0')
				{				/* F62.2 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.visa_tran_id);
					m_Strtranscn_identfr += cavv_buff;
				}

				if (vTranRecord.tlf01.visa_validation_code[0] != '\0')
				{				/* F62.3 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.visa_validation_code);
					m_Strvaldn_code += cavv_buff;
				}

				if (vTranRecord.tlf01.profile[0] != '\0')
				{				/* F60.8 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,vTranRecord.tlf01.profile);
					m_StrECI_value += cavv_buff;
				}

				if (vTranRecord.tlf01.product_codes[7].code[2] != '\0')
				{				/* F62.25 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.product_codes[7].code[2]);
					m_Strspend_qualfn_indcr += cavv_buff;
				}

				if (vTranRecord.tlf01.product_codes[7].quantity[0] != '\0')
				{				/* F126.13 */
				    memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,vTranRecord.tlf01.product_codes[7].quantity[0]);
					m_StrPOS_env_field += cavv_buff;
				}


	}
	else
	{
		 m_StrMessageLbl = "The Visa EDQP details for this Transaction was not found.";
	}
	UpdateData(FALSE);
   
   return TRUE;
}

