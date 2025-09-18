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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\CardDefinition.cpp  $
   
      Rev 1.15   Apr 08 2005 08:47:12   lmitchel
   Add delete function to Purchase Card MCC ranges and correcting correct errors from testing
   
      Rev 1.14   Dec 01 2003 11:28:48   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.13   Jul 10 2002 09:55:46   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.12   May 21 2002 13:31:46   dirby
   Modified to not exit when <enter> is pressed.  If OK is selected,
   the database gets updated still, but screen is not exited.
   SCR 587
   
      Rev 1.11   Jan 21 2000 12:22:42   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.10   Jan 14 2000 16:25:00   mbysani
   bug fixes
   
      Rev 1.4   Oct 15 1999 17:05:20   iarustam
   bug fixes
   
      Rev 1.3   Sep 30 1999 16:16:54   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:44:10   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:58   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:06   iarustam
   Initial Release
    
*
************************************************************************************/

// CardDefinition.cpp : implementation file
//
#include "stdafx.h"
#include "Atp.h"
#include "CardDefinition.h"
#include "CardTypeDescription.h"
#include "ChangeHistory.h"
#include "MyUtils.h"
#include "CardTypePropertySheet.h"
#include "NameList.h"
#include "Sharedmem.h"
extern "C"
{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString name;
extern UCF01 ucf01;
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CCardDefinition property page

IMPLEMENT_DYNCREATE(CCardDefinition, CPropertyPage)

CCardDefinition::CCardDefinition() : CPropertyPage(CCardDefinition::IDD)
{
	//{{AFX_DATA_INIT(CCardDefinition)
	m_StrCardTypeFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CCardDefinition::~CCardDefinition()
{
}

void CCardDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardDefinition)
	DDX_Control(pDX, IDC_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_CARD_TYPE_FLD, m_CntlCardTypeFld);
	DDX_Text(pDX, IDC_CARD_TYPE_FLD, m_StrCardTypeFld);
	DDV_MaxChars(pDX, m_StrCardTypeFld, 4);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CCardDefinition)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	ON_BN_CLICKED(IDC_VIEW_BUTTON, OnViewButton)
	ON_BN_CLICKED(IDC_LIST_BUTTON, OnListButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardDefinition message handlers
/******************************************************************************
 *
 * MODULE:	    OnInitDialog
 *
 * DESCRIPTION: This function performs special processing when 
 *              the dialog box is initialized.
 *
 * AUTHOR:      Narina Pogosyants
 *
 * INPUTS:      None
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  Bool(true--exited successfully,false--not exited)
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/

BOOL CCardDefinition::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();
	CString temp;
	CFont m_font;
	int nIndex = -1;
	LOGFONT lf;
	memset ( &lf, 0, sizeof(lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_StrCardTypeFld = "";
	m_StrMessageLbl = "";	
   m_nDialogMode = MODE_DONE;

	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "BIN Control" ) == 0 )
		    nIndex = i;
	}

	if ( nIndex != -1 )
	{
		if ( ucf01.control[nIndex].action[0] == '0' )
			m_AddButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[1] == '0' )
			m_DeleteButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[2] == '0' )
			m_UpdateButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[3] == '0' )
			m_ViewButton.EnableWindow(FALSE);
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/******************************************************************************
 *
 * MODULE:	    OnApply
 *
 * DESCRIPTION: This function indicates that user pressed Apply button.
 *
 * AUTHOR:      Narina Pogosyants
 *
 * INPUTS:      None
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
BOOL CCardDefinition::OnApply() 
{
	CTF01 sCardTypeDatabaseStructure;
	char strErrorMessage[200];

//   CString buttonlabel;
//   CWnd*   activebutton;

   /* This snippet of code prevents updating the database
    * and exiting the window when <enter> is pressed. The
    * database will only get updated when APPLY is selected
    * or if focus is on the OK button when <enter> is pressed
    * or OK is selected.
    */
/*   activebutton = GetFocus();
   activebutton->GetWindowText(buttonlabel);
   if ( buttonlabel != "&Apply"  && buttonlabel != "OK")
   {
      return FALSE;
   }
*/
	//if we are in update mode
    if (m_nDialogMode == MODE_EDIT)
	{
		//clean Card Type Database Structure
        memset(&sCardTypeDatabaseStructure, 0, sizeof(sCardTypeDatabaseStructure));
        //make description page active if its tab is clicked
		if (!pDescriptionPage->m_bDescription)
			((CPropertySheet*) this->GetParent())->SetActivePage(1);
		//call function to populate database from dialog
		PopulateCardTypeDatabaseStructureFromDialog(&sCardTypeDatabaseStructure);
        
		//this function updates CTF01
		int rcode = txdsapi_update_record ((pBYTE)&sCardTypeDatabaseStructure,
			                sizeof(CTF01), CTF01_DATA,strErrorMessage);
		
		if (rcode != PTEMSG_OK)
		{
			//give an error message if updating is failed
			m_StrMessageLbl = strErrorMessage;

			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
		else
		{
			//give a message that record is updated
			m_StrMessageLbl = "Record Updated";
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			//clean description dialog
			pDescriptionPage->CleanCardDescriptionDialog();
			//disable description dialog
			pDescriptionPage->EnableCardDescriptionDialog(FALSE);			
			
			//clean change history dialog if it was open
			if (pChangeHistoryPage->m_bChangeHistory)
				pChangeHistoryPage->CleanChangeHistory();
            
			//make the first page active
			((CPropertySheet*) this->GetParent())->SetActivePage(0);

			SetModified (FALSE);
			
			m_nDialogMode = MODE_DONE;
			m_CntlCardTypeFld.EnableWindow(TRUE);
			//return CPropertyPage::OnApply();
         return FALSE;
		}	
    }
    //if we are in add mode
	if (m_nDialogMode == MODE_ADD)
	{
		//clean CTF01
		memset(&sCardTypeDatabaseStructure, 0, sizeof(sCardTypeDatabaseStructure));
		//call the function that populates CTF01 structure from dialog
		if (!PopulateCardTypeDatabaseStructureFromDialog(&sCardTypeDatabaseStructure))
		{		
			return FALSE;
		}
		//this function inserts record into CTF01
		int rcode = txdsapi_insert_record ((pBYTE)&sCardTypeDatabaseStructure,
			                sizeof(CTF01),CTF01_DATA, strErrorMessage);
		if (rcode != PTEMSG_OK)
		{   
			//give an error message if insertion failed
			m_StrMessageLbl = strErrorMessage;
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
		else 
		{ 	
			//set current data field to "Record Added"
			memcpy((LPSTR)m_strCurrentData, "Record Added", sizeof(m_strCurrentData));
			//set previous data field to zero
			memset( m_strPreviousData, 0, sizeof(m_strPreviousData));
			//set field tag to zero
		    memset( m_strFieldTag, 0, sizeof(m_strFieldTag));
			//call function to populate history table
			PopulateHistoryTable();

			//make the first page active
			((CPropertySheet*) this->GetParent())->SetActivePage(0);		
			
			//set message label to "Record Added"
			m_StrMessageLbl = "Record Added";
			//focus cursor on card type field
			m_CntlCardTypeFld.SetFocus();
			//enable card type field
			m_CntlCardTypeFld.EnableWindow(TRUE);

			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
            //clean description dialog
			pDescriptionPage->CleanCardDescriptionDialog();
			//disable description dialog
			pDescriptionPage->EnableCardDescriptionDialog(FALSE);			
            //clean change history dialog if it was open
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();

			SetModified (FALSE);
			m_nDialogMode = MODE_DONE;
			//return CPropertyPage::OnApply();
         return FALSE;
		} 
	}	
	//return CPropertyPage::OnApply();
   return FALSE;
}

/******************************************************************************
 *
 * MODULE:	    OnCancel
 *
 * DESCRIPTION: This function indicates the user clicked Cancel button.
 *
 * AUTHOR:      Narina Pogosyants
 *
 * INPUTS:      None
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
void CCardDefinition::OnCancel() 
{   //clean ChangeHistory page if it was active
	if (pChangeHistoryPage->m_bChangeHistory)
		pChangeHistoryPage->CleanChangeHistory();
    //clean Description page if it was active
	if (pDescriptionPage->m_bDescription)
	{
		pDescriptionPage->CleanCardDescriptionDialog();
		pDescriptionPage->EnableCardDescriptionDialog(FALSE);	   
	}
	
	CPropertyPage::OnCancel();
}

/******************************************************************************
 *
 * MODULE:	    PopulateCardTypeDialogFromDatabase
 *
 * DESCRIPTION: This function populates dialog from Card Type database
 *              structure.
 *
 * AUTHOR:      Narina Pogosyants
 *
 * INPUTS:      pCardType - pointer of the Card Type structure             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  Bool(true--exited successfully,false--not exited)
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/

BOOL CCardDefinition::PopulateCardTypeDialogFromDatabase ( CTF01 pCardType)
{
	//sets fields to the values from CTF01 table
    pDescriptionPage->m_StrCardTypeFld = (LPSTR)pCardType.primary_key.card_type;
	pDescriptionPage->m_StrDescriptionFld = (LPSTR)pCardType.description;
	pDescriptionPage->UpdateData(FALSE);
	//enables dialog
	pDescriptionPage->EnableCardDescriptionDialog(TRUE);   
	
    return TRUE;
}

/******************************************************************************
 *
 * MODULE:	    PopulateCardTypeDatabaseStructureFromDialog
 *
 * DESCRIPTION: This function populates Card Type database structure
 *              from dialog.
 *
 * AUTHOR:      Narina Pogosyants

 * INPUTS:      pCardType - pointer to the Card Type structure *              
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  Bool(true--exited successfully,false--not exited)
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/

BOOL CCardDefinition::PopulateCardTypeDatabaseStructureFromDialog ( pCTF01 pCardType ) 
{
	//clean CTF01 structure
    ZeroMemory (pCardType, sizeof(CTF01));
    
	//copy card type value from string to CTF01
	memcpy ((LPTSTR)pCardType->primary_key.card_type,
		      m_StrCardTypeFld,	
		                sizeof(pCardType->primary_key.card_type));
	//copy card description from string to CTF01
	memcpy ((LPTSTR)pCardType->description,
		      pDescriptionPage->m_StrDescriptionFld,
			            sizeof(pCardType->description));
    //if we are in update mode
	if ( m_nDialogMode == MODE_EDIT )
	{   
		//compare current string value with one in the table
		if ( strcmp ( (char *)sCardTypeOld.description,
			      pDescriptionPage->m_StrDescriptionFld)!= 0)
		{
			//if values are the same populate history table
			//set current data to the value of description field
			memcpy ((LPTSTR) m_strCurrentData,
				      pDescriptionPage->m_StrDescriptionFld,
				                sizeof(m_strCurrentData));
			//set previous data to the value of description field in CTF01 table
			memcpy ((LPTSTR) m_strPreviousData,
				      sCardTypeOld.description,
					            sizeof(m_strPreviousData));
			//set field tag to "Description"
			memcpy ((LPTSTR) m_strFieldTag,
				      "Description",
					            sizeof(m_strFieldTag));
			PopulateHistoryTable();		
		}
	}    	
	return TRUE;
}

/******************************************************************************
 *
 * MODULE:	    CheckDialogForCompleteness
 *
 * DESCRIPTION: This function checks if all the required fields have been 
 *              filled in 
 *
 * AUTHOR:      Narina Pogosyants
 *
 * INPUTS:      None
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  Bool(true--exited successfully,false--not exited)
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
BOOL CCardDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;
     
    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	//this function checks if field is empty
    if (myUtilsObject.IsEmpty(m_StrCardTypeFld, this))
    {
		//give an error message if Card Type field is empty
		m_StrMessageLbl = "Please enter Card Type";	
		//set cursor on Card Type field
		m_CntlCardTypeFld.SetFocus();
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}
    return TRUE;
}

/******************************************************************************
 *
 * MODULE:	    PopulateHistoryTable
 *
 * DESCRIPTION: This function populates History database structure. *              
 *
 * AUTHOR:      Narina Pogosyants

 * INPUTS:      None          
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
void CCardDefinition::PopulateHistoryTable()
{
	CMyUtils myUtilsObject;
    HST01 sHistoryData;
    CString temp;

    CHAR dest_str[18]; 	
	char strErrorMsg [200] = "";
    char strDate[9] = "";
    char strTime[7] = "";
    
	//clean strings that hold date & time 
    memset ( strDate, 0, sizeof(strDate));
    memset ( strTime, 0, sizeof(strTime));
    
	//this function gets current date & time
    myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);
    
	//this function writes generated timestamp in string
    pintime_get_gmt ( dest_str );

	//clean HST01
    ZeroMemory ( &sHistoryData, sizeof(HST01));
    
	//copy file name to HST01
    memcpy ((LPTSTR) sHistoryData.file_name,
		      (LPCTSTR)"CTF01",
			            sizeof(sHistoryData.file_name));
	//copy date to HST01
    temp = strDate;
    if ( temp.GetLength() != 0 )
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    //copy card type to HST01
    memcpy ((LPTSTR) sHistoryData.file_primary_key,
		      (LPCTSTR)m_StrCardTypeFld,
			            sizeof(sHistoryData.file_primary_key));
    //copy timestamp to HST01
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,
		      (LPCTSTR)dest_str,
			            sizeof(sHistoryData.primary_key.unique_id));
	//copy time to HST01
    memcpy ((LPTSTR) sHistoryData.time_modified,
		      (LPCTSTR)strTime,
			            sizeof(sHistoryData.time_modified)); 
    //copy current value to HST01         
    memcpy ((LPTSTR)sHistoryData.current_data,
		      (LPCTSTR)m_strCurrentData,
			            sizeof(sHistoryData.current_data));
    //copy previous value to HST01
	memcpy ((LPTSTR)sHistoryData.previous_data,
		      (LPCTSTR)m_strPreviousData,
			            sizeof(sHistoryData.previous_data));
    //copy user name to HST01
	memcpy ((LPTSTR)sHistoryData.changed_by,
		      (LPCTSTR)ucf01.primary_key.name,
			            sizeof(sHistoryData.changed_by));
    //copy field tag to HST01
	memcpy ((LPTSTR)sHistoryData.field_tag,
		      (LPCTSTR)m_strFieldTag,
			            sizeof(sHistoryData.field_tag));
    //insert record into HST01    
    int rcode = txdsapi_insert_record((pBYTE)&sHistoryData,
		                sizeof(HST01), HST01_DATA, strErrorMsg);
	if ( rcode != PTEMSG_OK )
	{   
		//give an error message if insertion failed
		m_StrMessageLbl = strErrorMsg;
		UpdateData( FALSE );
	}    
}

/******************************************************************************
 *
 * MODULE:	    OnSetActive
 *
 * DESCRIPTION: This function 
 *
 * AUTHOR:      Narina Pogosyants
 *
 * INPUTS:      None
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
BOOL CCardDefinition::OnSetActive() 
{
	((CCardTypePropertySheet*) this->GetParent())->m_nActiveTab = 0;
	
	return CPropertyPage::OnSetActive();
}

BOOL CCardDefinition::OnHelpInfo(HELPINFO* pHelpInfo) 
{
/*	HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
	"bin.chm>Main",HH_HELP_CONTEXT,IDD_CARD_DEFINITION);	
*/	
	return TRUE;
}


void CCardDefinition::OnAddButton() 
{
	CMyUtils myUtilsObject;
	CTF01 sCardType;
	char strErrorMessage[200];    

	m_StrMessageLbl = "";

	m_nDialogMode = MODE_ADD;  

	if (!CheckDialogForCompleteness())
        return;
	 
	pDescriptionPage->m_nDialogMode = MODE_ADD;

	ZeroMemory(&sCardType, sizeof(CTF01)) ;

    memcpy ((LPSTR)(sCardType.primary_key.card_type),
	          m_StrCardTypeFld,
		                sizeof(sCardType.primary_key.card_type));

	int rcode = txdsapi_get_record ((pBYTE)&sCardType,
		      sizeof(CTF01), CTF01_DATA,strErrorMessage);
	if (rcode == PTEMSG_OK)
	{    
	    m_StrMessageLbl = "ERROR: Card Type already exists";
        m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
		    AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	    }
		return ;
    }	
    
	((CPropertySheet*) this->GetParent())->SetWindowText("Add Card Type");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pDescriptionPage->m_bDescription )
	{
		pDescriptionPage->CleanCardDescriptionDialog();
		pDescriptionPage->UpdateData(FALSE);		
		pDescriptionPage->EnableCardDescriptionDialog(TRUE);
	}
	pDescriptionPage->m_StrCardTypeFld = m_StrCardTypeFld;
	
	((CPropertySheet*) this->GetParent())->SetActivePage(1);	

	m_CntlCardTypeFld.EnableWindow(FALSE);	
}

