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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CalendarPage.cpp  $
   
      Rev 1.9   Dec 01 2003 11:28:46   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   Feb 01 2000 17:48:16   iarustam
   SCR # 333,340
   
      Rev 1.7   Jan 05 2000 14:53:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:43:52   iarustam
   Added manual batch entry GUI
   
      Rev 1.1   Dec 18 1998 13:42:30   iarustam
    
*
************************************************************************************/

// CalendarPage.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CalendarPage.h"
#include "JobDefDlg.h"
#include <string.h>
#include <memory.h>
#include "MyUtils.h"
#include "SharedMem.h"

extern "C"
{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSharedMem shrmem;

extern char login_name[51];
// for day chk
int day_chk;
CString strday;
CString wk_day;
int offset;
char numbuf[3];

 typedef struct 
	{
		
		unsigned char	workingday[2];
		unsigned char	profileid[16];

	}datedata;

/*typedef struct
 {
	
	struct
	{
		unsigned char year[5];
		unsigned char month[3];

	}primary_key;

	datedata day[31];
}CLF01;
  */
 

/////////////////////////////////////////////////////////////////////////////
// CCalendarPage property page

IMPLEMENT_DYNCREATE(CCalendarPage, CPropertyPage)

CCalendarPage::CCalendarPage() : CPropertyPage(CCalendarPage::IDD)
{
	//{{AFX_DATA_INIT(CCalendarPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_ThisMonthsDays = NULL;
	m_NewData = FALSE;
}

CCalendarPage::~CCalendarPage()
{
}

void CCalendarPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarPage)
	DDX_Control(pDX, IDC_YEAR, m_Year);
	DDX_Control(pDX, IDC_WEDNESDAY, m_Wednesday);
	DDX_Control(pDX, IDC_TUESDAY, m_Tuesday);
	DDX_Control(pDX, IDC_THURSDAY, m_Thursday);
	DDX_Control(pDX, IDC_SUNDAY, m_Sunday);
	DDX_Control(pDX, IDC_SATURDAY, m_Saturday);
	DDX_Control(pDX, IDC_MONTH, m_Month);
	DDX_Control(pDX, IDC_MONDAY, m_Monday);
	DDX_Control(pDX, IDC_MESSAGEDISPLAY, m_MessageDisplay);
	DDX_Control(pDX, IDC_FRIDAY, m_Friday);
	DDX_Control(pDX, IDC_DATEBUTTON9, m_Button9);
	DDX_Control(pDX, IDC_DATEBUTTON8, m_Button8);
	DDX_Control(pDX, IDC_DATEBUTTON7, m_Button7);
	DDX_Control(pDX, IDC_DATEBUTTON6, m_Button6);
	DDX_Control(pDX, IDC_DATEBUTTON5, m_Button5);
	DDX_Control(pDX, IDC_DATEBUTTON42, m_Button42);
	DDX_Control(pDX, IDC_DATEBUTTON41, m_Button41);
	DDX_Control(pDX, IDC_DATEBUTTON40, m_Button40);
	DDX_Control(pDX, IDC_DATEBUTTON4, m_Button4);
	DDX_Control(pDX, IDC_DATEBUTTON39, m_Button39);
	DDX_Control(pDX, IDC_DATEBUTTON38, m_Button38);
	DDX_Control(pDX, IDC_DATEBUTTON37, m_Button37);
	DDX_Control(pDX, IDC_DATEBUTTON36, m_Button36);
	DDX_Control(pDX, IDC_DATEBUTTON35, m_Button35);
	DDX_Control(pDX, IDC_DATEBUTTON34, m_Button34);
	DDX_Control(pDX, IDC_DATEBUTTON33, m_Button33);
	DDX_Control(pDX, IDC_DATEBUTTON32, m_Button32);
	DDX_Control(pDX, IDC_DATEBUTTON31, m_Button31);
	DDX_Control(pDX, IDC_DATEBUTTON30, m_Button30);
	DDX_Control(pDX, IDC_DATEBUTTON3, m_Button3);
	DDX_Control(pDX, IDC_DATEBUTTON29, m_Button29);
	DDX_Control(pDX, IDC_DATEBUTTON28, m_Button28);
	DDX_Control(pDX, IDC_DATEBUTTON27, m_Button27);
	DDX_Control(pDX, IDC_DATEBUTTON26, m_Button26);
	DDX_Control(pDX, IDC_DATEBUTTON25, m_Button25);
	DDX_Control(pDX, IDC_DATEBUTTON24, m_Button24);
	DDX_Control(pDX, IDC_DATEBUTTON23, m_Button23);
	DDX_Control(pDX, IDC_DATEBUTTON22, m_Button22);
	DDX_Control(pDX, IDC_DATEBUTTON21, m_Button21);
	DDX_Control(pDX, IDC_DATEBUTTON20, m_Button20);
	DDX_Control(pDX, IDC_DATEBUTTON2, m_Button2);
	DDX_Control(pDX, IDC_DATEBUTTON19, m_Button19);
	DDX_Control(pDX, IDC_DATEBUTTON18, m_Button18);
	DDX_Control(pDX, IDC_DATEBUTTON17, m_Button17);
	DDX_Control(pDX, IDC_DATEBUTTON16, m_Button16);
	DDX_Control(pDX, IDC_DATEBUTTON15, m_Button15);
	DDX_Control(pDX, IDC_DATEBUTTON14, m_Button14);
	DDX_Control(pDX, IDC_DATEBUTTON13, m_Button13);
	DDX_Control(pDX, IDC_DATEBUTTON12, m_Button12);
	DDX_Control(pDX, IDC_DATEBUTTON11, m_Button11);
	DDX_Control(pDX, IDC_DATEBUTTON10, m_Button10);
	DDX_Control(pDX, IDC_DATEBUTTON1, m_Button1);
	DDX_Text(pDX,IDC_DATEBUTTON15, m_strBtn15);   // inorder to get the day while setting the profile Id
	DDX_Text(pDX,IDC_DATEBUTTON16, m_strBtn16);
	DDX_Text(pDX,IDC_DATEBUTTON17, m_strBtn17);
	DDX_Text(pDX,IDC_DATEBUTTON18, m_strBtn18);
	DDX_Text(pDX,IDC_DATEBUTTON19, m_strBtn19);
	DDX_Text(pDX,IDC_DATEBUTTON20, m_strBtn20);
	DDX_Text(pDX,IDC_DATEBUTTON21, m_strBtn21);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalendarPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCalendarPage)
	ON_BN_CLICKED(IDC_NEXTMONTH, OnNextmonth)
	ON_BN_CLICKED(IDC_PREVIOUSMONTH, OnPreviousmonth)
	ON_COMMAND(IDC_SUNDAY,OnSundayClicked)
	ON_COMMAND(IDC_MONDAY,OnMondayClicked)
	ON_COMMAND(IDC_TUESDAY,OnTuesdayClicked)
	ON_COMMAND(IDC_WEDNESDAY,OnWednesdayClicked)
	ON_COMMAND(IDC_THURSDAY,OnThursdayClicked)
	ON_COMMAND(IDC_FRIDAY,OnFridayClicked)
	ON_COMMAND(IDC_SATURDAY,OnSaturdayClicked)
	ON_BN_CLICKED(IDOK,OnSaveDateData)
	//}}AFX_MSG_MAP
	
	ON_CONTROL_RANGE(NM_RCLICK,IDC_DATEBUTTON15,IDC_DATEBUTTON42,CCalendarPage::OnCalendarButtonRClicked)
	ON_CONTROL_RANGE(NM_CLICK,IDC_DATEBUTTON15,IDC_DATEBUTTON42,CCalendarPage::OnCalendarButtonLClicked)
	//ON_CONTROL_RANGE(BN_CLICKED,IDC_DATEBUTTON15,IDC_DATEBUTTON42,CCalendarPage::OnCalendarButtonLClicked)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarPage message handlers

