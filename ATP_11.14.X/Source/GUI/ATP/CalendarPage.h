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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CalendarPage.h  $
   
      Rev 1.6   Dec 01 2003 11:28:46   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:43:56   iarustam
   Added manual batch entry GUI
   
      Rev 1.1   Dec 18 1998 13:42:32   iarustam
    
*
************************************************************************************/
// CalendarPage.h : header file
//

#include "CalendarButton.h"
#include "Day.h"

/////////////////////////////////////////////////////////////////////////////
// CCalendarPage dialog

class CCalendarPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCalendarPage)

// Construction
public:
	CCalendarPage();
	~CCalendarPage();

// Dialog Data
	//{{AFX_DATA(CCalendarPage)
	enum { IDD = IDD_CALENDARPAGE };
	CStatic	m_Year;
	CStatic	m_Wednesday;
	CStatic	m_Tuesday;
	CStatic	m_Thursday;
	CStatic	m_Sunday;
	CStatic	m_Saturday;
	CStatic	m_Month;
	CStatic	m_Monday;
	CStatic	m_MessageDisplay;
	CStatic	m_Friday;
	CCalendarButton	m_Button9;
	CCalendarButton	m_Button8;
	CCalendarButton	m_Button7;
	CCalendarButton	m_Button6;
	CCalendarButton	m_Button5;
	CCalendarButton	m_Button42;
	CCalendarButton	m_Button41;
	CCalendarButton	m_Button40;
	CCalendarButton	m_Button4;
	CCalendarButton	m_Button39;
	CCalendarButton	m_Button38;
	CCalendarButton	m_Button37;
	CCalendarButton	m_Button36;
	CCalendarButton	m_Button35;
	CCalendarButton	m_Button34;
	CCalendarButton	m_Button33;
	CCalendarButton	m_Button32;
	CCalendarButton	m_Button31;
	CCalendarButton	m_Button30;
	CCalendarButton	m_Button3;
	CCalendarButton	m_Button29;
	CCalendarButton	m_Button28;
	CCalendarButton	m_Button27;
	CCalendarButton	m_Button26;
	CCalendarButton	m_Button25;
	CCalendarButton	m_Button24;
	CCalendarButton	m_Button23;
	CCalendarButton	m_Button22;
	CCalendarButton	m_Button21;
	CCalendarButton	m_Button20;
	CCalendarButton	m_Button2;
	CCalendarButton	m_Button19;
	CCalendarButton	m_Button18;
	CCalendarButton	m_Button17;
	CCalendarButton	m_Button16;
	CCalendarButton	m_Button15;
	CCalendarButton	m_Button14;
	CCalendarButton	m_Button13;
	CCalendarButton	m_Button12;
	CCalendarButton	m_Button11;
	CCalendarButton	m_Button10;
	CCalendarButton	m_Button1;
	CString m_strBtn15;
	CString m_strBtn16;
	CString m_strBtn17;
	CString m_strBtn18;
	CString m_strBtn19;
	CString m_strBtn20;
	CString m_strBtn21;

	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCalendarPage)
	public:
	virtual BOOL DestroyWindow();
	virtual void OnOK();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
//protected:
	public:
	
	CCalendarButton* m_DateButtons[42];
	 int m_ThisMonth;
	 int m_ThisYear;
	 int m_ThisDay;
	 int m_DayCount;
	 CDay* m_ThisMonthsDays;
	 BOOL m_NewData;
	 CString displaystr;
	 int m_nDialogMode;

	int CalcMonthLength( int Month);	
	void SetColumnButtons(int day);
	void SetColumnJobs(int day, CString job);
	void SetDefaultWorkingDays(void);
	void InitializeThisMonthsDays(void);
	BOOL GetDateData(int month, int year );
	BOOL CheckDialogForCompleteness();
	void DisplayDateData ( void);
	BYTE PopulateHistoryTable(int,int,int);
	CTime c_time;

	CString New_profile;
	// Generated message map functions
	//{{AFX_MSG(CCalendarPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnNextmonth();
	afx_msg void OnPreviousmonth();
	afx_msg void OnSundayClicked();
	afx_msg void OnMondayClicked();
	afx_msg void OnTuesdayClicked();
	afx_msg void OnWednesdayClicked();
	afx_msg void OnThursdayClicked();
	afx_msg void OnFridayClicked();
	afx_msg void OnSaturdayClicked();
	afx_msg void OnSaveDateData();
	//}}AFX_MSG
	afx_msg void OnCalendarButtonRClicked(UINT) ;
	afx_msg void OnCalendarButtonLClicked(UINT) ;



	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedDatebutton6();
	afx_msg void OnBnClickedDatebutton21();
};
