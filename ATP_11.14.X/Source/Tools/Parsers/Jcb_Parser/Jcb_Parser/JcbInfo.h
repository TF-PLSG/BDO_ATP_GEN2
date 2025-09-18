#pragma once


// JcbInfo dialog

class JcbInfo : public CDialogEx
{
	DECLARE_DYNAMIC(JcbInfo)

public:
	JcbInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~JcbInfo();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
