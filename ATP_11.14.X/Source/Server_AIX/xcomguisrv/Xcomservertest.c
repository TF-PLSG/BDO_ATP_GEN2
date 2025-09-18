/*
Krishna @TF, 26.5.11:
	- Comented all printf statements
	- Logging events whenever errors are reported. 
	- Return from App when IPC initialization failed

*/



#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <stdio.h>

#include "ntutils.h"
#include "pteipc.h"
#include "xcomgui_cmds.h"

#include "xcom.h"
//Pointer to structure SHR_MEM_LAYOUT of xcom
extern struct SHR_MEM_LAYOUT *shm_address;


/* External Function Prototypes */
extern CHAR     ServiceName[12] ;
extern INT      MainProcessDone ;

extern INT      EndProcessSignalled ;

#define   COMM_BUF_SZE		 1500
#define   MAX_Q_SIZE		 100
#define   MAX_APPNAME_SIZE   8
#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20
#define	  DEVICE_OK			 1
#define   MAX_NAME_LENGTH    25
#define   MAX_USERS			 100
#define   MAX_QUEUES		 300
#define   MAX_SHM_TABLES     30

CHAR    Version[]              = "ATP_11.1.0";
BYTE	ReqRsp_Buffer [1024]; 
INT		buflen;                  /* total size of ReqRsp_Buffer */

int Q_List[MAX_Q_SIZE];
int Q_Cnt;

int user_cntr;
int que_cntr;
int mem_cntr;

int endThreadSignalled;

pCHAR orig_info;
pCHAR orig_queue;

char Q_Name[20]={0};
 
CHAR strError[512] = {0} ;   

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
	int queueId;
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

struct XCOMSERVER xsvrusr;

char xsvr_buffer[sizeof(struct XCOMSERVER)] = {0};

void PinnacleMsgHandler (pPTE_MSG p_msg_in );

//Function to close the App. Being called by ATPKILL
extern void signal_handler( int signal );

//void signal_handler( int signal )
//{
//   /*printf( "in signal handler\n" );*/
//   EndProcessSignalled2 = 1;
//
//   pteipc_sleep(2000);
//}

/*
int push_SocketId(int id)
{
	int i=0;
	if(Q_Cnt < MAX_Q_SIZE)
	{
		//printf("\nQueue Elements");

		Q_List[Q_Cnt] = id;
		Q_Cnt++;

		//for(i=0;i< Q_Cnt;i++)
		//{
		//	printf("\n %d: %d", i, Q_List[i]);
		//}

		return 1;
	}

	return 0;
}

int pop_SocketId(int id)
{
	int i = 0;

	while(i < Q_Cnt)
	{
		if(Q_List[i] == id)
		{
			break;
		}
		i++;
	}

	if(Q_Cnt > 0)
	{
		while(i < Q_Cnt)
		{
			Q_List[i] = Q_List[i+1];
			i++;
		}

		Q_Cnt--;
	}
	else if(Q_Cnt == 0)
	{		
		Q_List[Q_Cnt] = 0;
		Q_Cnt = 0;
	}

//	if(i > 0)
//	{
//		for(i=0;i< Q_Cnt;i++)
//		{
//			printf("\n %d, %d", i, Q_List[i]);
//		}
//	}
//	else printf("NO Elements in Queue");

	return 0;
}
*/


