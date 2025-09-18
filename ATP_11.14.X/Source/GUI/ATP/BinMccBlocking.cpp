// BinMCCBlocking.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "BinMCCBlocking.h"
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
int nLowRange;
	 int nHighRange; 
extern CSharedMem shrmem;
extern MIDBINBLK01	sMidBinBlkRecord_Global ;
extern MIDBINBLK01 sOldMidBinBlkRecord_Global;

/////////////////////////////////////////////////////////////////////////////
// CBinMCCBlocking property page

IMPLEMENT_DYNCREATE(CBinMCCBlocking, CPropertyPage)

CBinMCCBlocking::CBinMCCBlocking() : CPropertyPage(CBinMCCBlocking::IDD)
{
	//{{AFX_DATA_INIT(CBinMCCBlocking)
	m_StrBinLengthFld1 = _T("");
	m_StrBinLowFld1 = _T("");
	m_StrHighRange1 = _T("");
	m_StrHighRange10 = _T("");
	m_StrHighRange2 = _T("");
	m_StrHighRange3 = _T("");
	m_StrHighRange4 = _T("");
	m_StrHighRange5 = _T("");
	m_StrHighRange6 = _T("");
	m_StrHighRange7 = _T("");
	m_StrHighRange8 = _T("");
	m_StrHighRange9 = _T("");
	m_StrLowRange1 = _T("");
	m_StrLowRange10 = _T("");
	m_StrLowRange2 = _T("");
	m_StrLowRange3 = _T("");
	m_StrLowRange4 = _T("");
	m_StrLowRange5 = _T("");
	m_StrLowRange6 = _T("");
	m_StrLowRange7 = _T("");
	m_StrLowRange8 = _T("");
	m_StrLowRange9 = _T("");
	m_StrMccBinTypeFld1 = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CBinMCCBlocking::~CBinMCCBlocking()
{
}

void CBinMCCBlocking::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBinMCCBlocking)
	DDX_Control(pDX, IDC_LOW_RANGE9, m_CntlLowRange9);
	DDX_Control(pDX, IDC_LOW_RANGE8, m_CntlLowRange8);
	DDX_Control(pDX, IDC_LOW_RANGE7, m_CntlLowRange7);
	DDX_Control(pDX, IDC_LOW_RANGE6, m_CntlLowRange6);
	DDX_Control(pDX, IDC_LOW_RANGE5, m_CntlLowRange5);
	DDX_Control(pDX, IDC_LOW_RANGE4, m_CntlLowRange4);
	DDX_Control(pDX, IDC_LOW_RANGE3, m_CntlLowRange3);
	DDX_Control(pDX, IDC_LOW_RANGE2, m_CntlLowRange2);
	DDX_Control(pDX, IDC_LOW_RANGE10, m_CntlLowRange10);
	DDX_Control(pDX, IDC_LOW_RANGE1, m_CntlLowRange1);
	DDX_Control(pDX, IDC_HIGH_RANGE9, m_CntlHighRange9);
	DDX_Control(pDX, IDC_HIGH_RANGE8, m_CntlHighRange8);
	DDX_Control(pDX, IDC_HIGH_RANGE7, m_CntlHighRange7);
	DDX_Control(pDX, IDC_HIGH_RANGE6, m_CntlHighRange6);
	DDX_Control(pDX, IDC_HIGH_RANGE5, m_CntlHighRange5);
	DDX_Control(pDX, IDC_HIGH_RANGE4, m_CntlHighRange4);
	DDX_Control(pDX, IDC_HIGH_RANGE3, m_CntlHighRange3);
	DDX_Control(pDX, IDC_HIGH_RANGE2, m_CntlHighRange2);
	DDX_Control(pDX, IDC_HIGH_RANGE10, m_CntlHighRange10);
	DDX_Control(pDX, IDC_HIGH_RANGE1, m_CntlHighRange1);
	DDX_Text(pDX, IDC_BIN_LENGTH_FLD1, m_StrBinLengthFld1);
	DDX_Text(pDX, IDC_BIN_LOW_FLD1, m_StrBinLowFld1);
	DDX_Text(pDX, IDC_HIGH_RANGE1, m_StrHighRange1);
	DDV_MaxChars(pDX, m_StrHighRange1, 5);
	DDX_Text(pDX, IDC_HIGH_RANGE10, m_StrHighRange10);
	DDV_MaxChars(pDX, m_StrHighRange10, 5);
	DDX_Text(pDX, IDC_HIGH_RANGE2, m_StrHighRange2);
	DDV_MaxChars(pDX, m_StrHighRange2, 5);
	DDX_Text(pDX, IDC_HIGH_RANGE3, m_StrHighRange3);
	DDV_MaxChars(pDX, m_StrHighRange3, 5);
	DDX_Text(pDX, IDC_HIGH_RANGE4, m_StrHighRange4);
	DDV_MaxChars(pDX, m_StrHighRange4, 5);
	DDX_Text(pDX, IDC_HIGH_RANGE5, m_StrHighRange5);
	DDV_MaxChars(pDX, m_StrHighRange5, 5);
	DDX_Text(pDX, IDC_HIGH_RANGE6, m_StrHighRange6);
	DDV_MaxChars(pDX, m_StrHighRange6, 5);
	DDX_Text(pDX, IDC_HIGH_RANGE7, m_StrHighRange7);
	DDV_MaxChars(pDX, m_StrHighRange7, 5);
	DDX_Text(pDX, IDC_HIGH_RANGE8, m_StrHighRange8);
	DDV_MaxChars(pDX, m_StrHighRange8, 5);
	DDX_Text(pDX, IDC_HIGH_RANGE9, m_StrHighRange9);
	DDV_MaxChars(pDX, m_StrHighRange9, 5);
	DDX_Text(pDX, IDC_LOW_RANGE1, m_StrLowRange1);
	DDV_MaxChars(pDX, m_StrLowRange1, 5);
	DDX_Text(pDX, IDC_LOW_RANGE10, m_StrLowRange10);
	DDV_MaxChars(pDX, m_StrLowRange10, 5);
	DDX_Text(pDX, IDC_LOW_RANGE2, m_StrLowRange2);
	DDV_MaxChars(pDX, m_StrLowRange2, 5);
	DDX_Text(pDX, IDC_LOW_RANGE3, m_StrLowRange3);
	DDV_MaxChars(pDX, m_StrLowRange3, 5);
	DDX_Text(pDX, IDC_LOW_RANGE4, m_StrLowRange4);
	DDV_MaxChars(pDX, m_StrLowRange4, 5);
	DDX_Text(pDX, IDC_LOW_RANGE5, m_StrLowRange5);
	DDV_MaxChars(pDX, m_StrLowRange5, 5);
	DDX_Text(pDX, IDC_LOW_RANGE6, m_StrLowRange6);
	DDV_MaxChars(pDX, m_StrLowRange6, 5);
	DDX_Text(pDX, IDC_LOW_RANGE7, m_StrLowRange7);
	DDV_MaxChars(pDX, m_StrLowRange7, 5);
	DDX_Text(pDX, IDC_LOW_RANGE8, m_StrLowRange8);
	DDV_MaxChars(pDX, m_StrLowRange8, 5);
	DDX_Text(pDX, IDC_LOW_RANGE9, m_StrLowRange9);
	DDV_MaxChars(pDX, m_StrLowRange9, 5);
	DDX_Text(pDX, IDC_MCC_BIN_TYPE_FLD1, m_StrMccBinTypeFld1);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBinMCCBlocking, CPropertyPage)
	//{{AFX_MSG_MAP(CBinMCCBlocking)
	ON_EN_KILLFOCUS(IDC_HIGH_RANGE1, OnKillfocusHighRange1)
	ON_EN_KILLFOCUS(IDC_HIGH_RANGE2, OnKillfocusHighRange2)
	ON_EN_KILLFOCUS(IDC_HIGH_RANGE3, OnKillfocusHighRange3)
	ON_EN_KILLFOCUS(IDC_HIGH_RANGE4, OnKillfocusHighRange4)
	ON_EN_KILLFOCUS(IDC_HIGH_RANGE5, OnKillfocusHighRange5)
	ON_EN_KILLFOCUS(IDC_HIGH_RANGE6, OnKillfocusHighRange6)
	ON_EN_KILLFOCUS(IDC_HIGH_RANGE7, OnKillfocusHighRange7)
	ON_EN_KILLFOCUS(IDC_HIGH_RANGE8, OnKillfocusHighRange8)
	ON_EN_KILLFOCUS(IDC_HIGH_RANGE9, OnKillfocusHighRange9)
	ON_EN_KILLFOCUS(IDC_HIGH_RANGE10, OnKillfocusHighRange10)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBinMCCBlocking message handlers

