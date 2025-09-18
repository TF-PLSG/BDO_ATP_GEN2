#ifndef JCB3DS2_DEFINITION
#define JCB3DS2_DEFINITION


#include "basictyp.h" 
#include "ChangeHistory.h"
#include "afxwin.h"


class CJcb3DS2Parameter : public CPropertyPage
{
	DECLARE_DYNCREATE(CJcb3DS2Parameter)
// Construction
public:
	CJcb3DS2Parameter();   // standard constructor
	~CJcb3DS2Parameter();

	CChangeHistory* pChangeHistoryPage;

	int m_nJCB3ds2Mode;
    int m_nDialogMode;
	int m_nPrevMode;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50]; 
	CTime c_time;
		BOOL CheckDialogForCompleteness();
		void PopulateHistoryTable();
		void PopulateJCB3ds2DialogFromDatabase(JCB3DS2 sjcb3ds2);
		char strFlag[2];
		BOOL GetValidationFlag();


	// Dialog Data
	//{{AFX_DATA(CCardsProfile)
	enum { IDD = ID_JCB3DS2_PARAMETERS };

	CEdit	m_Cntl3DS_Authentiction_RC_Value;
	CEdit	m_Cntl3DS_Authentication_Decline_Response_Code;
	CComboBox	m_Cntl3DS_AUTHENTICATION_VERIFY_FLAG;

	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;

	CString	m_Str3DS_Authentiction_RC_Value;
	CString	m_Str3DS_Authentication_Decline_Response_Code;
	CString	m_Str3DS_AUTHENTICATION_VERIFY_FLAG;


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
	afx_msg void OnAdd3DS2Button();
	afx_msg void OnDelete3DS2Button();
	afx_msg void OnUpdate3DS2Button();
	afx_msg void OnView3DS2Button();
	afx_msg void OnList3DS2Button();
	afx_msg void OnCbnSelchangeAuthenticationAttemptVerifyFlag();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedListmcc();
};
#endif