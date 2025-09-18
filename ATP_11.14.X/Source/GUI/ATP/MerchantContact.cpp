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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantContact.cpp  $
   
      Rev 1.9   Jul 08 2004 13:00:38   dirby
   Updated History File logging to make sure Merchant ID is in the
   File_Primary_Key and to make sure fields are not accidently logged
   when no changes have been made.
   SCR 1421
   
      Rev 1.8   Dec 01 2003 11:29:36   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Feb 10 2002 00:34:34   dirby
   Modified PopulateHistoryTable function so the unique_id field
   does not give a constraint violation.  It was frequently giving a
   unique constraint violation when updating the general and
   blocking tabs because it could do more than 1 within the same
   millisecond.  Also, if the data server returned with an error,
   there was an UpdateData(FALSE) command that would crash
   the program because it was trying to update a window that did
   not exist.  I replaced the command with an informational box.
   
      Rev 1.6   Jan 05 2000 14:53:54   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:52:36   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:44   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:48   iarustam
   Initial Release
   
      Rev 1.3   Apr 29 1999 14:56:34   iarustam
   Bug fixes
   
      Rev 1.2   Apr 01 1999 14:09:04   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:04   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:40   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:36   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:08   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:30   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:38   iarustam
    
*
************************************************************************************/
// MerchantContact.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantContact.h"
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

extern UCF01 ucf01;
//extern merch_ctr;
extern int merch_ctr;
extern CSharedMem shrmem;

extern CString  MerchantID;
extern CString  OrganizationID;

/////////////////////////////////////////////////////////////////////////////
// CMerchantContact property page

IMPLEMENT_DYNCREATE(CMerchantContact, CPropertyPage)

CMerchantContact::CMerchantContact() : CPropertyPage(CMerchantContact::IDD)
{
	//{{AFX_DATA_INIT(CMerchantContact)
	m_StrContactNameFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrPrimaryPhoneNumberFld = _T("");
	m_StrSecondaryPhoneNumberFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	//}}AFX_DATA_INIT
}

CMerchantContact::~CMerchantContact()
{
}

void CMerchantContact::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantContact)
	DDX_Control(pDX, IDC_SECONDARY_PHONE_NUMBER_FLD, m_CntlSecondaryPhoneNumberFld);
	DDX_Control(pDX, IDC_PRIMARY_PHONE_NUMBER_FLD, m_CntlPrimaryPhoneNumberFld);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Control(pDX, IDC_CONTACT_NAME_FLD, m_CntlContactNameFld);
	DDX_Text(pDX, IDC_CONTACT_NAME_FLD, m_StrContactNameFld);
	DDV_MaxChars(pDX, m_StrContactNameFld, 30);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_PRIMARY_PHONE_NUMBER_FLD, m_StrPrimaryPhoneNumberFld);
	DDV_MaxChars(pDX, m_StrPrimaryPhoneNumberFld, 17);
	DDX_Text(pDX, IDC_SECONDARY_PHONE_NUMBER_FLD, m_StrSecondaryPhoneNumberFld);
	DDV_MaxChars(pDX, m_StrSecondaryPhoneNumberFld, 17);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CHAIN_ID_FLD, m_StrOrganizationIDFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantContact, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantContact)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantContact message handlers


BOOL CMerchantContact::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

    m_bContact = TRUE;

	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_VIEW )
			EnableContactDialog(TRUE);	
		if ( m_nDialogMode != MODE_ADD )
			PopulateContactDialogFromDatabase ( sMerchantRecordOld );
	}  
	
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMerchantContact::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified (TRUE);
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CMerchantContact::EnableContactDialog ( int nStatus )
{
	m_CntlContactNameFld.EnableWindow(nStatus);
	m_CntlPrimaryPhoneNumberFld.EnableWindow(nStatus);
	m_CntlSecondaryPhoneNumberFld.EnableWindow(nStatus);

}

BOOL CMerchantContact::PopulateContactDialogFromDatabase ( MCF01 sMerchantRecord )
{
	CString temp;

	if ( m_nDialogMode != MODE_VIEW )	
		EnableContactDialog(TRUE);

	ZeroMemory ( &sMerchantRecordOld, sizeof ( MCF01) );

	m_StrMerchantIDFld = sMerchantRecord.primary_key.merchant_id;
	m_StrOrganizationIDFld = sMerchantRecord.primary_key.organization_id;


	m_StrContactNameFld = sMerchantRecord.contact;
	m_StrPrimaryPhoneNumberFld = sMerchantRecord.phone01;
	m_StrSecondaryPhoneNumberFld = sMerchantRecord.phone02;	
	
	UpdateData(FALSE);

	memcpy ( &sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));

	return TRUE;
}

void CMerchantContact::CleanContactDialog()
{
	m_StrContactNameFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrPrimaryPhoneNumberFld = _T("");
	m_StrSecondaryPhoneNumberFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");

	UpdateData (FALSE);
}

