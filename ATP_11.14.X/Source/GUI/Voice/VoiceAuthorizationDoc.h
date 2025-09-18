// VoiceAuthorizationDoc.h : interface of the CVoiceAuthorizationDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOICEAUTHORIZATIONDOC_H__20C11400_714A_11D3_9764_0008C789BA2A__INCLUDED_)
#define AFX_VOICEAUTHORIZATIONDOC_H__20C11400_714A_11D3_9764_0008C789BA2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CVoiceAuthorizationDoc : public CDocument
{
protected: // create from serialization only
	CVoiceAuthorizationDoc();
	DECLARE_DYNCREATE(CVoiceAuthorizationDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiceAuthorizationDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVoiceAuthorizationDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVoiceAuthorizationDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOICEAUTHORIZATIONDOC_H__20C11400_714A_11D3_9764_0008C789BA2A__INCLUDED_)
