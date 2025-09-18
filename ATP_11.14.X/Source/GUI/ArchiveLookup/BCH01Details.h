#if !defined(AFX_BCH01DETAILS_H__50E9C7C1_C6A7_472C_A903_4BC76EE4D7BC__INCLUDED_)
#define AFX_BCH01DETAILS_H__50E9C7C1_C6A7_472C_A903_4BC76EE4D7BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BCH01Details.h : header file
//
#include "MYUTILS.H"
/////////////////////////////////////////////////////////////////////////////
// CBCH01Details dialog

class CBCH01Details : public CDialog
{
// Construction
public:
	CBCH01Details(CWnd* pParent = NULL);   // standard constructor
	ARCHIVE_BCH01 sBatchRecord;
// Dialog Data
	//{{AFX_DATA(CBCH01Details)
	enum { IDD = IDD_BCH01DETAILS };
	CStatic	m_CntlStatus;
	CString	m_sz_batch_nbr;
	CString	m_sz_batch_status;
	CString	m_sz_device_id;
	CString	m_sz_merchant_id;
	CString	m_sz_open_date;
	CString	m_sz_open_time;
	CString	m_sz_release_date;
	CString	m_sz_release_time;
	CString	m_szStatus;
	CString	m_sz_total_credit_amt;
	CString	m_sz_total_credit_cnt;
	CString	m_sz_total_credit_refund_amt;
	CString	m_sz_total_credit_refund_cnt;
	CString	m_sz_total_debit_amt;
	CString	m_sz_total_debit_refund_amt;
	CString	m_sz_total_debit_refund_cnt;
	CString	m_sz_total_debit_cnt;
	CString	m_sz_total_void_credit_amt;
	CString	m_sz_total_void_credit_cnt;
	CString	m_sz_total_void_credit_refund_amt;
	CString	m_sz_total_void_credit_refund_cnt;
	CString	m_sz_visa_merchant_id;
	CString	m_sz_purge_date;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCH01Details)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBCH01Details)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCH01DETAILS_H__50E9C7C1_C6A7_472C_A903_4BC76EE4D7BC__INCLUDED_)
