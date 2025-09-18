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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardsTime2.h  $
   
      Rev 1.6   Dec 01 2003 11:28:58   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:12   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:46:20   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:08   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:16   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:06   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:29:42   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:14   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:16   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:16   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:24   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:54   iarustam
    
*
************************************************************************************/
// CardsTime2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsTime2 dialog
#ifndef CCARDSTIME2
#define CCARDSTIME2


class CCardsTime2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardsTime2)

// Construction
public:
	CCardsTime2();
	~CCardsTime2();

    CAF01 sProfileIdOld;
    CAF01 sProfileId;

    int m_nDialogMode;
    int nDays2;

    char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

    BOOL m_bTime2;

    void CleanTime2Dialog();
    void EnableTime2Dialog(int);
    void PopulateHistoryTable();

    BOOL PopulateTime2DialogFromDatabase( CAF01 sProfileId );
    BOOL PopulateDatabaseFromTime2Dialog( pCAF01 psProfileId );
    BOOL CompareStructures();   

	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCardsTime2)
	enum { IDD = IDD_CARDS_TIME2 };
	CEdit	m_CntlProfileId_Fld;
	CEdit	m_CntlType_Fld;
	CEdit	m_CntlNumber2A_Fld;
	CEdit	m_CntlNumber2_Fld;
	CEdit	m_CntlDays2_Fld;
	CEdit	m_CntlAmount2A_Fld;
	CEdit	m_CntlAmount2_Fld;
	CString	m_StrAmount2_Fld;
	CString	m_StrAmount2A_Fld;
	CString	m_StrDays2_Fld;
	CString	m_StrNumber2_Fld;
	CString	m_StrNumber2A_Fld;
	CString	m_StrType_Fld;
	CString	m_StrProfileId_Fld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardsTime2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardsTime2)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusDays2Fld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
