// BinMidBlocking.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "BinMidBlocking.h"
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

IMPLEMENT_DYNCREATE(CBinMidBlocking, CPropertyPage)

CBinMidBlocking::CBinMidBlocking() : CPropertyPage(CBinMidBlocking::IDD)
{
	//{{AFX_DATA_INIT(CBinMidBlocking)
	m_StrBinLengthFld = _T("");
	m_StrBinLowFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrBinTypeFld = _T("");
	//}}AFX_DATA_INIT
}

CBinMidBlocking::~CBinMidBlocking()
{
}

void CBinMidBlocking::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBinMidBlocking)
	DDX_Control(pDX, IDC_MERCHANT_LIST, m_List);
	DDX_Control(pDX, IDC_MERCHANT_ID, m_CntlMerchantIDFld);
	DDX_Control(pDX, IDC_MERCHANT_ADD, m_Add);
	DDX_Control(pDX, IDC_MERCHANT_DELETE, m_Delete);
	DDX_Text(pDX, IDC_BIN_LENGTH_FLD_MID, m_StrBinLengthFld);
	DDX_Text(pDX, IDC_BIN_LOW_FLD_MID, m_StrBinLowFld);
	DDX_Text(pDX, IDC_MERCHANT_ID, m_StrMerchantIDFld);
	DDV_MaxChars(pDX, m_StrMerchantIDFld, 15);
	DDX_Text(pDX, IDC_MESSAGE_MID_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_TYPE_FLD_MID, m_StrBinTypeFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBinMidBlocking, CPropertyPage)
	//{{AFX_MSG_MAP(CBinMidBlocking)
	ON_BN_CLICKED(IDC_MERCHANT_ADD, OnAdd)
	ON_BN_CLICKED(IDC_MERCHANT_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBinMidBlocking message handlers

BOOL CBinMidBlocking::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 20;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_MID_LBL)->SetFont (&m_font);

    m_bBlocking = TRUE;
	m_CntlMerchantIDFld.SetFocus();

    if ( m_nDialogMode == MODE_VIEW )
		PopulateBinDialogFromDatabase ( sMidBinBlkRecord_Global );
	else if (m_nDialogMode == MODE_EDIT )
      PopulateBinDialogFromDatabase ( sOldMidBinBlkRecord_Global );

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBinMidBlocking::CompareStructures()
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


	for(i=0;i<50 && sOldMidBinBlkRecord_Global.merchant_blocking[i].merchant_id[0]!= '\0';i++)
	{
		bFound = FALSE;
		for(j=0;j<50 && sMidBinBlkRecord_Global.merchant_blocking[j].merchant_id[0]!= '\0';j++)
		{
			if(strcmp((char *)sOldMidBinBlkRecord_Global.merchant_blocking[i].merchant_id,
				(char *)sMidBinBlkRecord_Global.merchant_blocking[j].merchant_id)==0)
			{
				bFound = TRUE ;
			}

		}
		if(!bFound)
		{
		   m_strCurrentData = "Record Deleted";
		   m_strFieldTag = "MID Bin Blocking";		
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
		   memcpy ((LPTSTR) sHistoryData.previous_data,     (LPCTSTR)sOldMidBinBlkRecord_Global.merchant_blocking[i].merchant_id, sizeof (sHistoryData.previous_data));	
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

	for(i=0;i<50 && sMidBinBlkRecord_Global.merchant_blocking[i].merchant_id[0]!= '\0';i++)
	{
		bFound = FALSE;
		for(j=0;j<50 && sOldMidBinBlkRecord_Global.merchant_blocking[j].merchant_id[0]!= '\0';j++)
		{
			if(strcmp((char *)sMidBinBlkRecord_Global.merchant_blocking[i].merchant_id,
				(char *)sOldMidBinBlkRecord_Global.merchant_blocking[j].merchant_id)==0)
			{
				bFound = TRUE;
			}

		}
		if(!bFound)
		{
		   m_strCurrentData = "Record Added";
		   m_strFieldTag = "MID Bin Blocking";
 
 
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
		   memcpy ((LPTSTR) sHistoryData.previous_data, (LPCTSTR)sMidBinBlkRecord_Global.merchant_blocking[i].merchant_id, sizeof (sHistoryData.previous_data));	
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

void CBinMidBlocking::OnAdd() 
{
   CMyUtils myUtils;
   	CString str1;
	int db_mid_len,i = 0;
	int entered_mid_len = 0; 
	int str_falg = 0;


   UpdateData(TRUE);

   if ( myUtils.IsEmpty ( m_StrMerchantIDFld, this ) )
   {
      m_StrMessageLbl = "Please enter Merchant ID";
      UpdateData(FALSE);
      return;
   }
	int len = m_StrMerchantIDFld.GetLength();
	char *str = m_StrMerchantIDFld.GetBuffer(len);
	for(i=0;i<len;i++)
	{
		if( (str[i] > 47 && str[i] < 58 )				  || 
						   (str[i] == 32)                 ||
						   (str[i] == 45)                 ||
						   (str[i] >= 65 && str[i] <=90 ) ||
						   (str[i] >= 97 && str[i] <=122 )) 
		{
				//The character is a NUMBER , Alphabet , space or '-'
		}
		else
		{
			m_StrMessageLbl = "No Special characters  except Hyphen('-') and Space(' ') is allowed";	
			m_CntlMerchantIDFld.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);			
			}
			m_StrMerchantIDFld.ReleaseBuffer(len);

			return ;
		}
	}

   if ( m_List.GetCount () == 50 )
   {
      m_StrMessageLbl = "Maximum number of Merchant IDs is 50";
      UpdateData(FALSE);
      return;
   }
	
   /*if (m_List.FindString(-1, m_StrMerchantIDFld) == LB_ERR)
   {
      m_List.AddString ( m_StrMerchantIDFld );
      m_StrMessageLbl = "";
		m_StrMerchantIDFld = "";
		
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
   }*/

	for (i = 0; i < m_List.GetCount(); i++)
	{
	   db_mid_len = m_List.GetTextLen(i);
	   m_List.GetText(i, str1.GetBuffer(db_mid_len));
	   str1.ReleaseBuffer();
	  entered_mid_len =  m_StrMerchantIDFld.GetLength();
	  if(db_mid_len == entered_mid_len)
	  {
			if(0 == strncmp(m_StrMerchantIDFld,str1,db_mid_len))
			{
				str_falg = 1;
				break ;
			}
			else
			{
				continue ;
			}
	  }
	  
	}
	if (str_falg == 1)
    {
		
		m_StrMessageLbl = "ERROR: Merchant ID already exists";	
		m_CntlMerchantIDFld.SetFocus();
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);	
    }
	else
    {
		m_List.AddString ( m_StrMerchantIDFld );
		m_StrMessageLbl = "";
		m_StrMerchantIDFld = "";
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
   }


   if ( m_List.GetCount() == 50 )
      m_Add.EnableWindow(FALSE);


	m_CntlMerchantIDFld.SetFocus();

	
}

void CBinMidBlocking::CleanBlockingDialog() 
{
	m_StrBinLengthFld = _T("");
	m_StrBinLowFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrBinTypeFld = _T("");
  
	m_List.ResetContent();
}

void CBinMidBlocking::PopulateBinDialogFromDatabase(MIDBINBLK01  sLocalMidBinBlkRecord) 
{
	CString temp;

	m_List.ResetContent();
	m_StrMerchantIDFld = "";

    for ( int i = 0; i < 50; i++)
    {
        temp = sLocalMidBinBlkRecord.merchant_blocking[i].merchant_id;
        int nResult = temp.IsEmpty();
        if (nResult == 0)
			m_List.AddString((char *)sLocalMidBinBlkRecord.merchant_blocking[i].merchant_id);
		
    }
	nOldCount = m_List.GetCount();

   if ( m_List.GetCount() == 50 )
      m_Add.EnableWindow(FALSE);

	SetModified ( FALSE );		
}

void CBinMidBlocking::PopulateDatabaseStructure()
{
   if ( !m_bBlocking )
      return;

    char temp [50] = "";

	//clear old id's from bin01 record
	for ( int idx = 0; idx < 50; idx++)
		memset ( sMidBinBlkRecord_Global.merchant_blocking[idx].merchant_id , 0, 15);

	int nCount = m_List.GetCount();
	
	for ( int i = 0; i < nCount; i++)
	{
		m_List.GetText ( i, temp);
		memcpy ( sMidBinBlkRecord_Global.merchant_blocking[i].merchant_id , (LPTSTR)temp, 15);
	}
	
}

void CBinMidBlocking::OnDelete() 
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
		m_StrMessageLbl = "Please select Merchant ID to delete";		
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
	}	
}

BOOL CBinMidBlocking::OnSetActive() 
{
   if	( m_nDialogMode == MODE_DONE || m_nDialogMode == MODE_VIEW )
   {
      m_CntlMerchantIDFld.EnableWindow(FALSE);
      m_Add.EnableWindow(FALSE);
      m_Delete.EnableWindow(FALSE);
      m_List.EnableWindow(FALSE);
   }
   else if ( m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT)
   {
      m_CntlMerchantIDFld.EnableWindow(TRUE);
      m_Add.EnableWindow(TRUE);
      m_Delete.EnableWindow(TRUE);
      m_List.EnableWindow(TRUE);
   }

	return CPropertyPage::OnSetActive();
}

BOOL CBinMidBlocking::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified (TRUE);	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}