BOOL CBinMCCBlocking::OnInitDialog() 
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

    m_bMCCBlocking = TRUE;	
	
    if ( m_nDialogMode == MODE_VIEW || m_nDialogMode == MODE_EDIT )
       PopulateBinDialogFromDatabase ( sBinRecord );	   

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CBinMCCBlocking::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified (TRUE);	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}


BOOL CBinMCCBlocking::CompareStructures( BIN01 sBinOld)
{
    
	if (!m_bMCCBlocking)
		 return TRUE;

	if (strcmp ( m_StrLowRange1, (char *)sBinOld.mcc_low[0].low_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrLowRange1, sizeof (m_strCurrentData));		
		memcpy(m_strPreviousData, sBinOld.mcc_low[0].low_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC1 Low ", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrHighRange1, (char *)sBinOld.mcc_high[0].high_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrHighRange1, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sBinOld.mcc_high[1].high_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC1 High ", sizeof (m_strFieldTag));      
		PopulateHistoryTable();
	}
	     
		
	if (strcmp ( m_StrLowRange2, (char *)sBinOld.mcc_low[1].low_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrLowRange2, sizeof (m_strCurrentData));		
		memcpy(m_strPreviousData, sBinOld.mcc_low[1].low_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC2 Low", sizeof (m_strFieldTag));
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrHighRange2, (char *)sBinOld.mcc_high[1].high_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrHighRange2, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sBinOld.mcc_high[1].high_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC2 High ", sizeof (m_strFieldTag));
		PopulateHistoryTable();

	}
	if (strcmp ( m_StrLowRange3, (char *)sBinOld.mcc_low[2].low_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrLowRange3, sizeof (m_strCurrentData));		
		memcpy(m_strPreviousData, sBinOld.mcc_low[2].low_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC3 Low ", sizeof (m_strFieldTag));
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrHighRange3, (char *)sBinOld.mcc_high[2].high_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrHighRange3, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sBinOld.mcc_high[2].high_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC3 High ", sizeof (m_strFieldTag));
		PopulateHistoryTable();

	}
	if (strcmp ( m_StrLowRange4, (char *)sBinOld.mcc_low[3].low_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrLowRange4, sizeof (m_strCurrentData));		
		memcpy(m_strPreviousData, sBinOld.mcc_low[3].low_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC4 Low ", sizeof (m_strFieldTag));
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrHighRange4, (char *)sBinOld.mcc_high[3].high_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrHighRange4, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sBinOld.mcc_high[3].high_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC4 High ", sizeof (m_strFieldTag));
		PopulateHistoryTable();

	}
	if (strcmp ( m_StrLowRange5, (char *)sBinOld.mcc_low[4].low_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrLowRange5, sizeof (m_strCurrentData));		
		memcpy(m_strPreviousData, sBinOld.mcc_low[4].low_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC5 Low ", sizeof (m_strFieldTag));
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrHighRange5, (char *)sBinOld.mcc_high[4].high_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrHighRange5, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sBinOld.mcc_high[4].high_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC5 High ", sizeof (m_strFieldTag));
		PopulateHistoryTable();

	}
	if (strcmp ( m_StrLowRange6, (char *)sBinOld.mcc_low[5].low_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrLowRange6, sizeof (m_strCurrentData));		
		memcpy(m_strPreviousData, sBinOld.mcc_low[5].low_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC6 Low ", sizeof (m_strFieldTag));
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrHighRange6, (char *)sBinOld.mcc_high[5].high_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrHighRange6, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sBinOld.mcc_high[5].high_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC6 High ", sizeof (m_strFieldTag));
		PopulateHistoryTable();

	}
    if (strcmp ( m_StrLowRange7, (char *)sBinOld.mcc_low[6].low_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrLowRange7, sizeof (m_strCurrentData));		
		memcpy(m_strPreviousData, sBinOld.mcc_low[6].low_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC7 Low ", sizeof (m_strFieldTag));
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrHighRange7, (char *)sBinOld.mcc_high[6].high_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrHighRange7, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sBinOld.mcc_high[6].high_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC7 High ", sizeof (m_strFieldTag));
		PopulateHistoryTable();

	}
if (strcmp ( m_StrLowRange8, (char *)sBinOld.mcc_low[7].low_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrLowRange8, sizeof (m_strCurrentData));		
		memcpy(m_strPreviousData, sBinOld.mcc_low[7].low_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC8 Low ", sizeof (m_strFieldTag));
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrHighRange8, (char *)sBinOld.mcc_high[7].high_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrHighRange8, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sBinOld.mcc_high[7].high_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC8 High ", sizeof (m_strFieldTag));
		PopulateHistoryTable();

	}
	if (strcmp ( m_StrLowRange9, (char *)sBinOld.mcc_low[8].low_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrLowRange9, sizeof (m_strCurrentData));		
		memcpy(m_strPreviousData, sBinOld.mcc_low[8].low_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC9 Low ", sizeof (m_strFieldTag));
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrHighRange9, (char *)sBinOld.mcc_high[8].high_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrHighRange9, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sBinOld.mcc_high[8].high_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC9 High ", sizeof (m_strFieldTag));
		PopulateHistoryTable();

	}
	if (strcmp ( m_StrLowRange10, (char *)sBinOld.mcc_low[9].low_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrLowRange10, sizeof (m_strCurrentData));		
		memcpy(m_strPreviousData, sBinOld.mcc_low[9].low_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC10 Low ", sizeof (m_strFieldTag));
		PopulateHistoryTable();
	}

	if (strcmp ( m_StrHighRange10, (char *)sBinOld.mcc_high[9].high_range) != 0)
	{
		memcpy(m_strCurrentData, m_StrHighRange10, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, sBinOld.mcc_high[9].high_range, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "MCC10 High ", sizeof (m_strFieldTag));
		PopulateHistoryTable();

	}

	return TRUE;
}

void CBinMCCBlocking::PopulateHistoryTable()
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

	strcpy ( strPrimaryKey, m_StrBinLengthFld1);
	 strcat ( strPrimaryKey, m_StrBinLowFld1 );
	 strncat ( strPrimaryKey, m_StrMccBinTypeFld1,1 );
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR)sHistoryData.file_name,				(LPCTSTR)"BIN01",			sizeof (sHistoryData.file_name));
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



void CBinMCCBlocking::CleanBlockingDialog() 
{
	m_StrBinLengthFld1 = _T("");
	m_StrBinLowFld1 = _T("");
	m_StrMessageLbl = _T("");
	m_StrMccBinTypeFld1 = _T("");
	m_StrLowRange1 = _T("");
	m_StrLowRange10 = _T("");
	m_StrLowRange2 = _T("");
	m_StrLowRange3 = _T("");
	m_StrLowRange4 = _T("");
	m_StrLowRange5 = _T("");
	m_StrLowRange6 = _T("");
	m_StrLowRange7 = _T("");
	m_StrLowRange8 = _T("");
	m_StrLowRange9 = _T("");
	m_StrHighRange1 = _T("");
	m_StrHighRange10 = _T("");
	m_StrHighRange2 = _T("");
	m_StrHighRange3 = _T("");
	m_StrHighRange4 = _T("");
	m_StrHighRange5 = _T("");
	m_StrHighRange6 = _T("");
	m_StrHighRange7 = _T("");
	m_StrHighRange8 = _T("");
	m_StrHighRange9 = _T("");
  

}


	void CBinMCCBlocking::EnableBlockingDialog ( int nStatus )
{
	if ( m_nDialogMode != MODE_VIEW )
	{
		m_CntlLowRange1.EnableWindow(nStatus);
		m_CntlHighRange1.EnableWindow(nStatus);
	}
	
}

BOOL CBinMCCBlocking::PopulateBinDialogFromDatabase ( BIN01 sBIN01Record )
{
 /* if ( m_nDialogMode != MODE_VIEW && m_bMCCBlocking)	
		EnableBlockingDialog(TRUE);
  */	
	m_StrLowRange1  =  sBIN01Record.mcc_low[0].low_range;
	m_StrHighRange1 =  sBIN01Record.mcc_high[0].high_range;
	m_StrLowRange2  =  sBIN01Record.mcc_low[1].low_range;
	m_StrHighRange2 =  sBIN01Record.mcc_high[1].high_range;
	m_StrLowRange3  =  sBIN01Record.mcc_low[2].low_range;
	m_StrHighRange3 =  sBIN01Record.mcc_high[2].high_range;
	m_StrLowRange4  =  sBIN01Record.mcc_low[3].low_range;
	m_StrHighRange4 =  sBIN01Record.mcc_high[3].high_range;
	m_StrLowRange5  =  sBIN01Record.mcc_low[4].low_range;
	m_StrHighRange5 =  sBIN01Record.mcc_high[4].high_range;
	m_StrLowRange6  =  sBIN01Record.mcc_low[5].low_range;
	m_StrHighRange6 =  sBIN01Record.mcc_high[5].high_range;
	m_StrLowRange7  =  sBIN01Record.mcc_low[6].low_range;
	m_StrHighRange7 =  sBIN01Record.mcc_high[6].high_range;
	m_StrLowRange8  =  sBIN01Record.mcc_low[7].low_range;
	m_StrHighRange8 =  sBIN01Record.mcc_high[7].high_range;
	m_StrLowRange9  =  sBIN01Record.mcc_low[8].low_range;
	m_StrHighRange9 =  sBIN01Record.mcc_high[8].high_range;
	m_StrLowRange10 =  sBIN01Record.mcc_low[9].low_range;
	m_StrHighRange10 = sBIN01Record.mcc_high[9].high_range;
   
   	return TRUE;
}

BOOL CBinMCCBlocking::PopulateDatabaseStructure( pBIN01 sBIN01Record )
{
   if ( !m_bMCCBlocking )
	return TRUE;


   
   int bin_idx = 0;
    
    if (!(m_StrLowRange1.IsEmpty() && m_StrHighRange1.IsEmpty())) //will not populate blank ranges to database
	{
		memcpy ( sBIN01Record->mcc_low[bin_idx].low_range,		m_StrLowRange1,	    sizeof ( sBIN01Record->mcc_low[bin_idx].low_range));
		memcpy ( sBIN01Record->mcc_high[bin_idx++].high_range,	m_StrHighRange1,	sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
	}
    if (!(m_StrLowRange2.IsEmpty() && m_StrHighRange2.IsEmpty())) //will not populate blank ranges to database
	{
		memcpy ( sBIN01Record->mcc_low[bin_idx].low_range,		m_StrLowRange2,	    sizeof ( sBIN01Record->mcc_low[bin_idx].low_range));
		memcpy ( sBIN01Record->mcc_high[bin_idx++].high_range,	m_StrHighRange2,	sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
	}
    if (!(m_StrLowRange3.IsEmpty() && m_StrHighRange3.IsEmpty())) //will not populate blank ranges to database
	{
		memcpy ( sBIN01Record->mcc_low[bin_idx].low_range,		m_StrLowRange3,	    sizeof ( sBIN01Record->mcc_low[bin_idx].low_range));
		memcpy ( sBIN01Record->mcc_high[bin_idx++].high_range,	m_StrHighRange3,	sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
	}
    if (!(m_StrLowRange4.IsEmpty() && m_StrHighRange4.IsEmpty())) //will not populate blank ranges to database
	{
		memcpy ( sBIN01Record->mcc_low[bin_idx].low_range,		m_StrLowRange4,	    sizeof ( sBIN01Record->mcc_low[bin_idx].low_range));
		memcpy ( sBIN01Record->mcc_high[bin_idx++].high_range,	m_StrHighRange4,	sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
	}
    if (!(m_StrLowRange5.IsEmpty() && m_StrHighRange5.IsEmpty())) //will not populate blank ranges to database
	{
		memcpy ( sBIN01Record->mcc_low[bin_idx].low_range,		m_StrLowRange5,	    sizeof ( sBIN01Record->mcc_low[bin_idx].low_range));
		memcpy ( sBIN01Record->mcc_high[bin_idx++].high_range,	m_StrHighRange5,	sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
	}
    if (!(m_StrLowRange6.IsEmpty() && m_StrHighRange6.IsEmpty())) //will not populate blank ranges to database
	{
		memcpy ( sBIN01Record->mcc_low[bin_idx].low_range,		m_StrLowRange6,		sizeof ( sBIN01Record->mcc_low[bin_idx].low_range));
		memcpy ( sBIN01Record->mcc_high[bin_idx++].high_range,	m_StrHighRange6,	sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
	}
    if (!(m_StrLowRange7.IsEmpty() && m_StrHighRange7.IsEmpty())) //will not populate blank ranges to database
	{
		memcpy ( sBIN01Record->mcc_low[bin_idx].low_range,		m_StrLowRange7,		sizeof ( sBIN01Record->mcc_low[bin_idx].low_range));
		memcpy ( sBIN01Record->mcc_high[bin_idx++].high_range,	m_StrHighRange7,	sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
	}
    if (!(m_StrLowRange8.IsEmpty() && m_StrHighRange8.IsEmpty())) //will not populate blank ranges to database
	{
		memcpy ( sBIN01Record->mcc_low[bin_idx].low_range,		m_StrLowRange8,		sizeof ( sBIN01Record->mcc_low[bin_idx].low_range));
		memcpy ( sBIN01Record->mcc_high[bin_idx++].high_range,	m_StrHighRange8,	sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
	}
    if (!(m_StrLowRange9.IsEmpty() && m_StrHighRange9.IsEmpty())) //will not populate blank ranges to database
	{
		memcpy ( sBIN01Record->mcc_low[bin_idx].low_range,		m_StrLowRange9,		sizeof ( sBIN01Record->mcc_low[bin_idx].low_range));
		memcpy ( sBIN01Record->mcc_high[bin_idx++].high_range,	m_StrHighRange9,	sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
	}
    if (!(m_StrLowRange10.IsEmpty() && m_StrHighRange10.IsEmpty())) //will not populate blank ranges to database
	{
		memcpy ( sBIN01Record->mcc_low[bin_idx].low_range,		m_StrLowRange10,	sizeof ( sBIN01Record->mcc_low[bin_idx].low_range));
		memcpy ( sBIN01Record->mcc_high[bin_idx++].high_range,	m_StrHighRange10,	sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
	}
	
	while(bin_idx < 10)//clear any old ranges left in bin01 record.
	{
		memset ( sBIN01Record->mcc_low[bin_idx].low_range, 0x00, sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
		memset ( sBIN01Record->mcc_high[bin_idx].high_range, 0x00, sizeof ( sBIN01Record->mcc_high[bin_idx].high_range));
		bin_idx++;
	}

  	
   return TRUE;
	
}
/////////////////////////////////////////////////////////////////////////////


BOOL CBinMCCBlocking::OnSetActive() 
{
	
	if	( m_nDialogMode == MODE_DONE || m_nDialogMode == MODE_VIEW )
   {
         m_CntlLowRange1.EnableWindow(FALSE);
		 m_CntlHighRange1.EnableWindow(FALSE);
         m_CntlLowRange2.EnableWindow(FALSE);
		 m_CntlHighRange2.EnableWindow(FALSE);
		 m_CntlLowRange3.EnableWindow(FALSE);
		 m_CntlHighRange3.EnableWindow(FALSE);
		 m_CntlLowRange4.EnableWindow(FALSE);
		 m_CntlHighRange4.EnableWindow(FALSE);
		 m_CntlLowRange5.EnableWindow(FALSE);
		 m_CntlHighRange5.EnableWindow(FALSE);
		 m_CntlLowRange6.EnableWindow(FALSE);
		 m_CntlHighRange6.EnableWindow(FALSE);
		 m_CntlLowRange7.EnableWindow(FALSE);
		 m_CntlHighRange7.EnableWindow(FALSE);
		 m_CntlLowRange8.EnableWindow(FALSE);
		 m_CntlHighRange8.EnableWindow(FALSE);
		 m_CntlLowRange9.EnableWindow(FALSE);
		 m_CntlHighRange9.EnableWindow(FALSE);
		 m_CntlLowRange10.EnableWindow(FALSE);
		 m_CntlHighRange10.EnableWindow(FALSE);
   }
  else if ( m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT)
   {
         m_CntlLowRange1.EnableWindow(TRUE);
		 m_CntlHighRange1.EnableWindow(TRUE);
         m_CntlLowRange2.EnableWindow(TRUE);
		 m_CntlHighRange2.EnableWindow(TRUE);
		 m_CntlLowRange3.EnableWindow(TRUE);
		 m_CntlHighRange3.EnableWindow(TRUE);
		 m_CntlLowRange4.EnableWindow(TRUE);
		 m_CntlHighRange4.EnableWindow(TRUE);
		 m_CntlLowRange5.EnableWindow(TRUE);
		 m_CntlHighRange5.EnableWindow(TRUE);
		 m_CntlLowRange6.EnableWindow(TRUE);
		 m_CntlHighRange6.EnableWindow(TRUE);
		 m_CntlLowRange7.EnableWindow(TRUE);
		 m_CntlHighRange7.EnableWindow(TRUE);
		 m_CntlLowRange8.EnableWindow(TRUE);
		 m_CntlHighRange8.EnableWindow(TRUE);
		 m_CntlLowRange9.EnableWindow(TRUE);
		 m_CntlHighRange9.EnableWindow(TRUE);
		 m_CntlLowRange10.EnableWindow(TRUE);
		 m_CntlHighRange10.EnableWindow(TRUE);  
  }

	
	return CPropertyPage::OnSetActive();
}

//BOOL CBinMCCBlocking::OnKillfocusHighRange1() 
void CBinMCCBlocking::OnKillfocusHighRange1() 
{
	
	CMyUtils myUtilsObject;
	 		
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_StrMessageLbl.Empty();

	//if both low and high are empty during edit, don't error
	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrLowRange1.IsEmpty() && m_StrHighRange1.IsEmpty())
		//return TRUE;
		return;

    if (myUtilsObject.IsEmpty (m_StrLowRange1, this))
    {
		m_StrMessageLbl = "Please enter Low Range1";	
		m_CntlLowRange1.SetFocus();		
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrHighRange1, this))
    {
		m_StrMessageLbl = "Please enter High Range1";	
		m_CntlLowRange1.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
		
	}	
			nLowRange = atoi(m_StrLowRange1);
			nHighRange = atoi(m_StrHighRange1);           
			
			if ( nHighRange < nLowRange)
			{
				m_StrMessageLbl = "ERROR: High Range1 cannot be less than or equal to Low Range1";	
				m_CntlLowRange1.SetFocus();
				
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
			
			//	m_nDialogMode = MODE_DONE;
				//return FALSE;
				return;
			}

			m_CntlLowRange2.EnableWindow(TRUE);
			m_CntlHighRange2.EnableWindow(TRUE);
		//	m_CntlLowRange2.SetFocus();     
			//return TRUE;	
			return;
}

//BOOL CBinMCCBlocking::OnKillfocusHighRange2() 
void CBinMCCBlocking::OnKillfocusHighRange2() 
{
	
	CMyUtils myUtilsObject;
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_StrMessageLbl.Empty();

	//if both low and high are empty during edit, don't error
	if(m_nDialogMode == MODE_EDIT && m_StrLowRange2.IsEmpty() && m_StrHighRange2.IsEmpty())
		//return TRUE;
		return;

    if (myUtilsObject.IsEmpty (m_StrLowRange2, this))
    {
		m_StrMessageLbl = "Please enter Low Range2";	
		m_CntlLowRange2.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE ;
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrHighRange2, this))
    {
		m_StrMessageLbl = "Please enter High Range2";	
		m_CntlLowRange2.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}


			nLowRange = atoi(m_StrLowRange2);
			nHighRange = atoi(m_StrHighRange2);
			if ( nHighRange < nLowRange)
			{
				m_StrMessageLbl = "ERROR: High Range2 cannot be less than  or equal to  Low Range2";	
				m_CntlLowRange2.SetFocus();
			
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
			//	m_nDialogMode = MODE_DONE;
			//	return FALSE;
				return;
			} 

		
			
			m_CntlLowRange3.EnableWindow(TRUE);
			m_CntlHighRange3.EnableWindow(TRUE);
			//m_CntlLowRange3.SetFocus();
			//return TRUE;
			return;
	
}

//BOOL CBinMCCBlocking::OnKillfocusHighRange3() 
void CBinMCCBlocking::OnKillfocusHighRange3() 
	{
	
	CMyUtils myUtilsObject;
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_StrMessageLbl.Empty();


	//if both low and high are empty during edit, don't error
	if(m_nDialogMode == MODE_EDIT && m_StrLowRange3.IsEmpty() && m_StrHighRange3.IsEmpty())
		//return TRUE;
		return;

    if (myUtilsObject.IsEmpty (m_StrLowRange3, this))
    {
		m_StrMessageLbl = "Please enter Low Range3";	
		m_CntlLowRange3.SetFocus();
		UpdateData(FALSE);
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	//	m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrHighRange3, this))
    {
		m_StrMessageLbl = "Please enter High Range3";	
		m_CntlLowRange3.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}
	
			nLowRange = atoi(m_StrLowRange3);
			nHighRange = atoi(m_StrHighRange3);

			if ( nHighRange < nLowRange)
			{
				m_StrMessageLbl = "ERROR: High Range3 cannot be less than or equal to Low Range3";	
				m_CntlLowRange3.SetFocus();
			
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
			//	m_nDialogMode = MODE_DONE;
				//return FALSE;
				return;
			} 	
        
		
		
			m_CntlLowRange4.EnableWindow(TRUE);
			m_CntlHighRange4.EnableWindow(TRUE);
			//m_CntlLowRange4.SetFocus();	

			//return TRUE;
			return;
}

//BOOL CBinMCCBlocking::OnKillfocusHighRange4() 
void CBinMCCBlocking::OnKillfocusHighRange4() 
{
	
	CMyUtils myUtilsObject;
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_StrMessageLbl.Empty();

	//if both low and high are empty during edit, don't error
	if(m_nDialogMode == MODE_EDIT && m_StrLowRange4.IsEmpty() && m_StrHighRange4.IsEmpty())
		//return TRUE;
		return;

    if (myUtilsObject.IsEmpty (m_StrLowRange4, this))
    {
		m_StrMessageLbl = "Please enter Low Range4";	
		m_CntlLowRange4.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrHighRange4, this))
    {
		m_StrMessageLbl = "Please enter High Range4";	
		m_CntlLowRange4.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}

			nLowRange = atoi(m_StrLowRange4);
			nHighRange = atoi(m_StrHighRange4);
			if ( nHighRange < nLowRange)
			{
				m_StrMessageLbl = "ERROR: High Range4 cannot be less than or equal to  Low Range4";	
				m_CntlLowRange4.SetFocus();
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				//m_nDialogMode = MODE_DONE;
				//return FALSE;
				return;
			} 	
        
		
			m_CntlLowRange5.EnableWindow(TRUE);
			m_CntlHighRange5.EnableWindow(TRUE);
			//m_CntlLowRange5.SetFocus();	
			//return TRUE;
			return;
}

//BOOL CBinMCCBlocking::OnKillfocusHighRange5() 
void CBinMCCBlocking::OnKillfocusHighRange5() 
{
	
	CMyUtils myUtilsObject;
 	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_StrMessageLbl.Empty();

	//if both low and high are empty during edit, don't error
	if(m_nDialogMode == MODE_EDIT && m_StrLowRange5.IsEmpty() && m_StrHighRange5.IsEmpty())
		//return TRUE;
		return;

    if (myUtilsObject.IsEmpty (m_StrLowRange5, this))
    {
		m_StrMessageLbl = "Please enter Low Range5";	
		m_CntlLowRange5.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	//	m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrHighRange5, this))
    {
		m_StrMessageLbl = "Please enter High Range5";	
		m_CntlHighRange5.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	//	m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}	
			nLowRange = atoi(m_StrLowRange5);
			nHighRange = atoi(m_StrHighRange5);
			if ( nHighRange < nLowRange)
			{
				m_StrMessageLbl = "ERROR: High Range5 cannot be less than or equal to  Low Range5";	
				m_CntlHighRange5.SetFocus();
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
			//	m_nDialogMode = MODE_DONE;
				//return FALSE;
				return;
			}  

	
			m_CntlLowRange6.EnableWindow(TRUE);
			m_CntlHighRange6.EnableWindow(TRUE);
			//m_CntlLowRange6.SetFocus();	
			//return TRUE;
			return;
}

//BOOL CBinMCCBlocking::OnKillfocusHighRange6() 
void CBinMCCBlocking::OnKillfocusHighRange6()
{
	
	CMyUtils myUtilsObject;
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_StrMessageLbl.Empty();

	//if both low and high are empty during edit, don't error
	if(m_nDialogMode == MODE_EDIT && m_StrLowRange6.IsEmpty() && m_StrHighRange6.IsEmpty())
		//return TRUE;
		return;

    if (myUtilsObject.IsEmpty (m_StrLowRange6, this))
    {
		m_StrMessageLbl = "Please enter Low Range6";	
		m_CntlLowRange6.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrHighRange6, this))
    {
		m_StrMessageLbl = "Please enter High Range6";	
		m_CntlHighRange6.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}
	
			nLowRange = atoi(m_StrLowRange6);
			nHighRange = atoi(m_StrHighRange6);
			if ( nHighRange < nLowRange)
			{
				m_StrMessageLbl = "ERROR: High Range6 cannot be less than or equal to  Low Range6";	
				m_CntlHighRange6.SetFocus();
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				//m_nDialogMode = MODE_DONE;
				//return FALSE;
				return;
			}

			m_CntlLowRange7.EnableWindow(TRUE);
			m_CntlHighRange7.EnableWindow(TRUE);
			//m_CntlLowRange7.SetFocus();	
			//return TRUE;
			return;
}

//BOOL CBinMCCBlocking::OnKillfocusHighRange7() 
void CBinMCCBlocking::OnKillfocusHighRange7()
{
	
	CMyUtils myUtilsObject;
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_StrMessageLbl.Empty();

	//if both low and high are empty during edit, don't error
	if(m_nDialogMode == MODE_EDIT && m_StrLowRange7.IsEmpty() && m_StrHighRange7.IsEmpty())
		//return TRUE;
		return;

    if (myUtilsObject.IsEmpty (m_StrLowRange7, this))
    {
		m_StrMessageLbl = "Please enter Low Range7";	
		m_CntlLowRange7.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	//	m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrHighRange7, this))
    {
		m_StrMessageLbl = "Please enter High Range7";	
		m_CntlHighRange7.SetFocus();
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}

		nLowRange = atoi(m_StrLowRange7);
		nHighRange = atoi(m_StrHighRange7);           
			
		if ( nHighRange < nLowRange)
		{
			m_StrMessageLbl = "ERROR: High Range7 cannot be less than or equal to  Low Range7";	
			m_CntlHighRange7.SetFocus();
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
				//m_nDialogMode = MODE_DONE;
				//return FALSE;
			return;
		}			

			m_CntlLowRange8.EnableWindow(TRUE);
			m_CntlHighRange8.EnableWindow(TRUE);
			//m_CntlLowRange8.SetFocus();	
			//return TRUE;
			return;
}

//BOOL CBinMCCBlocking::OnKillfocusHighRange8() 
void CBinMCCBlocking::OnKillfocusHighRange8() 
{
	
	CMyUtils myUtilsObject;
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_StrMessageLbl.Empty();

	//if both low and high are empty during edit, don't error
	if(m_nDialogMode == MODE_EDIT && m_StrLowRange8.IsEmpty() && m_StrHighRange8.IsEmpty())
		//return TRUE;
		return;

    if (myUtilsObject.IsEmpty (m_StrLowRange8, this))
    {
		m_StrMessageLbl = "Please enter Low Range8";	
		m_CntlLowRange8.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrHighRange8, this))
    {
		m_StrMessageLbl = "Please enter High Range8";	
		m_CntlHighRange8.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}
	
			nLowRange = atoi(m_StrLowRange8);
			nHighRange = atoi(m_StrHighRange8);
			if ( nHighRange < nLowRange)
			{
				m_StrMessageLbl = "ERROR: High Range8 cannot be less than or equal to  Low Range8";	
				m_CntlHighRange8.SetFocus();
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
			//	m_nDialogMode = MODE_DONE;
				//return FALSE;
				return;
			}		

			m_CntlLowRange9.EnableWindow(TRUE);
			m_CntlHighRange9.EnableWindow(TRUE);
			//m_CntlLowRange9.SetFocus();	
			//return TRUE;
			return;
}

//BOOL CBinMCCBlocking::OnKillfocusHighRange9() 
void CBinMCCBlocking::OnKillfocusHighRange9()
{
	
	CMyUtils myUtilsObject;
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_StrMessageLbl.Empty();

	//if both low and high are empty during edit, don't error
	if(m_nDialogMode == MODE_EDIT && m_StrLowRange9.IsEmpty() && m_StrHighRange9.IsEmpty())
		//return TRUE;
		return;

    if (myUtilsObject.IsEmpty (m_StrLowRange9, this))
    {
		m_StrMessageLbl = "Please enter Low Range9";	
		m_CntlLowRange9.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrHighRange9, this))
    {
		m_StrMessageLbl = "Please enter High Range9";	
		m_CntlHighRange9.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}
	
			nLowRange = atoi(m_StrLowRange9);
			nHighRange = atoi(m_StrHighRange9);
			if ( nHighRange < nLowRange)
			{
				m_StrMessageLbl = "ERROR: High Range9 cannot be less than or equal to Low Range9";	
				m_CntlHighRange9.SetFocus();
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				//m_nDialogMode = MODE_DONE;
				//return FALSE;
				return;
			}
		
		

				m_CntlLowRange10.EnableWindow(TRUE);
				m_CntlHighRange10.EnableWindow(TRUE);
				//m_CntlLowRange10.SetFocus();
				//return TRUE;
}

//BOOL CBinMCCBlocking::OnKillfocusHighRange10() 
void CBinMCCBlocking::OnKillfocusHighRange10() 
{
	
	CMyUtils myUtilsObject;
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_StrMessageLbl.Empty();

	//if both low and high are empty during edit, don't error
	if(m_nDialogMode == MODE_EDIT && m_StrLowRange10.IsEmpty() && m_StrHighRange10.IsEmpty())
		//return TRUE;
		return;

    if (myUtilsObject.IsEmpty (m_StrLowRange10, this))
    {
		m_StrMessageLbl = "Please enter Low Range10";	
		m_CntlLowRange10.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	//	m_nDialogMode = MODE_DONE;
		//return FALSE ;
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrHighRange10, this))
    {
		m_StrMessageLbl = "Please enter High Range10";	
		m_CntlHighRange10.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		//return FALSE;
		return;
	}
     
			nLowRange = atoi(m_StrLowRange10);
			nHighRange = atoi(m_StrHighRange10);
			if ( nHighRange < nLowRange)
			{
				m_StrMessageLbl = "ERROR: High Range10 cannot be less than or equal to Low Range10";	
				m_CntlHighRange10.SetFocus();
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				//m_nDialogMode = MODE_DONE;
				//return FALSE;
				return;
			}		

			//return TRUE;
			return;
	
}



BOOL CBinMCCBlocking::CheckDialogForCompleteness()
{

	CMyUtils myUtilsObject;

	char strErrorMessage [200] = "";

//	ZeroMemory ( &sAuthRecord, sizeof (CAF01));

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	//if both low and high are empty during edit, don't error
	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrLowRange1.IsEmpty() && m_StrHighRange1.IsEmpty())
		return TRUE;

	if (myUtilsObject.IsEmpty (m_StrHighRange1, this))
    {
		m_StrMessageLbl = "Please enter High Range1";	
		m_CntlLowRange1.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		return FALSE;
		
	}	
	nLowRange = atoi(m_StrLowRange1);
	nHighRange = atoi(m_StrHighRange1);           
			
	if ( nHighRange < nLowRange)
	{
		m_StrMessageLbl = "ERROR: High Range1 cannot be less than or equal to Low Range1";	
		m_CntlLowRange1.SetFocus();
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			
	//	m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrLowRange2.IsEmpty() && m_StrHighRange2.IsEmpty())
		return TRUE;

	if (myUtilsObject.IsEmpty (m_StrHighRange2, this))
    {
		m_StrMessageLbl = "Please enter High Range2";	
		m_CntlLowRange2.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		return FALSE;
		
	}	
	nLowRange = atoi(m_StrLowRange2);
	nHighRange = atoi(m_StrHighRange2);           
			
	if ( nHighRange < nLowRange)
	{
		m_StrMessageLbl = "ERROR: High Range2 cannot be less than or equal to Low Range2";	
		m_CntlLowRange2.SetFocus();
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			
	//	m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrLowRange3.IsEmpty() && m_StrHighRange3.IsEmpty())
		return TRUE;

	if (myUtilsObject.IsEmpty (m_StrHighRange3, this))
    {
		m_StrMessageLbl = "Please enter High Range3";	//finish putting in correct range 
		m_CntlLowRange3.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		return FALSE;
		
	}	
	nLowRange = atoi(m_StrLowRange3);
	nHighRange = atoi(m_StrHighRange3);           
			
	if ( nHighRange < nLowRange)
	{
		m_StrMessageLbl = "ERROR: High Range3 cannot be less than or equal to Low Range3";	
		m_CntlLowRange3.SetFocus();
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			
	//	m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrLowRange4.IsEmpty() && m_StrHighRange4.IsEmpty())
		return TRUE;

	if (myUtilsObject.IsEmpty (m_StrHighRange4, this))
    {
		m_StrMessageLbl = "Please enter High Range4";	
		m_CntlLowRange4.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		return FALSE;
		
	}	
	nLowRange = atoi(m_StrLowRange4);
	nHighRange = atoi(m_StrHighRange4);           
			
	if ( nHighRange < nLowRange)
	{
		m_StrMessageLbl = "ERROR: High Range4 cannot be less than or equal to Low Range4";	
		m_CntlLowRange4.SetFocus();
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			
	//	m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrLowRange5.IsEmpty() && m_StrHighRange5.IsEmpty())
		return TRUE;

	if (myUtilsObject.IsEmpty (m_StrHighRange5, this))
    {
		m_StrMessageLbl = "Please enter High Range5";	
		m_CntlLowRange5.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		return FALSE;
		
	}	
	nLowRange = atoi(m_StrLowRange5);
	nHighRange = atoi(m_StrHighRange5);           
			
	if ( nHighRange < nLowRange)
	{
		m_StrMessageLbl = "ERROR: High Range5 cannot be less than or equal to Low Range5";	
		m_CntlLowRange5.SetFocus();
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			
	//	m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrLowRange6.IsEmpty() && m_StrHighRange6.IsEmpty())
		return TRUE;

	if (myUtilsObject.IsEmpty (m_StrHighRange6, this))
    {
		m_StrMessageLbl = "Please enter High Range6";	
		m_CntlLowRange6.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		return FALSE;
		
	}	
	nLowRange = atoi(m_StrLowRange6);
	nHighRange = atoi(m_StrHighRange6);           
			
	if ( nHighRange < nLowRange)
	{
		m_StrMessageLbl = "ERROR: High Range6 cannot be less than or equal to Low Range6";	
		m_CntlLowRange6.SetFocus();
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			
	//	m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrLowRange7.IsEmpty() && m_StrHighRange7.IsEmpty())
		return TRUE;

	if (myUtilsObject.IsEmpty (m_StrHighRange7, this))
    {
		m_StrMessageLbl = "Please enter High Range7";	
		m_CntlLowRange7.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		return FALSE;
		
	}	
	nLowRange = atoi(m_StrLowRange7);
	nHighRange = atoi(m_StrHighRange7);           
			
	if ( nHighRange < nLowRange)
	{
		m_StrMessageLbl = "ERROR: High Range7 cannot be less than or equal to Low Range7";	
		m_CntlLowRange7.SetFocus();
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			
	//	m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrLowRange8.IsEmpty() && m_StrHighRange8.IsEmpty())
		return TRUE;

	if (myUtilsObject.IsEmpty (m_StrHighRange8, this))
    {
		m_StrMessageLbl = "Please enter High Range8";	
		m_CntlLowRange8.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		return FALSE;
		
	}	
	nLowRange = atoi(m_StrLowRange8);
	nHighRange = atoi(m_StrHighRange8);           
			
	if ( nHighRange < nLowRange)
	{
		m_StrMessageLbl = "ERROR: High Range8 cannot be less than or equal to Low Range8";	
		m_CntlLowRange8.SetFocus();
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			
	//	m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrLowRange9.IsEmpty() && m_StrHighRange9.IsEmpty())
		return TRUE;

	if (myUtilsObject.IsEmpty (m_StrHighRange9, this))
    {
		m_StrMessageLbl = "Please enter High Range9";	
		m_CntlLowRange9.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		return FALSE;
		
	}	
	nLowRange = atoi(m_StrLowRange9);
	nHighRange = atoi(m_StrHighRange9);           
			
	if ( nHighRange < nLowRange)
	{
		m_StrMessageLbl = "ERROR: High Range9 cannot be less than or equal to Low Range9";	
		m_CntlLowRange9.SetFocus();
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			
	//	m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	if((m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD) && m_StrLowRange10.IsEmpty() && m_StrHighRange10.IsEmpty())
		return TRUE;

	if (myUtilsObject.IsEmpty (m_StrHighRange10, this))
    {
		m_StrMessageLbl = "Please enter High Range10";	
		m_CntlLowRange10.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		//m_nDialogMode = MODE_DONE;
		return FALSE;
		
	}	
	nLowRange = atoi(m_StrLowRange10);
	nHighRange = atoi(m_StrHighRange10);           
			
	if ( nHighRange < nLowRange)
	{
		m_StrMessageLbl = "ERROR: High Range10 cannot be less than or equal to Low Range10";	
		m_CntlLowRange10.SetFocus();
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			
	//	m_nDialogMode = MODE_DONE;
		return FALSE;
	}


    return TRUE;

}


/********************************************************************/
