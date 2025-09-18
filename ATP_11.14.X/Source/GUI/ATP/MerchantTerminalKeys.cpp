/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalKeys.cpp  $
   
      Rev 1.12   Jul 21 2004 12:42:50   lmitchel
   SCR1388: Added new device type  = SMART CARD; dcf01.device_type  = '07'.  Added NONE option to encryption method in Keys dialog.
   
      Rev 1.11   Dec 01 2003 11:29:44   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.10   Mar 04 2003 16:29:46   lmitchel
   Validate length of TPK key and for hex characters; allow entry of blank key
   
      Rev 1.9   Feb 27 2003 10:05:12   lmitchel
   SCR955 - Modifications to expand the following key fields from 16 to 48 bytes:  IDC_CWK_FLD, IDC_ZMK_FLD, IDC_TPK_FLD.  Added validation routines to verify field entry is in Hex digits only and to check length of key entered.
   
      Rev 1.8   Jan 12 2000 14:14:48   mbysani
   Changes mades acc to bugs
   
      Rev 1.3   Oct 15 1999 17:15:28   iarustam
   bug fixes
   
      Rev 1.2   Sep 30 1999 16:17:30   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:54:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:52   iarustam
   Initial Release
   
 
    
*
************************************************************************************/
// MerchantTerminalKeys.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantTerminalKeys.h"
#include "MyUtils.h"
#include "SharedMem.h"
extern "C"
{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalKeys property page

IMPLEMENT_DYNCREATE(CMerchantTerminalKeys, CPropertyPage)

CMerchantTerminalKeys::CMerchantTerminalKeys() : CPropertyPage(CMerchantTerminalKeys::IDD)
{
	//{{AFX_DATA_INIT(CMerchantTerminalKeys)
	m_StrOrganizationIDFld = _T("");
	m_StrCommunicationKeyFld = _T("");
	m_StrICVDataFld = _T("");
	m_StrKekFld = _T("");
	m_StrKeyExchangeFld = _T("");
	m_StrNSPProcessFld = _T("");
	m_StrPadCharFld = _T("");
	m_StrPinKeyCryptogramFld = _T("");
	m_StrTerminalIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrEncryptionMethod = _T("");
	m_StrPinBlockFormat = _T("");
	m_StrTPKFld = _T("");
	//}}AFX_DATA_INIT
}

CMerchantTerminalKeys::~CMerchantTerminalKeys()
{
}

void CMerchantTerminalKeys::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantTerminalKeys)
	DDX_Control(pDX, IDC_TPK_FLD, m_CntlTPKFld);
	DDX_Control(pDX, IDC_PIN_KEY_CRYPTOGRAM_FLD, m_CntlPinKeyCryptogramFld);
	DDX_Control(pDX, IDC_PIN_BLOCK_FORMAT_FLD, m_PinBlockFormat);
	DDX_Control(pDX, IDC_PAD_CHARACTER_FLD, m_CntlPadCharFld);
	DDX_Control(pDX, IDC_NSP_PROCESS_FLD, m_CntlNSPProcessFld);
	DDX_Control(pDX, IDC_KEY_EXCHANGE_FLD, m_CntlKeyExchangeFld);
	DDX_Control(pDX, IDC_KEK_CRYPTOGRAM_FLD, m_CntlKekFld);
	DDX_Control(pDX, IDC_ICV_DATA_FLD, m_CntlICVDataFld);
	DDX_Control(pDX, IDC_ENCRYPTION_METHOD_COMBO, m_EncryptionMethod);
	DDX_Control(pDX, IDC_COM_KEY_CRYPTOGRAM_FLD, m_CntlCommunicationKeyFld);
	DDX_Text(pDX, IDC_CHAIN_ID_FLD, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_COM_KEY_CRYPTOGRAM_FLD, m_StrCommunicationKeyFld);
	DDV_MaxChars(pDX, m_StrCommunicationKeyFld, 16);
	DDX_Text(pDX, IDC_ICV_DATA_FLD, m_StrICVDataFld);
	DDV_MaxChars(pDX, m_StrICVDataFld, 16);
	DDX_Text(pDX, IDC_KEK_CRYPTOGRAM_FLD, m_StrKekFld);
	DDV_MaxChars(pDX, m_StrKekFld, 16);
	DDX_Text(pDX, IDC_KEY_EXCHANGE_FLD, m_StrKeyExchangeFld);
	DDV_MaxChars(pDX, m_StrKeyExchangeFld, 3);
	DDX_Text(pDX, IDC_NSP_PROCESS_FLD, m_StrNSPProcessFld);
	DDV_MaxChars(pDX, m_StrNSPProcessFld, 15);
	DDX_Text(pDX, IDC_PAD_CHARACTER_FLD, m_StrPadCharFld);
	DDV_MaxChars(pDX, m_StrPadCharFld, 1);
	DDX_Text(pDX, IDC_PIN_KEY_CRYPTOGRAM_FLD, m_StrPinKeyCryptogramFld);
	DDV_MaxChars(pDX, m_StrPinKeyCryptogramFld, 16);
	DDX_Text(pDX, IDC_TERMINAL_ID_FLD, m_StrTerminalIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_CBString(pDX, IDC_ENCRYPTION_METHOD_COMBO, m_StrEncryptionMethod);
	DDX_CBString(pDX, IDC_PIN_BLOCK_FORMAT_FLD, m_StrPinBlockFormat);
	DDX_Text(pDX, IDC_TPK_FLD, m_StrTPKFld);
	DDV_MaxChars(pDX, m_StrTPKFld, 48);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantTerminalKeys, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantTerminalKeys)
	ON_CBN_SELCHANGE(IDC_PIN_BLOCK_FORMAT_FLD, OnSelchangePinBlockFormatFld)
	ON_EN_KILLFOCUS(IDC_TPK_FLD, OnKillfocusTpkFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalKeys message handlers

BOOL CMerchantTerminalKeys::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

    m_bKeys = TRUE;

	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_VIEW )
			EnableKeysDialog(TRUE);
		if ( m_nDialogMode != MODE_ADD )
			PopulateKeysDialogFromDatabase ( sTerminalRecordOld );
	}  
	
