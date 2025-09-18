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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DualEnKeyCtrlPropertySheet.h  $
   
      Rev 1.0   Feb 02 2011 11:30:22   TF_Srikanth
   Added DualEncrKeyControl Class for Changing Encryption Key in Database from GUI  
    
*
************************************************************************************/

// DualEnKeyCtrlPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDualEnKeyCtrlPropertySheet
	#include "DualEncrKeyControl.h"
class CDualEnKeyCtrlPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDualEnKeyCtrlPropertySheet)

// Construction
public:
	CDualEnKeyCtrlPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDualEnKeyCtrlPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

CDualEncrKeyControl m_DualEncrKeyControl;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDualEnKeyCtrlPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDualEnKeyCtrlPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDualEnKeyCtrlPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
