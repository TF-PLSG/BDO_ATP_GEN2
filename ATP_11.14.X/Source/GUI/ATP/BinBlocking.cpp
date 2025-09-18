// BinBlocking.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "BinBlocking.h"
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
extern MIDBINBLK01	sMidBinBlkRecord_Global ;
extern MIDBINBLK01 sOldMidBinBlkRecord_Global;

/////////////////////////////////////////////////////////////////////////////
// CBinBlocking property page

IMPLEMENT_DYNCREATE(CBinBlocking, CPropertyPage)

CBinBlocking::CBinBlocking() : CPropertyPage(CBinBlocking::IDD)
{
	//{{AFX_DATA_INIT(CBinBlocking)
	m_StrBinLengthFld = _T("");
	m_StrBinLowFld = _T("");
	m_StrAcquirerIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrBinTypeFld = _T("");
	//}}AFX_DATA_INIT
}

CBinBlocking::~CBinBlocking()
{
}

void CBinBlocking::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBinBlocking)
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_ACQUIRER_ID, m_CntlAcquirerIDFld);
	DDX_Control(pDX, IDC_ADD, m_Add);
	DDX_Control(pDX, IDC_DELETE, m_Delete);
	DDX_Text(pDX, IDC_BIN_LENGTH_FLD, m_StrBinLengthFld);
	DDX_Text(pDX, IDC_BIN_LOW_FLD, m_StrBinLowFld);
	DDX_Text(pDX, IDC_ACQUIRER_ID, m_StrAcquirerIDFld);
	DDV_MaxChars(pDX, m_StrAcquirerIDFld, 11);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_TYPE_FLD, m_StrBinTypeFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBinBlocking, CPropertyPage)
	//{{AFX_MSG_MAP(CBinBlocking)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBinBlocking message handlers

BOOL CBinBlocking::OnInitDialog() 
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

    m_bBlocking = TRUE;
	m_CntlAcquirerIDFld.SetFocus();

    if ( m_nDialogMode == MODE_VIEW || m_nDialogMode == MODE_EDIT )
       PopulateBinDialogFromDatabase ( sBinRecord );

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBinBlocking::CompareStructures( BIN01 sBinOld, BIN01 sBinNew )
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

	 if (!m_bBlocking)
		 return;

	 strcpy ( strPrimaryKey, m_StrBinLengthFld);
	 strcat ( strPrimaryKey, m_StrBinLowFld );
	 strncat ( strPrimaryKey, m_StrBinTypeFld,1 );	
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
			int nResult = strcmp ((char *)sBinOld.acquirer_blocking[i].acquirer_id,(char *)sBinNew.acquirer_blocking[j].acquirer_id);
			if ( nResult == 0 )
			{
				bFound = TRUE;
				
			}
		}
	   if ( !bFound )
	   {
		   m_strCurrentData = "Record Deleted";
		   m_strFieldTag = "Acquirer ID";		
		   myUtilsObject.GetSystemTime(strTime);
		   pintime_get_gmt ( dest_str );
		   ZeroMemory ( &sHistoryData, sizeof (HST01));
		   memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"BIN01", sizeof (sHistoryData.file_name));
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
		   memcpy ((LPTSTR) sHistoryData.previous_data,     (LPCTSTR)sBinOld.acquirer_blocking[i].acquirer_id, sizeof (sHistoryData.previous_data));	
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
			int nResult = strcmp ((char *)sBinOld.acquirer_blocking[j].acquirer_id,(char *)sBinNew.acquirer_blocking[i].acquirer_id);
			if ( nResult == 0 )
			{
				bFound = TRUE;
			
			}
		}
	
	   if ( !bFound )
	   {
		   m_strCurrentData = "Record Added";
		   m_strFieldTag = "Acquirer ID ";
	 
         
		   myUtilsObject.GetSystemTime(strTime);
		   pintime_get_gmt ( dest_str );
		   ZeroMemory ( &sHistoryData, sizeof (HST01));
		   memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"BIN01", sizeof (sHistoryData.file_name));
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
		   memcpy ((LPTSTR) sHistoryData.previous_data,     (LPCTSTR)sBinNew.acquirer_blocking[i].acquirer_id, sizeof (sHistoryData.previous_data));	
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