BOOL CCalendarPage::OnInitDialog() 
{
	int count;
	CPropertyPage::OnInitDialog();
	SetModified(TRUE);	
	// initialize the DateButton Array
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGEDISPLAY)->SetFont (&m_font);
	

	m_DateButtons[0] = &m_Button1;
	m_DateButtons[1] = &m_Button2;
	m_DateButtons[2] = &m_Button3;
	m_DateButtons[3] = &m_Button4;
	m_DateButtons[4] = &m_Button5;
	m_DateButtons[5] = &m_Button6;
	m_DateButtons[6] = &m_Button7;
	m_DateButtons[7] = &m_Button8;
	m_DateButtons[8] = &m_Button9;
	m_DateButtons[9] = &m_Button10;
	m_DateButtons[10] = &m_Button11;
	m_DateButtons[11] = &m_Button12;
	m_DateButtons[12] = &m_Button13;
	m_DateButtons[13] = &m_Button14;
	m_DateButtons[14] = &m_Button15;
	m_DateButtons[15] = &m_Button16;
	m_DateButtons[16] = &m_Button17;
	m_DateButtons[17] = &m_Button18;
	m_DateButtons[18] = &m_Button19;
	m_DateButtons[19] = &m_Button20;
	m_DateButtons[20] = &m_Button21;
	m_DateButtons[21] = &m_Button22;
	m_DateButtons[22] = &m_Button23;
	m_DateButtons[23] = &m_Button24;
	m_DateButtons[24] = &m_Button25;
	m_DateButtons[25] = &m_Button26;
	m_DateButtons[26] = &m_Button27;
	m_DateButtons[27] = &m_Button28;
	m_DateButtons[28] = &m_Button29;
	m_DateButtons[29] = &m_Button30;
	m_DateButtons[30] = &m_Button31;
	m_DateButtons[31] = &m_Button32;
	m_DateButtons[32] = &m_Button33;
	m_DateButtons[33] = &m_Button34;
	m_DateButtons[34] = &m_Button35;
	m_DateButtons[35] = &m_Button36;
	m_DateButtons[36] = &m_Button37;
	m_DateButtons[37] = &m_Button38;
	m_DateButtons[38] = &m_Button39;
	m_DateButtons[39] = &m_Button40;
	m_DateButtons[40] = &m_Button41;
	m_DateButtons[41] = &m_Button42;
	

	CTime Today;
	// get the current date, get the current year and month, determine what weekday the 
	// firstday of the month occured on and set the offset for the
	// button array
	
	Today = CTime::GetCurrentTime();
	m_ThisMonth = Today.GetMonth();
	m_ThisYear = Today.GetYear();
	CTime FirstDay(m_ThisYear,m_ThisMonth,1,0,0,0);
	offset =  FirstDay.GetDayOfWeek() -1;

	// set the month and year labels 
	m_Month.SetWindowText(Today.Format("%B"));
	m_Year.SetWindowText(Today.Format("%Y"));
	
	// get the number of days for this month

	m_DayCount = CalcMonthLength(m_ThisMonth); 

	// create the data array for this month

	m_ThisMonthsDays = new CDay[m_DayCount];

	
	// set up the day structure for this month

	InitializeThisMonthsDays();
	
	// blank the day buttons
	for (int count = 0; count < 42; count++)
   {
		m_DateButtons[count]->SetWindowText(" ");
      m_DateButtons[count]->EnableWindow(FALSE);
   }
	// write the day number on the appropriate button
	
	for ( count = 1; count <= m_DayCount; count++)
   {
      m_DateButtons[count+offset-1]->EnableWindow(TRUE);
		m_DateButtons[count+offset-1]->SetWindowText(itoa(count,numbuf,10));
   }
	// grab this months data from the data server and display it, if no
	//data exists for this month set the default working days

	if (GetDateData( m_ThisMonth, m_ThisYear))
	{
		DisplayDateData();
		m_NewData = FALSE;
	}

	else
	
		SetDefaultWorkingDays();



	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}

