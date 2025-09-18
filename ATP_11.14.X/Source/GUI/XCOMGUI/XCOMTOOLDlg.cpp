// XCOMTOOLDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XCOMTOOL.h"
#include "XCOMTOOLDlg.h"
//#include "Windows.h"
#include "XCOMGUI_CMDS.h"
#include "RefreshWnd.h"
#include "Createwnd.h"
#include "DeleteWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef  __cplusplus
extern "C" {
#endif

	#include "ntutils.h"
	#include "pte.h"
	#include "ptetcp.h"
	#define COMM_BUF_SZE    1500
	#define DEVICE_OK		1
	#define ISO_PARSE_ERROR 21
	#define MAX_NAME_LENGTH 25
	#define MAX_USERS		100
	#define MAX_QUEUES		300
	#define MAX_SHM_TABLES  30
	#define MAX_SHM_MEMORY	5120000
	#define MAX_RECV_TIMEOUT_VAL 30
	#define QUENAME "xsvrtstA"

	extern "C"
	{
		//Krishna@TF, 10.6.11,: This Variable contains the Version no of the App
	   CHAR appProductVersion[MAX_APPVERSION_SIZE];
	}

	CHAR srvcmgr_que_name[] = QUENAME;
    CHAR error_msg[100];
    LONG time_out = MAX_RECV_TIMEOUT_VAL;

	CHAR Que_Name[10];
	CHAR Que_ID[4];
	CHAR User_Name[10];
	CHAR User_ID[4];
	
	int NoBytes,NoMsgs;

	CHAR Mem_Name[10];
	CHAR Mem_ID[4];

	//pCHAR List_Box_buffer = NULL_PTR;
	CHAR buffer[100];
	BOOL User_Update_f;
	BOOL Que_Update_f;
	BOOL Mem_Update_f;

	CListBox q_list;
	CListBox mem_list;
	CListBox user_list;	

	int user_cntr;
	int que_cntr;
    int mem_cntr;

	long sizeof_shm_seg;

	BOOL Flush_f;

	BOOL pattern_match;
	int search_idx;

	#ifdef WIN32
	   HANDLE   lhandle;
	   DWORD    lthread_id;
	#else
	   static   pthread_t tcp_timer_thread;
	#endif

	INT  login_failed;

	struct USERINFO
	{
		 char u_name[MAX_NAME_LENGTH];
		 int userID;
		 int isWaitingForMsg;
	};

	struct QUEINFO
	{
		char q_name[MAX_NAME_LENGTH];
		int totalMessages;
		int totalDataBytes;
		int totalAllowedMsgs;	// 0 means unlimited
		int totalAllowedData;	// 0 means unlimited
		int queueID;
	};

	struct MEMINFO
	{
		char mem_name[MAX_NAME_LENGTH];
		int tableId;
		int sizeofDataCurrentlyWritten;
		long maxTableSize;
	};

	struct XCOMSERVER
	{
		struct USERINFO u_info[MAX_USERS];
		struct QUEINFO  q_info[MAX_QUEUES];
		struct MEMINFO  mem_info[MAX_SHM_TABLES];
	};
	struct XCOMSERVER xsvrusr, xsvrusr1;
	
	char recv_buffer[sizeof(struct XCOMSERVER)] = {0};

	char tmp1[10]={0};
	char tmp2[10]={0};
	char tmp3[10]={0};
	char tmp4[10]={0};
	char tmp5[10]={0};

	CButton  *m_button_config;
	CButton  *m_button_create;
	CButton  *m_button_read;
	CButton  *m_button_write;
	CButton  *m_button_delete;
	CButton  *m_button_flush;
	CButton  *m_button_stop;

	void Enable_Disable_Buttons(BOOL Status)
	{
		m_button_config->EnableWindow(Status);
		m_button_create->EnableWindow(Status);
		m_button_read->EnableWindow(Status);
		m_button_write->EnableWindow(Status);
		m_button_delete->EnableWindow(Status);
		m_button_flush->EnableWindow(Status);	
		m_button_stop->EnableWindow(Status);
	}

	int Send_Msg(int CmdID, int SubID)
	{
		pPTE_MSG    p_msg_out;
		CHAR		temp_str[100];
		INT         rcode;

		CHAR temp[256] = "";
		CHAR Frame_Buffer[256] = "";
		BOOL Tx_f = FALSE;
		int i = 0, j = 0, cnt = 0;

		int user_sel_idx;
		int q_sel_idx;
		int mem_sel_idx;
		
		switch(CmdID)
		{
			case  MT_XCOM_GUI_LOGIN:
							  strcpy(temp,"SENDING LOGIN COMMAND");
							  Tx_f = TRUE;
			break;

			case  MT_XCOM_GUI_LOGOUT:
							  strcpy(temp,"SENDING LOGOUT COMMAND");
							  Tx_f = TRUE;
			break;

			case  MT_XCOM_GUI_USER:
								  switch(SubID)
								  {
									  case  ST1_XCOM_GUI_USER_DELETE:

									  			user_sel_idx = user_list.GetCurSel();
												user_list.GetText(user_sel_idx, Frame_Buffer);

												 cnt = i = j = 0;
												 memset(&temp, 0, sizeof(temp));
												 memset(&User_ID, 0, sizeof(User_ID));
												 memset(&User_Name, 0, sizeof(User_Name));

												  //Parse the string for Qid & Name
												  for(i=0; Frame_Buffer[i]!='\0' || i <20 ; i++)
												  {
													  if(Frame_Buffer[i] == '\t') 
													  {
														  cnt++; 
														  j = 0; 
														  continue; 
													  }

													  switch(cnt)
													  {
															case 0 : User_ID[j++] = Frame_Buffer[i];
																break;
															case 1 : User_Name[j++] = Frame_Buffer[i];
																break;
													  }
												}
												sprintf(temp, "%d,%s,",atoi(User_ID), User_Name);

												Tx_f = TRUE;
									  break;
								  }
			break;

			case  MT_XCOM_GUI_QUE:
								 switch(SubID)
								 {
									 	case  ST1_XCOM_GUI_QUE_CREATE:
														
														sprintf(temp, "%s,%d,%d,",Que_Name, NoBytes, NoMsgs);
														Tx_f = TRUE;
										break;

										case  ST1_XCOM_GUI_QUE_DELETE:
														  //temp = "SENDING XCOM_GUI_QUE_DELETE COMMAND";
														  
														  //Get the Selected Index String 
														  q_sel_idx = q_list.GetCurSel();
														  q_list.GetText(q_sel_idx,Frame_Buffer);
														  
														  cnt = i = j = 0;
														  memset(&temp, 0, sizeof(temp));
														  memset(&Que_ID, 0, sizeof(Que_ID));
														  memset(&Que_Name, 0, sizeof(Que_Name));

														  //Parse the string for Qid & Name
														  for(i=0; Frame_Buffer[i]!='\0' || i <20 ; i++)
														  {
															  if(Frame_Buffer[i] == '\t') 
															  { 
																  cnt++; 
																  j = 0; 
																  continue; 
															  }

															  switch(cnt)
															  {
																case 0 : Que_ID[j++] = Frame_Buffer[i];
																	break;
																case 1 : Que_Name[j++] = Frame_Buffer[i];
																	break;
															  }
														  }
														  sprintf(temp, "%s,%s,",Que_ID, Que_Name);
														  
														  Tx_f = TRUE;
										break;

										case  ST1_XCOM_GUI_QUE_READ:
														  strcpy(temp,"SENDING XCOM_GUI_QUE_READ COMMAND");
														  Tx_f = TRUE;
										break;

										case  ST1_XCOM_GUI_QUE_WRITE:
														  strcpy(temp,"SENDING XCOM_GUI_QUE_WRITE COMMAND");
														  Tx_f = TRUE;
										break;
								 }
			break;

			case  MT_XCOM_GUI_MEM:
								switch(SubID)
								{
										case  ST1_XCOM_GUI_MEM_CREATE:
														  //strcpy(temp,"SENDING XCOM_GUI_MEM_CREATE COMMAND");
														  sprintf(temp, "%s,%d,",Mem_Name, NoBytes);														  
														  Tx_f = TRUE;
										break;

										case  ST1_XCOM_GUI_MEM_DELETE:
														  //strcpy(temp,"SENDING XCOM_GUI_MEM_DELETE COMMAND");
														   mem_sel_idx = mem_list.GetCurSel();
														   mem_list.GetText(mem_sel_idx,Frame_Buffer);
														  
														  cnt = i = j = 0;
														  memset(&temp, 0, sizeof(temp));
														  memset(&Mem_ID, 0, sizeof(Mem_ID));
														  memset(&Mem_Name, 0, sizeof(Mem_Name));

														  //Parse the string for Qid & Name
														  for(i=0; Frame_Buffer[i]!='\0' || i <20 ; i++)
														  {
															  if(Frame_Buffer[i] == '\t') { cnt++; j = 0; continue; }

															  switch(cnt)
															  {
																case 0 : Mem_ID[j++] = Frame_Buffer[i];
																	break;
																case 1 : Mem_Name[j++] = Frame_Buffer[i];
																	break;
															  }

														  }
														  sprintf(temp, "%s,%s,",Mem_ID, Mem_Name);
														  Tx_f = TRUE;
										break;

										case  ST1_XCOM_GUI_MEM_READ:
														  strcpy(temp,"SENDING XCOM_GUI_MEM_READ COMMAND");
														  Tx_f = TRUE;
										break;

										case  ST1_XCOM_GUI_MEM_WRITE:
														  strcpy(temp,"SENDING XCOM_GUI_MEM_WRITE COMMAND");
														  Tx_f = TRUE;
										break;
								}
			break;			

			case MT_XCOM_GUI_STATUS_UPDATE :

				strcpy(temp, "Request XCOM_GUI_STATUS_UPDATE");

				Tx_f = TRUE;

				break;

			case MT_XCOM_GUI_FLUSH : 
				
				strcpy(temp, "sending MT_XCOM_GUI_FLUSH ");

				Tx_f = TRUE;

				break;
			
			default : break;
			
		}		

		if(Tx_f)
		{
		   p_msg_out   = NULL_PTR;
		   error_msg[0]= NULL_PTR;

		   // build the outgoing message
			p_msg_out = ptemsg_build_msg( CmdID,
										  SubID,
										  0,
										  srvcmgr_que_name,
										  interactive_que_name,
										  (pBYTE)temp,
										  strlen(temp),
										  0 );

			if( p_msg_out == NULL_PTR )
			{
				strcpy( error_msg, "Insufficient memory to build request message" );
				return PTEMSG_INSUFFICIENT_MEMORY;
			}

			// try to send the request
			rcode = ptetcp_send( p_msg_out, temp_str );

			free( p_msg_out );

			if( !rcode )
			{
				sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
				return PTEMSG_IPC_ERROR;
			}

			return 1;
		}

		return 0;
	}

	BYTE Copy_Message_Data_To_Global_Buffer (pCHAR indata, LONG msglen)
	{
	   BYTE  retval = DEVICE_OK;
	   BYTE   ReqRsp_Buffer[COMM_BUF_SZE];
	   int buflen, reqidx;

	   memset( ReqRsp_Buffer, 0x00, COMM_BUF_SZE );
	   if ( (0 < msglen) && (msglen < COMM_BUF_SZE) )
	   {
		  buflen = msglen;
		  reqidx = 0; 
		  memcpy( ReqRsp_Buffer, indata, msglen );
	   }
	   else
	   {
		  retval = ISO_PARSE_ERROR;
		  if ( msglen >= COMM_BUF_SZE )
		  {
			 /* Fill the ReqRsp_Buffer with as much of the message as
			  * we can so it can be stored in the trace file. End the
			  * buffer with three dots (...) so we know it is too large.
			  */
			 buflen = COMM_BUF_SZE - 1;
			 memcpy( ReqRsp_Buffer, indata, buflen - 3 );
			 memset( ReqRsp_Buffer+buflen-3, '.', 3 );
		  }
		  else
		  {
			 if ( msglen == 0 )
				strcpy( (char *)ReqRsp_Buffer, "Zero length message" );

			 else /* msglen is negative */
				strcpy( (char *)ReqRsp_Buffer, "Negative length message" );

			 /* This variable is used when logging the message to a trace file. */
			 buflen = strlen( (char *)ReqRsp_Buffer );
		  }
		}
		return( retval );
	} /* Copy_Message_Data_To_Global_Buffer */


	//int Recv_Msg()
	int Recv_Msg(HWND h)	
	{
		pPTE_MSG   p_msg_in;
		CHAR	error_str[100];
		pBYTE   p_data = NULL_PTR ;
		pPTE_MSG_DATA   p_msg_data = NULL_PTR;
		pCHAR   p_rec = NULL_PTR ;
		BYTE    retCode = 0 ;
		LONG    dataLength = 0L ;
		int msgtyp, submsgtyp;

		int i=0,j=0,cnt=0;

	   CHAR Recvd_Str[256]="";
	   CHAR CmdID[10]="";
	   CHAR SubID[4]="";
	   CHAR ID[4]="";
	   CHAR Resp_Str[256]="";
	   
	   int comma_cnt = 0;
	   int len = 0;

	   int user_cntr_1;
	   int que_cntr_1;
       int mem_cntr_1;
	  
	   char temp[256] = {0};

	    memset(&p_msg_in, 0, sizeof(pPTE_MSG));
		p_msg_in = ptetcp_receive( time_out, error_str );
		
		msgtyp = ptemsg_get_msg_type( p_msg_in) ;
		submsgtyp = ptemsg_get_msg_subtype1( p_msg_in) ;
	
		if( p_msg_in == NULL_PTR )
		{
			sprintf( error_msg, "Communication Error during request to ServiceManager: %s", error_str );
			AfxMessageBox(error_msg);
			
			Send_Msg(MT_XCOM_GUI_LOGIN, ST1_XCOM_GUI_RELOGIN);
			
			return PTEMSG_IPC_ERROR;
		}
		else
		{
			  memset(&p_data, 0, sizeof(p_data));
			  memset(&p_rec, 0, sizeof(p_rec));
			  memset(&p_msg_data, 0, sizeof(p_msg_data));

			  p_msg_data = ptemsg_get_pte_msg_data (p_msg_in);
			  p_data     = ptemsg_get_pte_msg_data_data (p_msg_data);
			  p_rec      = (pCHAR) p_data;

			  /* Copy the data to ReqRsp_Buffer */
			  dataLength = ptemsg_get_pte_msg_data_data_length( p_msg_data );
			  retCode    = Copy_Message_Data_To_Global_Buffer( p_rec, dataLength );
			 
			switch(msgtyp)
			{
				case MT_XCOM_GUI_ACK_LOGIN :

							memset(&Recvd_Str, 0, sizeof(Recvd_Str));
							strncpy(Recvd_Str, p_rec, dataLength);

							sizeof_shm_seg = atol(Recvd_Str);

				break;

				case MT_XCOM_GUI_ACK_QUE : 
										memset(&Recvd_Str, 0, sizeof(Recvd_Str));
										strncpy(Recvd_Str, p_rec, dataLength);

										switch(submsgtyp)
										{
											case ST1_XCOM_GUI_ACK_QUE_CREATE: 

												/*if(atoi(Recvd_Str)>0) 
												  {
													  sprintf(buffer, "%03d    %s    %d    %d", atoi(Recvd_Str), Que_Name, NoBytes, NoMsgs);
													  //Que_Update_f = TRUE;
													  List_Box_buffer = buffer;
													  q_list.AddString(List_Box_buffer);
												  }
												  */
											break;

											case ST1_XCOM_GUI_ACK_QUE_DELETE :  

												//if(atoi(Recvd_Str)> -1 ) 
												//q_list.DeleteString(q_sel_idx);
												
											break;
										}
				break;

				case MT_XCOM_GUI_ACK_MEM :

										memset(&Recvd_Str, 0, sizeof(Recvd_Str));
										strncpy(Recvd_Str, p_rec, dataLength);

									   	switch(submsgtyp)
										{
											case ST1_XCOM_GUI_ACK_MEM_CREATE: 

											/*	if(atoi(Recvd_Str)>0) 
												  {
													  sprintf(buffer, "%03d    %s    %d", atoi(Recvd_Str), Mem_Name, NoBytes);
													  //Que_Update_f = TRUE;
													  List_Box_buffer = buffer;
													  mem_list.AddString(List_Box_buffer);
												  }
											*/
											break;

											case ST1_XCOM_GUI_ACK_MEM_DELETE :  

												//if(atoi(Recvd_Str)> -1 ) 
												//mem_list.DeleteString(mem_sel_idx);
												
											break;
										}

									   
				break;

				case MT_XCOM_GUI_STATUS_UPDATE :

									memset(&xsvrusr, 0, sizeof(struct XCOMSERVER));
									memset(&xsvrusr1, 0, sizeof(struct XCOMSERVER));									
									memset(&recv_buffer, 0, sizeof(recv_buffer));
									//memcpy(&xsvrusr, (XCOMSERVER *)p_data, sizeof(struct XCOMSERVER));

									strncpy(recv_buffer, (pCHAR)p_data, dataLength);

									len=strlen(recv_buffer);

									cnt = 0;
									comma_cnt=0;

								    user_cntr_1 = 0;
								    que_cntr_1 = 0;
								    mem_cntr_1 = 0;

									memset(&tmp1, 0 , sizeof(tmp1));
									memset(&tmp2, 0 , sizeof(tmp2));
									memset(&tmp3, 0 , sizeof(tmp3));
									memset(&tmp4, 0 , sizeof(tmp4));
									memset(&tmp5, 0 , sizeof(tmp5));						


									for(i=0;i < len;i++)
									{
										if(recv_buffer[i] == '^')  
										{
											cnt++; 
											comma_cnt=0; 
											continue;
										}
										
										switch(cnt)
										{
											case 1 :  
											{
													 if(recv_buffer[i] == 'U' && recv_buffer[i+1] == '|')
													 {
														  i++; 
														  j=0;
														  memset(&tmp1, 0 , sizeof(tmp1));
														
														  continue;
													 }
													  else if(recv_buffer[i] == '!') break;

													  else if(recv_buffer[i] == ';') 
													  { 
														  comma_cnt = 0; 
														  j=0;

														  xsvrusr.u_info[user_cntr_1].isWaitingForMsg = atoi(tmp1);
														  xsvrusr.u_info[user_cntr_1].userID = atoi(tmp2);

														  memset(&tmp1, 0 , sizeof(tmp1));	
														  memset(&tmp2, 0 , sizeof(tmp2));	
														  
														  //V IMP. Increment User Cnt for every ';' ocurance
														  user_cntr_1++; 

														  continue; 
													  }
													  else if(recv_buffer[i] == ',')
													  {
														  comma_cnt++;
														  j=0;
														  							
														  continue;
													  }

													  switch(comma_cnt)
													  {

														  case 0 : xsvrusr.u_info[user_cntr_1].u_name[j] = recv_buffer[i];
																   j++;
																   break;
														  case 1 : tmp1[j++] = recv_buffer[i];
															  	   break;
														  case 2 : tmp2[j++] = recv_buffer[i];
																   break;

														  default : break;
													  }

											}// End of case 1
											break;
											
											case 2 :
											{
													if(recv_buffer[i] == 'Q' && recv_buffer[i+1] == '|')
													 {
														  i++; 
														  j=0;
														  
														  memset(&tmp1, 0 , sizeof(tmp1));
														  memset(&tmp2, 0 , sizeof(tmp2));
														  memset(&tmp3, 0 , sizeof(tmp3));
														  memset(&tmp4, 0 , sizeof(tmp4));
														  memset(&tmp5, 0 , sizeof(tmp5));

														  continue;
													 }
													  else if(recv_buffer[i] == '!') break;

													  else if(recv_buffer[i] == ';') 
													  { 
														  comma_cnt = 0; 
														  j=0;

														  xsvrusr.q_info[que_cntr_1].queueID = atoi(tmp1);
														  xsvrusr.q_info[que_cntr_1].totalMessages = atoi(tmp2);
														  xsvrusr.q_info[que_cntr_1].totalDataBytes = atoi(tmp3);
														  xsvrusr.q_info[que_cntr_1].totalAllowedMsgs = atoi(tmp4);
														  xsvrusr.q_info[que_cntr_1].totalAllowedData = atoi(tmp5);

														  memset(&tmp1, 0 , sizeof(tmp1));
														  memset(&tmp2, 0 , sizeof(tmp2));
														  memset(&tmp3, 0 , sizeof(tmp3));
														  memset(&tmp4, 0 , sizeof(tmp4));
														  memset(&tmp5, 0 , sizeof(tmp5));
														  
														  //V IMP. Increment Que Cnt for every ';' ocurance
														  que_cntr_1++; 

														  continue; 
													  }
													  else if(recv_buffer[i] == ',')
													  {
														  comma_cnt++;
														  j=0;
														  continue;
													  }

													  switch(comma_cnt)
													  {

														  case 0 : xsvrusr.q_info[que_cntr_1].q_name[j++] = recv_buffer[i];
																   break;
														  
														  case 1 : tmp1[j++] = recv_buffer[i];
																   break;
														  
														  case 2 : tmp2[j++] = recv_buffer[i];
																   break;
														  
														  case 3 : tmp3[j++] = recv_buffer[i];
																   break;

														  case 4 : tmp4[j++] = recv_buffer[i];
																   break;

														  case 5 : tmp5[j++] = recv_buffer[i];
																   break;

														  default: break;
													  }

											}break;//End of case 2

										case 3 :
												{
													if(recv_buffer[i] == 'M' && recv_buffer[i+1] == '|')
													 {
														  i++; 
														  j=0;

														  memset(&tmp1, 0 , sizeof(tmp1));
														  memset(&tmp2, 0 , sizeof(tmp2));
														  memset(&tmp3, 0 , sizeof(tmp3));
														  
														  continue;
													 }
													  else if(recv_buffer[i] == '!') break;

													  else if(recv_buffer[i] == ';') 
													  { 
														  comma_cnt = 0; 
														  j=0;

														  xsvrusr.mem_info[mem_cntr_1].tableId = atoi(tmp1);
														  xsvrusr.mem_info[mem_cntr_1].sizeofDataCurrentlyWritten = atoi(tmp2);
														  xsvrusr.mem_info[mem_cntr_1].maxTableSize = atoi(tmp3);
														  
														  memset(&tmp1, 0 , sizeof(tmp1));
														  memset(&tmp2, 0 , sizeof(tmp2));
														  memset(&tmp3, 0 , sizeof(tmp3));														  
														  
														  //V IMP. Increment Mem Cnt for every ';' ocurance
														  mem_cntr_1++; 

														  continue; 
													  }
													  else if(recv_buffer[i] == ',')
													  {
														  comma_cnt++;
														  j=0;
														  continue;
													  }

													  switch(comma_cnt)
													  {

														  case 0 : xsvrusr.mem_info[mem_cntr_1].mem_name[j++] = recv_buffer[i];
																   break;
														  
														  case 1 : tmp1[j++] = recv_buffer[i];
																   break;
														  
														  case 2 : tmp2[j++] = recv_buffer[i];
																   break;
														  
														  case 3 : tmp3[j++] = recv_buffer[i];
																   break;														

														  default : break;
													  }

											}break; //End of case 3
											

										}//End of switch();

									}

									user_cntr = user_cntr_1;
									que_cntr = que_cntr_1;
									mem_cntr = mem_cntr_1;

									xsvrusr1 = xsvrusr;

									User_Update_f = TRUE;
									Que_Update_f = TRUE;
									Mem_Update_f = TRUE;

									::SendMessage(h,WM_REFRESHWND,0,0);

					break;

					case MT_XCOM_GUI_ACK_USER:
					
							memset(&Recvd_Str, 0, sizeof(Recvd_Str));
							strncpy(Recvd_Str, p_rec, dataLength);

							switch(submsgtyp)
							{
									case ST1_XCOM_GUI_ACK_USER_CREATE: 

									break;

									case ST1_XCOM_GUI_ACK_USER_DELETE: 

									break;
							}
					
					break;

					case MT_XCOM_GUI_FLUSH_ACK : 

						Flush_f = TRUE;

						Enable_Disable_Buttons(FALSE);

					break;


			}//End of switch(msgtyp)
			
		}

		return 1;
	}

	/* Thread for Receiving the Data Sent by ATP Server */
	/*void Recv_Thread()
	{
	
		while(1)
		{
			Sleep(1000);

			if(Flush_f == FALSE && login_failed == 0) 
			{
				Recv_Msg();
			}			
		}
	}*/
	void Recv_Thread(void *param)
	{
		HWND *h = (HWND *) param;
		while(1)
		{
			//Sleep(1000);

			if(Flush_f == FALSE && login_failed == 0) 
			{
				Recv_Msg(*h);				
			}			
		}
	}


#ifdef  __cplusplus
	}
