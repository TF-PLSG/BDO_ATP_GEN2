/*********************************************************************************
 Module: NegativeCardFileControl.cpp : implementation file

 Copyright (c) 1998, Hypercom, Inc.                         
 All Rights Reserved.                                       

 Description: Logic for Negative Card property page
			  This module defines and uses NegativeCardFileControl class

 Application: ATP

 Author: Irina Arustamova

 Revision History:
  
 $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NegativeCardFileControl.cpp  $
   
      Rev 1.9   Dec 01 2003 11:29:50   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   Jan 05 2000 14:54:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:55:50   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:56   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:02   iarustam
   Initial Release
   
      Rev 1.5   May 10 1999 11:28:16   dperson
   Standardized error messages
   
      Rev 1.4   Apr 29 1999 14:56:46   iarustam
   Bug fixes
   
      Rev 1.3   Mar 22 1999 11:12:44   iarustam
   Bug fixes
   
   
      Rev 1.2   Mar 16 1999 16:30:12   iarustam
   Bug fixes
   
      Rev 1.1   Mar 04 1999 13:32:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:52   iarustam
   Initial Release
   

    

************************************************************************************/
// NegativeCardFileControl.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NegativeCardFileControl.h"
#include "MyUtils.h"
#include "SharedMem.h"	
extern "C"{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CNegativeCardFileControl property page

IMPLEMENT_DYNCREATE(CNegativeCardFileControl, CPropertyPage)

CNegativeCardFileControl::CNegativeCardFileControl() : CPropertyPage(CNegativeCardFileControl::IDD)
{
	//{{AFX_DATA_INIT(CNegativeCardFileControl)
	m_StrEndFld = _T("");
	m_StrFileNameFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrStartFld = _T("");
	//}}AFX_DATA_INIT
}

CNegativeCardFileControl::~CNegativeCardFileControl()
{
}

void CNegativeCardFileControl::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNegativeCardFileControl)
	DDX_Control(pDX, IDC_FILE_NAME_FLD, m_CntlFileNameFld);
	DDX_Control(pDX, IDC_START_FLD, m_CntlStartFld);
	DDX_Control(pDX, IDC_END_FLD, m_CntlEndFld);
	DDX_Control(pDX, IDC_BUTTON, m_Button);
	DDX_Text(pDX, IDC_END_FLD, m_StrEndFld);
	DDV_MaxChars(pDX, m_StrEndFld, 8);
	DDX_Text(pDX, IDC_FILE_NAME_FLD, m_StrFileNameFld);
	DDV_MaxChars(pDX, m_StrFileNameFld, 50);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_START_FLD, m_StrStartFld);
	DDV_MaxChars(pDX, m_StrStartFld, 8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNegativeCardFileControl, CPropertyPage)
	//{{AFX_MSG_MAP(CNegativeCardFileControl)
	ON_BN_CLICKED(IDC_BUTTON, OnButton)
	ON_EN_UPDATE(IDC_FILE_NAME_FLD, OnUpdateFileNameFld)
	ON_EN_UPDATE(IDC_START_FLD, OnUpdateStartFld)
	ON_EN_UPDATE(IDC_END_FLD, OnUpdateEndFld)
	ON_EN_KILLFOCUS(IDC_START_FLD, OnKillfocusStartFld)
	ON_EN_KILLFOCUS(IDC_END_FLD, OnKillfocusEndFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNegativeCardFileControl message handlers

/******************************************************************************
 
  MODULE:	    OnInitDialog
 
  DESCRIPTION: This function performs special processing when 
               the dialog box is initialized.
  INPUTS:      ucf01 - user structure             
 
  OUTPUTS:     None
 
  RTRN VALUE:  return TRUE unless you set the focus to a control
 
  AUTHOR:      Irina Arustamova
 
  MODIFICATIONS:
 
 ******************************************************************************/
BOOL CNegativeCardFileControl::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	CString temp;
	CMyUtils myUtilsObject;
	char strErrorMessage[200] = "";


	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	// Clean all strings
   m_StrEndFld = _T("");
	m_StrFileNameFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrStartFld = _T("");
	m_bFirstRecord = FALSE;


	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;

	int nIndex = -1;

	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "System Operations" ) == 0)
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( '0' == ucf01.control[nIndex].action[0]  &&
         ( '0' == ucf01.control[nIndex].action[1]  ) &&
         ( '0' == ucf01.control[nIndex].action[2]  ) &&
         ( '0' == ucf01.control[nIndex].action[3]  ) )

      {
			m_CntlEndFld.EnableWindow(FALSE);
			m_CntlStartFld.EnableWindow(FALSE);
			m_CntlFileNameFld.EnableWindow(FALSE);
      }
	}

	// Populate screen from database
   ZeroMemory ( &sNegFileRecordOld, sizeof (NFI02));
	memcpy( sNegFileRecordOld.primary_key.file_name, " ", sizeof (sNegFileRecordOld.primary_key.file_name));

	int rcode = txdsapi_get_record ( (pBYTE)&sNegFileRecordOld, 
											 sizeof (NFI02), NFI02_DATA, strErrorMessage );
	if ( PTEMSG_OK != rcode && PTEMSG_NOT_FOUND != rcode)
	{  	
		m_StrMessageLbl = strErrorMessage;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}
	m_StrFileNameFld = sNegFileRecordOld.primary_key.file_name;
	m_StrStartFld = sNegFileRecordOld.start_date;
	m_StrEndFld = sNegFileRecordOld.end_date;

