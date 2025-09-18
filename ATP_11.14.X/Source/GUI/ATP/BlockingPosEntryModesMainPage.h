#ifndef POS_DEFINITION
#define POS_DEFINITION


#include "ChangeHistory.h"
#include "BlockingPosEntryModesGeneralPage.h"

class CBlockingPosEntryModesMainPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBlockingPosEntryModesMainPage)
// Construction
public:
	CBlockingPosEntryModesMainPage();   // standard constructor
	~CBlockingPosEntryModesMainPage();

	CChangeHistory* pChangeHistoryPage;
	CBlockingPosEntryModesGeneralPage *pGeneralpage;

	int m_nBlockPOSentryMode;
    int m_nDialogMode;
	char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];  
	CTime c_time;

	// Dialog Data
	//{{AFX_DATA(CCardsProfile)
	enum { IDD = IDD_BLOCKPOSETRYMODE_MAIN };
	CEdit	m_CntlCardBrandType;
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CString	m_StrCardBrandType;

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
	afx_msg void OnAddPOSButton();
	afx_msg void OnDeletePOSButton();
	afx_msg void OnUpdatePOSButton();
	afx_msg void OnViewPOSButton();
	afx_msg void OnListPOSButton();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif