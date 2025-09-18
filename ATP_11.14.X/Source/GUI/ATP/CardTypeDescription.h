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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardTypeDescription.h  $
   
      Rev 1.5   Dec 01 2003 11:29:00   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:45:04   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:04   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:10   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:02   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:08   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:12   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:04   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:06   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:42   iarustam
    
*
************************************************************************************/
// CardTypeDescription.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardTypeDescription dialog
#ifndef CCARDTYPEDESCRIPTION
#define CCARDTYPEDESCRIPTION

class CCardTypeDescription : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardTypeDescription)

// Construction
public:
	CCardTypeDescription();
	~CCardTypeDescription();


	BOOL PopulateDialogFromDatabase();
	int m_nDialogMode;
	void EnableCardDescriptionDialog(int);
	BOOL m_bDescription;
	CTime c_time;
	void CleanCardDescriptionDialog();
// Dialog Data
	//{{AFX_DATA(CCardTypeDescription)
	enum { IDD = IDD_CARD_TYPE_DESCRIPTION };
	CEdit	m_CntlDescriptionFld;
	CString	m_StrCardTypeFld;
	CString	m_StrDescriptionFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardTypeDescription)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardTypeDescription)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