//================================================================================================

int CCalendarPage::CalcMonthLength( int Month)
{
	int mlength;
	
	switch( Month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:	mlength = 31;
					break;
		case 4:
		case 6:
		case 9:
		case 11:	mlength = 30;
					break;

		// its february, see if it is a leap year
		case 2:		int leapYear = m_ThisYear - 1992;
					leapYear %= 4; 
					if(leapYear == 0)
						mlength = 29;
					else
						mlength = 28;


	}

	return mlength;
}


//================================================================================================
void CCalendarPage::OnNextmonth() 
{ 	
	int count;
	if ( 12 == m_ThisMonth)
	{
		m_ThisMonth = 1;
		m_ThisYear += 1;
	}
	else 
		m_ThisMonth +=1;
	
	CTime FirstDay(m_ThisYear,m_ThisMonth,1,0,0,0);
	offset =  FirstDay.GetDayOfWeek() -1;
	m_Month.SetWindowText(FirstDay.Format("%B"));
	m_Year.SetWindowText(FirstDay.Format("%Y"));
	m_DayCount = CalcMonthLength(m_ThisMonth); 
	
	// delete the previous months data 
	
	delete[] m_ThisMonthsDays;
	
	// create the data array for this month
	
	m_ThisMonthsDays = new CDay[m_DayCount];
	
		
	// set up the day structure for this month
	
	InitializeThisMonthsDays();
	
	
	// blank the day buttons
	for (int count = 0; count < 42; count++)
	{
		m_DateButtons[count]->SetWindowText(" ");
		m_DateButtons[count]->EnableWindow(FALSE);
	}
	
	// set the buttons to the cleared state
	for (count = 0; count < 42; count++)
		m_DateButtons[count]->SetCheck(0);
	
		
	// write the days for the new month to the buttons
	for (count = 1; count <= m_DayCount; count++)
	{
		m_DateButtons[count+offset-1]->EnableWindow(TRUE);
		m_DateButtons[count+offset-1]->SetWindowText(itoa(count,numbuf,10));
	}
	
	// grab this months data from the data server and display it, if no
	//data exists for this month set the default working days
	
	if (GetDateData( m_ThisMonth, m_ThisYear))
	{
		DisplayDateData();
		m_NewData = FALSE;
	}
	
	else
		SetDefaultWorkingDays();

}

