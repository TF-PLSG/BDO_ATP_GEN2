// hostsimDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hostsimc.h"
#include "hostsimDlg.h"
#include "serialc.h"
#include "tcpip.h"
#include "ptecom.h"

#define RCFILE "Response_Code.txt"
#define JCB_RESPONSE_CODE_LEN 2

PRIVATE CHAR   global_err_str[100];

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C"
{
extern void   MainProcessor(int);
extern void   EndProcess();
   
extern LOCAL_DM_CONFIG dm_config;
extern int connection_type;
extern int pbsn_tcpip_port;
extern int g_using_tpdu_prefix;
extern char RespCodeinTxt[4];
extern char AuthCodeinTxt[7];
extern char RRNinTxt[13];
}


/*public:*/
CString commtype("TCP/IP");
CString sPort("Com2");
CString sBaud("9600 ");
CString sByte("8");
CString sParity("none");
CString sStop("1");
CString tHost("DOES NOT APPLY      ");
CString tPort("1401");



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
// CHostsimDlg dialog

CHostsimDlg::CHostsimDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CHostsimDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CHostsimDlg)
	m_StrPortNumberFld = _T("");
	//}}AFX_DATA_INIT
   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
 
void CHostsimDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CHostsimDlg)
   DDX_Control(pDX, IDC_EDIT1, m_Message);
   DDX_Control(pDX, IDCANCEL, m_exit);
   DDX_Control(pDX, IDC_COMMSETUP, m_CommSetup);
   DDX_Control(pDX, IDC_COMMTYPE, m_commtype);
   DDX_Control(pDX, IDC_OFFLINE, m_Offline);
   DDX_Control(pDX, IDC_ONLINE, m_Online);
	DDX_Text(pDX, IDC_PORT_NBR_FLD, m_StrPortNumberFld);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHostsimDlg, CDialog)
   //{{AFX_MSG_MAP(CHostsimDlg)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_ABOUT, OnAbout)
   ON_BN_CLICKED(IDC_ONLINE, OnOnline)
   ON_BN_CLICKED(IDC_OFFLINE, OnOffline)
   ON_BN_CLICKED(IDC_COMMSETUP, OnCommsetup)
   ON_BN_CLICKED(IDCANCEL ,OnCancel)
   ON_CBN_SELCHANGE(IDC_COMMTYPE, OnSelchangeCommtype)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHostsimDlg message handlers

