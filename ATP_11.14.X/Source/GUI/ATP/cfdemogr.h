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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\cfdemogr.h  $
   
      Rev 1.4   Dec 01 2003 11:29:04   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.3   Jan 05 2000 14:55:00   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:04:02   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:38   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:50   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:34   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:02   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:42   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:36   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:48:16   iarustam
    
*
************************************************************************************/
// CFDemogr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCFDemogr dialog
#ifndef DEMOGRPAGE
#define DEMOGRPAGE


class CCFDemogr : public CPropertyPage
{
	DECLARE_DYNCREATE(CCFDemogr)

// Construction
public:
	CCFDemogr();
	~CCFDemogr();

	int DialogMode;
	BOOL m_bCCF01;
	BOOL m_bDemogrChanged;
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCFDemogr)
	enum { IDD = IDD_CARDDEMOGR };
	CEdit	m_CntlSpecialtyCodeFld;
	CEdit	m_CntlVIPAccountCodeFld;
	CEdit	m_CntlMembershipIDFld;
	CEdit	m_CntlMaidenNameFld;
	CEdit	m_CntlCardholderIDFld;
	CEdit	m_CntlBlockStatusFld;
	CEdit	m_CardNbr;
	CEdit	m_CardType;
	CEdit	m_Zip;
	CEdit	m_WorkPhone;
	CEdit	m_State;
	CEdit	m_Profession;
	CEdit	m_Name;
	CEdit	m_HomePhone;
	CEdit	m_DOB;
	CEdit	m_Country;
	CEdit	m_City;
	CEdit	m_Addr2;
	CEdit	m_Addr1;
	CString	m_strAddr1;
	CString	m_strAddr2;
	CString	m_strCardNbr;
	CString	m_strCardType;
	CString	m_strCity;
	CString	m_strCountry;
	CString	m_strHomePhone;
	CString	m_strName;
	CString	m_strState;
	CString	m_strWorkPhone;
	CString	m_strZip;
	CString	m_strProfession;
	CString	m_strDOB;
	CString	m_strComEdit;
	CString	m_StrBlockStatusFld;
	CString	m_StrCardholderIDFld;
	CString	m_StrMaidenNameFld;
	CString	m_StrMembershipIDFld;
	CString	m_StrSpecialtyCodeFld;
	CString	m_StrVIPAccountCodeFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCFDemogr)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCFDemogr)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeAddr1();
	afx_msg void OnChangeAddr2();
	afx_msg void OnChangeCity();
	afx_msg void OnChangeCountry();
	afx_msg void OnChangeDob();
	afx_msg void OnChangeHomephone();
	afx_msg void OnChangeName();
	afx_msg void OnChangeProfession();
	afx_msg void OnChangeState();
	afx_msg void OnChangeWorkphone();
	afx_msg void OnChangeZip();
	afx_msg void OnChangeBlStatusFld();
	afx_msg void OnChangeCardholderIdFld();
	afx_msg void OnChangeMaidenNameFld();
	afx_msg void OnChangeMembershipIdFld();
	afx_msg void OnChangeSpecialtyCodeFld();
	afx_msg void OnChangeVipAccountCodeFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif