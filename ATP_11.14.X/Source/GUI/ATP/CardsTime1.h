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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardsTime1.h  $
   
      Rev 1.6   Dec 01 2003 11:28:56   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:12   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:46:06   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:08   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:14   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:06   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:29:42   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:12   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:16   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:14   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:20   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:52   iarustam
    
*
************************************************************************************/
// CardsTime1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsTime1 dialog
#ifndef CCARDSTIME1
#define CCARDSTIME1

class CCardsTime1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardsTime1)

// Construction
public:
	CCardsTime1();
	~CCardsTime1();

    CAF01 sProfileIdOld;
    CAF01 sProfileId;

    int m_nDialogMode;
    int nDays1;

    char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

    BOOL m_bTime1;

    void CleanTime1Dialog();
    void EnableTime1Dialog(int);
    void PopulateHistoryTable();

    BOOL PopulateTime1DialogFromDatabase( CAF01 sProfileId );
    BOOL PopulateDatabaseFromTime1Dialog( pCAF01 psProfileId );
    BOOL CompareStructures();
    
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCardsTime1)
	enum { IDD = IDD_CARDS_TIME1 };
	CEdit	m_CntlAmount1A_Fld;
	CEdit	m_CntlAmount1_Fld;
	CEdit	m_CntlDays1_Fld;
	CEdit	m_CntlProfileId_Fld;
	CEdit	m_CntlType_Fld;
	CEdit	m_CntlNumber1A_Fld;
	CEdit	m_CntlNumber1_Fld;
	CString	m_StrAmount1_Fld;
	CString	m_StrAmount1A_Fld;
	CString	m_StrDays1_Fld;
	CString	m_StrNumber1_Fld;
	CString	m_StrNumber1A_Fld;
	CString	m_StrType_Fld;
	CString	m_StrProfileId_Fld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardsTime1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardsTime1)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusDays1Fld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
