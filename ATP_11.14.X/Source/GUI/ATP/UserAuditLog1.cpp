// UserAuditLog1.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "UserAuditLog1.h"
extern "C"
{
#include "ptetcp.h"
#include "ntutils.h"
#include "Txutils.h"
#include "ptetime.h"	
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_ACTION_PERMSN 4
#define MAX_SUBSYSTEMS    20

struct Subsytm_Prmsn
{
	char actin_status[30];
}Subsytm_Prmsn_01[20];

/////////////////////////////////////////////////////////////////////////////
// CUserAuditLog dialog


CUserAuditLog::CUserAuditLog(CWnd* pParent /*=NULL*/)
: CDialog(CUserAuditLog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserAuditLog)
	m_StrStatus = _T("");
	m_timeFromDate = 0;
	m_timeToDate = 0;
	m_szSelectedUser = _T("");
	//}}AFX_DATA_INIT
	m_pFile = NULL;
}


void CUserAuditLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserAuditLog)
	DDX_Control(pDX, IDC_USER_LIST, m_CntlUserList);
	DDX_Control(pDX, IDC_TO_DATE, m_CntlToDate);
	DDX_Control(pDX, IDC_FROM_DATE, m_CntlFromDate);
	DDX_Control(pDX, IDC_START, m_CntlStartButton);
	DDX_Control(pDX, IDCANCEL, m_CntlCancelButton);
	DDX_Control(pDX, IDC_CURRENT_STATUS, m_CntlStatus);
	DDX_Text(pDX, IDC_CURRENT_STATUS, m_StrStatus);
	DDX_DateTimeCtrl(pDX, IDC_FROM_DATE, m_timeFromDate);
	DDX_DateTimeCtrl(pDX, IDC_TO_DATE, m_timeToDate);
	DDX_CBString(pDX, IDC_USER_LIST, m_szSelectedUser);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserAuditLog, CDialog)
//{{AFX_MSG_MAP(CUserAuditLog)
ON_BN_CLICKED(IDC_START, OnStart)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserAuditLog message handlers

void CUserAuditLog::OnStart() 
{
	UpdateData();
	
	CStringArray strArr;
	GetTotalLogs(strArr);
}

void CUserAuditLog::Mask_Fields(pCHAR fields)
{
	int fieldslen = 0;
	int nIndex 	= 0;
	//memset(masked_fields ,0x00, sizeof(masked_fields));
	fieldslen = strlen( fields );
	if (fieldslen > 0)
	{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		//strncpy( masked_fields, fields, fieldslen);
		for ( nIndex = fieldslen-5; nIndex >= fieldslen-11; nIndex-- )
		{
			fields[nIndex] = '*';
		}
	}
}

