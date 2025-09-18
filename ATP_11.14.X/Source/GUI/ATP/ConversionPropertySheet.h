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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ConversionPropertySheet.h  $
   
      Rev 1.6   Dec 01 2003 11:29:10   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:56:02   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:53:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:47:32   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:14   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:22   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:12   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:18   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:20   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:28   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:38   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:04   iarustam
    
*
************************************************************************************/
// ConversionPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConversionPropertySheet
#include "Conversion.h"
#include "CurrencyCode.h"
#include "ChangeHistory.h"

class CConversionPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CConversionPropertySheet)

// Construction
public:
	CConversionPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CConversionPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	void SetData();

// Attributes
public:

	CConversion m_ConversionPage;
	CCurrencyCode m_CurrencyPage;
	CChangeHistory m_ChangeHistoryPage;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConversionPropertySheet)
	//}}AFX_VIRTUAL
	BOOL CConversionPropertySheet::OnInitDialog();

// Implementation
public:
	virtual ~CConversionPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CConversionPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