/*	if ( m_nDialogMode == MODE_EDIT )
	{
		CString temp = sTerminalRecordOld.pin_block_format;
		int	index = atoi (temp);
		if (index == 1) {
			m_PinBlockFormat.SetCurSel(0);
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlPadCharFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 2) {
			m_PinBlockFormat.SetCurSel(1);		
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 3) {
			m_PinBlockFormat.SetCurSel(2);
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 4) {
			m_PinBlockFormat.SetCurSel(3);
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
			m_CntlPadCharFld.EnableWindow(FALSE);
		}
		else if (index == 5) {
			m_PinBlockFormat.SetCurSel(4);
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 9) {		
			m_PinBlockFormat.SetCurSel(5);
		}
		
		temp = sTerminalRecordOld.encryption_method;
		index = atoi (temp);
		if (index == 1)
			m_EncryptionMethod.SetCurSel(0);
		else if (index == 2)
			m_EncryptionMethod.SetCurSel(1);		
	}
*/
	/*if ( m_nDialogMode == MODE_ADD )
	{
		m_EncryptionMethod.SetCurSel(0);
		m_PinBlockFormat.SetCurSel(0);
	}*/

//	m_StrPinBlockFormat = "ANSI X9.8-1982";
//	m_StrEncryptionMethod = "DES (Session Key Management)";

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMerchantTerminalKeys::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified (TRUE);
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CMerchantTerminalKeys::EnableKeysDialog ( int nStatus )
{
	m_CntlCommunicationKeyFld.EnableWindow(nStatus);
	m_CntlICVDataFld.EnableWindow(nStatus);
	m_CntlKekFld.EnableWindow(nStatus);
	m_CntlKeyExchangeFld.EnableWindow(nStatus);
	m_CntlNSPProcessFld.EnableWindow(nStatus);
	m_CntlPadCharFld.EnableWindow(nStatus);
	m_CntlPinKeyCryptogramFld.EnableWindow(nStatus);
	m_CntlTPKFld.EnableWindow(nStatus);
	m_EncryptionMethod.EnableWindow(nStatus);
	//m_PinBlockFormat.EnableWindow(nStatus);

/*	if ( m_nDialogMode == MODE_EDIT )
	{
		CString temp = sTerminalRecordOld.pin_block_format;
		int	index = atoi (temp);
		if (index == 1) {
			m_StrPinBlockFormat = "ANSI X9.8-1982";
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlPadCharFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 2) {
			m_StrPinBlockFormat = "IBM 3624";
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 3) {
			m_StrPinBlockFormat = "PIN/pad or Docutel";
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 4) {
			m_StrPinBlockFormat = "IBM PIN pad";
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
			m_CntlPadCharFld.EnableWindow(FALSE);
		}
		else if (index == 5) {
			m_StrPinBlockFormat = "Burroughs";
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 9) {		
			m_StrPinBlockFormat = "IBM 4731";
		}
*/		

		CString temp = sTerminalRecordOld.encryption_method;
		int index = atoi (temp);
		if (index == 1)
			m_StrEncryptionMethod = "DES (Session Key Management)";
		else if (index == 2)
			m_StrEncryptionMethod = "DUKPT (Derived Unique Key-Per-Tran)";
		else 
			m_StrEncryptionMethod = "NONE";
//	}
}

BOOL CMerchantTerminalKeys::PopulateKeysDialogFromDatabase ( DCF01 sTerminalRecord )
{
	CString temp;
	CMyUtils myUtilsObject;	

	if ( m_nDialogMode != MODE_VIEW && m_bKeys )	
		EnableKeysDialog(TRUE);

//	ZeroMemory ( &sTerminalRecord, sizeof ( DCF01) );

	m_StrOrganizationIDFld = sTerminalRecord.organization_id;
	m_StrCommunicationKeyFld = sTerminalRecord.communication_key;
	m_StrICVDataFld = sTerminalRecord.icv;
	m_StrKekFld = sTerminalRecord.kek;
	m_StrKeyExchangeFld = sTerminalRecord.keyxchange_cnt;
	m_StrNSPProcessFld = sTerminalRecord.nsp_process;
	m_StrPadCharFld = sTerminalRecord.pad_char;
	m_StrPinKeyCryptogramFld = sTerminalRecord.pin_key;
	m_StrTerminalIDFld = sTerminalRecord.primary_key.device_id;
	m_StrMerchantIDFld = sTerminalRecord.merchant_id;
	m_StrTPKFld = sTerminalRecord.cwk;
	
	strPinFormat = sTerminalRecord.pin_block_format;
	strEncryptionMethod = sTerminalRecord.encryption_method;

	if ( strPinFormat == "1" )
		m_StrPinBlockFormat = "ANSI X9.8-1982";
	else if ( strPinFormat == "2" )
		m_StrPinBlockFormat = "Pin/Pad or Docutel";
	else if ( strPinFormat == "3" )
		m_StrPinBlockFormat = "IBM ATM and Diebold";
	else if ( strPinFormat == "4" )
		m_StrPinBlockFormat = "PLUS";
	else if ( strPinFormat == "5" )
		m_StrPinBlockFormat = "ISO 9564-1";


/*	int	index = atoi (strPinFormat);
		if (index == 1) {
			m_StrPinBlockFormat = "ANSI X9.8-1982";
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlPadCharFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 2) {
			m_StrPinBlockFormat = "Pin/Pad or Docutel";
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 3) {
			m_StrPinBlockFormat = "PIN/pad or Docutel";
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 4) {
			m_StrPinBlockFormat = "IBM ATM and Diebold";
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
			m_CntlPadCharFld.EnableWindow(FALSE);
		}
		else if (index == 5) {
			m_StrPinBlockFormat = "PLUS";
			m_CntlCommunicationKeyFld.EnableWindow(FALSE);
			m_CntlICVDataFld.EnableWindow(FALSE);
		}
		else if (index == 6) {		
			m_StrPinBlockFormat = "IBM 4731";
		}
		*/
		int index = atoi (strEncryptionMethod);
		if (index == 1)
			m_StrEncryptionMethod = "DES (Session Key Management)";
		else if (index == 2)
			m_StrEncryptionMethod = "DUKPT (Derived Unique Key-Per-Tran)";
		else m_StrEncryptionMethod = "None";

	
	UpdateData(FALSE);

	memcpy ( &sTerminalRecordOld, &sTerminalRecord, sizeof ( DCF01));

	return TRUE;

}

