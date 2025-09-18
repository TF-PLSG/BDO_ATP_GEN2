// UsersList.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "UsersList.h"
extern "C"
{
   #include "ptetcp.h"
   #include "ntutils.h"
   #include "Txutils.h"

}
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUsersList dialog


CUsersList::CUsersList(CWnd* pParent /*=NULL*/)
	: CDialog(CUsersList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUsersList)
	m_StrStatus = _T("");
	//}}AFX_DATA_INIT
	m_pFile = NULL;
}


void CUsersList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUsersList)
	DDX_Control(pDX, IDC_START, m_CntlStartBtn);
	DDX_Control(pDX, IDCANCEL, m_CntlCancelBtn);
	DDX_Control(pDX, IDC_CURRENT_STATUS, m_CntlStatus);
	DDX_Text(pDX, IDC_CURRENT_STATUS, m_StrStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUsersList, CDialog)
	//{{AFX_MSG_MAP(CUsersList)
	ON_BN_CLICKED(IDC_START, OnStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUsersList message handlers

void CUsersList::OnStart() 
{
	CFileDialog fd(FALSE, "csv",NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING , "Comma Seperated files (*.csv) | *.csv",this);
	CStringArray strArr;
	int totalUsers;
	CString Status;
	CTime theTime;		
	int year,month,day;		
	char szCurDir[200] = {0};
	char modifiedDate[10] = {0},toDayDate[10]={0};
	CHAR ini_file[256];
	
	GetAscendentConfigDirectory( ini_file );
	strcat( ini_file, "tf.ini" );
	GetCurrentDirectory(200, szCurDir);
	
	nNumberReturned = 0;
	strBinType = "";
	m_strName = "";
	strPrimaryKey1 = "";
	strNetworkType = "";
	
	theTime = CTime::GetCurrentTime();//Get Todays date
	year= 	theTime.GetYear ();
	month = theTime.GetMonth();
	day = theTime.GetDay ();

	long pass_exp_days,temp_pass_exp_days=0;
		char pass_days[10];
	    (void) GetPrivateProfileString(
        "GUI", 	                   // points to section name 
        "USER_PASSWORDEXPIRY_DAYS",	           // points to key name 
        "90",	               // points to default string 
        pass_days,               // points to destination buffer 
        sizeof(pass_days) - 1,   // size of destination buffer 
        ini_file );                // points to initialization filename 
		 
	    pass_days[sizeof(pass_days)-1] = 0; //Make sure string is terminated
		
		pass_exp_days = atoi(pass_days);
		temp_pass_exp_days = pass_exp_days;
	m_CntlStatus.SetWindowText("Calculating total users...");
	CalculateTotalUsers(strArr);
	totalUsers = strArr.GetSize();
	Status.Format("%d user(s) found",totalUsers);
	m_CntlStatus.SetWindowText(Status);
	if(fd.DoModal() == IDOK)
	{
		SetCurrentDirectory(szCurDir);
		CString filename = fd.GetPathName();
		m_pFile = fopen(filename, "w");
		if(m_pFile != NULL)
		{
			fprintf(m_pFile, "REPORT NAME:, USER LIST,\n");
			fprintf(m_pFile, "DATE:, %02d%02d%04d,\n",month, day, year);
			fprintf(m_pFile,"\n\n");

			fprintf(m_pFile, "USER NAME,USER ID,GROUP NAME,DATE CREATED,EXPIRY DATE,STATUS,USER ID STATUS ,DATE OF DEACTIVATION,LAST LOGIN TIME,LAST LOGOUT TIME, FUNCTION,ACCESS RIGHTS,\n");

			for(int i=0;i<totalUsers; i++)
			{
				
				Status.Format("Processing %d of %d Users",i+1, totalUsers);
				m_CntlStatus.SetWindowText(Status);
				if(GetUserRecord(strArr.GetAt(i)) == PTEMSG_OK)
				{
					sprintf(toDayDate,"%d%02d%02d",year,month,day);//converting toDayDate to yyyymmdd format
					strcpy(modifiedDate,(char*)sUserRecord.password_modified_date);//Store passwordmodified date
					
					long jday1 = 0, jday2 = 0;
					jday1 = Txutils_Calculate_Julian_Date((unsigned char*)modifiedDate);//calculate juliandate for modified date
					jday2 = Txutils_Calculate_Julian_Date((unsigned char*)toDayDate);//calculate juliandate for TodaysDate
					pass_exp_days += jday1;
					Txutils_Calculate_Gregorian_Date(pass_exp_days, modifiedDate);
					pass_exp_days = temp_pass_exp_days;
			

				CString uname = sUserRecord.user_info_1;
		//		uname += sUserRecord.user_info_2;
				uname +=(CString)((char *) sUserRecord.user_info_2);
				fprintf(m_pFile,"%s,%s,%s,%s,%s,%s,%s,%s,",uname,
													sUserRecord.primary_key.name,
													sUserRecord.group_id,
													sUserRecord.date_added,
													modifiedDate,
													sUserRecord.accountstatus,
													sUserRecord.User_Status,
													sUserRecord.Date_Of_Deactivation);
				
				if(sUserRecord.user_info_3[0] == 0)
					fprintf(m_pFile,",");
				else
					fprintf(m_pFile,"%02d-%02d-%04d [%02d:%02d:%02d],",sUserRecord.user_info_3[1],
																	sUserRecord.user_info_3[2],
																	sUserRecord.user_info_3[0] + 2000, 
																	sUserRecord.user_info_3[3],
																	sUserRecord.user_info_3[4],
																	sUserRecord.user_info_3[5]);
				if(sUserRecord.user_info_4[0] == 0)
					fprintf(m_pFile,",");
				else
					fprintf(m_pFile,"%02d-%02d-%04d [%02d:%02d:%02d],",	sUserRecord.user_info_4[1],
																	sUserRecord.user_info_4[2],
																	sUserRecord.user_info_4[0] + 2000, 
																	sUserRecord.user_info_4[3],
																	sUserRecord.user_info_4[4],
																	sUserRecord.user_info_4[5]);

				fprintf(m_pFile, "%s,",sUserRecord.control[0].subsystem_name);
				if(sUserRecord.control[0].action[0] == '1')
					fprintf(m_pFile,"Add-");
				if(sUserRecord.control[0].action[1] == '1')
					fprintf(m_pFile,"Delete-");
				if(sUserRecord.control[0].action[2] == '1')
					fprintf(m_pFile,"Update-");
				if(sUserRecord.control[0].action[3] == '1')
					fprintf(m_pFile,"View,");
				fprintf(m_pFile,"\n");

				for(int j=1;j<20;j++)
				{
					if(sUserRecord.control[j].subsystem_name[0] != '\0')
					{
						fprintf(m_pFile, ",,,,,,,,%s,",sUserRecord.control[j].subsystem_name);
						if(sUserRecord.control[j].action[0] == '1')
							fprintf(m_pFile,"Add-");
						if(sUserRecord.control[j].action[1] == '1')
							fprintf(m_pFile,"Delete-");
						if(sUserRecord.control[j].action[2] == '1')
							fprintf(m_pFile,"Update-");
						if(sUserRecord.control[j].action[3] == '1')
							fprintf(m_pFile,"View,");
						fprintf(m_pFile,"\n");
					}
				}
				fprintf(m_pFile,"\n");


				}
				else
				{
					return ;
				}


			}
			fprintf(m_pFile,"\n\n");
			fprintf(m_pFile,"Total Count:,%d,\n",totalUsers);
			fclose (m_pFile);
		}
		else
		{
			m_CntlStatus.SetWindowText("ERROR in opening the file");
		}

	}
	m_CntlStatus.SetWindowText("User list backup completed.");
	m_CntlCancelBtn.SetWindowText("Close");
	
}

int CUsersList::CalculateTotalUsers(CStringArray &strArr)
{
	int rcode, nSize, i;
	char strErrorMessage[200] = {0};
		ZeroMemory ( &sUserRecord, sizeof (UCF01));
	
	if ( !strPrimaryKey1.IsEmpty())
		strcpy((char *)sUserRecord.primary_key.name, strPrimaryKey1);
	else
		strcpy((char *)sUserRecord.primary_key.name, " ");    
	
	do
	{
		rcode = txdsapi_get_list((pBYTE)&sUserRecord, sizeof (UCF01), 
			(pBYTE)&sUserList,sizeof (UCF01_LIST), 0,
			UCF01_DATA, strErrorMessage) ;	
		if (rcode == PTEMSG_OK) 
		{
			nNumberReturned = atoi ((char *)&sUserList.num_returned);
			if( nNumberReturned == GUI_MAX_LIST_SIZE)
			{				
				CString strTemp = sUserList.name[nNumberReturned-1] ;
				strTemp.TrimRight() ; strTemp.TrimLeft() ;
				memcpy( sUserRecord.primary_key.name, (LPCTSTR) strTemp, sizeof (sUserRecord.primary_key.name)); 
				nSize = nNumberReturned - 1;			
			}
			else
				nSize = nNumberReturned;			
			
			for (  i = 0; i < nSize; i++ ) 
			{
				strArr.Add((char *)sUserList.name[i]);                 
			}
		}
	}while(nNumberReturned == GUI_MAX_LIST_SIZE);

	return rcode;

	
}

int CUsersList::GetUserRecord(CString username)
{
	char strErrorMessage[200];
	 memcpy ((LPSTR)(sUserRecord.primary_key.name), username , sizeof (sUserRecord.primary_key.name));

	int rcode = txdsapi_get_record  ((pBYTE)&sUserRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	if(rcode != PTEMSG_OK)
		m_CntlStatus.SetWindowText(strErrorMessage);
	return rcode;

}

