// FraudGuardGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "FraudGuardGeneral.h"
#include "SharedMem.h"
extern "C"
{
#include "PteTime.h"
}

extern UCF01 ucf01;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CFraudGuardGeneral property page

IMPLEMENT_DYNCREATE(CFraudGuardGeneral, CPropertyPage)

CFraudGuardGeneral::CFraudGuardGeneral() : CPropertyPage(CFraudGuardGeneral::IDD)
{
	//{{AFX_DATA_INIT(CFraudGuardGeneral)
	m_StrCallerIdFld = _T("");
	m_StrEchoTimerFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrNameId = _T("");
	m_StrPriorityFld = _T("");
	m_StrProductFld = _T("");
	m_StrQueNameFld = _T("");
	m_StrRequestTimerFld = _T("");
	m_StrRuleDomainFld = _T("");
	m_StrSourceFld = _T("");
	m_StrTpduFld = _T("");
	m_StrTypeFld = _T("");
	m_StrUserNameFld = _T("");
	m_StrApplicationCombo = "-1";
	//m_StrApplicationCombo = -1;
	//}}AFX_DATA_INIT
}

CFraudGuardGeneral::~CFraudGuardGeneral()
{
}

void CFraudGuardGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFraudGuardGeneral)
	DDX_Control(pDX, IDC_APPLICATIN_COMBO, m_CntlApplicationCombo);
	DDX_Control(pDX, IDC_USER_NAME_FLD, m_CntlUserNameFld);
	DDX_Control(pDX, IDC_TYPE_FLD, m_CntlTypeFld);
	DDX_Control(pDX, IDC_TPDU_FLD, m_CntlTpduFld);
	DDX_Control(pDX, IDC_SOURCE_FLD, m_CntlSourceFld);
	DDX_Control(pDX, IDC_RULE_DOMAIN_FLD, m_CntlRuleDomainFld);
	DDX_Control(pDX, IDC_REQUEST_TIMER_FLD, m_CntlRequestTimerFld);
	DDX_Control(pDX, IDC_QUE_NAME_FLD, m_CntlQueNameFld);
	DDX_Control(pDX, IDC_PRODUCT_FLD, m_CntlProductFld);
	DDX_Control(pDX, IDC_PRIORITY__FLD, m_CntlPriorityFld);
	DDX_Control(pDX, IDC_NAME_ID_FLD, m_CntlNameId);
	DDX_Control(pDX, IDC_MESSAGE_LBL, m_CntlMessageLbl);
	DDX_Control(pDX, IDC_ECHO_TIMER_FLD, m_CntlEchoTimerFld);
	DDX_Control(pDX, IDC_CALLER_ID_FLD, m_CntlCallerIdFld);
	DDX_Text(pDX, IDC_CALLER_ID_FLD, m_StrCallerIdFld);
	DDV_MaxChars(pDX, m_StrCallerIdFld, 5);
	DDX_Text(pDX, IDC_ECHO_TIMER_FLD, m_StrEchoTimerFld);
	DDV_MaxChars(pDX, m_StrEchoTimerFld, 3);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_NAME_ID_FLD, m_StrNameId);
	DDV_MaxChars(pDX, m_StrNameId, 10);
	DDX_Text(pDX, IDC_PRIORITY__FLD, m_StrPriorityFld);
	DDV_MaxChars(pDX, m_StrPriorityFld, 4);
	DDX_Text(pDX, IDC_PRODUCT_FLD, m_StrProductFld);
	DDV_MaxChars(pDX, m_StrProductFld, 30);
	DDX_Text(pDX, IDC_QUE_NAME_FLD, m_StrQueNameFld);
	DDV_MaxChars(pDX, m_StrQueNameFld, 15);
	DDX_Text(pDX, IDC_REQUEST_TIMER_FLD, m_StrRequestTimerFld);
	DDV_MaxChars(pDX, m_StrRequestTimerFld, 3);
	DDX_Text(pDX, IDC_RULE_DOMAIN_FLD, m_StrRuleDomainFld);
	DDV_MaxChars(pDX, m_StrRuleDomainFld, 80);
	DDX_Text(pDX, IDC_SOURCE_FLD, m_StrSourceFld);
	DDV_MaxChars(pDX, m_StrSourceFld, 30);
	DDX_Text(pDX, IDC_TPDU_FLD, m_StrTpduFld);
	DDV_MaxChars(pDX, m_StrTpduFld, 11);
	DDX_Text(pDX, IDC_TYPE_FLD, m_StrTypeFld);
	DDX_Text(pDX, IDC_USER_NAME_FLD, m_StrUserNameFld);
	DDV_MaxChars(pDX, m_StrUserNameFld, 30);
	DDX_CBString(pDX, IDC_APPLICATIN_COMBO, m_StrApplicationCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFraudGuardGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CFraudGuardGeneral)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFraudGuardGeneral message handlers


