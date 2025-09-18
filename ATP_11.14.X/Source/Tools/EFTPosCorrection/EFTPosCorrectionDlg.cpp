// EFTPosCorrectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EFTPosCorrection.h"
#include "EFTPosCorrectionDlg.h"

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
// CEFTPosCorrectionDlg dialog

CEFTPosCorrectionDlg::CEFTPosCorrectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEFTPosCorrectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEFTPosCorrectionDlg)
	m_StrFileName = _T("");
	m_Status = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEFTPosCorrectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEFTPosCorrectionDlg)
	DDX_Text(pDX, IDC_FILENAME, m_StrFileName);
	DDX_Text(pDX, IDC_STATUS, m_Status);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEFTPosCorrectionDlg, CDialog)
	//{{AFX_MSG_MAP(CEFTPosCorrectionDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEFTPosCorrectionDlg message handlers

BOOL CEFTPosCorrectionDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEFTPosCorrectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CEFTPosCorrectionDlg::OnPaint() 
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
HCURSOR CEFTPosCorrectionDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CEFTPosCorrectionDlg::OnBrowse() 
{
	GetFileNameFromDialog(m_StrFileName, "");
	UpdateData(FALSE);
}

void CEFTPosCorrectionDlg::GetFileNameFromDialog(CString &szFile, CString strFilter)
{
	CFileDialog dlg(TRUE);
    //dlg.m_ofn.nMaxFile = 511;
	int len = strFilter.GetLength();
	if(len > 0)
	{
		
		dlg.m_ofn.lpstrFilter= strFilter.GetBuffer(len);
		strFilter.ReleaseBuffer();
	}
	else
	{
		dlg.m_ofn.lpstrFilter="All Files (*.*)\0*.*\0\0";
	}
    dlg.m_ofn.lpstrTitle="Open EFTPos File";
	
    if(dlg.DoModal() == IDOK)	
    {
		try
		{
        szFile = dlg.GetPathName(); // return full path and filename	
		}
		catch(...)
		{

		}
    }
	else
	{
		szFile.Empty();
	}
	return ;
}

typedef struct _EFT_RECORD_T{
	char line10[1024];
	char line11[1024];
	char line13[1024];
	char line14[1024];
	char line15[1024];
	int error_record;
	int corrected;
}EFT_RECORD, *pEFT_RECORD;

typedef struct _SEGEMENT10_T{
	char primary_key[52];
	char rrn[13];
	char card_num[21];
	char merchant_id[12];
	char tran_date[7];
	char tran_time[7];
	char tx_amount[13];
} SEGMENT10, *pSEGEMENT10;

typedef struct _SEGEMENT11_T{
	char primary_key[52];
	char rrn[13];
	char card_num[21];
	char tran_date[7];
	char tran_time[7];
	char sg_num[3];
	char exp_date[5];
	char service_code[4];
	char pan_sequence_num[4];
	char perch_id[28];
	char merchant_id[12];
	char tran_id[16];
	char tx_amount[13];
} SEGMENT11, *pSEGEMENT11;


