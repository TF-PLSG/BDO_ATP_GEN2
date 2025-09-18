// SysConfig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSysConfig dialog

#include "srvcinfo.h"
#include "basictyp.h"

class CSysConfig : public CDialog
{
// Construction
public:
	BOOLEAN PopulateSysConfigDialog( pINT   HighAvailability,
                                    pCHAR  ThisMachineId,   
                                    pCHAR  OtherMachineId,  
                                    pCHAR  ThisXipcInstance,
                                    pCHAR  OtherXipcInstance,
                                    pCHAR  ThisLoggerMSQueName);

	CSysConfig(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSysConfig)
	enum { IDD = IDD_SYS_CONFIG };
	CButton	m_SwitchPrimarySecondary;
	CButton	m_MakeThisServerPrimary;
	CButton	m_MakeOtherServerPrimary;
	CString	m_HighAvailabilityIndicator;
	CString	m_OtherMachineId;
	CString	m_OtherXipcInstance;
	CString	m_ThisMachineId;
	CString	m_ThisXipcInstance;
	CString	m_PrimaryServer;
	CString	m_SecondaryServer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSysConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSysConfig)
	afx_msg void OnMakeOtherServerPrimary();
	afx_msg void OnMakeThisServerPrimary();
	afx_msg void OnSwitchPrimarySecondary();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
