// UserStatusControlView.cpp : implementation of the CUserStatusControlView class
//

#include "stdafx.h"
#include "UserStatusControl.h"

#include "UserStatusControlDoc.h"
#include "UserStatusControlView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserStatusControlView

IMPLEMENT_DYNCREATE(CUserStatusControlView, CView)

BEGIN_MESSAGE_MAP(CUserStatusControlView, CView)
	//{{AFX_MSG_MAP(CUserStatusControlView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserStatusControlView construction/destruction

CUserStatusControlView::CUserStatusControlView()
{
	// TODO: add construction code here

}

CUserStatusControlView::~CUserStatusControlView()
{
}

BOOL CUserStatusControlView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CUserStatusControlView drawing

void CUserStatusControlView::OnDraw(CDC* pDC)
{
	CUserStatusControlDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CUserStatusControlView diagnostics

#ifdef _DEBUG
void CUserStatusControlView::AssertValid() const
{
	CView::AssertValid();
}

void CUserStatusControlView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CUserStatusControlDoc* CUserStatusControlView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUserStatusControlDoc)));
	return (CUserStatusControlDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUserStatusControlView message handlers
