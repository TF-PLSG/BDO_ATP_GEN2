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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\GroupAccess.cpp  $
   
      Rev 1.8   Dec 01 2003 11:29:22   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 05 2000 14:53:34   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.0   Jun 28 1999 13:58:32   iarustam
   Initial Release
   
      Rev 1.3   Jun 11 1999 14:08:48   iarustam
   Bug fixes
   
   
      Rev 1.2   Apr 29 1999 14:56:22   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:29:54   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:26   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:26   iarustam
   Initial Release
   
      Rev 1.0   Feb 12 1999 11:05:30   iarustam
   initial release
   
      Rev 1.1   Dec 18 1998 13:43:24   iarustam
    
*
************************************************************************************/
// GroupAccess.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "GroupAccess.h"
//#include "PTEDataAccess.h"
#include "MyUtils.h"
extern "C" {
#include "PTETIME.h"
}

//#include "htmlhelp.h"
#include "GroupPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;

/////////////////////////////////////////////////////////////////////////////
// CGroupAccess property page

IMPLEMENT_DYNCREATE(CGroupAccess, CPropertyPage)

CGroupAccess::CGroupAccess() : CPropertyPage(CGroupAccess::IDD)
{
	//{{AFX_DATA_INIT(CGroupAccess)
	m_StrLabel1 = _T("");
	m_StrLabel10 = _T("");
	m_StrLabel2 = _T("");
	m_StrLabel3 = _T("");
	m_StrLabel4 = _T("");
	m_StrLabel5 = _T("");
	m_StrLabel6 = _T("");
	m_StrLabel7 = _T("");
	m_StrLabel8 = _T("");
	m_StrLabel9 = _T("");
	m_StrGroupIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrLabel11 = _T("");
	m_StrLabel12 = _T("");
	m_StrLabel13 = _T("");
	m_StrLabel15 = _T("");
	m_StrLabel19 = _T("");
	m_StrLabel20 = _T("");
	m_StrLabel21 = _T("");
	m_StrLabel22 = _T("");
	m_StrLabel23 = _T("");
	//}}AFX_DATA_INIT
}

CGroupAccess::~CGroupAccess()
{
}

void CGroupAccess::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupAccess)
	DDX_Control(pDX, IDC_VIEW_CHECK19, m_CntlViewCheck19);
	DDX_Control(pDX, IDC_UPDATE_CHECK19, m_CntlUpdateCheck19);
	DDX_Control(pDX, IDC_DELETE_CHECK19, m_CntlDeleteCheck19);
	DDX_Control(pDX, IDC_ADD_CHECK19, m_CntlAddCheck19);
	DDX_Control(pDX, IDC_CHECK_ALL, m_CntlCheckAll);
	DDX_Control(pDX, IDC_VIEW_CHECK15, m_CntlViewCheck15);
	DDX_Control(pDX, IDC_VIEW_CHECK13, m_CntlViewCheck13);
	DDX_Control(pDX, IDC_UPDATE_CHECK13, m_CntlUpdateCheck13);
	DDX_Control(pDX, IDC_DELETE_CHECK13, m_CntlDeleteCheck13);
	DDX_Control(pDX, IDC_ADD_CHECK13, m_CntlAddCheck13);
	DDX_Control(pDX, IDC_UPDATE_CHECK12, m_CntlUpdateCheck12);
	DDX_Control(pDX, IDC_VIEW_CHECK12, m_CntlViewCheck12);
	DDX_Control(pDX, IDC_DELETE_CHECK12, m_CntlDeleteCheck12);
	DDX_Control(pDX, IDC_ADD_CHECK12, m_CntlAddCheck12);
	DDX_Control(pDX, IDC_VIEW_CHECK11, m_CntlViewCheck11);
	DDX_Control(pDX, IDC_UPDATE_CHECK11, m_CntlUpdateCheck11);
	DDX_Control(pDX, IDC_DELETE_CHECK11, m_CntlDeleteCheck11);
	DDX_Control(pDX, IDC_ADD_CHECK11, m_CntlAddCheck11);
	DDX_Control(pDX, IDC_VIEW_CHECK9, m_CntlViewCheck9);
	DDX_Control(pDX, IDC_VIEW_CHECK8, m_CntlViewCheck8);
	DDX_Control(pDX, IDC_VIEW_CHECK7, m_CntlViewCheck7);
	DDX_Control(pDX, IDC_VIEW_CHECK6, m_CntlViewCheck6);
	DDX_Control(pDX, IDC_VIEW_CHECK5, m_CntlViewCheck5);
	DDX_Control(pDX, IDC_VIEW_CHECK4, m_CntlViewCheck4);
	DDX_Control(pDX, IDC_VIEW_CHECK3, m_CntlViewCheck3);
	DDX_Control(pDX, IDC_VIEW_CHECK2, m_CntlViewCheck2);
	DDX_Control(pDX, IDC_VIEW_CHECK10, m_CntlViewCheck10);
	DDX_Control(pDX, IDC_VIEW_CHECK1, m_CntlViewCheck1);
	DDX_Control(pDX, IDC_UPDATE_CHECK9, m_CntlUpdateCheck9);
	DDX_Control(pDX, IDC_UPDATE_CHECK8, m_CntlUpdateCheck8);
	DDX_Control(pDX, IDC_UPDATE_CHECK7, m_CntlUpdateCheck7);
	DDX_Control(pDX, IDC_UPDATE_CHECK6, m_CntlUpdateCheck6);
	DDX_Control(pDX, IDC_UPDATE_CHECK5, m_CntlUpdateCheck5);
	DDX_Control(pDX, IDC_UPDATE_CHECK4, m_CntlUpdateCheck4);
	DDX_Control(pDX, IDC_UPDATE_CHECK3, m_CntlUpdateCheck3);
	DDX_Control(pDX, IDC_UPDATE_CHECK2, m_CntlUpdateCheck2);
	DDX_Control(pDX, IDC_UPDATE_CHECK10, m_CntlUpdateCheck10);
	DDX_Control(pDX, IDC_UPDATE_CHECK1, m_CntlUpdateCheck1);
	DDX_Control(pDX, IDC_DELETE_CHECK9, m_CntlDeleteCheck9);
	DDX_Control(pDX, IDC_DELETE_CHECK8, m_CntlDeleteCheck8);
	DDX_Control(pDX, IDC_DELETE_CHECK7, m_CntlDeleteCheck7);
	DDX_Control(pDX, IDC_DELETE_CHECK6, m_CntlDeleteCheck6);
	DDX_Control(pDX, IDC_DELETE_CHECK5, m_CntlDeleteCheck5);
	DDX_Control(pDX, IDC_DELETE_CHECK4, m_CntlDeleteCheck4);
	DDX_Control(pDX, IDC_DELETE_CHECK3, m_CntlDeleteCheck3);
	DDX_Control(pDX, IDC_DELETE_CHECK2, m_CntlDeleteCheck2);
	DDX_Control(pDX, IDC_DELETE_CHECK10, m_CntlDeleteCheck10);
	DDX_Control(pDX, IDC_DELETE_CHECK1, m_CntlDeleteCheck1);
	DDX_Control(pDX, IDC_GTLF_EXTRACT, m_CntlTlfExtract);
	DDX_Control(pDX, IDC_ADD_CHECK9, m_CntlAddCheck9);
	DDX_Control(pDX, IDC_ADD_CHECK8, m_CntlAddCheck8);
	DDX_Control(pDX, IDC_ADD_CHECK7, m_CntlAddCheck7);
	DDX_Control(pDX, IDC_ADD_CHECK6, m_CntlAddCheck6);
	DDX_Control(pDX, IDC_ADD_CHECK5, m_CntlAddCheck5);
	DDX_Control(pDX, IDC_ADD_CHECK4, m_CntlAddCheck4);
	DDX_Control(pDX, IDC_ADD_CHECK3, m_CntlAddCheck3);
	DDX_Control(pDX, IDC_ADD_CHECK2, m_CntlAddCheck2);
	DDX_Control(pDX, IDC_ADD_CHECK10, m_CntlAddCheck10);
	DDX_Control(pDX, IDC_ADD_CHECK1, m_CntlAddCheck1);
	DDX_Text(pDX, IDC_LABEL1, m_StrLabel1);
	DDX_Text(pDX, IDC_LABEL10, m_StrLabel10);
	DDX_Text(pDX, IDC_LABEL2, m_StrLabel2);
	DDX_Text(pDX, IDC_LABEL3, m_StrLabel3);
	DDX_Text(pDX, IDC_LABEL4, m_StrLabel4);
	DDX_Text(pDX, IDC_LABEL5, m_StrLabel5);
	DDX_Text(pDX, IDC_LABEL6, m_StrLabel6);
	DDX_Text(pDX, IDC_LABEL7, m_StrLabel7);
	DDX_Text(pDX, IDC_LABEL8, m_StrLabel8);
	DDX_Text(pDX, IDC_LABEL9, m_StrLabel9);
	DDX_Text(pDX, IDC_GROUP_ID_FLD, m_StrGroupIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_LABEL11, m_StrLabel11);
	DDX_Text(pDX, IDC_LABEL12, m_StrLabel12);
	DDX_Text(pDX, IDC_LABEL13, m_StrLabel13);
	DDX_Text(pDX, IDC_LABEL15, m_StrLabel15);
	DDX_Text(pDX, IDC_LABEL19, m_StrLabel19);
	DDX_Text(pDX, IDC_LABEL20, m_StrLabel20);
	DDX_Text(pDX, IDC_FRAUD_LABEL21, m_StrLabel22);
	DDX_Control(pDX, IDC_GLOBAL_VIEW_CHECK20, m_CntlViewCheck20);
	DDX_Control(pDX, IDC_GLOBAL_UPDATE_CHECK20, m_CntlUpdateCheck20);
	DDX_Text(pDX, IDC_LABEL21, m_StrLabel21);
	DDX_Control(pDX, IDC_USER_STATUS_VIEW_CHECK21, m_CntlViewCheck21);
	DDX_Control(pDX, IDC_FRAUD_VIEW_CHECK21, m_CntlViewCheck22);
	DDX_Control(pDX, IDC_FRAUD_UPDATE_CHECK21, m_CntlUpdateCheck22);
	DDX_Control(pDX, IDC_FRAUD_DELETE_CHECK21, m_CntlDeleteCheck22);
	DDX_Control(pDX, IDC_FRAUD_ADD_CHECK21, m_CntlAddCheck22);
	DDX_Text(pDX, IDC_CF_LABEL23, m_StrLabel23);
	DDX_Control(pDX, IDC_CF_VIEW_CHECK23, m_CntlViewCheck23);
	DDX_Control(pDX, IDC_CF_UPDATE_CHECK23, m_CntlUpdateCheck23);
	DDX_Control(pDX, IDC_CF_DELETE_CHECK23, m_CntlDeleteCheck23);
	DDX_Control(pDX, IDC_CF_ADD_CHECK23, m_CntlAddCheck23);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupAccess, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupAccess)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_CHECK_ALL, OnCheckAll)
	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(IDC_DELETE_CHECK19, &CGroupAccess::OnBnClickedDeleteCheck19)
	//ON_BN_CLICKED(IDC_DELETE_CHECK13, &CGroupAccess::OnBnClickedDeleteCheck13)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupAccess message handlers

BOOL CGroupAccess::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CFont m_font;
	LOGFONT lf;
   ATP01 sATPStructure;
   char strErrorMessage[200] = "";

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	ZeroMemory(&sATPStructure,sizeof(ATP01)) ;

 //  memcpy ( sATPStructure.site_name, "ATP SigCapture Concord EFS", sizeof (sATPStructure.site_name));

	int rcode = txdsapi_get_record  ((pBYTE)&sATPStructure,sizeof (ATP01), ATP01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{   
		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE; 

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
    }

 /*   m_StrLabel1 = "System Administration";	
	 m_StrLabel2 = "System Configuration";
    m_StrLabel3 = "System Operations";
    m_StrLabel4 = "BIN Control";
    m_StrLabel5 = "Institution Control";
    m_StrLabel6 = "Authorization Control";
    m_StrLabel7 = "Merchant Control";
    m_StrLabel8 = "Network Control";
    m_StrLabel9 = "Card Account Control";
    m_StrLabel10 = "User Control";*/

	m_StrLabel1 = sATPStructure.system_components[0].subsystem_name;	//System Administration
	m_StrLabel2 = sATPStructure.system_components[1].subsystem_name;	//System Configuration 
	m_StrLabel3 = sATPStructure.system_components[2].subsystem_name;	//System Operations    
	m_StrLabel4 = sATPStructure.system_components[3].subsystem_name;	//BIN Control          
	m_StrLabel5 = sATPStructure.system_components[4].subsystem_name;	//Authorization Control
	m_StrLabel6 = sATPStructure.system_components[5].subsystem_name;	//Merchant Control     
	m_StrLabel7 = sATPStructure.system_components[6].subsystem_name;	//Device Definition    
	m_StrLabel8 = sATPStructure.system_components[7].subsystem_name;	//Network Control      
	m_StrLabel9 = sATPStructure.system_components[8].subsystem_name;	//Card Account Control 
	m_StrLabel10 = sATPStructure.system_components[9].subsystem_name;	//Voice Authorization  
	m_StrLabel11 = sATPStructure.system_components[10].subsystem_name;	//Transaction Lookup   
	m_StrLabel12 = sATPStructure.system_components[14].subsystem_name;	//Recurring Payment    
	//m_StrLabel13 = sATPStructure.system_components[12].subsystem_name;	//Dual Encryption      
	m_StrLabel15 = sATPStructure.system_components[13].subsystem_name;	//Service Manager      
	m_StrLabel19 = sATPStructure.system_components[15].subsystem_name;	//Service Manager      
	m_StrLabel20 = sATPStructure.system_components[16].subsystem_name;
	m_StrLabel21 = sATPStructure.system_components[17].subsystem_name;
	m_StrLabel22 = sATPStructure.system_components[18].subsystem_name;
	m_StrLabel23 = sATPStructure.system_components[19].subsystem_name; //chipfallback
   if ( m_StrLabel1.IsEmpty() )
   {
      m_CntlAddCheck1.ShowWindow(FALSE);
      m_CntlDeleteCheck1.ShowWindow(FALSE);
      m_CntlViewCheck1.ShowWindow(FALSE);
      m_CntlUpdateCheck1.ShowWindow(FALSE);
   }

   if ( m_StrLabel2.IsEmpty() )
   {
      m_CntlAddCheck2.ShowWindow(FALSE);
      m_CntlDeleteCheck2.ShowWindow(FALSE);
      m_CntlViewCheck2.ShowWindow(FALSE);
      m_CntlUpdateCheck2.ShowWindow(FALSE);
   }
   if ( m_StrLabel3.IsEmpty() )
   {
      m_CntlAddCheck3.ShowWindow(FALSE);
      m_CntlDeleteCheck3.ShowWindow(FALSE);
      m_CntlViewCheck3.ShowWindow(FALSE);
      m_CntlUpdateCheck3.ShowWindow(FALSE);
   }
   if ( m_StrLabel4.IsEmpty() )
   {
      m_CntlAddCheck4.ShowWindow(FALSE);
      m_CntlDeleteCheck4.ShowWindow(FALSE);
      m_CntlViewCheck4.ShowWindow(FALSE);
      m_CntlUpdateCheck4.ShowWindow(FALSE);
   }
   if ( m_StrLabel5.IsEmpty() )
   {
      m_CntlAddCheck5.ShowWindow(FALSE);
      m_CntlDeleteCheck5.ShowWindow(FALSE);
      m_CntlViewCheck5.ShowWindow(FALSE);
      m_CntlUpdateCheck5.ShowWindow(FALSE);
   }
   if ( m_StrLabel6.IsEmpty() )
   {
      m_CntlAddCheck6.ShowWindow(FALSE);
      m_CntlDeleteCheck6.ShowWindow(FALSE);
      m_CntlViewCheck6.ShowWindow(FALSE);
      m_CntlUpdateCheck6.ShowWindow(FALSE);
   }
   if ( m_StrLabel7.IsEmpty() )
   {
      m_CntlAddCheck7.ShowWindow(FALSE);
      m_CntlDeleteCheck7.ShowWindow(FALSE);
      m_CntlViewCheck7.ShowWindow(FALSE);
      m_CntlUpdateCheck7.ShowWindow(FALSE);
   }
   if ( m_StrLabel8.IsEmpty() )
   {
      m_CntlAddCheck8.ShowWindow(FALSE);
      m_CntlDeleteCheck8.ShowWindow(FALSE);
      m_CntlViewCheck8.ShowWindow(FALSE);
      m_CntlUpdateCheck8.ShowWindow(FALSE);
   }
   if ( m_StrLabel9.IsEmpty() )
   {
      m_CntlAddCheck9.ShowWindow(FALSE);
      m_CntlDeleteCheck9.ShowWindow(FALSE);
      m_CntlViewCheck9.ShowWindow(FALSE);
      m_CntlUpdateCheck9.ShowWindow(FALSE);
   }
   if ( m_StrLabel10.IsEmpty() )
   {
      m_CntlAddCheck10.ShowWindow(FALSE);
      m_CntlDeleteCheck10.ShowWindow(FALSE);
      m_CntlViewCheck10.ShowWindow(FALSE);
      m_CntlUpdateCheck10.ShowWindow(FALSE);
   }
   if ( m_StrLabel11.IsEmpty() )
   {
      m_CntlAddCheck11.ShowWindow(FALSE);
      m_CntlDeleteCheck11.ShowWindow(FALSE);
      m_CntlViewCheck11.ShowWindow(FALSE);
      m_CntlUpdateCheck11.ShowWindow(FALSE);
	  m_CntlTlfExtract.ShowWindow(FALSE);
   }

   if ( m_StrLabel12.IsEmpty() )
   {
      m_CntlAddCheck12.ShowWindow(FALSE);
      m_CntlDeleteCheck12.ShowWindow(FALSE);
      m_CntlViewCheck12.ShowWindow(FALSE);
      m_CntlUpdateCheck12.ShowWindow(FALSE);
   }

   if ( m_StrLabel13.IsEmpty() )
   {
      m_CntlAddCheck13.ShowWindow(FALSE);
      m_CntlDeleteCheck13.ShowWindow(FALSE);
      m_CntlViewCheck13.ShowWindow(FALSE);
      m_CntlUpdateCheck13.ShowWindow(FALSE);
   }

 	//Service Manager
   if ( m_StrLabel15.IsEmpty() )
   {
      m_CntlViewCheck15.ShowWindow(FALSE);
   }

   //ATP Archiving
   if ( m_StrLabel19.IsEmpty() )
   {
      m_CntlAddCheck19.ShowWindow(FALSE);
      m_CntlDeleteCheck19.ShowWindow(FALSE);
      m_CntlViewCheck19.ShowWindow(FALSE);
      m_CntlUpdateCheck19.ShowWindow(FALSE);
   }
   if ( m_StrLabel20.IsEmpty() )
   {
      
      m_CntlViewCheck20.ShowWindow(FALSE);
      m_CntlUpdateCheck20.ShowWindow(FALSE);
   }
   if ( m_StrLabel21.IsEmpty() )
   {
      m_CntlViewCheck21.ShowWindow(FALSE);
   }
   if(m_StrLabel22.IsEmpty())
   {
		m_CntlViewCheck22.ShowWindow(FALSE);
		m_CntlUpdateCheck22.ShowWindow(FALSE);
		m_CntlAddCheck22.ShowWindow(FALSE);
		m_CntlDeleteCheck22.ShowWindow(FALSE);
   }
    if(m_StrLabel23.IsEmpty())
   {
     		m_CntlAddCheck23.ShowWindow(FALSE);
     		m_CntlDeleteCheck23.ShowWindow(FALSE);
     		m_CntlViewCheck23.ShowWindow(FALSE);
     		m_CntlUpdateCheck23.ShowWindow(FALSE);
     		m_CntlAddCheck23.SetCheck(0);
     		m_CntlDeleteCheck23.SetCheck(0);
     		m_CntlViewCheck23.SetCheck(0);
     		m_CntlUpdateCheck23.SetCheck(0);
   }

  SetModified ( FALSE );

	if ( m_nDialogMode == MODE_ADD )
		EnableGroupAccessDialog(TRUE );
	else
		PopulateDialogFromDatabase();

	m_bAccess = TRUE;

	UpdateData(FALSE);
	
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CGroupAccess::PopulateControlStructure ()

{
   
        ZeroMemory ( m_sControlNew, sizeof (m_sControlNew));

        if(m_CntlAddCheck1.GetCheck() == 1)
            m_sControlNew[0].action[0] = '1';
        else 
            m_sControlNew[0].action[0] = '0';
        if(m_CntlDeleteCheck1.GetCheck() == 1)
            m_sControlNew[0].action[1] = '1';
        else 
            m_sControlNew[0].action[1] = '0';
        if(m_CntlUpdateCheck1.GetCheck() == 1)
            m_sControlNew[0].action[2] = '1';
        else 
            m_sControlNew[0].action[2] = '0';
        if(m_CntlViewCheck1.GetCheck() == 1)
            m_sControlNew[0].action[3] = '1';
        else 
            m_sControlNew[0].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[0].subsystem_name, m_StrLabel1, sizeof (m_sControlNew[0].subsystem_name));


        if(m_CntlAddCheck2.GetCheck() == 1)
            m_sControlNew[1].action[0] = '1';
        else 
            m_sControlNew[1].action[0] = '0';
        if(m_CntlDeleteCheck2.GetCheck() == 1)
            m_sControlNew[1].action[1] = '1';
        else 
            m_sControlNew[1].action[1] = '0';
        if(m_CntlUpdateCheck2.GetCheck() == 1)
            m_sControlNew[1].action[2] = '1';
        else 
            m_sControlNew[1].action[2] = '0';
        if(m_CntlViewCheck2.GetCheck() == 1)
            m_sControlNew[1].action[3] = '1';
        else 
            m_sControlNew[1].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[1].subsystem_name, m_StrLabel2, sizeof (m_sControlNew[1].subsystem_name));

        if(m_CntlAddCheck3.GetCheck() == 1)
            m_sControlNew[2].action[0] = '1';
        else 
            m_sControlNew[2].action[0] = '0';
        if(m_CntlDeleteCheck3.GetCheck() == 1)
            m_sControlNew[2].action[1] = '1';
        else 
            m_sControlNew[2].action[1] = '0';
        if(m_CntlUpdateCheck3.GetCheck() == 1)
            m_sControlNew[2].action[2] = '1';
        else 
            m_sControlNew[2].action[2] = '0';
        if(m_CntlViewCheck3.GetCheck() == 1)
            m_sControlNew[2].action[3] = '1';
        else 
            m_sControlNew[2].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[2].subsystem_name, m_StrLabel3, sizeof (m_sControlNew[2].subsystem_name));

        if(m_CntlAddCheck4.GetCheck() == 1)
            m_sControlNew[3].action[0] = '1';
        else 
            m_sControlNew[3].action[0] = '0';
        if(m_CntlDeleteCheck4.GetCheck() == 1)
            m_sControlNew[3].action[1] = '1';
        else 
            m_sControlNew[3].action[1] = '0';
        if(m_CntlUpdateCheck4.GetCheck() == 1)
            m_sControlNew[3].action[2] = '1';
        else 
            m_sControlNew[3].action[2] = '0';
        if(m_CntlViewCheck4.GetCheck() == 1)
            m_sControlNew[3].action[3] = '1';
        else 
            m_sControlNew[3].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[3].subsystem_name, m_StrLabel4, sizeof (m_sControlNew[3].subsystem_name));

        if(m_CntlAddCheck5.GetCheck() == 1)
            m_sControlNew[4].action[0] = '1';
        else 
            m_sControlNew[4].action[0] = '0';
        if(m_CntlDeleteCheck5.GetCheck() == 1)
            m_sControlNew[4].action[1] = '1';
        else 
            m_sControlNew[4].action[1] = '0';
        if(m_CntlUpdateCheck5.GetCheck() == 1)
            m_sControlNew[4].action[2] = '1';
        else 
            m_sControlNew[4].action[2] = '0';
        if(m_CntlViewCheck5.GetCheck() == 1)
            m_sControlNew[4].action[3] = '1';
        else 
            m_sControlNew[4].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[4].subsystem_name, m_StrLabel5, sizeof (m_sControlNew[4].subsystem_name));

        if(m_CntlAddCheck6.GetCheck() == 1)
            m_sControlNew[5].action[0] = '1';
        else 
            m_sControlNew[5].action[0] = '0';
        if(m_CntlDeleteCheck6.GetCheck() == 1)
            m_sControlNew[5].action[1] = '1';
        else 
            m_sControlNew[5].action[1] = '0';
        if(m_CntlUpdateCheck6.GetCheck() == 1)
            m_sControlNew[5].action[2] = '1';
        else 
            m_sControlNew[5].action[2] = '0';
        if(m_CntlViewCheck6.GetCheck() == 1)
            m_sControlNew[5].action[3] = '1';
        else 
            m_sControlNew[5].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[5].subsystem_name, m_StrLabel6, sizeof (m_sControlNew[5].subsystem_name));

        if(m_CntlAddCheck7.GetCheck() == 1)
            m_sControlNew[6].action[0] = '1';
        else 
            m_sControlNew[6].action[0] = '0';
        if(m_CntlDeleteCheck7.GetCheck() == 1)
            m_sControlNew[6].action[1] = '1';
        else 
            m_sControlNew[6].action[1] = '0';
        if(m_CntlUpdateCheck7.GetCheck() == 1)
            m_sControlNew[6].action[2] = '1';
        else 
            m_sControlNew[6].action[2] = '0';
        if(m_CntlViewCheck7.GetCheck() == 1)
            m_sControlNew[6].action[3] = '1';
        else 
            m_sControlNew[6].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[6].subsystem_name, m_StrLabel7, sizeof (m_sControlNew[6].subsystem_name));

        if(m_CntlAddCheck8.GetCheck() == 1)
            m_sControlNew[7].action[0] = '1';
        else 
            m_sControlNew[7].action[0] = '0';
        if(m_CntlDeleteCheck8.GetCheck() == 1)
            m_sControlNew[7].action[1] = '1';
        else 
            m_sControlNew[7].action[1] = '0';
        if(m_CntlUpdateCheck8.GetCheck() == 1)
            m_sControlNew[7].action[2] = '1';
        else 
            m_sControlNew[7].action[2] = '0';
        if(m_CntlViewCheck8.GetCheck() == 1)
            m_sControlNew[7].action[3] = '1';
        else 
            m_sControlNew[7].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[7].subsystem_name, m_StrLabel8, sizeof (m_sControlNew[7].subsystem_name));

        if(m_CntlAddCheck9.GetCheck() == 1)
            m_sControlNew[8].action[0] = '1';
        else 
            m_sControlNew[8].action[0] = '0';
        if(m_CntlDeleteCheck9.GetCheck() == 1)
            m_sControlNew[8].action[1] = '1';
        else 
            m_sControlNew[8].action[1] = '0';
        if(m_CntlUpdateCheck9.GetCheck() == 1)
            m_sControlNew[8].action[2] = '1';
        else 
            m_sControlNew[8].action[2] = '0';
        if(m_CntlViewCheck9.GetCheck() == 1)
            m_sControlNew[8].action[3] = '1';
        else 
            m_sControlNew[8].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[8].subsystem_name, m_StrLabel9, sizeof (m_sControlNew[8].subsystem_name));

        if(m_CntlAddCheck10.GetCheck() == 1)
            m_sControlNew[9].action[0] = '1';
        else 
            m_sControlNew[9].action[0] = '0';
        if(m_CntlDeleteCheck10.GetCheck() == 1)
            m_sControlNew[9].action[1] = '1';
        else 
            m_sControlNew[9].action[1] = '0';
        if(m_CntlUpdateCheck10.GetCheck() == 1)
            m_sControlNew[9].action[2] = '1';
        else 
            m_sControlNew[9].action[2] = '0';
        if(m_CntlViewCheck10.GetCheck() == 1)
            m_sControlNew[9].action[3] = '1';
        else 
            m_sControlNew[9].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[9].subsystem_name, m_StrLabel10,sizeof (m_sControlNew[9].subsystem_name));

        if(m_CntlAddCheck11.GetCheck() == 1)
            m_sControlNew[10].action[0] = '1';
        else 
            m_sControlNew[10].action[0] = '0';
        if(m_CntlDeleteCheck11.GetCheck() == 1)
            m_sControlNew[10].action[1] = '1';
        else 
            m_sControlNew[10].action[1] = '0';
        if(m_CntlUpdateCheck11.GetCheck() == 1)
            m_sControlNew[10].action[2] = '1';
        else 
            m_sControlNew[10].action[2] = '0';
        if(m_CntlViewCheck11.GetCheck() == 1)
            m_sControlNew[10].action[3] = '1';
        else 
            m_sControlNew[10].action[3] = '0';
		  if(m_CntlTlfExtract.GetCheck() == 1)
            m_sControlNew[10].action[4] = '1';
        else 
            m_sControlNew[10].action[4] = '0';
		memcpy ((LPTSTR)m_sControlNew[10].subsystem_name, m_StrLabel11,sizeof (m_sControlNew[10].subsystem_name));