// Populate History Property Page
	ZeroMemory (&pChangeHistoryPage->sHistoryRecord, sizeof (HST01));

    memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.primary_key.unique_id),"                 ",sizeof (pChangeHistoryPage->sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.file_name),"NFI02",sizeof (pChangeHistoryPage->sHistoryRecord.file_name));
    memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.file_primary_key),sNegFileRecordOld.primary_key.file_name,sizeof (pChangeHistoryPage->sHistoryRecord.file_primary_key));

//	m_StrStartFld = m_StrStartFld.Right(2) + "/" + m_StrStartFld.Mid(4,2) + "/"  + m_StrStartFld.Left(4);
//	m_StrEndFld = m_StrEndFld.Right(2) + "/" + m_StrEndFld.Mid(4,2) + "/" + m_StrEndFld.Left(4);
	m_StrStartFld = m_StrStartFld.Right(2) +  m_StrStartFld.Mid(4,2) +  m_StrStartFld.Left(4);
	m_StrEndFld = m_StrEndFld.Right(2) +  m_StrEndFld.Mid(4,2) +  m_StrEndFld.Left(4);

	if (myUtilsObject.IsEmpty (m_StrFileNameFld, this))
	{
		m_bFirstRecord = TRUE;
	}


	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/******************************************************************************
 
  MODULE:	   OnButton
 
  DESCRIPTION: Calles file dialog for selecting file name.

  INPUTS:      None             
 
  OUTPUTS:     None
 
  RTRN VALUE:  None
 
  AUTHOR:      Irina Arustamova
 
  MODIFICATIONS:
 
 ******************************************************************************/

void CNegativeCardFileControl::OnButton() 
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, this);
	fileDlg.DoModal();

	m_StrFileNameFld = fileDlg.GetPathName();
	UpdateData(FALSE);
}


/******************************************************************************
 
  MODULE:	   CheckDialogForCompleteness
 
  DESCRIPTION: Check for all required fields.

  INPUTS:      None             
 
  OUTPUTS:     None
 
  RTRN VALUE:  TRUE if all fields are filled out
               FALSE if field is empty
 
  AUTHOR:      Irina Arustamova
 
  MODIFICATIONS:
 
 ******************************************************************************/

BOOL CNegativeCardFileControl::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	if (myUtilsObject.IsEmpty (m_StrFileNameFld, this))
    {
		m_StrMessageLbl = "Please enter File Name";	
		m_CntlFileNameFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrStartFld, this))
    {
		m_StrMessageLbl = "Please enter Start";	
		m_CntlStartFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}


	if (myUtilsObject.IsEmpty (m_StrEndFld, this))
    {
		m_StrMessageLbl = "Please enter End";	
		m_CntlEndFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	
    return TRUE;
}

/******************************************************************************
 
  MODULE:	   OnApply
 
  DESCRIPTION: Checkes dialog for dompleteness, formates data, deletes previous
                record from database and inserts new record..

  INPUTS:      None             
 
  OUTPUTS:     TRUE if data saved and history table populated
               FALSE if failed
 
  RTRN VALUE:  None
 
  AUTHOR:      Irina Arustamova
 
  MODIFICATIONS:
 
 ******************************************************************************/
