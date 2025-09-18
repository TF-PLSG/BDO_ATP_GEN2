#ifndef CAVV_DEFINITION
#define CAVV_DEFINITION


#include "basictyp.h" 
#include "ChangeHistory.h"

class CVisa3DS2ConfigMainPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CVisa3DS2ConfigMainPage)
// Construction
public:
	CVisa3DS2ConfigMainPage();   // standard constructor
	~CVisa3DS2ConfigMainPage();

	CChangeHistory* pChangeHistoryPage;

	int m_nCAVVMode;
    int m_nDialogMode;
	int m_nPrevMode;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50]; 
	CTime c_time;
	BOOL CheckDialogForCompleteness();
	void PopulateHistoryTable();
	void PopulateCAVVDialogFromDatabase(VISA3DS2 svisa3ds2);


	// Dialog Data
	//{{AFX_DATA(CCardsProfile)
	enum { IDD = ID_VISA3DS2Config_MAIN };
	CEdit	m_CntlCAVV;
	CEdit	m_CntlCAVVresponsecode;
	CEdit	m_CntlCAVVdescription;
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CString	m_StrCAVV;
	CString	m_StrCAVVresponsecode;
	CString	m_StrCAVVdescription;
	CString m_StrMessageLbl;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardsProfile)
		public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual void OnOK();	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBinDefinition)
	afx_msg void OnAddCAVVButton();
	afx_msg void OnDeleteCAVVButton();
	afx_msg void OnUpdateCAVVButton();
	afx_msg void OnViewCAVVButton();
	afx_msg void OnListCAVVButton();	
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedListmcc();
	afx_msg void OnStnClickedCardbrandLbl();
	afx_msg void OnEnChangeCavv();
};
#endif