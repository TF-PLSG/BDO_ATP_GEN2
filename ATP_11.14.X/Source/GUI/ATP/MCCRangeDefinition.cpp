// MCCRangeDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MCCRangeDefinition.h"
#include "BinList.h"
#include "MyUtils.h"
#include "SharedMem.h"
extern "C"
{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
//extern merch_ctr;
extern int merch_ctr;

extern CString  MerchantID;
extern CString  OrganizationID;
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CMCCRangeDefinition dialog


CMCCRangeDefinition::CMCCRangeDefinition(CWnd* pParent /*=NULL*/)
	: CDialog(CMCCRangeDefinition::IDD, pParent)
{
	m_nSelectedItem = -1;
	m_bListDone = false;
	m_bEditMode = false;
	memset(&sMCC01List, 0, sizeof(MCC01_GUI_LIST));
	memset(&sMCC01ListRecord, 0, sizeof(MCC01));
	m_bAdd = true;
	m_bEdit = true;
	m_bView = true;
	m_bDelete = true;
	//{{AFX_DATA_INIT(CMCCRangeDefinition)
	m_nHigh = 0;
	m_nLow = 0;
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}


void CMCCRangeDefinition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCCRangeDefinition)
	DDX_Control(pDX, IDCANCEL, m_CntlCancelBtn);
	DDX_Control(pDX, IDC_LOW, m_CntlLow);
	DDX_Control(pDX, IDC_HIGH, m_CntlHigh);
	DDX_Control(pDX, IDC_NEXT20, m_CntlNext20Btn);
	DDX_Control(pDX, IDC_ADD, m_CntlAddBtn);
	DDX_Control(pDX, IDC_DELETE, m_CntlDeleteBtn);
	DDX_Control(pDX, IDC_EDIT, m_CntlEditBtn);
	DDX_Control(pDX, IDC_RANGE_LIST, m_CntlRangeList);
	DDX_Text(pDX, IDC_HIGH, m_nHigh);
	DDV_MinMaxInt(pDX, m_nHigh, 1000, 9999);
	DDX_Text(pDX, IDC_LOW, m_nLow);
	DDV_MinMaxInt(pDX, m_nLow, 1000, 9999);
	DDX_Text(pDX, IDC_STATUS, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCCRangeDefinition, CDialog)
	//{{AFX_MSG_MAP(CMCCRangeDefinition)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_RANGE_LIST, OnDblclkRangeList)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_NEXT20, OnNext20)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMCCRangeDefinition message handlers

void CMCCRangeDefinition::OnAdd() 
{
	CString szLow, szHigh;
	char lpszLow[10]  = {0};
	char lpszHigh[10] = {0};
	MCC01 sMCC01Record;
	int rcode;
	char strErrorMessage[200] = {0};
	m_StrMessageLbl = "";

	UpdateData();
	if(m_nLow > m_nHigh)
	{
		AfxMessageBox("Low value cannot be greater than high value");
		return;
	}
	szLow.Format("%d",m_nLow);
	szHigh.Format("%d",m_nHigh);
	sprintf((char *)sMCC01Record.primary_key.low,  "%d", m_nLow);
	sprintf((char *)sMCC01Record.primary_key.high, "%d", m_nHigh);

	rcode = txdsapi_insert_record( (pBYTE)&sMCC01Record,sizeof (MCC01), MCC01_DATA, strErrorMessage );
	if(rcode != PTEMSG_OK)
	{
		m_StrMessageLbl = strErrorMessage;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}
	else
	{
		m_StrMessageLbl = "Record Added";
		m_CntlRangeList.InsertItem(0, szLow, szHigh);
	}
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

}
int SelectedItemForEdit = -1;
void CMCCRangeDefinition::OnEdit() 
{
	CString szText;
	m_CntlEditBtn.GetWindowText(szText);
	if(szText.Compare("Edit") == 0)
	{
		if(m_CntlRangeList.GetSelectedItemCount() < 1)
		{
			AfxMessageBox("Please select a range to edit");
			return;
		}
		CString szLow, szHigh;
		int sel = m_CntlRangeList.GetFirstSelectedItem();
		szLow = m_CntlRangeList.GetItemText(sel, 0);
		szHigh = m_CntlRangeList.GetItemText(sel, 1);
		m_nLow = atoi(szLow);
		m_nHigh = atoi(szHigh);
		m_CntlEditBtn.SetWindowText("Save");
		UpdateData(FALSE);
		SelectedItemForEdit = sel;
	}
	else
	{
		
			CString szLow, szHigh;
	
		UpdateData();
		if(m_nLow > m_nHigh)
		{
			AfxMessageBox("Low value cannot be greater than high value");
			return;
		}
		
	

		MCC01 sMCC01Record;
		int rcode;
		char strErrorMessage[200] = {0};
		m_StrMessageLbl = "";
		szLow  = m_CntlRangeList.GetItemText(m_nSelectedItem, 0);
		szHigh = m_CntlRangeList.GetItemText(m_nSelectedItem, 1);
		
		sprintf((char *)sMCC01Record.primary_key.low,  "%s", szLow);
		sprintf((char *)sMCC01Record.primary_key.high, "%s", szHigh);
		
		rcode = txdsapi_delete_record( (pBYTE)&sMCC01Record,sizeof (MCC01), MCC01_DATA, strErrorMessage );
		if(rcode != PTEMSG_OK)
		{
			m_StrMessageLbl = "Unable to edit";
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		}
		else
		{
			m_CntlLow.GetWindowText(szLow);
			m_CntlHigh.GetWindowText(szHigh);
			sprintf((char *)sMCC01Record.primary_key.low,  "%d", m_nLow);
			sprintf((char *)sMCC01Record.primary_key.high, "%d", m_nHigh);
			
			rcode = txdsapi_insert_record( (pBYTE)&sMCC01Record,sizeof (MCC01), MCC01_DATA, strErrorMessage );
			if(rcode != PTEMSG_OK)
			{
				m_StrMessageLbl = strErrorMessage;
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				return ;
			}
			else
			{
				m_StrMessageLbl = "Record Edited";
				m_CntlRangeList.SetItemText(m_nSelectedItem, 0, szLow);
				m_CntlRangeList.SetItemText(m_nSelectedItem, 1, szHigh);
				//m_CntlRangeList.InsertItem(0, szLow, szHigh);
				EditMode(false);
			}
		}
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

	}
	
}

BOOL CMCCRangeDefinition::OnInitDialog() 
{
	CDialog::OnInitDialog();
		CFont m_font;
	LOGFONT lf;
CString temp;
	int nIndex = -1;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_STATUS)->SetFont (&m_font);

		for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Merchant Control" ) == 0)
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( ucf01.control[nIndex].action[0] == '0' )
		{
			m_bAdd = false;
			m_CntlAddBtn.EnableWindow(FALSE);
		}
		if ( ucf01.control[nIndex].action[1] == '0' )
		{
			m_bDelete = false;
			m_CntlDeleteBtn.EnableWindow(FALSE);
		}
		if ( ucf01.control[nIndex].action[2] == '0' )
		{
			m_bEdit= false;
			m_CntlEditBtn.EnableWindow(FALSE);
		}
		if ( ucf01.control[nIndex].action[3] == '0' )
		{
			m_bView = false;
			m_CntlNext20Btn.EnableWindow(FALSE);
		}
	}
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);

	m_CntlRangeList.SetHeadings("LOW, 60;HIGH, 60");
	strcpy((char *)sMCC01ListRecord.primary_key.low, " ");
	LoadListBox();
		return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMCCRangeDefinition::LoadListBox()
{
		char strErrorMessage[200];
	CString temp;
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize;
	int nST2 = 0;

	
	if(!m_bView)
		return;
	int rcode = txdsapi_get_list ((pBYTE)&sMCC01ListRecord, sizeof(MCC01),
									(pBYTE)&sMCC01List, sizeof(MCC01_GUI_LIST),
									nST2, MCC01_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{
		nNumberReturned = atoi ((char *)&sMCC01List.num_returned);
        if( nNumberReturned == GUI_MAX_LIST_SIZE)
        {
			memcpy(sMCC01ListRecord.primary_key.low, (LPCTSTR)sMCC01List.mcc01_pk[nNumberReturned-1].primary_key.low, sizeof(sMCC01ListRecord.primary_key.low));
			memcpy(sMCC01ListRecord.primary_key.high, (LPCTSTR)sMCC01List.mcc01_pk[nNumberReturned-1].primary_key.high, sizeof(sMCC01ListRecord.primary_key.high));
			memcpy(sMCC01ListRecord.allow, (LPCTSTR)sMCC01List.mcc01_pk[nNumberReturned-1].allow, sizeof(sMCC01ListRecord.allow));
			nSize = nNumberReturned - 1;
		}
		else
		{
			nSize = nNumberReturned;
			m_bListDone = true;
		}
		int nIndex = m_CntlRangeList.GetItemCount();
		for(int i=0; i< nSize; i++)
		{
			m_CntlRangeList.InsertItem(nIndex++, (LPCTSTR)sMCC01List.mcc01_pk[i].primary_key.low, (LPCTSTR)sMCC01List.mcc01_pk[i].primary_key.high);
		}
	}
}
void CMCCRangeDefinition::OnDblclkRangeList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	if(!m_bEdit && !m_bDelete)
		return;
	m_nSelectedItem = m_CntlRangeList.GetFirstSelectedItem();
	
	if(m_nSelectedItem > -1 )
	{
		EditMode(true);
		CString str = m_CntlRangeList.GetItemText(m_nSelectedItem, 0);
		m_CntlLow.SetWindowText(str);
		str = m_CntlRangeList.GetItemText(m_nSelectedItem, 1);
		m_CntlHigh.SetWindowText(str);
	}
	*pResult = 0;
}

void CMCCRangeDefinition::OnDelete() 
{
	CString szLow, szHigh;
	char lpszLow[10]  = {0};
	char lpszHigh[10] = {0};
	MCC01 sMCC01Record;
	int rcode;
	char strErrorMessage[200] = {0};
	m_StrMessageLbl = "";

	if(m_nSelectedItem < 0)
		return;

	szLow  = m_CntlRangeList.GetItemText(m_nSelectedItem, 0);
	szHigh = m_CntlRangeList.GetItemText(m_nSelectedItem, 1);
	CString str;
	str.Format("Are you sure to delete %s - %s?",szLow, szHigh);

	INT ret = AfxMessageBox(str , MB_YESNO | MB_ICONQUESTION );
	if( ret == IDNO )
		return;

	sprintf((char *)sMCC01Record.primary_key.low,  "%s", szLow);
	sprintf((char *)sMCC01Record.primary_key.high, "%s", szHigh);

	rcode = txdsapi_delete_record( (pBYTE)&sMCC01Record,sizeof (MCC01), MCC01_DATA, strErrorMessage );
	if(rcode != PTEMSG_OK)
	{
		m_StrMessageLbl = strErrorMessage;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}
	else
	{
		m_StrMessageLbl = "Record Deleted";
		//m_CntlRangeList.InsertItem(0, szLow, szHigh);
		m_CntlRangeList.DeleteAllSelectedItems();
		EditMode(false);
	}
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
}

void CMCCRangeDefinition::OnNext20() 
{
	if(m_bListDone)
	{
		m_StrMessageLbl = "End of List";
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

	LoadListBox();
	
}

void CMCCRangeDefinition::OnCancel() 
{

	if(!m_bEditMode)
	{
		CDialog::OnCancel();
	}
	else
	{
		EditMode(false);
	}
}

void CMCCRangeDefinition::EditMode(bool bEdit)
{
	if(bEdit)
	{
		m_bEditMode = true;
		if(m_bAdd)
			m_CntlAddBtn.EnableWindow(!m_bEditMode);
		if(m_bView)
			m_CntlNext20Btn.EnableWindow(!m_bEditMode);
		if(m_bEdit)
			m_CntlEditBtn.EnableWindow(m_bEditMode);
		if(m_bDelete)
			m_CntlDeleteBtn.EnableWindow(m_bEditMode);
		if(m_bEdit)
			m_CntlEditBtn.SetWindowText("Save");
		
		m_CntlCancelBtn.SetWindowText("Cancel");
	}
	else
	{
		m_bEditMode = false;
		if(m_bAdd)
			m_CntlAddBtn.EnableWindow(!m_bEditMode);
		if(m_bView)
			m_CntlNext20Btn.EnableWindow(!m_bEditMode);
		if(m_bEdit)
			m_CntlEditBtn.EnableWindow(m_bEditMode);
		if(m_bDelete)
			m_CntlDeleteBtn.EnableWindow(m_bEditMode);
		if(m_bEdit)
			m_CntlEditBtn.SetWindowText("Edit");
		m_CntlCancelBtn.SetWindowText("Close");
		m_CntlLow.SetWindowText("0");
		m_CntlHigh.SetWindowText("0");
	}
}