void CEFTPosCorrectionDlg::OnOK() 
{
	UpdateData(TRUE);
	char line[1024] = {0};
	char segment[3] = {0};
	char rrn[13] = {0};
	EFT_RECORD eft;
	SEGMENT10 seg;
	SEGMENT11 S11;
	EFT_RECORD error_record;
	unsigned long count = 0;
	unsigned long wrote = 0;
	unsigned __int64 no_error_amount = 0;
	unsigned __int64 corrected_amount = 0;
	unsigned long records = 0;
	unsigned long no_err_record_count = 0;
	unsigned long corrected_record_count = 0;
	if(m_StrFileName.GetLength() > 0)
	{
		FILE *fptr = fopen(m_StrFileName, "r");
		CString str1 = m_StrFileName + "_No_Error";
		CString str2 = m_StrFileName + "_Error_Corrected";
		FILE *no_error_file = fopen(str1, "w");
		FILE *corrected_file = fopen(str2, "w");
		if(fptr != NULL && no_error_file != NULL && corrected_file != NULL)
		{
			memset(&seg, 0, sizeof(SEGMENT10));
			memset(&S11, 0, sizeof(SEGMENT11));
			memset(&eft, 0, sizeof(EFT_RECORD));
			while(fgets(line, 1023, fptr) != NULL)
			{
				count++;
				int len = strlen(line);
				if( strstr( line, "PRCHIDAccount Reload") != NULL)
				{
					int a = 1 + 1;
				}
				if(len > 0)
				{
					len = len - 1;
					line[len] = '\0';
					if(len > 52)
					{
						segment[0] = line[51];
						segment[1] = line[52];
						segment[3] = '\0';
						if((strncmp(seg.primary_key, line, 51) == 0) && strcmp(segment, "10") != 0)
						{
							if(strcmp(segment, "11") == 0)
							{
								strcpy(eft.line11, line);
								int i = 0;
								
								for(i=110; i< len; i++)
								{								
									//if(line[i] != ' ')
									if(eft.line11[i] >= '0' && eft.line11[i] <= '9')
									{
										break;
									}
								}
								if(i > 110)
								{
									eft.error_record = 1;
								}
								else
								{
									if(strncmp(&eft.line11[72], "   ", 3) == 0 && strncmp(&eft.line11[75], "000", 3) == 0)
									{
										eft.error_record = 1;
									}
									else
									{
										eft.error_record = 0;
									}
								}
							}
							else if(strcmp(segment, "13") == 0)
							{
								strcpy(eft.line13, line);
							}
							else if(strcmp(segment, "14") == 0)
							{
								strcpy(eft.line14, line);
							}
							else if(strcmp(segment, "15") == 0)
							{
								strcpy(eft.line15, line);
							}
							else
							{
								/// ???????
							}
						}
						else
						{
							if(eft.error_record == 1)
							{
								if(eft.line11[0] != 0)
								{
									if(strncmp(&eft.line11[72], "   ", 3) == 0 && strncmp(&eft.line11[75], "000", 3) == 0)
									{
										if( strstr( eft.line11, "PRCHIDAccount Reload") != NULL)
										{
											int a = 1 + 1;
										}
										//Perch id corruption
										
										char newline[1024] = {0};
										strncpy(newline, eft.line11, 72);
										strcat(newline, "000      ");	//71 + 9 = 80

										int i=0; 
										int length = strlen(eft.line11);

										//Get perch id
										char perch_id[28] = {0};
										for(i=81; i < 110; i++)
										{
											if(eft.line11[i] != ' ')
												break;
										}
										if(i < 110)
										{
											//There is a perch id.
											strncpy(perch_id, &eft.line11[i], 25);
										}
										else
										{
											//There is no perch id. Only blank spaces.
											strcpy(perch_id, "                           ");
										}
										

										//get cad merch id

										for(i=110; i < length; i++)
										{
											//if(eft.line11[i] != ' ')
											if(eft.line11[i] >= '0' && eft.line11[i] <= '9')
												break;
										}
										
										char cad_merch_id[11] = {0};
										strncpy(cad_merch_id, &eft.line11[i], 10);
										
										i = i + 10;
										length = strlen(eft.line11);
										for(; i < length; i++)
										{
											if(eft.line11[i] != ' ' && eft.line11[i] != '0')
												break;
										}
										
										char tid[16] = {0};
										strncpy(tid, &eft.line11[i], 15);
										
										strncat(newline, perch_id, 25);
										strcat(newline, "    ");
										strcat(newline, cad_merch_id);
										strcat(newline, "                           000        ");
										strcat(newline, tid);
										
										for(i = strlen(newline) -1; i< 245; i++)
										{
											newline[i] = ' ';
										}
										newline[i] = '\0';
										strcpy(eft.line11, newline);										
										
									}
									else
									{
										//only merch corruption
										char newline[1024] = {0};
										strncpy(newline, eft.line11, 110);
										
										//get cad merch id
										int i=0; 
										int length = strlen(eft.line11);

										for(i=110; i < length; i++)
										{
											//if(eft.line11[i] != ' ')
											if(eft.line11[i] >= '0' && eft.line11[i] <= '9')
												break;
										}
										
										char cad_merch_id[11] = {0};
										strncpy(cad_merch_id, &eft.line11[i], 10);
										
										i = i + 10;
										length = strlen(eft.line11);
										for(; i < length; i++)
										{
											if(eft.line11[i] != ' ' && eft.line11[i] != '0')
												break;
										}
										
										char tid[16] = {0};
										strncpy(tid, &eft.line11[i], 15);
										
										strcat(newline, cad_merch_id);
										strcat(newline, "                           000        ");
										strcat(newline, tid);
										
										for(i = strlen(newline) -1; i< 245; i++)
										{
											newline[i] = ' ';
										}
										newline[i] = '\0';
										strcpy(eft.line11, newline);		
										
									}
									
								}
								else
								{
									memset(eft.line11, 0, sizeof(eft.line11));
								}
							
								
								
								//Previous record is error record
								memcpy(&error_record, &eft, sizeof(EFT_RECORD));
								eft.error_record = 0;
								eft.corrected = 1;
								//memset(&eft, 0, sizeof(EFT_RECORD));
							}
							// Previous record is correct . 
							
							if(strcmp(segment, "10") == 0)
							{
								if(count > 1)
								{
									if(eft.corrected == 1)
									{
										//Error record. Corrected
										corrected_record_count++;
										CString sz = seg.tx_amount;
										sz.TrimLeft();
										sz.TrimRight();
										corrected_amount += atol(sz);
										if(eft.line10[0] != 0)
										{
											fprintf( corrected_file, "%s\n", eft.line10);
											wrote++;
										}
										
										if(eft.line11[0] != 0)
										{
											fprintf( corrected_file, "%s\n", eft.line11);
											wrote++;
										}
										
										if(eft.line13[0] != 0)
										{
											fprintf( corrected_file, "%s\n", eft.line13);
											wrote++;
										}
										
										if(eft.line14[0] != 0)
										{
											fprintf( corrected_file, "%s\n", eft.line14);
											wrote++;
										}
										
										if(eft.line15[0] != 0)
										{
											fprintf( corrected_file, "%s\n", eft.line15);
											wrote++;
										}
									}
									else
									{
										//No error record.
										no_err_record_count++;
										CString sz = seg.tx_amount;
										sz.TrimLeft();
										sz.TrimRight();
										no_error_amount += atol(sz);
										if(eft.line10[0] != 0)
										{
											fprintf( no_error_file, "%s\n", eft.line10);
											wrote++;
										}
										
										if(eft.line11[0] != 0)
										{
											fprintf( no_error_file, "%s\n", eft.line11);
											wrote++;
										}
										
										if(eft.line13[0] != 0)
										{
											fprintf( no_error_file, "%s\n", eft.line13);
											wrote++;
										}
										
										if(eft.line14[0] != 0)
										{
											fprintf( no_error_file, "%s\n", eft.line14);
											wrote++;
										}
										
										if(eft.line15[0] != 0)
										{
											fprintf( no_error_file, "%s\n", eft.line15);
											wrote++;
										}
										
									}
								}
								//Write previous record in the file
								
								//Write previous record in the file
								memset(&seg, 0, sizeof(SEGMENT10));
								memset(&eft, 0, sizeof(EFT_RECORD));
								
								records++;
								strcpy(eft.line10, line);
								
								
								//51 52 segment
								//59 , 12 chars are txnamount
								
								
								/*Segment 10*/
								strncpy(seg.primary_key, &line[0], 51);
								strncpy(seg.card_num, &line[20], 19);
								strncpy(seg.tran_date, &line[39], 6);
								strncpy(seg.tran_time, &line[45], 6);
								strncpy(seg.tx_amount, &line[59], 12);
								
								strncpy(seg.rrn, &line[108], 12);
							}
							else
							{
								//Error in the eft pos file
								//Either first line or last line.
								
								if(count > 1)
								{
									//if last line, write the eft record.
									if(eft.corrected == 1)
									{
										//Error record. Corrected
										corrected_record_count++;
										CString sz = seg.tx_amount;
										sz.TrimLeft();
										sz.TrimRight();
										corrected_amount += atol(sz);
										if(eft.line10[0] != 0)
										{
											fprintf( corrected_file, "%s\n", eft.line10);
											wrote++;
										}
										
										if(eft.line11[0] != 0)
										{
											fprintf( corrected_file, "%s\n", eft.line11);
											wrote++;
										}
										
										if(eft.line13[0] != 0)
										{
											fprintf( corrected_file, "%s\n", eft.line13);
											wrote++;
										}
										
										if(eft.line14[0] != 0)
										{
											fprintf( corrected_file, "%s\n", eft.line14);
											wrote++;
										}
										
										if(eft.line15[0] != 0)
										{
											fprintf( corrected_file, "%s\n", eft.line15);
											wrote++;
										}
										
									}
									else
									{
										//No error record.
										no_err_record_count++;
										CString sz = seg.tx_amount;
										sz.TrimLeft();
										sz.TrimRight();
										no_error_amount += atol(sz);
										if(eft.line10[0] != 0)
										{
											fprintf( no_error_file, "%s\n", eft.line10);
											wrote++;
										}
										
										if(eft.line11[0] != 0)
										{
											fprintf( no_error_file, "%s\n", eft.line11);
											wrote++;
										}
										
										if(eft.line13[0] != 0)
										{
											fprintf( no_error_file, "%s\n", eft.line13);
											wrote++;
										}
										
										if(eft.line14[0] != 0)
										{
											fprintf( no_error_file, "%s\n", eft.line14);
											wrote++;
										}
										
										if(eft.line15[0] != 0)
										{
											fprintf( no_error_file, "%s\n", eft.line15);
											wrote++;
										}		
										
										
									}

									//Trailer
									sprintf(line, "%1s%09d%016I64u%1s%016s%016d%1s%015I64u%170s",
											"9",         
											 wrote - 1,
											 corrected_amount,
											 " ",
											 "0",
											 corrected_record_count,
											 "+",
											 corrected_amount,         
											 " ");
									fprintf(corrected_file, "%s\n",line);

									sprintf(line, "%1s%09d%016I64u%1s%016s%016d%1s%015I64u%170s",
										"9",         
										 wrote - 1,
										 no_error_amount,
										 " ",
										 "0",
										 no_err_record_count,
										 "+",
										 no_error_amount,         
										 " ");
										
									fprintf(no_error_file, "%s\n",line);
										wrote ++;

									
								}
								else
								{
									//First line
									fprintf(no_error_file, "%s\n", line);
									fprintf(corrected_file, "%s\n", line);
									wrote++;
								}
							}
						}
						
						
					}
					else
					{
						//Just write line in the file
						fprintf(no_error_file, "%s\n", line);
						wrote++;
					}
				}
				else
				{
					//Just write line in the file
					fprintf(no_error_file, "%s\n", line);
					wrote++;
				}
				
				
				memset(line, 0, sizeof(line));
			}
			fclose(no_error_file);
			fclose(fptr);
		}
	}
	
	m_Status.Format("%lu Lines Read. %lu Lines Wrote",count, wrote);
	UpdateData(FALSE);
	
	
	
}

void ProcessEFTRecord(char *line)
{

}