int CUserAuditLog::GetTotalLogs(CStringArray &strArr)
{
	CString CstrTemp;
	CTime theTime = CTime::GetCurrentTime();
	m_lTotalRecords = 0;
	m_lRecordsWritten = 0;
	int rcode = 0;
	int nSize = 0;
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned = 0;
	char strErrorMessage[200] = {0};
	char strDate[30] = {0};
	char strTime[30] = {0};
    char szCurDir[200] = {0};
	char Prvs_access_data[50]={0};
	char Curnt_access_data[50]={0};
	char Access_Field_Tag[50]={0};
	int year,month,day,hr,min,sec = 0;
	char strtimeFromDate[20]={0};
	char strtimeToDate[20]={0};
	char strhistorytabletimeDate[20]={0};
	LONG  int_timeFromDate = 0;
	LONG  int_timeToDate = 0;
	LONG  int_historytabletimeToDate = 0; 
	int count = 0;
	FILE *log_fp= NULL;
	char buffer[256] = {0};
	char log_filename[100] = {0};
	char timestamp[40] = {0};
	CHAR ini_file[256] = {0};
	CHAR logenable_flag[5] = {0};
	//audit log issue
	CHAR last_uname[51] = {0};

	GetAscendentConfigDirectory( ini_file );
	strcat( ini_file, "tf.ini" );
	
	(void) GetPrivateProfileString(
        "USERAUDITLOG_PARAMETER", 	                   // points to section name 
        "ENABLE_AUDITLOG",	   // points to key name 
        "",					   // points to default string 
        logenable_flag,             // points to destination buffer 
        sizeof(logenable_flag) - 1, // size of destination buffer 
        ini_file );    

	if('1' == logenable_flag[0])
	{
		GetPinnacleLogDirectory( log_filename );
		strcat(log_filename,"UserAuditLog");
		ptetime_get_timestamp(timestamp);
		strcat(log_filename,timestamp);
		strcat(log_filename,".txt");
		log_fp = fopen( log_filename, "w+" );

		if(log_fp == NULL)
		{
			CreateDirectory("..//log",NULL);
			log_fp = fopen( log_filename, "w+" );
		}
	}


	year =0; month =0; day = 0;
	year = m_timeFromDate.GetYear();
	month = m_timeFromDate.GetMonth();
	day = m_timeFromDate.GetDay();
	sprintf((char *)strtimeFromDate,"%d%02d%02d",year,month,day); 
	int_timeFromDate= atoi(strtimeFromDate);
	
	year =0; month =0; day = 0;
	year = m_timeToDate.GetYear();
	month = m_timeToDate.GetMonth();
	day = m_timeToDate.GetDay();
	sprintf((char *)strtimeToDate,"%d%02d%02d",year,month,day);
	int_timeToDate= atoi(strtimeToDate);
	
	GetCurrentDirectory(200, szCurDir);
	memset(&sHistoryRecord, 0, sizeof(HST01));
	memset(&sHistoryList, 0, sizeof(HST01_LIST));
	sHistoryRecord.primary_key.unique_id[0] = '0';
	
	CFileDialog fd(FALSE, "csv",NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING , "Comma Seperated files (*.csv) | *.csv",this);
	if(fd.DoModal() == IDOK)
	{
		SetCurrentDirectory(szCurDir);
		CString filename = fd.GetPathName();
		m_pFile = fopen(filename, "w");
		if(m_pFile == NULL)
		{
			m_CntlStatus.SetWindowText("ERROR in opening the file");
			return -1;
		}
		fprintf(m_pFile,"REPORT NAME:, AUDIT LOG,\n");
		fprintf(m_pFile,"DATE:,%02d-%02d-%04d\n",theTime.GetMonth(), theTime.GetDay(), theTime.GetYear());
		//fprintf(m_pFile,"DATE, TIME, USER ID/ GROUPID, MAINTAINED BY, MODULE NAME, FIELD CHANGED, PREVIOUS DATA, CURRENT DATA, DATA STATUS, RECORD MODIFIED\n\n");
		
		fprintf(m_pFile,"DATE, TIME, USER NAME, USER ID, GROUPID, MAINTAINED BY, MODULE NAME, FIELD CHANGED, PREVIOUS DATA, CURRENT DATA, DATA STATUS, RECORD MODIFIED,"
						"SYSTEM ADMINISTRATION ,SYSTEM CONFIGURATION ,SYSTEM OPERATION ,BIN CONTROL , AUTHORIZATION CONTROL, MERCHANT CONTROL ,"
						"DEVICE CONTROL , NETWORK CONTROL ,CARD ACCOUNT CONTROL , VOICE AUTHORIZATION ,TRANSACTION LOOKUP ,DUAL ENCRYPTION,"
						"SERVICE MANAGER ,OVERRIDE MERCHANT DENIAL, ATP ARCHVING, SYSTEM GLOBAL PARAMETER, USER STATUS CONTROL\n\n");

		//fprintf(m_pFile,"date_modified, time_modified, changed_by,         ,file_name,   field_tag,     previous_data, current_data");	//8 fields
		//fprintf(m_pFile,"%s, %s, %s, ,%s,%s,%s,%s");
		
	}
	else
	{
		SetCurrentDirectory(szCurDir);
		return -2;
	}
	
	do
	{
		BeginWaitCursor();
		nNumberReturned = 0;
		unsigned char call_type = ST2_GET_HST01_BY_USER_NAME +1;
		if(m_szSelectedUser.CompareNoCase("All") == 0)
		{
			memset(sHistoryRecord.changed_by,0,sizeof(sHistoryRecord.changed_by));
			if (0 == strncmp((char *)strtimeFromDate, (char *)strtimeToDate, 8))
			{
				call_type = ST2_GET_HST01_BY_USER_NAME +3;
			}
			else
			{
				call_type = ST2_GET_HST01_BY_USER_NAME +1;
			}
		}
		else
		{
			strncpy((char *)sHistoryRecord.changed_by,m_szSelectedUser,sizeof(sHistoryRecord.changed_by));

			if (0 == strncmp((char *)strtimeFromDate, (char *)strtimeToDate, 8))
			{
				call_type = ST2_GET_HST01_BY_USER_NAME +4;
			}
			else
			{
				call_type = ST2_GET_HST01_BY_USER_NAME +2;
			}

		}
		memset(&sHistoryList, 0, sizeof(HST01_LIST));

		if(sHistoryRecord.primary_key.unique_id[0] == '0')
		{
			year =0; month =0; day = 0;
			year = m_timeFromDate.GetYear();
			month = m_timeFromDate.GetMonth();
			day = m_timeFromDate.GetDay();
			sprintf((char *)sHistoryRecord.primary_key.unique_id,"%d%02d%02d",year,month,day);
		}
		//audit log issue copy end date
		sprintf((char *)sHistoryRecord.reserved,"%d%02d%02d",m_timeToDate.GetYear(),m_timeToDate.GetMonth(),m_timeToDate.GetDay());

		if( ('1' == logenable_flag[0]) &&
			(0 == count  || ( count != 1 && 0 == count%2)))
		{
			memset(timestamp,0x00,sizeof(timestamp));
			ptetime_get_timestamp(timestamp);
			sprintf(buffer,"Request sent to server to pull records, count:%d, Time stamp:%s\n",count,timestamp);
			fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
		}
		rcode = txdsapi_get_list ((pBYTE)&sHistoryRecord, sizeof (HST01), 
			(pBYTE)&sHistoryList, sizeof (HST01_LIST),
			call_type , 
			HST01_DATA, strErrorMessage);

		if( ('1' == logenable_flag[0]) &&
			(0 == count  || ( count != 1 && 0 == count%2)))
		{
			memset(timestamp,0x00,sizeof(timestamp));
			ptetime_get_timestamp(timestamp);
			sprintf(buffer,"Received the record from server, count:%d, rcode:%d, Time stamp:%s \n",count,rcode,timestamp);
			fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
		}

		if (rcode == PTEMSG_OK) 
		{	
			nNumberReturned = atoi ((char *)&sHistoryList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
				CString strTemp = sHistoryList.history[nNumberReturned-1].primary_key.unique_id ;
				strTemp.TrimRight() ; strTemp.TrimLeft() ;
				memcpy ( &sHistoryRecord, &sHistoryList.history[nNumberReturned-1], sizeof (HST01));
				nSize = nNumberReturned - 1;
            }
			else if(nNumberReturned == 0 )
			{
				break;
			}
            else 
            {
				nSize = nNumberReturned;
            }

			memset(&last_uname, 0, sizeof(last_uname));
			for (  int i = 0; i < nSize; i++ ) 
			{
				strArr.Add((char *)sHistoryList.history[i].primary_key.unique_id);
				m_lTotalRecords++;
				sprintf(strErrorMessage,"Processing %d",m_lTotalRecords);
				m_CntlStatus.SetWindowText(strErrorMessage);
				
				//fprintf(m_pFile,"date_modified, time_modified, changed_by,         ,file_name,   field_tag,     previous_data, current_data");	//8 fields

				if(sHistoryList.history[i].changed_by[0] != '\0')
				{	
					year =0; month =0; day = 0;
					CString szDateModified = sHistoryList.history[i].date_modified;
					CString szTimeModified;
					if(!szDateModified.IsEmpty())
					{
						year =0; month =0; day = 0;
						year = atoi(szDateModified.Mid(0,4));
						month = atoi(szDateModified.Mid(4,2));
						day = atoi(szDateModified.Mid(6,2));
					}
		
					CTime modifiedtime(year,month,day,0,0,0);
					year =0; month =0; day = 0;
					year = m_timeFromDate.GetYear();
					month = m_timeFromDate.GetMonth();
					day = m_timeFromDate.GetDay();
					sprintf((char *)strhistorytabletimeDate,"%d%02d%02d",year,month,day); 
					int_historytabletimeToDate= atoi(strhistorytabletimeDate);

					if(int_historytabletimeToDate >= int_timeFromDate && int_historytabletimeToDate <= int_timeToDate)
					{
						char user_name[15]={0};
			
						CString str = sHistoryList.history[i].changed_by;
						
						//audit log issue
						if((strlen(last_uname) > 0 && strncmp((char*)sHistoryList.history[i].changed_by, last_uname, strlen(last_uname)) == 0) ||
						   strncmp((char*)sHistoryList.history[i].changed_by, "ATP", 3) == 0)
						{
							//same user no need to fetch again from UCF01
							rcode = PTEMSG_OK;

							//HCTS2, ATP updates the card status hence not required to fetch from UCF01.
							if(strncmp((char*)sHistoryList.history[i].changed_by, "ATP", 3) == 0)
							{
								memset(&m_sUserRecord, 0, sizeof(UCF01));
								memcpy((m_sUserRecord.primary_key.name), "ATP", 3);
							}
						}
						else
						{
							memset(&m_sUserRecord, 0, sizeof(UCF01));
							memcpy ((m_sUserRecord.primary_key.name), sHistoryList.history[i].changed_by , sizeof (m_sUserRecord.primary_key.name));
							memset(&last_uname, 0, sizeof(last_uname));
							memcpy(last_uname, sHistoryList.history[i].changed_by , sizeof (m_sUserRecord.primary_key.name));
							//DB call
							rcode = txdsapi_get_record  ((pBYTE)&m_sUserRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);	
						}
						
						if(rcode == PTEMSG_OK)
						{
							szDateModified = sHistoryList.history[i].date_modified;
							szTimeModified = sHistoryList.history[i].time_modified;
							year = month = day = hr = min = sec = 0;
							if(!szDateModified.IsEmpty())
							{
								year = atoi(szDateModified.Mid(0,4));
								month = atoi(szDateModified.Mid(4,2));
								day = atoi(szDateModified.Mid(6,2));
							}
							if(!szTimeModified.IsEmpty())
							{
								hr = atoi(szTimeModified.Mid(0,2));
								min = atoi(szTimeModified.Mid(2,2));
								sec = atoi(szTimeModified.Mid(4,2));
							}
							
							strncpy(user_name,(char *)m_sUserRecord.user_info_1,strlen((char *)m_sUserRecord.user_info_1));
							strncat(user_name,(char *)m_sUserRecord.user_info_2,strlen((char *)m_sUserRecord.user_info_2));
							user_name[strlen(user_name)]='\0';
						

							if( (strcmp((char *)sHistoryList.history[i].current_data,"Record Added") == 0) )
							{
								Expand_Permns_from_num_to_strings();
								fprintf(m_pFile,"%02d-%02d-%04d, %02d:%02d:%02d, %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s"
										,month,day,year,
										hr,min,sec,
										user_name,
										m_sUserRecord.primary_key.name,
										m_sUserRecord.group_id,
										sHistoryList.history[i].changed_by,
										sHistoryList.history[i].file_name,
										sHistoryList.history[i].field_tag,
										sHistoryList.history[i].previous_data,
										"",
										"Record Added",
										sHistoryList.history[i].file_primary_key,
										Subsytm_Prmsn_01[0].actin_status,
										Subsytm_Prmsn_01[1].actin_status,
										Subsytm_Prmsn_01[2].actin_status,
										Subsytm_Prmsn_01[3].actin_status,
										Subsytm_Prmsn_01[4].actin_status,
										Subsytm_Prmsn_01[5].actin_status,
										Subsytm_Prmsn_01[6].actin_status,
										Subsytm_Prmsn_01[7].actin_status,
										Subsytm_Prmsn_01[8].actin_status,
										Subsytm_Prmsn_01[9].actin_status,
										Subsytm_Prmsn_01[10].actin_status,
										Subsytm_Prmsn_01[11].actin_status,
										Subsytm_Prmsn_01[13].actin_status,
										Subsytm_Prmsn_01[14].actin_status,
										Subsytm_Prmsn_01[15].actin_status,
										Subsytm_Prmsn_01[16].actin_status,
										Subsytm_Prmsn_01[17].actin_status);
								fprintf(m_pFile,"\n");
								m_lRecordsWritten++;
							}
							else if((strcmp((char *)sHistoryList.history[i].current_data,"Record Deleted") == 0) )
							{
								Expand_Permns_from_num_to_strings();
								fprintf(m_pFile,"%02d-%02d-%04d, %02d:%02d:%02d, %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s"
										,month,day,year,
										hr,min,sec,
										user_name,
										m_sUserRecord.primary_key.name,
										m_sUserRecord.group_id,
										sHistoryList.history[i].changed_by,
										sHistoryList.history[i].file_name,
										sHistoryList.history[i].field_tag,
										sHistoryList.history[i].previous_data,
										"",
										"Record Deleted",
										sHistoryList.history[i].file_primary_key,
										Subsytm_Prmsn_01[0].actin_status,
										Subsytm_Prmsn_01[1].actin_status,
										Subsytm_Prmsn_01[2].actin_status,
										Subsytm_Prmsn_01[3].actin_status,
										Subsytm_Prmsn_01[4].actin_status,
										Subsytm_Prmsn_01[5].actin_status,
										Subsytm_Prmsn_01[6].actin_status,
										Subsytm_Prmsn_01[7].actin_status,
										Subsytm_Prmsn_01[8].actin_status,
										Subsytm_Prmsn_01[9].actin_status,
										Subsytm_Prmsn_01[10].actin_status,
										Subsytm_Prmsn_01[11].actin_status,
										Subsytm_Prmsn_01[13].actin_status,
										Subsytm_Prmsn_01[14].actin_status,
										Subsytm_Prmsn_01[15].actin_status,
										Subsytm_Prmsn_01[16].actin_status,
										Subsytm_Prmsn_01[17].actin_status);
								fprintf(m_pFile,"\n");
								m_lRecordsWritten++;
							}
 							else
							{
								if(strncmp((char*)sHistoryList.history[i].changed_by, "ATP", 3) == 0)
									memset(Subsytm_Prmsn_01,0x00,sizeof(Subsytm_Prmsn_01));
								else
									Expand_Permns_from_num_to_strings();
								memset(Curnt_access_data,0x00,sizeof(Curnt_access_data));
								memset(Prvs_access_data,0x00,sizeof(Prvs_access_data));	
								if(strstr((char *)sHistoryList.history[i].field_tag,"Permission"))
								{
									int check=0; // condtion check variable
									if(sHistoryList.history[i].previous_data[0]=='1')
									{
										strncpy(Prvs_access_data,"Add",4);
										Prvs_access_data[strlen(Prvs_access_data)]='_';
										check=1;
									}
									if(sHistoryList.history[i].previous_data[1]=='1')
									{
										strncat(Prvs_access_data,"Delete",6);
										Prvs_access_data[strlen(Prvs_access_data)]='_';
										check=1;
									}
									if(sHistoryList.history[i].previous_data[2]=='1')
									{
										strncat(Prvs_access_data,"Update",6);
										Prvs_access_data[strlen(Prvs_access_data)]='_';
										check=1;
									}
									if(sHistoryList.history[i].previous_data[3]=='1')
									{
										strncat(Prvs_access_data,"View",4);
										check=1;
									}
									if(check==1)
									{
										int temp_len=strlen(Prvs_access_data);
										strcpy(Prvs_access_data+temp_len," Only");
										Prvs_access_data[strlen(Prvs_access_data)]='\0';
										check=0;
									}


									if(sHistoryList.history[i].current_data[0]=='1')
									{
										strncpy(Curnt_access_data,"Add",4);
										Curnt_access_data[strlen(Curnt_access_data)]='_';
										check=1;
									}
									if(sHistoryList.history[i].current_data[1]=='1')
									{
										strncat(Curnt_access_data,"Delete",6);
										Curnt_access_data[strlen(Curnt_access_data)]='_';
										check=1;
									}
									if(sHistoryList.history[i].current_data[2]=='1')
									{
										strncat(Curnt_access_data,"Update",6);
										Curnt_access_data[strlen(Curnt_access_data)]='_';
										check=1;
									}
									if(sHistoryList.history[i].current_data[3]=='1')
									{
										strncat(Curnt_access_data,"View",4);
										check=1;
									}
									if(check==1)
									{
										int temp_len=strlen(Curnt_access_data);
										strcpy(Curnt_access_data+temp_len," Only");
										Curnt_access_data[strlen(Curnt_access_data)]='\0';
										check=0;
									}
								}
								else
								{
									strcpy(Curnt_access_data,(char *)sHistoryList.history[i].current_data);
									strcpy(Prvs_access_data,(char *)sHistoryList.history[i].previous_data);
							
									if((strcmp((char *)sHistoryList.history[i].file_name,"PAF01") == 0) )
									{
										if((strcmp((char *)sHistoryList.history[i].field_tag,"Name") == 0) ||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"Account Name") == 0) )
										{
											if((char *)sHistoryList.history[i].previous_data[0]!='\0')
											{
												Mask_Fields(Prvs_access_data);
											}
											if((char *)sHistoryList.history[i].current_data[0]!='\0')
											{
												Mask_Fields(Curnt_access_data);
											}
										}
									}
									else if((strcmp((char *)sHistoryList.history[i].file_name,"CCF02P") == 0) )
									{
										if((strcmp((char *)sHistoryList.history[i].field_tag,"Name") == 0) 			  ||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"Primary Acc Nbr") == 0) ||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"Bal.Acct.Nbr") == 0) )
										{
											if((char *)sHistoryList.history[i].previous_data[0]!='\0')
											{
												Mask_Fields(Prvs_access_data);
											}
											if((char *)sHistoryList.history[i].current_data[0]!='\0')
											{
												Mask_Fields(Curnt_access_data);
										    }
										}
									}
								
									else if((strcmp((char *)sHistoryList.history[i].file_name,"NCF01") == 0) )
									{
										if((strcmp((char *)sHistoryList.history[i].field_tag,"ZMK") == 0) ||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"CWK") == 0) ||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"KEK") == 0) ||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"Name") == 0))
										{
											if((char *)sHistoryList.history[i].previous_data[0]!='\0')
											{
												Mask_Fields(Prvs_access_data);
											}
											if((char *)sHistoryList.history[i].current_data[0]!='\0')
											{
												Mask_Fields(Curnt_access_data);
											}
										}
									}
									else if((strcmp((char *)sHistoryList.history[i].file_name,"CAF01") == 0) )
									{
										if((strcmp((char *)sHistoryList.history[i].field_tag,"MKAC") == 0) 				||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"3D_CAK") == 0) 			||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"3DS-2_Code") == 0) 		||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"Extd 3ds2.0 Key") == 0) 	||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"MC_CAK") == 0) 			||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"CVK A") == 0) 			||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"CVK B") == 0) 			||
										   (strcmp((char *)sHistoryList.history[i].field_tag,"Name") == 0))
										{
											if((char *)sHistoryList.history[i].previous_data[0]!='\0')
											{
												Mask_Fields(Prvs_access_data);
											}
											if((char *)sHistoryList.history[i].current_data[0]!='\0')
											{
												Mask_Fields(Curnt_access_data);
											}
										}
									}
								}
								fprintf(m_pFile,"%02d-%02d-%04d, %02d:%02d:%02d, %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s"
										,month,day,year,
										hr,min,sec,
										user_name,
										m_sUserRecord.primary_key.name,
										m_sUserRecord.group_id,
										sHistoryList.history[i].changed_by,
										sHistoryList.history[i].file_name,
										sHistoryList.history[i].field_tag,
										Prvs_access_data,
										Curnt_access_data,
										"Record Updated",
										sHistoryList.history[i].file_primary_key,
										Subsytm_Prmsn_01[0].actin_status,
										Subsytm_Prmsn_01[1].actin_status,
										Subsytm_Prmsn_01[2].actin_status,
										Subsytm_Prmsn_01[3].actin_status,
										Subsytm_Prmsn_01[4].actin_status,
										Subsytm_Prmsn_01[5].actin_status,
										Subsytm_Prmsn_01[6].actin_status,
										Subsytm_Prmsn_01[7].actin_status,
										Subsytm_Prmsn_01[8].actin_status,
										Subsytm_Prmsn_01[9].actin_status,
										Subsytm_Prmsn_01[10].actin_status,
										Subsytm_Prmsn_01[11].actin_status,
										Subsytm_Prmsn_01[13].actin_status,
										Subsytm_Prmsn_01[14].actin_status,
										Subsytm_Prmsn_01[15].actin_status,
										Subsytm_Prmsn_01[16].actin_status,
										Subsytm_Prmsn_01[17].actin_status);
								fprintf(m_pFile,"\n");
								m_lRecordsWritten++;
							}
						}
					}
				}
			}
			if( ('1' == logenable_flag[0]) &&
				(count != 1 && count != 0 && 0 == count%2) )
			{
				memset(timestamp,0x00,sizeof(timestamp));
				ptetime_get_timestamp(timestamp);
				sprintf(buffer,"Records are written into CSV file, count:%d, Time stamp:%s \n",count,timestamp);
				fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
			}
			count++;
		}
		else
		{
			nNumberReturned = 0;
		}
	}
	while(nNumberReturned == GUI_MAX_LIST_SIZE);
	sprintf(strErrorMessage,"%ld records processed",m_lTotalRecords);
	m_CntlStatus.SetWindowText(strErrorMessage);
	fprintf(m_pFile,"\n\nTOTAL RECORDS:,%lu,\n",m_lRecordsWritten);
	fclose(m_pFile);
	sprintf(strErrorMessage,"Audit log report completed. %ld records written",m_lRecordsWritten);
	m_CntlStatus.SetWindowText(strErrorMessage);
	EndWaitCursor();
	m_CntlCancelButton.SetWindowText("Close");
	if(('1' == logenable_flag[0]))
	{
		if(count%2 != 0)
		{
			memset(timestamp,0x00,sizeof(timestamp));
			ptetime_get_timestamp(timestamp);
			sprintf(buffer,"Records are written into CSV file, count:%d, Time stamp:%s \n",count,timestamp);
			fwrite( buffer, sizeof( CHAR ), strlen( buffer ), log_fp );
		}
		fclose(log_fp);
	}
	return rcode;
}

