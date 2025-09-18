#pragma once


// VisaInfo dialog

class VisaInfo : public CDialogEx
{
	DECLARE_DYNAMIC(VisaInfo)

public:
	VisaInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~VisaInfo();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