void CMerchantTerminalKeys::CleanKeysDialog()
{
	m_StrOrganizationIDFld = _T("");
	m_StrCommunicationKeyFld = _T("");
	m_StrICVDataFld = _T("");
	m_StrKekFld = _T("");
	m_StrKeyExchangeFld = _T("");
	m_StrNSPProcessFld = _T("");
	m_StrPadCharFld = _T("");
	m_StrPinKeyCryptogramFld = _T("");
	m_StrTerminalIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrTPKFld = _T("");
	m_StrPinBlockFormat = "";


	UpdateData (FALSE);
}

BOOL  CMerchantTerminalKeys::PopulateDatabaseFromKeysDialog ( pDCF01 psTerminalRecord )
{

	
	memcpy ( psTerminalRecord->communication_key,	m_StrCommunicationKeyFld,	sizeof ( psTerminalRecord->communication_key));
	memcpy ( psTerminalRecord->icv,					m_StrICVDataFld,			sizeof ( psTerminalRecord->icv));
	memcpy ( psTerminalRecord->kek,					m_StrKekFld,				sizeof ( psTerminalRecord->kek));
	memcpy ( psTerminalRecord->keyxchange_cnt,		m_StrKeyExchangeFld,		sizeof ( psTerminalRecord->keyxchange_cnt));
	memcpy ( psTerminalRecord->nsp_process,			m_StrNSPProcessFld,			sizeof ( psTerminalRecord->nsp_process));
	memcpy ( psTerminalRecord->pad_char,			m_StrPadCharFld,			sizeof ( psTerminalRecord->pad_char));
	memcpy ( psTerminalRecord->pin_key,				m_StrPinKeyCryptogramFld,	sizeof ( psTerminalRecord->pin_key));	
	memcpy ( psTerminalRecord->cwk,				    m_StrTPKFld,	            sizeof ( psTerminalRecord->cwk));	

	if ( m_bKeys )
	{
	int index = m_PinBlockFormat.GetCurSel();

		switch (index)
		{
		case 0:
		 strPinFormat = "1";
		break;
		
		case 1:
		 strPinFormat = "2";
		break;

		case 2:
		 strPinFormat = "3";
		break;
		
		case 3:
		 strPinFormat = "4";
		break;

		case 4:
		 strPinFormat = "5";
		break;

		default:
		 strPinFormat = "";

		break;
		
		} //switch

		index = m_EncryptionMethod.GetCurSel();

		if (index == 0)
			strEncryptionMethod = "1";
		else if (index == 1)
			strEncryptionMethod = "2";
		else strEncryptionMethod = "";
	}

	if ( !m_bKeys && m_nDialogMode == MODE_ADD )
	{
		strPinFormat = "1";
		strEncryptionMethod = "1";
	}

		memcpy ( psTerminalRecord->pin_block_format,	strPinFormat,	sizeof ( psTerminalRecord->pin_block_format));	
		memcpy ( psTerminalRecord->encryption_method,	strEncryptionMethod,	sizeof ( psTerminalRecord->encryption_method));	

	
	return TRUE;
}