#endif



/////////////////////////////////////////////////////////////////////////////
// CXCOMTOOLDlg dialog

CXCOMTOOLDlg::CXCOMTOOLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXCOMTOOLDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXCOMTOOLDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXCOMTOOLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXCOMTOOLDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	//USERINFO PARAMS
	DDX_Control(pDX, IDC_LIST1, user_list);	
	DDX_Control(pDX, IDC_USERINFO, user_label1);
	DDX_Control(pDX, IDC_USERINFO_HEADER, user_label2);
	DDX_Control(pDX, IDC_STATIC_USER_SUMMARY, user_summary);

	//QUESYS PARAMS
	DDX_Control(pDX, IDC_LIST2, q_list);	
	DDX_Control(pDX, IDC_QUESYS, q_label1);
	DDX_Control(pDX, IDC_QUESYS_HEADER,	q_label2);	
	DDX_Control(pDX, IDC_STATIC_QUE_SUMMARY, que_summary);

	//MEMSYS PARAMS
	DDX_Control(pDX, IDC_LIST3, mem_list);	
	DDX_Control(pDX, IDC_STATIC_MEMSYS, mem_label1);
	DDX_Control(pDX, IDC_STATIC_MEMSYS_HEADER,	mem_label2);
	DDX_Control(pDX, IDC_STATIC_MEM_SUMMARY, mem_summary);

	//DATE & TIME PARAMS
	//DDX_Control(pDX, IDC_STATIC_DATE_TIME, date_time_label);
	
	//APP NAME PARAMS
	DDX_Control(pDX, IDC_STATIC_APP_NAME, app_name);

	//SEARCH PARAMS
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_edit_search);
}

