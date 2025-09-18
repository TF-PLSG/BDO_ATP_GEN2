// UserStatusControlDoc.cpp : implementation of the CUserStatusControlDoc class
//

#include "stdafx.h"
#include "UserStatusControl.h"

#include "UserStatusControlDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserStatusControlDoc

IMPLEMENT_DYNCREATE(CUserStatusControlDoc, CDocument)

BEGIN_MESSAGE_MAP(CUserStatusControlDoc, CDocument)
	//{{AFX_MSG_MAP(CUserStatusControlDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserStatusControlDoc construction/destruction

CUserStatusControlDoc::CUserStatusControlDoc()
{
	// TODO: add one-time construction code here

}

CUserStatusControlDoc::~CUserStatusControlDoc()
{
}

BOOL CUserStatusControlDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CUserStatusControlDoc serialization

void CUserStatusControlDoc::Serialize(CArchive& ar)
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
// CUserStatusControlDoc diagnostics

#ifdef _DEBUG
void CUserStatusControlDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUserStatusControlDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUserStatusControlDoc commands
