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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\cfgeneral.h  $
   
      Rev 1.10   Apr 01 2005 10:13:40   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.9   Dec 01 2003 11:29:04   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   Nov 09 2001 16:46:26   lmitchel
   GUI support for RSB cards - Added 2 new fields, add_on_rate, add_on_term to card general page including support for add, update, view, change history
   
      Rev 1.7   Jan 25 2000 14:01:08   iarustam
   bug fixes
   
      Rev 1.6   Jan 05 2000 14:55:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Oct 19 1999 11:52:42   iarustam
   bug fixes
   
      Rev 1.1   Sep 22 1999 16:04:16   iarustam
   Added manual batch entry GUI
   
      Rev 1.5   Jul 08 1999 16:19:50   iarustam
   Bug fixes
   
      Rev 1.0   Jul 01 1999 14:18:38   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:52   iarustam
   Initial Release
   
      Rev 1.4   May 21 1999 10:18:02   iarustam
   Bug fixes
   
   
      Rev 1.3   Apr 29 1999 14:57:20   iarustam
   Bug fixes
   
      Rev 1.2   Mar 22 1999 11:13:10   iarustam
   Bug fixes
   
   
      Rev 1.1   Mar 16 1999 16:30:42   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:34   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:02   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:44   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:40   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:24   iarustam
    
*
************************************************************************************/
// CFGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCFGeneral dialog
#ifndef GENPAGE
#define GENPAGE


class CCFGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CCFGeneral)

// Construction
public:
	CCFGeneral();
	~CCFGeneral();

	int DialogMode;
	BOOL bFoundCCF03 ;
	CTime c_time;

/*static void CCFGeneral::SetDirty()
{
	SetModified(TRUE);
}
*/
// Dialog Data
	//{{AFX_DATA(CCFGeneral)
	enum { IDD = IDD_CARDGENERAL };
	CEdit	m_CntlPanSeqNbr;
	CEdit	m_cntlSecExpDate;
	CEdit	m_ctrlPesoTranLimit;
	CEdit	m_ctrlDollarTranLimit;
	CEdit	m_cntlRsbTerm;
	CEdit	m_cntlRsbRate;
	CEdit	m_CntlPinOffset;
	CButton	m_VisaPurchase;
	CEdit	m_CntlPinRetryFld;
	CEdit	m_CntlCode9Fld;
	CEdit	m_CntlCode8Fld;
	CEdit	m_CntlCode7Fld;
	CEdit	m_CntlCode6Fld;
	CEdit	m_CntlCode5Fld;
	CEdit	m_CntlCode4Fld;
	CEdit	m_CntlCode3Fld;
	CEdit	m_CntlCode2Fld;
	CEdit	m_CntlCode10Fld;
	CEdit	m_CntlCode1Fld;
	CButton	m_CashBonus;
	CEdit	m_ctrlVipProf;
	CButton	m_ctrlStatList;
	CButton	m_ctrlProfList;
	CEdit	m_ctrlPan;
	CEdit	m_ctrlExpDate;
	CEdit	m_ctrlEffDate;
	CEdit	m_ctrlCurCode;
	CEdit	m_ctrlCreditLine;
	CEdit	m_ctrlCountryCode;
	CEdit	m_ctrlCardStatus;
	CStatic	m_ComEdit;
	CString	m_strCardNbr;
	CString	m_strCardType;
	CString	m_strCardStatus;
	CString	m_strEffDate;
	CString	m_strExpDate;
	CString	m_strPAN;
	CString	m_strPinOffset;
	CString	m_strCreditLine;
	CString	m_strCurCode;
	CString	m_strVipProf;
	CString	m_strOutstAuth;
	CString	m_strCountryCode;
	CString	m_strComEdit;
	CString	m_StrCode1Fld;
	CString	m_StrCode10Fld;
	CString	m_StrCode2Fld;
	CString	m_StrCode3Fld;
	CString	m_StrCode4Fld;
	CString	m_StrCode5Fld;
	CString	m_StrCode6Fld;
	CString	m_StrCode7Fld;
	CString	m_StrCode8Fld;
	CString	m_StrCode9Fld;
	CString	m_StrPesoCurrentFld;
	CString	m_StrPesoSavingsFld;
	CString	m_StrDollarCurrentFld;
	CString	m_StrDollarSavingsFld;
	CString	m_StrPinRetryFld;
	BOOL	m_BoolCashBonus;
	BOOL	m_BoolVisaPurchase;
	CString	m_strRsbRate;
	CString	m_strRsbTerm;
	CString	m_strDollarTranLimit;
	CString	m_strPesoTranLimit;
	CString	m_strSecExpDate;
	CString	m_StrPanSeqNbr;
	CString m_StrMobNbr ;

	//}}AFX_DATA
	

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCFGeneral)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCFGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnProflist();
	afx_msg void OnStatlist();
	afx_msg void OnChangeCardstatus();
	afx_msg void OnChangeCountrycode();
	afx_msg void OnChangeCreditline();
	afx_msg void OnChangeCurcode();
	afx_msg void OnChangeEffectivedate();
	afx_msg void OnChangeExpdate();
	afx_msg void OnChangePan();
	afx_msg void OnChangeVipprof();
	afx_msg void OnCashBonus();
	afx_msg void OnChangeCode1Fld();
	afx_msg void OnChangeCode10Fld();
	afx_msg void OnChangeCode2Fld();
	afx_msg void OnChangeCode3Fld();
	afx_msg void OnChangeCode4Fld();
	afx_msg void OnChangeCode5Fld();
	afx_msg void OnChangeCode6Fld();
	afx_msg void OnChangeCode7Fld();
	afx_msg void OnChangeCode8Fld();
	afx_msg void OnChangeCode9Fld();
	afx_msg void OnChangePinRetryFld();
	afx_msg void OnVisaPurchase();
	afx_msg void OnChangePinoffset();
	afx_msg void OnChangeRsbrate();
	afx_msg void OnChangeRsbterm();
	afx_msg void OnKillfocusRsbrate();
	afx_msg void OnChangePesoTranLimit();
	afx_msg void OnChangeDollarTranLimit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
