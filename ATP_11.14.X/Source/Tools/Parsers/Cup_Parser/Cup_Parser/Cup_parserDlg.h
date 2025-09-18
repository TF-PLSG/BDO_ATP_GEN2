
// ex_amex_parserDlg.h : header file
//

#pragma once


// Cex_amex_parserDlg dialog
class Cex_amex_parserDlg : public CDialogEx
{
// Construction
public:
	Cex_amex_parserDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EX_AMEX_PARSER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_flowfile;
	CString m_parsedmssg;
	afx_msg void OnBnClickedParse();
	afx_msg void OnBnClickedClear();
	CRichEditCtrl m_CntlParsedMsg;
	afx_msg void OnEnChangeParsedmsg();
	afx_msg void OnEnChangeFlowfile();
	afx_msg void OnBnClickedcopyButton();
	CButton copyParsedData;
	afx_msg void OnBnClickedaboutButton();
	afx_msg void OnBnClickedExport();
	CButton exportbtn;
};