/** recurring payment
        if(m_CntlAddCheck12.GetCheck() == 1)
            m_sControlNew[11].action[0] = '1';
        else 
            m_sControlNew[11].action[0] = '0';
        if(m_CntlDeleteCheck12.GetCheck() == 1)
            m_sControlNew[11].action[1] = '1';
        else 
            m_sControlNew[11].action[1] = '0';
        if(m_CntlUpdateCheck12.GetCheck() == 1)
            m_sControlNew[11].action[2] = '1';
        else 
            m_sControlNew[11].action[2] = '0';
        if(m_CntlViewCheck12.GetCheck() == 1)
            m_sControlNew[11].action[3] = '1';
        else 
            m_sControlNew[11].action[3] = '0';
  */

		// Override Merchant Denial	//The earlier controls used for recurring payment are reused
        if(m_CntlAddCheck12.GetCheck() == 1)
            m_sControlNew[14].action[0] = '1';
        else 
            m_sControlNew[14].action[0] = '0';
        if(m_CntlDeleteCheck12.GetCheck() == 1)
            m_sControlNew[14].action[1] = '1';
        else 
            m_sControlNew[14].action[1] = '0';
        if(m_CntlUpdateCheck12.GetCheck() == 1)
            m_sControlNew[14].action[2] = '1';
        else 
            m_sControlNew[14].action[2] = '0';
        if(m_CntlViewCheck12.GetCheck() == 1)
            m_sControlNew[14].action[3] = '1';
        else 
            m_sControlNew[14].action[3] = '0';
  		memcpy ((LPTSTR)m_sControlNew[14].subsystem_name, m_StrLabel12,sizeof (m_sControlNew[14].subsystem_name));

	/*	Dual Encryption
	if(m_CntlAddCheck13.GetCheck() == 1)
            m_sControlNew[11].action[0] = '1';
        else 
            m_sControlNew[11].action[0] = '0';
        if(m_CntlDeleteCheck13.GetCheck() == 1)
            m_sControlNew[11].action[1] = '1';
        else 
            m_sControlNew[11].action[1] = '0';
        if(m_CntlUpdateCheck13.GetCheck() == 1)
            m_sControlNew[11].action[2] = '1';
        else 
            m_sControlNew[11].action[2] = '0';
        if(m_CntlViewCheck13.GetCheck() == 1)
            m_sControlNew[11].action[3] = '1';
        else 
            m_sControlNew[11].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[11].subsystem_name, m_StrLabel13,sizeof (m_sControlNew[11].subsystem_name));
*/
		m_sControlNew[13].action[0] = '0';
		m_sControlNew[13].action[1] = '0';
		m_sControlNew[13].action[2] = '0';
        if(m_CntlViewCheck15.GetCheck() == 1)
            m_sControlNew[13].action[3] = '1';
        else 
            m_sControlNew[13].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[13].subsystem_name, m_StrLabel15,sizeof (m_sControlNew[13].subsystem_name));

			// ATP Archiving
        if(m_CntlAddCheck19.GetCheck() == 1)
            m_sControlNew[15].action[0] = '1';
        else 
            m_sControlNew[15].action[0] = '0';
        if(m_CntlDeleteCheck19.GetCheck() == 1)
            m_sControlNew[15].action[1] = '1';
        else 
            m_sControlNew[15].action[1] = '0';
        if(m_CntlUpdateCheck19.GetCheck() == 1)
            m_sControlNew[15].action[2] = '1';
        else 
            m_sControlNew[15].action[2] = '0';
        if(m_CntlViewCheck19.GetCheck() == 1)
            m_sControlNew[15].action[3] = '1';
        else 
            m_sControlNew[15].action[3] = '0';
  		memcpy ((LPTSTR)m_sControlNew[15].subsystem_name, m_StrLabel19,sizeof (m_sControlNew[15].subsystem_name));

		// system global parameter
		m_sControlNew[16].action[0] = '0';
		m_sControlNew[16].action[1] = '0';
		if(m_CntlUpdateCheck20.GetCheck() == 1)
            m_sControlNew[16].action[2] = '1';
        else 
            m_sControlNew[16].action[2] = '0';
        if(m_CntlViewCheck20.GetCheck() == 1)
            m_sControlNew[16].action[3] = '1';
        else 
            m_sControlNew[16].action[3] = '0';
  		memcpy ((LPTSTR)m_sControlNew[16].subsystem_name, m_StrLabel20,sizeof (m_sControlNew[16].subsystem_name));

		//USER STATUS CONTROL
		m_sControlNew[17].action[0] = '0';
		m_sControlNew[17].action[1] = '0';
       m_sControlNew[17].action[2] = '0';

	   if(m_CntlViewCheck21.GetCheck() == 1)
            m_sControlNew[17].action[3] = '1';
        else 
            m_sControlNew[17].action[3] = '0';
  		memcpy ((LPTSTR)m_sControlNew[17].subsystem_name, m_StrLabel21,sizeof (m_sControlNew[17].subsystem_name));

		// Fraud Control
        if(m_CntlAddCheck22.GetCheck() == 1)
            m_sControlNew[18].action[0] = '1';
        else 
            m_sControlNew[18].action[0] = '0';
        if(m_CntlDeleteCheck22.GetCheck() == 1)
            m_sControlNew[18].action[1] = '1';
        else 
            m_sControlNew[18].action[1] = '0';
        if(m_CntlUpdateCheck22.GetCheck() == 1)
            m_sControlNew[18].action[2] = '1';
        else 
            m_sControlNew[18].action[2] = '0';
        if(m_CntlViewCheck22.GetCheck() == 1)
            m_sControlNew[18].action[3] = '1';
        else 
            m_sControlNew[18].action[3] = '0';
  		memcpy ((LPTSTR)m_sControlNew[18].subsystem_name, m_StrLabel22,sizeof (m_sControlNew[15].subsystem_name));


		// CF Rules Definition
        if(m_CntlAddCheck23.GetCheck() == 1)
            m_sControlNew[19].action[0] = '1';
        else 
            m_sControlNew[19].action[0] = '0';
        if(m_CntlDeleteCheck23.GetCheck() == 1)
            m_sControlNew[19].action[1] = '1';
        else 
            m_sControlNew[19].action[1] = '0';
        if(m_CntlUpdateCheck23.GetCheck() == 1)
            m_sControlNew[19].action[2] = '1';
        else 
            m_sControlNew[19].action[2] = '0';
        if(m_CntlViewCheck23.GetCheck() == 1)
            m_sControlNew[19].action[3] = '1';
        else 
            m_sControlNew[19].action[3] = '0';
  		memcpy ((LPTSTR)m_sControlNew[19].subsystem_name, m_StrLabel23,sizeof (m_sControlNew[19].subsystem_name));

    return TRUE;
}