BEGIN_MESSAGE_MAP(CXCOMTOOLDlg, CDialog)
	//{{AFX_MSG_MAP(CXCOMTOOLDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CONFIG, OnBtnConfig)
	ON_BN_CLICKED(IDC_BTN_CREATE, OnBtnCreate)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelchangeList2)
	ON_LBN_SELCHANGE(IDC_LIST3, OnSelchangeList3)
	ON_BN_CLICKED(IDC_BTN_READ, OnBtnRead)
	ON_BN_CLICKED(IDC_BTN_RETRY, OnBtnRetry)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_BN_CLICKED(IDC_BTN_WRITE, OnBtnWrite)
	ON_BN_CLICKED(IDC_BTN_FLUSH, OnBtnFlush)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBtnDelete)
	ON_LBN_SETFOCUS(IDC_LIST2, OnSetfocusList2)
	ON_LBN_SETFOCUS(IDC_LIST1, OnSetfocusList1)
	ON_LBN_SETFOCUS(IDC_LIST3, OnSetfocusList3)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH_NEXT, OnButtonSearchNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXCOMTOOLDlg message handlers

BOOL CXCOMTOOLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CDC* dc;
	CRect size;
	CFont* pOldFont;
	INT ListBoxWidth;

	//set tab stops for dialog connect list box
	CRect rectDialogList;	
	int nDialogTabStop[4];

	//CWnd hWnd;
	//HDC hdc= (struct HDC__ *)GetDC();
	//struct HWND__ *hwnd = GetSafeHwnd();
	//hwnd->
	//AfxMessageBox();	

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	q_list.SetFocus();
	q_list.SetFont((CFont *)"Verdana",TRUE);	
	q_label1.SetFont((CFont *)"Ariel",TRUE);
	q_label2.SetFont((CFont *)"Ariel",TRUE);

	user_list.SetFocus();
	user_list.SetFont((CFont *)"Verdana",TRUE);
	user_label1.SetFont((CFont *)"Ariel",TRUE);
	user_label2.SetFont((CFont *)"Ariel",TRUE);

	mem_list.SetFocus();
	mem_list.SetFont((CFont *)"Verdana",TRUE);
	mem_label1.SetFont((CFont *)"Ariel",TRUE);
	mem_label2.SetFont((CFont *)"Ariel",TRUE);

	app_name.SetFont((CFont *)"Verdana",TRUE);
	//user_summary.SetFont((CFont *)"Ariel",TRUE);
	//que_summary.SetFont((CFont *)"Ariel",TRUE);
	//mem_summary.SetFont((CFont *)"Ariel",TRUE);
	
	//date_time_label.SetFont((CFont *)"Ariel",TRUE);

	m_button_config = (CButton *)GetDlgItem(IDC_BTN_CONFIG);
	m_button_create = (CButton *)GetDlgItem(IDC_BTN_CREATE);
	m_button_read   = (CButton *)GetDlgItem(IDC_BTN_READ);
	m_button_write  = (CButton *)GetDlgItem(IDC_BTN_WRITE);
	m_button_delete = (CButton *)GetDlgItem(IDC_BTN_DELETE);
	m_button_flush  = (CButton *)GetDlgItem(IDC_BTN_FLUSH);
	m_button_stop   = (CButton *)GetDlgItem(IDC_BTN_STOP);

	//SetWindowPos(&CWnd::wndTop, 60, 350, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);

	HWND *h = NULL;
	h = (HWND *) malloc(sizeof(HWND));
	*h = this->m_hWnd;

	
	//START THE DEFAULT TIMER
	Timeval=StartTimer(1000);

	//RESET THE FLAGS TO FALSE
	QueList_f = MemList_f = UserList_f = FALSE;

	Flush_f = FALSE;

	Connect_Applnk();

    if( login_failed )
	{
      //sprintf( m_ConnectString, "Unable to Connect to Primary Server: %s", m_ThisMachine );
	  //AfxMessageBox( m_ConnectString, MB_ICONSTOP | MB_OK );

	  //Close the Dialog if the Conenction to Server is not successful
	  CDialog::OnCancel();
	  return FALSE;
	}
	else
	{
		sprintf( m_ConnectString, "Connected to Primary Server: %s", m_ThisMachine );
		//AfxMessageBox( m_ConnectString, MB_ICONSTOP | MB_OK );
		Send_Msg(MT_XCOM_GUI_LOGIN, 0);

#if defined(WIN32)
		lhandle = CreateThread(  NULL,
									0,
									(LPTHREAD_START_ROUTINE)Recv_Thread,
										h,
									0,
									&lthread_id );
		if( lhandle == NULL )
		{
			LogEvent( "ASCENDENT: initialize_activity_timer, Unable to create activity timer thread", ERROR_MSG );
			return FALSE;
		}
#elif defined(SOLARIS_BUILD)
		rc = pthread_create (&tcp_timer_thread, NULL, (void*(*)(void*))Recv_Thread, (void*)NULL);
		if (rc != 0)
		{
			LogEvent( "ASCENDENT: initialize_activity_timer, Unable to create activity timer thread", ERROR_MSG );
			return FALSE;
		}
#endif

	}
	
	//Set Tab stops for User, Mem & Que list Boxes	
	//Set Tab Stops for User List Box
	user_list.GetWindowRect(&rectDialogList);
	
	dc = GetDC();
	size = rectDialogList;	
	// figure out what font the box has, and select it
	pOldFont = dc->SelectObject(GetFont());

	// accurately measure the average width of a character
  	dc->DrawText(_T("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"), -1, &size,
  		DT_SINGLELINE | DT_NOCLIP | DT_CALCRECT);

	//  find the tab stop, being careful with rounding
	ListBoxWidth = ((rectDialogList.Width()*4) / ((25+size.Width())/52));

    nDialogTabStop[0] = ( ListBoxWidth * 10 ) / 100;
    nDialogTabStop[1] = ( ListBoxWidth * 15 ) / 100;
    
	dc->SelectObject(pOldFont);
	ReleaseDC(dc);

	user_list.SetTabStops(2, nDialogTabStop);

	//Set Tab Stops for Que List Box
	q_list.GetWindowRect(&rectDialogList);
	
	dc = GetDC();
	size = rectDialogList;	
	// figure out what font the box has, and select it
	pOldFont = dc->SelectObject(GetFont());

	// accurately measure the average width of a character
  	dc->DrawText(_T("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"), -1, &size,
  		DT_SINGLELINE | DT_NOCLIP | DT_CALCRECT);

	//  find the tab stop, being careful with rounding
	ListBoxWidth = ((rectDialogList.Width()*4) / ((25+size.Width())/52));

    nDialogTabStop[0] = ( ListBoxWidth * 10 ) / 100;
    nDialogTabStop[1] = ( ListBoxWidth * 40 ) / 100;
    nDialogTabStop[2] = ( ListBoxWidth * 50 ) / 100;
	
	dc->SelectObject(pOldFont);
	ReleaseDC(dc);

	q_list.SetTabStops(3, nDialogTabStop);


	//Set Tab Stops for Mem List Box
	mem_list.GetWindowRect(&rectDialogList);
	
	dc = GetDC();
	size = rectDialogList;	
	// figure out what font the box has, and select it
	pOldFont = dc->SelectObject(GetFont());

	// accurately measure the average width of a character
  	dc->DrawText(_T("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"), -1, &size,
  		DT_SINGLELINE | DT_NOCLIP | DT_CALCRECT);

	//  find the tab stop, being careful with rounding
	ListBoxWidth = ((rectDialogList.Width()*4) / ((25+size.Width())/52));

    nDialogTabStop[0] = ( ListBoxWidth * 12 ) / 100;
    nDialogTabStop[1] = ( ListBoxWidth * 40 ) / 100;
	
	dc->SelectObject(pOldFont);
	ReleaseDC(dc);

	mem_list.SetTabStops(2, nDialogTabStop);




	//user_list.AddString("101    Utest1    GET");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXCOMTOOLDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXCOMTOOLDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

