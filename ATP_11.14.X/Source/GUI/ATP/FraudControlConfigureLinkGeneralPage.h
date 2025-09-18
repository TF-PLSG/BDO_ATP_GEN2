#ifndef FRAUD_GENERAL_LINK
#define FRAUD_GENERAL_LINK

#include "basictyp.h" 
#include "ChangeHistory.h"

class CFraudControlConfigurationLinkGeneralpage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFraudControlConfigurationLinkGeneralpage)

public:
	CFraudControlConfigurationLinkGeneralpage();  // * // standard constructor 
	virtual ~CFraudControlConfigurationLinkGeneralpage();			// *

	BOOL m_bFraudGeneral;
	int m_nDialogMode;                   	//  as per rqrmnt
	int m_nPrevMode;						//	as per rqrmnt
//	CVector <CString> arrColumnNames;

	CTime c_time;
	CString m_strFieldTag; 
	CString m_Strmodulename;
	CChangeHistory* pChangeHistoryPage;

	char strStatus[2];
	char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];


	FCLF01 fclf01_old;
	FCLF01 fclf01;
	BOOL CompareStructures();
	void EnableConfigurGeneralDialog(int);
	void CleanConfigurGeneralDialog();
	void PopulateHistoryTable();
	BOOL PopulateDatabaseFromGeneralDialog( pFCLF01 psFclf01 );
	BOOL PopulateGeneralDialogFromDatabase( pFCLF01 psFclf01  );

	//enum { IDD = ID_CONFIGURE_LINK_GENERAL };  // rqrd 
	enum { IDD = ID_CONFIGURE_LINK_GENERAL };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CButton m_CntlEnableFraudControl;
	CButton m_CntlEnableFraudRule;
	CString	m_StrMessageLbl;




protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // * // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);					

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()								

};

#endif