//============================================================================================

void CCalendarPage::OnPreviousmonth() 
{
	int count;
	if ( 1 == m_ThisMonth)
	{
		m_ThisMonth = 12;
		m_ThisYear -= 1;
	}
	else 
		m_ThisMonth -=1;
	
	CTime FirstDay(m_ThisYear,m_ThisMonth,1,0,0,0);
	offset =  FirstDay.GetDayOfWeek() -1;
	m_Month.SetWindowText(FirstDay.Format("%B"));
	m_Year.SetWindowText(FirstDay.Format("%Y"));
	m_DayCount = CalcMonthLength(m_ThisMonth);
	
	// delete the previous months data 
	
	delete[] m_ThisMonthsDays;
	
	// create the data array for this month
	
	m_ThisMonthsDays = new CDay[m_DayCount];
	
		
	// set up the day structure for this month
	
	InitializeThisMonthsDays();
	
	// blank the day buttons
	for (int count = 0; count < 42; count++)
	{
		m_DateButtons[count]->SetWindowText(" ");
		m_DateButtons[count]->EnableWindow(FALSE);
	}
		
	// set the buttons to the cleared state
	for (count = 0; count < 42; count++)
		m_DateButtons[count]->SetCheck(0);
	
	
	
	// write the days for the new month to the buttons
	for (count = 1; count <= m_DayCount; count++)
	{
		m_DateButtons[count+offset-1]->EnableWindow(TRUE);
		m_DateButtons[count+offset-1]->SetWindowText(itoa(count,numbuf,10));
	}
	
	// grab this months data from the data server and display it, if no
	//data exists for this month set the default working days
	
	if (GetDateData( m_ThisMonth, m_ThisYear))
	{
		DisplayDateData();
		m_NewData = FALSE;
	}
	else
		SetDefaultWorkingDays();
	
	
	
}

//=============================================================================
void CCalendarPage::OnSundayClicked(void)
{
	CJobDefDlg defdlg;
	strday="";
	strday = "Sunday";
	int day = 0;
	if( IDOK == defdlg.DoModal())
	{
		
		SetColumnJobs(day,defdlg.m_ProfileName);
		CString displaystr;
		displaystr = defdlg.m_ProfileName;
		New_profile = defdlg.m_ProfileName;
		m_MessageDisplay.SetWindowText(displaystr + " defined as the job profile for all Sundays this month");
	}
	UpdateData(TRUE);
	wk_day = m_strBtn15;
	day = atoi(wk_day);
	PopulateHistoryTable(m_ThisMonthsDays->GetYear(),m_ThisMonthsDays->GetMonth(),day);

}


//=============================================================================
void CCalendarPage::OnMondayClicked(void)
{
	
	CJobDefDlg defdlg;
	strday="";
	strday = "Monday";
	int day = 1;
	if( IDOK == defdlg.DoModal())
	{
		SetColumnJobs(day,defdlg.m_ProfileName);
		CString displaystr = defdlg.m_ProfileName;
		New_profile = defdlg.m_ProfileName;
	  	m_MessageDisplay.SetWindowText(displaystr +" defined as the job profile for all Mondays this month");
	}
	UpdateData(TRUE);
	wk_day = m_strBtn16;
	day = atoi(wk_day);
	PopulateHistoryTable(m_ThisMonthsDays->GetYear(),m_ThisMonthsDays->GetMonth(),day);

}

//=============================================================================
void CCalendarPage::OnTuesdayClicked(void)
{
	CJobDefDlg defdlg;
	strday="";
	strday = "Tuesday";
	int day = 2;
	if( IDOK == defdlg.DoModal())
	{
		SetColumnJobs(day,defdlg.m_ProfileName);
		CString displaystr = defdlg.m_ProfileName;
		New_profile = defdlg.m_ProfileName;
	  	m_MessageDisplay.SetWindowText(displaystr + " defined as the job profile for all Tuesdays this month");
	}
	UpdateData(TRUE);
	wk_day = m_strBtn17;
	day = atoi(wk_day);
	PopulateHistoryTable(m_ThisMonthsDays->GetYear(),m_ThisMonthsDays->GetMonth(),day);
}

//=============================================================================
void CCalendarPage::OnWednesdayClicked(void)
{
	CJobDefDlg defdlg;
	strday="";
	strday = "Wednesday";
	int day = 3;
	if( IDOK == defdlg.DoModal())
	{
		SetColumnJobs(day,defdlg.m_ProfileName);
		CString displaystr = defdlg.m_ProfileName;
		New_profile = defdlg.m_ProfileName;
	  	m_MessageDisplay.SetWindowText(displaystr + " defined as the job profile for all Wednesdays this month");
	}
	UpdateData(TRUE);
	wk_day = m_strBtn18;
	day = atoi(wk_day);
	PopulateHistoryTable(m_ThisMonthsDays->GetYear(),m_ThisMonthsDays->GetMonth(),day);
}

//=============================================================================
void CCalendarPage::OnThursdayClicked(void)
{
	CJobDefDlg defdlg;
	strday="";
	strday = "Thuday";
	int day = 4;
	if( IDOK == defdlg.DoModal())
	{
		SetColumnJobs(day,defdlg.m_ProfileName);
		CString displaystr = defdlg.m_ProfileName;
		New_profile = defdlg.m_ProfileName;
	  	m_MessageDisplay.SetWindowText(displaystr + " defined as the job profile for all Thursdays this month");
	}
	UpdateData(TRUE);
	wk_day = m_strBtn19;
	day = atoi(wk_day);
	PopulateHistoryTable(m_ThisMonthsDays->GetYear(),m_ThisMonthsDays->GetMonth(),day);
}

//=============================================================================
void CCalendarPage::OnFridayClicked(void)
{
	CJobDefDlg defdlg;
	strday="";
	strday = "Friday";
	int day = 5;
	if( IDOK == defdlg.DoModal())
	{
		SetColumnJobs(day,defdlg.m_ProfileName);
		CString displaystr = defdlg.m_ProfileName;
		New_profile = defdlg.m_ProfileName;
	  	m_MessageDisplay.SetWindowText( displaystr + " defined as the job profile for all Fridays this month");
	}
	UpdateData(TRUE);
	wk_day = m_strBtn20;
	day = atoi(wk_day);
	PopulateHistoryTable(m_ThisMonthsDays->GetYear(),m_ThisMonthsDays->GetMonth(),0);
}

//=============================================================================
void CCalendarPage::OnSaturdayClicked(void)
{
	CJobDefDlg defdlg;
	strday="";
	strday = "Saturday";
	int day = 6;
	if( IDOK == defdlg.DoModal())
	{	
		SetColumnJobs(day,defdlg.m_ProfileName);
		CString displaystr = defdlg.m_ProfileName;
		New_profile = defdlg.m_ProfileName;
	  	m_MessageDisplay.SetWindowText( displaystr + " defined as the job profile for all Saturdays this month");
	}
	UpdateData(TRUE);
	wk_day = m_strBtn21;
	day = atoi(wk_day);
	PopulateHistoryTable(m_ThisMonthsDays->GetYear(),m_ThisMonthsDays->GetMonth(),m_ThisMonthsDays->GetDay());
}