void CBinBlocking::OnAdd() 
{
   CMyUtils myUtils;

   UpdateData(TRUE);

   if ( myUtils.IsEmpty ( m_StrAcquirerIDFld, this ) )
   {
      m_StrMessageLbl = "Please enter Acquirer ID";
      UpdateData(FALSE);
      return;
   }

   if ( m_List.GetCount () == 20 )
   {
      m_StrMessageLbl = "Maximum number of Acquirer IDs is 20";
      UpdateData(FALSE);
      return;
   }
	
   if (m_List.FindStringExact(-1, m_StrAcquirerIDFld) == LB_ERR)
   {
      m_List.AddString ( m_StrAcquirerIDFld );
      m_StrMessageLbl = "";
		m_StrAcquirerIDFld = "";
		
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
   }
   else
   {
		m_StrMessageLbl = "ERROR: Acquirer ID already exists";	
		m_CntlAcquirerIDFld.SetFocus();
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
   }


   if ( m_List.GetCount() == 20 )
      m_Add.EnableWindow(FALSE);


	m_CntlAcquirerIDFld.SetFocus();

	
}

void CBinBlocking::CleanBlockingDialog() 
{
	m_StrBinLengthFld = _T("");
	m_StrBinLowFld = _T("");
	m_StrAcquirerIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrBinTypeFld = _T("");
  
	m_List.ResetContent();
}

void CBinBlocking::PopulateBinDialogFromDatabase( BIN01 sBIN01Record ) 
{
	CString temp;

	m_List.ResetContent();
	m_StrAcquirerIDFld = "";

    for ( int i = 0; i < 20; i++)
    {
        temp = sBIN01Record.acquirer_blocking[i].acquirer_id;
        int nResult = temp.IsEmpty();
        if (nResult == 0)
        m_List.AddString((char *)sBIN01Record.acquirer_blocking[i].acquirer_id);
		
    }
	nOldCount = m_List.GetCount();

   if ( m_List.GetCount() == 20 )
      m_Add.EnableWindow(FALSE);

	SetModified ( FALSE );		
}

void CBinBlocking::PopulateDatabaseStructure( pBIN01 sBIN01Record )
{
   if ( !m_bBlocking )
      return;

    char temp [50] = "";

	//clear old id's from bin01 record
	for ( int idx = 0; idx < 20; idx++)
		memset ( sBIN01Record->acquirer_blocking[idx].acquirer_id , 0, 11);

	int nCount = m_List.GetCount();
	
	for ( int i = 0; i < nCount; i++)
	{
		m_List.GetText ( i, temp);
		memcpy ( sBIN01Record->acquirer_blocking[i].acquirer_id , (LPTSTR)temp, 11);
	}
	
}

void CBinBlocking::OnDelete() 
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
		m_StrMessageLbl = "Please select Acquirer ID to delete";		
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
	}	
}

BOOL CBinBlocking::OnSetActive() 
{
   if	( m_nDialogMode == MODE_DONE || m_nDialogMode == MODE_VIEW )
   {
      m_CntlAcquirerIDFld.EnableWindow(FALSE);
      m_Add.EnableWindow(FALSE);
      m_Delete.EnableWindow(FALSE);
      m_List.EnableWindow(FALSE);
   }
   else if ( m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT)
   {
      m_CntlAcquirerIDFld.EnableWindow(TRUE);
      m_Add.EnableWindow(TRUE);
      m_Delete.EnableWindow(TRUE);
      m_List.EnableWindow(TRUE);
   }

	return CPropertyPage::OnSetActive();
}

BOOL CBinBlocking::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified (TRUE);	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}


