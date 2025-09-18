// DBFToolWindowsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DBFToolWindows.h"
#include "DBFToolWindowsDlg.h"
#include "dbf.h"
extern "C" {
#include "ptedes.h"
WORD key[4] = {0x13A7, 0x6135, 0x9CDF, 0xA852};
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBFToolWindowsDlg dialog

CDBFToolWindowsDlg::CDBFToolWindowsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDBFToolWindowsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDBFToolWindowsDlg)
	m_szDbfName = _T("");
	m_szDesKey = _T("");
	m_szStatus = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_szFileExt = _T("");
}

void CDBFToolWindowsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDBFToolWindowsDlg)
	DDX_Control(pDX, IDC_GET_VERSION, m_CntlGetVersion);
	DDX_Control(pDX, IDOK, m_CntlDecrypt);
	DDX_Control(pDX, IDC_OPENFILE, m_CntlOpenFile);
	DDX_Control(pDX, IDC_KEY_ENC, m_CntlIsKeyEcnrypted);
	DDX_Control(pDX, IDC_STATUS, m_CntlStatus);
	DDX_Control(pDX, IDC_DES_KEY, m_CntlDesKey);
	DDX_Control(pDX, IDC_DBF_FILE, m_CntlDbfName);
	DDX_Text(pDX, IDC_DBF_FILE, m_szDbfName);
	DDX_Text(pDX, IDC_DES_KEY, m_szDesKey);
	DDX_Text(pDX, IDC_STATUS, m_szStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDBFToolWindowsDlg, CDialog)
	//{{AFX_MSG_MAP(CDBFToolWindowsDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_CHANGE(IDC_DES_KEY, OnChangeDesKey)
	ON_BN_CLICKED(IDC_OPENFILE, OnOpenfile)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_GET_VERSION, OnGetVersion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBFToolWindowsDlg message handlers

BOOL CDBFToolWindowsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	m_CntlOpenFile.ShowWindow(SW_HIDE);
	m_CntlStatus.SetWindowText("Ready");
	CString szTitle;
	this->GetWindowText(szTitle);
	CString appProdVer;
	appProdVer.LoadString(IDS_VERSION);
	szTitle += " - " + appProdVer;
	this->SetWindowText(szTitle);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDBFToolWindowsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDBFToolWindowsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDBFToolWindowsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDBFToolWindowsDlg::OnBrowse() 
{
	CFileDialog ofd(TRUE, NULL,NULL, OFN_HIDEREADONLY | OFN_ENABLESIZING , "TXT Files (*.txt) | *.txt|DBF files (*.dbf) | *.dbf|",this);
	m_CntlOpenFile.ShowWindow(SW_HIDE);
	if(ofd.DoModal() == IDOK)
	{
		m_szDbfName = ofd.GetPathName();
		m_szFileExt = ofd.GetFileExt();
		
	}
	UpdateData(FALSE);	
}

void CDBFToolWindowsDlg::OnOK() 
{
	CString msg;
	UpdateData();
	CString szDecDesKey;
	if(m_szDbfName.IsEmpty())
	{
		m_CntlStatus.SetWindowText("Please provide a dbf file");
		return;
	}
	if(m_szDesKey.IsEmpty())
	{
		m_CntlStatus.SetWindowText("Please enter Key to decrypt");
		return;
	}
;

	if(m_CntlIsKeyEcnrypted.GetCheck())
	{
		int len = m_szDesKey.GetLength();
		DecryptPassword(m_szDesKey.GetBuffer(len), szDecDesKey.GetBuffer(32));

		m_szDesKey.ReleaseBuffer(len);
		szDecDesKey.ReleaseBuffer(32);
	}
	else
	{
		szDecDesKey = m_szDesKey;
	}

	msg.Format("Selected file %s",m_szDbfName);
	m_CntlStatus.SetWindowText(msg);
	m_CntlOpenFile.ShowWindow(SW_HIDE);
	int len = m_szDbfName.GetLength();
	
	int keylen = szDecDesKey.GetLength();

	int ret = 0;

	if(m_szFileExt.CompareNoCase("dbf") == 0)
	{
		ret = dbf_decrypt_file(m_szDbfName.GetBuffer(len), szDecDesKey.GetBuffer(keylen));
	}
	else if (m_szFileExt.CompareNoCase("txt") == 0)
	{
		ret = txt_decrypt_file(m_szDbfName.GetBuffer(len), szDecDesKey.GetBuffer(keylen));;
	}
	else
	{
		ret = FILE_INCOMPATIBLE;
	}

	if(ret > 0)
	{
		msg.Format("%s file sucessfully decrypted",m_szDbfName);
		m_CntlOpenFile.ShowWindow(SW_SHOW);
	}
	else
	{
		if(ret == VERSION_INCOMPATIBLE)
		{
			msg.Format("Decryption failed!!! Version incompatable");
		}
		else if(ret == FILE_INCOMPATIBLE)
		{
			msg.Format("Decryption failed!!! File incompatable");
		}
		else if(ret == ALEADY_DECRYPTED)
		{
			msg.Format("Decryption failed!!! File already decrypted");
		}
		else
			msg.Format("Decryption failed!!!");
	}
	m_CntlStatus.SetWindowText(msg);
	m_szDbfName.ReleaseBuffer(len);
	szDecDesKey.ReleaseBuffer(keylen);

}


void CDBFToolWindowsDlg::OnChangeDesKey() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	if(m_szDesKey.GetLength() > 0)
	{
		m_CntlDecrypt.EnableWindow(TRUE);
		m_CntlGetVersion.EnableWindow(TRUE);
	}
	else
	{
		m_CntlDecrypt.EnableWindow(FALSE);
		m_CntlGetVersion.EnableWindow(FALSE);
	}

	if(m_szDesKey.GetLength() > 8)
	{
		m_CntlIsKeyEcnrypted.SetCheck(TRUE);
	}
	else
	{
		m_CntlIsKeyEcnrypted.SetCheck(FALSE);
	}
	
}

void CDBFToolWindowsDlg::OnOpenfile() 
{

	HINSTANCE hInst = ShellExecute(0, "open", m_szDbfName,NULL, NULL ,SW_SHOWNORMAL);
}

HBRUSH CDBFToolWindowsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	

	return hbr;
}

void CDBFToolWindowsDlg::OnGetVersion() 
{
		CString msg;
	BYTE header[2200] = {0};
	
	UpdateData();
	int len = m_szDbfName.GetLength();

	if(len == 0)
	{
		msg.Format("Invalid file name");
	}
	else
	{
		char line[5120] = {0};
		CString szLine;
		FILE *fptr = fopen(m_szDbfName,"rb");
		if(fptr == NULL)
		{
			msg.Format("Unable to open dbf file");
		}
		else
		{
			//Last line will not definitely more than 100. This is will go to second last line. 
			//The total text in the last will is 75 chars. The total records number is the remaining 25 chars which in real time will not happen.
			fseek(fptr, -100, SEEK_END);
			memset(line, 0, sizeof(line));
			if(fgets(line, 5000, fptr) != NULL)
			{
				if(fgets(line, 5000, fptr) != NULL)
				{
					if(line[0] == '[')
					{
						
						szLine = line;
						int pos = szLine.Find('|', 0);
						int pos2 = szLine.Find('|', pos + 9);
						CString szVersion = szLine.Mid(pos +9, pos2 - (pos +9));
						szVersion.TrimRight();
						msg.Format("DBF File Version is '%s'",szVersion);
					}
				}
			}
		}
	}
	m_CntlStatus.SetWindowText(msg);
	
}
