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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\NAMELIST.H  $
   
      Rev 1.7   Sep 05 2006 10:46:32   dirby
   Added code for a new Cash Bonus program (see CB_... files)
   SCR 22385
   
      Rev 1.6   Jul 07 2005 15:49:24   lmitchel
   Implementation of Fraud Guard Requirements - New Fraud Guard sub  menu under Network Definition
   
      Rev 1.5   Dec 01 2003 11:29:48   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:55:34   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:56   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:02   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:44   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:50   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:44   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:34   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:04   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:28   iarustam
    
*
************************************************************************************/
// NameList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNameList dialog




class CNameList : public CDialog
{
// Construction
public:
	CNameList(CWnd* pParent = NULL);   // standard constructor
   
	int nNumberReturned;
	CString strBinType;
	CString m_strName;
	CString strPrimaryKey1;
	CString strNetworkType;

    int m_nIndicator;
	int m_top_index;
    
	UCF01_LIST sUserList;
	UCF01 sUserRecord;
	//UCF03 snewUserRecord;


    UCF02 sGroupRecord;
    UCF02_LIST sGroupList;

    NGF01 sNegGroupRecord;
    NGF01_GUI_LIST sNegGroupList;
	NGF01_LIST sNGFList;

    ATP02 sWorkstationRecord;
    ATP02_GUI_LIST sWorkstationList;

	CAF01 sAuthRecord;
	CAF01_GUI_LIST sAuthList;

	CTF01 sCardRecord;
	CTF01_GUI_LIST sCardList;

//	PTF01 sPrefixRecord;
//	PTF01_GUI_LIST sPrefixList;

    CRF01 sCurrencyCode;
    CRF01_GUI_LIST sConversionList;

	NCF01 sNetworkId;	
	NCF01_GUI_LIST sNetworkList;

	FG01  sFraudGuardRecord;
	FG01_GUI_LIST sFraudGuardList;

	BON02  sBon02Record;
	BON02_GUI_LIST sBon02List;

	FCLF01 fclf01networkid;
	FCLF01_GUI_LIST fclf01networkidlist;

	FRULE01 FRULE01networkid;
	FRULE01_GUI_LIST frulenetworkidlist;
	
	CAF01_KEYBLOCK sCaf01Keyblock01;
	CAF01_KEYBLOCK_GUI_LIST sCaf01Keyblock01List;

#ifndef AAA_HEADER
#define AAA_HEADER
	NCF01_KEYBLOCK sNCF01keyblock01;
	NCF01_KEYBLOCK_GUI_LIST sNCF01keyblockList;
#endif

	CTime c_time;


// Dialog Data
	//{{AFX_DATA(CNameList)
	enum { IDD = IDD_LIST };
	CButton	m_OkButton;
	CButton	m_NextButton;
	CListBox	m_CntlList;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNameList)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkList1();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnNextButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