//==========================================================================================

void CCalendarPage::OnCalendarButtonRClicked(UINT)
{
	
	CString buttonlabel;
	CCalendarButton* activebutton;
	
	// get the active button and read its label, if 
	// the label is not blank calculate the index into 
	// the day table and check to see if a job is defined 
	// for the day. If it is put it in the job def dlg, if not just
	// display the job def dialog with a blank profile name. 
	// if the user presses ok, save the profile name 
	// in the day structure. 

	activebutton = (CCalendarButton*)GetFocus();
	activebutton->GetWindowText(buttonlabel);
	if ( " " != buttonlabel)
	{
		int dayindex = atoi(buttonlabel) -1;
		CJobDefDlg jdefdlg;

		if( m_ThisMonthsDays[dayindex].HasProfile())
			jdefdlg.m_ProfileName = m_ThisMonthsDays[dayindex].GetJobProfile();

		if( IDOK == jdefdlg.DoModal())
		{		
			m_ThisMonthsDays[dayindex].SetJobProfile(jdefdlg.m_ProfileName);			
			displaystr = jdefdlg.m_ProfileName;
			
			if (!CheckDialogForCompleteness())
            return;
			displaystr += " defined as the job profile for ";
			CString monthstr;
			m_Month.GetWindowText(monthstr);
			displaystr = displaystr + monthstr +" " + buttonlabel;
			m_MessageDisplay.SetWindowText(displaystr);
		}
	}

}

//==========================================================================================
void CCalendarPage::SetColumnButtons( int day)
{
	CString numstring;

	// for all the buttons that have a number on them
	// set them as checked, and update the day table to indicate they are working days
	
	for ( int row = 0; row <= 5 ; row++)
	{
		m_DateButtons[(row*7) + day]->GetWindowText(numstring);
		if( " " != numstring)
		{
		 	m_DateButtons[(row*7)+ day]->SetCheck(1);
			int dayindex = atoi(numstring) -1;
			m_ThisMonthsDays[dayindex].MakeWorkingDay();
		}
      
			
	}
}

//============================================================================================
void CCalendarPage::SetDefaultWorkingDays(void)
{
	SetColumnButtons(1);
	SetColumnButtons(2);
	SetColumnButtons(3);
	SetColumnButtons(4);
	SetColumnButtons(5);

	m_NewData = TRUE;
}
//===============================================================================================
void CCalendarPage::InitializeThisMonthsDays(void)
{
	for ( int count = 1; count <= m_DayCount; count++)
	{
		m_ThisMonthsDays[count - 1].SetDay(count);
		m_ThisMonthsDays[count - 1].SetMonth(m_ThisMonth);
		m_ThisMonthsDays[count - 1].SetYear( m_ThisYear);
	}
}

//==========================================================================================
void CCalendarPage::SetColumnJobs( int day, CString job)
{
	CString numstring;

	// for each button in the column see if it is a valid day
	// if it is a valid day see if it is a working day
	// if it is a working day set the job
	
	for ( int row = 0; row <= 5 ; row++)
	{
		m_DateButtons[(row*7) + day]->GetWindowText(numstring);
		if( " " != numstring)
		{
		//	if( 1 == m_DateButtons[(row*7)+ day]->GetCheck())
		//	{
				int dayindex = atoi(numstring) -1;
				m_ThisMonthsDays[dayindex].SetJobProfile(job);
		//	}
		}
	}
}

//==========================================================================================

void CCalendarPage::OnCalendarButtonLClicked(UINT)
{
	
	CString buttonlabel;
	CString displaystr;
	CCalendarButton* activebutton;
	
	// get the active button and read its label, if 
	// the label is not blank calculate the index into 
	// the day table get the buttons state and update the day table

	activebutton = (CCalendarButton*)GetFocus();
	activebutton->GetWindowText(buttonlabel);

   if ( " " != buttonlabel)
	{
		int dayindex = atoi(buttonlabel) -1;
		if ( 1 == activebutton->GetCheck())
		{
			m_ThisMonthsDays[dayindex].MakeWorkingDay();
			buttonlabel += " set to working day";
			day_chk = 1;
		}
		else
		{
			m_ThisMonthsDays[dayindex].MakeHoliday();
			buttonlabel += " set to Holiday";
			day_chk = 0;
		}
   }   


	m_Month.GetWindowText(displaystr);
	m_MessageDisplay.SetWindowText(displaystr + " " +buttonlabel);

	PopulateHistoryTable(m_ThisMonthsDays->GetYear(),m_ThisMonthsDays->GetMonth(),atoi(buttonlabel));
 
}