BOOL CNegativeCardFileControl::OnApply() 
{
	NFI02 sNegFileRecord;
	char strErrorMessage[200] = "";
	CString temp;

	CString strStartFld, strEndFld;

	if (!CheckDialogForCompleteness())
		return FALSE;

	memset(&sNegFileRecord, 0, sizeof(NFI02)) ;

	if ( !ValidateDate ( m_StrStartFld ) )
	{
		m_StrMessageLbl = "ERROR: Invalid Start Date";
		m_CntlStartFld.SetFocus();
		UpdateData(FALSE);
		return FALSE;

	}

	if ( !ValidateDate ( m_StrEndFld ) )
	{
		m_StrMessageLbl = "ERROR: Invalid End Date";
		m_CntlEndFld.SetFocus();
		UpdateData(FALSE);
		return FALSE;

	}
   // deletes / from data fields
	//strStartFld = m_StrStartFld.Right(4) + m_StrStartFld.Mid(3,2) + m_StrStartFld.Left(2);
	//strEndFld = m_StrEndFld.Right(4) + m_StrEndFld.Mid(3,2) + m_StrEndFld.Left(2);

	if ( m_StrStartFld > m_StrEndFld )
	{
		m_StrMessageLbl = "ERROR: Start Day cannot be  greater than End Day";
		UpdateData(FALSE);
		return FALSE;
	}

// Deletes record from database
	memcpy (sNegFileRecord.primary_key.file_name, sNegFileRecordOld.primary_key.file_name, sizeof (sNegFileRecord.primary_key.file_name));
		
	if ( !m_bFirstRecord )
	{
	int rcode = txdsapi_delete_record ( (pBYTE)&sNegFileRecord, 
											 sizeof (NFI02), NFI02_DATA, strErrorMessage );
		if (PTEMSG_OK != rcode )
		{  	
			m_StrMessageLbl = strErrorMessage;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
	}
// Inserts new record
	memcpy (sNegFileRecord.primary_key.file_name, m_StrFileNameFld, sizeof (sNegFileRecord.primary_key.file_name));
	memcpy (sNegFileRecord.start_date, strStartFld, sizeof (sNegFileRecord.start_date));
	memcpy (sNegFileRecord.end_date, strEndFld, sizeof (sNegFileRecord.end_date));

	int rcode = txdsapi_insert_record ( (pBYTE)&sNegFileRecord, 
											 sizeof (NFI02), NFI02_DATA, strErrorMessage );
	if ( PTEMSG_OK != rcode )
	{  	
		m_StrMessageLbl = strErrorMessage;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}
	else 
	{    
        if ( m_bFirstRecord )
			m_StrMessageLbl = "Record Added";
		else
			m_StrMessageLbl = "Record Updated";	
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	}
// Compares strings for populating Change History table 
	if ( sNegFileRecordOld.primary_key.file_name != m_StrFileNameFld)
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		memcpy (m_strCurrentData, m_StrFileNameFld, sizeof (m_strCurrentData));
		memcpy (m_strPreviousData, sNegFileRecordOld.primary_key.file_name, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "File Name", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sNegFileRecordOld.start_date != strStartFld)
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		temp = sNegFileRecordOld.start_date;
		temp = temp.Mid(6,2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);

		memcpy (m_strCurrentData, m_StrStartFld, sizeof (m_strCurrentData));
		memcpy (m_strPreviousData, temp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Start Date", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( sNegFileRecordOld.end_date != strEndFld)
	{
		memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));

		temp = sNegFileRecordOld.end_date;
		temp = temp.Mid(6,2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);

		memcpy (m_strCurrentData, m_StrEndFld, sizeof (m_strCurrentData));
		memcpy (m_strPreviousData, temp, sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "End Date", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
// Updates change history page
	if ( pChangeHistoryPage->m_bChangeHistory )
	{
		memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.primary_key.unique_id)," ",sizeof (pChangeHistoryPage->sHistoryRecord.primary_key.unique_id));
		memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.file_name),"NFI02",sizeof (pChangeHistoryPage->sHistoryRecord.file_name));
		memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.file_primary_key),sNegFileRecordOld.primary_key.file_name,sizeof (pChangeHistoryPage->sHistoryRecord.file_primary_key));

		pChangeHistoryPage->PopulateHistoryDialog();
	}
	memcpy (sNegFileRecordOld.primary_key.file_name, m_StrFileNameFld, sizeof (sNegFileRecordOld.primary_key.file_name));
	memcpy (sNegFileRecordOld.start_date, strStartFld, sizeof (sNegFileRecordOld.start_date));
	memcpy (sNegFileRecordOld.end_date, strEndFld, sizeof (sNegFileRecordOld.end_date));

	return CPropertyPage::OnApply();
	
}