void CCardDefinition::OnDeleteButton() 
{
	CString strMessage;
	CTF01 sCardType;
    int nReturnCode;    
	char strErrorMessage[200];

	m_StrMessageLbl = "";

    m_nDialogMode = MODE_EDIT;

    if (!CheckDialogForCompleteness())
        return;   
   
    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	strMessage.Format ("Are you sure you want to delete %s ?", (LPCTSTR)m_StrCardTypeFld);

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION);

	if (nReturnCode != IDOK)
		return ;

	memcpy ((LPTSTR)sCardType.primary_key.card_type,
	          m_StrCardTypeFld,
			            sizeof(sCardType.primary_key.card_type));
	
	int rcode = txdsapi_delete_record ((pBYTE)&sCardType,
		                sizeof( CTF01 ), CTF01_DATA, strErrorMessage);
	if (rcode != PTEMSG_OK)
	{        
		m_StrMessageLbl = "ERROR: Card Type does not exist";
      m_nDialogMode = MODE_DONE;

		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

    else
	{
        m_StrMessageLbl = "Record Deleted";
		

		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		memset ( m_strPreviousData, 0, sizeof(m_strPreviousData));
		memset ( m_strFieldTag, 0, sizeof(m_strFieldTag));
        memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof(m_strCurrentData));
        PopulateHistoryTable();
		m_StrCardTypeFld = _T("");
		if (pDescriptionPage->m_bDescription)
		{
			pDescriptionPage->CleanCardDescriptionDialog();
			pDescriptionPage->EnableCardDescriptionDialog(FALSE);
		}

		m_nDialogMode = MODE_DONE;
		
		m_CntlCardTypeFld.EnableWindow(TRUE);

		if (pChangeHistoryPage->m_bChangeHistory)
			pChangeHistoryPage->CleanChangeHistory();

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }
}

