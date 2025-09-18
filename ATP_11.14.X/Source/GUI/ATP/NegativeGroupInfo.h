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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NegativeGroupInfo.h  $
   
      Rev 1.5   Dec 01 2003 11:29:52   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:08   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:56:22   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:00   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:06   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:48   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:54   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:48   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:40   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:16   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:18   iarustam
    
*
************************************************************************************/
// NegativeGroupInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNegativeGroupInfo dialog
#ifndef NEG_GROUP_INFO
#define NEG_GROUP_INFO

#include "app_info.h"

class CNegativeGroupInfo : public CPropertyPage
{
	DECLARE_DYNCREATE(CNegativeGroupInfo)

// Construction
public:
	CNegativeGroupInfo();
	~CNegativeGroupInfo();

	NGF01 sNegGroupRecordOld;
	NBF01 sNegBinRecord;
	NBF01_GUI_LIST sNegBinList;

	int m_nOldCount;
	BOOL m_bGeneral;
	BOOL bDone;
	BOOL bNext;
	int m_nDialogMode;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
    char m_strFieldTag[50];

	void EnableGeneralDialog ( int );
	void PopulateHistoryTable();
	void CleanGeneralDialog();
	void PopulateNegBinListDialog();
	BOOL PopulateGeneralDialogFromDatabase ( NGF01 );
	BOOL PopulateDatabaseFromGeneralDialog ( pNGF01 );
	BOOL CompareStructures();
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CNegativeGroupInfo)
	enum { IDD = IDD_NEG_GROUP_INFO };
	CListCtrl	m_OldList;
	CButton	m_NextButton;
	CListCtrl	m_NewList;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CString	m_StrMessageLbl;
	CString	m_StrNegGroupIDFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNegativeGroupInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNegativeGroupInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnNextButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
