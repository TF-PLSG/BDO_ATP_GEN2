// TranPosSearchView.h : interface of the CTranPosSearchView class
//
/////////////////////////////////////////////////////////////////////////////

class CTranPosSearchView : public CView
{
protected: // create from serialization only
	CTranPosSearchView();
	DECLARE_DYNCREATE(CTranPosSearchView)

// Attributes
public:
	CTranPosSearchDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTranPosSearchView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTranPosSearchView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTranPosSearchView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TranPosSearchView.cpp
inline CTranPosSearchDoc* CTranPosSearchView::GetDocument()
   { return (CTranPosSearchDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
