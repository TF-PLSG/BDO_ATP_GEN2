#pragma once


// DinersInfo dialog

class DinersInfo : public CDialogEx
{
	DECLARE_DYNAMIC(DinersInfo)

public:
	DinersInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~DinersInfo();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