LRESULT CXCOMTOOLDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	CTime ct;
	int i = 0;
	int j = 0;
	int str_len=0;
	
	int user_sel_idx;
	int q_sel_idx;
	int mem_sel_idx;
	int user_scroll_idx;
	int q_scroll_idx;
	int mem_scroll_idx;
	
	long mem_total_usage;
	long que_total_msg;
	long que_total_allowed_msg;
	
	float user_percent;
	float que_percent;	
	float mem_percent;
	float mem_total;
	

	int user_max  = MAX_USERS;
	int que_max = MAX_QUEUES;
	long mem_max = MAX_SHM_MEMORY;
	
	char temp[256] = {0};	

	switch(message)
	{
		case WM_REFRESHWND:
				
				  UpdateData();

				 /* ct=CTime::GetCurrentTime();
				  sprintf(temp ,"DATE:%02d-%02d-%04d  TIME:%02d:%02d:%02d",
					   			 ct.GetDay(), ct.GetMonth(), ct.GetYear(),  ct.GetHour(),ct.GetMinute(),ct.GetSecond());
				  date_time_label.SetWindowText(temp);
				  */

				  if(User_Update_f)
				  {
						  User_Update_f = FALSE;
						  user_list.SetRedraw(FALSE);

						  user_sel_idx = user_list.GetCurSel();
						  user_scroll_idx = user_list.GetScrollPos(SB_VERT);
						  user_list.ResetContent();

						  for(j = 0;j < user_cntr;j++)
						  {
								//USER LIST	
								if(xsvrusr1.u_info[j].userID != -1)
							  	{
									memset(&temp, 0, sizeof(temp));

									if(xsvrusr1.u_info[j].isWaitingForMsg == 1)
									{
										sprintf(temp ,"%03d\t%s\tGET",xsvrusr.u_info[j].userID, xsvrusr1.u_info[j].u_name);
									}
									else
									{
										sprintf(temp ,"%03d\t%s\t***",xsvrusr.u_info[j].userID, xsvrusr1.u_info[j].u_name);
									}

									user_list.InsertString(j,temp);
								}
						  }

						  user_percent = (((float)user_cntr)*100)/((float)user_max);

						  sprintf(temp, "Users : %d/%d (%0.1f",
							  user_cntr, MAX_USERS, user_percent);
						  strcat(temp, " %)");

						  user_summary.SetWindowText(temp);

						  user_list.SetCurSel(user_sel_idx);
						  user_list.SetScrollPos(SB_VERT,user_scroll_idx,TRUE);
						  user_list.SetTopIndex(user_scroll_idx);

						  user_list.SetRedraw(TRUE);
				  }
				  if(Que_Update_f)
				  {
					   Que_Update_f = FALSE;
					   q_list.SetRedraw(FALSE);

					   q_sel_idx = q_list.GetCurSel();
					   q_scroll_idx = q_list.GetScrollPos(SB_VERT);
					   q_list.ResetContent();

					   str_len = 0;
					   que_total_msg = 0;
					   que_total_allowed_msg = 0;

					  for(j = 0;j < que_cntr;j++)
					  {
							//QUE LIST	
							if(xsvrusr1.q_info[j].queueID != -1)
							{
								memset(&temp, 0, sizeof(temp));								
						
								sprintf(temp ,"%03d\t%s\t%01d/%d\t%d/%d", 
									xsvrusr1.q_info[j].queueID, 
									xsvrusr1.q_info[j].q_name, 
									xsvrusr1.q_info[j].totalMessages, 
									xsvrusr1.q_info[j].totalAllowedMsgs, 
									xsvrusr1.q_info[j].totalDataBytes,
									xsvrusr1.q_info[j].totalAllowedData);
								
								q_list.InsertString(j,temp);

								que_total_msg += xsvrusr1.q_info[j].totalMessages;
								que_total_allowed_msg += xsvrusr1.q_info[j].totalAllowedMsgs;
							}
					  }

					   que_percent = (((float)que_cntr)*100/((float)MAX_QUEUES));
					   sprintf(temp, "Ques : %d/%d (%0.1f", que_cntr, MAX_QUEUES, que_percent);
					   strcat(temp, " %)");

					   que_percent = (((float)que_total_allowed_msg)*100)/ ((float)sizeof_shm_seg);

					   sprintf(&temp[strlen(temp)], "\nData Usage : %ld.%02dM/%ld.%02dM (%0.1f", 
												   que_total_allowed_msg/1000000, que_total_allowed_msg%100,
												   sizeof_shm_seg/1000000, sizeof_shm_seg%100 , que_percent);
					   strcat(temp, " %)");
					   sprintf(&temp[strlen(temp)], "\nTotal Msgs   : %ld/0", que_total_msg);

					   que_summary.SetWindowText(temp);

					   q_list.SetCurSel(q_sel_idx);
					   q_list.SetScrollPos(SB_VERT,q_scroll_idx,TRUE);
					   q_list.SetTopIndex(q_scroll_idx);

					   q_list.UpdateWindow();
					   q_list.SetRedraw(TRUE);
				  }
				  if(Mem_Update_f)
				  {
					  Mem_Update_f = FALSE;	
					  mem_list.SetRedraw(FALSE);

					  mem_sel_idx = mem_list.GetCurSel();
					  mem_scroll_idx = mem_list.GetScrollPos(SB_VERT);
					  mem_list.ResetContent();

					  mem_total_usage = 0;

					  for(j = 0;j < mem_cntr;j++)
					  {
							//MEM LIST	
							if(xsvrusr.mem_info[j].tableId != -1)
							{
								memset(&temp, 0, sizeof(temp));							

								sprintf(temp ,"%03d\t%s\t%d/%d",
									xsvrusr1.mem_info[j].tableId, 
									xsvrusr1.mem_info[j].mem_name, 
									xsvrusr1.mem_info[j].sizeofDataCurrentlyWritten, 
									xsvrusr1.mem_info[j].maxTableSize);
								
								mem_list.InsertString(j,temp);

								mem_total_usage += xsvrusr1.mem_info[j].maxTableSize;
							}
					  }

					  //mem_total = ((mem_total_usage*100)/50)/1000;
					  mem_total = (((float)mem_total_usage)*100)/((float)sizeof_shm_seg);

					  mem_percent = (((float)mem_cntr)*100/((float)MAX_SHM_TABLES));

					  sprintf(temp, "Mem Pool : %d/%d (%0.1f",
										  mem_cntr, MAX_SHM_TABLES, mem_percent);
					  strcat(temp, " %)\n");

					  sprintf(&temp[strlen(temp)], "Usage : %ld.%02ldM/%ld.%dM (%0.1f", 
											mem_total_usage/1000000, mem_total_usage%100,
											sizeof_shm_seg/1000000, sizeof_shm_seg%100,
											mem_total);

					  strcat(temp, " %)");
					  mem_summary.SetWindowText(temp);

					  mem_list.SetCurSel(mem_sel_idx);
					  mem_list.SetScrollPos(SB_VERT,mem_scroll_idx,TRUE);
				      mem_list.SetTopIndex(mem_scroll_idx);
				  }
				  mem_list.SetRedraw(TRUE);
				  UpdateData(FALSE);

				  //Send_Msg(MT_XCOM_GUI_STATUS_UPDATE, 0);
			break;

		case WM_TIMER:
		{	
			if(Flush_f == FALSE)
			{
				Send_Msg(MT_XCOM_GUI_STATUS_UPDATE, 0);
				//Sleep(200);
				//Recv_Msg(this->m_hWnd);
			}
		}
		break;

		case WM_ENDSESSION :
			break;

		default: break;

	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CXCOMTOOLDlg::OnBtnConfig() 
{
	// TODO: Add your control notification handler code here
	CRefreshWnd refreshWnd = new  CRefreshWnd();
	refreshWnd.DoModal();
	
	if(atoi(refreshWnd.Refresh_Val)>=500)
	{
		if (!KillTimer (Timeval))
			::MessageBox(NULL,"Error While Killing the Timer","ShutEvt",1);	

		Timeval=StartTimer(atoi(refreshWnd.Refresh_Val));
	}

	//closesocket(lhSocket);
	
}

void CXCOMTOOLDlg::OnBtnCreate() 
{
	// TODO: Add your control notification handler code here
	if(QueList_f)
	{
		//QueList_f = FALSE;

		CCreatewnd create = new CCreatewnd();
		create.Que_f = TRUE;
		create.Mem_f = FALSE;

		//create.create_name.SetWindowText("CREATE QUEUE TABLE");
		
		create.DoModal();
	
		if(create.OK_f)		
		{
			create.OK_f = FALSE;

			strcpy(Que_Name, create.Createwnd_NAME);
			NoBytes = atoi(create.Createwnd_NoBytes);
			NoMsgs  = atoi(create.Createwnd_NoMsgs);

			Send_Msg(MT_XCOM_GUI_QUE, ST1_XCOM_GUI_QUE_CREATE);
			//AfxMessageBox(create.Createwnd_NoBytes);
		}
	}

	else if(MemList_f)
	{
		//MemList_f = FALSE;

		CCreatewnd create = new CCreatewnd();
		create.Mem_f = TRUE;
		create.Que_f = FALSE;

		create.DoModal();

		if(create.OK_f)
		{
			create.OK_f = FALSE;
			strcpy(Mem_Name, create.Createwnd_NAME);
			NoBytes = atoi(create.Createwnd_NoBytes);

			Send_Msg(MT_XCOM_GUI_MEM, ST1_XCOM_GUI_MEM_CREATE);
			//AfxMessageBox(create.Createwnd_NoBytes);
		}
	}

	else AfxMessageBox("Please select any element of QUESYS or MEMSYS");

	//user_list.SetCurSel(-1);
	//mem_list.SetCurSel(-1);
	//q_list.SetCurSel(-1);
	
}

UINT  CXCOMTOOLDlg::StartTimer (UINT TimerDuration)
{
        UINT    TimerVal;

		TimerVal = SetTimer (IDT_TIMER, TimerDuration, NULL);  // Starting the Timer
        if (TimerVal == 0)
        {
              AfxMessageBox("Unable to obtain timer");
        }
		
      return TimerVal;

}// end StartTimer

void CXCOMTOOLDlg::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	UserList_f = TRUE;
	MemList_f = QueList_f = FALSE;	

	mem_list.SetCurSel(-1);
	q_list.SetCurSel(-1);
	
}