void CNegativeCardFileControl::OnUpdateFileNameFld() 
{
	SetModified (TRUE);	
}

void CNegativeCardFileControl::OnUpdateStartFld() 
{
	SetModified (TRUE);	
	
}

void CNegativeCardFileControl::OnUpdateEndFld() 
{
	SetModified (TRUE);	
	
}

/******************************************************************************
 
  MODULE:	   OnKillfocusStartFld
 
  DESCRIPTION: Checkes start date for valid date.

  INPUTS:      None             
 
  OUTPUTS:     None
 
  RTRN VALUE:  None
 
  AUTHOR:      Irina Arustamova
 
  MODIFICATIONS:
 
 ******************************************************************************/
void CNegativeCardFileControl::OnKillfocusStartFld() 
{
/*	UpdateData (TRUE);

	CMyUtils myUtilsObject;

	m_StrMessageLbl = "";
	UpdateData (FALSE);


// Return if field is empry
   if ( myUtilsObject.IsEmpty ( m_StrStartFld, this ))
		return;


	int length;

	length = m_StrStartFld.GetLength();

	if (length < 8 && length != 0)
	{
		m_CntlStartFld.SetFocus();
		m_StrMessageLbl = "ERROR: Invalid date";
		UpdateData(FALSE);
		m_bValidDay = FALSE;
		return;

	}
// Serches for invalid characters
	int res = strspn ( m_StrStartFld, "1234567890/");

	if ( res < length )
	{
		m_CntlStartFld.SetFocus();
		m_StrMessageLbl = "ERROR: Invalid character";
		m_bValidDay = FALSE;
		UpdateData(FALSE);
		return;
	}
// Validates date
	if (!ValidateDate ( m_StrStartFld ))
	{
		m_CntlStartFld.SetFocus();
		m_StrMessageLbl = "ERROR: Invalid date format";
		m_bValidDay = FALSE;
		UpdateData(FALSE);
		return;
	}
	
// Format date
   if ( 8 == length )
	{
		m_StrStartFld = m_StrStartFld.Left(2) + "/" + 
								m_StrStartFld.Mid(2,2) + "/" +
								m_StrStartFld.Right(4);
		UpdateData(FALSE);
	}

	if ( 9 == length )
	{
		int pos = m_StrStartFld.Find ("/");

		if ( 2 == pos )	
		{
			m_StrStartFld = m_StrStartFld.Left(5) + "/" + 								
									m_StrStartFld.Right(4);
			UpdateData(FALSE);
		}

		if ( 4 == pos )	
		{
			m_StrStartFld = m_StrStartFld.Left(2) + "/" + 								
									m_StrStartFld.Right(7);
			UpdateData(FALSE);
		}
	}

	nStartDay = nDay;
	nStartMonth = nMonth;
	nStartYear = nYear;
	*/
}

/******************************************************************************
 
  MODULE:	   OnKillfocusEndFld
 
  DESCRIPTION: Validates End Date field.

  INPUTS:      None             
 
  OUTPUTS:     None
 
  RTRN VALUE:  None
 
  AUTHOR:      Irina Arustamova
 
  MODIFICATIONS:
 
 ******************************************************************************/
void CNegativeCardFileControl::OnKillfocusEndFld() 
{
	/*CString strEndMonth, strStartMonth;
	CString strEndDay, strStartDay;
	CString strEndYear, strStartYear;

	CMyUtils myUtilsObject;

	int nEndMonth;
	int nEndDay;
	int nEndYear;

	UpdateData (TRUE);
// Return if field is empty
	if ( myUtilsObject.IsEmpty ( m_StrEndFld, this ))
	   return;

	m_StrMessageLbl = "";
	UpdateData (FALSE);

	int length;
// Checkes length
	length = m_StrEndFld.GetLength();

	if (length < 8 && 0 != length)
	{
	   m_CntlEndFld.SetFocus();
	   m_StrMessageLbl = "ERROR: Invalid date";
		m_bValidDay = FALSE;
		UpdateData(FALSE);
		return;
	}
// Checkes for valid characters
	int res = strspn ( m_StrEndFld, "1234567890/");

	if ( res < length )
	{
		m_CntlEndFld.SetFocus();
		m_StrMessageLbl = "ERROR: Invalid character";
		m_bValidDay = FALSE;
		UpdateData(FALSE);
		return;
	}

	if (!ValidateDate ( m_StrEndFld ))
	{
	//	m_CntlEndFld.SetFocus();
		m_bValidDay = FALSE;
		UpdateData(FALSE);
		return;
	}
	
// Formates date
   if ( 8 == length )
	{
		m_StrEndFld = m_StrEndFld.Left(2) + "/" + 
								m_StrEndFld.Mid(2,2) + "/" +
								m_StrEndFld.Right(4);
		UpdateData(FALSE);
	}

	if ( 9 == length )
	{
		int pos = m_StrEndFld.Find ("/");

		if ( 2 == pos )	
		{
			m_StrEndFld = m_StrEndFld.Left(5) + "/" + 								
									m_StrEndFld.Right(4);
			UpdateData(FALSE);
		}

		if ( 4 == pos )	
		{
			m_StrEndFld = m_StrEndFld.Left(2) + "/" + 								
									m_StrEndFld.Right(7);
			UpdateData(FALSE);
		}
	}

	strEndMonth = m_StrEndFld.Mid(3,2);
	strEndDay = m_StrEndFld.Left(2);
	strEndYear = m_StrEndFld.Right(4);

	strStartMonth = m_StrStartFld.Mid(3,2);
	strStartDay = m_StrStartFld.Left(2);
	strStartYear = m_StrStartFld.Right(4);

	nEndMonth = atoi ( strEndMonth );
	nEndDay = atoi ( strEndDay );
	nEndYear = atoi ( strEndYear );

	nStartMonth = atoi ( strStartMonth );
	nStartDay = atoi ( strStartDay );
	nStartYear = atoi ( strStartYear );

// Checkes for end date being less than start date
   if ( nEndYear < nStartYear )
	{
		m_StrMessageLbl = "ERROR: Start Day cannot be greater than End Day";
		UpdateData(FALSE);
		m_bValidDay = FALSE;
		return;
//		m_CntlEndFld.SetFocus();
	}

	if ( nEndYear == nStartYear && nEndMonth < nStartMonth )
	{
		m_StrMessageLbl = "ERROR: Start Day cannot be  greater than End Day";
		UpdateData(FALSE);
		m_bValidDay = FALSE;
		return;
//		m_CntlEndFld.SetFocus();
	}

	if ( nEndYear == nStartYear && nEndMonth == nStartMonth  && nEndDay < nStartDay)
	{
		m_StrMessageLbl = "ERROR: Start Day cannot be  greater than End Day";
		UpdateData(FALSE);
		m_bValidDay = FALSE;
		return;
//		m_CntlEndFld.SetFocus();
	}

	*/
}

