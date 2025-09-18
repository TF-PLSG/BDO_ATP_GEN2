#if !defined(AFX_BCH20DETAILS_H__60A43DD9_2357_4794_BB8C_645D26033A39__INCLUDED_)
#define AFX_BCH20DETAILS_H__60A43DD9_2357_4794_BB8C_645D26033A39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BCH20Details.h : header file
//
#include "MYUTILS.H"
/////////////////////////////////////////////////////////////////////////////
// CBCH20Details dialog

class CBCH20Details : public CDialog
{
// Construction
public:
	CBCH20Details(CWnd* pParent = NULL);   // standard constructor
	ARCHIVE_BCH20 sBatchRecord;
// Dialog Data
	//{{AFX_DATA(CBCH20Details)
	enum { IDD = IDD_BCH20DETAILS };
	CString	m_sz_auth_number;
	CString	m_sz_batch_nbr;
	CString	m_sz_card_nbr;
	CString	m_sz_device_id;
	CString	m_sz_invoice_nbr;
	CString	m_sz_merchant_id;
	CString	m_sz_processing_code;
	CString	m_sz_response_code;
	CString	m_sz_retrieval_ref_num;
	CString	m_sz_tran_amount;
	CString	m_sz_tran_date;
	CString	m_sz_tran_time;
	CString	m_sz_transaction_id;
	CString	m_sz_def_gross_amt;
	CString	m_sz_exp_date;
	CString	m_sz_open_date;
	CString	m_sz_open_time;
	CString	m_sz_sys_trace_audit_num;
	CString	m_sz_transmission_timestamp;
	CString	m_sz_processed;
	CString	m_sz_purge_date;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCH20Details)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBCH20Details)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCH20DETAILS_H__60A43DD9_2357_4794_BB8C_645D26033A39__INCLUDED_)