void CGroupAccess::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18]; 
    char strFieldTag [19];
    char temp[5];
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
    CString temp1;

    char strDate[9] = "";
    char strTime[7] = "";

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    
    myUtilsObject.GetSystemDate(strDate);  
    
    for (int i = 0; i < MAX_SUBSYSTEMS; i++)
    {
        int nResultCode = strcmp ((char *)m_sControlNew[i].action,(char *)m_sControl[i].action);
            //if (m_sControlNew[i].action[j] != m_sControl[i].action[j] )
        if ( nResultCode != 0 )
    
        {
            myUtilsObject.GetSystemTime(strTime);
            pintime_get_gmt ( dest_str );
            ZeroMemory ( &sHistoryData, sizeof (HST01));
            memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"UCF02", sizeof (sHistoryData.file_name));
            temp1 = strDate;
            temp1 = temp1.Right(4) + temp1.Left(2) + temp1.Mid(2,2);
    
			CHAR wstation[21]; DWORD wsize;

			wsize = sizeof( wstation );
			GetComputerName( wstation, &wsize );

			memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
            memcpy ((LPTSTR) sHistoryData.date_modified, temp1, sizeof (sHistoryData.date_modified));
            memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)m_StrGroupIDFld, sizeof (sHistoryData.file_primary_key));
            memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
            memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
            memcpy (strFieldTag, "Access Rights ", sizeof (strFieldTag));
            itoa ( i+1, temp, 10);
            strcat (strFieldTag, temp);
            memcpy ((LPTSTR) sHistoryData.field_tag,     (LPCTSTR)strFieldTag, sizeof (sHistoryData.field_tag));
            memcpy ((LPTSTR) sHistoryData.previous_data, (LPCTSTR)m_sControl[i].action, sizeof (sHistoryData.previous_data));  
            memcpy ((LPTSTR) sHistoryData.current_data, (LPCTSTR)m_sControlNew[i].action, sizeof (sHistoryData.current_data));                        
			memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));

        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
			if ( rcode != PTEMSG_OK )
			{
				m_StrMessageLbl = strErrorMsg;
				UpdateData( FALSE );
			}
            
        }
    }
}

