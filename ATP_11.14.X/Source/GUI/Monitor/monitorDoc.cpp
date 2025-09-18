/*******************************************************************************
*  
* MODULE:      monitordoc.cpp
*  
* TITLE:       
*  
* DESCRIPTION:
*
* APPLICATION: Ascendent System Monitor
*
* AUTHOR:  Ram Malathkar
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\monitor\files\monitorDoc.cpp  $
   
      Rev 1.3   Apr 04 2005 15:58:48   lmitchel
   Rebuild 4_4_1
   
      Rev 1.2   Aug 29 2000 15:24:38   lmitchel
   New implementation of monitor including new functions for displaying device and network controller statistics and dialog status messages.  Two windows added to the existing monitor to display device and network transaction counts (sent, received, tps rate) and dialog status (connected, disconnected).  The new windows are updated with current data every nn seconds defined by the interval in the monitor.ini file.  Functions added to send and recieved stats and status updates form the controllers.
   
      Rev 1.1   Nov 24 1999 14:32:02   lmitchel
   Recompile w/new libraries ver 3_2_0/core1_3_1_3
   
      Rev 1.0   24 Feb 1999 15:59:16   rmalathk
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:33:38   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:00:12   MSALEH
   initial release
   
      Rev 1.0   28 Oct 1998 16:31:16   jhapitas
   Initial checkin
*
*******************************************************************************/
// monitorDoc.cpp : implementation of the CMonitorDoc class
//

#include "stdafx.h"
#include "monitor.h"

#include "monitorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonitorDoc

IMPLEMENT_DYNCREATE(CMonitorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMonitorDoc, CDocument)
	//{{AFX_MSG_MAP(CMonitorDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonitorDoc construction/destruction

CMonitorDoc::CMonitorDoc()
{
	// TODO: add one-time construction code here

}

CMonitorDoc::~CMonitorDoc()
{
}

BOOL CMonitorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorDoc serialization

void CMonitorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMonitorDoc diagnostics

#ifdef _DEBUG
void CMonitorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMonitorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMonitorDoc commands
