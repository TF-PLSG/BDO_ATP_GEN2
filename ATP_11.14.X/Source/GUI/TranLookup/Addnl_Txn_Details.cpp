/*********************************************************************************

************************************************************************************/
// TransactionDetails.cpp : implementation file
//

#include "stdafx.h"
#include "TranSearch.h"
#include "Addnl_Txn_Details.h"
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
// CTransactionDetails dialog

IMPLEMENT_DYNCREATE(CAddlFldsTransactionDetails, CPropertyPage)
CAddlFldsTransactionDetails::CAddlFldsTransactionDetails(): CPropertyPage(CAddlFldsTransactionDetails::IDD)
{
	//{{AFX_DATA_INIT(CTransactionDetails)

	//}}AFX_DATA_INIT
	CString m_StrCAVV_Result_code			=_T("");      
	CString m_StrPOS_Env_Field				=_T("");
	CString m_StrVisa_3DS2_TranID			=_T("");
	CString m_StrVisa_3DS2_Auth_Rslt_code   =_T("");
	CString m_StrVisa_3DS2_Auth_mthd		=_T("");
	CString m_StrVisa_3DS2_Sec_indcr		=_T("");
	CString m_StrVisa_Resp_reason_code		=_T("");
	CString m_StrVisa_STIP_reason_code		=_T("");
	CString m_StrVisa_MVV_value				=_T("");
	CString m_StrMC_Merch_Advice_code		=_T("");
	CString m_StrMC_TranLink_ID				=_T("");
	CString m_StrMC_Prgrm_prtcl				=_T("");
	CString m_StrMC_DS_Tranid				=_T("");
	CString m_StrMC_UCAF_value				=_T("");
	CString m_StrMC_Merch_Cntry_orgn		=_T("");
	CString m_StrMC_Paymt_accnt_ref			=_T("");
	CString m_StrMC_TCC_value				=_T("");
	CString m_StrMC_Merch_paymt_gwy_id		=_T("");
	CString m_StrMC_Sanction_score_addnl_info		=_T("");
	CString m_StrMC_OBS_value		        =_T("");
	CString m_StrMC_Trscn_purpose			=_T("");
	CString m_StrDCI_Func_code		        =_T("");
	CString m_StrDCI_Err_DE_value           =_T("");
	CString m_StrDCI_Addl_Auth_data         =_T("");
	CString m_StrAMX_ECI_value				=_T("");
	CString m_StrAMX_AEVV_value				=_T("");
	CString m_StrAMX_Trasn_id				=_T("");
	CString m_StrAMX_3DS2_version			=_T("");
	CString m_StrUPI_AVN_value				=_T("");
	CString m_StrUPI_DS_Transn_id			=_T("");
	CString m_StrUPI_Secrty_Risk_IP_value	=_T("");
	CString m_StrUPI_3DS2_version			=_T("");
	CString m_StrUPI_ECI_value				=_T("");
	CString m_StrUPI_Secrty_Info_AM_value	=_T("");
	CString m_StrJCB_3DS2_CAVV_value		=_T("");
	CString m_StrJCB_ECI_value				=_T("");
	CString m_StrJCB_3DS2_version			=_T("");
	CString m_StrPhysical_Term_serl_num		=_T("");
	CString m_StrPIN_service_code			=_T("");
	CString m_StrSecrty_srvcs_info			=_T("");
	CString m_StrMessageLbl					=_T("");
	
}


