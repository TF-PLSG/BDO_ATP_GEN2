#ifndef BDKKEY_CONTROL
#define BDKKEY_CONTROL

#include "OnlinePINConfigureBDKKeyGeneral.h"
#include "basictyp.h" 
#include "ChangeHistory.h"

class CBDKKeyConfigurationLink : public CPropertyPage
{
	DECLARE_DYNAMIC(CBDKKeyConfigurationLink)

public:
	CBDKKeyConfigurationLink();  // * // standard constructor 
	~CBDKKeyConfigurationLink();			// *
	int m_nDialogMode;                   	//  as per rqrmnt
	int m_nPrevMode;						//	as per rqrmnt

//	CVector <CString> arrColumnNames;

	void PopulateHistoryTable();
	BOOL PopulateDatabaseFromConfigureBDKKeyDialog( pBDKKEY01);
	BOOL PopulateConfigureBDKKeyDialogFromDatabase( BDKKEY01);
	BOOL CheckDialogForCompleteness();

	char strFilePrimaryKey[30];
	char strStatus[2];
	char m_strCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

	BDKKEY01 BDKKEY01_old;

	CTime c_time;
	CString m_strFieldTag; 
	CString m_Strmodulename;
	CChangeHistory* pChangeHistoryPage;
	CBDKKeyConfigurationGeneral* pCBDKKeyConfigurationGeneral;
	//enum { IDD = ID_CONFIGURE_FRAUD_RULE_PAGE };  // rqrd 
	enum { IDD = ID_CONFIGURE_BDK_KEY_PAGE };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
//	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CEdit	m_CntlCardBrandFld;
	CString m_StrCardBrandFld;
	CString m_StrMessageLbl;
public:
	virtual void OnCancel();
	virtual BOOL OnApply();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // * // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);					
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedConfigurekeyAddButton();
	afx_msg void OnBnClickedConfigurekeyDeleteButton();
	afx_msg void OnBnClickedConfigurekeyUpdateButton();
	afx_msg void OnBnClickedConfigurekeyViewButton();
	afx_msg void OnBnClickedConfigurekeyListButton();
	DECLARE_MESSAGE_MAP()	


};

#endif