BOOL CGroupAccess::PopulateDialogFromDatabase ()
{
	

	if ( m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_VIEW )
    {
		if ( m_nDialogMode == MODE_EDIT )
			EnableGroupAccessDialog(TRUE );
		else
			EnableGroupAccessDialog(FALSE );

    if (m_sControl[0].action[0] == '1')
        m_CntlAddCheck1.SetCheck(1);
    if (m_sControl[0].action[1] == '1')
        m_CntlDeleteCheck1.SetCheck(1);
    if (m_sControl[0].action[2] == '1')
        m_CntlUpdateCheck1.SetCheck(1);
    if (m_sControl[0].action[3] == '1')
        m_CntlViewCheck1.SetCheck(1);

    if (m_sControl[1].action[0] == '1')
        m_CntlAddCheck2.SetCheck(1);
    if (m_sControl[1].action[1] == '1')
        m_CntlDeleteCheck2.SetCheck(1);
    if (m_sControl[1].action[2] == '1')
        m_CntlUpdateCheck2.SetCheck(1);
    if (m_sControl[1].action[3] == '1')
        m_CntlViewCheck2.SetCheck(1);

    if (m_sControl[2].action[0] == '1')
        m_CntlAddCheck3.SetCheck(1);
    if (m_sControl[2].action[1] == '1')
        m_CntlDeleteCheck3.SetCheck(1);
    if (m_sControl[2].action[2] == '1')
        m_CntlUpdateCheck3.SetCheck(1);
    if (m_sControl[2].action[3] == '1')
        m_CntlViewCheck3.SetCheck(1);

    if (m_sControl[3].action[0] == '1')
        m_CntlAddCheck4.SetCheck(1);
    if (m_sControl[3].action[1] == '1')
        m_CntlDeleteCheck4.SetCheck(1);
    if (m_sControl[3].action[2] == '1')
        m_CntlUpdateCheck4.SetCheck(1);
    if (m_sControl[3].action[3] == '1')
        m_CntlViewCheck4.SetCheck(1);

    if (m_sControl[4].action[0] == '1')
        m_CntlAddCheck5.SetCheck(1);
    if (m_sControl[4].action[1] == '1')
        m_CntlDeleteCheck5.SetCheck(1);
    if (m_sControl[4].action[2] == '1')
        m_CntlUpdateCheck5.SetCheck(1);
    if (m_sControl[4].action[3] == '1')
        m_CntlViewCheck5.SetCheck(1);

    if (m_sControl[5].action[0] == '1')
        m_CntlAddCheck6.SetCheck(1);
    if (m_sControl[5].action[1] == '1')
        m_CntlDeleteCheck6.SetCheck(1);
    if (m_sControl[5].action[2] == '1')
        m_CntlUpdateCheck6.SetCheck(1);
    if (m_sControl[5].action[3] == '1')
        m_CntlViewCheck6.SetCheck(1);

     if (m_sControl[6].action[0] == '1')
        m_CntlAddCheck7.SetCheck(1);
    if (m_sControl[6].action[1] == '1')
        m_CntlDeleteCheck7.SetCheck(1);
    if (m_sControl[6].action[2] == '1')
        m_CntlUpdateCheck7.SetCheck(1);
    if (m_sControl[6].action[3] == '1')
        m_CntlViewCheck7.SetCheck(1);

     if (m_sControl[7].action[0] == '1')
        m_CntlAddCheck8.SetCheck(1);
    if (m_sControl[7].action[1] == '1')
        m_CntlDeleteCheck8.SetCheck(1);
    if (m_sControl[7].action[2] == '1')
        m_CntlUpdateCheck8.SetCheck(1);
    if (m_sControl[7].action[3] == '1')
        m_CntlViewCheck8.SetCheck(1);

     if (m_sControl[8].action[0] == '1')
        m_CntlAddCheck9.SetCheck(1);
    if (m_sControl[8].action[1] == '1')
        m_CntlDeleteCheck9.SetCheck(1);
    if (m_sControl[8].action[2] == '1')
        m_CntlUpdateCheck9.SetCheck(1);
    if (m_sControl[8].action[3] == '1')
        m_CntlViewCheck9.SetCheck(1);

     if (m_sControl[9].action[0] == '1')
        m_CntlAddCheck10.SetCheck(1);
    if (m_sControl[9].action[1] == '1')
        m_CntlDeleteCheck10.SetCheck(1);
    if (m_sControl[9].action[2] == '1')
        m_CntlUpdateCheck10.SetCheck(1);
    if (m_sControl[9].action[3] == '1')
        m_CntlViewCheck10.SetCheck(1);

    if (m_sControl[10].action[0] == '1')
        m_CntlAddCheck11.SetCheck(1);
    if (m_sControl[10].action[1] == '1')
        m_CntlDeleteCheck11.SetCheck(1);
    if (m_sControl[10].action[2] == '1')
        m_CntlUpdateCheck11.SetCheck(1);
    if (m_sControl[10].action[3] == '1')
        m_CntlViewCheck11.SetCheck(1);
	if(m_sControl[10].action[4] == '1')
		m_CntlTlfExtract.SetCheck(1);
/*
    if (m_sControl[11].action[0] == '1')
        m_CntlAddCheck12.SetCheck(1);
    if (m_sControl[11].action[1] == '1')
        m_CntlDeleteCheck12.SetCheck(1);
    if (m_sControl[11].action[2] == '1')
        m_CntlUpdateCheck12.SetCheck(1);
    if (m_sControl[11].action[3] == '1')
        m_CntlViewCheck12.SetCheck(1);
*/
	//Earlier recurring payment controls are being used for Override Merchant Control
    if (m_sControl[14].action[0] == '1')
        m_CntlAddCheck12.SetCheck(1);
    if (m_sControl[14].action[1] == '1')
        m_CntlDeleteCheck12.SetCheck(1);
    if (m_sControl[14].action[2] == '1')
        m_CntlUpdateCheck12.SetCheck(1);
    if (m_sControl[14].action[3] == '1')
        m_CntlViewCheck12.SetCheck(1);

    if (m_sControl[11].action[0] == '1')
        m_CntlAddCheck13.SetCheck(1);
    if (m_sControl[11].action[1] == '1')
        m_CntlDeleteCheck13.SetCheck(1);
    if (m_sControl[11].action[2] == '1')
        m_CntlUpdateCheck13.SetCheck(1);
    if (m_sControl[11].action[3] == '1')
        m_CntlViewCheck13.SetCheck(1);

    if (m_sControl[13].action[3] == '1')
        m_CntlViewCheck15.SetCheck(1);

	//atp archiving
   if (m_sControl[15].action[0] == '1')
        m_CntlAddCheck19.SetCheck(1);
    if (m_sControl[15].action[1] == '1')
        m_CntlDeleteCheck19.SetCheck(1);
    if (m_sControl[15].action[2] == '1')
        m_CntlUpdateCheck19.SetCheck(1);
    if (m_sControl[15].action[3] == '1')
        m_CntlViewCheck19.SetCheck(1);

	if (m_sControl[16].action[2] == '1')
        m_CntlUpdateCheck20.SetCheck(1);
    if (m_sControl[16].action[3] == '1')
        m_CntlViewCheck20.SetCheck(1);

	if (m_sControl[17].action[3] == '1')
        m_CntlViewCheck21.SetCheck(1);

					//Fraud Control
		if (m_sControl[18].action[0] == '1')
			m_CntlAddCheck22.SetCheck(1);
		if (m_sControl[18].action[1] == '1')
			m_CntlDeleteCheck22.SetCheck(1);
		if (m_sControl[18].action[2] == '1')
			m_CntlUpdateCheck22.SetCheck(1);
		if (m_sControl[18].action[3] == '1')
			m_CntlViewCheck22.SetCheck(1);

		
		//Chipfallback
		if (m_sControl[19].action[0] == '1')
			m_CntlAddCheck23.SetCheck(1);
		if (m_sControl[19].action[1] == '1')
			m_CntlDeleteCheck23.SetCheck(1);
		if (m_sControl[19].action[2] == '1')
			m_CntlUpdateCheck23.SetCheck(1);
		if (m_sControl[19].action[3] == '1')
			m_CntlViewCheck23.SetCheck(1);

	
}
    	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	SetModified ( FALSE );

	return TRUE;
}

