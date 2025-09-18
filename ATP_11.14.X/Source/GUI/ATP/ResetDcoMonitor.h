#pragma once

#include "ListExCtrl.h"

// CResetDcoMonitor dialog

class CResetDcoMonitor : public CDialog
{
	DECLARE_DYNAMIC(CResetDcoMonitor)

public:
	CResetDcoMonitor(CWnd* pParent = NULL);   // standard constructor
	virtual ~CResetDcoMonitor();

// Dialog Data
	enum { IDD = IDD_DIALOG6 };
	CStatic	m_HostCntlGh1;
	CStatic	m_HostCntlGh2;
	CStatic	m_HostCntlMkt1;
	CStatic	m_HostCntlMkt2;
	CStatic	m_PortCntlGh1;
	CStatic	m_PortCntlGh2;
	CStatic	m_PortCntlMkt1;
	CStatic	m_PortCntlMkt2;
	CStatic m_StatusCntlGh1;
	CStatic m_StatusCntlGh2;
	CStatic m_StatusCntlMkt1;
	CStatic m_StatusCntlMkt2;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnIdle();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMfcbutton1();
	afx_msg int update_reset_status();
	void write_to_reset_file_cpp(char *);
	afx_msg void OnCloseBtn();
	afx_msg void OnClose();
};
