// DinersInfo.cpp : implementation file
//

#include "stdafx.h"
#include "Diners_Parser.h"
#include "DinersInfo.h"
#include "afxdialogex.h"


// DinersInfo dialog

IMPLEMENT_DYNAMIC(DinersInfo, CDialogEx)

DinersInfo::DinersInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(DinersInfo::IDD, pParent)
{

}

DinersInfo::~DinersInfo()
{
}

void DinersInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DinersInfo, CDialogEx)
END_MESSAGE_MAP()


// DinersInfo message handlers