void CGroupAccess::EnableGroupAccessDialog(int nStatus)
{
	m_CntlCheckAll.EnableWindow(nStatus);

	m_CntlUpdateCheck1.EnableWindow(nStatus);
	m_CntlUpdateCheck2.EnableWindow(nStatus);
	m_CntlUpdateCheck3.EnableWindow(nStatus);
	m_CntlUpdateCheck4.EnableWindow(nStatus);
	m_CntlUpdateCheck5.EnableWindow(nStatus);
	m_CntlUpdateCheck6.EnableWindow(nStatus);
	m_CntlUpdateCheck7.EnableWindow(nStatus);
	m_CntlUpdateCheck8.EnableWindow(nStatus);
	m_CntlUpdateCheck9.EnableWindow(nStatus);
	m_CntlUpdateCheck10.EnableWindow(nStatus);
	m_CntlUpdateCheck11.EnableWindow(nStatus);
	m_CntlUpdateCheck12.EnableWindow(nStatus);
	m_CntlUpdateCheck13.EnableWindow(FALSE);
	m_CntlUpdateCheck13.ShowWindow(FALSE);
	m_CntlUpdateCheck19.EnableWindow(nStatus);
	m_CntlUpdateCheck20.EnableWindow(nStatus);
	m_CntlViewCheck21.EnableWindow(nStatus);
	m_CntlUpdateCheck22.EnableWindow(nStatus);
	m_CntlUpdateCheck23.EnableWindow(nStatus);

	m_CntlAddCheck1.EnableWindow(nStatus);
	m_CntlAddCheck2.EnableWindow(nStatus);
	m_CntlAddCheck3.EnableWindow(nStatus);
	m_CntlAddCheck4.EnableWindow(nStatus);
	m_CntlAddCheck5.EnableWindow(nStatus);
	m_CntlAddCheck6.EnableWindow(nStatus);
	m_CntlAddCheck7.EnableWindow(nStatus);
	m_CntlAddCheck8.EnableWindow(nStatus);
	m_CntlAddCheck9.EnableWindow(nStatus);
	m_CntlAddCheck10.EnableWindow(nStatus);
	m_CntlAddCheck11.EnableWindow(nStatus);
	m_CntlAddCheck12.EnableWindow(nStatus);
	m_CntlAddCheck13.EnableWindow(FALSE);
	m_CntlAddCheck13.ShowWindow(FALSE);
	m_CntlAddCheck19.EnableWindow(nStatus);
	m_CntlAddCheck22.EnableWindow(nStatus);
	m_CntlAddCheck23.EnableWindow(nStatus);

	m_CntlDeleteCheck1.EnableWindow(nStatus);
	m_CntlDeleteCheck2.EnableWindow(nStatus);
	m_CntlDeleteCheck3.EnableWindow(nStatus);
	m_CntlDeleteCheck4.EnableWindow(nStatus);
	m_CntlDeleteCheck5.EnableWindow(nStatus);
	m_CntlDeleteCheck6.EnableWindow(nStatus);
	m_CntlDeleteCheck7.EnableWindow(nStatus);
	m_CntlDeleteCheck8.EnableWindow(nStatus);
	m_CntlDeleteCheck9.EnableWindow(nStatus);
	m_CntlDeleteCheck10.EnableWindow(nStatus);
	m_CntlDeleteCheck11.EnableWindow(nStatus);
	m_CntlDeleteCheck12.EnableWindow(nStatus);
	m_CntlDeleteCheck13.EnableWindow(FALSE);
	m_CntlDeleteCheck13.ShowWindow(FALSE);
	m_CntlDeleteCheck19.EnableWindow(nStatus);
	m_CntlDeleteCheck22.EnableWindow(nStatus);
	m_CntlDeleteCheck23.EnableWindow(nStatus);

	m_CntlViewCheck1.EnableWindow(nStatus);
	m_CntlViewCheck2.EnableWindow(nStatus);
	m_CntlViewCheck3.EnableWindow(nStatus);
	m_CntlViewCheck4.EnableWindow(nStatus);
	m_CntlViewCheck5.EnableWindow(nStatus);
	m_CntlViewCheck6.EnableWindow(nStatus);
	m_CntlViewCheck7.EnableWindow(nStatus);
	m_CntlViewCheck8.EnableWindow(nStatus);
	m_CntlViewCheck9.EnableWindow(nStatus);
	m_CntlViewCheck10.EnableWindow(nStatus);
	m_CntlViewCheck11.EnableWindow(nStatus);
	m_CntlViewCheck12.EnableWindow(nStatus);
	m_CntlViewCheck13.EnableWindow(FALSE);
	m_CntlViewCheck13.ShowWindow(FALSE);
	m_CntlViewCheck15.EnableWindow(nStatus);
	m_CntlViewCheck19.EnableWindow(nStatus);
	m_CntlViewCheck20.EnableWindow(nStatus);
	m_CntlViewCheck22.EnableWindow(nStatus);
	m_CntlViewCheck23.EnableWindow(nStatus);

	m_CntlTlfExtract.EnableWindow(nStatus);
}

