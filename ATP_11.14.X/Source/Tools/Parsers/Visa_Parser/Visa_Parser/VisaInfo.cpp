// VisaInfo.cpp : implementation file
//

#include "stdafx.h"
#include "Visa_Parser.h"
#include "VisaInfo.h"
#include "afxdialogex.h"


// VisaInfo dialog

IMPLEMENT_DYNAMIC(VisaInfo, CDialogEx)

VisaInfo::VisaInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(VisaInfo::IDD, pParent)
{

}

VisaInfo::~VisaInfo()
{
}

void VisaInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VisaInfo, CDialogEx)
END_MESSAGE_MAP()


// VisaInfo message handlers
