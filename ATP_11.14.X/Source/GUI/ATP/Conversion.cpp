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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\Conversion.cpp  $
   
      Rev 1.7   Dec 01 2003 11:29:10   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 05 2000 14:53:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:47:22   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:14   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:20   iarustam
   Initial Release
   
      Rev 1.3   May 10 1999 11:12:08   dperson
   Standardized error messages
   
      Rev 1.2   Apr 29 1999 14:56:12   iarustam
   Bug fixes
   
      Rev 1.1   Mar 22 1999 11:12:16   iarustam
   Bug fixes
   
   
      Rev 1.0   Feb 23 1999 11:40:16   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:18   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:26   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:36   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:02   iarustam
    
*
************************************************************************************/
// Conversion.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "Conversion.h"
#include "SharedMem.h"

extern CSharedMem shrmem;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConversion property page

IMPLEMENT_DYNCREATE(CConversion, CPropertyPage)

CConversion::CConversion() : CPropertyPage(CConversion::IDD)
{
	//{{AFX_DATA_INIT(CConversion)
	m_StrCurrencyCodeFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrConversionRateFld = _T("");
	m_StrNameFld = _T("");
	//}}AFX_DATA_INIT
}

CConversion::~CConversion()
{
}

void CConversion::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConversion)
	DDX_Control(pDX, IDC_NAME, m_CntlNameFld);
	DDX_Control(pDX, IDC_CONVERSION_RATE_FLD, m_CntlConversionRateFld);
	DDX_Control(pDX, IDC_CURRENCY_CODE_FLD, m_CntlCurrencyCodeFld);
	DDX_Text(pDX, IDC_CURRENCY_CODE_FLD, m_StrCurrencyCodeFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CONVERSION_RATE_FLD, m_StrConversionRateFld);
	DDX_Text(pDX, IDC_NAME, m_StrNameFld);
	DDV_MaxChars(pDX, m_StrNameFld, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConversion, CPropertyPage)
	//{{AFX_MSG_MAP(CConversion)
	ON_EN_UPDATE(IDC_CONVERSION_RATE_FLD, OnUpdateConversionRateFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConversion message handlers
BOOL CConversion::OnInitDialog() 
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

	//SetModified ( FALSE );

		if (m_nDialogMode != MODE_DONE )
    {
		if (m_nDialogMode != MODE_VIEW)
        EnableConvertCodeDialog(TRUE);
        if ( m_nDialogMode != MODE_ADD )
        PopulateDialogFromDatabase ();
    }    
   m_bConversion = TRUE;
	m_StrConversionRateFld = "0000.0000";
	m_StrMessageLbl = "";
   m_StrNameFld = "";
	
	mystring = m_StrConversionRateFld;

	UpdateData(FALSE);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/**************************************************************/
 //////////////////////////////////////////////////////////////
/**************************************************************/

BOOL CConversion::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    if (( m_nDialogMode != MODE_VIEW) && (m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

/**************************************************************/
 //////////////////////////////////////////////////////////////
/**************************************************************/

BOOL CConversion::PopulateDialogFromDatabase()
{
	if ( m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_VIEW )
	{
		EnableConvertCodeDialog(TRUE );
   }
        	
	if (!UpdateData(FALSE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	return TRUE;
}

/**************************************************************/
 //////////////////////////////////////////////////////////////
/**************************************************************/

void CConversion::EnableConvertCodeDialog(int nStatus)
{
	if ( m_nDialogMode != MODE_VIEW )
		m_CntlConversionRateFld.EnableWindow(nStatus);

}

/**************************************************************/
 //////////////////////////////////////////////////////////////
/**************************************************************/

void CConversion::CleanConvertCodeDialog()
{
   m_StrCurrencyCodeFld = "";
	m_StrConversionRateFld = "0000.0000";
   m_StrNameFld = "";
	m_StrMessageLbl = "";
   UpdateData(FALSE);
}




void CConversion::OnUpdateConversionRateFld() 
{
	int count,i;
	CString val;
	CString a,b,c,d;
	CString strNew, strCurrent;
	BOOL bFlag;
	int len, len1;

	bFlag = FALSE;
	UpdateData(TRUE);

	len = m_StrConversionRateFld.GetLength();
	if ( len == 1)
	{
		m_StrConversionRateFld = "0000.000" + m_StrConversionRateFld;
		mystring = m_StrConversionRateFld;
		UpdateData(FALSE);
		return;
	}

	else if ( len >1 && len <9)
	{
		m_StrMessageLbl = "ERROR: Invalid data";
		m_StrConversionRateFld = mystring;
		UpdateData(FALSE);
		return;

	}

	val = m_StrConversionRateFld;

	if (strcmp ( mystring, m_StrConversionRateFld) == 0)
		return;

	a = mystring;
	b = m_StrConversionRateFld;

	for  ( int i = 0; i <9; i++ )
		{
			c = a.Left(1);
			d = b.Left(1);
			len = a.GetLength();
			len1 = b.GetLength();
			if ( c == d)
			{
				if ( len == 1)
				{
					a = a.Right(1);
					b = b.Right(1);
			//		strCurrent = strCurrent + d;
				}
				else
				{
					a = a.Right(len-1);
					b = b.Right(len1-1);
			//		strCurrent = strCurrent + d;
				}				
			}
			else 
			{
				strNew = d;
				a = a.Right(len-1);
				b = b.Right(len1-2);
				bFlag = TRUE;
			}
		}

	if ( !bFlag )
		strNew = b;
	int res = strspn ( strNew, "0123456789");

	if ( res == 0 )
	{
		m_StrMessageLbl = "ERROR: Invalid data";
		m_StrConversionRateFld = mystring;
		UpdateData(FALSE);
		return;
	}

	a = mystring.Left(1);

	if ( a != "0")
		{
		m_StrMessageLbl = "ERROR: Invalid data";
		m_StrConversionRateFld = mystring;
		UpdateData(FALSE);
		return;
	}

	val = mystring;

	for ( i =0; i<8; i++)
	{
		len = val.GetLength();
		val = val.Right(len-1);
		a = val.Left(1);

		if ( a != ".")
			strCurrent = strCurrent + a;		
	}

	strCurrent = strCurrent + strNew;
	a = "";

	for ( i = 0; i < 4; i++ )
	{
		len = strCurrent.GetLength();
		a = a + strCurrent.Left(1);
		strCurrent = strCurrent.Right(len-1);
	}

	a = a + ".";

for ( i = 0; i < 4; i++ )
	{
		len = strCurrent.GetLength();
		a = a + strCurrent.Left(1);
		strCurrent = strCurrent.Right(len-1);
	}



	m_StrConversionRateFld = a;
	mystring = m_StrConversionRateFld;
	m_StrMessageLbl = "";


	UpdateData(FALSE); 
	
}

BOOL CConversion::CheckDialogForCompleteness() 
{
/*	CString a,c, val;
	int len;
		a = m_StrConversionRateFld;
		val = m_StrConversionRateFld;

		len = m_StrConversionRateFld.GetLength();

		if (len < 9)
			{	
				m_StrMessageLbl = "ERROR: Invalid data";
				UpdateData(FALSE);
				m_StrConversionRateFld = val;
				return FALSE;
			}
		
		for  ( int i = 0; i <=9; i++ )
		{
			c = a.Left(1);
			if ( i<4 || i >4)
			{
				if (  strspn ( c, "0123456789") == 0)
				{	
					m_StrMessageLbl = "ERROR: Invalid data";
					UpdateData(FALSE);
					m_StrConversionRateFld = val;
					return FALSE;
				}
			}
								
				if ( i == 4 && c != ".")
				{	
					m_StrMessageLbl = "ERROR: Invalid data";
					UpdateData(FALSE);
					m_StrConversionRateFld = val;
					return FALSE;
				}
				
					len = a.GetLength();
					if ( len == 1)
						a = a.Right(1);
					else
						a = a.Right(len-1);
				
			}*/
		
return TRUE;
}

BOOL CConversion::OnSetActive() 
{
	if 	( m_nDialogMode == MODE_DONE || m_nDialogMode == MODE_VIEW )
	{
		m_CntlConversionRateFld.EnableWindow(FALSE);
      m_CntlNameFld.EnableWindow(FALSE);
	}
	else if ( m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT )
	{
		m_CntlConversionRateFld.EnableWindow(TRUE);
      m_CntlNameFld.EnableWindow(TRUE);
	}   

	
	return CPropertyPage::OnSetActive();
}
