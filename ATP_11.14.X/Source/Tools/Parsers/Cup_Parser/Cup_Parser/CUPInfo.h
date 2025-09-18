#pragma once
#include "Resource.h"

// AmexInfo dialog

class CUPInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CUPInfo)

public:
	CUPInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUPInfo();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
