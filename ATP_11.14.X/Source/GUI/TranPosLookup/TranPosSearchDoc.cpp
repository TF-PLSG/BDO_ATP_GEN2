// TranPosSearchDoc.cpp : implementation of the CTranPosSearchDoc class
//

#include "stdafx.h"
#include "TranPosSearch.h"

#include "TranPosSearchDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTranPosSearchDoc

IMPLEMENT_DYNCREATE(CTranPosSearchDoc, CDocument)

BEGIN_MESSAGE_MAP(CTranPosSearchDoc, CDocument)
	//{{AFX_MSG_MAP(CTranPosSearchDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTranPosSearchDoc construction/destruction

CTranPosSearchDoc::CTranPosSearchDoc()
{
	// TODO: add one-time construction code here

}

CTranPosSearchDoc::~CTranPosSearchDoc()
{
}

BOOL CTranPosSearchDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTranPosSearchDoc serialization

void CTranPosSearchDoc::Serialize(CArchive& ar)
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
// CTranPosSearchDoc diagnostics

#ifdef _DEBUG
void CTranPosSearchDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTranPosSearchDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTranPosSearchDoc commands