void CXCOMTOOLDlg::OnSelchangeList2() 
{
	// TODO: Add your control notification handler code here
	QueList_f = TRUE;
	MemList_f = UserList_f = FALSE;	
	
	mem_list.SetCurSel(-1);
	user_list.SetCurSel(-1);
	
}

void CXCOMTOOLDlg::OnSelchangeList3() 
{
	// TODO: Add your control notification handler code here
	MemList_f = TRUE;
	QueList_f = UserList_f = FALSE;	

	q_list.SetCurSel(-1);
	user_list.SetCurSel(-1);
	
}

void CXCOMTOOLDlg::OnBtnRead() 
{
	// TODO: Add your control notification handler code here
		if(QueList_f)       Send_Msg(MT_XCOM_GUI_QUE, ST1_XCOM_GUI_QUE_READ);
	else if(MemList_f)  Send_Msg(MT_XCOM_GUI_MEM, ST1_XCOM_GUI_MEM_READ);
	else if(UserList_f) Send_Msg(MT_XCOM_GUI_USER, ST1_XCOM_GUI_USER_READ);
}
	


void CXCOMTOOLDlg::OnBtnRetry() 
{
	// TODO: Add your control notification handler code here
	if(Flush_f == TRUE)
	{
		Connect_Applnk();

		Enable_Disable_Buttons(TRUE);		
	}
	
}

