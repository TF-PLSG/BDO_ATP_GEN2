// TPSRMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TPSR_GUI.h"
#include "TPSRMainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTPSRMainDlg dialog

IMPLEMENT_DYNCREATE(CTPSRMainDlg, CPropertyPage)

CTPSRMainDlg::CTPSRMainDlg(CWnd* pParent /*=NULL*/)
: CPropertyPage(CTPSRMainDlg::IDD)
{
	//{{AFX_DATA_INIT(CTPSRMainDlg)
	m_StrSent = _T("0");
	m_StrReceived = _T("0");
	m_StrToolStatus = _T("");
	m_StrResRecv = _T("");
	m_StrTxnSent = _T("");
	//}}AFX_DATA_INIT
	m_DevIds.RemoveAll();
	m_bRunning = false;
	m_bShowConsole = false;
}


void CTPSRMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTPSRMainDlg)
	DDX_Control(pDX, IDC_SHOW_CONSOLE, m_CntlShowConsole);
	DDX_Control(pDX, IDC_RESRECV, m_CntlResRecv);
	DDX_Control(pDX, IDC_TXNSENT, m_CntlTxnSent);
	DDX_Control(pDX, IDC_STATIC_SENT, m_CntlStaticSent);
	DDX_Control(pDX, IDC_STATIC_RECV, m_CntlStaticRecv);
	DDX_Control(pDX, IDRESETCOUNTER, m_CntlResetCounters);
	DDX_Control(pDX, IDCANCEL, m_CntlStop);
	DDX_Control(pDX, IDRUN, m_CntlRun);
	DDX_Text(pDX, IDC_TOOL_STATUS, m_StrToolStatus);
	DDX_Text(pDX, IDC_RESRECV, m_StrResRecv);
	DDX_Text(pDX, IDC_TXNSENT, m_StrTxnSent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTPSRMainDlg, CPropertyPage)
//{{AFX_MSG_MAP(CTPSRMainDlg)
ON_BN_CLICKED(IDRUN, OnRun)
ON_BN_CLICKED(IDCANCEL, OnStop)
ON_BN_CLICKED(IDRESETCOUNTER, OnResetcounter)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_SHOW_CONSOLE, OnShowConsole)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTPSRMainDlg message handlers

BOOL CTPSRMainDlg::OnApply() 
{
	
	return CPropertyPage::OnApply();
}

void CTPSRMainDlg::OnRun() 
{
	char lpszHwnd[16] = {0};
	char cmd[512] = {0};
	char folder_name[15] = {0};
	CTime timeStamp = CTime::GetCurrentTime();
	tm osTime;  // A structure containing time elements.
	timeStamp.GetLocalTm(&osTime);

	sprintf(folder_name,"A%04d%02d%02d%02d%02d%02d",(osTime.tm_year+1900),(osTime.tm_mon+1),osTime.tm_mday,osTime.tm_hour,osTime.tm_min,osTime.tm_sec);
	sprintf(cmd ,"tpsrtool_win.exe %d %s",this->m_hWnd,folder_name);
	sprintf(lpszHwnd,"%d",this->m_hWnd);
	
	ZeroMemory( &m_si, sizeof(m_si) );
	m_si.cb = sizeof(m_si);
	ZeroMemory( &m_pi, sizeof(m_pi) );

		m_CntlResetCounters.EnableWindow(FALSE);
		m_CntlRun.EnableWindow(FALSE);
		m_CntlStop.EnableWindow(FALSE);
		m_StrToolStatus = "Initializing the transactions....";
		m_CntlResRecv.ShowWindow(SW_SHOW);
		m_CntlStaticRecv.ShowWindow(SW_SHOW);
		m_CntlStaticSent.ShowWindow(SW_SHOW);
		m_CntlTxnSent.ShowWindow(SW_SHOW);
		UpdateData(FALSE);
		m_bRunning = true;

	// Start the child process.
	if( !CreateProcess( NULL, // No module name (use command line).
		cmd,	// Command line.
		NULL,	// Process handle not inheritable.
		NULL,	// Thread handle not inheritable.
		FALSE,	// Set handle inheritance to FALSE.
		0,		// CREATE_NO_WINDOW, // No creation flags.
		NULL,	// Use parent's environment block.
		NULL,	// Use parent's starting directory.
		&m_si,	// Pointer to STARTUPINFO structure.
		&m_pi ) // Pointer to PROCESS_INFORMATION structure.
		)
	{
		AfxMessageBox( "CreateProcess failed." );
				m_CntlResetCounters.EnableWindow(TRUE);
				m_CntlRun.EnableWindow(TRUE);
				m_CntlStop.EnableWindow(FALSE);
				m_StrToolStatus = "Unable to start the tool";
				m_CntlResRecv.ShowWindow(SW_HIDE);
				m_CntlStaticRecv.ShowWindow(SW_HIDE);
				m_CntlStaticSent.ShowWindow(SW_HIDE);
				m_CntlTxnSent.ShowWindow(SW_HIDE);
				UpdateData(FALSE);
				m_bRunning = false;
	}
	else
	{
		m_CntlResetCounters.EnableWindow(FALSE);
		m_CntlRun.EnableWindow(FALSE);
		m_CntlStop.EnableWindow(TRUE);
		m_StrToolStatus = "TPSR Tool Started";
		m_CntlResRecv.ShowWindow(SW_SHOW);
		m_CntlStaticRecv.ShowWindow(SW_SHOW);
		m_CntlStaticSent.ShowWindow(SW_SHOW);
		m_CntlTxnSent.ShowWindow(SW_SHOW);
		UpdateData(FALSE);
		m_bRunning = true;
		
	}
}

