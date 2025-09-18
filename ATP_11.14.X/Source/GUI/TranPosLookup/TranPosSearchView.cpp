// TranPosSearchView.cpp : implementation of the CTranPosSearchView class
//

#include "stdafx.h"
#include "TranPosSearch.h"

#include "TranPosSearchDoc.h"
#include "TranPosSearchView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTranPosSearchView

IMPLEMENT_DYNCREATE(CTranPosSearchView, CView)

BEGIN_MESSAGE_MAP(CTranPosSearchView, CView)
	//{{AFX_MSG_MAP(CTranPosSearchView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTranPosSearchView construction/destruction

CTranPosSearchView::CTranPosSearchView()
{
	// TODO: add construction code here

}

CTranPosSearchView::~CTranPosSearchView()
{
}

BOOL CTranPosSearchView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTranPosSearchView drawing

void CTranPosSearchView::OnDraw(CDC* pDC)
{
	CTranPosSearchDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CTranPosSearchView diagnostics

#ifdef _DEBUG
void CTranPosSearchView::AssertValid() const
{
	CView::AssertValid();
}

void CTranPosSearchView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTranPosSearchDoc* CTranPosSearchView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTranPosSearchDoc)));
	return (CTranPosSearchDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTranPosSearchView message handlers