void CAddlFldsTransactionDetails::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransactionDetails)
	DDX_Control(pDX, IDC_CAVV_RESULT_CODE 			,m_CntlCAVV_Result_code				);
	DDX_Control(pDX, IDC_POS_ENVIRONMENT_FIELD 		,m_CntlPOS_Env_Field				);
	DDX_Control(pDX, IDC_VS_3DS2_TRANSACTIONID		,m_CntlVisa_3DS2_TranID				);
	DDX_Control(pDX, IDC_VS_3DS2_AUTH_RESULT_CODE	,m_CntlVisa_3DS2_Auth_Rslt_code		);
	DDX_Control(pDX, IDC_VS_3DS2_AUTHN_METHOD		,m_CntlVisa_3DS2_Auth_mthd			);
	DDX_Control(pDX, IDC_VS_3DS2_SECURE_INDCR		,m_CntlVisa_3DS2_Sec_indcr			);
	DDX_Control(pDX, IDC_VS_RESPONSE_REASON_CODE	,m_CntlVisa_Resp_reason_code		);
	DDX_Control(pDX, IDC_VS_STIP_REASON_CODE		,m_CntlVisa_STIP_reason_code		);
	DDX_Control(pDX, IDC_VS_MVV_VALUE				,m_CntlVisa_MVV_value				);
	DDX_Control(pDX, IDC_MC_MERCH_ADVICE_CODE		,m_CntlMC_Merch_Advice_code			);
	DDX_Control(pDX, IDC_MC_TRAN_LNK_IDENTIFIER		,m_CntlMC_TranLink_ID				);
	DDX_Control(pDX, IDC_MC_PROGRAM_PROTOCOL		,m_CntlMC_Prgrm_prtcl				);
	DDX_Control(pDX, IDC_MC_DS_TRAN_ID				,m_CntlMC_DS_Tranid					);
	DDX_Control(pDX, IDC_MC_UCAF_VALUE				,m_CntlMC_UCAF_value				);
	DDX_Control(pDX, IDC_MC_MERCH_CTRY_OF_ORIGIN	,m_CntlMC_Merch_Cntry_orgn			);
	DDX_Control(pDX, IDC_MC_PAYMT_ACCNT_REFERENCE	,m_CntlMC_Paymt_accnt_ref			);
	DDX_Control(pDX, IDC_MC_TCC_VALUE				,m_CntlMC_TCC_value					);
	DDX_Control(pDX, IDC_MC_MERCH_PAYMT_GWAY_ID		,m_CntlMC_Merch_paymt_gwy_id		);
	DDX_Control(pDX, IDC_MC_SANCTION_SCORE_ADDL_INFO,m_CntlMC_Sanction_score_addnl_info	);
	DDX_Control(pDX, IDC_MC_ON_BEHALF_OF_SERVICE	,m_CntlMC_OBS_value					);
	DDX_Control(pDX, IDC_MC_TRANSACTION_PURPOSE		,m_CntlMC_Trscn_purpose				);
	DDX_Control(pDX, IDC_DCI_FUNCTION_CODE			,m_CntlDCI_Func_code				);
	DDX_Control(pDX, IDC_DCI_ERR_DATA_ELEMENT_VALUE	,m_CntlDCI_Err_DE_value				);
	DDX_Control(pDX, IDC_DCI_ADDL_AUTH_DATA			,m_CntlDCI_Addl_Auth_data			);
	DDX_Control(pDX, IDC_AMX_ECI_VALUE				,m_CntlAMX_ECI_value				);
	DDX_Control(pDX, IDC_AMX_AEVV_VALUE				,m_CntlAMX_AEVV_value				);
	DDX_Control(pDX, IDC_AMX_TRNSN_ID				,m_CntlAMX_Trasn_id					);
	DDX_Control(pDX, IDC_AMX_3DS2_VERSION			,m_CntlAMX_3DS2_version				);
	DDX_Control(pDX, IDC_CUP_AVN_VALUE				,m_CntlUPI_AVN_value				);
	DDX_Control(pDX, IDC_CUP_DS_TRANID				,m_CntlUPI_DS_Transn_id				);
	DDX_Control(pDX, IDC_CUP_SECRTY_RISK_INFO_IP_VALUE	  ,m_CntlUPI_Secrty_Risk_IP_value    );
	DDX_Control(pDX, IDC_CUP_3DS2_VERSION			,m_CntlUPI_3DS2_version				);
	DDX_Control(pDX, IDC_CUP_ECI_VALUE				,m_CntlUPI_ECI_value				);
	DDX_Control(pDX, IDC_CUP_USAGE_AM_VALUE			,m_CntlUPI_Secrty_Info_AM_value		);
	DDX_Control(pDX, IDC_JCB_CAVV_VALUE				,m_CntlJCB_3DS2_CAVV_value			);
	DDX_Control(pDX, IDC_JCB_ECI_VALUE				,m_CntlJCB_ECI_value				);
	DDX_Control(pDX, IDC_JCB_3DS2_VERSION			,m_CntlJCB_3DS2_version				);
	DDX_Control(pDX, IDC_PHYSICAL_TERM_SERL_NUMBER  ,m_CntlPhysical_Term_serl_num		);
	DDX_Control(pDX, IDC_PIN_SERVICE_CODE			,m_CntlPIN_service_code				);
	DDX_Control(pDX, IDC_MC_SECRTY_SRVCS_INFO		,m_CntlSecrty_srvcs_info			);

		DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CAVV_RESULT_CODE 				,m_StrCAVV_Result_code				);
	DDX_Text(pDX, IDC_POS_ENVIRONMENT_FIELD 		,m_StrPOS_Env_Field					);
	DDX_Text(pDX, IDC_VS_3DS2_TRANSACTIONID			,m_StrVisa_3DS2_TranID				);
	DDX_Text(pDX, IDC_VS_3DS2_AUTH_RESULT_CODE		,m_StrVisa_3DS2_Auth_Rslt_code		);
	DDX_Text(pDX, IDC_VS_3DS2_AUTHN_METHOD			,m_StrVisa_3DS2_Auth_mthd			);
	DDX_Text(pDX, IDC_VS_3DS2_SECURE_INDCR			,m_StrVisa_3DS2_Sec_indcr			);
	DDX_Text(pDX, IDC_VS_RESPONSE_REASON_CODE		,m_StrVisa_Resp_reason_code			);
	DDX_Text(pDX, IDC_VS_STIP_REASON_CODE			,m_StrVisa_STIP_reason_code			);
	DDX_Text(pDX, IDC_VS_MVV_VALUE					,m_StrVisa_MVV_value				);
	DDX_Text(pDX, IDC_MC_MERCH_ADVICE_CODE			,m_StrMC_Merch_Advice_code			);
	DDX_Text(pDX, IDC_MC_TRAN_LNK_IDENTIFIER		,m_StrMC_TranLink_ID				);
	DDX_Text(pDX, IDC_MC_PROGRAM_PROTOCOL			,m_StrMC_Prgrm_prtcl				);
	DDX_Text(pDX, IDC_MC_DS_TRAN_ID					,m_StrMC_DS_Tranid					);
	DDX_Text(pDX, IDC_MC_UCAF_VALUE					,m_StrMC_UCAF_value					);
	DDX_Text(pDX, IDC_MC_MERCH_CTRY_OF_ORIGIN		,m_StrMC_Merch_Cntry_orgn			);
	DDX_Text(pDX, IDC_MC_PAYMT_ACCNT_REFERENCE		,m_StrMC_Paymt_accnt_ref			);
	DDX_Text(pDX, IDC_MC_TCC_VALUE					,m_StrMC_TCC_value					);
	DDX_Text(pDX, IDC_MC_MERCH_PAYMT_GWAY_ID		,m_StrMC_Merch_paymt_gwy_id			);
	DDX_Text(pDX, IDC_MC_SANCTION_SCORE_ADDL_INFO	,m_StrMC_Sanction_score_addnl_info	);
	DDX_Text(pDX, IDC_MC_ON_BEHALF_OF_SERVICE		,m_StrMC_OBS_value					);
	DDX_Text(pDX, IDC_MC_TRANSACTION_PURPOSE		,m_StrMC_Trscn_purpose				);
	DDX_Text(pDX, IDC_DCI_FUNCTION_CODE				,m_StrDCI_Func_code					);
	DDX_Text(pDX, IDC_DCI_ERR_DATA_ELEMENT_VALUE	,m_StrDCI_Err_DE_value				);
	DDX_Text(pDX, IDC_DCI_ADDL_AUTH_DATA			,m_StrDCI_Addl_Auth_data			);
	DDX_Text(pDX, IDC_AMX_ECI_VALUE					,m_StrAMX_ECI_value					);
	DDX_Text(pDX, IDC_AMX_AEVV_VALUE				,m_StrAMX_AEVV_value				);
	DDX_Text(pDX, IDC_AMX_TRNSN_ID					,m_StrAMX_Trasn_id					);
	DDX_Text(pDX, IDC_AMX_3DS2_VERSION				,m_StrAMX_3DS2_version				);
	DDX_Text(pDX, IDC_CUP_AVN_VALUE					,m_StrUPI_AVN_value					);
	DDX_Text(pDX, IDC_CUP_DS_TRANID					,m_StrUPI_DS_Transn_id				);
	DDX_Text(pDX, IDC_CUP_SECRTY_RISK_INFO_IP_VALUE	  ,m_StrUPI_Secrty_Risk_IP_value    );
	DDX_Text(pDX, IDC_CUP_3DS2_VERSION				,m_StrUPI_3DS2_version				);
	DDX_Text(pDX, IDC_CUP_ECI_VALUE					,m_StrUPI_ECI_value					);
	DDX_Text(pDX, IDC_CUP_USAGE_AM_VALUE			,m_StrUPI_Secrty_Info_AM_value		);
	DDX_Text(pDX, IDC_JCB_CAVV_VALUE				,m_StrJCB_3DS2_CAVV_value			);
	DDX_Text(pDX, IDC_JCB_ECI_VALUE					,m_StrJCB_ECI_value					);
	DDX_Text(pDX, IDC_JCB_3DS2_VERSION				,m_StrJCB_3DS2_version				);
	DDX_Text(pDX, IDC_PHYSICAL_TERM_SERL_NUMBER		,m_StrPhysical_Term_serl_num		);
	DDX_Text(pDX, IDC_PIN_SERVICE_CODE				,m_StrPIN_service_code				);
	DDX_Text(pDX, IDC_MC_SECRTY_SRVCS_INFO			,m_StrSecrty_srvcs_info				);
		//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddlFldsTransactionDetails, CPropertyPage)
