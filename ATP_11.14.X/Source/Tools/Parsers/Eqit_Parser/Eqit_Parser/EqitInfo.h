#pragma once


// EqitInfo dialog

class EqitInfo : public CDialogEx
{
	DECLARE_DYNAMIC(EqitInfo)

public:
	EqitInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~EqitInfo();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
