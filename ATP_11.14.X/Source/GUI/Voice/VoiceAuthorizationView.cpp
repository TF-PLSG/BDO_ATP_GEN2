// VoiceAuthorizationView.cpp : implementation of the CVoiceAuthorizationView class
//

#include "stdafx.h"
#include "VoiceAuthorization.h"

#include "VoiceAuthorizationDoc.h"
#include "VoiceAuthorizationView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationView

IMPLEMENT_DYNCREATE(CVoiceAuthorizationView, CView)

BEGIN_MESSAGE_MAP(CVoiceAuthorizationView, CView)
	//{{AFX_MSG_MAP(CVoiceAuthorizationView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationView construction/destruction

CVoiceAuthorizationView::CVoiceAuthorizationView()
{
	// TODO: add construction code here

}

CVoiceAuthorizationView::~CVoiceAuthorizationView()
{
}

BOOL CVoiceAuthorizationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationView drawing

void CVoiceAuthorizationView::OnDraw(CDC* pDC)
{
	CVoiceAuthorizationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationView diagnostics

#ifdef _DEBUG
void CVoiceAuthorizationView::AssertValid() const
{
	CView::AssertValid();
}

void CVoiceAuthorizationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVoiceAuthorizationDoc* CVoiceAuthorizationView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVoiceAuthorizationDoc)));
	return (CVoiceAuthorizationDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationView message handlers
