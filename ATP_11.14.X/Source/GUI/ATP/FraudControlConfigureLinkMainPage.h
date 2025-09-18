#ifndef FRAUD_LINK
#define FRAUD_LINK

#include "basictyp.h" 
#include "ChangeHistory.h"
#include "FraudControlConfigureLinkGeneralPage.h"

#include "MyUtils.h"

class CFraudControlConfigurationLinkpage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFraudControlConfigurationLinkpage)

public:
	CFraudControlConfigurationLinkpage();  // * // standard constructor 
	virtual ~CFraudControlConfigurationLinkpage();			// *
	int m_nDialogMode;                   	//  as per rqrmnt
	int m_nPrevMode;						//	as per rqrmnt
//	CVector <CString> arrColumnNames;
	BOOL CheckDialogForCompleteness();
	void PopulateHistoryTable();
	BOOL PopulateDatabaseFromConfigureDialog( pFCLF01);
	BOOL PopulateConfigureLinkDialogFromDatabase( pFCLF01);

	FCLF01 fclf01_old;
	FCLF01 fclf01;
	char strFilePrimaryKey[30];
	char strStatus[2];
	char m_strCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];


	CTime c_time;
	CString m_strFieldTag; 
	CString m_Strmodulename;
	CChangeHistory* pChangeHistoryPage;
	CFraudControlConfigurationLinkGeneralpage* pFraudControlConfigurationLinkGeneralpage;
	enum { IDD = ID_CONFIGURE_LINK_PAGE };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CEdit	m_CntlNetowrkFld;
	CString m_StrNetowrkFld;
	CString	m_StrMessageLbl;
public:
	virtual void OnCancel();
	virtual BOOL OnApply();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // * // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);					
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()								

public:
	afx_msg void OnBnClickedConfigureLinkAddButton();
	afx_msg void OnBnClickedConfigureLinkDeleteButton();
	afx_msg void OnBnClickedConfigureLinkUpdateButton();
	afx_msg void OnBnClickedConfigureLinkViewButton();
	afx_msg void OnBnClickedConfigureLinkListButton();
};

#endif