int CUserAuditLog::GetUserRecord(CString username)
{
	char strErrorMessage[300];
	int rcode = -1;
	if(!username.IsEmpty())
	{
		memcpy ((m_sUserRecord.primary_key.name), username , sizeof (m_sUserRecord.primary_key.name));
		
		rcode = txdsapi_get_record  ((pBYTE)&m_sUserRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);
		if(rcode != PTEMSG_OK)
			m_CntlStatus.SetWindowText(strErrorMessage);
	}
	return rcode;
	
}

BOOL CUserAuditLog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_CntlStatus.SetWindowText("Loading users list... Please Wait...");
	
	m_CntlUserList.AddString("All");
	m_CntlUserList.SetCurSel(0);
	LoadUsers();

	m_CntlStatus.SetWindowText("Ready");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CUserAuditLog::LoadUsers()
{
	UCF01_LIST sUserList;
	UCF01 sUserRecord;
	int rcode;
	int nSize;
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
	char strErrorMessage[200];
    char strDate[30] = "";
	char strTime[30] = "";
    char szCurDir[200] = {0};
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
				memcpy( &sUserRecord.primary_key.name, (LPCTSTR) strTemp, sizeof (sUserRecord.primary_key.name)); 
				nSize = nNumberReturned - 1;			
			}
			else
				nSize = nNumberReturned;			
			
			
			for ( int i = 0; i < nSize; i++ ) {
				m_CntlUserList.AddString((char *)sUserList.name[i]);                 
			}
		}
	}while(nNumberReturned == GUI_MAX_LIST_SIZE);
	
	return TRUE;
}