/**********************************************************************************
Function which fetches the Xcom Params from SHR_MEM & stores it in custom defined Structure
***********************************************************************************/
void getXcomParams()
{
	int idx;
	user_cntr = 0;
    que_cntr = 0;
    mem_cntr = 0;

	memset(&xsvrusr, 0, sizeof(struct XCOMSERVER));

	for(idx=0;idx< MAX_QUEUES ;idx++)
    {
        if(idx < TOTAL_CMD_SLOTS_AVAILABLE)
		{
			//if(shm_address->cmdSlot[idx].user.username[0] != '\0')
			if(shm_address->cmdSlot[idx].user.userid != -1 && strlen(shm_address->cmdSlot[idx].user.username) > 0)
			{   				
				strncpy(xsvrusr.u_info[user_cntr].u_name, shm_address->cmdSlot[idx].user.username, strlen(shm_address->cmdSlot[idx].user.username));
				xsvrusr.u_info[user_cntr].isWaitingForMsg = shm_address->cmdSlot[idx].user.isWaitingForMsg;
				xsvrusr.u_info[user_cntr].userID = shm_address->cmdSlot[idx].user.userid;
				user_cntr++;
			}			
		}
		if(idx < MAX_SHM_TABLES)
		{
			//if(shm_address->xcomTables.userTables[idx].tableName[0]!= '\0')
			if(shm_address->xcomTables.userTables[idx].tableId!= -1 &&  strlen(shm_address->xcomTables.userTables[idx].tableName) > 0 && shm_address->xcomTables.userTables[idx].destroyed == 0)
			{				
				strncpy(xsvrusr.mem_info[mem_cntr].mem_name, shm_address->xcomTables.userTables[idx].tableName, strlen(shm_address->xcomTables.userTables[idx].tableName));
				xsvrusr.mem_info[mem_cntr].tableId = shm_address->xcomTables.userTables[idx].tableId;
				xsvrusr.mem_info[mem_cntr].sizeofDataCurrentlyWritten = shm_address->xcomTables.userTables[idx].sizeofDataCurrentlyWritten;
				xsvrusr.mem_info[mem_cntr].maxTableSize = shm_address->xcomTables.userTables[idx].maxTableSize;
				mem_cntr++;	

				//printf("\nMemID:%d, Memname : %s, Is Destroyed :%d", xsvrusr.mem_info[idx].tableId, xsvrusr.mem_info[idx].mem_name, shm_address->xcomTables.userTables[idx].destroyed);
			}
		}
		//if(shm_address->xcomQueues.userQueues[idx].queueName[0]!= '\0')
		if(shm_address->xcomQueues.userQueues[idx].queueId!= -1 && strlen(shm_address->xcomQueues.userQueues[idx].queueName)> 0)
		{				 
			 strncpy(xsvrusr.q_info[que_cntr].q_name, shm_address->xcomQueues.userQueues[idx].queueName, strlen(shm_address->xcomQueues.userQueues[idx].queueName));
			 xsvrusr.q_info[que_cntr].totalMessages = shm_address->xcomQueues.userQueues[idx].totalMessages;
			 xsvrusr.q_info[que_cntr].totalDataBytes = shm_address->xcomQueues.userQueues[idx].totalDataBytes;
			 xsvrusr.q_info[que_cntr].totalAllowedMsgs = shm_address->xcomQueues.userQueues[idx].totalAllowedMsgs;
			 xsvrusr.q_info[que_cntr].totalAllowedData = shm_address->xcomQueues.userQueues[idx].totalAllowedData;
			 xsvrusr.q_info[que_cntr].queueId = shm_address->xcomQueues.userQueues[idx].queueId;
			 que_cntr++;

			 //printf("\nQue:%d, QueName:%s",  xsvrusr.q_info[idx].queueId, xsvrusr.q_info[idx].q_name);
		}
	}	
}