void CXCOMTOOLDlg::OnBtnStop() 
{
	// TODO: Add your control notification handler code here
	Send_Msg(MT_XCOM_GUI_LOGOUT, MT_XCOM_GUI_LOGOUT);

	if (!KillTimer (Timeval))
		::MessageBox(NULL,"Error While Killing the Timer","ShutEvt",1);

	Disconnect_Applnk();

	CDialog::OnCancel();
	
}

void CXCOMTOOLDlg::OnBtnWrite() 
{
	// TODO: Add your control notification handler code here
	if(QueList_f)       Send_Msg(MT_XCOM_GUI_QUE, ST1_XCOM_GUI_QUE_WRITE);
	else if(MemList_f)  Send_Msg(MT_XCOM_GUI_MEM, ST1_XCOM_GUI_MEM_WRITE);
	else if(UserList_f) Send_Msg(MT_XCOM_GUI_USER, ST1_XCOM_GUI_USER_WRITE);
	
}

void CXCOMTOOLDlg::OnBtnFlush() 
{
	// TODO: Add your control notification handler code here
	if(Flush_f == FALSE && login_failed == 0)
	{
		Send_Msg(MT_XCOM_GUI_FLUSH, 0);

		Flush_f = TRUE;
		Disconnect_Applnk();		

		Enable_Disable_Buttons(FALSE);

		user_list.ResetContent();
		q_list.ResetContent();
		mem_list.ResetContent();
	}
}

