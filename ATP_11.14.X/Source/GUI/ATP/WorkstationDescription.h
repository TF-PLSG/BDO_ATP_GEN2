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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\WorkstationDescription.h  $
   
      Rev 1.5   Dec 01 2003 11:30:26   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:34   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:03:10   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:34   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:44   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:16   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:30   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:18   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:34   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:20   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:12   iarustam
    
*
************************************************************************************/
// WorkstationDescription.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkstationDescription dialog
#ifndef WORKSTATIONDESCRIPTION
#define WORKSTATIONDESCRIPTION

class CWorkstationDescription : public CPropertyPage
{
	DECLARE_DYNCREATE(CWorkstationDescription)

// Construction
public:
	CWorkstationDescription();
	~CWorkstationDescription();

	BOOL PopulateDialogFromDatabase();
	int m_nDialogMode;
	void EnableWorkstationDescriptionDialog(int);
	BOOL m_bDescription;

   ATP02 atp02;

	void CleanWorkstationDescriptionDialog();
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CWorkstationDescription)
	enum { IDD = IDD_WORKSTATION_DESCRIPTION };
	CEdit	m_WorkstationIdCtrl;
	CComboBox	m_WorkstationStatus;
	CString	m_StrMessageLbl;
	CString	m_WorkstationIdStr;
	CString	m_StrWorkstationStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWorkstationDescription)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWorkstationDescription)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeWorkstationStatus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


#endif