BOOL  CMerchantContact::PopulateDatabaseFromContactDialog ( pMCF01 psMerchantRecord )
{
	if ( !m_bContact )
		return TRUE;


	memcpy ( psMerchantRecord->contact,	m_StrContactNameFld,		 sizeof ( psMerchantRecord->contact));
	memcpy ( psMerchantRecord->phone01,	m_StrPrimaryPhoneNumberFld,	 sizeof ( psMerchantRecord->phone01));
	memcpy ( psMerchantRecord->phone02,	m_StrSecondaryPhoneNumberFld,sizeof ( psMerchantRecord->phone02));

//	memcpy ( &sMerchantRecordNew, psMerchantRecord, sizeof ( MCF01));

	return TRUE;
}

BOOL CMerchantContact::CompareStructures()
{
   if ( m_bContact == TRUE )
   {
      if (strcmp ( m_StrContactNameFld, (char *)sMerchantRecordOld.contact) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrContactNameFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.contact, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Contact Name", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

      if (strcmp ( m_StrPrimaryPhoneNumberFld, (char *)sMerchantRecordOld.phone01) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrPrimaryPhoneNumberFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.phone01, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Primary Phone Number", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }

      if (strcmp ( m_StrSecondaryPhoneNumberFld, (char *)sMerchantRecordOld.phone02) != 0)
      {
         memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
         memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
         memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

         memcpy(m_strCurrentData, m_StrSecondaryPhoneNumberFld, sizeof (m_strCurrentData));
         memcpy(m_strPreviousData, sMerchantRecordOld.phone02, sizeof (m_strPreviousData));
         memcpy(m_strFieldTag, "Sec. Phone Number", sizeof (m_strFieldTag));

         PopulateHistoryTable();
      }
   }
	return TRUE;
}

void CMerchantContact::PopulateHistoryTable()
{
   HST01 sHistoryData;
   CHAR dest_str[18];   
   CMyUtils myUtilsObject;
   char strErrorMsg[200] = "";
   char strDate[9]       = "";
   char strTime[7]       = "";
   char strPrimaryKey[50]= "";
   CHAR s_merch_ctr[4]   = "";
   CHAR strDisplayErrMsg[250];
   CString temp;


   strcpy( strPrimaryKey, MerchantID     );
   strcat( strPrimaryKey, OrganizationID );

   myUtilsObject.GetSystemDate(strDate);  
   myUtilsObject.GetSystemTime(strTime);

   /* Get unique primary key for history table.
    * Use a static counter, instead of milliseconds,
    * to prevent unique constraint violations.
    */
   pintime_get_gmt ( dest_str );
   merch_ctr++;
   if ( merch_ctr > 999 )
      merch_ctr = 0;
   sprintf( s_merch_ctr, "%03d", merch_ctr );
   memcpy( &dest_str[14], s_merch_ctr, 3 );

   ZeroMemory ( &sHistoryData, sizeof (HST01));
   memcpy ((LPTSTR)sHistoryData.file_name,
           (LPCTSTR)"MCF01",
           sizeof (sHistoryData.file_name));

   temp = strDate;
   temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);

   CHAR wstation[21]; DWORD wsize;

   wsize = sizeof( wstation );
   GetComputerName( wstation, &wsize );

   memcpy ((LPTSTR)sHistoryData.reserved,             (LPTSTR)wstation,               sizeof(sHistoryData.reserved));
   memcpy ((LPTSTR) sHistoryData.date_modified,       temp,                           sizeof (sHistoryData.date_modified));
   memcpy ((LPTSTR)sHistoryData.file_primary_key,     (LPCTSTR)strPrimaryKey,         sizeof (sHistoryData.file_primary_key));
   memcpy ((LPTSTR)sHistoryData.primary_key.unique_id,(LPCTSTR)dest_str,              sizeof (sHistoryData.primary_key.unique_id));        
   memcpy ((LPTSTR)sHistoryData.time_modified,        (LPCTSTR)strTime,               sizeof (sHistoryData.time_modified));              
   memcpy ((LPTSTR)sHistoryData.current_data,         (LPCTSTR)m_strCurrentData,      sizeof (sHistoryData.current_data));
   memcpy ((LPTSTR)sHistoryData.previous_data,        (LPCTSTR)m_strPreviousData,     sizeof (sHistoryData.previous_data));
   memcpy ((LPTSTR)sHistoryData.field_tag,            (LPCTSTR)m_strFieldTag,         sizeof (sHistoryData.field_tag));
   memcpy ((LPTSTR)sHistoryData.changed_by,           (LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));

   int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
   if ( rcode != PTEMSG_OK )
   {
      m_nDialogMode = MODE_DONE;
      memset( strDisplayErrMsg, 0x00, sizeof(strDisplayErrMsg) );
      sprintf( strDisplayErrMsg,
              "Unable to update History Table HST01 - %s",
               strErrorMsg );
      AfxMessageBox( strDisplayErrMsg, MB_OK );
   }
   return;
}