void CUserAuditLog::Expand_Permns_from_num_to_strings()
{
	int check=0; // condtion check variable
	memset(Subsytm_Prmsn_01,0x00,sizeof(Subsytm_Prmsn_01));
	for(int k=0;k<MAX_SUBSYSTEMS;k++)
	{
		if(strlen((char *)m_sUserRecord.control[k].action)==NULL)
		{
			continue;
		}
		else
		{
			if(m_sUserRecord.control[k].action[0]=='1')
			{
				strncpy(Subsytm_Prmsn_01[k].actin_status,"Add",4);
				Subsytm_Prmsn_01[k].actin_status[strlen(Subsytm_Prmsn_01[k].actin_status)]='_';
				check=1;
			}
			if(m_sUserRecord.control[k].action[1]=='1')
			{
				strncat(Subsytm_Prmsn_01[k].actin_status,"Delete",6);
				Subsytm_Prmsn_01[k].actin_status[strlen(Subsytm_Prmsn_01[k].actin_status)]='_';
				check=1;
			}
			if(m_sUserRecord.control[k].action[2]=='1')
			{
				strncat(Subsytm_Prmsn_01[k].actin_status,"Update",6);
				Subsytm_Prmsn_01[k].actin_status[strlen(Subsytm_Prmsn_01[k].actin_status)]='_';
				check=1;
			}
			if(m_sUserRecord.control[k].action[3]=='1')
			{
				strncat(Subsytm_Prmsn_01[k].actin_status,"View",4);
				check=1;
			}
			if(check==1)
			{
				int temp_len=strlen(Subsytm_Prmsn_01[k].actin_status);
				strcpy(Subsytm_Prmsn_01[k].actin_status+temp_len," Only");
				Subsytm_Prmsn_01[k].actin_status[strlen(Subsytm_Prmsn_01[k].actin_status)]='\0';
			}
		}
		
	}
}
