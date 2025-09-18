// stipmgrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "stipmgr.h"
#include "stipmgrDlg.h"
#include "myutils.h"
#include "HelpSTIP.h"
#include "Loginfrm.h"

//#include "afxwin.h"

extern "C"
{
#include "TXDSAPI.h"
#include "ntutils.h"
#include "ptesystm.h"
#include "genutil.h"
#include "txutils.h"
#include "txtrace.h"
#include "app_info.h"
#include "ptetcp.h"
};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REPEAT_COUNTER		2
CHAR devds_queue_name[]="devdsA";
//CHAR repds_queue_name[]="repdsA";
CHAR netds_queue_name[]="netdsA";
//CHAR application_que_name[];

extern CString strUserName;
//extern char strFileRecord[20];
extern char server_name[56];

extern "C"
{
   #include "ntutils.h"

}
extern "C"
{
	//Krishna@TF, Contains Version number of the App
	extern CHAR  appProductVersion[MAX_APPVERSION_SIZE];
}
CString idle_time_val_str;

CSharedMem shrmem;
CLoginfrm loginfrm;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_appversion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual void OnOK1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_appversion = _T("");
	//}}AFX_DATA_INIT
	char temp_buff[50];
	sprintf(temp_buff, "ATP - STIPMGR. Version : %s", appProductVersion);
	m_appversion = temp_buff;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_STATIC_APPVERSION, m_appversion);
	DDV_MaxChars(pDX, m_appversion, 30);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStipmgrDlg dialog

CStipmgrDlg::CStipmgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStipmgrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStipmgrDlg)
	m_strCardNum = _T("");
	m_strMerchantId = _T("");
	m_strTranDate = _T("");
	m_strMessage_Lbl = _T("");
	m_chkResendAll = FALSE;
	m_strCounter = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pLock = new CDMLock("IdleLock");
}

CStipmgrDlg::~CStipmgrDlg()
{
	 delete pLock;
   pLock = NULL;
}

void CStipmgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStipmgrDlg)
	DDX_Control(pDX, IDCANCEL, m_cntlCancle);
	DDX_Control(pDX, IDC_BUTTON_HELP, m_Help_Button);
	DDX_Control(pDX, IDC_BUTTON_NEXT10, m_cntlNext);
	DDX_Control(pDX, IDC_SEARCH_BUTTON, m_Search_Button);
	DDX_Control(pDX, IDC_EDIT_COUNTER, m_cntlCounter);
	DDX_Control(pDX, IDC_CHECK, m_cntlResendAll);
	DDX_Control(pDX, IDC_LIST_RESEND, m_cntlList);
	DDX_Control(pDX, IDC_COMBO1, m_CNBHostType);
	DDX_Control(pDX, IDC_BUTTON2, m_Resend_All_Button);
	DDX_Control(pDX, IDC_BUTTON1, m_Resend_Button);
	DDX_Control(pDX, IDC_EDIT3, m_cntlTranDate);
	DDX_Control(pDX, IDC_EDIT2, m_cntlMerchantId);
	DDX_Control(pDX, IDC_EDIT1, m_cntlCardNum);
	DDX_Text(pDX, IDC_EDIT1, m_strCardNum);
	DDV_MaxChars(pDX, m_strCardNum, 19);
	DDX_Text(pDX, IDC_EDIT2, m_strMerchantId);
	DDV_MaxChars(pDX, m_strMerchantId, 15);
	DDX_Text(pDX, IDC_EDIT3, m_strTranDate);
	DDX_Text(pDX, IDC_STATIC_LBL, m_strMessage_Lbl);
	DDX_Check(pDX, IDC_CHECK, m_chkResendAll);
	DDX_Text(pDX, IDC_EDIT_COUNTER, m_strCounter);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStipmgrDlg, CDialog)
	//{{AFX_MSG_MAP(CStipmgrDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeComboNetworkController)
	ON_EN_CHANGE(IDC_EDIT1, OnGetCardNumber)
	ON_EN_CHANGE(IDC_EDIT2, OnGetMerchantID)
	ON_EN_CHANGE(IDC_EDIT3, OnTranDate)
	ON_BN_CLICKED(IDC_BUTTON_RESEND, OnResendButton)
	ON_BN_CLICKED(IDC_BUTTON_RESEND_ALL, OnResendAllButton)
	ON_BN_CLICKED(IDC_BUTTON5, OnSearchButton)
	ON_EN_CHANGE(IDC_EDIT_COUNTER, OnChangeRepeatCounter)
	ON_BN_CLICKED(IDC_BUTTON_HELP, OnSTIPHelp)
	ON_BN_CLICKED(IDC_BUTTON_NEXT10, OnNext10Tx)
	ON_BN_CLICKED(IDOK, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStipmgrDlg message handlers

BOOL CStipmgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	CFont m_font;
    LOGFONT lf;
    int nHeader;
	char strHeaderData[14][50];
	LV_COLUMN lvColumn ;
	CString title_buffer,title_text;
	UCF01 sUCFRecord;
	int rcode;
	char strErrorMessage[260] = "";
	char ini_file[256];

	memset ( &lf, 0, sizeof (lf));
    lf.lfHeight = 13;
    lf.lfWeight = 700;
    strcpy(lf.lfFaceName, "Arial");
    m_font.CreateFontIndirect (&lf);
    GetDlgItem(IDC_STATIC_LBL)->SetFont (&m_font);	

		//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);



    memcpy (strHeaderData[0], "Card Number",    sizeof (strHeaderData[0]) );
    memcpy (strHeaderData[1], "Merchant ID",    sizeof (strHeaderData[1]) );
    memcpy (strHeaderData[2], "Device ID",      sizeof (strHeaderData[1]) );
    memcpy (strHeaderData[3], "Tran Date",      sizeof (strHeaderData[3]) );
    memcpy (strHeaderData[4], "Tran Time",      sizeof (strHeaderData[4]) );
    memcpy (strHeaderData[5], "Tran Amount",    sizeof (strHeaderData[5]) );
    memcpy (strHeaderData[6], "Sys Trace Nbr",  sizeof (strHeaderData[9]) );
	memcpy (strHeaderData[7], "Ret Ref Num",  sizeof (strHeaderData[9]) );
    memcpy (strHeaderData[8],"Transaction ID", sizeof (strHeaderData[11]));
//    memcpy (strHeaderData[8],"System Date",    sizeof (strHeaderData[12]));
    

    for (nHeader = 0; nHeader < 9; nHeader++)
    {
      lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
      lvColumn.fmt = LVCFMT_LEFT ;
      lvColumn.pszText = strHeaderData[nHeader] ;
      lvColumn.iSubItem = nHeader ;

      m_cntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
      m_cntlList.SetColumnWidth (0,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_cntlList.SetColumnWidth (1,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_cntlList.SetColumnWidth (2,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_cntlList.SetColumnWidth (3,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_cntlList.SetColumnWidth (4,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_cntlList.SetColumnWidth (5,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_cntlList.SetColumnWidth (6,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_cntlList.SetColumnWidth (7,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_cntlList.SetColumnWidth (8,  LVSCW_AUTOSIZE_USEHEADER) ;
	  m_cntlList.SetColumnWidth (9,  LVSCW_AUTOSIZE_USEHEADER) ;
      
   }
	/*Set title bar with site and host id*/
	GetWindowText(title_text);
	title_buffer.Format("%s  (%s)",title_text,server_name);
	SetWindowText(title_buffer);

	m_cntlCardNum.EnableWindow(false);
	m_cntlMerchantId.EnableWindow(false);
	m_cntlTranDate.EnableWindow(false);
	m_Search_Button.EnableWindow(false);
	m_Help_Button.EnableWindow(false);
	m_cntlNext.EnableWindow(false);
	m_cntlList.EnableWindow(false);
	m_Resend_Button.EnableWindow(false);
	m_Resend_All_Button.EnableWindow(false);

	// Get the access rights to Transaction Lookup screen
   memset ( &sUCFRecord, 0x00, sizeof(UCF01) );
   memcpy ( sUCFRecord.primary_key.name,
            strUserName,
            sizeof(sUCFRecord.primary_key.name) );

   rcode = txdsapi_get_record( (pBYTE)&sUCFRecord,
                                sizeof(UCF01),
                                UCF01_DATA,
                                strErrorMessage );

   if( PTEMSG_NOT_FOUND == rcode )
   {
      m_strMessage_Lbl = "Invalid User ID";
	  UpdateData(FALSE);
   }
   else if ( PTEMSG_INVALID_DATATYPE == rcode )
   {
      m_strMessage_Lbl = "Please try again";
      UpdateData(FALSE);	
   }
   else if( PTEMSG_OK != rcode )
   {
      m_strMessage_Lbl = strErrorMessage;
      UpdateData(FALSE);
   }

   else if( PTEMSG_OK == rcode )
   {
		m_cntlCardNum.EnableWindow(true);
		m_cntlMerchantId.EnableWindow(true);
		m_cntlTranDate.EnableWindow(true);
		m_Search_Button.EnableWindow(true);
		m_Help_Button.EnableWindow(true);
		m_cntlList.EnableWindow(true);

   }


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
	
	

	int CurrentIndex = -1;
	CurrentIndex = GetSelectedItem();
	CHAR       AppName [20];
	GetAppName( AppName );
	
	 //Krishna, 10.4.11 : Below changes are made for Idle time calculation, 
   //displaying the Login Screen & blocking other screens from editing.
   	GetAscendentConfigDirectory( ini_file );
	strcat( ini_file, "tf.ini" );

	//Get the Idle TIme from tf.ini
	GetPrivateProfileString(
        "GUI", 	                   // points to section name 
        "IDLE_TIMEOUT_VALUE",	   // points to key name 
        "600",					   // points to default string 
        idle_time_val,             // points to destination buffer 
        sizeof(idle_time_val) - 1, // size of destination buffer 
        ini_file );                // points to initialization filename 
		 
	    idle_time_val[sizeof(idle_time_val)] = 0; //Make sure string is terminated
		
		idle_time = atoi(idle_time_val);

	//Start the Timer
	timer_handle = StartTimer(1000);

    if (timer_handle == 0)
    {
          AfxMessageBox("Unable to obtain timer");
    }

   shrmem.CreateMutexLock();
   shrmem.CreateSharedMemorySegment();
   shrmem.MapSharedMemorySegment();

   //Krishna : Get the Current time & store it in SHM
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	ResetCntr();

//	m_cntlCardNum.SetFocus();
	return (false);  // return TRUE  unless you set the focus to a control
}

void CStipmgrDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CStipmgrDlg::OnPaint() 
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
HCURSOR CStipmgrDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CStipmgrDlg::ValidateTime ( CString strTime )
{
	CString strHH;
	CString strMM;
	int nHH, nMM;
	UpdateData(TRUE);
	CMyUtils myUtilsObject;

	if ( myUtilsObject.IsEmpty ( strTime, this ))
		return TRUE;

	int length = strTime.GetLength();	

	if ( length < 4 )
	{
		m_strMessage_Lbl = "Invalid Time Format";
		UpdateData(FALSE);
		return FALSE;
	}

	strHH = strTime.Left(2);
	strMM = strTime.Right(2);

	nHH = atoi ( strHH );
	nMM = atoi ( strMM );

	if ( nHH == 0 && nMM == 0 )
	{
		m_strMessage_Lbl = "ERROR:Invalid Data!";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nHH > 23 || nMM >59 )
	{
		m_strMessage_Lbl = "ERROR:Invalid Data!";
		UpdateData(FALSE);
		return FALSE;
	}

	return TRUE;

}

BOOL CStipmgrDlg::ValidateDate ( CString strDate )
{
	CString strDay;
	CString strMonth;
	CString strYear;
	CMyUtils myUtilsObject;

	int nDay, nMonth, nYear;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return TRUE;

	int length = strDate.GetLength();	
	int res = strspn ( strDate,"1234567890" );

	if ( length < 8 )
	{
		m_strMessage_Lbl = "Invalid Date Format";
		UpdateData(FALSE);
		return FALSE;
	}
	
	strDay = strDate.Left(2);
	strMonth = strDate.Mid(2,2);
	strYear = strDate.Right(4);

	nDay = atoi ( strDay );
	nMonth = atoi ( strMonth );
	nYear = atoi ( strYear );

	if ( nDay == 0 || nMonth == 0 || nYear ==0)
	{
		m_strMessage_Lbl = "ERROR:Invalid Data!";
		UpdateData(FALSE);
		return FALSE;
	}


	if ( nMonth > 12 )
	{
		m_strMessage_Lbl = "ERROR:Invalid Month!";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nMonth == 1 || nMonth == 3 || nMonth == 5 || 
		 nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12 )
	{
		if ( nDay > 31 )
		{
			m_strMessage_Lbl = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth == 4 || nMonth == 6 || nMonth == 9 || 
		 nMonth == 11 )
	{
		if ( nDay > 30 )
		{
			m_strMessage_Lbl = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth ==2 )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			m_strMessage_Lbl = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
		if (res == 0 && nDay >29)
		{
			m_strMessage_Lbl = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	return TRUE;
}



void CStipmgrDlg::OnSelchangeComboNetworkController() 
{
	// TODO: Add your control notification handler code here
    int hosttypeint = -1;
	m_strHostTypeSelected = _T(" ");

	// Get the NetworkControllers List from DataBase

	hosttypeint = m_CNBHostType.GetCurSel();

	if( hosttypeint == 0 )
		m_strHostTypeSelected = "ncvis2";
	else if( hosttypeint == 1 )
		m_strHostTypeSelected = "nposa2";
	else if( hosttypeint == 2 )
		m_strHostTypeSelected = "ncjcb2";
	else if( hosttypeint == 3 )
		m_strHostTypeSelected = "ncmcr2";
	else if( hosttypeint == 4 )
		m_strHostTypeSelected = "natma2";
	
}

void CStipmgrDlg::OnGetCardNumber() 
{
	m_cntlCardNum.GetWindowText(m_strCardNum);
	
	
}

void CStipmgrDlg::OnGetMerchantID() 
{
	m_cntlMerchantId.GetWindowText(m_strMerchantId);
}

void CStipmgrDlg::OnTranDate() 
{
	m_cntlTranDate.GetWindowText(m_strTranDate);
	
}


void CStipmgrDlg::PopulateTranListDialog()
{
	CFont m_font;
    LV_ITEM lvItem;
	char strErrorMessage[200];
	CString temp;
    char strTemp[20];
	
	bDone = FALSE;
	int nSize; 
	int nCount=0;
	int nRowItem = 0;
    int nActualItem = 0;
	nCount = m_cntlList.GetItemCount();
	CMyUtils myUtilsObject;

   	UpdateData(TRUE);
    m_strMessage_Lbl = "Your Request Is Being Processed, Please Wait...";
    UpdateData(FALSE);

	//Retrieve TLF01 table 

	int rcode = txdsapi_get_list ((pBYTE)&sTLFLimRecord,sizeof(TLF01_LIMITS), (pBYTE)&sTLFList, 
                                sizeof (TLF01_GUI_LIST),ST2_DB_SELECT_STANDIN,TLF01_DATA,strErrorMessage);


	if ( PTEMSG_OK == rcode )
	{
		nNumberReturned = atoi ((char *)&sTLFList->num_returned);
		if ( nNumberReturned == 0 )
		{
			m_strMessage_Lbl= "No records Found";
			bDone = TRUE;
            nSize = 0;
			UpdateData(FALSE);
		}
		
	else if( nNumberReturned == GUI_MAX_LIST_SIZE1)
      {				
         memcpy( sTLFLimRecord.transaction_id, (LPCTSTR) sTLFList->tlf01_structure[nNumberReturned-1].primary_key.transaction_id, sizeof (sTLFLimRecord.transaction_id)); 
         nSize = nNumberReturned - 1;
         m_strMessage_Lbl = "";
		 m_cntlNext.EnableWindow(true);
         UpdateData(FALSE);
      }
      else
      {
         m_strMessage_Lbl = "End of list";
         bDone = TRUE;
         nSize = nNumberReturned;
         UpdateData(FALSE);
		 m_cntlNext.EnableWindow(false);
      }

		for (int nItem = 0; nItem < nSize ; nItem++,nRowItem ++)
		{
			// will now insert the items and subitems into the list view
			for (int nSubItem = 0; nSubItem < 10; nSubItem++)
         {
            lvItem.mask = LVIF_TEXT ;
            lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
            lvItem.iSubItem = nSubItem;

            if (nSubItem == 0) { 
               lvItem.pszText = (char *)sTLFList->tlf01_structure[nItem].card_num;                   
            }
            else if (nSubItem == 1) { 
               lvItem.pszText = (char *)sTLFList->tlf01_structure[nItem].merchant_id;
            }
            else if (nSubItem == 2) { 
               lvItem.pszText = (char *)sTLFList->tlf01_structure[nItem].terminal_id;
            }
            else if (nSubItem == 3) { 
               if ( sTLFList->tlf01_structure[nItem].date_yyyymmdd[0] != 0x00 )
               {
   					temp = sTLFList->tlf01_structure[nItem].date_yyyymmdd;
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;                    
               }
               else
                  lvItem.pszText = " ";
            }
            else if (nSubItem == 4) { 
               if ( sTLFList->tlf01_structure[nItem].time_hhmmss[0] != 0x00 )
               {
                  temp = sTLFList->tlf01_structure[nItem].time_hhmmss;
                  temp = temp.Left(2) + ":" + temp.Mid(2,2);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;                    
               }
               else
                  lvItem.pszText = " ";
            }
            else if (nSubItem == 5) { 
               temp = sTLFList->tlf01_structure[nItem].total_amount;
               memcpy ( strTemp, myUtilsObject.DisplayAmount(temp), sizeof (strTemp));
               lvItem.pszText = strTemp ;
			}
            else if (nSubItem == 6) { 
               lvItem.pszText = (char *)sTLFList->tlf01_structure[nItem].sys_trace_audit_num;
            }
            else if (nSubItem == 7) { 
               lvItem.pszText = (char *)sTLFList->tlf01_structure[nItem].retrieval_ref_num;
            }
            else if (nSubItem == 8) { 
               lvItem.pszText = (char *)sTLFList->tlf01_structure[nItem].primary_key.transaction_id;
            }
            else if (nSubItem == 9) { 
               if ( sTLFList->tlf01_structure[nItem].system_date[0] != 0x00 )
               {
   					temp = sTLFList->tlf01_structure[nItem].system_date;
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;   
               }
               else
                  lvItem.pszText = " ";
            }


			if (nSubItem == 0) 
               nActualItem = m_cntlList.InsertItem(nCount+nSize,(char *)sTLFList->tlf01_structure[nItem].card_num);
            else 
               m_cntlList.SetItem(&lvItem);
         } // for
      } // for
      m_cntlList.SetFocus() ;
      int nCount = m_cntlList.GetItemCount();

      m_cntlList.EnsureVisible( nCount-1, TRUE);
		

	}
		else if ( PTEMSG_INVALID_DATATYPE == rcode )
	   {
		  m_strMessage_Lbl = "Please try again";
	   }
		else if( PTEMSG_OK != rcode )
	   {
		  m_strMessage_Lbl = strErrorMessage;
	   }

	   UpdateData(FALSE);

}

int CStipmgrDlg::GetSelectedItem()
{
	int selected= -1;
	POSITION pos = m_cntlList.GetFirstSelectedItemPosition();

	if ( pos != NULL )
	{
		while ( pos )
		{
			int nItem = m_cntlList.GetNextSelectedItem( pos );
			selected = nItem;
		}
	}
	return (selected);
}

//int CStipmgrDlg::OnResendButton() 
void CStipmgrDlg::OnResendButton()
{
	
	pPTE_MSG p_msg = NULL_PTR ; 
    pPTE_MSG p_msg_out;
    pPTE_MSG p_msg_in;
    pPTE_MSG_DATA   p_msg_data = NULL_PTR ;
    pBYTE           p_data = NULL_PTR ;   
    LONG     retCode = 0L ;
    CHAR     strError[512] = {0} ;
    CHAR     strTemp[512] = {0} ;
	CHAR  transaction_que_name[16];
	char  temp[10];
	CHAR			temp_str[100];
	int len=0;
	char error_msg[100];
	int CurrentIndex = -1;
	int IndexCount = 0;
	INT repeat_ctr =0;
	int nRowSelected = -1;

	
	memset(temp_str,0x00,sizeof(temp_str));
    IndexCount =   m_cntlList.GetSelectedCount();
	if ( IndexCount == 0 )
	{
		m_strMessage_Lbl="Please Select An Item From The List...";
		UpdateData(false);
		//return 0;
		return;
	}
	nRowSelected = m_cntlList.GetNextItem (-1, LVNI_SELECTED) ;

	if( nRowSelected == -1)
	{
		m_strMessage_Lbl="Please Select An Item From The List!";
		UpdateData(false);
		//return 0;
		return;
	}

	BeginWaitCursor();
	m_strMessage_Lbl = "Resending, Wait.....";

	BYTE buffer2[sizeof(AUTH_TX)+sizeof(TLF01)];

	strcpy(temp,m_strHostTypeSelected);
	strcat(temp,"A");
	strcpy(transaction_que_name,temp);
	
	// Get Repeat Counter from Dialog Box
	int length = m_strCounter.GetLength();
	if (length == 0)
	{
		repeat_ctr = REPEAT_COUNTER;
	}

	else
	{
		repeat_ctr = atoi(m_strCounter);
	}


	memcpy(sTLFList->tlf01_structure[nRowSelected].handler_queue,
			"applnkA",7);

	memcpy(sTLFList->tlf01_structure[nRowSelected].originator_queue,
			"stpmgr",7);

	memcpy(sTLFList->tlf01_structure[nRowSelected].orig_retrieval_ref_num,
			sTLFList->tlf01_structure[nRowSelected].retrieval_ref_num,
			sizeof(sTLFList->tlf01_structure[nRowSelected].retrieval_ref_num));

	memset(sTLFList->tlf01_structure[nRowSelected].retrieval_ref_num,0x00,
			sizeof(sTLFList->tlf01_structure[nRowSelected].retrieval_ref_num));

	memcpy(sTLFList->tlf01_structure[nRowSelected].orig_message,
			sTLFList->tlf01_structure[nRowSelected].message_type,
			sizeof(sTLFList->tlf01_structure[nRowSelected].message_type));

	memcpy(sTLFList->tlf01_structure[nRowSelected].message_type,"0120",4);

	memcpy(sTLFList->tlf01_structure[nRowSelected].response_code," ",2);

	strcpy(pcCHAR(sTLFList->tlf01_structure[nRowSelected].response_text)," ");

	sTLFList->tlf01_structure[nRowSelected].tx_key = AUTH_ADVICE;

	memcpy(buffer2,&sTLFList->tlf01_structure[nRowSelected],sizeof(TLF01));

	p_msg_out = ptemsg_build_msg(   MT_AUTH_REQUEST,	   
								    0,
									0,
									transaction_que_name,
									NULL_PTR,
									(pBYTE)buffer2,
									sizeof(buffer2),
									0 );   // Build Message

	if( p_msg_out == NULL_PTR )
	{
	   	strcpy( error_msg, "Insufficient memory to build request message" );
		/*return 0;*/
		return;
	}

	
	p_msg_in = ptetcp_send_receive(p_msg_out,5,temp_str ); 

	if( p_msg_in == NULL_PTR )
	{
		sprintf( error_msg, "Communication Error during request to %s: %s", transaction_que_name,temp_str );
		m_strMessage_Lbl="Error while sending ... Resending";
	}

	p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
	p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

	if (p_msg_in == NULL_PTR)
	{
		do 
			{
				memcpy(sTLFList->tlf01_structure[nRowSelected].message_type,"0121",4);
				memcpy(sTLFList->tlf01_structure[nRowSelected].response_code,"  ",2);
				sTLFList->tlf01_structure[nRowSelected].tx_key = AUTH_ADVICE_REPEAT;
				strcpy(pcCHAR(sTLFList->tlf01_structure[nRowSelected].response_text)," ");
				memcpy(buffer2,&sTLFList->tlf01_structure[nRowSelected],sizeof(TLF01));

				p_msg_out = ptemsg_build_msg(  MT_AUTH_REQUEST,	   
												0,
												0,
												transaction_que_name,
												NULL_PTR,
												(pBYTE)buffer2,
												sizeof(buffer2),
												0 );   // Build Message

				if( p_msg_out == NULL_PTR )
				{
	   				strcpy( error_msg, "Insufficient memory to build request message" );
					//return 0;
					return;
				}
				
				p_msg_in = ptetcp_send_receive(p_msg_out,5,temp_str ); 

				if( p_msg_in == NULL_PTR )
				{
					sprintf( error_msg, "Communication Error during request to %s: %s", transaction_que_name,temp_str );
				}
				repeat_ctr--;
			} while ( (repeat_ctr != 0) && ( p_msg_in == NULL_PTR)); // End of do, while
	} // End of if
	EndWaitCursor();		   
	if( p_msg_in == NULL_PTR )
	{
		m_strMessage_Lbl= "Communication Error During Resend Request";
		UpdateData(false);
		//return 0;
		return;
	}
	p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
	p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );


	BYTE buffer3[sizeof(AUTH_TX) + sizeof(TLF01)];
	memcpy(&sTlf01, p_data , sizeof(TLF01)+sizeof (AUTH_TX));

	if(strncmp((char *)sTlf01->response_code,"00",2) == 0)
	{
		memcpy(sTlf01->product_codes[1].code," ",sizeof(sTlf01->product_codes[1].code));
		memcpy(buffer3+sizeof(AUTH_TX),&sTlf01,sizeof(TLF01));

		p_msg_out = ptemsg_build_msg(  MT_DB_REQUEST,							
										ST1_DB_UPDATE,
										0,
										devds_queue_name,
										NULL_PTR,
										(pBYTE)&buffer3,
										sizeof(buffer3),
										TLF01_DATA );   // Build Message

		if (p_msg_out == NULL_PTR )
		{
			strcpy(error_msg,"Insufficient memory to build request message");
			//return 0 ;
			return;
		}

		p_msg_in = ptetcp_send_receive(p_msg_out,5,temp_str ); 

		if( p_msg_in == NULL_PTR )
		{
			sprintf( error_msg, "Communication Error during request to %s: %s", devds_queue_name,temp_str );
			m_strMessage_Lbl = "Update Database Failed";
			UpdateData(false);
			//return 0;
			return;
		}
		p_msg_data = ptemsg_get_pte_msg_data (p_msg_in); 
		p_data   = ptemsg_get_pte_msg_data_data (p_msg_data); 
		p_data   = p_data + sizeof(AUTH_TX);

		m_strMessage_Lbl = "Resend And Update Database Successfull";
		UpdateData(false);
	}
	else
	{
		m_strMessage_Lbl = sTlf01->response_text;
		UpdateData(false);
	}

	//return 1;	
	return;
}

//int CStipmgrDlg::OnResendAllButton() 
void CStipmgrDlg::OnResendAllButton() 
{
	pPTE_MSG p_msg = NULL_PTR ; 
    pPTE_MSG p_msg_out;
    pPTE_MSG p_msg_in;
    pPTE_MSG_DATA   p_msg_data = NULL_PTR ;
    pBYTE           p_data = NULL_PTR ;   
    LONG     retCode = 0L ;
    CHAR     strError[512] = {0} ;
    CHAR     strTemp[512] = {0} ;
	CHAR  transaction_que_name[16];
	CHAR  temp[10];
	CHAR			temp_str[100];
	m_chkResendAll= false;
	INT len=0;
	CHAR error_msg[100];
	INT CurrentIndex = -1;
	INT IndexCount = 0;
	INT i;
	INT         repeat_ctr =0;
	BYTE buffer3[sizeof(AUTH_TX) + sizeof(TLF01)];

	m_chkResendAll = m_cntlResendAll.GetCheck();
	if ( true != m_chkResendAll )
	{
		m_strMessage_Lbl ="Please select check box for Resend All";
		UpdateData(false);
		//return 0;
		return;
	}
	else
	{
		BeginWaitCursor();
		m_strMessage_Lbl = "Resending, Wait.....";

		BYTE buffer2[sizeof(AUTH_TX)+sizeof(TLF01)];
		m_strMessage_Lbl=" ";
		UpdateData(false);
		// Get Repeat Counter from Dialog Box
		int length = m_strCounter.GetLength();

		if ( length == 0)
		{
			repeat_ctr = REPEAT_COUNTER;
		}
		else
		{
			repeat_ctr = atoi(m_strCounter);
		}
		if ( nNumberReturned != 0 )
		{
			m_strMessage_Lbl = " ";
			for ( i=0; i < nNumberReturned; i++ )
			{
				memcpy(sTLFList->tlf01_structure[i].handler_queue,
						"applnkA",7);

				memcpy(sTLFList->tlf01_structure[i].originator_queue,
					   "stpmgr",7);

				memcpy(sTLFList->tlf01_structure[i].orig_retrieval_ref_num,
					   sTLFList->tlf01_structure[i].retrieval_ref_num,
					   sizeof(sTLFList->tlf01_structure[i].retrieval_ref_num));

				memset(sTLFList->tlf01_structure[i].retrieval_ref_num,0x00,
					   sizeof(sTLFList->tlf01_structure[i].retrieval_ref_num));

				memcpy(sTLFList->tlf01_structure[i].orig_message,
					   sTLFList->tlf01_structure[i].message_type,
					   sizeof(sTLFList->tlf01_structure[i].message_type));

				memcpy(sTLFList->tlf01_structure[i].message_type,"0120",4);

				memcpy(sTLFList->tlf01_structure[i].response_code," ",2);

				strcpy(pcCHAR(sTLFList->tlf01_structure[i].response_text)," ");

				sTLFList->tlf01_structure[i].tx_key = AUTH_ADVICE;

				memcpy(buffer2,&sTLFList->tlf01_structure[i],sizeof(TLF01));
				strcpy(temp,m_strHostTypeSelected);
				strcat(temp,"A");
				strcpy(transaction_que_name,temp);

				p_msg_out = ptemsg_build_msg(  MT_AUTH_REQUEST,				
												   0,
												   0,
												   transaction_que_name,
												   NULL_PTR,
												   (pBYTE)buffer2,
												   sizeof(buffer2),
												   0 );   // Build Message

				if( p_msg_out == NULL_PTR )
				{
	   				strcpy( error_msg, "Insufficient memory to build request message" );
					//return 0;
					return;
				}

				p_msg_in = ptetcp_send_receive(p_msg_out,5,error_msg ); 

				if( p_msg_in == NULL_PTR )
				{
					sprintf( error_msg, "Communication Error during request to %s: %s", transaction_que_name,temp_str );
					m_strMessage_Lbl = " Failed, Resending ....."; 
					UpdateData(false);
				}

				p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
				p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

				if (p_msg_in == NULL_PTR)
				{
					do 
					{

						memcpy(sTLFList->tlf01_structure[i].message_type,"0121",4);
						memcpy(sTLFList->tlf01_structure[i].response_code,"  ",2);
						sTLFList->tlf01_structure[i].tx_key = AUTH_ADVICE_REPEAT;
						strcpy(pcCHAR(sTLFList->tlf01_structure[i].response_text)," ");
						memcpy(buffer2,&sTLFList->tlf01_structure[i],sizeof(TLF01));

						p_msg_out = ptemsg_build_msg(  MT_AUTH_REQUEST,	   
														0,
														0,
														transaction_que_name,
														NULL_PTR,
														(pBYTE)buffer2,
														sizeof(buffer2),
														0 );   // Build Message

						if( p_msg_out == NULL_PTR )
						{
	   						strcpy( error_msg, "Insufficient memory to build request message" );
							//return 0;
							return;
						}
						p_msg_in = ptetcp_send_receive(p_msg_out,5,error_msg );

 						if( p_msg_in  == NULL_PTR )
						{
							sprintf( error_msg, "Communication Error during request to %s: %s", transaction_que_name,temp_str );
						}
						repeat_ctr--;
					} while ( (repeat_ctr != 0) && (p_msg_in == NULL_PTR )); // End of do, while
				}
				EndWaitCursor();
				if( p_msg_in == NULL_PTR )
				{
					m_strMessage_Lbl = "Communication Error During Resend All Request";
					UpdateData(false);
					//return 0;
					return;
				}
				p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
				p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

				memset(&sTlf01,0x00,sizeof(TLF01));
				memcpy(sTlf01, p_data , sizeof(TLF01));
				if(strncmp((char *)sTlf01->response_code,"00",2) == 0)
				{
					memcpy(sTlf01->product_codes[1].code," ",sizeof(sTlf01->product_codes[1].code));
					memcpy(buffer3+sizeof(AUTH_TX),&sTlf01,sizeof(TLF01));

					p_msg_out = ptemsg_build_msg(   MT_DB_REQUEST,							
													ST1_DB_UPDATE,
													0,
													devds_queue_name,
													NULL_PTR,
													(pBYTE)&buffer3,
													sizeof(buffer3),
													TLF01_DATA );   // Build Message

					if (p_msg_out == NULL_PTR )
					{
						strcpy(error_msg,"Insufficient memory to build request message");
						//return 0 ;
						return;
					}

					p_msg_in = ptetcp_send_receive(p_msg_out,5,temp_str ); 

					if( p_msg_in == NULL_PTR )
					{
						sprintf( error_msg, "Communication Error during request to %s: %s", devds_queue_name,temp_str );
						m_strMessage_Lbl = "Update Database Failed";
						UpdateData(false);
						//return 0;
						return;
					}
					p_msg_data = ptemsg_get_pte_msg_data (p_msg_in); 
					p_data   = ptemsg_get_pte_msg_data_data (p_msg_data); 
					p_data   = p_data + sizeof(AUTH_TX);
					m_strMessage_Lbl = "Database Update Successfull";
					UpdateData(false);
					p_data=NULL_PTR;
				}
				else
				{
					m_strMessage_Lbl = sTlf01->response_text;
					UpdateData(false);
				}
			} // End of for loop
		} // End of outer if 

	//return 1 ;	
		return;
	}	
}

void CStipmgrDlg::OnSearchButton() 
{
	
	CMyUtils  myUtilsObject;

   CString strCardNum    = ""; // 19
   CString strMerchantId = ""; // 15
   CString strTranDate   = ""; //  8
   CString strTranTime   = ""; //  6
   CString temp;
   int len=0;
   INT vlen = 0;
   m_cntlList.DeleteAllItems();

   ZeroMemory ( &sTLFLimRecord, sizeof ( TLF01_LIMITS ) );
   

   if ( m_strHostTypeSelected.GetLength() == 0)
   {
	   m_strMessage_Lbl="Please Select a Network Controller...";
	   m_CNBHostType.SetFocus();
	   UpdateData(false);
	   return ;
   }
	
   if(( m_strCardNum.GetLength() == 0) && ( m_strMerchantId.GetLength() == 0) 
	   && ( m_strTranDate.GetLength() == 0))
   {
	   m_strMessage_Lbl="Please Enter A Search Criteria...";
	   m_cntlCardNum.SetFocus();
	   UpdateData(false);
	   return ;
   }

      memcpy ( sTLFLimRecord.card_num_min,m_strCardNum,
              sizeof (sTLFLimRecord.card_num_min) );


      memcpy ( sTLFLimRecord.merchant_id_min,m_strMerchantId,
               sizeof (sTLFLimRecord.merchant_id_min) );


   /* ---- */
   /* DATE */
   /* ---- */
   if ( !(myUtilsObject.IsEmpty(m_strTranDate, this)) ) 
   {
	   if (!ValidateDate( m_strTranDate ))
	   {
		   m_cntlTranDate.SetFocus();
		   return;
	   }
      /* Reformat start date from DDMMYYYY into YYYYMMDD. */
      temp = m_strTranDate.Right(4) +
             m_strTranDate.Mid(2,2) +
             m_strTranDate.Left(2);

      memcpy ( sTLFLimRecord.date_yyyymmdd_start, temp,
               sizeof (sTLFLimRecord.date_yyyymmdd_start) );
   }

	memcpy(sTLFLimRecord.acquirer_id,m_strHostTypeSelected,sizeof(sTLFLimRecord.acquirer_id));
	  
	PopulateTranListDialog();

	m_Resend_Button.EnableWindow(true);
	m_Resend_All_Button.EnableWindow(true);


	return;
	
}

void CStipmgrDlg::OnChangeRepeatCounter() 
{
	m_cntlCounter.GetWindowText(m_strCounter);
	
}

void CStipmgrDlg::OnSTIPHelp() 
{
	CHelpSTIP dlg;
	dlg.DoModal();
	
}

void CStipmgrDlg::OnNext10Tx() 
{
	if(!bDone)
	{
		PopulateTranListDialog();
	}
	else
	{
		m_strMessage_Lbl = "End of list";
		m_cntlNext.EnableWindow(false);
		UpdateData(false);
	}
	
}

LRESULT CStipmgrDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
    {	

      case WM_TIMER:
		{
			cntr++;
			
			shm_time = shrmem.GetLastUpdateTime();

			if(shm_time > c_time)
			{
				c_time = shm_time;
				ResetCntr();
			}
			if ( cntr == idle_time/2 )
			{
				shrmem.SetIdleStatus(0);
			}
			//Check whether Max Idle Time has elapsed
			if(cntr >= idle_time)
			{
				ret = LockScreen();

				if(ret == -1 || ret == -2)
				{				
					::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
				}
			}
		}
		break;
	
		//On X button press of Dialog
		case WM_CLOSE:				
				
				StopTimer();
				//Delete the SHM segment & Delete Mutex Lock
				shrmem.DestroySharedMemorySegment();
				shrmem.DeleteMutexLock();
				
				break;

	 default:break;			
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

BOOL CStipmgrDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	ResetCntr();
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}


int CStipmgrDlg::StartTimer(int time)
{
	int handle = SetTimer (IDT_TIMER, time, NULL);  // Start the Timer

	return handle;
}

bool CStipmgrDlg::StopTimer()
{
	if (!KillTimer (timer_handle))	//Stop Timer	
	{
				::MessageBox(NULL,"Error While Killing the Timer","ShutEvt",1);	
		return false;
	}

	return true;
}

void CStipmgrDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
				
	//Delete the associated Mutex lock 
	shrmem.DeleteMutexLock();

	CDialog::OnCancel();
}


short CStipmgrDlg::LockScreen()
{
	int retcode = 0;

	CLoginfrm idleloginfrm;

	//Stop the Timer & Display Login Screen
	StopTimer();

	ResetCntr();

	if(pLock->Lock())
	{
		status = shrmem.GetIdleStatus();

		if(status == 0)
		{
			FlashWindow(true);

			AfxMessageBox("Session Timeout. Please Login");
			//::MessageBox(this->m_hWnd,"Session Timeout. Please Login","STIP Manager",MB_OK);
			
			ret = idleloginfrm.CheckLoginDetails();

			if(ret == CANCEL)
			{
				//Update SHM with IdleStatus as 2 meaning to Close all the Apps
				shrmem.SetIdleStatus(2);
				
				retcode = -1;			
			}
			else
			{	
				shrmem.SetIdleStatus(1);
				retcode =  1;
			}
		}
		else if (status == 1)
		{
				retcode = 0;
			// Continue. Password check was successful.
		}
		else if (status == 2)
		{
			retcode =  -1;			
		}
		else
		{
			retcode = -2;
			LogEvent("Unkown SHM Status",ERROR_MSG);
			// TODO:  Error case - log a message. Status is out of sync.
		}
		//After Login Validation is successful, release the Mutex Lock
		pLock->Unlock();
	}

	StartTimer(1000);

	return retcode;	
}

void CStipmgrDlg::ResetCntr()
{
	cntr = 0;
}

void CAboutDlg::OnOK1() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
