// RulesexceptionList.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "RulesExceptionList.h"
#include "MyUtils.h"
#include "SharedMem.h"

extern "C" {
#include "PTETIME.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CRulesDefinnitionExceptionList property page

IMPLEMENT_DYNCREATE(CRulesDefinitionExceptionList, CPropertyPage)

CRulesDefinitionExceptionList::CRulesDefinitionExceptionList() : CPropertyPage(CRulesDefinitionExceptionList::IDD)
{
	//{{AFX_DATA_INIT(CBinBlocking)
	m_StrRuleID_Fld = _T("");
	m_StrExceptionCardNumFld = _T("");
	m_StrMessageLbl = _T("");

	//}}AFX_DATA_INIT
}

CRulesDefinitionExceptionList::~CRulesDefinitionExceptionList()
{
}

void CRulesDefinitionExceptionList::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBinBlocking)
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_EXCEPTION_CARD_NUM_ID, m_CntlExceptionCardNumFld);
	DDX_Control(pDX, IDC_ADD, m_Add);
	DDX_Control(pDX, IDC_DELETE, m_Delete);
	DDX_Text(pDX, IDC_RULE_ID_FLD, m_StrRuleID_Fld);
	DDX_Text(pDX, IDC_EXCEPTION_CARD_NUM_ID, m_StrExceptionCardNumFld);
	DDV_MaxChars(pDX, m_StrExceptionCardNumFld, 20);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRulesDefinitionExceptionList, CPropertyPage)
	//{{AFX_MSG_MAP(CBinBlocking)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBinBlocking message handlers

BOOL CRulesDefinitionExceptionList::OnInitDialog() 
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

    m_bExceptionCardNum = TRUE;
	m_CntlExceptionCardNumFld.SetFocus();

    if ( m_nDialogMode == MODE_VIEW || m_nDialogMode == MODE_EDIT )
       PopulateCardBlockingDialogFromDatabase ( sCR01Record );

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRulesDefinitionExceptionList::CompareStructures( CR01 sCR01Old, CR01 sCR01New )
{
    HST01 sHistoryData;
    CHAR dest_str[18];
    char strDate[9] = "";
    char strTime[7] = "";
    CMyUtils myUtilsObject;
	 int nNewCount;
	 BOOL bFound = FALSE;
	 int nIndex;
	 char temp[10] = "";
	 int i,j;
	 char strErrorMsg[200] = "";
    CString temp1;
    CString m_strCurrentData, m_strFieldTag;
    char strPrimaryKey[100] = "";

	 if (!m_bExceptionCardNum)
		 return;

	 strcpy ( strPrimaryKey, m_StrRuleID_Fld);
	 nNewCount = m_List.GetCount();

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    
    myUtilsObject.GetSystemDate(strDate);

    
	 for (  i = 0; i < nOldCount; i++ )
    { 
		bFound = FALSE;
		for (  j = 0; j < nNewCount; j++ )
		{	
			nIndex = i;
			int nResult = strcmp ((char *)sCR01Old.cardnum_blocking[i].exception_cardnum,(char *)sCR01New.cardnum_blocking[j].exception_cardnum);
			if ( nResult == 0 )
			{
				bFound = TRUE;
				
			}
		}
	   if ( !bFound )
	   {
		   m_strCurrentData = "Record Deleted";
		   m_strFieldTag = "Exception Card Number";		
		   myUtilsObject.GetSystemTime(strTime);
		   pintime_get_gmt ( dest_str );
		   ZeroMemory ( &sHistoryData, sizeof (HST01));
		   memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CR01", sizeof (sHistoryData.file_name));
         temp1 = strDate;
         temp1 = temp1.Right(4) + temp1.Left(2) + temp1.Mid(2,2);
    
         CHAR wstation[21]; DWORD wsize;

         wsize = sizeof( wstation );
         GetComputerName( wstation, &wsize );

	      memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
         memcpy ((LPTSTR) sHistoryData.date_modified, temp1, sizeof (sHistoryData.date_modified));
		   memcpy ((LPTSTR) sHistoryData.file_primary_key,  strPrimaryKey, sizeof (sHistoryData.file_primary_key));
		   memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		   memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
		   memcpy ((LPTSTR) sHistoryData.field_tag,     (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));
		   memcpy ((LPTSTR) sHistoryData.previous_data,     (LPCTSTR)sCR01Old.cardnum_blocking[i].exception_cardnum, sizeof (sHistoryData.previous_data));	
		   memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
		   memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
       int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		   if ( rcode != PTEMSG_OK )
		   {
			   m_StrMessageLbl = strErrorMsg;
			   UpdateData( FALSE );
		   }
	   }
	}

	bFound = FALSE;
    
	for (  i = 0; i < nNewCount; i++ )
    { 
		bFound = FALSE;
		for ( int j = 0; j < nOldCount && !bFound; j++ )
		{	
			nIndex = i;
			int nResult = strcmp ((char *)sCR01Old.cardnum_blocking[j].exception_cardnum,(char *)sCR01New.cardnum_blocking[i].exception_cardnum);
			if ( nResult == 0 )
			{
				bFound = TRUE;
			
			}
		}
	
	   if ( !bFound )
	   {
		   m_strCurrentData = "Record Added";
		   m_strFieldTag = "Exception Card Number";
	 
         
		   myUtilsObject.GetSystemTime(strTime);
		   pintime_get_gmt ( dest_str );
		   ZeroMemory ( &sHistoryData, sizeof (HST01));
		   memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CR01", sizeof (sHistoryData.file_name));
         temp1 = strDate;
         temp1 = temp1.Right(4) + temp1.Left(2) + temp1.Mid(2,2);
    
         CHAR wstation[21]; DWORD wsize;

         wsize = sizeof( wstation );
         GetComputerName( wstation, &wsize );

	      memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
         memcpy ((LPTSTR) sHistoryData.date_modified, temp1, sizeof (sHistoryData.date_modified));
		   memcpy ((LPTSTR) sHistoryData.file_primary_key,  strPrimaryKey, sizeof (sHistoryData.file_primary_key));
		   memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
		   memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
		   memcpy ((LPTSTR) sHistoryData.field_tag,     (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));
		   memcpy ((LPTSTR) sHistoryData.previous_data,     (LPCTSTR)sCR01New.cardnum_blocking[i].exception_cardnum, sizeof (sHistoryData.previous_data));	
		   memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
		   memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
         int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		      if ( rcode != PTEMSG_OK )
		      {
			      m_StrMessageLbl = strErrorMsg;
			      UpdateData( FALSE );
		      }
	   }
	}
}

void CRulesDefinitionExceptionList::OnAdd() 
{
   CMyUtils myUtils;

   UpdateData(TRUE);

   if ( myUtils.IsEmpty ( m_StrExceptionCardNumFld, this ) )
   {
      m_StrMessageLbl = "Please enter Card number";
      UpdateData(FALSE);
      return;
   }

   if ( m_List.GetCount () == 50 )
   {
      m_StrMessageLbl = "Maximum number of Card numbers is 50";
      UpdateData(FALSE);
      return;
   }
	
   if (m_List.FindStringExact(-1, m_StrExceptionCardNumFld) == LB_ERR)
   {
      m_List.AddString ( m_StrExceptionCardNumFld );
      m_StrMessageLbl = "";
		m_StrExceptionCardNumFld = "";
		
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
   }
   else
   {
		m_StrMessageLbl = "ERROR: Card Num already exists";	
		m_CntlExceptionCardNumFld.SetFocus();
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
   }


   if ( m_List.GetCount() == 50 )
      m_Add.EnableWindow(FALSE);


	m_CntlExceptionCardNumFld.SetFocus();

	
}

void CRulesDefinitionExceptionList::CleanCardBlockingDialog() 
{
	m_StrRuleID_Fld = _T("");
	m_StrExceptionCardNumFld = _T("");
	m_StrMessageLbl = _T("");
  
	m_List.ResetContent();
}

void CRulesDefinitionExceptionList::PopulateCardBlockingDialogFromDatabase( CR01 sCR01Record ) 
{
	CString temp;

	m_List.ResetContent();
	m_StrExceptionCardNumFld = "";

    for ( int i = 0; i < 50; i++)
    {
        temp = sCR01Record.cardnum_blocking[i].exception_cardnum;
        int nResult = temp.IsEmpty();
        if (nResult == 0)
        m_List.AddString((char *)sCR01Record.cardnum_blocking[i].exception_cardnum);
		
    }
	nOldCount = m_List.GetCount();

   if ( m_List.GetCount() == 50 )
      m_Add.EnableWindow(FALSE);

	SetModified ( FALSE );		
}

void CRulesDefinitionExceptionList::PopulateDatabaseStructurefromCardBlockingDialog( pCR01 sCR01Record )
{
   if ( !m_bExceptionCardNum )
      return;

    char temp [50] = "";

	for ( int idx = 0; idx < 50; idx++)
		memset ( sCR01Record->cardnum_blocking[idx].exception_cardnum , 0, 20);

	int nCount = m_List.GetCount();
	
	for ( int i = 0; i < nCount; i++)
	{
		m_List.GetText ( i, temp);
		memcpy ( sCR01Record->cardnum_blocking[i].exception_cardnum , (LPTSTR)temp, 20);
	}
	
}

void CRulesDefinitionExceptionList::OnDelete() 
{
	int nIndex;    

	m_StrMessageLbl = "";		
	if (!UpdateData(FALSE))
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);

    nIndex = m_List.GetCurSel();
    if (nIndex > -1)
    {
        m_List.DeleteString (nIndex);
        SetModified(TRUE);

    }	
	else
	{
		m_StrMessageLbl = "Please select Card Number to delete";		
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
	}	
}

BOOL CRulesDefinitionExceptionList::OnSetActive() 
{
   if	( m_nDialogMode == MODE_DONE || m_nDialogMode == MODE_VIEW )
   {
      m_CntlExceptionCardNumFld.EnableWindow(FALSE);
      m_Add.EnableWindow(FALSE);
      m_Delete.EnableWindow(FALSE);
      m_List.EnableWindow(FALSE);
   }
   else if ( m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT)
   {
      m_CntlExceptionCardNumFld.EnableWindow(TRUE);
      m_Add.EnableWindow(TRUE);
      m_Delete.EnableWindow(TRUE);
      m_List.EnableWindow(TRUE);
   }

	return CPropertyPage::OnSetActive();
}

BOOL CRulesDefinitionExceptionList::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified (TRUE);	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}