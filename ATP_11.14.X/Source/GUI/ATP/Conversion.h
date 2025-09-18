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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\Conversion.h  $
   
      Rev 1.6   Dec 01 2003 11:29:10   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:47:26   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:14   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:20   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:12   iarustam
   Bug fixes
   
      Rev 1.1   Mar 22 1999 11:12:16   iarustam
   Bug fixes
   
   
      Rev 1.0   Feb 23 1999 11:40:16   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:20   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:26   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:36   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:02   iarustam
    
*
************************************************************************************/
// Conversion.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConversion dialog
#ifndef CONVERSION
#define CONVERSION

class CConversion : public CPropertyPage
{
	DECLARE_DYNCREATE(CConversion)

// Construction
public:
	CConversion();
	~CConversion();

	BOOL PopulateDialogFromDatabase();
	int m_nDialogMode;
    void EnableConvertCodeDialog(int);
	BOOL m_bConversion;  
    void CleanConvertCodeDialog();
	BOOL CheckDialogForCompleteness() ;

	CString mystring;
	CTime c_time;	

// Dialog Data
	//{{AFX_DATA(CConversion)
	enum { IDD = IDD_CONVERSION_RATE };
	CEdit	m_CntlNameFld;
	CEdit	m_CntlConversionRateFld;
	CEdit	m_CntlCurrencyCodeFld;
	CString	m_StrCurrencyCodeFld;
	CString	m_StrMessageLbl;
	CString	m_StrConversionRateFld;
	CString	m_StrNameFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConversion)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CConversion)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateConversionRateFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