//{{AFX_MSG_MAP(CTransactionDetails)
//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_MC_MERCH_PAYMT_GWAY_ID, &CAddlFldsTransactionDetails::OnEnChangeMcMerchPaymtGwayId)
	ON_EN_CHANGE(IDC_VS_MVV_VALUE, &CAddlFldsTransactionDetails::OnEnChangeVsMvvValue)
	ON_EN_CHANGE(IDC_MC_MERCH_CTRY_OF_ORIGIN, &CAddlFldsTransactionDetails::OnEnChangeMcMerchCtryOfOrigin)
	ON_EN_CHANGE(IDC_AMX_TRNSN_ID, &CAddlFldsTransactionDetails::OnEnChangeAmxTrnsnId)
	ON_EN_CHANGE(IDC_AMX_AEVV_VALUE, &CAddlFldsTransactionDetails::OnEnChangeAmxAevvValue)
	ON_EN_CHANGE(IDC_POS_ENVIRONMENT_FIELD, &CAddlFldsTransactionDetails::OnEnChangePosEnvironmentField)
	ON_EN_CHANGE(IDC_MC_TRANSACTION_PURPOSE, &CAddlFldsTransactionDetails::OnEnChangeMcTransactionPurpose)
	ON_EN_CHANGE(IDC_CUP_3DS2_VERSION, &CAddlFldsTransactionDetails::OnEnChangeCup3ds2Version)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetails message handlers

