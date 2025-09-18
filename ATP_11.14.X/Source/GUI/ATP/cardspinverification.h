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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\cardspinverification.h  $
   
      Rev 1.4   Dec 01 2003 11:28:52   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.3   Jan 05 2000 14:54:36   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:03:44   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:36   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:48   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:32   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:20   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:36   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:22   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:14   iarustam
    
*
************************************************************************************/
// CardsPinVerification.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsPinVerification dialog
#ifndef CCARDSPINVERIFICATION
#define CCARDSPINVERIFICATION

class CCardsPinVerification : public CDialog
{
// Construction
public:
	CCardsPinVerification(CWnd* pParent = NULL);   // standard constructor

    int m_nDialogMode;

    CAF01 sProfileId;
    CAF01 sProfileIdOld;

    BOOL CCardsPinVerification::PopulatePinVerificationDialogFromDatabase( CAF01 sProfileId );
    BOOL CCardsPinVerification::PopulateDatabaseFromPinVerificationDialog( pCAF01 psProfileId );
    BOOL CCardsPinVerification::CheckPinKeyData(CString strData);

    void CCardsPinVerification::EnablePinVerificationDialog(int nStatus);
    void CCardsPinVerification::CleanPinVerificationDialog();

	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCardsPinVerification)
	enum { IDD = IDD_CARDS_PIN_VERIFICATION };
	CEdit	m_CntlTrackPinOffsetLoc_Fld;
	CEdit	m_CntlPinOffsetLocation_Fld;
	CEdit	m_CntlPinKey_Fld;
	CEdit	m_CntlOffsetLength_Fld;
	CEdit	m_CntlDesimTable_Fld;
	CString	m_StrDesimTable_Fld;
	CString	m_StrOffsetLength_Fld;
	CString	m_StrPinKey_Fld;
	CString	m_StrPinOffsetLocation_Fld;
	CString	m_StrTrackPinOffsetLoc_Fld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardsPinVerification)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCardsPinVerification)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