void CXCOMTOOLDlg::OnBtnDelete() 
{
	// TODO: Add your control notification handler code here
	if(QueList_f)
	{
		//QueList_f = FALSE;

		CDeleteWnd del_wnd = new CDeleteWnd();

		del_wnd.Que_f = TRUE;
		del_wnd.Mem_f = FALSE;
		del_wnd.User_f = FALSE;

		//create.create_name.SetWindowText("CREATE QUEUE TABLE");
		
		del_wnd.DoModal();
	
		if(del_wnd.OK_f)
		{
			del_wnd.OK_f = FALSE;
			Send_Msg(MT_XCOM_GUI_QUE, ST1_XCOM_GUI_QUE_DELETE);
			
			//AfxMessageBox("DELETE QUE ELEMENT CONFIRMED");
		}
	}

	else if(MemList_f)
	{
		//MemList_f = FALSE;

		CDeleteWnd del_wnd = new CDeleteWnd();

		del_wnd.Mem_f = TRUE;
		del_wnd.Que_f = FALSE;
		del_wnd.User_f = FALSE;

		del_wnd.DoModal();		
		
		if(del_wnd.OK_f)
		{
			del_wnd.OK_f = FALSE;
			Send_Msg(MT_XCOM_GUI_MEM, ST1_XCOM_GUI_MEM_DELETE);
		}
	}
	else if(UserList_f)
	{
		//UserList_f = FALSE;

		CDeleteWnd del_wnd = new CDeleteWnd();

		del_wnd.User_f = TRUE;
		del_wnd.Mem_f = FALSE;
		del_wnd.Que_f = FALSE;

		del_wnd.DoModal();		
		
		if(del_wnd.OK_f)
		{
			del_wnd.OK_f = FALSE;
			Send_Msg(MT_XCOM_GUI_USER, ST1_XCOM_GUI_USER_DELETE);
		}
	}

	else AfxMessageBox("Please select any element of USERINFO or QUESYS or MEMSYS");

	//user_list.SetCurSel(-1);
	//mem_list.SetCurSel(-1);
	//q_list.SetCurSel(-1);
	
}



void CXCOMTOOLDlg::OnSetfocusList1() 
{
	// TODO: Add your control notification handler code here
	UserList_f = TRUE;
	MemList_f = QueList_f = FALSE;	

	mem_list.SetCurSel(-1);
	q_list.SetCurSel(-1);	
}

void CXCOMTOOLDlg::OnSetfocusList2() 
{
	// TODO: Add your control notification handler code here
	QueList_f = TRUE;
	MemList_f = UserList_f = FALSE;	
	
	mem_list.SetCurSel(-1);
	user_list.SetCurSel(-1);
}


void CXCOMTOOLDlg::OnSetfocusList3() 
{
	// TODO: Add your control notification handler code here
	MemList_f = TRUE;
	QueList_f = UserList_f = FALSE;	

	q_list.SetCurSel(-1);
	user_list.SetCurSel(-1);	
}


void CXCOMTOOLDlg::Connect_Applnk()
{
	//Krishna@TF, 10.6.11, Get App Product Version from String Table & copy it into buffer 
	//which is being used in the ptetcp lib in ptetcp_connect()	
	CString appProdVer;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	strncpy(appProductVersion, appProdVer, appProdVer.GetLength());
	//End Krishna changes

	GetAppName( AppName );
    login_failed = 0;

    ptetcp_init( error_msg );

    // get the primary machine id from registry, login into primary 
    // machine instance
    if( !GetFirstServerId( m_ThisMachine ) )
    {
        AfxMessageBox( "First Server Id not found in INI file", MB_ICONSTOP | MB_OK );
        strcpy( m_ThisMachine, "Local" );
        login_failed = 1;
    }

    if( !ptetcp_connect( m_ThisMachine, error_msg ) )
    {
      sprintf( buffer, "Unable to Connect to %s. Error: %s", m_ThisMachine, error_msg );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
      login_failed = 1;
    }

    //check if high availability configuration
    if( !GetHighAvailableIndicator( &m_HighAvailability ) )
    {
      AfxMessageBox( "High Availablity Indicator not Found in INI File", MB_ICONSTOP | MB_OK );
    }

	if(login_failed == 0)
	{
		if(Flush_f)
		{
			Send_Msg(MT_XCOM_GUI_LOGIN, ST1_XCOM_GUI_RELOGIN);
			Flush_f = FALSE;
		}		
	}
}

void CXCOMTOOLDlg::Disconnect_Applnk()
{
	ptetcp_disconnect();

}

