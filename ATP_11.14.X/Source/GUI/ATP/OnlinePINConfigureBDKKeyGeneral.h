#ifndef BDKKEY_CONFIGURE
#define BDKKEY_CONFIGURE

#include "basictyp.h" 
#include "ChangeHistory.h"
#include "OnlinePINConfigureBDKKey.h"

class CBDKKeyConfigurationGeneral : public CPropertyPage
{
	DECLARE_DYNAMIC(CBDKKeyConfigurationGeneral)

public:
	CBDKKeyConfigurationGeneral();  // * // standard constructor 
	virtual ~CBDKKeyConfigurationGeneral();			// *
	int m_nDialogMode;                   	//  as per rqrmnt
	int m_nPrevMode;						//	as per rqrmnt
	BOOL m_bBDKKEYGeneral;
//	CVector <CString> arrColumnNames;

	CTime c_time;
	CString m_strFieldTag; 
	CString m_Strmodulename;
	CChangeHistory* pChangeHistoryPage;

	char strStatus[2];
	char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

	BDKKEY01 BDKKEY01_old;


	void EnableConfigurGeneralDialog(int);
	void CleanConfigurGeneralDialog();
	void PopulateHistoryTable();
	BOOL PopulateDatabaseFromGeneralDialog( pBDKKEY01 psBDKKEY01 );
	BOOL PopulateGeneralDialogFromDatabase( BDKKEY01 sBDKKEY01  );
	BOOL CompareStructures();
	//enum { IDD = ID_CONFIGURE_FRAUD_RULE_GENERAL };  // rqrd 
	enum { IDD = ID_CONFIGURE_BDK_KEY_GENERAL };	
	CEdit	m_CntlCard_brand        ; 
	CEdit	m_CntlBdk_key  			; 
		
	CString	m_StrCard_brand         ;
	CString	m_StrBdk_key   			;

	CString	m_StrMessageLbl			;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // * // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);					
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	//afx_msg void OnKillfocusBDKKeyFld();
	DECLARE_MESSAGE_MAP()								

};

#endif