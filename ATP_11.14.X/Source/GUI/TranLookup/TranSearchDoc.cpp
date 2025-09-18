// TranSearchDoc.cpp : implementation of the CTranSearchDoc class
//

#include "stdafx.h"
#include "TranSearch.h"

#include "TranSearchDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTranSearchDoc

IMPLEMENT_DYNCREATE(CTranSearchDoc, CDocument)

BEGIN_MESSAGE_MAP(CTranSearchDoc, CDocument)
	//{{AFX_MSG_MAP(CTranSearchDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTranSearchDoc construction/destruction

CTranSearchDoc::CTranSearchDoc()
{
	// TODO: add one-time construction code here

}

CTranSearchDoc::~CTranSearchDoc()
{
}

BOOL CTranSearchDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTranSearchDoc serialization

void CTranSearchDoc::Serialize(CArchive& ar)
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
// CTranSearchDoc diagnostics

#ifdef _DEBUG
void CTranSearchDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTranSearchDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTranSearchDoc commands