BOOL CHostsimDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Add "About..." menu item to system menu.

   // IDM_ABOUTBOX must be in the system command range.
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);
   CMenu* pSysMenu = GetSystemMenu(FALSE);
   CString strAboutMenu;
   strAboutMenu.LoadString(IDS_ABOUTBOX);
   if (!strAboutMenu.IsEmpty())
   {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
   }

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);       // Set big icon
   SetIcon(m_hIcon, FALSE);      // Set small icon
   
   // TODO: Add extra initialization here
   connection_type     = DM_PORTTYPE_TCP;
   g_using_tpdu_prefix = false; 
   pbsn_tcpip_port     = 1401;

   ZeroMemory(&dm_config, sizeof(dm_config));
   strcpy(dm_config.db_connection_type,"TCP/IP");
   strcpy(dm_config.db_host_name, tHost);
   dm_config.db_host_tcp_port = pbsn_tcpip_port;

   m_commtype.SetCurSel(1);
   m_commtype.GetLBText(1, m_commtypeSelected);
   m_Offline.EnableWindow(FALSE);

   m_Message.EnableWindow(FALSE);
   m_Message.SetWindowText(dm_config.db_connection_type);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHostsimDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHostsimDlg::OnPaint() 
{

/*    HWND hwndButton;
   HWND hwnd = GetActiveWindow();
hwndButton = CreateWindow(  
    "BUTTON",   // predefined class 
    "BLA",       // button text 
    WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles 
 
    // Size and position values are given explicitly, because 
    // the CW_USEDEFAULT constant gives zero values for buttons. 
    10,         // starting x position 
    10,         // starting y position 
    100,        // button width 
    100,        // button height 
    hwnd,       // parent window 
    NULL,       // No menu 
    (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
    NULL);      // pointer not needed 
 ShowWindow(hwndButton,SW_SHOW);*/
   
   
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
HCURSOR CHostsimDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

void CHostsimDlg::OnAbout() 
{
   
   CAboutDlg dlgAbout;
   dlgAbout.DoModal();     

}

void CHostsimDlg::OnOnline() 
{

	char filename[20] = {0};
	FILE *fResponse_Code;
	char valuefromRCfile[10] = {0};
	char valueofACfromfile[7]={0};
	char valueofRRNfromfile[13]={0};

	sprintf(filename,"%s",RCFILE);
   
   //establish_connection(); 
/* if (m_commtypeSelected=="Serial")
       dm_conn_successful = serial_convert((char *)DMConfig.db_com_port,
                                (char *)DMConfig.db_baud_rate,
                                (char *)DMConfig.db_byte_size,
                                (char *)DMConfig.db_parity,
                                (char *)DMConfig.db_stop_bits,
                                incoming_buf);
   else
        dm_conn_successful = tcp_convert(DMConfig.db_tcp_port, incoming_buf);

    if (dm_conn_successful==FALSE)
      return;  */
/**************************************************
    char buffer[1000];
   char bufff1[21]="bbbabbabbabbabbabbab";
   unsigned char* p1;
   unsigned char* p2;
   buffer[0]=0;
   buffer[1]=0;
   p1=(unsigned char*)&buffer[0];
    p2=(unsigned char*)&buffer[2];
   p_Add_Priv_Use_Data("29", bufff1, strlen(bufff1), p1, p2);
   p_Add_Priv_Use_Data("29", bufff1, strlen(bufff1), p1, p2);

************************************/
   
   m_commtype.EnableWindow(FALSE);
   m_Online.EnableWindow(FALSE);
   m_Offline.EnableWindow(TRUE);
   m_Offline.SetFocus();
    m_CommSetup.EnableWindow(FALSE);
   m_exit.EnableWindow(FALSE);
    BeginWaitCursor();
   SetUpComm();
   MainProcessor(connection_type);
    EndWaitCursor();

	fResponse_Code = fopen(filename,"r+");

	if(fResponse_Code == NULL)
	{
		sprintf( global_err_str, "Unable to open file - %s", filename  );
		AfxMessageBox(global_err_str,MB_ICONSTOP | MB_OK);
	}

	else
	{
		GetPrivateProfileString1(
		"RESPONSE_CODE",	            /*  points to section name  */
		"JCBRC",                   /*  points to key name  */
		"0",                     /*  points to default string  */
		valuefromRCfile,                         /*  points to destination buffer  */
		sizeof(valuefromRCfile) - 1,             /*  size of destination buffer  */
		filename                        /*  points to initialization filename  */
	   );
		
		strncpy(RespCodeinTxt,valuefromRCfile,3);
		sprintf( global_err_str, "Value of JCB Response Code is %s", RespCodeinTxt);
		AfxMessageBox(global_err_str,MB_ICONINFORMATION | MB_OK);



		GetPrivateProfileString1(
		"AUTH_CODE",	            /*  points to section name  */
		"JCBAC",                   /*  points to key name  */
		"0",                     /*  points to default string  */
		valueofACfromfile,                         /*  points to destination buffer  */
		sizeof(valueofACfromfile) - 1,             /*  size of destination buffer  */
		filename                        /*  points to initialization filename  */
	   );

		strncpy(AuthCodeinTxt,valueofACfromfile,6);
		sprintf( global_err_str, "Value of JCB Auth Code is %s", AuthCodeinTxt);
		AfxMessageBox(global_err_str,MB_ICONINFORMATION | MB_OK);

		GetPrivateProfileString1(
		"RRN",	            /*  points to section name  */
		"JCBRRN",                   /*  points to key name  */
		"0",                     /*  points to default string  */
		valueofRRNfromfile,                         /*  points to destination buffer  */
		sizeof(valueofRRNfromfile) - 1,             /*  size of destination buffer  */
		filename                        /*  points to initialization filename  */
	   );

		strncpy(RRNinTxt,valueofRRNfromfile,12);
		sprintf( global_err_str, "Value of JCB RRN is %s", RRNinTxt);
		AfxMessageBox(global_err_str,MB_ICONINFORMATION | MB_OK);

	}
/*
    BYTE msg[80];
    int len = 76;

    msg[0] = 0x60; msg[1] = 0x00; msg[2] = 0x04; msg[3] = 0xff; msg[4] = 0x00; msg[5] = 0x02;
    msg[6] = 0x00; msg[7] = 0x30; msg[8] = 0x20; msg[9] = 0x05; msg[10] = 0x80; msg[11] = 0x20;
    msg[12] = 0xc0; msg[13] = 0x00; msg[14] = 0x04; msg[15] = 0x00; msg[16] = 0x00; msg[17] = 0x00;
    msg[18] = 0x00; msg[19] = 0x00; msg[20] = 0x99; msg[21] = 0x99; msg[22] = 0x99; msg[23] = 0x99;
    msg[24] = 0x00; msg[25] = 0x36; msg[26] = 0x74; msg[27] = 0x00; msg[28] = 0x20; msg[29] = 0x00;
    msg[30] = 0x04; msg[31] = 0x00; msg[32] = 0x24; msg[33] = 0x34; msg[34] = 0x20; msg[35] = 0x11;
    msg[36] = 0x23; msg[37] = 0x45; msg[38] = 0x67; msg[39] = 0x89; msg[40] = 0x3d; msg[41] = 0x99;
    msg[42] = 0x12; msg[43] = 0x67; msg[44] = 0x89; msg[45] = 0x35; msg[46] = 0x33; msg[47] = 0x30;
    msg[48] = 0x30; msg[49] = 0x30; msg[50] = 0x30; msg[51] = 0x30; msg[52] = 0x34; msg[53] = 0x30;
    msg[54] = 0x31; msg[55] = 0x32; msg[56] = 0x37; msg[57] = 0x31; msg[58] = 0x32; msg[59] = 0x33;
    msg[60] = 0x34; msg[61] = 0x35; msg[62] = 0x36; msg[63] = 0x37; msg[64] = 0x38; msg[65] = 0x39;
    msg[66] = 0x30; msg[67] = 0x31; msg[68] = 0x00; msg[69] = 0x06; msg[70] = 0x34; msg[71] = 0x38;
    msg[72] = 0x34; msg[73] = 0x39; msg[74] = 0x32; msg[75] = 0x33;

    host_notify( msg, len );
*/

}

void CHostsimDlg::OnOffline() 
{
   m_commtype.EnableWindow(TRUE);
   m_Offline.EnableWindow(FALSE);
   m_Online.EnableWindow(TRUE);
   m_Online.SetFocus();
   m_CommSetup.EnableWindow(TRUE);
   m_exit.EnableWindow(TRUE);
   BeginWaitCursor();
   EndProcess();
   EndWaitCursor();
   
}

void CHostsimDlg::OnCommsetup() 
{
  
   if (m_commtypeSelected == "Serial")
   {
       Serialc dlgSerial;
      strcpy (dm_config.db_connection_type,"Serial");
      dlgSerial.DoModal(); 

   }
  else
   {
      strcpy (dm_config.db_connection_type,"TCP/IP");
      tcpip dlgTcpip;
      dlgTcpip.DoModal();     
	  m_StrPortNumberFld = dlgTcpip.m_StrPort;
	  UpdateData(FALSE);
   }
   
}

void CHostsimDlg::OnSelchangeCommtype() 
{
   int commtypeint = m_commtype.GetCurSel();

   m_commtype.GetLBText( commtypeint, m_commtypeSelected );
   strcpy (dm_config.db_connection_type,m_commtypeSelected);
   
   if (m_commtypeSelected == "Serial") 
      connection_type = DM_PORTTYPE_SERIAL;
   else
      connection_type = DM_PORTTYPE_TCP;
}

/*void CHostsimDlg::OnSelchangeCommtype() */

void CHostsimDlg::OnCancel() 
{
   // TODO: Add extra cleanup here
   
   CDialog::OnCancel();
}

void CHostsimDlg::SetUpComm() 
{
   // TODO: Add extra cleanup here
   if (connection_type == DM_PORTTYPE_TCP)
   {
      strcpy (dm_config.db_connection_type,"TCP/IP");
      strcpy (dm_config.db_host_name, tHost);
      pbsn_tcpip_port = atoi(tPort);
      dm_config.db_host_tcp_port = pbsn_tcpip_port;
      g_using_tpdu_prefix = false;
   }
   else
   {
      strcpy (dm_config.db_connection_type, "Serial"); 
      strcpy (dm_config.db_com_port,        sPort); 

      if (sBaud == "38400")
         strcpy(dm_config.db_baud_rate,"38400"); 
      else 
         if (sBaud == "19200")
            strcpy (dm_config.db_baud_rate,"19200");
         else
            strcpy (dm_config.db_baud_rate,"9600"); 

      strcpy (dm_config.db_byte_size, sByte); 

      if (sParity == "odd ")
         strcpy (dm_config.db_parity,"odd"); 
      else
         strcpy (dm_config.db_parity,sParity); 

      if (sStop == "1  ")
         strcpy (dm_config.db_stop_bits,"1"); 
      else 
         if (sStop == "2  ")
            strcpy (dm_config.db_stop_bits,"2");
         else
            strcpy (dm_config.db_stop_bits,"1.5"); 

      g_using_tpdu_prefix = true;
   }
return;
}

int CHostsimDlg::GetPrivateProfileString1(char * section_name,
                             char * key_name,
                             char * default_str,
                             char * dest_buffer,
                             UINT  buffer_size,
                             char * file_name)
{
    FILE *src_file;
    CHAR buffer[256], full_section_name[256];
    INT  found_sect, found_eq;
    UINT temp;
	INT i=0;
	char RespCodeBuffer[256] = {0}, RespCodeBuffer1[256] = {0};

    /* copy the default string to the destination buffer */
    strcpy( dest_buffer, default_str );
    sprintf( full_section_name, "[%s]", section_name);

    /* try to open the source file */
    src_file = fopen( file_name, "r+" );
    if( src_file == NULL )
    {
        sprintf( global_err_str, "Unable to open file - %s", file_name  );
        return 0;
    }

    /* file is open, go through each line of the file..... */
    found_sect = 0;
    while( !feof( src_file ) )
    {
        fgets( buffer, sizeof( buffer ), src_file );

        /* get rid of line-feed character */
        if( buffer[ strlen( buffer) - 1 ] == 0xA )
            buffer[ strlen( buffer) - 1 ] = 0;
        if( ferror( src_file ) )
        {
            sprintf( global_err_str, "File read error - %s", file_name  );
            fclose( src_file );
            return 0;
        }

        /* ....and look for the specified section */
        if( strstr( buffer, full_section_name ) != NULL )
        {
            /* found the section, break out */
            found_sect = 1;
            break;
        }
    }
    
    /* if end of file was reached and section was not found, return */
    if( !found_sect )
    {
        sprintf( global_err_str, "section not found" );
        fclose( src_file );
        return 0;
    }

    /* section has been located, now look for requested key_name */
    while( !feof( src_file ) )
    {
        /* get each line in the section.... */
        fgets( buffer, sizeof( buffer ), src_file );

        /* get rid of line-feed character */
        if( buffer[ strlen( buffer) - 1 ] == 0xA )
            buffer[ strlen( buffer) - 1 ] = 0;
        if( ferror( src_file ) )
        {
            sprintf( global_err_str, "File read error - %s", file_name  );
            fclose( src_file );
            return 0;
        }

        /* ....and check if it contains the required key_name */
        if( strncmp( key_name, buffer, strlen( key_name ) ) == 0 )
        {
            /* found the key_name, retrieve the value */
            found_eq = 0;
            for( temp = strlen( key_name ); temp < strlen( buffer ); temp++ )
            {
                if( buffer[temp] == '=' )
                {
                    found_eq = 1;
                    break;
                }
            }

            if( !found_eq )
            {
                sprintf( global_err_str, "File format error - %s", file_name  );
                fclose( src_file );
                return 0;
            }

            while( ++temp < strlen( buffer ) && buffer[temp] == ' ' );
            if( strlen( &buffer[temp] ) > buffer_size )
            {
                sprintf( global_err_str, "buffer size insufficient - %s", file_name  );
                fclose( src_file );
                return 0;
            }


           	if(strncmp(full_section_name,"[RESPONSE_CODE]",15)==0)
			{
				strncpy(dest_buffer, &buffer[temp],JCB_RESPONSE_CODE_LEN);
			}
			else if(strncmp(full_section_name,"[AUTH_CODE]",10)==0)
			{
				strncpy(dest_buffer, &buffer[temp],6);
			}
			else if(strncmp(full_section_name,"[RRN]",5)==0)
			{
				strncpy(dest_buffer, &buffer[temp],12);
			}
			//strncpy(RespCodeBuffer, &buffer[temp],3);

			//while(i<=1)
			//{
			//	RespCodeBuffer1[i] = RespCodeBuffer[i];
			//	++i;
			//}
			//strcpy(dest_buffer, RespCodeBuffer1);
            fclose( src_file );
            return 1;
        }
        else if( buffer[0] == '-' )
        {
            /* reached end of section, key_name was not found */
            sprintf( global_err_str, "key not found" );
            fclose( src_file );
            return 0;
        }
    }

    /* reached end of file, key_name was not found */
    sprintf( global_err_str, "key not found" );
	fclose( src_file );
    return 0;
}