void CGroupAccess::CleanGroupAccessDialog()
{
	m_CntlUpdateCheck1.SetCheck(0);
	m_CntlUpdateCheck2.SetCheck(0);
	m_CntlUpdateCheck3.SetCheck(0);
	m_CntlUpdateCheck4.SetCheck(0);
	m_CntlUpdateCheck5.SetCheck(0);
	m_CntlUpdateCheck6.SetCheck(0);
	m_CntlUpdateCheck7.SetCheck(0);
	m_CntlUpdateCheck8.SetCheck(0);
	m_CntlUpdateCheck9.SetCheck(0);
	m_CntlUpdateCheck10.SetCheck(0);
	m_CntlUpdateCheck11.SetCheck(0);
	m_CntlUpdateCheck12.SetCheck(0);
	m_CntlUpdateCheck13.SetCheck(0);
	m_CntlUpdateCheck19.SetCheck(0);
	m_CntlUpdateCheck20.SetCheck(0);
	m_CntlViewCheck21.SetCheck(0);
	m_CntlViewCheck22.SetCheck(0);
	m_CntlViewCheck23.SetCheck(0);

	m_CntlAddCheck1.SetCheck(0);
	m_CntlAddCheck2.SetCheck(0);
	m_CntlAddCheck3.SetCheck(0);
	m_CntlAddCheck4.SetCheck(0);
	m_CntlAddCheck5.SetCheck(0);
	m_CntlAddCheck6.SetCheck(0);
	m_CntlAddCheck7.SetCheck(0);
	m_CntlAddCheck8.SetCheck(0);
	m_CntlAddCheck9.SetCheck(0);
	m_CntlAddCheck10.SetCheck(0);
	m_CntlAddCheck11.SetCheck(0);
	m_CntlAddCheck12.SetCheck(0);
	m_CntlAddCheck13.SetCheck(0);
	m_CntlAddCheck19.SetCheck(0);
	m_CntlAddCheck22.SetCheck(0);
	m_CntlAddCheck23.SetCheck(0);

	m_CntlDeleteCheck1.SetCheck(0);
	m_CntlDeleteCheck2.SetCheck(0);
	m_CntlDeleteCheck3.SetCheck(0);
	m_CntlDeleteCheck4.SetCheck(0);
	m_CntlDeleteCheck5.SetCheck(0);
	m_CntlDeleteCheck6.SetCheck(0);
	m_CntlDeleteCheck7.SetCheck(0);
	m_CntlDeleteCheck8.SetCheck(0);
	m_CntlDeleteCheck9.SetCheck(0);
	m_CntlDeleteCheck10.SetCheck(0);
	m_CntlDeleteCheck11.SetCheck(0);
	m_CntlDeleteCheck12.SetCheck(0);
	m_CntlDeleteCheck13.SetCheck(0);
	m_CntlDeleteCheck19.SetCheck(0);
	m_CntlDeleteCheck22.SetCheck(0);
	m_CntlDeleteCheck23.SetCheck(0);

	m_CntlViewCheck1.SetCheck(0);
	m_CntlViewCheck2.SetCheck(0);
	m_CntlViewCheck3.SetCheck(0);
	m_CntlViewCheck4.SetCheck(0);
	m_CntlViewCheck5.SetCheck(0);
	m_CntlViewCheck6.SetCheck(0);
	m_CntlViewCheck7.SetCheck(0);
	m_CntlViewCheck8.SetCheck(0);
	m_CntlViewCheck9.SetCheck(0);
	m_CntlViewCheck10.SetCheck(0);
	m_CntlViewCheck11.SetCheck(0);
	m_CntlViewCheck12.SetCheck(0);
	m_CntlViewCheck13.SetCheck(0);
	m_CntlViewCheck15.SetCheck(0);

	m_CntlViewCheck19.SetCheck(0);
	m_CntlViewCheck20.SetCheck(0);
	m_CntlViewCheck22.SetCheck(0);
	m_CntlViewCheck23.SetCheck(0);

	m_CntlTlfExtract.SetCheck(0);
	m_StrGroupIDFld = "";
	m_StrMessageLbl = "";
	 
	UpdateData(FALSE);
}