void CCardDefinition::OnUpdateButton() 
{
    HST01 sHistoryRecord;
	char strErrorMessage[200];
    
    m_nDialogMode = MODE_EDIT;
    pDescriptionPage->m_nDialogMode = MODE_EDIT;
    pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->bNext = FALSE;

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;
    memcpy ((LPSTR)(sCardTypeOld.primary_key.card_type),
		      m_StrCardTypeFld,
			            sizeof(sCardTypeOld.primary_key.card_type));

	int rcode = txdsapi_get_record ((pBYTE)&sCardTypeOld, sizeof(CTF01), CTF01_DATA,strErrorMessage);
	if (rcode != PTEMSG_OK)
	{        
        m_StrMessageLbl = "ERROR: Card Type does not exist";
      m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
        return;
    }

	pDescriptionPage->m_StrCardTypeFld = m_StrCardTypeFld;	   

    ZeroMemory (&sHistoryRecord, sizeof(sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),
		      "                 ",
			            sizeof(sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),
		      "CTF01",
			            sizeof(sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key),
		      (LPSTR)sCardTypeOld.primary_key.card_type,
			            sizeof(sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord),
		      &sHistoryRecord,
			            sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);  
    

	((CPropertySheet*) this->GetParent())->SetWindowText("Update Card Type");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
	
	if (pChangeHistoryPage->m_bChangeHistory)
		pChangeHistoryPage->PopulateHistoryDialog();

	PopulateCardTypeDialogFromDatabase (sCardTypeOld);

	m_CntlCardTypeFld.EnableWindow(FALSE);		
}