BOOL CMerchantTerminalKeys::CompareStructures()
{
	CString temp;

/*	if (strcmp ( strPinFormat, (char *)sTerminalRecordOld.pin_block_format) != 0)
	{
		if ( strPinFormat == "1" )
			temp = "ANSI X9.8-1982";
		else if ( strPinFormat == "2" )
			temp = "Pin/Pad or Docutel";
		else if ( strPinFormat == "3" )
			temp = "IBM ATM and Diebold";
		else if ( strPinFormat == "4" )
			temp = "PLUS";
		else if ( strPinFormat == "5" )
			temp = "ISO 9564-1";
		memcpy(m_strCurrentData, temp, sizeof (m_strCurrentData));

        temp = sTerminalRecordOld.pin_block_format;
		if ( temp == "1" )
			temp = "ANSI X9.8-1982";
		else if ( temp == "2" )
			temp = "Pin/Pad or Docutel";
		else if ( temp == "3" )
			temp = "IBM ATM and Diebold";
		else if ( temp == "4" )
			temp = "PLUS";
		else if ( temp == "5" )
			temp = "ISO 9564-1";
		memcpy(m_strPreviousData, temp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Pin Block Format", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}*/
	if (strcmp ( strEncryptionMethod, (char *)sTerminalRecordOld.encryption_method) != 0)
	{
		memcpy(m_strCurrentData, strEncryptionMethod, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.encryption_method, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Encryption Method", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrPinKeyCryptogramFld, (char *)sTerminalRecordOld.pin_key) != 0)
	{
		memcpy(m_strCurrentData, m_StrPinKeyCryptogramFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.pin_key, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Pin Key Crypto", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrTPKFld, (char *)sTerminalRecordOld.cwk) != 0)
	{
		memcpy(m_strCurrentData, m_StrTPKFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.cwk, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "TPK", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	if (strcmp ( m_StrPadCharFld, (char *)sTerminalRecordOld.pad_char) != 0)
	{
		memcpy(m_strCurrentData, m_StrPadCharFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.pad_char, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Pad Char", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrNSPProcessFld, (char *)sTerminalRecordOld.nsp_process) != 0)
	{
		memcpy(m_strCurrentData, m_StrNSPProcessFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.nsp_process, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "NSP Process", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrKeyExchangeFld, (char *)sTerminalRecordOld.keyxchange_cnt) != 0)
	{
		memcpy(m_strCurrentData, m_StrKeyExchangeFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.keyxchange_cnt, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Key Exchange", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	
	if (strcmp ( m_StrKekFld, (char *)sTerminalRecordOld.kek) != 0)
	{
		memcpy(m_strCurrentData, m_StrKekFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.kek, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Kek", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrICVDataFld, (char *)sTerminalRecordOld.icv) != 0)
	{
		memcpy(m_strCurrentData, m_StrICVDataFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.icv, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "ICV Data", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrCommunicationKeyFld, (char *)sTerminalRecordOld.communication_key) != 0)
	{
		memcpy(m_strCurrentData, m_StrCommunicationKeyFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sTerminalRecordOld.communication_key, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Communication Key", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	
	return TRUE;
}

void CMerchantTerminalKeys::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";

	char strPrimaryKey[50];

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy ( strPrimaryKey, m_StrTerminalIDFld);
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR)sHistoryData.file_name,				(LPCTSTR)"DCF01",			sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR)sHistoryData.file_primary_key,		(LPCTSTR)strPrimaryKey,		sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR)sHistoryData.primary_key.unique_id,	(LPCTSTR)dest_str,			sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR)sHistoryData.time_modified,			(LPCTSTR)strTime,			sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data,			(LPCTSTR)m_strCurrentData,	sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data,			(LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.field_tag,				(LPCTSTR)m_strFieldTag,		sizeof (sHistoryData.field_tag));
	memcpy ((LPTSTR)sHistoryData.changed_by,			(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
}

void CMerchantTerminalKeys::OnSelchangePinBlockFormatFld() 
{
/*	int index;

   index = m_PinBlockFormat.GetCurSel();

   UpdateData(TRUE);

   switch ( index ) {
   case 0: 	   		
		m_StrPadCharFld = "";	
		m_StrCommunicationKeyFld = "";		
		m_StrICVDataFld = "";	

		UpdateData(FALSE);

		m_CntlCommunicationKeyFld.EnableWindow(FALSE);
		m_CntlPadCharFld.EnableWindow(FALSE);
		m_CntlICVDataFld.EnableWindow(FALSE);
	break;

    case 1:						
		m_StrICVDataFld = "";	

		UpdateData(FALSE);

		m_CntlCommunicationKeyFld.EnableWindow(TRUE);
		m_CntlPadCharFld.EnableWindow(TRUE);
		m_CntlICVDataFld.EnableWindow(FALSE);
	break;

	case 2:	
		m_StrCommunicationKeyFld = "";		
		m_StrICVDataFld = "";	

		UpdateData(FALSE);

		m_CntlCommunicationKeyFld.EnableWindow(FALSE);
		m_CntlPadCharFld.EnableWindow(TRUE);
		m_CntlICVDataFld.EnableWindow(FALSE);
	break;	 

	 case 3:
		m_StrPadCharFld = "";	
		m_StrCommunicationKeyFld = "";		
		m_StrICVDataFld = "";	

		UpdateData(FALSE);

		m_CntlCommunicationKeyFld.EnableWindow(FALSE);
		m_CntlPadCharFld.EnableWindow(FALSE);
		m_CntlICVDataFld.EnableWindow(FALSE);
	break;

	 case 4:		 	
		m_StrCommunicationKeyFld = "";		
		m_StrICVDataFld = "";	

		UpdateData(FALSE);

		m_CntlCommunicationKeyFld.EnableWindow(FALSE);
		m_CntlPadCharFld.EnableWindow(TRUE);
		m_CntlICVDataFld.EnableWindow(FALSE);
	break;

	 case 5:
		m_CntlCommunicationKeyFld.EnableWindow(TRUE);
		m_CntlPadCharFld.EnableWindow(TRUE);
		m_CntlICVDataFld.EnableWindow(TRUE);
	break;


   }
*/	
}

/*02/18/03 LM SCR955 - Added function to validate TPK for length and Hex entry only */
void CMerchantTerminalKeys::OnKillfocusTpkFld() 
{
	INT			key_length = 0;
	CMyUtils    myUtilsObject;

    if (!UpdateData(TRUE))
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	
	key_length = m_StrTPKFld.GetLength();

	if(key_length == 0)	/*TPK Field can be left blank*/
	return;

	/*Check TPK keys are hex characters only (0-9, A-F) - return to input if they are not else continue*/
	if(!myUtilsObject.IsHexEntry(m_StrTPKFld))
	{
		AfxMessageBox( "TPK: Invalid Hex Character Entered. Value Characters are 0 - 9 and A - F.", 
												MB_ICONSTOP | MB_OK );
	 	m_CntlTPKFld.SetFocus();
		return;
	}

	/*check num chars enter entered = 16, 32, or 48 and post info message if not*/
	/*user can continue to apply changes with invalid length */
	
	if( (key_length != 16) && 
	    (key_length != 32) &&
	    (key_length != 48) )
	{
		AfxMessageBox( "TPK: The number of characters entered should be 16, 32, or 48.", MB_ICONINFORMATION | MB_OK );
  
	}
} /*end void CMerchantTerminalKeys::OnKillfocusTpkFld() */
