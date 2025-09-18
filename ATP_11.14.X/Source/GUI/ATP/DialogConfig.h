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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DialogConfig.h  $
   
      Rev 1.5   Dec 01 2003 11:29:18   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:48:54   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:22   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:28   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:20   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:24   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:36   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:50   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:14   iarustam
    
*
************************************************************************************/
// DialogConfig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfig dialog

class CDialogConfig : public CPropertyPage
{
	DECLARE_DYNCREATE(CDialogConfig)

// Construction
public:
	CDialogConfig();
	~CDialogConfig();
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CDialogConfig)
	enum { IDD = IDD_TPDU_ROUTER };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDialogConfig)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