//============================================================================================
// this function retrieves date data from the data server

BOOL CCalendarPage::GetDateData(int month, int year)
{
	//return FALSE;

	CLF01 ThisMonthsData;
	char errormessage  [256];

	// set up the key for the data search, leading zeroes must be present

	itoa(year,(char*)ThisMonthsData.primary_key.year,10);
	if ( month < 10)
	{
		ThisMonthsData.primary_key.month[0] = 0x30;
		itoa(month,(char*)&ThisMonthsData.primary_key.month[1],10);
	}

	else
		itoa(month,(char*)ThisMonthsData.primary_key.month,10);


	// send the data request
	
	unsigned char  result = txdsapi_get_record( (pBYTE)&ThisMonthsData, sizeof (CLF01), CLF01_DATA, errormessage);
	
	//unsigned char result;
	if ( result == PTEMSG_OK)
	{
		
		// cycle through the imported data array, 
		// transfer data into the day array

		// *****ASSUMPTION****** data is sorted in ascending order and no days are skipped
		
		for( int count = 0; count < m_DayCount; count++)
		{
			if( 'Y' == ThisMonthsData.day[count].working_day[0])
				m_ThisMonthsDays[count].MakeWorkingDay();
			else
			    m_ThisMonthsDays[count].MakeHoliday();

			if( NULL != ThisMonthsData.day[count].profile_id[0])
				m_ThisMonthsDays[count].SetJobProfile(ThisMonthsData.day[count].profile_id);
		}

		return TRUE;

	}
	
	//m_MessageDisplay.SetWindowText(errormessage);
	return FALSE;


}

//============================================================================================
void CCalendarPage::DisplayDateData(void)
{
	CString numstring;	
	
	
	// iterate through the button structure, check each day to see if it is a working
	// day, if it is, set the button to checked
	
	for ( int count = 0; count < 42; count++)
	{
	
		m_DateButtons[count]->GetWindowText(numstring);
		if( " " != numstring)
		{
			int dayindex = atoi(numstring) -1;
			if (m_ThisMonthsDays[dayindex].IsWorkingDay())
				m_DateButtons[count]->SetCheck(BST_CHECKED);
						
		}
	}


}
//============================================================================================
void CCalendarPage::OnSaveDateData()
{
	
	// create the save data array and NULL fill it
	
	CLF01 savedata;
	memset((void*)&savedata,0x00,sizeof(savedata));
	
	// set the primary key
	
	itoa(m_ThisYear,(char*)savedata.primary_key.year,10);
	
	
	// if the month value is less than 10 insert a leading zero
	
	if ( m_ThisMonth < 10)
	{
		savedata.primary_key.month[0] = 0x30;
		itoa(m_ThisMonth,(char*)&savedata.primary_key.month[1],10);
	}

	else
		itoa(m_ThisMonth,(char*)savedata.primary_key.month,10);



	// transfer data from the day array into the save data array
	// convert all day data to unsigned char strings. 

	for( int count= 0; count < m_DayCount; count++)
	{
		if (m_ThisMonthsDays[count].IsWorkingDay())
			savedata.day[count].working_day[0] = 'Y';
		else 
			savedata.day[count].working_day[0] ='N';
		
		CString profilestr = m_ThisMonthsDays[count].GetJobProfile();
		strcpy((char*)&savedata.day[count].profile_id,(LPCTSTR) profilestr);

	}

	// if this is new data send it to the data server to be added
	// if it is updated data send it to the data serverto be updated
	// if the save is successful tell the user the data was saved, if not
	// display the errormessage
	
	char errormessage[256];
	BYTE result;
	
	if ( TRUE == m_NewData)
		result = txdsapi_insert_record((pBYTE)&savedata,sizeof (CLF01), CLF01_DATA, errormessage);
	
	else 
		result = txdsapi_update_record((pBYTE)&savedata,sizeof (CLF01), CLF01_DATA, errormessage);

	if ( PTEMSG_OK == result)
		 m_MessageDisplay.SetWindowText("Date Data Saved");
	
	else
		 m_MessageDisplay.SetWindowText(errormessage);

}

