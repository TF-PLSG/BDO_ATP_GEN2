// ResetDcoMonitor.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "ResetDcoMonitor.h"
#include "afxdialogex.h"
#include "afxmt.h"
#include "TXDSAPI.H"

#define MONITOR1  1
#define MONITOR2  2
#define MONITOR3  3
#define MONITOR4  4
#define CLEAR     5
#define LOAD    "LOADING..."
#define SUCCESS "SUCCESS"
#define FAIL    "FAILED"

extern CHAR hostname1[50], hostname2[50], hostname3[50], hostname4[50];
extern SHORT port_num1, port_num2, port_num3, port_num4;
int resetFileFlag;//dco reset file flag from ini

CMutex mut;

// CResetDcoMonitor dialog

IMPLEMENT_DYNAMIC(CResetDcoMonitor, CDialog)

CResetDcoMonitor::CResetDcoMonitor(CWnd* pParent /*=NULL*/)
	: CDialog(CResetDcoMonitor::IDD, pParent)
{

}

CResetDcoMonitor::~CResetDcoMonitor()
{
}

void CResetDcoMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MONGH1, m_HostCntlGh1);
	DDX_Control(pDX, IDC_MONGH2, m_HostCntlGh2);
	DDX_Control(pDX, IDC_MONMKT1, m_HostCntlMkt1);
	DDX_Control(pDX, IDC_MONMKT2, m_HostCntlMkt2);

	DDX_Control(pDX, IDC_MONGH3, m_PortCntlGh1);
	DDX_Control(pDX, IDC_MONGH4, m_PortCntlGh2);
	DDX_Control(pDX, IDC_MONGH5, m_PortCntlMkt1);
	DDX_Control(pDX, IDC_MONGH6, m_PortCntlMkt2);

	DDX_Control(pDX, IDC_MONGH7, m_StatusCntlGh1);
	DDX_Control(pDX, IDC_MONGH8, m_StatusCntlGh2);
	DDX_Control(pDX, IDC_MONGH9, m_StatusCntlMkt1);	
	DDX_Control(pDX, IDC_MONGH10, m_StatusCntlMkt2);
}


BEGIN_MESSAGE_MAP(CResetDcoMonitor, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DCORESET, &CResetDcoMonitor::OnBnClickedMfcbutton1)
	ON_BN_CLICKED(IDCANCEL, OnCloseBtn)
END_MESSAGE_MAP()


// CResetDcoMonitor message handlers

BOOL CResetDcoMonitor::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char hostbuffer[100] = {0};
	char portbuffer[20]  = {0};

	sprintf(hostbuffer, "%s", hostname1);
	sprintf(portbuffer, "%hi", port_num1);
	m_HostCntlGh1.SetWindowText(hostbuffer);
	m_PortCntlGh1.SetWindowText(portbuffer);

	sprintf(hostbuffer, "%s", hostname2);
	sprintf(portbuffer, "%hi", port_num2);
	m_HostCntlGh2.SetWindowText(hostbuffer);
	m_PortCntlGh2.SetWindowText(portbuffer);

	sprintf(hostbuffer, "%s", hostname3);
	sprintf(portbuffer, "%hi", port_num3);
	m_HostCntlMkt1.SetWindowText(hostbuffer);
	m_PortCntlMkt1.SetWindowText(portbuffer);

	sprintf(hostbuffer, "%s", hostname4);
	sprintf(portbuffer, "%hi", port_num4);
	m_HostCntlMkt2.SetWindowText(hostbuffer);
	m_PortCntlMkt2.SetWindowText(portbuffer);

	m_StatusCntlGh1.SetWindowText("");
	m_StatusCntlGh2.SetWindowText("");
	m_StatusCntlMkt1.SetWindowText("");
	m_StatusCntlMkt2.SetWindowText("");

	memset(hostbuffer, 0, sizeof(hostbuffer));
	memset(portbuffer, 0, sizeof(portbuffer));
	get_reset_status(CLEAR);
	UpdateData(FALSE);

	return TRUE;
}

