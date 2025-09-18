/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardDefinition.h  $
   
      Rev 1.6   Dec 01 2003 11:28:48   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:44:14   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:00   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:06   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:55:58   iarustam
   Bug fixes
   
      Rev 1.1   Mar 08 1999 17:20:02   iarustam
   Bug fixes
   
   
      Rev 1.0   Feb 23 1999 11:40:06   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:08   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:58   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:00   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:36   iarustam
    
*
************************************************************************************/
// CardDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardDefinition dialog

#ifndef CARD_DEFINITION
#define CARD_DEFINITION

#include "CardTypeDescription.h"
#include "ChangeHistory.h"



class CCardDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardDefinition)

// Construction
public:
	CCardDefinition();
	~CCardDefinition();

	BOOL PopulateCardTypeDialogFromDatabase ( CTF01 );
	BOOL PopulateCardTypeDatabaseStructureFromDialog ( pCTF01 );
	BOOL CheckDialogForCompleteness();
	void PopulateHistoryTable();

	CCardTypeDescription *pDescriptionPage;
	CChangeHistory *pChangeHistoryPage;

	int m_nDialogMode;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

	CTF01 sCardTypeOld;
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CCardDefinition)
	enum { IDD = IDD_CARD_DEFINITION };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CEdit	m_CntlCardTypeFld;
	CString	m_StrCardTypeFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardDefinition)
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
	//{{AFX_MSG(CCardDefinition)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	afx_msg void OnListButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
