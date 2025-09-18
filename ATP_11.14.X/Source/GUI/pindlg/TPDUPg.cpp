// TPDUPg.cpp : implementation file
//

#include "stdafx.h"
#include "Sharedmem.h"

extern "C"
{
#include "basictyp.h"
#include "pte.h"
#include "coreinfo.h"
#include "cdsapi.h"

#include "ptemsg.h"
}


#include "PinDlg.h"
#include "TPDUPg.h"
#include "RouterRec.h"
#include "TPDURec.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL AppliedComSet;
extern BOOL AppliedTPDUSet;
extern BOOL AppliedRouterSet;
extern BOOL Connected;
extern ROUTER_TABLE_LIST Router_list;
extern TPDU_TABLE_LIST TPDU_list;
extern ROUTER_TABLE Router;	
extern TPDU	TPDUTable;		
extern HANDLE  hGET_TPDU;
extern HANDLE  hGET_ROUTER;
extern HANDLE  hUPDATE_ROUTER;
extern HANDLE  hUPDATE_TPDU;
extern HANDLE  hINSERT_ROUTER;
extern HANDLE  hINSERT_TPDU;
extern HANDLE  hDELETE_ROUTER;
extern HANDLE  hDELETE_TPDU;

//This cntr Variable is defined as to access its value across the other dialogs.
extern int cntr;
extern UINT timer_handle;
extern CSharedMem shrmem;
extern CTime c_time, shm_time;
/////////////////////////////////////////////////////////////////////////////
// CTPDUPg property page

IMPLEMENT_DYNCREATE(CTPDUPg, CPropertyPage)

CTPDUPg::CTPDUPg() : CPropertyPage(CTPDUPg::IDD)
{
	//{{AFX_DATA_INIT(CTPDUPg)
	//}}AFX_DATA_INIT
}

CTPDUPg::~CTPDUPg()
{
}

void CTPDUPg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTPDUPg)
	DDX_Control(pDX, IDC_APPLY2, m_Apply2_btn);
	DDX_Control(pDX, IDC_ADD, m_Add_btn);
	DDX_Control(pDX, IDC_TRANLEN_LBL, m_TranLen_lbl);
	DDX_Control(pDX, IDC_TPDUSTR_LBL, m_TPDUStr_lbl);
	DDX_Control(pDX, IDC_TPDUID_LBL, m_TPDUId_lbl);
	DDX_Control(pDX, IDC_TPDU_TBL_GRB, m_TPDU_tbl_grb);
	DDX_Control(pDX, IDC_SRCLEN_LBL, m_SrcLen);
	DDX_Control(pDX, IDC_ROUTER_TBL_GRB, m_Router_tbl_grb);
	DDX_Control(pDX, IDC_OFFSET_LBL, m_Offset_lbl);
	DDX_Control(pDX, IDC_LORANGE_LBL, m_LoRange_lbl);
	DDX_Control(pDX, IDC_LENGTH_LBL, m_Length_lbl);
	DDX_Control(pDX, IDC_HIRANGE_LBL, m_HiRange_lbl);
	DDX_Control(pDX, IDC_EDIT, m_Edit_btn);
	DDX_Control(pDX, IDC_DTTYPE_LBL, m_DataType_lbl);
	DDX_Control(pDX, IDC_DESTQUE_LBL, m_DestQue_lbl);
	DDX_Control(pDX, IDC_DESTLEN_LBL, m_DestLen_lbl);
	DDX_Control(pDX, IDC_DELETE, m_Delete_btn);
	DDX_Control(pDX, IDC_APPDESC_LBL, m_AppDesc_lbl);
	DDX_Control(pDX, IDC_ROUTER_TBL_LST, m_Router_tbl);
	DDX_Control(pDX, IDC_TPDU_TBL_LST, m_TPDU_tbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTPDUPg, CPropertyPage)
	//{{AFX_MSG_MAP(CTPDUPg)
	ON_LBN_SETFOCUS(IDC_ROUTER_TBL_LST, OnSetfocusRouterTblLst)
	ON_LBN_SETFOCUS(IDC_TPDU_TBL_LST, OnSetfocusTpduTblLst)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_LBN_DBLCLK(IDC_ROUTER_TBL_LST, OnDblclkRouterTblLst)
	ON_LBN_DBLCLK(IDC_TPDU_TBL_LST, OnDblclkTpduTblLst)
	ON_BN_CLICKED(IDC_CLOSE2, OnClose2)
	ON_BN_CLICKED(IDC_APPLY2, OnApply2)
	ON_BN_CLICKED(IDC_HELP2, OnHelp2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTPDUPg message handlers
BOOL CTPDUPg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int i;
	RouterChangeNumber = 0;
	TPDUChangeNumber = 0;

	int tabTPDU [4] = {70,120,180,240};
	m_TPDU_tbl.SetTabStops(4,tabTPDU);

	int tabRauter [7] = {55,160,185,215,255,285,350}; 
	m_Router_tbl.SetTabStops(7,tabRauter);


	//populate tpdu and router lists
	for (i = 0; i < TPDU_list.num_returned; ++i)
	{
		ZeroMemory(&TPDUTable,sizeof(TPDUTable));
		memcpy(&TPDUTable,&(TPDU_list.tpdu_record[i]),sizeof(TPDUTable));
		BuildTPDUStr();
		m_TPDU_tbl.AddString(BldString);
	}

	for (i = 0; i < Router_list.num_returned; ++i)
	{
		ZeroMemory(&Router,sizeof(Router));
		memcpy(&Router,&(Router_list.router_record[i]),sizeof(Router));
		BuildRouterStr();
		m_Router_tbl.AddString(BldString);
	}

	m_TPDU_tbl.SetFocus();
	ActiveList = TLIST;

	hUPDATE_ROUTER = CreateEvent(NULL,FALSE,FALSE,NULL);
	hUPDATE_TPDU = CreateEvent(NULL,FALSE,FALSE,NULL);
	hINSERT_ROUTER = CreateEvent(NULL,FALSE,FALSE,NULL);
	hINSERT_TPDU = CreateEvent(NULL,FALSE,FALSE,NULL);
	hDELETE_ROUTER = CreateEvent(NULL,FALSE,FALSE,NULL);
	hDELETE_TPDU = CreateEvent(NULL,FALSE,FALSE,NULL);
	hGET_TPDU = CreateEvent(NULL,FALSE,FALSE,NULL);
	hGET_ROUTER = CreateEvent(NULL,FALSE,FALSE,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTPDUPg::OnSetfocusRouterTblLst() 
{
	m_TPDU_tbl.SetCurSel(-1);
	ActiveList = RLIST;
}

void CTPDUPg::OnSetfocusTpduTblLst() 
{
	m_Router_tbl.SetCurSel(-1);	
	ActiveList = TLIST;
}

void CTPDUPg::OnAdd() 
{
	int res;
	CTPDURec TPDUDlg;
	CRouterRec RouterDlg;

	if (ActiveList == TLIST)
	{
		TPDUDlg.m_Mode = MODE_ADD;
		ZeroMemory(&TPDUTable,sizeof(TPDUTable));
		res = TPDUDlg.DoModal();
		if (res == IDOK)
		{
			if (cdsapi_insert_tpdu_config_snd(&TPDUTable,ErrMsg) != PTEMSG_OK)
			{
				AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
				return ;
			}
			if (!(WaitForSingleObject(hINSERT_TPDU,WAITRESP_TIME) == WAIT_OBJECT_0))
			{
				AfxMessageBox( "Failed to insert TPDU record.", MB_OK | MB_ICONSTOP, 0 );
				return;
			}
			ResetEvent(hINSERT_TPDU);

			AppliedTPDUSet = FALSE;
			SetModified(TRUE);
			m_Apply2_btn.EnableWindow(TRUE);
			BuildTPDUStr();
			m_TPDU_tbl.SetCurSel(m_TPDU_tbl.AddString(BldString));
		}
		else
			return;
	}
	else
		if (ActiveList == RLIST)
		{
			RouterDlg.m_Mode = MODE_ADD;
			ZeroMemory(&Router,sizeof(Router));
			res = RouterDlg.DoModal();
			if (res == IDOK)
			{
				
				if( ( 0 == strncmp((char *)Router.router_datatype,"i",1)) ||
					( 0 == strncmp((char *)Router.router_datatype,"I",1)))
				{
					int router_len=0;
					char strrouter_len[10]={0};
					lstrcpy(strrouter_len,(char *)Router.router_length);
					router_len = atoi(strrouter_len);
					if(router_len > 4)
					{
						strcpy(ErrMsg,"Invalid Router Length , should not be more than 4");
						AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
						return ;
					}
				}
				SetNewKey();
				if (cdsapi_insert_router_config_snd(&Router,ErrMsg) != PTEMSG_OK)
				{
					AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
					return ;
				}
				if (!(WaitForSingleObject(hINSERT_ROUTER,WAITRESP_TIME) == WAIT_OBJECT_0))
				{
					AfxMessageBox( "Failed to insert Router record.", MB_OK | MB_ICONSTOP, 0 );
					return;
				}
				ResetEvent(hINSERT_ROUTER);

				AppliedRouterSet = FALSE;
				SetModified(TRUE);
				m_Apply2_btn.EnableWindow(TRUE);
				BuildRouterStr();
				m_Router_tbl.SetCurSel(	m_Router_tbl.AddString(BldString));
			}
			else
				return;
		}
		else
			AfxMessageBox( "Please, click on the table to add record to.", MB_OK | MB_ICONSTOP, 0 );

}

void CTPDUPg::SetNewKey() 
{
	int c,count,maxid,itemkey;
	CString ListItem;
	char buffer[4];
	CString newkey,s3("000"),s2("00"),s1("0");

	maxid = 0;
	count = m_Router_tbl.GetCount();
	if (count == 0)
	{
		lstrcpy( (char *)Router.router_identifier, "0001");
		return;
	}

	for (c = 0; c < count; ++c)
	{
		m_Router_tbl.GetText(c,ListItem);
		itemkey = atoi(ListItem.Right(4));
		if (itemkey > maxid)
			maxid = itemkey;
	}

	_itoa(++maxid,buffer,10);
	newkey.Format(buffer);
	if (maxid < 10)
		newkey = s3 + newkey;
	else
		if (maxid < 100)
			newkey = s2 + newkey;
		else
			if (maxid < 1000)
				newkey = s1 + newkey;

	lstrcpy( (char *)Router.router_identifier, newkey);

	return ;
}



void CTPDUPg::OnEdit() 
{
	int res, index;
	CTPDURec TPDUDlg;
	CRouterRec RouterDlg;
	CString ListItem;


	index = m_TPDU_tbl.GetCurSel();
	if (index != LB_ERR)
	{
		TPDUDlg.m_Mode = MODE_EDIT;

		m_TPDU_tbl.GetText(index,ListItem);
		ZeroMemory(&TPDUTable,sizeof(TPDUTable));
		strncpy((char *)TPDUTable.tpdu_id,ListItem,strcspn(ListItem,"\t"));
		//run function to get the rest of the fields from DMS
		if (cdsapi_get_tpdu_record_snd(&TPDUTable,ErrMsg) != PTEMSG_OK)
		{
			AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
			return;
		}
				
		if (!(WaitForSingleObject(hGET_TPDU,WAITRESP_TIME) == WAIT_OBJECT_0))
		{
			AfxMessageBox( "Failed to retrieve selected TPDU line.", MB_OK | MB_ICONSTOP, 0 );
			return;
		}
		ResetEvent(hGET_TPDU);

		TPDUDlg.m_TPDUId.Format( (char *)TPDUTable.tpdu_id );
		TPDUDlg.m_TransLen.Format( (char *)TPDUTable.tran_len );
		TPDUDlg.m_DestLen.Format( (char *)TPDUTable.dst_len );
		TPDUDlg.m_SrcLen.Format( (char *)TPDUTable.src_len );
		TPDUDlg.m_TPDUStr.Format( (char *)TPDUTable.tpdu_string);

		res = TPDUDlg.DoModal();
		if (res == IDOK)
		{
			if (cdsapi_update_tpdu_config_snd(&TPDUTable,ErrMsg) != PTEMSG_OK)
			{
				AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
				return ;
			}
			if (!(WaitForSingleObject(hUPDATE_TPDU,WAITRESP_TIME) == WAIT_OBJECT_0))
			{
				AfxMessageBox( "Failed to update TPDU record.", MB_OK | MB_ICONSTOP, 0 );
				return;
			}
			ResetEvent(hUPDATE_TPDU);

			AppliedTPDUSet = FALSE;
			SetModified(TRUE);
			m_Apply2_btn.EnableWindow(TRUE);
			m_TPDU_tbl.DeleteString(index);
			BuildTPDUStr();
			m_TPDU_tbl.SetCurSel(m_TPDU_tbl.InsertString(index,BldString));
		}
		else
			return;
	}
	else
	{
		index = m_Router_tbl.GetCurSel();
		if (index != LB_ERR)
		{
			m_Router_tbl.GetText(index,ListItem);
			ZeroMemory(&Router,sizeof(Router));
			lstrcpy( (char *)Router.router_identifier, ListItem.Right(4));
			//run function to get the rest of the fields from DMS
			if (cdsapi_get_router_record_snd(&Router,ErrMsg) != PTEMSG_OK)
			{
				AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
				return;
			}

			if (!(WaitForSingleObject(hGET_ROUTER,WAITRESP_TIME) == WAIT_OBJECT_0))
			{
				AfxMessageBox( "Failed to retrieve selected ROUTER line.", MB_OK | MB_ICONSTOP, 0 );
				return;
			}
			ResetEvent(hGET_ROUTER);

			RouterDlg.m_DestQue.Format( (char *)Router.router_queue );
			RouterDlg.m_AppDesc.Format( (char *)Router.router_app_desc );
			RouterDlg.m_Offset.Format( (char *)Router.router_offset );
			RouterDlg.m_Length.Format( (char *)Router.router_length );
			RouterDlg.m_DataType.Format( (char *)Router.router_datatype);
			RouterDlg.m_LoRange.Format( (char *)Router.router_lo_end );
			RouterDlg.m_HiRange.Format( (char *)Router.router_hi_end);

			RouterDlg.m_Mode = MODE_EDIT;
			res = RouterDlg.DoModal();
			if (res == IDOK)
			{
				if (cdsapi_update_router_config_snd(&Router,ErrMsg) != PTEMSG_OK)
				{
					AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
					return ;
				}
				if (!(WaitForSingleObject(hUPDATE_ROUTER,WAITRESP_TIME) == WAIT_OBJECT_0))
				{
					AfxMessageBox( "Failed to update Router record.", MB_OK | MB_ICONSTOP, 0 );
					return;
				}
				ResetEvent(hUPDATE_ROUTER);

				AppliedRouterSet = FALSE;
				SetModified(TRUE);
				m_Apply2_btn.EnableWindow(TRUE);
				m_Router_tbl.DeleteString(index);
				BuildRouterStr();
				m_Router_tbl.SetCurSel(m_Router_tbl.InsertString(index,BldString));
			}
			else
				return;
		}
		else
			AfxMessageBox( "Please, select the line to edit.", MB_OK | MB_ICONSTOP, 0 );
	}

}

void CTPDUPg::OnDelete() 
{
	int index;
	CString ListItem;
	GENERIC_KEYS GenKey;

	index = m_TPDU_tbl.GetCurSel();
	if (index != LB_ERR)
	{
		m_TPDU_tbl.GetText(index,ListItem);
		ZeroMemory(&GenKey,sizeof(GenKey));
		strncpy((char *)GenKey.key1,ListItem,strcspn(ListItem,"\t"));
		if (cdsapi_delete_record_snd(&GenKey,TPDU_DATA,ErrMsg) != PTEMSG_OK)
		{
			AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
			return;
		}
		if (!(WaitForSingleObject(hDELETE_TPDU,WAITRESP_TIME) == WAIT_OBJECT_0))
		{
			AfxMessageBox( "Failed to delete TPDU record.", MB_OK | MB_ICONSTOP, 0 );
			return;
		}
		ResetEvent(hDELETE_TPDU);

		m_TPDU_tbl.DeleteString(index);
		AppliedTPDUSet = FALSE;
		SetModified(TRUE);
		m_Apply2_btn.EnableWindow(TRUE);
	}
	else
	{
		index = m_Router_tbl.GetCurSel();
		if (index != LB_ERR)
		{
			m_Router_tbl.GetText(index,ListItem);
			ZeroMemory(&GenKey,sizeof(GenKey));
			lstrcpy( (char *)GenKey.key1, ListItem.Right(4));
			if (cdsapi_delete_record_snd(&GenKey,ROUTER_DATA,ErrMsg) != PTEMSG_OK)
			{
				AfxMessageBox( ErrMsg, MB_OK | MB_ICONSTOP, 0 );
				return;
			}
			if (!(WaitForSingleObject(hDELETE_ROUTER,WAITRESP_TIME) == WAIT_OBJECT_0))
			{
				AfxMessageBox( "Failed to delete Router record.", MB_OK | MB_ICONSTOP, 0 );
				return;
			}
			ResetEvent(hDELETE_ROUTER);

			m_Router_tbl.DeleteString(index);
			AppliedRouterSet = FALSE;
			SetModified(TRUE);
			m_Apply2_btn.EnableWindow(TRUE);
		}
		else
			AfxMessageBox( "Please, select the line to delete.", MB_OK | MB_ICONSTOP, 0 );
	}
}

void CTPDUPg::BuildTPDUStr() 
{
	BldString[0] = '\0';
	lstrcpy(BldString,(char *)TPDUTable.tpdu_id);
	lstrcat(BldString,"\t");
	lstrcat(BldString,(char *)TPDUTable.tran_len);
	lstrcat(BldString,"\t");
	lstrcat(BldString,(char *)TPDUTable.dst_len);
	lstrcat(BldString,"\t");
	lstrcat(BldString,(char *)TPDUTable.src_len);
	lstrcat(BldString,"\t");
	lstrcat(BldString,(char *)TPDUTable.tpdu_string);

	return ;
}	

void CTPDUPg::BuildRouterStr() 
{

	BldString[0] = '\0';
	lstrcpy(BldString,(char *)Router.router_queue);
	lstrcat(BldString,"\t");
	lstrcat(BldString,(char *)Router.router_app_desc);
	lstrcat(BldString,"\t");
	lstrcat(BldString,(char *)Router.router_offset);
	lstrcat(BldString,"\t");
	lstrcat(BldString,(char *)Router.router_length);
	lstrcat(BldString,"\t");
	lstrcat(BldString,(char *)Router.router_datatype);
	lstrcat(BldString,"\t");
	lstrcat(BldString,(char *)Router.router_lo_end);
	lstrcat(BldString,"\t");
	lstrcat(BldString,(char *)Router.router_hi_end);
	lstrcat(BldString,"\t");
	lstrcat(BldString,(char *)Router.router_identifier);
	return ;
}
BOOL CTPDUPg::OnSetActive() 
{
	if (AppliedComSet == FALSE || AppliedTPDUSet == FALSE || AppliedRouterSet == FALSE)
		m_Apply2_btn.EnableWindow(TRUE);
	else
		m_Apply2_btn.EnableWindow(FALSE);

	if (Connected == TRUE)	
	{
		m_Add_btn.EnableWindow(FALSE);
		m_TranLen_lbl.EnableWindow(FALSE);
		m_TPDUStr_lbl.EnableWindow(FALSE);
		m_TPDUId_lbl.EnableWindow(FALSE);
		m_TPDU_tbl_grb.EnableWindow(FALSE);
		m_SrcLen.EnableWindow(FALSE);
		m_Router_tbl_grb.EnableWindow(FALSE);
		m_Offset_lbl.EnableWindow(FALSE);
		m_LoRange_lbl.EnableWindow(FALSE);
		m_Length_lbl.EnableWindow(FALSE);
		m_HiRange_lbl.EnableWindow(FALSE);
		m_Edit_btn.EnableWindow(FALSE);
		m_DataType_lbl.EnableWindow(FALSE);
		m_DestQue_lbl.EnableWindow(FALSE);
		m_DestLen_lbl.EnableWindow(FALSE);
		m_Delete_btn.EnableWindow(FALSE);
		m_AppDesc_lbl.EnableWindow(FALSE);
		m_Router_tbl.EnableWindow(FALSE);
		m_TPDU_tbl.EnableWindow(FALSE);
	}
	else
	{

		m_Add_btn.EnableWindow(TRUE);
		m_TranLen_lbl.EnableWindow(TRUE);
		m_TPDUStr_lbl.EnableWindow(TRUE);
		m_TPDUId_lbl.EnableWindow(TRUE);
		m_TPDU_tbl_grb.EnableWindow(TRUE);
		m_SrcLen.EnableWindow(TRUE);
		m_Router_tbl_grb.EnableWindow(TRUE);
		m_Offset_lbl.EnableWindow(TRUE);
		m_LoRange_lbl.EnableWindow(TRUE);
		m_Length_lbl.EnableWindow(TRUE);
		m_HiRange_lbl.EnableWindow(TRUE);
		m_Edit_btn.EnableWindow(TRUE);
		m_DataType_lbl.EnableWindow(TRUE);
		m_DestQue_lbl.EnableWindow(TRUE);
		m_DestLen_lbl.EnableWindow(TRUE);
		m_Delete_btn.EnableWindow(TRUE);
		m_AppDesc_lbl.EnableWindow(TRUE);
		m_Router_tbl.EnableWindow(TRUE);
		m_TPDU_tbl.EnableWindow(TRUE);
	}

	return CPropertyPage::OnSetActive();
}

void CTPDUPg::OnDblclkRouterTblLst() 
{
	OnEdit();	
}

void CTPDUPg::OnDblclkTpduTblLst() 
{
	OnEdit();
	
}

void CTPDUPg::OnClose2() 
{	
	//Delete the associated Mutex lock 
	shrmem.DestroySharedMemorySegment();
	shrmem.DeleteMutexLock();

	m_PSheet->PressButton(PSBTN_CANCEL);
	
}

void CTPDUPg::OnApply2() 
{
	m_PSheet->PressButton(PSBTN_APPLYNOW);
	if (AppliedComSet == TRUE && AppliedTPDUSet == TRUE && AppliedRouterSet == TRUE)
		m_Apply2_btn.EnableWindow(FALSE);
}

void CTPDUPg::OnHelp2() 
{
	m_PSheet->PressButton(PSBTN_HELP);
	
}

BOOL CTPDUPg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	cntr = 0;
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
