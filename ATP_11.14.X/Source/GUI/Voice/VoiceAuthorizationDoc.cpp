// VoiceAuthorizationDoc.cpp : implementation of the CVoiceAuthorizationDoc class
//

#include "stdafx.h"
#include "VoiceAuthorization.h"

#include "VoiceAuthorizationDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationDoc

IMPLEMENT_DYNCREATE(CVoiceAuthorizationDoc, CDocument)

BEGIN_MESSAGE_MAP(CVoiceAuthorizationDoc, CDocument)
	//{{AFX_MSG_MAP(CVoiceAuthorizationDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationDoc construction/destruction

CVoiceAuthorizationDoc::CVoiceAuthorizationDoc()
{
	// TODO: add one-time construction code here

}

CVoiceAuthorizationDoc::~CVoiceAuthorizationDoc()
{
}

BOOL CVoiceAuthorizationDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationDoc serialization

void CVoiceAuthorizationDoc::Serialize(CArchive& ar)
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
// CVoiceAuthorizationDoc diagnostics

#ifdef _DEBUG
void CVoiceAuthorizationDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVoiceAuthorizationDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationDoc commands


