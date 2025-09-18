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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardsVelocity.h  $
   
      Rev 1.6   Dec 01 2003 11:29:00   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:14   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:46:44   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:10   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:18   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:08   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:29:44   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:14   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:18   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:20   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:30   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:58   iarustam
    
*
************************************************************************************/
// CardsVelocity.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsVelocity dialog
#ifndef CCARDSVELOCITY
#define CCARDSVELOCITY

class CCardsVelocity : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardsVelocity)

// Construction
public:
	CCardsVelocity();
	~CCardsVelocity();

    CAF01 sProfileIdOld;
    FAF01 sFleetCardOld;
    CAF01 sProfileId;
    FAF01 sFleetCard;

    int m_nDialogMode;

    char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

    BOOL m_bVelocity;

    void EnableVelocityDialog(int nStatus);
    void CleanVelocityDialog();
    void PopulateHistoryTable();
    void OnUpdateAmountFld(CString myString);

    BOOL PopulateDatabaseFromVelocityDialog( pFAF01 psFleetCard );
    BOOL PopulateVelocityDialogFromDatabase(  FAF01 sFleetCard );
    BOOL CompareStructures();
    BOOL CheckTime(CString strTime);

	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCardsVelocity)
	enum { IDD = IDD_CARDS_VELOCITY };
	CEdit	m_CntlToTime1_Fld;
	CEdit	m_CntlToTime9_Fld;
	CEdit	m_CntlToTime8_Fld;
	CEdit	m_CntlToTime7_Fld;
	CEdit	m_CntlToTime6_Fld;
	CEdit	m_CntlToTime5_Fld;
	CEdit	m_CntlToTime4_Fld;
	CEdit	m_CntlToTime3_Fld;
	CEdit	m_CntlToTime2_Fld;
	CEdit	m_CntlToTime10_Fld;
	CEdit	m_CntlType_Fld;
	CEdit	m_CntlProfileId_Fld;
	CEdit	m_CntlFromTime9_Fld;
	CEdit	m_CntlFromTime8_Fld;
	CEdit	m_CntlFromTime7_Fld;
	CEdit	m_CntlFromTime6_Fld;
	CEdit	m_CntlFromTime5_Fld;
	CEdit	m_CntlFromTime4_Fld;
	CEdit	m_CntlFromTime3_Fld;
	CEdit	m_CntlFromTime10_Fld;
	CEdit	m_CntlFromTime1_Fld;
	CEdit	m_CntlFromTime2_Fld;
	CEdit	m_CntlAmount10_Fld;
	CEdit	m_CntlAmount1_Fld;
	CEdit	m_CntlAmount9_Fld;
	CEdit	m_CntlAmount8_Fld;
	CEdit	m_CntlAmount7_Fld;
	CEdit	m_CntlAmount6_Fld;
	CEdit	m_CntlAmount5_Fld;
	CEdit	m_CntlAmount4_Fld;
	CEdit	m_CntlAmount3_Fld;
	CEdit	m_CntlAmount2_Fld;
	CString	m_StrAmount1_Fld;
	CString	m_StrAmount2_Fld;
	CString	m_StrAmount3_Fld;
	CString	m_StrAmount4_Fld;
	CString	m_StrAmount5_Fld;
	CString	m_StrAmount6_Fld;
	CString	m_StrAmount7_Fld;
	CString	m_StrAmount8_Fld;
	CString	m_StrAmount9_Fld;
	CString	m_StrAmount10_Fld;
	CString	m_StrFromTime3_Fld;
	CString	m_StrFromTime2_Fld;
	CString	m_StrFromTime1_Fld;
	CString	m_StrFromTime4_Fld;
	CString	m_StrFromTime5_Fld;
	CString	m_StrFromTime6_Fld;
	CString	m_StrFromTime7_Fld;
	CString	m_StrFromTime8_Fld;
	CString	m_StrFromTime9_Fld;
	CString	m_StrFromTime10_Fld;
	CString	m_StrType_Fld;
	CString	m_StrProfileId_Fld;
	CString	m_StrToTime10_Fld;
	CString	m_StrToTime2_Fld;
	CString	m_StrToTime3_Fld;
	CString	m_StrToTime4_Fld;
	CString	m_StrToTime5_Fld;
	CString	m_StrToTime6_Fld;
	CString	m_StrToTime7_Fld;
	CString	m_StrToTime8_Fld;
	CString	m_StrToTime9_Fld;
	CString	m_StrMessageLbl;
	CString	m_StrToTime1_Fld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardsVelocity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardsVelocity)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusFrom1Fld();
	afx_msg void OnKillfocusFrom2Fld();
	afx_msg void OnKillfocusFrom3Fld();
	afx_msg void OnKillfocusFrom4Fld();
	afx_msg void OnKillfocusFrom5Fld();
	afx_msg void OnKillfocusFrom6Fld();
	afx_msg void OnKillfocusFrom7Fld();
	afx_msg void OnKillfocusFrom8Fld();
	afx_msg void OnKillfocusFrom9Fld();
	afx_msg void OnKillfocusFrom10Fld();
	afx_msg void OnKillfocusTo1Fld();
	afx_msg void OnKillfocusTo2Fld();
	afx_msg void OnKillfocusTo3Fld();
	afx_msg void OnKillfocusTo4Fld();
	afx_msg void OnKillfocusTo5Fld();
	afx_msg void OnKillfocusTo6Fld();
	afx_msg void OnKillfocusTo7Fld();
	afx_msg void OnKillfocusTo8Fld();
	afx_msg void OnKillfocusTo9Fld();
	afx_msg void OnKillfocusTo10Fld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
