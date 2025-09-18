// XCOMTOOLDlg.h : header file
//

#if !defined(AFX_XCOMTOOLDLG_H__B88BAFE3_1830_4467_825A_936FF96A673A__INCLUDED_)
#define AFX_XCOMTOOLDLG_H__B88BAFE3_1830_4467_825A_936FF96A673A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define  IDT_TIMER  WM_USER + 200

#define WM_REFRESHWND WM_USER+101

/////////////////////////////////////////////////////////////////////////////
// CXCOMTOOLDlg dialog

class CXCOMTOOLDlg : public CDialog
{
// Construction
public:
	CXCOMTOOLDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CXCOMTOOLDlg)
	enum { IDD = IDD_XCOMTOOL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXCOMTOOLDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	//User Defined Objects
	CButton m_btn1;	
	
	CStatic q_label1;
	CStatic q_label2;

	
	CStatic user_label1;
	CStatic user_label2;

	UINT Timeval;

	
	CStatic mem_label1;
	CStatic mem_label2;

	CStatic date_time_label;

	CStatic app_name;
	CStatic version;
	CStatic user_summary;
	CStatic que_summary;
	CStatic mem_summary;

	CEdit m_edit_search;

	BOOL QueList_f;
	BOOL MemList_f;
	BOOL UserList_f;

	char lData[256];
	int lConnect;
    int lLength;
	char Recv_Data[256];

	char * GetIPAdd();
	char * Socket_Connect();
	void Socket_Close();
	char * Socket_Send(char data[]);
	void Socket_Recv();

	void Connect_Applnk();
	void Disconnect_Applnk();
	   
	CHAR m_ThisMachine[50];
    CHAR m_ConnectString[256];
    INT  m_HighAvailability;
	CHAR AppName[12];
    CHAR buffer[256];    
    CHAR error_msg[100];

	HWND *hwnd;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CXCOMTOOLDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnConfig();
	afx_msg void OnBtnCreate();
	afx_msg void OnSelchangeList1();
	afx_msg void OnSelchangeList2();
	afx_msg void OnSelchangeList3();
	afx_msg void OnBtnRead();
	afx_msg void OnBtnRetry();
	afx_msg void OnBtnStop();
	afx_msg void OnBtnWrite();
	afx_msg void OnBtnFlush();
	afx_msg void OnBtnDelete();
	afx_msg UINT  StartTimer (UINT TimerDuration);  // Start the Timer
	afx_msg void OnSetfocusList2();
	afx_msg void OnSetfocusList1();
	afx_msg void OnSetfocusList3();
	afx_msg void OnButtonSearch();
	afx_msg void OnClose();
	afx_msg void OnButtonSearchNext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XCOMTOOLDLG_H__B88BAFE3_1830_4467_825A_936FF96A673A__INCLUDED_)
