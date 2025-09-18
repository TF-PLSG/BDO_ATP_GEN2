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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SchedulerPropertySheet.h  $
   
      Rev 1.5   Dec 01 2003 11:30:12   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:22   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:00:04   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:18   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:26   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:04   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:18   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:06   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:12   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:48   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:46   iarustam
    
*
************************************************************************************/
// SchedulerPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSchedulerPropertySheet
#include "SchedulerConfig.h"
#include "ShedulerControl.h"

class CSchedulerPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSchedulerPropertySheet)

// Construction
public:
	CSchedulerPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSchedulerPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

   CSchedulerConfig m_SchedulerConfig;
   CShedulerControl m_SchedulerControl;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSchedulerPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSchedulerPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSchedulerPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
