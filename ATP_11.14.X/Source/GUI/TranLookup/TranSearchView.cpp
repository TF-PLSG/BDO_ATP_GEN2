// TranSearchView.cpp : implementation of the CTranSearchView class
//

#include "stdafx.h"
#include "TranSearch.h"

#include "TranSearchDoc.h"
#include "TranSearchView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTranSearchView

IMPLEMENT_DYNCREATE(CTranSearchView, CView)

BEGIN_MESSAGE_MAP(CTranSearchView, CView)
	//{{AFX_MSG_MAP(CTranSearchView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTranSearchView construction/destruction

CTranSearchView::CTranSearchView()
{
	// TODO: add construction code here

}

CTranSearchView::~CTranSearchView()
{
}

BOOL CTranSearchView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTranSearchView drawing

void CTranSearchView::OnDraw(CDC* pDC)
{
	CTranSearchDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CTranSearchView diagnostics

#ifdef _DEBUG
void CTranSearchView::AssertValid() const
{
	CView::AssertValid();
}

void CTranSearchView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTranSearchDoc* CTranSearchView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTranSearchDoc)));
	return (CTranSearchDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTranSearchView message handlers