/*********************************************************************************
Function which sends the XcomParam to Xcom GUI
**********************************************************************************/
void sendXcomParams()
{   
	char err_str[20];
	int i;	
	unsigned char retCode;
	int ret_code;
	char socket_str[5]={0};
	pPTE_MSG p_msg_out = NULL_PTR ;

     //printf("\nBefore Memcpy");
     //memset(buffer, 0, sizeof(buffer));
	 //memcpy(buffer, &xsvrusr, sizeof(struct XCOMSERVER));
	 //printf("\nAfter Memcpy");

	memset(&xsvr_buffer, 0, sizeof(xsvr_buffer));
	
	strncpy(xsvr_buffer,"^U|", 3);
	
	for(i=0;i<user_cntr;i++)
	{
		 sprintf(&xsvr_buffer[strlen(xsvr_buffer)], "%s,%d,%d;",
									xsvrusr.u_info[i].u_name, 
									xsvrusr.u_info[i].isWaitingForMsg,
									xsvrusr.u_info[i].userID);
	}
	strcat(xsvr_buffer, "!");

	strcat(xsvr_buffer, "^Q|");
	
	for(i=0;i<que_cntr;i++)
	{
		 sprintf(&xsvr_buffer[strlen(xsvr_buffer)], "%s,%d,%d,%d,%d,%d;", 
									 xsvrusr.q_info[i].q_name, 
									 xsvrusr.q_info[i].queueId,
									 xsvrusr.q_info[i].totalMessages,
									 xsvrusr.q_info[i].totalDataBytes,
									 xsvrusr.q_info[i].totalAllowedMsgs,
									 xsvrusr.q_info[i].totalAllowedData);
	}
	strcat(xsvr_buffer, "!");

	strcat(xsvr_buffer, "^M|");
	
	for(i=0;i<mem_cntr;i++)
	{
		if(strlen(xsvrusr.mem_info[i].mem_name) > 0)
		{
		sprintf(&xsvr_buffer[strlen(xsvr_buffer)], "%s,%d,%d,%d;", 
									xsvrusr.mem_info[i].mem_name,
									xsvrusr.mem_info[i].tableId,
									xsvrusr.mem_info[i].sizeofDataCurrentlyWritten,
									xsvrusr.mem_info[i].maxTableSize);
		}
	}
	strcat(xsvr_buffer, "!");

	//sprintf(&xsvr_buffer[strlen(xsvr_buffer)], "^S|%ld,!", (SIZEOFSHMSEG - sizeof(struct SHR_MEM_LAYOUT));
	
	p_msg_out = ptemsg_build_msg( MT_XCOM_GUI_STATUS_UPDATE,
									  0,
									  0,
									  NULL_PTR,
									  NULL_PTR,
									  (pBYTE)xsvr_buffer,  //(pBYTE)&xsvrusr,
									  strlen(xsvr_buffer),//(long)sizeof(struct XCOMSERVER),
									  (BYTE)0 );  

	if (p_msg_out == NULL_PTR)
	{
		strcpy (strError, "Insufficient Memory to build terminal response message") ;
		//printf("Error : %s", strError);
		LogEvent(strError, ERROR_MSG);
		return;
	}	
	 
	ptemsg_set_orig_comms_info( p_msg_out, orig_info );
	ret_code = pteipc_send( p_msg_out, orig_queue );
	//printf("Qsend() retcode : %d", ret_code);

	/*strcpy(orig_queue, Q_Name);	
	LONG     que_id;
	LONG     rcode;
	QIDLIST  send_quelist;
	ret_code = que_id = QueAccess (orig_queue);

	if (que_id >= 0)
	{
		QueListBuild (send_quelist, que_id, QUE_EOL);

		ret_code = QueSend (QUE_Q_ANY, send_quelist, p_msg_out, ptemsg_get_msg_length (p_msg_out), 
						1L, NULL_PTR, QUE_NOWAIT);
	}
	*/
	if( ret_code < 0 )
	{
		// attempt to send response failed. Log the error
		pteipc_get_errormsg( retCode, strError );
		sprintf( err_str, "Ascendent: Service Manager: Error sending response to %s - %s", orig_queue, strError );
		//printf( "\nError in pteipc_send() : %s", err_str );
		LogEvent( err_str, ERROR_MSG );
	}	
  
	if( p_msg_out != NULL_PTR )
     free( p_msg_out );	 
}



