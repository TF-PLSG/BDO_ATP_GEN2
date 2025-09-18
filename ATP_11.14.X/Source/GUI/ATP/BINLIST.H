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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\BINLIST.H  $
   
      Rev 1.7   Dec 01 2003 11:28:44   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 05 2000 14:52:48   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Oct 15 1999 17:02:28   iarustam
   bug fixes
   
      Rev 1.1   Sep 22 1999 15:39:50   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:48   iarustam
   Initial Release
   
 
   
*
************************************************************************************/
// BinList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBinList dialog

class CBinList : public CDialog
{
// Construction
public:
	CBinList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBinList)
	enum { IDD = IDD_BIN_NAME };
	CButton	m_AddButton;
	CButton	m_DeleteButton;
	CButton	m_DetailsButton;
	CButton	m_CntlNextButton;
	CListCtrl	m_CntlList;
	CString	m_StrMessageLbl;
	//}}AFX_DATA
	DEF01 sDEFRecord;
	DEF01_GUI_LIST sDEFList;

	BIN01 sBinRecord;
	BIN01_GUI_LIST sBinList;

	CR01 sCR01;

	MCF01 sMerchantRecord;
	MCF01_GUI_LIST sMerchantList;

	OBS01 sObs01;
	OBS01_GUI_LIST sObsList;

	JCB3DS2 sjcb3ds2;
	JCB3DS2_GUI_LIST sjcb3ds2List;

	VISA3DS2 sVisa3ds2;
	VISA3DS2_GUI_LIST sVisa3ds2List;

	CCL01 sccl01;
	CCL01_GUI_LIST sccl01List;

	BDKKEY01 sbdkkey01;
	BDKKEY01_GUI_LIST sbdkkey01List;

	CR01_GUI_LIST sCR01List;

	BCH10_GUI_LIST sBatchList;
	BCH10 sBatchRecord;

	NBF01_GUI_LIST sNegBinList;
	NBF01 sNegBinRecord;

	ACF01_GUI_LIST sACFList;
	ACF01 sACFRecord;

	CCF04_GUI_LIST sCCFList;
	CCF04 sCCFRecord; 

	DCF01 sTerminalRecord;
	DCF01_GUI_LIST sTerminalList;

	PAF01 sPAFRecord;
	PAF01_GUI_LIST sPAFList;
	CString m_strPan;

	CAF01 sCAFRecord;
	CAF01_GUI_LIST sCAFList;
	CString m_strProfId;
	CString m_strType;

	CSF01 sCSFRecord;
	CSF01_GUI_LIST sCSFList;
	CString m_strStatus;

	CCF02P sCCF02Record;
	CCF02P_GUI_LIST sCCF02List;
	CString m_strCardNbr;
	CString m_strCardType;

	OMC01 sOMC01Record;
	OMC01_GUI_LIST sOMC01List;

	MGF01 sMGF01Record;
	MGF01_GUI_LIST sMGF01List;

	MPPARA01 sMPPARA01Record;
	MPPARA01_GUI_LIST sMPPARA01_GUI_LISTList;

	BOOL bNext;
	BOOL bDone;
	BOOL m_bParam;
	int m_nDialogMode;
   int m_nViewMode;
	CString strItem0;
	CString strItem1;
	CString strItem2;
	CString strItem3;
   char m_strBatchStatus[3];
	void PopulateBinListDialog( int );
	void PopulateBinRuleListDialog( int );
	void PopulateMerchantListDialog();
	void PopulateObsListDialog();
	void PopulateVisa3ds2ListDialog();
	void PopulateJcb3ds2ListDialog();
	void PopulateCcl01ListDialog();
	void PopulateBdkkey01ListDialog();
	void PopulateBatchListDialog();
	void PopulateTerminalListDialog();
	void PopulatePAFListDialog();
	void PopulateCAFListDialog();
	void PopulateCSFListDialog();
	void PopulateOMCListDialog();
	void PopulateCCF02ListDialog( int );
	void PopulateNegBinListDialog();
	void PopulateACFListDialog( int );
	void PopulateCCF04ListDialog( int );
	void PopulateDEFListDialog();
	void PopulateMGF01ListDialog();
	void PopulateMPPARA01ListDialog ();
	void PopulateRuleIDListDialog();
	CTime c_time;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBinList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBinList)
	afx_msg void OnNextButton();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkNameList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDetailsButton();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
