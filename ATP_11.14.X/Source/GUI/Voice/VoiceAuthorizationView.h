// VoiceAuthorizationView.h : interface of the CVoiceAuthorizationView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOICEAUTHORIZATIONVIEW_H__20C11402_714A_11D3_9764_0008C789BA2A__INCLUDED_)
#define AFX_VOICEAUTHORIZATIONVIEW_H__20C11402_714A_11D3_9764_0008C789BA2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CVoiceAuthorizationView : public CView
{
protected: // create from serialization only
	CVoiceAuthorizationView();
	DECLARE_DYNCREATE(CVoiceAuthorizationView)

// Attributes
public:
	CVoiceAuthorizationDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiceAuthorizationView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVoiceAuthorizationView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVoiceAuthorizationView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in VoiceAuthorizationView.cpp
inline CVoiceAuthorizationDoc* CVoiceAuthorizationView::GetDocument()
   { return (CVoiceAuthorizationDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOICEAUTHORIZATIONVIEW_H__20C11402_714A_11D3_9764_0008C789BA2A__INCLUDED_)
