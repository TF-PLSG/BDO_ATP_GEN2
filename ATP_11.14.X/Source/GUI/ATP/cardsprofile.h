#if !defined(AFX_CARDSPROFILE_H__44F17612_8F72_4423_B86F_26A1680BBFDC__INCLUDED_)
#define AFX_CARDSPROFILE_H__44F17612_8F72_4423_B86F_26A1680BBFDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// cardsprofile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsProfile dialog

class CCardsProfile : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardsProfile)
// Construction
public:
	CCardsProfile(CWnd* pParent = NULL);   // standard constructor

	CAF01_EMV sProfileIdOld;
    int m_nCardMode;
    int m_nDialogMode;
	int nSelectedIndex;

    char strStatus[2];
    char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];  
	unsigned char m_PosEntryModes[2];


    BOOL m_bProcessing;
// Dialog Data
	//{{AFX_DATA(CCardsProfile)
	enum { IDD = IDD_CARDS_PROFILE };
	CEdit	m_CntlCardType;
	CEdit	m_CntlProfileId;
	CEdit	m_CntlCCLiCVC;
	CEdit	m_CntlCCLPanSeq;
	CEdit	m_CntlCCLServiceCode;
	CEdit	m_CntlCCTiCVC;
	CEdit	m_CntlCCTPanSeq;
	CEdit	m_CntlCCTServiceCode;
	CEdit	m_CntlMSCatc;
	CEdit	m_CntlMSCiCVC;
	CEdit	m_CntlMSCPanSeq;
	CEdit	m_CntlMSCServiceCode;
	CEdit	m_CntlMSCUnPredictableNo;
	CString	m_StrCCLiCVC;
	CString	m_StrCCLPanSeq;
	CString	m_StrCCLServiceCode;
	CString	m_StrCCTiCVC;
	CString	m_StrCCTPanSeq;
	CString	m_StrCCTServiceCode;
	CString	m_StrMSCatc;
	CString	m_StrMSCiCVC;
	CString	m_StrMSCPanSeq;
	CString	m_StrMSCServiceCode;
	CString	m_StrMSCUnPredictableNo;
	CEdit	m_CntlCCLiCVC2;
	CEdit	m_CntlCCLPanSeq2;
	CEdit	m_CntlCCLServiceCode2;
	CEdit	m_CntlCCTiCVC2;
	CEdit	m_CntlCCTPanSeq2;
	CEdit	m_CntlCCTServiceCode2;
	CEdit	m_CntlMSCatc2;
	CEdit	m_CntlMSCiCVC2;
	CEdit	m_CntlMSCPanSeq2;
	CEdit	m_CntlMSCServiceCode2;
	CEdit	m_CntlMSCUnPredictableNo2;
	CString	m_StrCCLiCVC2;
	CString	m_StrCCLPanSeq2;
	CString	m_StrCCLServiceCode2;
	CString	m_StrCCTiCVC2;
	CString	m_StrCCTPanSeq2;
	CString	m_StrCCTServiceCode2;
	CString	m_StrMSCatc2;
	CString	m_StrMSCiCVC2;
	CString	m_StrMSCPanSeq2;
	CString	m_StrMSCServiceCode2;
	CString	m_StrMSCUnPredictableNo2;
	CString	m_StrProfileId_Fld;
	CString	m_StrCardType_Fld;
	CString	m_StrProfileId;
	CString	m_StrCardType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardsProfile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	

	public:

    //BOOL m_bMerchant;

    void CleanCardsProfileDialog();
	void EnableCardsProfileDialog(int nStatus);
    void PopulateHistoryTable();

    BOOL PopulateCardsProfileDialogFromDatabase( CAF01_EMV sProfileId );    
    BOOL PopulateDatabaseFromCardsProfile( pCAF01_EMV psProfileId );
    BOOL CompareStructures(pCAF01_EMV psProfileId);
    
	CTime c_time;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCardsProfile)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CARDSPROFILE_H__44F17612_8F72_4423_B86F_26A1680BBFDC__INCLUDED_)