/******************************************************************************************/

void CFraudGuardGeneral::CleanGeneralDialog()
{
	m_StrApplicationCombo = _T("");
	m_StrCallerIdFld = _T("");
	m_StrEchoTimerFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrPriorityFld = _T("");
	m_StrProductFld = _T("");
	m_StrQueNameFld = _T("");
	m_StrRequestTimerFld = _T("");
	m_StrRuleDomainFld = _T("");
	m_StrSourceFld = _T("");
	m_StrTpduFld = _T("");
	m_StrUserNameFld = _T("");
 
	UpdateData(FALSE);

}

/******************************************************************************************/
/******************************************************************************************/

void CFraudGuardGeneral::EnableGeneralDialog(int nStatus)
{
	if ( m_nDialogMode != MODE_VIEW )
    {	
  	    m_CntlApplicationCombo.EnableWindow(nStatus);
		m_CntlCallerIdFld.EnableWindow(nStatus);
		m_CntlEchoTimerFld.EnableWindow(nStatus);
		m_CntlPriorityFld.EnableWindow(nStatus);
		m_CntlProductFld.EnableWindow(nStatus);
		m_CntlQueNameFld.EnableWindow(nStatus);
		m_CntlRequestTimerFld.EnableWindow(nStatus);
		m_CntlRuleDomainFld.EnableWindow(nStatus);
		m_CntlSourceFld.EnableWindow(nStatus);
		m_CntlTpduFld.EnableWindow(nStatus);
		m_CntlUserNameFld.EnableWindow(nStatus);
	}
}

/******************************************************************************************/
/******************************************************************************************/

BOOL CFraudGuardGeneral::PopulateDatabaseFromFGuardDialog( pFG01 psFGuard )
{
   memcpy ( psFGuard->name,          m_StrNameId,           sizeof ( psFGuard->name ));
   memcpy ( psFGuard->type,          m_StrTypeFld,				1 );
   memcpy ( psFGuard->application,   m_StrApplicationCombo, sizeof ( psFGuard->application));
   memcpy ( psFGuard->caller_id,     m_StrCallerIdFld,      sizeof ( psFGuard->caller_id));
   memcpy ( psFGuard->echo_timer,    m_StrEchoTimerFld,     sizeof ( psFGuard->echo_timer));
   memcpy ( psFGuard->priority,      m_StrPriorityFld,      sizeof ( psFGuard->priority));
   memcpy ( psFGuard->product,       m_StrProductFld,       sizeof ( psFGuard->product));
   memcpy ( psFGuard->queue_name,    m_StrQueNameFld,       sizeof ( psFGuard->queue_name));
   memcpy ( psFGuard->request_timer, m_StrRequestTimerFld,  sizeof ( psFGuard->request_timer));
   memcpy ( psFGuard->rule_domain,   m_StrRuleDomainFld,    sizeof ( psFGuard->rule_domain));
   memcpy ( psFGuard->source,        m_StrSourceFld,        sizeof ( psFGuard->source));
   memcpy ( psFGuard->tpdu_id,       m_StrTpduFld,          sizeof ( psFGuard->tpdu_id));
   memcpy ( psFGuard->username,      m_StrUserNameFld,      sizeof ( psFGuard->username));

   return TRUE;
}

/******************************************************************************************/
/******************************************************************************************/

BOOL CFraudGuardGeneral::PopulateFraudGuardDialogFromDatabase( FG01 sFGuard )
{
   ZeroMemory ( &sFGuardOld, sizeof (FG01));  

   m_StrNameId     = sFGuard.name;

   if ( sFGuard.type[0] == 'A' )
      m_StrTypeFld = "ACQUIRER";
   else if ( sFGuard.type[0] == 'I' )
      m_StrTypeFld = "ISSUER";

   m_StrApplicationCombo = sFGuard.application;
   m_StrCallerIdFld      = sFGuard.caller_id;
   m_StrEchoTimerFld     = sFGuard.echo_timer;
   m_StrPriorityFld      = sFGuard.priority;
   m_StrProductFld       = sFGuard.product;
   m_StrQueNameFld       = sFGuard.queue_name;
   m_StrRequestTimerFld  = sFGuard.request_timer;
   m_StrRuleDomainFld    = sFGuard.rule_domain;
   m_StrSourceFld        = sFGuard.source;
   m_StrTpduFld          = sFGuard.tpdu_id;
   m_StrUserNameFld      = sFGuard.username;

   memcpy ( &sFGuardOld, &sFGuard, sizeof ( FG01));
   return TRUE;
}
/******************************************************************************************/

