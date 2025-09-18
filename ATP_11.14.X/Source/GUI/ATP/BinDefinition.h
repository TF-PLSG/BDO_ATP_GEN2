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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\BinDefinition.h  $
   
      Rev 1.10   Sep 28 2006 10:27:44   dirby
   Added a check box to the Bin Range GUI to indicate if a bin range
   is eligible or not for Fraudguard.  If not checked, then txns are
   sent to Fraudguard (if they meet other qualifications).  If checked,
   then txns will not go to Fraudguard (regardless of other qualifications).
   The new check box shares the Mod_Check field with the Mod10
   Check value and the Cash Bonus Eligible value.  Each of these
   three values is either ON or OFF, so they are represented by a
   single bit within the Mod_Check field.
   SCR 21851
   
      Rev 1.9   Apr 01 2005 10:13:28   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.8   Dec 01 2003 11:28:42   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Sep 11 2002 16:44:02   dirby
   Added function prototype 'get_mod10_cash_bonus_value'.
   SCR 804
   
      Rev 1.6   Sep 28 2001 12:41:50   SYARLAGA
   Added BinMccBlocking.
   
      Rev 1.5   Apr 17 2000 09:56:54   iarustam
   added bin blocking
   
   
      Rev 1.4   Jan 05 2000 14:52:52   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:40:54   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:48   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:52   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:46   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:56   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:58   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:28   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:06   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:12   iarustam
    
*
************************************************************************************/
// BinDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBinDefinition dialog
#ifndef BIN_DEFINITION
#define BIN_DEFINITION

#include "ChangeHistory.h"
#include "BinGeneral.h"
#include "BinBlocking.h"
#include "BinMCCBlocking.h"
#include "BinMidBlocking.h"

class CBinDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CBinDefinition)

// Construction
public:
	CBinDefinition();
	~CBinDefinition();

	BOOL CheckDialogForCompleteness();
	BOOL GetBinType();
	BOOL PopulateBinDatabaseStructureFromDialog (pBIN01);
	void PopulateHistoryTable();

    CChangeHistory* pChangeHistoryPage;
    CBinGeneral* pGeneralPage;
    CBinBlocking* pBlockingPage;
	CBinMCCBlocking* pMccBlockingPage;
	CBinMidBlocking* pMidBlockingPage;

    int m_nDialogMode;
	char strType[2];
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

	BIN01 sBinOldRecord;
//	PTF01 sPrefixTimeRecordOld;
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CBinDefinition)
	enum { IDD = IDD_BIN_DEFINITION };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CComboBox	m_CntlType;
	CEdit	m_CntlBinLengthFld;
	CEdit	m_CntlBinLowFld;
	CString	m_StrBinLowFld;
	CString	m_StrMessageLbl;
	CString	m_StrBinLengthFld;
	CString	m_StrType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBinDefinition)
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
	afx_msg void OnAddBinButton();
	afx_msg void OnDeleteBinButton();
	afx_msg void OnUpdateBinButton();
	afx_msg void OnViewBinButton();
	afx_msg void OnListBinButton();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnKillfocusBinLengthFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