INT CAddlFldsTransactionDetails :: Check_If_AMEX_Transaction (char *card_num)
{
	if((card_num[0]=='3') &&
	   ((card_num[1] == '4') ||
		(card_num[1] == '7')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT CAddlFldsTransactionDetails :: Check_If_Acquiring_Transaction (char * handler_queue)
{
	if(	strncmp(handler_queue,"dcpiso",6) == 0 ||
		strncmp(handler_queue,"dcpimp",6) == 0 ||
		strncmp(handler_queue,"dcpos",5) == 0 )
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT CAddlFldsTransactionDetails :: Check_If_Issuing_Transaction (char * handler_queue)
{
	if((strncmp(handler_queue,"ncvis3",6) == 0 ||
		strncmp(handler_queue,"ncmcr",5) == 0 ||
		strncmp(handler_queue,"ncpos",5) == 0 ||
		strncmp(handler_queue,"ncatm",5) == 0 ||
		strncmp(handler_queue,"ncjcb",5) == 0 ||
		strncmp(handler_queue,"nccup",5) == 0 ||
		strncmp(handler_queue,"ncdci",5) == 0 ) ||
		(strncmp(handler_queue,"ncvis3",6) == 0 &&
		 (0 == strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"nceqit",6) ||
		  0 == strncmp((char *)sTranRecord.tlf01.authorizing_host_queue,"neqit2",6))))

	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT CAddlFldsTransactionDetails :: Check_If_Diners_Transaction (char *card_num)
{
	if((card_num[0]=='3') &&
	   ((card_num[1] == '0') ||
		(card_num[1] == '6') ||
		(card_num[1] == '8') ||
		(card_num[1] == '9') ))
	{
		return true ;
	}
	else if ((card_num[0]=='6') &&
			 ((card_num[1] == '0') ||
			 (card_num[1] == '4') ||
			 (card_num[1] == '5')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}



INT CAddlFldsTransactionDetails :: Check_If_MC_Transaction (char *card_num)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,card_num,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	card_num[0]=='5')
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


INT CAddlFldsTransactionDetails :: Check_If_VISA_Transaction (char *card_num)
{
	if (card_num[0]=='4')
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT CAddlFldsTransactionDetails :: Check_If_JCB_Transaction (char *card_num)
{
	if((card_num[0]=='3') &&
	   (card_num[1]=='5'))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

BOOL CAddlFldsTransactionDetails::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CFont m_font;
	LOGFONT lf;
	CHAR strErrorMessage[256]= {0};
	/*strncpy( (char *)sTranRecord.tlf01.primary_key.transaction_id, (char *)Transactionid ,sizeof(Transactionid));
	int rcode = txdsapi_get_record ( (pBYTE)&sTranRecord, sizeof (sTranRecord), TLF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
            m_StrMessageLbl = "The Additional Fields for this Transaction was not found.";
			break;
		}
		UpdateData(FALSE);
		return FALSE;
	}*/

	if (m_bTran)
	{
		PopulateAddlFldsFraudTranDialogFromDatabase ( sTranRecord );
	}

	if (m_bTran_stip)
	{
		PopulateAddlFldsFraudTranSTIPDialogFromDatabase ( sTranRecordS );
	}

		UpdateData(FALSE);	
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAddlFldsTransactionDetails::PopulateAddlFldsFraudTranDialogFromDatabase ( ADDLFLDSTRAN_RECORD sTranRecord  )
{
	CHAR strErrorMessage[256]= {0};
	char	cavv_buff[1024] = {0};
	MRA01   mra01_table   ;
	PTSN01  sptsn01 = {0};
	char	Cavv_components[20]={0};
	char	Version[3] = {0};
	int		rcode = 0;
	char * substr=NULL;

	rcode = txdsapi_get_record ( (pBYTE)&sTranRecord, sizeof (sTranRecord), TLF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
            m_StrMessageLbl = "ERROR:The Additional fields for the transaction record was not found.";
			break;
		}
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
		return FALSE;
	}

	//Get PTSN01 record
	if  (true ==  Check_If_Acquiring_Transaction((char *)sTranRecord.tlf01.handler_queue))
	{
		strncpy((char *)sptsn01.transaction_id,(char *)sTranRecord.tlf01.primary_key.transaction_id,sizeof(sptsn01.transaction_id));
		rcode = txdsapi_get_record ( (pBYTE)&sptsn01, sizeof (sptsn01), PTSN01_DATA, strErrorMessage);
	
		if ( rcode != PTEMSG_OK )
		{
			switch( rcode )
			{
				case PTEMSG_NOT_FOUND:
					//m_StrMessageLbl = "ERROR:The transaction record for this batch was not found.";
				break;
			}
			//m_StrMessageLbl = strErrorMessage;
			UpdateData(FALSE);
			//return FALSE;
		}
	}


		if(Check_If_VISA_Transaction ((char *)sTranRecord.tlf01.card_num) == true)
		{
			if( strncmp((char *)sTranRecord.tlf01.message_type,"0100",4) == 0 ||
				strncmp((char *)sTranRecord.tlf01.message_type,"0200",4) == 0 ||
				strncmp((char *)sTranRecord.tlf01.message_type,"0220",4) == 0 ||
				(strncmp((char *)sTranRecord.tlf01.message_type,"0120",4) == 0 &&
				strncmp((char *)sTranRecord.tlf01.pos_condition_code,"59",2) == 0))
			{
				if(sTranRecord.tlf01.cvc != '\0')
				{
					m_StrCAVV_Result_code += sTranRecord.tlf01.cvc;
				}
				if(sTranRecord.tlf01.product_codes[7].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" , sTranRecord.tlf01.product_codes[7].quantity[0]);
					m_StrPOS_Env_Field += cavv_buff;
				}
			}
			if(sTranRecord.tlf01.operator_id[0] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s%s" , sTranRecord.tlf01.operator_id,sTranRecord.tlf01.workstation);
					m_StrVisa_3DS2_TranID += cavv_buff;
			
			}

			if(sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0' )
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c%c" ,sTranRecord.tlf01.dcf01_retired_cwk[0], sTranRecord.tlf01.dcf01_retired_cwk[1] );
					m_StrVisa_3DS2_Auth_Rslt_code += cavv_buff;

			}
			
			if(sTranRecord.tlf01.dcf01_retired_cwk[2] != '\0' )
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c%c" ,sTranRecord.tlf01.dcf01_retired_cwk[2], sTranRecord.tlf01.dcf01_retired_cwk[3] );
					m_StrVisa_3DS2_Auth_mthd += cavv_buff;

			}
			if('\0' != sTranRecord.tlf01.product_codes[0].code[2])
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" , sTranRecord.tlf01.product_codes[0].code[2]);
					m_StrVisa_3DS2_Sec_indcr += cavv_buff;
			}

			if(sTranRecord.tlf01.resp_source[0] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,sTranRecord.tlf01.resp_source[0]);
					m_StrVisa_Resp_reason_code += cavv_buff;
			}
			if(sTranRecord.tlf01.resp_source[3] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,sTranRecord.tlf01.resp_source[3]);
					m_StrVisa_STIP_reason_code += cavv_buff;
			}

			if(sTranRecord.tlf01.product_codes[2].amount[0] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,sTranRecord.tlf01.product_codes[2].amount);
					m_StrVisa_MVV_value += (char *)sTranRecord.tlf01.product_codes[2].amount;
			}

		}
		else if (Check_If_MC_Transaction ((char *)sTranRecord.tlf01.card_num) == true)
		{
			if (0 != strncmp((char *)sTranRecord.tlf01.product_codes[1].code, "stndin", 6))
			{
				if(sTranRecord.tlf01.product_codes[0].amount[0] != '\0' && (strstr((char *)(sTranRecord.tlf01.product_codes[0].amount), "DE48 SE84:") != NULL))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s", sTranRecord.tlf01.product_codes[0].amount);
					m_StrMC_Merch_Advice_code += cavv_buff;
				}
				else if(sTranRecord.tlf01.response_text[0] != '\0' && (strstr((char *)(sTranRecord.tlf01.response_text), "DE48 SE84:") != NULL))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					substr = strstr((char *)sTranRecord.tlf01.response_text,"DE48 SE84:");
					sprintf (cavv_buff,"%s", substr);
					m_StrMC_Merch_Advice_code += cavv_buff;
				}
			}				

				if(sTranRecord.tlf01.product_codes[19].amount[0] != '\0'        && 
				   0 != strncmp((char *)sTranRecord.tlf01.issuer_id,"nceqit",6) &&
			       0 != strncmp((char *)sTranRecord.tlf01.issuer_id,"neqit2",6) &&
				   0 != strncmp((char *)sTranRecord.tlf01.handler_queue,"dcp",3) &&
				   0 != strncmp((char *)sTranRecord.tlf01.handler_queue,"VOICE",5))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n TranLink Identifier(DE105 SE01) = %s  \r\n", sTranRecord.tlf01.product_codes[19].amount);
					m_StrMC_TranLink_ID +=  (char *)sTranRecord.tlf01.product_codes[19].amount;
				}

				if(sTranRecord.tlf01.product_codes[10].amount[0] != '\0' && 
				   ((0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso",6) ||
				     0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"VOICE", 5))|| 0 == strncmp((char *)sTranRecord.tlf01.acquirer_id,"ncmcrd",6)) )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s%s", sTranRecord.tlf01.product_codes[10].amount, sTranRecord.tlf01.product_codes[1].amount);
					m_StrMC_TranLink_ID += cavv_buff;
				}

				if( sTranRecord.tlf01.product_codes[11].quantity[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" , sTranRecord.tlf01.product_codes[11].quantity[0]);
					m_StrMC_Prgrm_prtcl += cavv_buff;
				}
				if(sTranRecord.tlf01.workstation[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 SF66 SF2(DS Transaction ID) = %s%s  \r\n" ,sTranRecord.tlf01.workstation,sTranRecord.tlf01.operator_id);
					m_StrMC_DS_Tranid += (char *)sTranRecord.tlf01.workstation;
					m_StrMC_DS_Tranid += (char *)sTranRecord.tlf01.operator_id;
				}
				if(sTranRecord.tlf01.product_codes[5].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 SF43 = %s  \r\n" ,sTranRecord.tlf01.product_codes[5].quantity);
					m_StrMC_UCAF_value = sTranRecord.tlf01.product_codes[5].quantity;
				}
					if(sTranRecord.tlf01.orig_auth_number[0] != '\0' && sTranRecord.tlf01.tx_key !=AUTH_REVERSAL_RESPONSE) 
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 SE37 SF04 = %s \r\n" ,sTranRecord.tlf01.orig_auth_number);
					m_StrMC_Merch_Cntry_orgn = sTranRecord.tlf01.orig_auth_number;
				}

				if(sTranRecord.tlf01.down_payment[0] != '\0' && sTranRecord.tlf01.total_interest[0] != '\0' && sTranRecord.tlf01.actual_amount[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s%s%s" ,sTranRecord.tlf01.down_payment,sTranRecord.tlf01.total_interest,sTranRecord.tlf01.actual_amount);
					m_StrMC_Paymt_accnt_ref += cavv_buff;

				}
				if(sTranRecord.tlf01.dbrefund_amount[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 SE37 SF05 = %s \r\n" ,sTranRecord.tlf01.dbrefund_amount);
					m_StrMC_Merch_paymt_gwy_id = sTranRecord.tlf01.dbrefund_amount;

				}
				if(sTranRecord.tlf01.product_codes[19].quantity[0] != '\0' && sTranRecord.tlf01.product_codes[16].quantity[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s%s" ,sTranRecord.tlf01.product_codes[19].quantity,sTranRecord.tlf01.product_codes[16].quantity);
					m_StrMC_OBS_value += cavv_buff;
				}
				if(sTranRecord.tlf01.remote_nsp_type[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 TCC Value= %s \r\n" ,sTranRecord.tlf01.remote_nsp_type);
					m_StrMC_TCC_value = sTranRecord.tlf01.remote_nsp_type;

				}

				if(sTranRecord.tlf01.visa_validation_code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,sTranRecord.tlf01.visa_validation_code);
					m_StrSecrty_srvcs_info += cavv_buff;
				}
				if(sTranRecord.tlf01.product_codes[10].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE108 SE03 SF05 = %s  \r\n" ,sTranRecord.tlf01.product_codes[10].quantity);
					m_StrMC_Trscn_purpose = sTranRecord.tlf01.product_codes[10].quantity;
				}
				if((sTranRecord.tlf01.product_codes[8].amount[0] != '\0' || sTranRecord.tlf01.product_codes[14].amount[0] != '\0' )&&
					strcmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso") != 0)
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 SE67 SF02 = %s%s  \r\n" , sTranRecord.tlf01.product_codes[8].amount, sTranRecord.tlf01.product_codes[14].amount);
					sprintf (cavv_buff,"%s%s" , sTranRecord.tlf01.product_codes[8].amount, sTranRecord.tlf01.product_codes[14].amount);
					m_StrMC_Sanction_score_addnl_info += cavv_buff;
				}

				if (sTranRecord.tlf01.agent_id_len[0] != '\0' && 
					(strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcrd", 5) == 0 || 
					 strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr3", 6) == 0) &&
					 strncmp((char *)sTranRecord.tlf01.acquirer_id,"ncmcr2",6)  == 0 )				
				{
						memset(cavv_buff,0x00,sizeof(cavv_buff));
						sprintf (cavv_buff,"%s" ,sTranRecord.tlf01.agent_id_len);
						m_StrPIN_service_code += cavv_buff;
				}
		}
		else if(true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
		{
				if(sTranRecord.tlf01.product_codes[0].code[0] != '\0' &&
					 0 != strncmp((char *)sTranRecord.tlf01.dcf01_retired_cwk+4, "00", 2))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE24 = %s \r" ,sTranRecord.tlf01.product_codes[0].code);
					m_StrDCI_Func_code = sTranRecord.tlf01.product_codes[0].code;
				}
				if (sTranRecord.tlf01.operator_id[0] != '\0')
				{
					memset(cavv_buff, 0x00, sizeof(cavv_buff));
					//sprintf(cavv_buff,"\r\n De44.4 value = %c%c \r" ,sTranRecord.tlf01.operator_id[0], sTranRecord.tlf01.operator_id[1]);
					sprintf(cavv_buff,"%c%c" ,sTranRecord.tlf01.operator_id[0], sTranRecord.tlf01.operator_id[1]);
					m_StrDCI_Err_DE_value += cavv_buff;

				}
				//if (sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0' && sTranRecord.tlf01.operator_id[0] != '\0' && sTranRecord.tlf01.workstation[0] != '\0')
				if (sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf(cavv_buff,"\r\n DE122 value = %s \r\n", sTranRecord.tlf01.dcf01_retired_cwk);
					m_StrDCI_Addl_Auth_data += (char * )sTranRecord.tlf01.dcf01_retired_cwk;
				}
		}
		else if(true == Check_If_AMEX_Transaction ((char *)sTranRecord.tlf01.card_num))
		{
				if(sTranRecord.tlf01.profile[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf(cavv_buff,"\r\n B61 SF4 = %s  \r", sTranRecord.tlf01.profile);
					m_StrAMX_ECI_value = sTranRecord.tlf01.profile;
				}
				if(sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf(cavv_buff,"\r\n B61 SF6 = %s  \r", sTranRecord.tlf01.dcf01_retired_cwk);
					m_StrAMX_AEVV_value = sTranRecord.tlf01.dcf01_retired_cwk;
				}
				if(sTranRecord.tlf01.operator_id[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf(cavv_buff,"\r\n B61 SF8 = %s%s\r",sTranRecord.tlf01.workstation,sTranRecord.tlf01.operator_id);
					sprintf(cavv_buff,"%s%s",sTranRecord.tlf01.workstation,sTranRecord.tlf01.operator_id);
					m_StrAMX_Trasn_id += cavv_buff;
				}
				if(sTranRecord.tlf01.workstation[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					memset(Cavv_components, 0x00, sizeof(Cavv_components));
					strncpy(Cavv_components,(char *)sTranRecord.tlf01.workstation,12);
					//sprintf(cavv_buff,"\r\n 3DS vers = %s\r\n", Cavv_components);
					sprintf(cavv_buff,"%s", Cavv_components);
					m_StrAMX_3DS2_version += cavv_buff;
				}
		}
		else if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5) ||
				0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6))
		{

				if(sTranRecord.tlf01.product_codes[1].amount[0] != '\0' && sTranRecord.tlf01.product_codes[2].amount[0] != '\0' && sTranRecord.tlf01.product_codes[8].amount[0] != '\0' &&
					sTranRecord.tlf01.product_codes[0].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE61.6 AVN Value= %s%s%s%s \r\n" ,sTranRecord.tlf01.product_codes[1].amount,sTranRecord.tlf01.product_codes[2].amount,sTranRecord.tlf01.product_codes[8].amount,sTranRecord.tlf01.product_codes[0].code);
					sprintf (cavv_buff,"%s%s%s%s" ,sTranRecord.tlf01.product_codes[1].amount,sTranRecord.tlf01.product_codes[2].amount,sTranRecord.tlf01.product_codes[8].amount,sTranRecord.tlf01.product_codes[0].code);
					m_StrUPI_AVN_value += cavv_buff;

				}

				if(sTranRecord.tlf01.product_codes[13].amount[0] != '\0' && sTranRecord.tlf01.product_codes[18].amount[0] != '\0' && sTranRecord.tlf01.product_codes[18].amount[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE125 DSTransaction= %s%s%s \r\n" ,sTranRecord.tlf01.product_codes[13].amount,sTranRecord.tlf01.product_codes[18].amount,sTranRecord.tlf01.product_codes[19].amount);
					sprintf (cavv_buff,"%s%s%s" ,sTranRecord.tlf01.product_codes[13].amount,sTranRecord.tlf01.product_codes[18].amount,sTranRecord.tlf01.product_codes[19].amount);
					m_StrUPI_DS_Transn_id += cavv_buff;

				}
				if(sTranRecord.tlf01.product_codes[15].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n De125 Tag01 Usage IP= %s \r\n" ,sTranRecord.tlf01.product_codes[15].code);
					sprintf (cavv_buff,"%s" ,sTranRecord.tlf01.product_codes[15].code);
					m_StrUPI_Secrty_Risk_IP_value += cavv_buff;
				}

				if(sTranRecord.tlf01.product_codes[0].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n 3D Secure Version= %s \r\n" ,&sTranRecord.tlf01.product_codes[0].code[3]);
					sprintf (cavv_buff,"%c" ,sTranRecord.tlf01.product_codes[0].code[3]);
					m_StrUPI_3DS2_version += cavv_buff;
				}

				if(sTranRecord.tlf01.profile[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n ECI(DE61.4)= %s \r\n" ,sTranRecord.tlf01.product_codes[5].code);
					sprintf (cavv_buff,"%s" ,sTranRecord.tlf01.profile);
					m_StrUPI_ECI_value += cavv_buff;
				}		
				if(sTranRecord.tlf01.resp_source[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE 61.6 Usage 5(AM)= %s \r\n" ,sTranRecord.tlf01.resp_source);
					m_StrUPI_Secrty_Info_AM_value = sTranRecord.tlf01.resp_source;
				}

			
		}
		else if(true == Check_If_JCB_Transaction ((char *)sTranRecord.tlf01.card_num))
		{
			if(sTranRecord.tlf01.product_codes[3].amount[0] != '\0')
			{
				strncpy(Cavv_components,(char *)sTranRecord.tlf01.product_codes[3].amount,6);
				strncat(Cavv_components,(char *)&sTranRecord.tlf01.product_codes[6].amount[2],10);
				strncat(Cavv_components,(char *)sTranRecord.tlf01.product_codes[8].amount,6);
				memset(cavv_buff,0x00,sizeof(cavv_buff));
				//sprintf (cavv_buff,"\r\nCAVV Components= %s \r\n" ,Cavv_components);
				sprintf (cavv_buff,"%s" ,Cavv_components);
				m_StrJCB_3DS2_CAVV_value += cavv_buff;
			}
			if(sTranRecord.tlf01.profile[0] != '\0')
			{		
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\nECI= %s \r\n" ,sTranRecord.tlf01.profile);
					m_StrJCB_ECI_value = sTranRecord.tlf01.profile;
			}

			if(sTranRecord.tlf01.product_codes[3].amount[0] != '\0')
			{		
				    strncpy(Version,(char *)&sTranRecord.tlf01.product_codes[3].amount[2],2);
					//memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n3D Secure Version= %s \r\n" ,Version);
					m_StrJCB_3DS2_version = Version;
			}
			
		}
		if(true ==  Check_If_Acquiring_Transaction((char *)sTranRecord.tlf01.handler_queue))
			{
				if(sptsn01.serial_number[0] != '\0')
				{		
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,sptsn01.serial_number);
					m_StrPhysical_Term_serl_num += cavv_buff;
				}
			}

	UpdateData(FALSE);
   return TRUE;

}

BOOL CAddlFldsTransactionDetails::PopulateAddlFldsFraudTranSTIPDialogFromDatabase ( ADDLFLDSTRAN_STIP_RECORD sTranRecord  )
{
	CHAR strErrorMessage[256]= {0};
	char	cavv_buff[1024] = {0};
	MRA01   mra01_table   ;
	PTSN01  sptsn01 = {0};
	char	Cavv_components[20]={0};
	char	Version[3] = {0};
	int		rcode = 0;
	char * substr=NULL;

	rcode = txdsapi_get_record ( (pBYTE)&sTranRecord, sizeof (sTranRecord), TLF01_STIP_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
            m_StrMessageLbl = "ERROR:The Additional fields for the transaction record was not found.";
			break;
		}
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
		return FALSE;
	}

	//Get PTSN01 record
	if  (true ==  Check_If_Acquiring_Transaction((char *)sTranRecord.tlf01.handler_queue))
	{
		strncpy((char *)sptsn01.transaction_id,(char *)sTranRecord.tlf01.primary_key.transaction_id,sizeof(sptsn01.transaction_id));
		rcode = txdsapi_get_record ( (pBYTE)&sptsn01, sizeof (sptsn01), PTSN01_DATA, strErrorMessage);
	
		if ( rcode != PTEMSG_OK )
		{
			switch( rcode )
			{
				case PTEMSG_NOT_FOUND:
					//m_StrMessageLbl = "ERROR:The transaction record for this batch was not found.";
				break;
			}
			//m_StrMessageLbl = strErrorMessage;
			UpdateData(FALSE);
			//return FALSE;
		}
	}


		if(Check_If_VISA_Transaction ((char *)sTranRecord.tlf01.card_num) == true)
		{
			if( strncmp((char *)sTranRecord.tlf01.message_type,"0100",4) == 0 ||
				strncmp((char *)sTranRecord.tlf01.message_type,"0200",4) == 0 ||
				strncmp((char *)sTranRecord.tlf01.message_type,"0220",4) == 0 ||
				(strncmp((char *)sTranRecord.tlf01.message_type,"0120",4) == 0 &&
				strncmp((char *)sTranRecord.tlf01.pos_condition_code,"59",2) == 0))
			{
				if(sTranRecord.tlf01.cvc != '\0')
				{
					m_StrCAVV_Result_code += sTranRecord.tlf01.cvc;
				}
				if(sTranRecord.tlf01.product_codes[7].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" , sTranRecord.tlf01.product_codes[7].quantity[0]);
					m_StrPOS_Env_Field += cavv_buff;
				}
			}
			if(sTranRecord.tlf01.operator_id[0] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s%s" , sTranRecord.tlf01.operator_id,sTranRecord.tlf01.workstation);
					m_StrVisa_3DS2_TranID += cavv_buff;
			
			}

			if(sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0' )
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c%c" ,sTranRecord.tlf01.dcf01_retired_cwk[0], sTranRecord.tlf01.dcf01_retired_cwk[1] );
					m_StrVisa_3DS2_Auth_Rslt_code += cavv_buff;

			}
			
			if(sTranRecord.tlf01.dcf01_retired_cwk[2] != '\0' )
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c%c" ,sTranRecord.tlf01.dcf01_retired_cwk[2], sTranRecord.tlf01.dcf01_retired_cwk[3] );
					m_StrVisa_3DS2_Auth_mthd += cavv_buff;

			}
			if('\0' != sTranRecord.tlf01.product_codes[0].code[2])
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" , sTranRecord.tlf01.product_codes[0].code[2]);
					m_StrVisa_3DS2_Sec_indcr += cavv_buff;
			}

			if(sTranRecord.tlf01.resp_source[0] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,sTranRecord.tlf01.resp_source[0]);
					m_StrVisa_Resp_reason_code += cavv_buff;
			}
			if(sTranRecord.tlf01.resp_source[3] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" ,sTranRecord.tlf01.resp_source[3]);
					m_StrVisa_STIP_reason_code += cavv_buff;
			}

			if(sTranRecord.tlf01.product_codes[2].amount[0] != '\0')
			{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,sTranRecord.tlf01.product_codes[2].amount);
					m_StrVisa_MVV_value += (char *)sTranRecord.tlf01.product_codes[2].amount;
			}

		}
		else if (Check_If_MC_Transaction ((char *)sTranRecord.tlf01.card_num) == true)
		{
			if (0 != strncmp((char *)sTranRecord.tlf01.product_codes[1].code, "stndin", 6))
			{
				if(sTranRecord.tlf01.product_codes[0].amount[0] != '\0' && (strstr((char *)(sTranRecord.tlf01.product_codes[0].amount), "DE48 SE84:") != NULL))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s", sTranRecord.tlf01.product_codes[0].amount);
					m_StrMC_Merch_Advice_code += cavv_buff;
				}
				else if(sTranRecord.tlf01.response_text[0] != '\0' && (strstr((char *)(sTranRecord.tlf01.response_text), "DE48 SE84:") != NULL))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					substr = strstr((char *)sTranRecord.tlf01.response_text,"DE48 SE84:");
					sprintf (cavv_buff,"%s", substr);
					m_StrMC_Merch_Advice_code += cavv_buff;
				}
			}				

				if(sTranRecord.tlf01.product_codes[19].amount[0] != '\0'        && 
				   0 != strncmp((char *)sTranRecord.tlf01.issuer_id,"nceqit",6) &&
			       0 != strncmp((char *)sTranRecord.tlf01.issuer_id,"neqit2",6) &&
				   0 != strncmp((char *)sTranRecord.tlf01.handler_queue,"dcp",3) &&
				   0 != strncmp((char *)sTranRecord.tlf01.handler_queue,"VOICE",5))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n TranLink Identifier(DE105 SE01) = %s  \r\n", sTranRecord.tlf01.product_codes[19].amount);
					m_StrMC_TranLink_ID +=  (char *)sTranRecord.tlf01.product_codes[19].amount;
				}

				if(sTranRecord.tlf01.product_codes[10].amount[0] != '\0' && 
				   ((0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso",6) ||
				     0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"VOICE", 5))|| 0 == strncmp((char *)sTranRecord.tlf01.acquirer_id,"ncmcrd",6)) )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s%s", sTranRecord.tlf01.product_codes[10].amount, sTranRecord.tlf01.product_codes[1].amount);
					m_StrMC_TranLink_ID += cavv_buff;
				}

				if( sTranRecord.tlf01.product_codes[11].quantity[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%c" , sTranRecord.tlf01.product_codes[11].quantity[0]);
					m_StrMC_Prgrm_prtcl += cavv_buff;
				}
				if(sTranRecord.tlf01.workstation[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 SF66 SF2(DS Transaction ID) = %s%s  \r\n" ,sTranRecord.tlf01.workstation,sTranRecord.tlf01.operator_id);
					m_StrMC_DS_Tranid += (char *)sTranRecord.tlf01.workstation;
					m_StrMC_DS_Tranid += (char *)sTranRecord.tlf01.operator_id;
				}
				if(sTranRecord.tlf01.product_codes[5].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 SF43 = %s  \r\n" ,sTranRecord.tlf01.product_codes[5].quantity);
					m_StrMC_UCAF_value = sTranRecord.tlf01.product_codes[5].quantity;
				}
					if(sTranRecord.tlf01.orig_auth_number[0] != '\0' && sTranRecord.tlf01.tx_key !=AUTH_REVERSAL_RESPONSE) 
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 SE37 SF04 = %s \r\n" ,sTranRecord.tlf01.orig_auth_number);
					m_StrMC_Merch_Cntry_orgn = sTranRecord.tlf01.orig_auth_number;
				}

				if(sTranRecord.tlf01.down_payment[0] != '\0' && sTranRecord.tlf01.total_interest[0] != '\0' && sTranRecord.tlf01.actual_amount[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s%s%s" ,sTranRecord.tlf01.down_payment,sTranRecord.tlf01.total_interest,sTranRecord.tlf01.actual_amount);
					m_StrMC_Paymt_accnt_ref += cavv_buff;

				}
				if(sTranRecord.tlf01.dbrefund_amount[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 SE37 SF05 = %s \r\n" ,sTranRecord.tlf01.dbrefund_amount);
					m_StrMC_Merch_paymt_gwy_id = sTranRecord.tlf01.dbrefund_amount;

				}
				if(sTranRecord.tlf01.product_codes[19].quantity[0] != '\0' && sTranRecord.tlf01.product_codes[16].quantity[0] != '\0' )
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s%s" ,sTranRecord.tlf01.product_codes[19].quantity,sTranRecord.tlf01.product_codes[16].quantity);
					m_StrMC_OBS_value += cavv_buff;
				}
				if(sTranRecord.tlf01.remote_nsp_type[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 TCC Value= %s \r\n" ,sTranRecord.tlf01.remote_nsp_type);
					m_StrMC_TCC_value = sTranRecord.tlf01.remote_nsp_type;

				}

				if(sTranRecord.tlf01.visa_validation_code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,sTranRecord.tlf01.visa_validation_code);
					m_StrSecrty_srvcs_info += cavv_buff;
				}
				if(sTranRecord.tlf01.product_codes[10].quantity[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE108 SE03 SF05 = %s  \r\n" ,sTranRecord.tlf01.product_codes[10].quantity);
					m_StrMC_Trscn_purpose = sTranRecord.tlf01.product_codes[10].quantity;
				}
				if((sTranRecord.tlf01.product_codes[8].amount[0] != '\0' || sTranRecord.tlf01.product_codes[14].amount[0] != '\0' )&&
					strcmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso") != 0)
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE48 SE67 SF02 = %s%s  \r\n" , sTranRecord.tlf01.product_codes[8].amount, sTranRecord.tlf01.product_codes[14].amount);
					sprintf (cavv_buff,"%s%s" , sTranRecord.tlf01.product_codes[8].amount, sTranRecord.tlf01.product_codes[14].amount);
					m_StrMC_Sanction_score_addnl_info += cavv_buff;
				}

				if (sTranRecord.tlf01.agent_id_len[0] != '\0' && 
					(strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcrd", 5) == 0 || 
					 strncmp((char *)sTranRecord.tlf01.handler_queue,"ncmcr3", 6) == 0) &&
					 strncmp((char *)sTranRecord.tlf01.acquirer_id,"ncmcr2",6)  == 0 )				
				{
						memset(cavv_buff,0x00,sizeof(cavv_buff));
						sprintf (cavv_buff,"%s" ,sTranRecord.tlf01.agent_id_len);
						m_StrPIN_service_code += cavv_buff;
				}
		}
		else if(true ==  Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num))
		{
				if(sTranRecord.tlf01.product_codes[0].code[0] != '\0' &&
					 0 != strncmp((char *)sTranRecord.tlf01.dcf01_retired_cwk+4, "00", 2))
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE24 = %s \r" ,sTranRecord.tlf01.product_codes[0].code);
					m_StrDCI_Func_code = sTranRecord.tlf01.product_codes[0].code;
				}
				if (sTranRecord.tlf01.operator_id[0] != '\0')
				{
					memset(cavv_buff, 0x00, sizeof(cavv_buff));
					//sprintf(cavv_buff,"\r\n De44.4 value = %c%c \r" ,sTranRecord.tlf01.operator_id[0], sTranRecord.tlf01.operator_id[1]);
					sprintf(cavv_buff,"%c%c" ,sTranRecord.tlf01.operator_id[0], sTranRecord.tlf01.operator_id[1]);
					m_StrDCI_Err_DE_value += cavv_buff;

				}
				//if (sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0' && sTranRecord.tlf01.operator_id[0] != '\0' && sTranRecord.tlf01.workstation[0] != '\0')
				if (sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf(cavv_buff,"\r\n DE122 value = %s \r\n", sTranRecord.tlf01.dcf01_retired_cwk);
					m_StrDCI_Addl_Auth_data += (char * )sTranRecord.tlf01.dcf01_retired_cwk;
				}
		}
		else if(true == Check_If_AMEX_Transaction ((char *)sTranRecord.tlf01.card_num))
		{
				if(sTranRecord.tlf01.profile[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf(cavv_buff,"\r\n B61 SF4 = %s  \r", sTranRecord.tlf01.profile);
					m_StrAMX_ECI_value = sTranRecord.tlf01.profile;
				}
				if(sTranRecord.tlf01.dcf01_retired_cwk[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf(cavv_buff,"\r\n B61 SF6 = %s  \r", sTranRecord.tlf01.dcf01_retired_cwk);
					m_StrAMX_AEVV_value = sTranRecord.tlf01.dcf01_retired_cwk;
				}
				if(sTranRecord.tlf01.operator_id[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf(cavv_buff,"\r\n B61 SF8 = %s%s\r",sTranRecord.tlf01.workstation,sTranRecord.tlf01.operator_id);
					sprintf(cavv_buff,"%s%s",sTranRecord.tlf01.workstation,sTranRecord.tlf01.operator_id);
					m_StrAMX_Trasn_id += cavv_buff;
				}
				if(sTranRecord.tlf01.workstation[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					memset(Cavv_components, 0x00, sizeof(Cavv_components));
					strncpy(Cavv_components,(char *)sTranRecord.tlf01.workstation,12);
					//sprintf(cavv_buff,"\r\n 3DS vers = %s\r\n", Cavv_components);
					sprintf(cavv_buff,"%s", Cavv_components);
					m_StrAMX_3DS2_version += cavv_buff;
				}
		}
		else if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup" ,5) ||
				0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"nccup3" ,6))
		{

				if(sTranRecord.tlf01.product_codes[1].amount[0] != '\0' && sTranRecord.tlf01.product_codes[2].amount[0] != '\0' && sTranRecord.tlf01.product_codes[8].amount[0] != '\0' &&
					sTranRecord.tlf01.product_codes[0].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE61.6 AVN Value= %s%s%s%s \r\n" ,sTranRecord.tlf01.product_codes[1].amount,sTranRecord.tlf01.product_codes[2].amount,sTranRecord.tlf01.product_codes[8].amount,sTranRecord.tlf01.product_codes[0].code);
					sprintf (cavv_buff,"%s%s%s%s" ,sTranRecord.tlf01.product_codes[1].amount,sTranRecord.tlf01.product_codes[2].amount,sTranRecord.tlf01.product_codes[8].amount,sTranRecord.tlf01.product_codes[0].code);
					m_StrUPI_AVN_value += cavv_buff;

				}

				if(sTranRecord.tlf01.product_codes[13].amount[0] != '\0' && sTranRecord.tlf01.product_codes[18].amount[0] != '\0' && sTranRecord.tlf01.product_codes[18].amount[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE125 DSTransaction= %s%s%s \r\n" ,sTranRecord.tlf01.product_codes[13].amount,sTranRecord.tlf01.product_codes[18].amount,sTranRecord.tlf01.product_codes[19].amount);
					sprintf (cavv_buff,"%s%s%s" ,sTranRecord.tlf01.product_codes[13].amount,sTranRecord.tlf01.product_codes[18].amount,sTranRecord.tlf01.product_codes[19].amount);
					m_StrUPI_DS_Transn_id += cavv_buff;

				}
				if(sTranRecord.tlf01.product_codes[15].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n De125 Tag01 Usage IP= %s \r\n" ,sTranRecord.tlf01.product_codes[15].code);
					sprintf (cavv_buff,"%s" ,sTranRecord.tlf01.product_codes[15].code);
					m_StrUPI_Secrty_Risk_IP_value += cavv_buff;
				}

				if(sTranRecord.tlf01.product_codes[0].code[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n 3D Secure Version= %s \r\n" ,&sTranRecord.tlf01.product_codes[0].code[3]);
					sprintf (cavv_buff,"%c" ,sTranRecord.tlf01.product_codes[0].code[3]);
					m_StrUPI_3DS2_version += cavv_buff;
				}

				if(sTranRecord.tlf01.profile[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n ECI(DE61.4)= %s \r\n" ,sTranRecord.tlf01.product_codes[5].code);
					sprintf (cavv_buff,"%s" ,sTranRecord.tlf01.profile);
					m_StrUPI_ECI_value += cavv_buff;
				}		
				if(sTranRecord.tlf01.resp_source[0] != '\0')
				{
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n DE 61.6 Usage 5(AM)= %s \r\n" ,sTranRecord.tlf01.resp_source);
					m_StrUPI_Secrty_Info_AM_value = sTranRecord.tlf01.resp_source;
				}

			
		}
		else if(true == Check_If_JCB_Transaction ((char *)sTranRecord.tlf01.card_num))
		{
			if(sTranRecord.tlf01.product_codes[3].amount[0] != '\0')
			{
				strncpy(Cavv_components,(char *)sTranRecord.tlf01.product_codes[3].amount,6);
				strncat(Cavv_components,(char *)&sTranRecord.tlf01.product_codes[6].amount[2],10);
				strncat(Cavv_components,(char *)sTranRecord.tlf01.product_codes[8].amount,6);
				memset(cavv_buff,0x00,sizeof(cavv_buff));
				//sprintf (cavv_buff,"\r\nCAVV Components= %s \r\n" ,Cavv_components);
				sprintf (cavv_buff,"%s" ,Cavv_components);
				m_StrJCB_3DS2_CAVV_value += cavv_buff;
			}
			if(sTranRecord.tlf01.profile[0] != '\0')
			{		
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\nECI= %s \r\n" ,sTranRecord.tlf01.profile);
					m_StrJCB_ECI_value = sTranRecord.tlf01.profile;
			}

			if(sTranRecord.tlf01.product_codes[3].amount[0] != '\0')
			{		
				    strncpy(Version,(char *)&sTranRecord.tlf01.product_codes[3].amount[2],2);
					//memset(cavv_buff,0x00,sizeof(cavv_buff));
					//sprintf (cavv_buff,"\r\n3D Secure Version= %s \r\n" ,Version);
					m_StrJCB_3DS2_version = Version;
			}
			
		}
		if(true ==  Check_If_Acquiring_Transaction((char *)sTranRecord.tlf01.handler_queue))
			{
				if(sptsn01.serial_number[0] != '\0')
				{		
					memset(cavv_buff,0x00,sizeof(cavv_buff));
					sprintf (cavv_buff,"%s" ,sptsn01.serial_number);
					m_StrPhysical_Term_serl_num += cavv_buff;
				}
			}

	UpdateData(FALSE);
   return TRUE;

}


void CAddlFldsTransactionDetails::OnEnChangeMcMerchPaymtGwayId()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CAddlFldsTransactionDetails::OnEnChangeVsMvvValue()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CAddlFldsTransactionDetails::OnEnChangeMcMerchCtryOfOrigin()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CAddlFldsTransactionDetails::OnEnChangeAmxTrnsnId()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CAddlFldsTransactionDetails::OnEnChangeAmxAevvValue()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CAddlFldsTransactionDetails::OnEnChangePosEnvironmentField()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CAddlFldsTransactionDetails::OnEnChangeMcTransactionPurpose()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CAddlFldsTransactionDetails::OnEnChangeCup3ds2Version()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