BOOL CFraudGuardGeneral::OnInitDialog() 
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

	m_bGeneral = TRUE;


	if (m_nDialogMode != MODE_DONE )
    {		
        EnableGeneralDialog(TRUE);	
		
        if ( m_nDialogMode != MODE_ADD )
        PopulateFraudGuardDialogFromDatabase (sFGuardOld);
    }	

	if (!UpdateData(FALSE))
    {
        AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
    }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************************/
							
BOOL CFraudGuardGeneral::CompareStructures()
{	

    if (strcmp ( m_StrCallerIdFld, (char *)sFGuardOld.caller_id) != 0)
    {
        memcpy(m_StrCurrentData, m_StrCallerIdFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFGuardOld.caller_id, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Caller ID", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
    }
    if (strcmp ( m_StrSourceFld, (char *)sFGuardOld.source) != 0)
    {
        memcpy(m_StrCurrentData, m_StrSourceFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFGuardOld.source, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Source", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
    }
    if (strcmp ( m_StrUserNameFld, (char *)sFGuardOld.username) != 0)
    {
        memcpy(m_StrCurrentData, m_StrUserNameFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFGuardOld.username, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "User Name", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
    }
    if (strcmp ( m_StrApplicationCombo, (char *)sFGuardOld.application) != 0)
    {
        memcpy(m_StrCurrentData, m_StrApplicationCombo, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFGuardOld.application, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Application", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
    }
    if (strcmp ( m_StrRuleDomainFld, (char *)sFGuardOld.rule_domain) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrRuleDomainFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFGuardOld.rule_domain, sizeof (m_StrPreviousData)-1);
        memcpy(m_StrFieldTag, "Rule Domain", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
	}     
    if (strcmp ( m_StrProductFld, (char *)sFGuardOld.product) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrProductFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFGuardOld.product, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Product", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
    }  
    if (strcmp ( m_StrPriorityFld, (char *)sFGuardOld.priority) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrPriorityFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFGuardOld.priority, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Priority", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
    }
	if (strcmp ( m_StrQueNameFld, (char *)sFGuardOld.queue_name) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrQueNameFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFGuardOld.queue_name, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Queue Name", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
    }
	if (strcmp ( m_StrTpduFld, (char *)sFGuardOld.tpdu_id) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrTpduFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFGuardOld.tpdu_id, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "TPDU ID", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
    }
	if (strcmp ( m_StrRequestTimerFld, (char *)sFGuardOld.request_timer) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrRequestTimerFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFGuardOld.request_timer, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Request Timer", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
    }
	if (strcmp ( m_StrEchoTimerFld, (char *)sFGuardOld.echo_timer) != 0)
    {  
        memcpy(m_StrCurrentData, m_StrEchoTimerFld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sFGuardOld.echo_timer, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag, "Echo Timer", sizeof(m_StrFieldTag));
        PopulateHistoryTable();
    }

        return TRUE;
}

/******************************************************************************************/
/******************************************************************************************/

void CFraudGuardGeneral::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
    CString temp;

    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";
    CHAR wstation[21]; DWORD wsize;

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

    strcpy ( strPrimaryKey, m_StrNameId);
    strncat( strPrimaryKey, m_StrTypeFld, 1);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,         (LPCTSTR)"FG01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR) sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey,         sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str,      sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime,               sizeof (sHistoryData.time_modified)); 
	memcpy ((LPTSTR) sHistoryData.field_tag,         (LPCTSTR)m_StrFieldTag,         sizeof (sHistoryData.field_tag));
	memcpy ((LPTSTR) sHistoryData.previous_data,     (LPCTSTR)m_StrPreviousData,     sizeof (sHistoryData.previous_data));
    memcpy ((LPTSTR) sHistoryData.current_data,      (LPCTSTR)m_StrCurrentData,      sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR) sHistoryData.changed_by,        (LPCTSTR)ucf01.primary_key.name,sizeof (sHistoryData.changed_by));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
    
}

BOOL CFraudGuardGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
