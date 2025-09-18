// AmexInfo.cpp : implementation file
//

#include "stdafx.h"
#include "MCInfo.h"
#include "afxdialogex.h"


// AmexInfo dialog

IMPLEMENT_DYNAMIC(MCInfo, CDialogEx)

MCInfo::MCInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(MCInfo::IDD, pParent)
{

}

MCInfo::~MCInfo()
{
}

void MCInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MCInfo, CDialogEx)
END_MESSAGE_MAP()


// AmexInfo message handlers
