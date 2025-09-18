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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardsTime3a.h  $
   
      Rev 1.6   Dec 01 2003 11:28:58   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:12   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:46:30   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:10   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:16   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:08   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:29:42   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:14   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:16   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:18   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:26   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:56   iarustam
    
*
************************************************************************************/
// CardsTime3a.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsTime3a dialog
#ifndef CCARDSTIME3A
#define CCARDSTIME3A


class CCardsTime3a : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardsTime3a)

// Construction
public:
	CCardsTime3a();
	~CCardsTime3a();

    CAF01 sProfileIdOld;
    CAF01 sProfileId;

    int m_nDialogMode;
    int nDays3;

    char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];

    BOOL m_bTime3;

    void CleanTime3Dialog();
    void EnableTime3Dialog(int);
    void PopulateHistoryTable();

    BOOL PopulateTime3DialogFromDatabase( CAF01 sProfileId );
    BOOL PopulateDatabaseFromTime3Dialog( pCAF01 psProfileId );
    BOOL CompareStructures();

	
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCardsTime3a)
	enum { IDD = IDD_CARDS_TIME3 };
	CEdit	m_CntlProfileId_Fld;
	CEdit	m_CntlType_Fld;
	CEdit	m_CntlNumber3A_Fld;
	CEdit	m_CntlNumber3_Fld;
	CEdit	m_CntlDays3_Fld;
	CEdit	m_CntlAmount3A_Fld;
	CEdit	m_CntlAmount3_Fld;
	CString	m_StrAmount3_Fld;
	CString	m_StrAmount3A_Fld;
	CString	m_StrDays3_Fld;
	CString	m_StrNumber3_Fld;
	CString	m_StrNumber3A_Fld;
	CString	m_StrType_Fld;
	CString	m_StrProfileId_Fld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardsTime3a)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardsTime3a)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusDays3Fld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