void MainProcessor (void)
{
   pPTE_MSG p_msg = NULL_PTR ; 
   LONG     retCode = 0L ;
   INT      rc;
   INT      err_ctr = 0;
   CHAR     strError[512] = {0} ;
   CHAR     strTemp[512] = {0} ;
   CHAR     strAppName[512] = {0} ;
   CHAR     strInstanceName[512] = {0} ;
   CHAR     time_date[25] = "";
   CHAR     seconds   [3] = "";
   
   endThreadSignalled = 0;

   /* Shutdown upon `kill` command */
   if( signal( SIGTERM, SIG_IGN ) != SIG_IGN )
	    signal( SIGTERM, signal_handler );

   GetAppName (strAppName) ;
   GetXipcInstanceName (strInstanceName) ;
   sprintf( strTemp,
           "Starting the Service: %s, version %s",
            ServiceName, Version );
   LogEvent( strTemp, INFO_MSG );
   //printf( "%s\n", strTemp );

    #ifdef _DEBUG
      pteipc_trace_on(); 
   #endif

   if(!pteipc_init_single_instance_app (strAppName, strInstanceName))
   {
	   //printf("\n Inside pteipc_init_single_instance_app()");

      strcpy (strError, "Failed to create XIPC queues");

	  /* signal that we are ready to shut down and quit */
      //pteipc_shutdown_single_instance_app();
     // MainProcessDone = 1;
	  perror(strError);
      
	  //Krishna@TF, 26.5.11: return if failed to Init & create Queues
	  LogEvent( "pteipc_init_single_instance_app() Failed. Terminating App", ERROR_MSG );
	  return;
   } 
   
   //Q_Cnt = 0;     

   //Close the App either if Applnk is closed or if Term Signal is recvd
   while ( !EndProcessSignalled )
   {
      /* You are blocked here waiting for a message on either app queue or control que  */
      /* if there is no message on either que for 1 seconds, the blocking call returns */
      p_msg = pteipc_receive (application_que_name, control_que_name, 1, &retCode);

      if (p_msg != NULL_PTR)
      {
         PinnacleMsgHandler (p_msg);
         free (p_msg);
      }
	/*  else if(retCode == QUE_ER_TIMEOUT)
	  {
		if(Q_Cnt > 0)
		{
			getXcomParams();

			sendXcomParams();
		}
	  }*/
      else if (retCode != QUE_ER_TIMEOUT) 
      {
         pteipc_get_errormsg( retCode, strTemp );
         sprintf(strError, "Error on pteipc_receive %s", strTemp);
		 perror(strError);

		 /* There is a problem with the queue.  Delay 1 second
          * so as to not flood Applnk's queue and syslog.
          */
         #ifdef WIN32
            Sleep( 1000 );
         #else
            sleep( 2 );
         #endif

		break;
      }
   }
   pteipc_shutdown_single_instance_app ();
   MainProcessDone = 1;
   free (p_msg);
   return;
} /* MainProcessor */