/******************************************************************************
 
  MODULE:	   ValidateDate
 
  DESCRIPTION: Validates format, number of days, 
                valid day, month, year, 
                leap year.

  INPUTS:      None             
 
  OUTPUTS:     None
 
  RTRN VALUE:  None
 
  AUTHOR:      Irina Arustamova
 
  MODIFICATIONS:
 
 ******************************************************************************/
 BOOL CNegativeCardFileControl::ValidateDate ( CString strDate )
{
	CString strDay;
	CString strMonth;
	CString strYear;
	CMyUtils myUtilsObject;

	int nDay, nMonth, nYear;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return TRUE;

	int length = strDate.GetLength();	
	int res = strspn ( strDate,"1234567890" );

	if ( length < 8 )
	{
		m_StrMessageLbl = "ERROR: Invalid date format";
		UpdateData(FALSE);
		return FALSE;
	}
	
	strDay = strDate.Left(2);
	strMonth = strDate.Mid(2,2);
	strYear = strDate.Right(4);

	nDay = atoi ( strDay );
	nMonth = atoi ( strMonth );
	nYear = atoi ( strYear );

	if ( nDay == 0 || nMonth == 0 || nYear ==0)
	{
		m_StrMessageLbl = "ERROR: Invalid data";
		UpdateData(FALSE);
		return FALSE;
	}


	if ( nMonth > 12 )
	{
		m_StrMessageLbl = "ERROR: Invalid month";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nMonth == 1 || nMonth == 3 || nMonth == 5 || 
		 nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12 )
	{
		if ( nDay > 31 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth == 4 || nMonth == 6 || nMonth == 9 || 
		 nMonth == 11 )
	{
		if ( nDay > 30 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth ==2 )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
		if (res == 0 && nDay >29)
		{
			m_StrMessageLbl = "ERROR: Invalid Day";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	return TRUE;
}
/*BOOL CNegativeCardFileControl::ValidateDate ( CString strDate )
{
	CString strDay;
	CString strMonth;
	CString strYear;



	UpdateData(TRUE);

	int length = strDate.GetLength();	
	int res = strspn ( strDate,"1234567890" );

	if (2 == res && length < 10 )
	{
	//	m_StrMessageLbl = "ERROR: Invalid date format";
		UpdateData(FALSE);
		return FALSE;
	}
// Validate format
	if ( 10 == length  )
	{
		int pos = strDate.Find ("/");

		if ( 2 != pos )		
		{
			m_StrMessageLbl = "ERROR: Invalid date format";
			UpdateData(FALSE);
			return FALSE;
		}
	

		CString temp = strDate.Right(7);
		pos = temp.Find ("/");

		if ( 2 != pos )		
		{
			m_StrMessageLbl = "ERROR: Invalid date format";
			UpdateData(FALSE);
			return FALSE;
		}
		

		strDay = strDate.Left(2);
		strMonth = strDate.Mid(3,2);
		strYear = strDate.Right(4);
	}

	if ( 9 == length )
	{
		int pos = strDate.Find ("/");

		if ( pos != 2 && pos != 4)
		{
			m_StrMessageLbl = "ERROR: Invalid date format";
			UpdateData(FALSE);
			return FALSE;
		}
		if ( 2 == pos )
		{
			strDay = strDate.Left(2);
			strMonth = strDate.Mid(3,2);
			strYear = strDate.Right(4);
		}

		if ( 4 == pos )
		{
			strDay = strDate.Left(2);
			strMonth = strDate.Mid(2,2);
			strYear = strDate.Right(4);
		}
	}

	if ( 8 == length)
	{
	
		strDay = strDate.Left(2);
		strMonth = strDate.Mid(2,2);
		strYear = strDate.Right(4);
		
	}
// Validates Month
	nDay = atoi ( strDay );
	nMonth = atoi ( strMonth );
	nYear = atoi ( strYear );

	if ( nMonth > 12 )
	{
		m_StrMessageLbl = "ERROR: Invalid month";
		UpdateData(FALSE);
		return FALSE;
	}
// Validates Day
	if ( 1 == nMonth  || 3 == nMonth || 5 == nMonth || 
		 7 == nMonth || 8 == nMonth || 10 == nMonth || 12 == nMonth )
	{
		if ( nDay > 31 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( 4 == nMonth || 6 == nMonth || 9 == nMonth || 
		 11 == nMonth )
	{
		if ( nDay > 30 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( 2 == nMonth )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
		if ( 0 == res && nDay > 29)
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	m_bValidDay = TRUE;
	return TRUE;
}*/

/******************************************************************************
 
  MODULE:	   PopulateHistoryTable
 
  DESCRIPTION: Populates history table.

  INPUTS:      None             
 
  OUTPUTS:     None
 
  RTRN VALUE:  None
 
  AUTHOR:      Irina Arustamova
 
  MODIFICATIONS:
 
 ******************************************************************************/
void CNegativeCardFileControl::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	 CMyUtils myUtilsObject;
	 char strErrorMsg[200] = "";
    CString temp;

    char strDate[9] = "";
    char strTime[7] = "";

	 char strPrimaryKey[50];

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	 memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	 strcpy ( strPrimaryKey, m_StrFileNameFld);
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"NFI02", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	 memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
	 memcpy ((LPTSTR)sHistoryData.previous_data,			(LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	 memcpy ((LPTSTR)sHistoryData.field_tag,				(LPCTSTR)m_strFieldTag,		sizeof (sHistoryData.field_tag));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	 if ( PTEMSG_OK != rcode )
	 {
    		m_StrMessageLbl = strErrorMsg;
    		m_nDialogMode = MODE_DONE;
    		UpdateData( FALSE );
	 }
    
}


BOOL CNegativeCardFileControl::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