void CCardDefinition::OnViewButton() 
{
    CTF01 sCardType;
    HST01 sHistoryRecord;
	 char strErrorMessage[200];

	 m_StrMessageLbl = "";

    m_nDialogMode = MODE_VIEW;
    pDescriptionPage->m_nDialogMode = MODE_VIEW;    
    pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->bNext = FALSE;
   
    if (!CheckDialogForCompleteness())
        return;	
	
    memcpy ((LPSTR)(sCardType.primary_key.card_type),
		      m_StrCardTypeFld,
			            sizeof(sCardType.primary_key.card_type) );
   
	int rcode = txdsapi_get_record ((pBYTE)&sCardType,
		                sizeof(CTF01), CTF01_DATA,strErrorMessage);

	if (rcode != PTEMSG_OK)
	{     
      m_StrMessageLbl = "ERROR: Card Type does not exist";
      m_nDialogMode = MODE_DONE;

		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
        return;
    }

    ZeroMemory (&sHistoryRecord, sizeof(sHistoryRecord));

    ZeroMemory (&sHistoryRecord, sizeof(sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),
		      "                 ",
			            sizeof(sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),
		      "CTF01",
			            sizeof(sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key),
		      (LPSTR)sCardType.primary_key.card_type,
			            sizeof(sHistoryRecord.file_primary_key));
    memcpy ( &(pChangeHistoryPage->sHistoryRecord),
		      &sHistoryRecord,
			            sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);      
    

	((CPropertySheet*) this->GetParent())->SetWindowText("View Card Type");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if (pChangeHistoryPage->m_bChangeHistory)
		pChangeHistoryPage->PopulateHistoryDialog();

    SetModified (FALSE);

	PopulateCardTypeDialogFromDatabase (sCardType);

	m_CntlCardTypeFld.EnableWindow(TRUE);	
}

void CCardDefinition::OnListButton() 
{
	CNameList listDlg;
    int nReturnCode;

	m_StrMessageLbl = "";

	UpdateData(TRUE);

    listDlg.m_nIndicator = MODE_CARD;
    listDlg.strPrimaryKey1 = m_StrCardTypeFld;
   
    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
        m_StrCardTypeFld = name;
        
    if (!UpdateData(FALSE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
}

BOOL CCardDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