//================================================================================================

BOOL CCalendarPage::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	delete[] m_ThisMonthsDays;
	
	return CPropertyPage::DestroyWindow();
}

BOOL CCalendarPage::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;
	CJobDefDlg jdefdlg;

    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
   
    if (myUtilsObject.IsEmpty (displaystr, this))
    {
		/*m_MessageDisplay.SetWindowText("ERROR: Job Profile name can not be blank!");		
		if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}*/
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}
    return TRUE;
}


void CCalendarPage::OnOK() 
{

	CPropertyPage::OnOK();
}

BOOL CCalendarPage::OnApply() 
{	
	
	OnSaveDateData();
	
	return CPropertyPage::OnApply();
}


BOOL CCalendarPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	SetModified(TRUE);
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BYTE CCalendarPage::PopulateHistoryTable(int year,int month,int day)
{
	// get ucf01,
	// get month detalis of the chnged month 
	// if details not present privious data null and current data will be filled.
	UCF01 ucf01;
	CLF01 clf01;
	BYTE rcode=-1;
	char error_msg[100]={0};
	HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
    CString temp;
    CHAR wstation[21]; DWORD wsize;
	char buf[40]={0};

    char strDate[9] = "";
    char strTime[7] = "";
    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    
    myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"UCF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );
	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR)sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));   
	memcpy ((LPTSTR)sHistoryData.changed_by,login_name, sizeof(sHistoryData.changed_by));
	memcpy ((LPTSTR)sHistoryData.file_primary_key,"Calender", sizeof (sHistoryData.file_primary_key));
	if(!strday.IsEmpty())
	{
		memset(buf,0x00,sizeof(buf));
		sprintf(buf,"WEEK_%d_%d %s ProfileID",year,month,strday);// YYYYMMDD
		memcpy ((LPTSTR)sHistoryData.field_tag,buf, sizeof(sHistoryData.field_tag));
		memcpy ((LPTSTR)sHistoryData.current_data,New_profile, sizeof (sHistoryData.current_data));
	}
	else
	{
		memset(buf,0x00,sizeof(buf));
		sprintf(buf,"DAY_%d/%d/%d Working Day Info",year,month,day); // YYYYMMDD
		memcpy ((LPTSTR)sHistoryData.field_tag,buf, sizeof(sHistoryData.field_tag));
		if(day_chk==0)
		{
			memcpy ((LPTSTR)sHistoryData.current_data,"N", sizeof (sHistoryData.current_data));
		}
		else
		{
			memcpy ((LPTSTR)sHistoryData.current_data,"Y", sizeof (sHistoryData.current_data));
		}
	}
	
	strcpy( (char *)ucf01.primary_key.name, login_name );  
	rcode = txdsapi_get_record( (pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error_msg );
	if(rcode != PTEMSG_OK)
	{
		AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
		return 0;
	}
	itoa(year,(char*)clf01.primary_key.year,10);
	if ( month < 10)
	{
		clf01.primary_key.month[0] = 0x30;
		itoa(month,(char*)&clf01.primary_key.month[1],10);
	}
	else
	{
		itoa(month,(char*)clf01.primary_key.month,10);
	}
	rcode = txdsapi_get_record( (pBYTE)&clf01,sizeof (CLF01), CLF01_DATA, error_msg );
	if(rcode == PTEMSG_NOT_FOUND )
	{
		memcpy ((LPTSTR)sHistoryData.previous_data,"", sizeof (sHistoryData.current_data));
	}
	else 
	{
		if(!strday.IsEmpty())
		{	
			memcpy ((LPTSTR)sHistoryData.previous_data,clf01.day[day-1].profile_id, sizeof (sHistoryData.previous_data));
		}
		else
		{
			memcpy ((LPTSTR)sHistoryData.previous_data,clf01.day[day-1].working_day, sizeof (sHistoryData.previous_data));
		}
   		
	}
	rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA,  strErrorMsg);
	if ( rcode != PTEMSG_OK )
	{
		AfxMessageBox( strErrorMsg, MB_ICONSTOP | MB_OK );
		return 0;
	}
	return 1;
}