void CXCOMTOOLDlg::OnButtonSearch() 
{
	// TODO: Add your control notification handler code here
	int i=0,j=0,cnt=0, sel_idx=0;
	char search_name[10]={'\0'};

	m_edit_search.GetWindowText(search_name, 10);

	char listBuffer[256]={'\0'};

	if(UserList_f == false && QueList_f == false && MemList_f == false)
		UserList_f = true;

back:

	if(!pattern_match)
	{
		if(UserList_f)
		{
			while(sel_idx < user_cntr)
			{
				  memset(&listBuffer, 0x00, sizeof(listBuffer));
				  memset(&User_Name, 0x00, sizeof(User_Name));

				  user_list.GetText(sel_idx,listBuffer);
				
				  //Parse the string for Qid & Name
				  for(i=0; listBuffer[i]!='\0' || i <20 ; i++)
				  {			
					  if(listBuffer[i] == '\t') 
					  { 
						  cnt++; 
						  j = 0; 
						  continue; 
					  }

					 if(cnt == 1)
					 {
						 User_Name[j++] = listBuffer[i];
					 }
					 else if(cnt>=2) 
					 {
						 cnt = 0;
						 break;
					 }
				  }

				  //if(strcmp(User_Name, search_name)==0)
				  if(strstr(User_Name, search_name)!=NULL)
				  {
					  user_list.SetCurSel(sel_idx);

					  pattern_match = TRUE;

					  search_idx = sel_idx+1;

					  break;
				  }

				  sel_idx++;
			}
			if(sel_idx > user_cntr)
			{
				sel_idx = 0;

				QueList_f = TRUE;
			}
		}

		if(QueList_f)
		{
			while(sel_idx < user_cntr)
			{
				  memset(&listBuffer, 0x00, sizeof(listBuffer));
				  memset(&User_Name, 0x00, sizeof(User_Name));

				  q_list.GetText(sel_idx,listBuffer);
				
				  //Parse the string for Qid & Name
				  for(i=0; listBuffer[i]!='\0' || i <20 ; i++)
				  {			
					  if(listBuffer[i] == '\t') 
					  { 
						  cnt++; 
						  j = 0; 
						  continue; 
					  }

					 if(cnt == 1)
					 {
						 User_Name[j++] = listBuffer[i];
					 }
					 else if(cnt>=2) 
					 {
						 cnt = 0;
						 break;
					 }
				  }

				  //if(strcmp(User_Name, search_name)==0)
				  if(strstr(User_Name, search_name)!=NULL)
				  {
					  q_list.SetCurSel(sel_idx);

					  pattern_match = TRUE;

					  search_idx = sel_idx+1;

					  break;
				  }

				  sel_idx++;
			}
			if(sel_idx > user_cntr)
			{
				sel_idx = 0;

				MemList_f = TRUE;
			}

		}
		
		if(MemList_f)
		{
			while(sel_idx < user_cntr)
			{
				  memset(&listBuffer, 0x00, sizeof(listBuffer));
				  memset(&User_Name, 0x00, sizeof(User_Name));

				  mem_list.GetText(sel_idx,listBuffer);
				
				  //Parse the string for Qid & Name
				  for(i=0; listBuffer[i]!='\0' || i <20 ; i++)
				  {			
					  if(listBuffer[i] == '\t') 
					  { 
						  cnt++; 
						  j = 0; 
						  continue; 
					  }

					 if(cnt == 1)
					 {
						 User_Name[j++] = listBuffer[i];
					 }
					 else if(cnt>=2) 
					 {
						 cnt = 0;
						 break;
					 }
				  }

				  //if(strcmp(User_Name, search_name)==0)
				  if(strstr(User_Name, search_name)!=NULL)
				  {
					  mem_list.SetCurSel(sel_idx);

					  pattern_match = TRUE;

					  search_idx = sel_idx+1;

					  break;
				  }

				  sel_idx++;
			}

			if(sel_idx > user_cntr)
			{
				sel_idx = 0;

				UserList_f = TRUE;

				goto back;
			}

		}
	}
	else if(pattern_match)
	{
		OnButtonSearchNext();

	}
	
}

void CXCOMTOOLDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	OnBtnStop();

	CDialog::OnClose();
}

void CXCOMTOOLDlg::OnButtonSearchNext() 
{
	// TODO: Add your control notification handler code here

	int i=0,j=0,cnt=0, sel_idx=0;
	char search_name[10]={'\0'};
	char listBuffer[256]={'\0'};

	m_edit_search.GetWindowText(search_name, 10);
	
	if(pattern_match)
	{	
back:	if(UserList_f)
		{
			sel_idx = search_idx;

			while(sel_idx < user_cntr)
			{
				  memset(&listBuffer, 0x00, sizeof(listBuffer));
				  memset(&User_Name, 0x00, sizeof(User_Name));

				  user_list.GetText(sel_idx,listBuffer);
				
				  //Parse the string for Qid & Name
				  for(i=0; listBuffer[i]!='\0' || i <20 ; i++)
				  {			
					  if(listBuffer[i] == '\t') 
					  { 
						  cnt++; 
						  j = 0; 
						  continue; 
					  }

					 if(cnt == 1)
					 {
						 User_Name[j++] = listBuffer[i];
					 }
					 else if(cnt>=2) 
					 {
						 cnt = 0;
						 break;
					 }
				  }

				  //if(strcmp(User_Name, search_name)==0)
				  if(strstr(User_Name, search_name)!=NULL)
				  {
					  user_list.SetCurSel(sel_idx);

					  pattern_match = TRUE;

					  search_idx = sel_idx+1;

					  break;
				  }

				  sel_idx++;
			}

			if(sel_idx >= user_cntr)
			{
				//pattern_match = FALSE;
				search_idx = 0;
				user_list.SetCurSel(-1);
				UserList_f = FALSE;
				QueList_f = TRUE;
			}
		

		}
		if(QueList_f)
		{
			sel_idx = search_idx;

			while(sel_idx < que_cntr)
			{
				  memset(&listBuffer, 0x00, sizeof(listBuffer));
				  memset(&User_Name, 0x00, sizeof(User_Name));

				  q_list.GetText(sel_idx,listBuffer);
				
				  //Parse the string for Qid & Name
				  for(i=0; listBuffer[i]!='\0' || i <20 ; i++)
				  {			
					  if(listBuffer[i] == '\t') 
					  { 
						  cnt++; 
						  j = 0; 
						  continue; 
					  }

					 if(cnt == 1)
					 {
						 User_Name[j++] = listBuffer[i];
					 }
					 else if(cnt>=2) 
					 {
						 cnt = 0;
						 break;
					 }
				  }

				  //if(strcmp(User_Name, search_name)==0)
				  if(strstr(User_Name, search_name)!=NULL)
				  {
					  q_list.SetCurSel(sel_idx);

					  pattern_match = TRUE;

					  search_idx = sel_idx+1;

					  break;
				  }

				  sel_idx++;
			}

			if(sel_idx >= que_cntr)
			{
				//pattern_match = FALSE;
				search_idx = 0;
				q_list.SetCurSel(-1);
				QueList_f = FALSE;
				MemList_f = TRUE;
			}

		}
		if(MemList_f)
		{
			sel_idx = search_idx;

			while(sel_idx < mem_cntr)
			{
				  memset(&listBuffer, 0x00, sizeof(listBuffer));
				  memset(&User_Name, 0x00, sizeof(User_Name));

				  mem_list.GetText(sel_idx,listBuffer);
				
				  //Parse the string for Qid & Name
				  for(i=0; listBuffer[i]!='\0' || i <20 ; i++)
				  {			
					  if(listBuffer[i] == '\t') 
					  { 
						  cnt++; 
						  j = 0; 
						  continue; 
					  }

					 if(cnt == 1)
					 {
						 User_Name[j++] = listBuffer[i];
					 }
					 else if(cnt>=2) 
					 {
						 cnt = 0;
						 break;
					 }
				  }

				  //if(strcmp(User_Name, search_name)==0)
				  if(strstr(User_Name, search_name)!=NULL)
				  {
					  mem_list.SetCurSel(sel_idx);

					  pattern_match = TRUE;

					  search_idx = sel_idx+1;

					  break;
				  }

				  sel_idx++;
			}

			if(sel_idx >= mem_cntr)
			{
				mem_list.SetCurSel(-1);
				//pattern_match = FALSE;
				search_idx = 0;
				MemList_f = FALSE;
				UserList_f = TRUE;

				goto back;
			}

		}
	}
	
	
}
