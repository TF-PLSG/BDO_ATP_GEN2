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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ATPDOC.CPP  $
   
      Rev 1.5   Dec 01 2003 11:28:38   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:52:38   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:40:26   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:44   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:48   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:38   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:50   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:52   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:08   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 22 1999 09:51:14   iarustam
   Changes for core 1_3_0
   
   
      Rev 1.1   Dec 18 1998 13:40:44   iarustam
    
*
************************************************************************************/

// AtpDoc.cpp : implementation of the CAtpDoc class
//

#include "stdafx.h"
#include "Atp.h"

#include "AtpDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAtpDoc

IMPLEMENT_DYNCREATE(CAtpDoc, CDocument)

BEGIN_MESSAGE_MAP(CAtpDoc, CDocument)
	//{{AFX_MSG_MAP(CAtpDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAtpDoc construction/destruction

CAtpDoc::CAtpDoc()
{
	// TODO: add one-time construction code here

}

CAtpDoc::~CAtpDoc()
{
}

BOOL CAtpDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAtpDoc serialization

void CAtpDoc::Serialize(CArchive& ar)
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
// CAtpDoc diagnostics

#ifdef _DEBUG
void CAtpDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAtpDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAtpDoc commands
