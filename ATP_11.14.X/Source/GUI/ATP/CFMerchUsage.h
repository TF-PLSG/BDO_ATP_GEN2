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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFMerchUsage.h  $
   
      Rev 1.5   Dec 01 2003 11:29:06   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:00   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:43:02   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:54   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:58   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:54   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:02   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:04   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:48   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:46   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:26   iarustam
    
*
************************************************************************************/
// CFMerchUsage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCFMerchUsage dialog
#ifndef MERCHUSPAGE
#define MERCHUSPAGE


class CCFMerchUsage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCFMerchUsage)

// Construction
public:
	CCFMerchUsage();
	~CCFMerchUsage();

	int DialogMode;
	BOOL m_bMerchUsage;
	BOOL m_bCCF03;

	void PopulateMerchUsDialog();
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCFMerchUsage)
	enum { IDD = IDD_CARDMERCHUSAGE };
	CListCtrl	m_ctrlRepeatList;
	CListCtrl	m_ctrlMCCList;
	CStatic	m_ComEdit;
	CString	m_strCardNbr;
	CString	m_strCardType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCFMerchUsage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCFMerchUsage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
