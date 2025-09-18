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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardPlusPropSheet.h  $
   
      Rev 1.7   Dec 01 2003 11:28:50   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jul 10 2002 09:55:48   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.5   Jan 05 2000 14:53:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:44:30   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:02   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:08   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:01:14   dperson
   Added PVCS header
*   
************************************************************************************/
// CardPlusPropSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardPlusPropSheet

#include "CardPlusMain.h"
#include "CardPlusData.h"

class CCardPlusPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CCardPlusPropSheet)

// Construction
public:
	CCardPlusPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CCardPlusPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CCardPlusMain m_MainPage;
	CCardPlusData m_DataPage;

	int m_nActiveTab;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardPlusPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCardPlusPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCardPlusPropSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	BOOL CCardPlusPropSheet::OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