void EndProcess(void)
{
	return;
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
	  retval = -1;
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


void PinnacleMsgHandler (pPTE_MSG p_msg_in )
{   
   pPTE_MSG p_msg_out = NULL_PTR ;
   pPTE_MSG_DATA   p_msg_data = NULL_PTR ;

   pBYTE   p_data = NULL_PTR ;
   
   int msgtyp;
   BYTE retCode = 0 ;   
   LONG dataLength = 0L ;
   
   CHAR buffer[100]="";
   
   pCHAR p_rec = NULL_PTR ;

   CHAR SockID[5] = "";
   int i=0,j=0,cnt=0;

   CHAR Recvd_Str[256]="";
   CHAR USER_ID[4]="";
   CHAR ID[4]="";
   CHAR MEM_ID[4]="";
   CHAR Name[10]="";
   CHAR NoBytes[4]="";
   CHAR NoMsgs[4]="";   

   int ret_code = 0;   

   int Msg_Sub_Type;

   char Broad_Cast_f = FALSE;

   int qid;
   int memid;
   int userid;

	LONG     que_id;
	LONG     rcode;
	QIDLIST  send_quelist;

   memset(&p_data,0, sizeof(p_data));
   orig_queue = ptemsg_get_msg_orig_queue( p_msg_in );
   orig_info = ptemsg_get_msg_orig_comms_info( p_msg_in );
   //ptestats_accumulate_msg_stats( p_msg_in );

    //printf("\n Pinnacle Handler Q Name : %s", orig_queue);   
	strncpy(Q_Name, orig_queue, strlen(orig_queue));

   msgtyp = ptemsg_get_msg_type( p_msg_in) ;

   p_msg_data = ptemsg_get_pte_msg_data (p_msg_in);
   p_data     = ptemsg_get_pte_msg_data_data (p_msg_data);
   p_rec      = (pCHAR) p_data;
		 
   dataLength = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   retCode    = Copy_Message_Data_To_Global_Buffer( p_rec, dataLength );
   
   strncpy(Recvd_Str, (pCHAR) p_data, dataLength);
   //printf("\nRecvd Cmd : %s", Recvd_Str);   

   strcpy(buffer, orig_info);

	while(buffer[i++]!='\0')
	{
		if(buffer[i] == ',') 
		{
			cnt++;
			continue;
		}

		if(cnt == 1)
		{
			SockID[j++] = buffer[i];
		}
	}

	//printf("\n Message from Socket ID : %s", SockID);

	strcpy(buffer, "");

   switch (msgtyp)
   {
	   case MT_XCOM_GUI_LOGIN: 

			//printf("\nNew User Logged in. ID : %s", SockID);

			//push_SocketId(atoi(SockID));

            memset(&buffer, 0x00, sizeof(buffer));
			sprintf(buffer, "%ld", (SIZEOFSHMSEG - sizeof(struct SHR_MEM_LAYOUT)));

			p_msg_out = ptemsg_build_msg( MT_XCOM_GUI_ACK_LOGIN,
										  0,
										  0,
										  NULL_PTR,
										  NULL_PTR,
										  (pBYTE)buffer,
										  strlen(buffer),
										  0 );

			if (p_msg_out == NULL_PTR)
			{
				strcpy (strError, "Insufficient Memory to build terminal response message") ;
				return;
			}

		break;

		case MT_XCOM_GUI_LOGOUT: 

			Broad_Cast_f = TRUE;

			//printf("\n User Logged OUT. ID : %s", SockID);

			//pop_SocketId(atoi(SockID));

			strcpy(buffer, "");

			p_msg_out = ptemsg_build_msg( MT_XCOM_GUI_ACK_LOGOUT,
										  0,
										  0,
										  NULL_PTR,
										  NULL_PTR,
										  (pBYTE)buffer,
										  strlen(buffer),
										  0 );

			if (p_msg_out == NULL_PTR)
			{
				strcpy (strError, "Insufficient Memory to build terminal response message") ;
				return;
			}
		break;

		case MT_XCOM_GUI_QUE:

			switch(ptemsg_get_msg_subtype1( p_msg_in))
			{
				case ST1_XCOM_GUI_QUE_CREATE: 

							i = j = cnt = 0;

							Msg_Sub_Type = ST1_XCOM_GUI_ACK_QUE_CREATE;

							for(i=0;Recvd_Str[i]!='\0' || i< 20;i++)
							{
								if(Recvd_Str[i] == ',') { cnt++; j=0; continue; }

								switch(cnt)
								{
									case 0 : Name[j++] = Recvd_Str[i]; break;
									case 1 : NoBytes[j++] = Recvd_Str[i]; break;
									case 2 : NoMsgs[j++] = Recvd_Str[i]; break;
									default : break;
								}
							}
							
							qid = QueCreate(Name,atoi(NoBytes),atoi(NoMsgs));

							if(qid > 0)
							{
								sprintf(buffer, "New Que Created, QID : %d", qid);																
							}

							else
							{
								sprintf(buffer,"Que Creation Failed");								
							}
							LogEvent(buffer, INFO_MSG);
				break;

			    case ST1_XCOM_GUI_QUE_DELETE: 

							cnt = j = 0;

							Msg_Sub_Type = ST1_XCOM_GUI_ACK_QUE_DELETE;

							for(i=0;Recvd_Str[i]!='\0' || i< 20;i++)
							{
								if(Recvd_Str[i] == ',') { cnt++; j=0; continue; }

								switch(cnt)
								{
									case 0 : ID[j++] = Recvd_Str[i]; break;
									case 1 : Name[j++] = Recvd_Str[i]; break;
									default : break;
								}
							}

							qid = atoi(ID);

							ret_code = QueDestroy(qid);

							if(!ret_code)
							{
								sprintf(buffer,"\nQue :%d Deleted. Ret Code :%d", qid, ret_code);								
							}
							else
							{
								sprintf(buffer,"\n FAILED, Deleting Que :  %d. Ret Code :%d ", qid, ret_code);								
							}
							LogEvent(buffer, INFO_MSG);
				break;

				case ST1_XCOM_GUI_QUE_READ: 

							Msg_Sub_Type = ST1_XCOM_GUI_ACK_QUE_READ;
				break;

				case ST1_XCOM_GUI_QUE_WRITE: 

							Msg_Sub_Type = ST1_XCOM_GUI_ACK_QUE_WRITE;						
				break;
			}

			//Krishna@TF: For any update on Ques, get the latest info of SHM structure & post it to XCOMTOOL
			getXcomParams();
				
			sendXcomParams();

				p_msg_out = ptemsg_build_msg( MT_XCOM_GUI_ACK_QUE,
											  Msg_Sub_Type,
											  0,
											  NULL_PTR,
											  NULL_PTR,
											  (pBYTE)buffer,
											  (long)strlen(buffer),
											  (BYTE)0 );

				if (p_msg_out == NULL_PTR)
				{
					strcpy (strError, "Insufficient Memory to build terminal response message") ;
					LogEvent(strError, ERROR_MSG);
					return;
				}
		break;
   

	  case MT_XCOM_GUI_MEM :
				
				switch(ptemsg_get_msg_subtype1( p_msg_in))
				{
					case ST1_XCOM_GUI_MEM_CREATE: 

											cnt = j = 0;

											Msg_Sub_Type = ST1_XCOM_GUI_ACK_MEM_CREATE;

											for(i=0;Recvd_Str[i]!='\0' || i< 20;i++)
											{
												if(Recvd_Str[i] == ',') { cnt++; j=0; continue; }

												switch(cnt)
												{
													case 0 : Name[j++] = Recvd_Str[i]; break;
													case 1 : NoBytes[j++] = Recvd_Str[i]; break;													
													default : break;
												}
											}
											
											memid = MemCreate(Name,atoi(NoBytes));

											if(memid > 0)
											{
												sprintf(buffer,"\nNew Mem Table Created, MEM-ID : %d", memid);
											}

											else
											{
												sprintf(buffer,"Mem Creation Failed");
											}
											LogEvent(buffer, INFO_MSG);
						break;

					case ST1_XCOM_GUI_MEM_DELETE: 
											cnt = j = 0;

											Msg_Sub_Type = ST1_XCOM_GUI_ACK_MEM_DELETE;

											for(i=0;Recvd_Str[i]!='\0' || i< 20;i++)
											{
												if(Recvd_Str[i] == ',') { cnt++; j=0; continue; }

												switch(cnt)
												{
													case 0 : ID[j++] = Recvd_Str[i]; break;
													case 1 : Name[j++] = Recvd_Str[i]; break;
													default : break;
												}
											}

											memid = atoi(ID);

											//printf("ID : %d, Name:%s",memid, Name);

											ret_code = MemDestroy(memid);

											if(!ret_code)
											{
												sprintf(buffer,"Mem Table :%d Deleted. Ret Code :%d", memid, ret_code);
											}
											else
											{
												sprintf(buffer,"FAILED, Deleting Mem Table : %d. Ret Code :%d ", memid, ret_code);
												LogEvent(buffer, INFO_MSG);
											}
						break;
				}

				//Krishna@TF: For any update on Memory Tables, get the latest info of SHM structure & post it to XCOMTOOL
				getXcomParams();
				
				sendXcomParams();

				p_msg_out = ptemsg_build_msg( MT_XCOM_GUI_ACK_MEM,
											  Msg_Sub_Type,
											  0,
											  NULL_PTR,
											  NULL_PTR,
											  (pBYTE)buffer,
											  (long)strlen(buffer),
											  (BYTE)0 );

				if (p_msg_out == NULL_PTR)
				{
					strcpy (strError, "Insufficient Memory to build terminal response message") ;
					return;
				}

		  break;

		 case MT_XCOM_GUI_STATUS_UPDATE :

				//printf("\n Request recvd for STATUS_UPDATE");

				getXcomParams();
				
				sendXcomParams();
				
				/*p_msg_out = ptemsg_build_msg( MT_XCOM_GUI_STATUS_UPDATE,
								  0,
								  0,
								  NULL_PTR,
								  NULL_PTR,
								  (pBYTE)xsvr_buffer,  //(pBYTE)&xsvrusr,
								  strlen(xsvr_buffer),//(long)sizeof(struct XCOMSERVER),
								  (BYTE)0 );  */
				
		 break;

		case MT_XCOM_GUI_USER : 
			switch(ptemsg_get_msg_subtype1( p_msg_in))
			{
				case ST1_XCOM_GUI_USER_DELETE : 
											cnt = j = 0;

											Msg_Sub_Type = ST1_XCOM_GUI_ACK_USER_DELETE;

											memset(&ID, 0, sizeof(ID));
											memset(&Name, 0, sizeof(Name));

											for(i=0;Recvd_Str[i]!='\0' || i< 20;i++)
											{
												if(Recvd_Str[i] == ',') { cnt++; j=0; continue; }

												switch(cnt)
												{
													case 0 : ID[j++] = Recvd_Str[i]; break;
													case 1 : Name[j++] = Recvd_Str[i]; break;
													default : break;
												}
											}

											userid = atoi(ID);

											ret_code = XipcAbort(userid);

											sprintf(buffer,"User ID : %d Deleted. Status is : %d", userid, ret_code);
											LogEvent(buffer, INFO_MSG);
					break;
			}

			//Krishna@TF: For any update on User info, get the latest info of SHM structure & post it to XCOMTOOL
			getXcomParams();
				
			sendXcomParams();

			p_msg_out = ptemsg_build_msg( MT_XCOM_GUI_ACK_USER,
											  Msg_Sub_Type,
											  0,
											  NULL_PTR,
											  NULL_PTR,
											  (pBYTE)buffer,
											  (long)strlen(buffer),
											  (BYTE)0 );

				if (p_msg_out == NULL_PTR)
				{
					strcpy (strError, "Insufficient Memory to build terminal response message") ;
					return;
				}
		
			break;

	  case MT_XCOM_GUI_FLUSH : 

		  p_msg_out = ptemsg_build_msg( MT_XCOM_GUI_FLUSH_ACK,
											  0,
											  0,
											  NULL_PTR,
											  NULL_PTR,
											  (pBYTE)buffer,
											  (long)strlen(buffer),
											  (BYTE)0 );

				if (p_msg_out == NULL_PTR)
				{
					strcpy (strError, "Insufficient Memory to build terminal response message") ;
					LogEvent(buffer, INFO_MSG);
					return;
				}				

			   Q_Cnt = 0;
			   
			   return;
								
		  break;

      default: LogEvent("PinnacleMsgHandler: unknown msg_type received", WARN_MSG);
	  break;
   }  /* switch msg_type */

    if(p_msg_out != NULL_PTR)
	{
		ptemsg_set_orig_comms_info( p_msg_out, orig_info );
		retCode = (unsigned char)pteipc_send( p_msg_out, orig_queue );
		//printf("Qsend() retcode : %d", retCode);

		/*rcode = que_id = QueAccess (orig_queue);

		if (que_id >= 0)
		{
			QueListBuild (send_quelist, que_id, QUE_EOL);

			retCode = QueSend (QUE_Q_ANY, send_quelist, p_msg_out, ptemsg_get_msg_length (p_msg_out), 
							1L, NULL_PTR, QUE_NOWAIT);
		}*/
		
		if( retCode < 0 )
		{
			// attempt to send response failed. Log the error
			pteipc_get_errormsg( retCode, strError );
			sprintf( buffer, "Ascendent: Service Manager: Error sending response to %s - %s. Ret code : %d", orig_queue, strError, retCode );
			//printf( "%s\n", buffer );
			LogEvent( buffer, ERROR_MSG );
		}
	  
		if( p_msg_out != NULL_PTR )
		 free( p_msg_out );	
	}

} /* PinnacleMsgHandler */


