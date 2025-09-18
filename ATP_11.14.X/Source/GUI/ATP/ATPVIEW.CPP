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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ATPVIEW.CPP  $
   
      Rev 1.9   Dec 01 2003 11:28:38   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   May 01 2002 16:08:54   jgrguric
   Added new configuration logic for CONNECTMODE
   parameter to allow multiple or single connections.
   
      Rev 1.7   Jan 05 2000 14:52:38   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:40:36   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:44   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:48   iarustam
   Initial Release
   
      Rev 1.4   May 10 1999 10:56:22   dperson
   Standardized error messages
   
      Rev 1.3   Apr 29 1999 14:55:38   iarustam
   Bug fixes
   
      Rev 1.2   Mar 04 1999 13:31:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:50   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:54   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:14   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.2   Jan 22 1999 09:51:16   iarustam
   Changes for core 1_3_0
   
   
      Rev 1.1   Jan 21 1999 13:05:26   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:18:52   iarustam
    
   
      Rev 1.1   Dec 18 1998 13:41:32   iarustam
    
*
************************************************************************************/

// AtpView.cpp : implementation of the CAtpView class
//

#include "stdafx.h"
#include "Atp.h"

#include "AtpDoc.h"
#include "AtpView.h"



extern "C"
{
   #include "ptetcp.h"
   #include "ntutils.h"
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern UCF01 ucf01;

//Added 5/1/02 by JMG
extern CHAR connect_str[];

/////////////////////////////////////////////////////////////////////////////
// CAtpView

IMPLEMENT_DYNCREATE(CAtpView, CScrollView)

BEGIN_MESSAGE_MAP(CAtpView, CScrollView)
	//{{AFX_MSG_MAP(CAtpView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAtpView construction/destruction

CAtpView::CAtpView()
{
	m_pdcDisplayMemory = new CDC;
    m_pBitmap = new CBitmap;

}

CAtpView::~CAtpView()
{
   // Following disconnect logic by JMG on 4/26/02 as we can now connect and disconnect
   // on every operation
   if ( 0 == strcmp("SINGLE", connect_str) )
     // Configured for SINGLE applnk connection at start up
	 // Next line added by JMG on 6/14/01 to disconnect applnk connection
	 // before exiting this GUI. Will prevent applnk & system problems.
	 ptetcp_disconnect();

    delete m_pdcDisplayMemory;
    delete m_pBitmap;
//AppCleanUp();
  /*  if (nConnectionType == _XIPC)  {
	
		m_pApplicationObject->m_RemoteLoginObject.CloseGuiApp() ;
	}
	else if (nConnectionType == _TCPIP)
		DeletePteToTcpObject() ;*/
}

BOOL CAtpView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAtpView drawing

void CAtpView::OnDraw(CDC* pDC)
{
	CAtpDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

    pDC->StretchBlt (300, -300, m_sizeDest.cx, -m_sizeDest.cy, m_pdcDisplayMemory, 0, 0,
                     m_sizeSource.cx, m_sizeSource.cy, SRCCOPY);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CAtpView diagnostics

#ifdef _DEBUG
void CAtpView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CAtpView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CAtpDoc* CAtpView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAtpDoc)));
	return (CAtpDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAtpView message handlers
//***************************************************************
//***************************************************************
struct menu_struct* CAtpView::find_menu_table( pCHAR menu_item )
{
    INT i;

    // loop through the menu table and look for the item
    for( i = 0; i < N_MENUENTRY; i++ )
    {
        if( strcmp( menu_table[i].menu_str, menu_item ) == 0 )
            return( &menu_table[i] );
    }

    // if we reached here, menu item was not found
    return NULL_PTR;

}
	

void CAtpView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	

   LogEvent( "in OnInitialUpdate", INFO_MSG );
   UCF01 temp_ucf01 = {0};
   

	CSize totalSize( 800, 800);
    CSize lineSize = CSize (totalSize.cx/100, totalSize.cy/100);
    SetScrollSizes (MM_LOENGLISH, totalSize, totalSize, lineSize);

    BITMAP bm;
    if (m_pdcDisplayMemory->GetSafeHdc() == NULL ) {
        CClientDC dc(this);
        OnPrepareDC(&dc);
        m_pBitmap->LoadBitmap(IDB_BITMAP2);
        m_pdcDisplayMemory->CreateCompatibleDC(&dc);
        m_pdcDisplayMemory->SelectObject(m_pBitmap);
        m_pBitmap->GetObject(sizeof(bm), &bm);
        m_sizeSource.cx = bm.bmWidth;
        m_sizeSource.cy = bm.bmHeight;
        m_sizeDest = m_sizeSource;
        dc.DPtoLP(&m_sizeDest);
    }

	

	
   LogEvent( "bitmap done", INFO_MSG );

   // set the menu items for the user
      // set the menu items for the user
   menu_struct *menu_data; INT loop;
   CMenu mnu;
   CFrameWnd *pWnd = GetParentFrame();
   CMenu *tmp_menu = pWnd->GetMenu();
   mnu.Attach( tmp_menu->m_hMenu );
   INT ram;
   ATP01 sATPStructure;
   char strErrorMessage[200] = "";
	ZeroMemory(&sATPStructure,sizeof(ATP01)) ;

  // memcpy ( sATPStructure.site_name, "ATP SigCapture Concord EFS", sizeof (sATPStructure.site_name));

	int rcode = txdsapi_get_record  ((pBYTE)&sATPStructure,sizeof (ATP01), ATP01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{   
		AfxMessageBox(strErrorMessage);
		return;
    }
	else if ( rcode == PTEMSG_INVALID_DATATYPE )
	{
		AfxMessageBox("Please try again");
		return;	
	}
	memcpy(&temp_ucf01,&ucf01,sizeof(ucf01));

	strncpy((char *)temp_ucf01.control[12].subsystem_name,(char *)ucf01.control[11].subsystem_name,sizeof(ucf01.control[11].subsystem_name));
	strncpy((char *)temp_ucf01.control[12].action,(char *)ucf01.control[11].action,sizeof(ucf01.control[11].action));

   for( loop = N_MENUENTRY; loop >= 0; loop-- )
   {
      if( temp_ucf01.control[loop].action[VIEW_BYTE] != '1' )
      {
		  CString str;
         //menu_data = find_menu_table( (pCHAR)ucf01.control[loop].subsystem_name ); 
         menu_data = find_menu_table( (pCHAR)sATPStructure.system_components[loop].subsystem_name ); 
         if( menu_data != NULL_PTR )
		 {
			 mnu.GetMenuString(menu_data->position, str, MF_BYPOSITION);
            ram = mnu.DeleteMenu( menu_data->position, MF_BYPOSITION );
		 }
      }

   } //for

   LogEvent( "menu initialized", INFO_MSG );
   mnu.Detach();
   LogEvent( "menu detached", INFO_MSG );

}

BOOLEAN CAtpView::AppCleanUp()
{
	TRACE ("In AppCleanUp\n") ;
	// Terminate the data processing thread
	//TerminateThread (m_hGetTransactionsThreadHandle, 0) ;
    //TRACE ("AppCleanUp->Before Waiting for m_hGetTransactionsThreadHandle to die\n") ;
	//WaitForSingleObject(m_hGetTransactionsThreadHandle, INFINITE) ;
	//TRACE ("Main Thread is now dead\n") ;
	
    // APPLINK
/*	if (nConnectionType == _XIPC)  {
		// Log the data processing thread out of XIPC 
//		XipcAbort (m_ThreadXipcUid) ;
		// Destroy the interactive que and log out of the pte_instance
		//TRACE ("About to call CloseGuiApp\n") ;
		m_pApplicationObject->m_RemoteLoginObject.CloseGuiApp() ;
	}
	else if (nConnectionType == _TCPIP)
		DeletePteToTcpObject() ;*/
    // APPLINK

	// Following disconnect logic by JMG on 4/26/02 as we can now connect and disconnect
   // on every operation
   if ( 0 == strcmp("SINGLE", connect_str) )
     // Configured for SINGLE applnk connection at start up
	 // Next line added by JMG on 6/14/01 to disconnect applnk connection
	 // before exiting this GUI. Will prevent applnk & system problems.
	 ptetcp_disconnect();

	return TRUE ;
}
