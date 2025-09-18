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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardStatRec.cpp  $
   
      Rev 1.9   Dec 01 2003 11:28:54   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   Jan 05 2000 14:53:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.3   Sep 30 1999 16:16:56   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:44:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:02   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:08   iarustam
   Initial Release
   
      Rev 1.4   Jun 11 1999 14:07:20   iarustam
   Bug fixes
   
   
      Rev 1.3   Jun 02 1999 16:22:58   iarustam
   Bug fixes
   
      Rev 1.2   May 10 1999 11:06:56   dperson
   Standardized error messages
   
      Rev 1.1   Apr 29 1999 14:56:00   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:06   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:10   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:00   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:02   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:36   iarustam
    
*
************************************************************************************/

// CardStatRec.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CardStatRec.h"
#include "SharedMem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CCardStatRec dialog


CCardStatRec::CCardStatRec(CWnd* pParent /*=NULL*/)
	: CDialog(CCardStatRec::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCardStatRec)
	m_strCardStat = _T("");
	m_strDescr = _T("");
	m_strISOResp = _T("");
	m_strMacValue = _T("");
	m_strTextMes = _T("");
	//}}AFX_DATA_INIT
}


void CCardStatRec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardStatRec)
	DDX_Control(pDX, IDC_COMMENTS, m_ctrlComEdit);
	DDX_Control(pDX, IDC_ISORESP, m_ctrlISOResp);
	DDX_Control(pDX, IDC_CARDSTAT, m_ctrlCardStat);
	DDX_Control(pDX, IDC_MAC_VALUE, m_ctrlMacValue);
	DDX_Text(pDX, IDC_CARDSTAT, m_strCardStat);
	DDV_MaxChars(pDX, m_strCardStat, 2);
	DDX_Text(pDX, IDC_DESCR, m_strDescr);
	DDV_MaxChars(pDX, m_strDescr, 14);
	DDX_Text(pDX, IDC_ISORESP, m_strISOResp);
	DDV_MaxChars(pDX, m_strISOResp, 2);
	DDX_Text(pDX, IDC_MAC_VALUE, m_strMacValue);
	DDV_MaxChars(pDX, m_strMacValue, 2);
	DDX_Text(pDX, IDC_TEXTMES, m_strTextMes);
	DDV_MaxChars(pDX, m_strTextMes, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardStatRec, CDialog)
	//{{AFX_MSG_MAP(CCardStatRec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardStatRec message handlers

BOOL CCardStatRec::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	m_ctrlComEdit.SetFont (&m_font);

	if (DialogMode == MODE_EDIT)
		m_ctrlCardStat.EnableWindow(FALSE);

	//if (DialogMode == MODE_ADD)
	//	m_ctrlISOResp.EnableWindow(FALSE);

		
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCardStatRec::OnOK() 
{
	CSF01 sCSFRecord;
	//char ErrMsg[300];

	UpdateData(TRUE);

	if (DialogMode == MODE_ADD && m_strCardStat.IsEmpty())
	{
		m_ctrlComEdit.SetWindowText("Please enter Card Status");
		m_ctrlCardStat.SetFocus();
		return;
	}

	if (DialogMode == MODE_EDIT && m_strISOResp.IsEmpty())
	{
		m_ctrlComEdit.SetWindowText("Please enter ISO Response");
		m_ctrlISOResp.SetFocus();
		return;
	}

	ZeroMemory (&sCSFRecord,sizeof(CSF01));
	lstrcpy ((LPTSTR) sCSFRecord.primary_key.status, m_strCardStat);
	lstrcpy ((LPTSTR) sCSFRecord.description,        m_strDescr);
	lstrcpy ((LPTSTR) sCSFRecord.mac_value,      m_strMacValue);
	lstrcpy ((LPTSTR) sCSFRecord.iso_resp_code,      m_strISOResp);
	lstrcpy ((LPTSTR) sCSFRecord.resp_text_msg,      m_strTextMes);

	//if (txdsapi_get_record((pBYTE)&sCSFRecord,sizeof(CSF01),CSF01_DATA,ErrMsg) == PTEMSG_OK)
	//{
	//	m_ctrlComEdit.SetWindowText("ERROR: Card Status already exists");
	//	return ;
	//}

	
	CDialog::OnOK();
}

BOOL CCardStatRec::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
