/////////////////////////////////////////////////////////////////////////////
// RULE_DEFINITION_MAIN dialog

#ifndef RULE_DEFINITION_MAIN
#define RULE_DEFINITION_MAIN

#include "RulesDefinitionProcessing.h"
#include "RulesExceptionList.h"
#include "ChangeHistory.h"

class CRuleDefinitionMain : public CPropertyPage
{
	DECLARE_DYNCREATE(CRuleDefinitionMain)

// Construction

	public:
	CRuleDefinitionMain();
	~CRuleDefinitionMain();
    
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
    char strFilePrimaryKey[50];

	CR01 sRuleIdOld;


    int m_nDialogMode;
	BOOL m_bProcessing;



    void PopulateHistoryTable();    

    BOOL PopulateRuleIdDialogFromDatabase (CR01 sRuleId);    
    BOOL PopulateDatabaseStructureFromRuleIdDialog ( pCR01 psRuleId); 
    BOOL CheckDialogForCompleteness();
    BOOL CheckTime();


    CRulesDefinitionProcessing* pRulesProcessingPage;
	CRulesDefinitionExceptionList *pCardNumBlockingPage;
	CChangeHistory* pChangeHistoryPage;
	//CRuleDefinitionMain *pRulesProfilePage;

		enum { IDD = ID_RULESDEFINITION_MAIN };
		CButton	m_ViewButton;
		CButton	m_UpdateButton;
		CButton	m_ListButton;
		CButton	m_DeleteButton;
		CButton	m_AddButton;
		CEdit	m_CntlRuleID;
		CString	m_StrMessageLbl;
		CString	m_StrRuleID_Fld;

	
	CTime c_time;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardsFleetDefinition)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardsFleetDefinition)
	virtual BOOL OnInitDialog();
	afx_msg void OnRulesAddButton();
	afx_msg void OnRulesDeleteButton();
	afx_msg void OnRulesListButton();
	afx_msg void OnRulesUpdateButton();
	afx_msg void OnRulesViewButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif