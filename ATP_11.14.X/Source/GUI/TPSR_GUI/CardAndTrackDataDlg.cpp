// CardAndTrackDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tpsr_gui.h"
#include "CardAndTrackDataDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardAndTrackDataDlg property page

IMPLEMENT_DYNCREATE(CCardAndTrackDataDlg, CPropertyPage)

CCardAndTrackDataDlg::CCardAndTrackDataDlg() : CPropertyPage(CCardAndTrackDataDlg::IDD)
{
	//{{AFX_DATA_INIT(CCardAndTrackDataDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CCardAndTrackDataDlg::~CCardAndTrackDataDlg()
{
}

void CCardAndTrackDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardAndTrackDataDlg)
	DDX_Control(pDX, IDC_TRACK_DATA, m_CntlTrackData);
	DDX_Control(pDX, IDC_CARD_NUMBERS, m_CntlCardNumbers);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardAndTrackDataDlg, CPropertyPage)
//{{AFX_MSG_MAP(CCardAndTrackDataDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardAndTrackDataDlg message handlers

BOOL CCardAndTrackDataDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCardAndTrackDataDlg::OnOK() 
{
	UpdateData(TRUE);
	CString cardnumbers;
	m_CntlCardNumbers.GetWindowText(cardnumbers);
	CString trackdata;
	m_CntlTrackData.GetWindowText(trackdata);
	CString tempDataStorage;
	CString cardTempData;
	CString trackTempData;
	char tempdata[512];
	int datalen;
	char cardlen[3];
	int no_of_cards = m_CntlCardNumbers.GetLineCount();
	m_CntlCardNumbers.LineIndex(2);
				
	int no_of_trackdata = m_CntlTrackData.GetLineCount();
	datalen = m_CntlCardNumbers.LineLength(1);
	
	if(cardnumbers.IsEmpty() || trackdata.IsEmpty())
	{
		AfxMessageBox("Please input some data");
		return;
	}
	if(no_of_cards == no_of_trackdata)
	{
		FILE *fptr = fopen("tpsr_card.lst","w");
		if(fptr != NULL)
		{
			for(int i=0;i<no_of_cards;i++)
			{
				m_CntlCardNumbers.LineIndex(i);
				datalen = m_CntlCardNumbers.LineLength(i);
				m_CntlCardNumbers.GetLine(i,cardTempData.GetBuffer(datalen),datalen);
				cardTempData.ReleaseBuffer(datalen);
				cardTempData.Remove(' ');
				cardTempData.Remove(13);
				datalen = cardTempData.GetLength();
				sprintf(cardlen, "%2d",datalen);
				if(datalen % 2 != 0)
				{
					cardTempData = cardTempData + "0";
				}
				cardTempData = cardlen + cardTempData;
				
				datalen = m_CntlTrackData.LineLength(i);
				m_CntlTrackData.GetLine(i,trackTempData.GetBuffer(datalen), datalen);
				trackTempData.ReleaseBuffer(datalen);
				trackTempData.MakeUpper();
				strcpy(tempdata, trackTempData);
				for (int i=0; i< strlen(tempdata); i++)
				{
					if(tempdata[i] < 48 || tempdata[i] > 57)	/* < 0 or >9*/
					{
						if(tempdata[i] >70) /* < A or > F*/
						{
							trackTempData.Remove(tempdata[i]);
						}
						else
						{
							if(tempdata[i] < 67)
								if(tempdata[i] != '=')
								{
									trackTempData.Remove(tempdata[i]);
								}

						}
					}
				}
				trackTempData.Remove(';');
				trackTempData.Remove(' ');
				trackTempData.Remove(13);
				trackTempData.Replace('=','d');
				trackTempData.Replace('?','f');
				trackTempData = "37" + trackTempData.Left(38);
				fprintf(fptr, "%s%s\n",cardTempData,trackTempData);
			}
			fclose(fptr);
			AfxMessageBox("Card and Track data List file prepared");
		}
		
	}
	else
	{
		AfxMessageBox("Cannot map card numbers to track data. Input same number of cards and trackdata");
	}
}
