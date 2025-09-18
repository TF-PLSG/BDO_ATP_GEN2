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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardsPinVerifVisa.h  $
   
      Rev 1.7   Dec 01 2003 11:28:54   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 05 2000 14:53:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:45:42   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:06   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:14   iarustam
   Initial Release
   
      Rev 1.3   May 10 1999 11:06:16   dperson
   Added PVCS header
*   
************************************************************************************/
// CardsPinVerifVisa.h : header file
//
#ifndef CCARDSPINVERIFVISA
#define CCARDSPINVERIFVISA
/////////////////////////////////////////////////////////////////////////////
// CCardsPinVerifVisa dialog

class CCardsPinVerifVisa : public CDialog
{
// Construction
public:
	CCardsPinVerifVisa(CWnd* pParent = NULL);   // standard constructor
    int m_nDialogMode;

    CAF01 sProfileId;
    CAF01 sProfileIdOld;

    BOOL CCardsPinVerifVisa::PopulatePinVerifVisaDialogFromDatabase( CAF01 sProfileId );
    BOOL CCardsPinVerifVisa::PopulateDatabaseFromPinVerifVisaDialog( pCAF01 psProfileId );
    

    void CCardsPinVerifVisa::EnablePinVerifVisaDialog(int nStatus);
    void CCardsPinVerifVisa::CleanPinVerifVisaDialog();

	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCardsPinVerifVisa)
	enum { IDD = IDD_CARDS_PIN_VERIF_VISA };
	CEdit	m_CntlPVKLocTr2Fld;
	CEdit	m_CntlPVKLocTr1Fld;
	CEdit	m_CntlPVKLenTr2Fld;
	CEdit	m_CntlPVKLenTr1Fld;
	CEdit	m_CntlOffset2Fld;
	CEdit	m_CntlOffset1Fld;
	CEdit	m_CntlLocation2Fld;
	CEdit	m_CntlLocation1Fld;
	CEdit	m_CntlKeyRightFld;
	CEdit	m_CntlKeyLeftFld;
	CEdit	m_CntlKeyIndFld;
	CString	m_StrKeyIndFld;
	CString	m_StrKeyLeftFld;
	CString	m_StrKeyRightFld;
	CString	m_StrLocation1Fld;
	CString	m_StrLocation2Fld;
	CString	m_StrMessageLbl;
	CString	m_StrOffset1Fld;
	CString	m_StrOffset2Fld;
	CString	m_StrPVKLenTr1Fld;
	CString	m_StrPVKLenTr2Fld;
	CString	m_StrPVKLocTr1Fld;
	CString	m_StrPVKLocTr2Fld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardsPinVerifVisa)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCardsPinVerifVisa)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