void CTPSRMainDlg::OnStop() 
{
	DWORD exitCode;
	UpdateData(TRUE);
	if(GetExitCodeProcess(m_pi.hProcess,&exitCode))
	{
		if(exitCode == 259 )
		{	
			if(TerminateProcess(m_pi.hProcess,0))
			{
				m_CntlResetCounters.EnableWindow(TRUE);
				m_CntlRun.EnableWindow(TRUE);
				m_CntlStop.EnableWindow(FALSE);
				m_StrToolStatus = "Please check the Log folder for transaction details";
				UpdateData(FALSE);
				m_bRunning = false;
				
				m_StatisticsDlg.m_StrRecv = m_StrResRecv;
				m_StatisticsDlg.m_StrSent = m_StrTxnSent;
				m_StatisticsDlg.DoModal();
			}
			else
			{
				
				AfxMessageBox("Unable to terminate the tool");
			}
		}
		else
		{
			AfxMessageBox("Tool already terminated");
			m_CntlResetCounters.EnableWindow(TRUE);
			m_CntlRun.EnableWindow(TRUE);
			m_CntlStop.EnableWindow(FALSE);
			m_StrToolStatus = "Please check the Log folder for transaction details";
			UpdateData(FALSE);
			m_bRunning = false;
			m_StatisticsDlg.m_StrRecv = m_StrResRecv;
			m_StatisticsDlg.m_StrSent = m_StrTxnSent;
			m_StatisticsDlg.DoModal();
			
		}
	}
}

BOOL CTPSRMainDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_CntlResRecv.ShowWindow(SW_HIDE);
	m_CntlStaticRecv.ShowWindow(SW_HIDE);
	m_CntlStaticSent.ShowWindow(SW_HIDE);
	m_CntlTxnSent.ShowWindow(SW_HIDE);
	
	m_StrResRecv = "0";
	m_StrTxnSent = "0";
	UpdateData(FALSE);

	RegisterHotKey(this->m_hWnd, 100, MOD_CONTROL | MOD_ALT, 'S');

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CTPSRMainDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	int received=0;
	int sent=0;
	char strReceived[20] = {0};
	
	switch(message)
	{
	case WM_REPONSERECEIVED:
		
		received = GetDlgItemInt(IDC_RESRECV);
		received++;
		SetDlgItemInt(IDC_RESRECV,received);
		return true;
		
		break;
	case WM_TRANSACTIONSENT:
		
		sent = GetDlgItemInt(IDC_TXNSENT);
		sent++;
		SetDlgItemInt(IDC_TXNSENT,sent);
		return true;
		break;
	case WM_STAN:
		m_DevIds.Add(wParam);
		m_StanIds.Add(lParam);
		sent = GetDlgItemInt(IDC_TXNSENT);
		sent++;
		SetDlgItemInt(IDC_TXNSENT,sent);
		break;
	case WM_CONSOLE_HANDLE:
		m_hConsoleHandle = (HWND)wParam;
		break;
	case WM_HOTKEY:
		OnShowConsole();
		break;
	default:
		break;
	}	
	return CPropertyPage::DefWindowProc(message, wParam, lParam);
}


void CTPSRMainDlg::OnResetcounter() 
{
	
	SetDlgItemInt(IDC_TXNSENT,0);
	SetDlgItemInt(IDC_RESRECV,0);
}

void CTPSRMainDlg::OnCancel() 
{
	if(m_bRunning)
	{
		int dlgRet = this->MessageBox("Transactions are being sent. Do you want to stop the transactions?","Confirm",MB_YESNO);
		if(dlgRet == IDYES)
		{
			OnStop();
			CPropertyPage::OnCancel();
		}
		else
		{
			return;
		}
	}
	
	
}

void CTPSRMainDlg::OnExit() 
{
	OnCancel();
}

void CTPSRMainDlg::OnShowConsole() 
{

	if(m_hConsoleHandle != NULL)
	{
		if(m_bShowConsole)
		{
			::ShowWindow(m_hConsoleHandle, SW_HIDE);
			m_bShowConsole = false;
		}
		else
		{
			::ShowWindow(m_hConsoleHandle, SW_SHOW);
			m_bShowConsole = true;
		}
	}
}