BOOL CGroupAccess::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW)
		SetModified ( TRUE );
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}


BOOL CGroupAccess::OnHelpInfo(HELPINFO* pHelpInfo) 
{
/*	HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
		"security.chm>Main",HH_HELP_CONTEXT,IDD_GROUP_DEFINITION);	
*/	
	return TRUE;
}


BOOL CGroupAccess::OnSetActive() 
{
	((CGroupPropertySheet*) this->GetParent())->m_nActiveTab = 1;
	
	return CPropertyPage::OnSetActive();
}

void CGroupAccess::OnCheckAll() 
{
	int check = m_CntlCheckAll.GetCheck();
	m_CntlUpdateCheck1.SetCheck(check);
	m_CntlUpdateCheck2.SetCheck(check);
	m_CntlUpdateCheck3.SetCheck(check);
	m_CntlUpdateCheck4.SetCheck(check);
	m_CntlUpdateCheck5.SetCheck(check);
	m_CntlUpdateCheck6.SetCheck(check);
	m_CntlUpdateCheck7.SetCheck(check);
	m_CntlUpdateCheck8.SetCheck(check);
	m_CntlUpdateCheck9.SetCheck(check);
	m_CntlUpdateCheck10.SetCheck(check);
	m_CntlUpdateCheck11.SetCheck(check);
	m_CntlUpdateCheck12.SetCheck(check);
	m_CntlUpdateCheck13.SetCheck(check);
	m_CntlUpdateCheck19.SetCheck(check);
	m_CntlUpdateCheck20.SetCheck(check);
	m_CntlUpdateCheck22.SetCheck(check);
	m_CntlUpdateCheck23.SetCheck(check);
	m_CntlViewCheck21.SetCheck(check);
	m_CntlViewCheck22.SetCheck(check);
	m_CntlViewCheck23.SetCheck(check);

	m_CntlAddCheck1.SetCheck(check);
	m_CntlAddCheck2.SetCheck(check);
	m_CntlAddCheck3.SetCheck(check);
	m_CntlAddCheck4.SetCheck(check);
	m_CntlAddCheck5.SetCheck(check);
	m_CntlAddCheck6.SetCheck(check);
	m_CntlAddCheck7.SetCheck(check);
	m_CntlAddCheck8.SetCheck(check);
	m_CntlAddCheck9.SetCheck(check);
	m_CntlAddCheck10.SetCheck(check);
	m_CntlAddCheck11.SetCheck(check);
	m_CntlAddCheck12.SetCheck(check);
	m_CntlAddCheck13.SetCheck(check);
	m_CntlAddCheck19.SetCheck(check);
	m_CntlAddCheck22.SetCheck(check);
	m_CntlAddCheck23.SetCheck(check);

	m_CntlDeleteCheck1.SetCheck(check);
	m_CntlDeleteCheck2.SetCheck(check);
	m_CntlDeleteCheck3.SetCheck(check);
	m_CntlDeleteCheck4.SetCheck(check);
	m_CntlDeleteCheck5.SetCheck(check);
	m_CntlDeleteCheck6.SetCheck(check);
	m_CntlDeleteCheck7.SetCheck(check);
	m_CntlDeleteCheck8.SetCheck(check);
	m_CntlDeleteCheck9.SetCheck(check);
	m_CntlDeleteCheck10.SetCheck(check);
	m_CntlDeleteCheck11.SetCheck(check);
	m_CntlDeleteCheck12.SetCheck(check);
	m_CntlDeleteCheck13.SetCheck(check);
	m_CntlDeleteCheck19.SetCheck(check);
	m_CntlDeleteCheck22.SetCheck(check);
	m_CntlDeleteCheck23.SetCheck(check);

	m_CntlViewCheck1.SetCheck(check);
	m_CntlViewCheck2.SetCheck(check);
	m_CntlViewCheck3.SetCheck(check);
	m_CntlViewCheck4.SetCheck(check);
	m_CntlViewCheck5.SetCheck(check);
	m_CntlViewCheck6.SetCheck(check);
	m_CntlViewCheck7.SetCheck(check);
	m_CntlViewCheck8.SetCheck(check);
	m_CntlViewCheck9.SetCheck(check);
	m_CntlViewCheck10.SetCheck(check);
	m_CntlViewCheck11.SetCheck(check);
	m_CntlViewCheck12.SetCheck(check);
	m_CntlViewCheck13.SetCheck(check);
	m_CntlViewCheck15.SetCheck(check);
	m_CntlViewCheck19.SetCheck(check);
	m_CntlViewCheck20.SetCheck(check);
	m_CntlViewCheck22.SetCheck(check);
	m_CntlViewCheck23.SetCheck(check);

	m_CntlTlfExtract.SetCheck(check);
}