void CResetDcoMonitor::OnBnClickedMfcbutton1()
{
	// TODO: Add your control notification handler code here
	char error_msg[256] = {0};
	int rcode=0;
	int MAX_MONITORS = 4;
	int mon=0;
	int success = 0;

	resetFileFlag = get_log_flag_ini_data();
	write_to_reset_file_cpp("RESET DCO MONITOR REQUEST FROM ATP");
	get_reset_status(CLEAR);
	int ret = MessageBox("Do you want to Reset the all multiple monitor connections?\n"
					"WARNING:\n"
					"If you select YES, This will close all the existing monitors connections.\n"
					"All the open monitors will be automatically closed.",
					"Are you sure",MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2);
	if(ret == IDYES)
	{ 
		DWORD   dwThreadIdArray[4];
		HANDLE  hThreadArray[4]; 

		BeginWaitCursor();
		write_to_reset_file_cpp("Calling - txdsapi_clear_monitor_sockets_all_new");

		for(int i=0; i<MAX_MONITORS; i++)
		{
			mon = i+1;
			hThreadArray[i] = CreateThread( NULL,
									NULL,
									(LPTHREAD_START_ROUTINE)txdsapi_clear_monitor_sockets_all_new,
									(LPVOID)mon,
									NULL,
									&dwThreadIdArray[i] );
	
			if( hThreadArray[i] == NULL )
			{
				sprintf(error_msg, "Unable to create a receiving thread for %d", i+1);
				write_to_reset_file_cpp(error_msg);
			}
			else
			{
				sprintf(error_msg, "Created a receiving thread for %d", i+1);
				write_to_reset_file_cpp(error_msg);
			}
		}
		
		WaitForMultipleObjects(MAX_MONITORS, hThreadArray, TRUE, 300000);
		write_to_reset_file_cpp("Execution completed - txdsapi_clear_monitor_sockets_all_new");
		EndWaitCursor();

		for(int i=0; i<MAX_MONITORS; i++)
		{
			sprintf(error_msg, "Closing Thread Handle : %d", i+1);
			write_to_reset_file_cpp(error_msg);
			CloseHandle(hThreadArray[i]);
			TerminateThread(this, 0);
		}
		write_to_reset_file_cpp("Closed all threads - txdsapi_clear_monitor_sockets_all_new");
		
		if(update_reset_status() == 0)
			AfxMessageBox("All open DCO Monitors are closed.\nThe connections are reset", MB_ICONINFORMATION);
		else
			AfxMessageBox("Failed to reset the all DCO Monitor connections.\nVerify server status and Try again!", MB_ICONERROR);

		GetDlgItem(IDC_DCORESET)->ShowWindow(SW_HIDE);
	}
}

int CResetDcoMonitor::update_reset_status()
{
	int success = 1;

	m_StatusCntlGh1.SetWindowText(get_reset_status(MONITOR1));
	m_StatusCntlGh2.SetWindowText(get_reset_status(MONITOR2));
	m_StatusCntlMkt1.SetWindowText(get_reset_status(MONITOR3));
	m_StatusCntlMkt2.SetWindowText(get_reset_status(MONITOR4));

	UpdateData(FALSE);

	if( strcmp(get_reset_status(MONITOR1), SUCCESS) == 0 &&
		strcmp(get_reset_status(MONITOR2), SUCCESS) == 0 &&
		strcmp(get_reset_status(MONITOR3), SUCCESS) == 0 &&
		strcmp(get_reset_status(MONITOR4), SUCCESS) == 0 )
	{
		success = 0;
	}
	get_reset_status(CLEAR);
	return success;
}

void CResetDcoMonitor::OnCloseBtn()
{
	process_disconnect_dco_reset_connections();
	OnOK();
}

void CResetDcoMonitor::OnClose()
{
	process_disconnect_dco_reset_connections();
	OnOK();
}

void CResetDcoMonitor::write_to_reset_file_cpp(char *log)
{
	FILE *tp;
	CHAR buffer[1024] = {0};
	CHAR		time_str[50];
	struct tm * cur_time;
	time_t      clock ;

	CSingleLock lock(&mut);
	mut.Lock();

	if(resetFileFlag == 1)
	{
		time( &clock );
		cur_time = localtime( &clock );
		sprintf( time_str, "%04d-%02d-%02d  %02d:%02d:%02d", cur_time->tm_year + 1900,
			cur_time->tm_mon + 1, cur_time->tm_mday, cur_time->tm_hour,
			cur_time->tm_min, cur_time->tm_sec ) ;

		sprintf(buffer, "%s:- %s\n", time_str, log);
		tp = fopen("..\\log\\dco_monitor_reset_logs.txt", "a+");
		fwrite( buffer, sizeof( CHAR ), strlen( buffer ), tp );
		fflush(tp);
		fclose(tp);
	}

	mut.Unlock();
}


BOOL CResetDcoMonitor::OnIdle()
{
	return TRUE;
}