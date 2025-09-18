/****************************************************************************************

   Module: srvcmonDoc.cpp

   Title: Pinnacle Services Monitor

   $Log:   N:\pvcs\PTE\CORE\Srvcmgr2\Gui\srvcmonDoc.cpp  $
   
      Rev 1.0   27 May 1998 16:54:26   rmalathk
   Initial Revision
   
*****************************************************************************************/
// srvcmonDoc.cpp : implementation of the CSrvcmonDoc class
//

#include "stdafx.h"
#include "srvcmon.h"

#include "srvcmonDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonDoc

IMPLEMENT_DYNCREATE(CSrvcmonDoc, CDocument)

BEGIN_MESSAGE_MAP(CSrvcmonDoc, CDocument)
	//{{AFX_MSG_MAP(CSrvcmonDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonDoc construction/destruction

CSrvcmonDoc::CSrvcmonDoc()
{
	// TODO: add one-time construction code here

}

CSrvcmonDoc::~CSrvcmonDoc()
{
}

BOOL CSrvcmonDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonDoc serialization

void CSrvcmonDoc::Serialize(CArchive& ar)
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
// CSrvcmonDoc diagnostics

#ifdef _DEBUG
void CSrvcmonDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSrvcmonDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSrvcmonDoc commands
