#ifndef OBS_DEFINITION
#define OBS_DEFINITION


#include "basictyp.h" 
#include "ChangeHistory.h"


class CMC3DSecureOBSAuthenticationMainPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMC3DSecureOBSAuthenticationMainPage)
// Construction
public:
	CMC3DSecureOBSAuthenticationMainPage();   // standard constructor
	~CMC3DSecureOBSAuthenticationMainPage();

	CChangeHistory* pChangeHistoryPage;

	int m_nOBSMode;
    int m_nDialogMode;
	int m_nPrevMode;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50]; 
	CTime c_time;
		BOOL CheckDialogForCompleteness();
		void PopulateHistoryTable();
		void PopulateOBSDialogFromDatabase(OBS01 sobs01);


	// Dialog Data
	//{{AFX_DATA(CCardsProfile)
	enum { IDD = ID_MC3DSECUREOBS_MAIN };
	CEdit	m_Cntlonbehalfservice;
	CEdit	m_Cntlonbehalfresult;
	CEdit	m_Cntlresponsecode;
	CEdit	m_Cntldescription;
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CString	m_Stronbehalfservice;
	CString	m_Stronbehalfresult;
	CString	m_Strresponsecode;
	CString	m_Strdescription;
	CString m_StrMessageLbl;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardsProfile)
		public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBinDefinition)
	afx_msg void OnAddOBSButton();
	afx_msg void OnDeleteOBSButton();
	afx_msg void OnUpdateOBSButton();
	afx_msg void OnViewOBSButton();
	afx_msg void OnListOBSButton();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedListmcc();
};
#endif