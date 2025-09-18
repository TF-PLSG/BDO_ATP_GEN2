/* xcom.c : Defines the entry point for the Shared object.

    Include -lrt in the link flags while compiling/linking with
    XCOM. This dependency is due to nanosleep() function called in
    QueGet() below.
*/

#include <time.h>
#include "xcom.h"
#include "xipc.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <syslog.h>

#include <stdarg.h>

#define MAX_COMM_THREADS MAX_USERS
int ret_code;
void logger(int id, char *buffer);
int find_empty_slot();
int post_commands();
void logoff();
int getCommPos();
int getCommPos_falcon(int id);
int ChangeCommPosition(int pos);

struct SHR_MEM_LAYOUT *shm_address;
char pid[10];
struct COMM_LAYOUT *callee[MAX_COMM_THREADS] = {NULL};
char signature[10] = {"XCOMSIGN"};
int callee_tid[MAX_COMM_THREADS] = {0};
int no_of_callee = 0;
char buffer[256];
int err_code;

#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					)
{
    return TRUE;
}
#endif

INT XipcMaxThreads = 100;

// This has to be a dynamic structure based on configuration
// profile pte_ipc.cfg
//
// This is only for testing purpose.
//

// External variables required by MyMalloc library.
extern void *sharedMemAllocStartAddr;
extern void *sharedMemBaseAddr;
extern void *endOfSharedmemory;

#define sharedMemEndAddr (sharedMemAllocStartAddr + SIZEOFSHMSEG)

int getCommPos_falcon(int id)
{
	int i = 0;

	for(i=0;i<MAX_COMM_THREADS;i++)
	{
		if(callee_tid[i] == id)
			break;
	}
	if(i == MAX_COMM_THREADS)
		i = -1;
	return i;
}
int getCommPos()
{
	int i = 0;
	int id = pthread_self();
	for(i=0;i<MAX_COMM_THREADS;i++)
	{
		if(callee_tid[i] == id)
			break;
	}
	if(i == MAX_COMM_THREADS)
		i = -1;
	return i;
}
int findCommPos(int *retcode)
{
	int i = 0;
	int id = pthread_self();
	for(i=0;i<MAX_COMM_THREADS;i++)
	{
		if(callee_tid[i] == id)
		{
			*retcode = XIPC_ER_DUPLICATE;
			break;
		}
		if(callee_tid[i] == -1 || callee[i] == NULL)
		{
			*retcode = XIPC_SUCCESS;
			break;
		}
	}
	if(i == MAX_COMM_THREADS)
	{
		*retcode = XIPC_FAILURE;
		i = -1;
	}
	return i;
}
int ChangeCommPosition(int pos)
{
	int i=0;
	for(i = pos;i<no_of_callee;i++)
	{
		callee[i] = callee[i+1];
		callee_tid[i] = callee_tid[i+1];
	}
	
	return 0;
}

int find_empty_slot() {
    int i = 0;
    int ret = 0;
    for (i = 0; i < MAX_USERS; i++) {
        ret = SEMA_TRYWAIT(&(shm_address->cmdSlot[i].w_lock));
        if (ret == 0) {
			SEMA_TRYWAIT(&(shm_address->cmdSlot[i].response.r_lock));
            break;
        }
    }
    return i;
}

XINT	GLOBAL XAPI XipcLoginEx C_PROTOARGS((CHAR FAR *instance, CHAR FAR *username, XINT build_number))
{
	int slot, i;
	int id;
	XINT temp_ret_val =0;
	int shmid;
	key_t shmkey;
	
	char buf[512];
	char AscendentRoot[256] = {0};
	struct COMM_LAYOUT *comm = NULL;

	if(no_of_callee == 0)
	{
		memset(callee, 0, (sizeof(struct COMM_LAYOUT *) *MAX_COMM_THREADS));
		memset(callee_tid, 0, sizeof(int) * MAX_COMM_THREADS);
		memset(signature, 0, 10);
		strcpy(signature, "XCOMSIGN");
		
		if(getRoot(AscendentRoot) == 1)
		{
			strcat(AscendentRoot,SHMKEYPATH);
		}
		else
		{
			return(-11);
		}
		shmkey = ftok(AscendentRoot,SHMKEYID);
		if ( shmkey == (key_t)-1 )
		{
			sprintf(buffer, "main: ftok() for shm failed\n");
			logger( LOG_ERR, buffer);
			
			return -1;
		}

		/* Get ID of shared memory segment using instance name as key.    */
		/* This memory segment is created by xcom daemon and if it is not */
		/* running, shmget() call below shall fail.                       */
		shmid = shmget(shmkey, SIZEOFSHMSEG, 0666 );
		if (shmid == -1)
		{
			/* Log error to eventlog */
			sprintf(buffer, "Error: XCOM daemon is not started\n");
			logger( LOG_ERR, buffer);
			return -2;
		}

		/* Attach the shared memory segment to the server process. */
		shm_address = (struct SHR_MEM_LAYOUT*) shmat(shmid, NULL, 0);
		if ( shm_address == NULL )
		{
			sprintf(buffer, "main: shmat() failed\n");
			logger( LOG_ERR, buffer);
			return -3;
		}

		//
		//  Initialize globals for MYMALLOC library
		//
		// 'sizeof(SHR_MEM_LAYOUT)' bytes will be used as header.
		// Use rest of the shared memory for queue and user data.
		sharedMemAllocStartAddr = (char *) shm_address + sizeof(struct SHR_MEM_LAYOUT);
		if ( sharedMemAllocStartAddr > sharedMemEndAddr )
		{
			// Shared memory too small
			sprintf (buffer, "Shared Memory too small.\n");
			logger( LOG_ERR, buffer);
			
			return -4;
		}
		// Use base address same as alloc start address.
		sharedMemBaseAddr = sharedMemAllocStartAddr; // as required by mymalloc library
		endOfSharedmemory = sharedMemAllocStartAddr + (SIZEOFSHMSEG - sizeof(struct SHR_MEM_LAYOUT));

	}

	sprintf(buffer, "\n\n XCOM LOGIN called by %s..... \n\n", (char*) username);
	logger ( LOG_INFO, buffer);
	
	slot = find_empty_slot();
	
	if (slot == MAX_USERS)
	{
		sprintf(buffer, "No command slots are available\n");
		logger( LOG_ERR, buffer);
		return -1;
	}

	sprintf(buffer, "Command slot allocated - Slot ID : %d\n",slot);
	logger( LOG_INFO, buffer);

	comm = &(shm_address->cmdSlot[slot]);

	//Need to store the thread id to remember the thread login.
	int cid = findCommPos(&temp_ret_val);

	if(cid > -1)
	{
		if(temp_ret_val == XIPC_ER_DUPLICATE)
		{
			printf(buf, "Not CRITICAL But an ERROR!!! Duplicate Login called from the same thread. Might not have been logged out earlier. updating the new slot. Old slot should be release by xcomcmd %d\n", temp_ret_val);
			logger( LOG_ERR, buf);
		}

		callee_tid[cid] = pthread_self();
		callee[cid] = comm;
		sprintf(buffer, "Thread id = %ld tid = %d : %d\n",pthread_self(), callee_tid[cid], cid);
		logger( LOG_INFO, buffer);
	}
	else
	{
		printf(buf, "CRITICAL!!! Max Thread logon exceeded! %d. This application will not respond anymore. Restart this process. RetCode = %d\n",MAX_COMM_THREADS, temp_ret_val);
		logger( LOG_ERR, buf);
		//Free the locks made by FindCmdSlot
		SEMA_POST(&(comm->w_lock));	//Release only write lock and findCmdSlot will check the w_lock first.
       // SEMA_POST(&(comm->response.r_lock)); // No release of response lock. As it should be locked. No threads are waiting on this lock.
		return -1;
	}

	comm->command.cmd = USERLOGIN;

	comm->command.cmdStruct.UserLogin.Pid = getpid();
	comm->command.cmdStruct.UserLogin.Tid = pthread_self();
	strcpy(comm->command.cmdStruct.UserLogin.Username,username);
	strcpy(comm->command.cmdStruct.UserLogin.Instance,instance);

	no_of_callee++;
	//cid++;
	SEMA_POST_L(&(comm->command.c_lock));
	SEMA_WAIT_L(&(comm->response.r_lock));
	temp_ret_val = comm->response.respStruct.UserLogin.RetUid;
	if(temp_ret_val < 0)
	{
		sprintf(buffer, "Login error for user %s %d\n",username, temp_ret_val);
		logger( LOG_INFO, buffer);
	}
	return temp_ret_val;
}

XINT	GLOBAL XAPI	QueAccess C_PROTOARGS((CHAR FAR *arg1))
{
	struct COMM_LAYOUT *comm = NULL;

	XINT temp_ret_val = QUE_ER_BADQUENAME;
	MSGHDR msgHdr;
	char buffer[512];
	
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
	{
		sprintf(buffer, "QA: pos=%d c_tid[0]=%ld c[0]=0x%16X  %s thread_id = %d\n",pos, callee_tid[0], callee[0], signature, pthread_self());
		logger( LOG_ERR, buffer);
		return XIPC_ER_NOTLOGGEDIN;
	}
	if(arg1 != NULL && arg1[0] != '\0')
	{
		comm->command.cmd = QUEUEACCESS;
		comm->command.cmdStruct.QueAccess.MsgHdr = msgHdr;
		strcpy(comm->command.cmdStruct.QueAccess.Name,arg1);
		comm->command.cmdStruct.QueAccess.NameLen = strlen(arg1);
		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		temp_ret_val = comm->response.respStruct.QueAccess.RetCode;
		if(temp_ret_val  < 0)
		{
			if((strstr(comm->user.username,"qcheck") != NULL))
			{
				//No log print for qcheck tool
			}
			else
			{
				sprintf(buffer, "ERROR in QueAccess (%s) \t QueID %d \t pid=%d from user = %s\n",arg1,temp_ret_val,getpid(),comm->user.username);
				syslog ( LOG_INFO, buffer);
			}
		}
		else
		{
			temp_ret_val = comm->response.respStruct.QueAccess.RetQid;
		}
	}
	return temp_ret_val;
}


XINT	GLOBAL XAPI	QueAccess_falcon C_PROTOARGS((CHAR FAR *arg1, XINT t_id ))
{
	struct COMM_LAYOUT *comm = NULL;

	XINT temp_ret_val = QUE_ER_BADQUENAME;
	MSGHDR msgHdr;
	char buffer[512];


	int pos = getCommPos_falcon(t_id);		/*int pos = pthread_self();*/

	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
	{
		sprintf(buffer, "QA: pos=%d c_tid[0]=%ld c[0]=0x%16X  %s thread_id = %d\n",pos, callee_tid[0], callee[0], signature, pthread_self());
		logger( LOG_ERR, buffer);
		return XIPC_ER_NOTLOGGEDIN;
	}
	if(arg1 != NULL && arg1[0] != '\0')
	{
		comm->command.cmd = QUEUEACCESS;
		comm->command.cmdStruct.QueAccess.MsgHdr = msgHdr;
		strcpy(comm->command.cmdStruct.QueAccess.Name,arg1);
		comm->command.cmdStruct.QueAccess.NameLen = strlen(arg1);
		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		temp_ret_val = comm->response.respStruct.QueAccess.RetCode;
		if(temp_ret_val  < 0)
		{
			if((strstr(comm->user.username,"qcheck") != NULL))
			{
				//No log print for qcheck tool
			}
			else
			{
				sprintf(buffer, "ERROR in QueAccess (%s) \t QueID %d \t pid=%d from user = %s\n",arg1,temp_ret_val,getpid(),comm->user.username);
				syslog ( LOG_INFO, buffer);
			}
		}
		else
		{
			temp_ret_val = comm->response.respStruct.QueAccess.RetQid;
		}
	}
	return temp_ret_val;
}
XINT		GLOBAL XAPI	QueSpool C_PROTOARGS((XINT qid, CHAR FAR *basename))
{
	struct COMM_LAYOUT *comm = NULL;

	XINT temp_ret_val =0;
	MSGHDR msgHdr;

	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	if(basename != NULL)
	{
		comm->command.cmd = QUEUESPOOL;
		
		comm->command.cmdStruct.QueSpool.arg1 = qid;
		strcpy(comm->command.cmdStruct.QueSpool.arg2,basename);
		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		temp_ret_val = comm->response.respStruct.QueSpool.RetCode;
		if( temp_ret_val == 0)
		{
			temp_ret_val = comm->response.respStruct.QueSpool.RetQid;
		}
	}
	return temp_ret_val;
}

XINT		GLOBAL XAPI	QueDestroy C_PROTOARGS((XINT qid))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = QUE_ER_BADQID;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	char buf[512];
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );

	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;
	if(qid > -1)
	{
		sprintf(buf,"Queue to be Destroyed = %d in tid = %d\tslot = %d\n",qid,pthread_self(),comm->place_holder_id);
		logger(LOG_INFO,buf);
		comm->command.cmd = QUEUEDESTROY;
		
		comm->command.cmdStruct.QueDelete.Qid = qid;
		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		
		temp_ret_val = comm->response.respStruct.QueDelete.RetCode;
	}
	return temp_ret_val;
}

XINT		GLOBAL XAPI	QuePurge C_PROTOARGS((XINT qid))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val =QUE_ER_BADQID;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	if(qid > -1)
	{
		comm->command.cmd = QUEUEPURGE;
		comm->command.cmdStruct.QueDelete.Qid = qid;

		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));

		temp_ret_val = comm->response.respStruct.QueDelete.RetCode;
	}
	return temp_ret_val;
}

XINT		GLOBAL XAPI	QueRead C_PROTOARGS((MSGHDR FAR *pMsgHdr, XANY FAR *pMsg, XINT hdrSize))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = QUE_ER_NOTFOUND;
	char buf[512];
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		{
		sprintf(buffer, "QR: pos=%d c_tid[0]=%ld c[0]=0x%16X  %s thread_id = %d\n",pos, callee_tid[0], callee[0], signature, pthread_self());
		logger( LOG_ERR, buffer);
		return XIPC_ER_NOTLOGGEDIN;
	}

	if( pMsgHdr != NULL && pMsg!= NULL & hdrSize > 0)
	{
		comm->command.cmd = QUEUEREAD;
		memcpy(&(comm->command.cmdStruct.QueRead.MsgHdr),pMsgHdr,sizeof(MSGHDR));

		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		temp_ret_val = comm->response.respStruct.QueRead.RetCode;
		if(temp_ret_val == 0)
		{
			// Before using the data, convert it from offset to an absolute address
			if ( comm->response.respStruct.QueRead.data != NULL )
			{
				comm->response.respStruct.QueRead.data = (struct CHUNK_HEADER *)( ((char*)sharedMemBaseAddr) +
				        (unsigned long)(comm->response.respStruct.QueRead.data) );
			
			if(hdrSize != comm->response.respStruct.QueRead.MsgLen)
			{
				//printf("ERROR: Mismatch in msg size - hdrSize = %d, msglen = %d",hdrSize, comm->response.respStruct.QueRead.MsgLen);
				sprintf(buf,"ERROR: Mismatch in msg size - hdrSize = %d, msglen = %d",hdrSize, comm->response.respStruct.QueRead.MsgLen);
				logger(LOG_ERR, buf);
				FILE *llptr = fopen("xcom_applnk_trace.log","w");
				if(llptr != NULL)
				{
					fwrite(comm->response.respStruct.QueRead.data, comm->response.respStruct.QueRead.MsgLen, 1, llptr);
					fclose(llptr);
				}
			}
				memcpy(pMsg, comm->response.respStruct.QueRead.data, hdrSize);
				//memcpy(pMsg, comm->response.respStruct.QueRead.data, comm->response.respStruct.QueRead.MsgLen);
				temp_ret_val = comm->response.respStruct.QueRead.MsgLen;
				myfree(comm->response.respStruct.QueRead.data);
				//This myfree is freeing the memory allocated by QueSend
			}
			else
			{
				sprintf(buf,"ERROR in QueRead. Data is NULL\n");
				logger(LOG_INFO,buf);
			}
		}
		else
		{
			sprintf(buf,"ERROR in QueRead. RetCode= %d\n",temp_ret_val);
			logger(LOG_INFO,buf);
		}
	}
	return temp_ret_val;
}

PQIDLIST	FAR XCDECL	QueListBuild C_PROTOARGS((PQIDLIST qidList, ...))
{
	PQIDLIST temp_ret_val=0;
	char msg[256] = "";
	XINT i, j = 0;

	va_list varg;
	XINT varg_x = ((XINT)-2);

	//memset (qidList, sizeof(QIDLIST), 0x0);
	memset (qidList, 0, sizeof(QIDLIST));

	va_start ( varg, qidList );
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	for (i = 0; i < QUE_LEN_QIDLIST &&  varg_x != QUE_EOL ; i++)
	{
	        varg_x = va_arg(varg, XINT);
	        if ( varg_x == QUE_EOL )
	                break;

	        // We have valid queid - should be greater than equal to zero.
	        if ( varg_x >= 0 )
	        {
	                qidList[j++] = varg_x;
	        }
	        else if ( varg_x == QUE_M_EA_CODE )
	        {
	                // Ignore the next two zero arguments (see example call
	                // in read_from_queues() ATP function).
	                varg_x = va_arg(varg, XINT);
	                varg_x = va_arg(varg, XINT);
	        }
	        else
	        {	// Invalid Que-id
	                sprintf (msg, "ERROR (%d) - Invalid Que-ID's passed to QueListBuild().\n",QUE_ER_BADQID);
					logger(LOG_ERR, msg);
	                return NULL;	// Invalid Queue id
	        }
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	va_end ( varg );
	qidList[j++] = QUE_EOL;	// End of array. Value is -1

	return qidList;
}

/*
arg1  - QUE_Q_ANY | arg2 - send_quelist | arg3 - pMsg | arg4 - msgLength | arg5 - priority | arg6 - NULLPTR | arg7 - Wait option | arg8 to arg10 Not used
*/

XINT		GLOBAL XAPI	QueSend C_PROTOARGS((XINT arg1,
											PQIDLIST qidList, XANY FAR *pMsg, XINT msgLen,
											XINT priority, XINT FAR *nullPtr, XINT waitOption, XINT arg8,
											XANY FAR *arg9, XIPC_CALLBACK_FUNC FAR *arg10))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val =0;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		{
		sprintf(buffer, "QS: pos=%d c_tid[0]=%ld c[0]=0x%16X  %s thread_id = %d\n",pos, callee_tid[0], callee[0], signature, pthread_self());
		logger( LOG_ERR, buffer);
		return XIPC_ER_NOTLOGGEDIN;
	}

	if(qidList != NULL && pMsg!= NULL & msgLen > 0)
	{
		#ifdef XCOMDEBUG
			sprintf(buffer, "%s QueSend to %s\n",comm->user.username,shm_address->xcomQueues.userQueues[qidList[0]].queueName);
			logger ( LOG_INFO, buffer);
		#endif 
		comm->command.cmd = QUEUESEND;
		memcpy(comm->command.cmdStruct.QueSend.QueList,qidList,sizeof(comm->command.cmdStruct.QueSend.QueList));

			// IMPORTANT:
			//
			// Allocate memory to store this data packet in shared memory.
			// This Data will be shared between processes. This address is
			// pointing to a location in shared memory. As the same shared
			// memory may get attached to different logical addresses in
			// different processes, absolute addresses must be converted to
			// 'offsets' into the shared memory before they can be shared
			// across multiple processes.
			//
			// Example illustration:
			// ----------------------
			//		Process 1
			// - Shared memory is attached to address 200 to 299 (100 bytes)
			// - Allocated address is 140
			// - First two bytes of shared memory needs to store this
			//   address to pass on to other processes
			// - Address stored for passing between different processes
			//   is 40 (140 -100)
			//   i.e., ( [absolute address] - [base address] )
			// - 40 gets written in first two bytes of the shared memory at
			//    addresses 100-101
			//
			// 		Process 2
			// - Another process attaching the same 100 bytes of shared
			// memory may get starting address as 500 (from addr 500 to 599)
			// - Shared memory location (first two bytes), that stores the
			//   address written by process 1, is 40 (from location 500
			//   and 501)
			// - Process 2 adds its base address (500) to get absolute
			//   address in process-2's memory = 540
			//      i.e. [offset] + [base address]
			//
			// All address links inside shared memory must be stored as
			// offsets.
			//
			comm->command.cmdStruct.QueSend.Data = mymalloc(msgLen);
			//This mymalloc will be freed in QueRead
		if(comm->command.cmdStruct.QueSend.Data == NULL)
		{
		    sprintf(buffer, "Mymalloc for %d failed in QueSend(xcom.c) Line %d\n", msgLen, __LINE__ );
			logger ( LOG_ERR, buffer);
			return -1;
		}
		memcpy(comm->command.cmdStruct.QueSend.Data, pMsg, msgLen);
		// IMPORTANT: Now convert this address to an offset as explained above
		if ( comm->command.cmdStruct.QueSend.Data != NULL )
		{
		// This offset will be converted back to absolute address in xcomdaemon,
		// if required.
		comm->command.cmdStruct.QueSend.Data = (struct CHUNK_HEADER*)
		            ( (char*)(comm->command.cmdStruct.QueSend.Data) - (char*)sharedMemBaseAddr );
		}
		//Only for users sending to dialog will fail. Other process quesend will be on indefinite wait
		if(0 == strcmp(comm->user.username,"tswtpdu"))
		{
			comm->command.cmdStruct.QueSend.WaitOption = waitOption;
		}
		else
		{
			comm->command.cmdStruct.QueSend.WaitOption = XIPC_BLOCKOPT_WAIT;
		}
		comm->command.cmdStruct.QueSend.DataLen = msgLen;
		comm->command.cmdStruct.QueSend.Priority = priority;
		

		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		temp_ret_val = comm->response.respStruct.QueSend.RetCode;
		if(temp_ret_val == 0)
		{
			//temp_ret_val = comm->response.respStruct.QueSend.DataLen;
		}
		else
		{
			// Free this chunk as daemon couldn't add it to the send queue.
			// If we do not free it here, this chunk will be lost and cause
			// memory leak, untimately resulting in filling up entire shared
			// memory.
			if ( comm->command.cmdStruct.QueSend.Data != NULL )
			{
				comm->command.cmdStruct.QueSend.Data = (struct CHUNK_HEADER *)( ((char*)sharedMemBaseAddr) +
				        (unsigned long)(comm->command.cmdStruct.QueSend.Data) );
				myfree(comm->command.cmdStruct.QueSend.Data);
			}
			
			 sprintf(buffer, "ERROR in QueSend (%d) RetCode = %d\n",qidList[0], temp_ret_val);
			logger ( LOG_ERR, buffer);
			return -2;
		}
	}

	return temp_ret_val;
}
/*
arg1  - QUE_Q_ANY | arg2 - send_quelist | arg3 - pMsg | arg4 - msgLength | arg5 - priority | arg6 - NULLPTR | arg7 - Wait option | arg8 to arg10 Not used
*/

XINT		GLOBAL XAPI	QueSend_falcon C_PROTOARGS((XINT arg1,
											PQIDLIST qidList, XANY FAR *pMsg, XINT msgLen,
											XINT priority, XINT FAR *nullPtr, XINT waitOption, XINT arg8,
											XANY FAR *arg9, XIPC_CALLBACK_FUNC FAR *arg10, XINT t_id))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val =0;

	int pos = getCommPos_falcon(t_id);		/*int pos = pthread_self();*/

	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		{
		sprintf(buffer, "QS: pos=%d c_tid[0]=%ld c[0]=0x%16X  %s thread_id = %d\n",pos, callee_tid[0], callee[0], signature, pthread_self());
		logger( LOG_ERR, buffer);
		return XIPC_ER_NOTLOGGEDIN;
	}

	if(qidList != NULL && pMsg!= NULL & msgLen > 0)
	{
		#ifdef XCOMDEBUG
			sprintf(buffer, "%s QueSend to %s\n",comm->user.username,shm_address->xcomQueues.userQueues[qidList[0]].queueName);
			logger ( LOG_INFO, buffer);
		#endif
		comm->command.cmd = QUEUESEND;
		memcpy(comm->command.cmdStruct.QueSend.QueList,qidList,sizeof(comm->command.cmdStruct.QueSend.QueList));

			// IMPORTANT:
			//
			// Allocate memory to store this data packet in shared memory.
			// This Data will be shared between processes. This address is
			// pointing to a location in shared memory. As the same shared
			// memory may get attached to different logical addresses in
			// different processes, absolute addresses must be converted to
			// 'offsets' into the shared memory before they can be shared
			// across multiple processes.
			//
			// Example illustration:
			// ----------------------
			//		Process 1
			// - Shared memory is attached to address 200 to 299 (100 bytes)
			// - Allocated address is 140
			// - First two bytes of shared memory needs to store this
			//   address to pass on to other processes
			// - Address stored for passing between different processes
			//   is 40 (140 -100)
			//   i.e., ( [absolute address] - [base address] )
			// - 40 gets written in first two bytes of the shared memory at
			//    addresses 100-101
			//
			// 		Process 2
			// - Another process attaching the same 100 bytes of shared
			// memory may get starting address as 500 (from addr 500 to 599)
			// - Shared memory location (first two bytes), that stores the
			//   address written by process 1, is 40 (from location 500
			//   and 501)
			// - Process 2 adds its base address (500) to get absolute
			//   address in process-2's memory = 540
			//      i.e. [offset] + [base address]
			//
			// All address links inside shared memory must be stored as
			// offsets.
			//
			comm->command.cmdStruct.QueSend.Data = mymalloc(msgLen);
			//This mymalloc will be freed in QueRead
		if(comm->command.cmdStruct.QueSend.Data == NULL)
		{
		    sprintf(buffer, "Mymalloc for %d failed in QueSend(xcom.c) Line %d\n", msgLen, __LINE__ );
			logger ( LOG_ERR, buffer);
			return -1;
		}
		memcpy(comm->command.cmdStruct.QueSend.Data, pMsg, msgLen);
		// IMPORTANT: Now convert this address to an offset as explained above
		if ( comm->command.cmdStruct.QueSend.Data != NULL )
		{
		// This offset will be converted back to absolute address in xcomdaemon,
		// if required.
		comm->command.cmdStruct.QueSend.Data = (struct CHUNK_HEADER*)
		            ( (char*)(comm->command.cmdStruct.QueSend.Data) - (char*)sharedMemBaseAddr );
		}
		//Only for users sending to dialog will fail. Other process quesend will be on indefinite wait
		if(0 == strcmp(comm->user.username,"tswtpdu"))
		{
			comm->command.cmdStruct.QueSend.WaitOption = waitOption;
		}
		else
		{
			comm->command.cmdStruct.QueSend.WaitOption = XIPC_BLOCKOPT_WAIT;
		}
		comm->command.cmdStruct.QueSend.DataLen = msgLen;
		comm->command.cmdStruct.QueSend.Priority = priority;


		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		temp_ret_val = comm->response.respStruct.QueSend.RetCode;
		if(temp_ret_val == 0)
		{
			//temp_ret_val = comm->response.respStruct.QueSend.DataLen;
		}
		else
		{
			// Free this chunk as daemon couldn't add it to the send queue.
			// If we do not free it here, this chunk will be lost and cause
			// memory leak, untimately resulting in filling up entire shared
			// memory.
			if ( comm->command.cmdStruct.QueSend.Data != NULL )
			{
				comm->command.cmdStruct.QueSend.Data = (struct CHUNK_HEADER *)( ((char*)sharedMemBaseAddr) +
				        (unsigned long)(comm->command.cmdStruct.QueSend.Data) );
				myfree(comm->command.cmdStruct.QueSend.Data);
			}

			 sprintf(buffer, "ERROR in QueSend (%d) RetCode = %d\n",qidList[0], temp_ret_val);
			logger ( LOG_ERR, buffer);
			return -2;
		}
	}

	return temp_ret_val;
}

XINT		GLOBAL XAPI	QueGet (
						MSGHDR FAR * MsgHdr,
						XINT QueSelectCode,
						PQIDLIST QidList,
						XINT FAR * PriorityPtrArg,
						XINT FAR * QidPtrArg,
						XINT  waitOption,
						XINT  timeoutOption,
						XANY  FAR * Option3,
						XIPC_CALLBACK_FUNC FAR * Option4)
{
	//Option3 and Option4 are not used
	struct COMM_LAYOUT *comm = NULL;
	//int ret;
	TIMESPEC_T timeout;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		{
		sprintf(buffer, "QG: pos=%d c_tid[0]=%ld c[0]=0x%16X  %s thread_id = %d\n",pos, callee_tid[0], callee[0], signature, pthread_self());
		logger( LOG_ERR, buffer);
		return XIPC_ER_NOTLOGGEDIN;
	}
	if(MsgHdr != NULL)
	{
		comm->command.cmd = QUEUEGET;
		memcpy(comm->command.cmdStruct.QueGet.QueList,QidList,sizeof(comm->command.cmdStruct.QueGet.QueList));

		comm->command.cmdStruct.QueGet.Priority = QueSelectCode;
		comm->command.cmdStruct.QueGet.Timeout = timeoutOption;
		comm->command.cmdStruct.QueGet.WaitOption = waitOption;
        // send the command to xcom daemon
        SEMA_POST_L(&(comm->command.c_lock));
        SEMA_WAIT_L(&(comm->response.r_lock));

        // response received
        ret_code = comm->response.respStruct.QueGet.RetCode;
        if(ret_code == QUE_SUCCESS)
            memcpy(MsgHdr, &(comm->response.respStruct.QueGet.MsgHdr), sizeof(MSGHDR));
    }
    else
	{
        ret_code = -5;    // "ERROR: Invalid arguments" - use correct error code.
		sprintf(buffer, "ERROR in QueGet - MsgHdr is NULL\t User Name = %s\n",comm->user.username);
		logger( LOG_INFO, buffer);
	}
	return ret_code;
}

XINT GLOBAL XAPI XipcLogout P0()
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val =0;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
	{
		if(comm != NULL)
		{
			{
				sprintf(buffer, "LO1: pos=%d c_tid[0]=%ld c[0]=0x%16X  %s thread_id = %d\n",pos, callee_tid[0], callee[0], signature, pthread_self());
				logger( LOG_ERR, buffer);
			}
			sprintf(buffer, "XipcLogoutFailed - Not logged-in. %s - %d\n",comm->user.username, comm->user.userid);
			logger ( LOG_ERR, buffer);
		}
		else
		{
			{
			sprintf(buffer, "LO2: pos=%d c_tid[0]=%ld c[0]=0x%16X  %s thread_id = %d\n",pos, callee_tid[0], callee[0], signature, pthread_self());
			logger( LOG_ERR, buffer);

			}
			logger ( LOG_ERR, "XipcLogoutFailed - Not logged-in.\n");
		}
		temp_ret_val = -1;
	}
	else
	{
		//*((int*)sharedMemAllocStartAddr) -= 1;	// deccrement - num of logged-in users.
		comm->command.cmd = USERLOGOUT;
		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));

		sprintf(buffer, "USerLogout %s ret code %d - Slot id %d\n",comm->user.username,comm->response.respStruct.UserLogout.RetCode,comm->place_holder_id );
		logger ( LOG_INFO, buffer);

		comm->command.cmd = 0;
		comm->response.resp = 0;

		//Unlock the write lock of the command slot. Basically freeing the command slot
		temp_ret_val = SEMA_TRYWAIT(&(comm->w_lock));
		if (temp_ret_val == 0 || temp_ret_val == -1)
		{
			SEMA_POST(&(comm->w_lock));
			sprintf(buffer, "XipcLogout - Slot released\n");
			logger ( LOG_INFO, buffer);
		}
		comm = NULL;
		callee_tid[pos] = -1;
		callee[pos] = NULL;
		no_of_callee--;
		//if(no_of_callee == 0)
		//	cid = 0;
	}

	return temp_ret_val;
}


XINT QueFreeze(void)
{
		//Not used in ATP
	return 0;
}

XINT GLOBAL XAPI	QueCreate C_PROTOARGS((CHAR FAR *qname, XINT limit1, XINT limit2))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = -1;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );

	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		{
		sprintf(buffer, "QC: pos=%d c_tid[0]=%ld c[0]=0x%16X  %s thread_id = %d\n",pos, callee_tid[0], callee[0], signature, pthread_self());
		logger( LOG_ERR, buffer);
		return XIPC_ER_NOTLOGGEDIN;
	}
	if(qname != NULL)
	{
		comm->command.cmd = QUEUECREATE;
		strcpy(comm->command.cmdStruct.QueCreate.Name,qname);
		comm->command.cmdStruct.QueCreate.NameLen = strlen(qname);
		comm->command.cmdStruct.QueCreate.Pid = getpid();

		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		temp_ret_val = comm->response.respStruct.QueCreate.RetCode;
		if(temp_ret_val == 0)
		{
			temp_ret_val = comm->response.respStruct.QueCreate.RetQid;
			#ifdef XCOMDEBUG
				sprintf(buffer, "QueCreate QueName = %s\tRetCode = %d\n",qname,temp_ret_val);
				logger ( LOG_INFO, buffer);
			#endif
		}		
		else
		{
			temp_ret_val = comm->response.respStruct.QueCreate.RetCode;
			sprintf(buffer, "ERROR in QueCreate RetCode = %d\n",temp_ret_val);
			logger ( LOG_ERR, buffer);
		}
	}
	return temp_ret_val;
}

XINT	GLOBAL XAPI	QueDelete C_PROTOARGS((XINT qid))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = QUE_ER_BADQID;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	if(qid > -1)
	{
		comm->command.cmd = QUEUEDELETE;
		comm->command.cmdStruct.QueDelete.Qid = qid;

		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));

		temp_ret_val = comm->response.respStruct.QueDelete.RetCode;
	}
	return temp_ret_val;
}

XINT	GLOBAL XAPI	QueInfoQue C_PROTOARGS((XINT qid, QUEINFOQUE FAR *infoQue))
{
	//Returns the uid of the user created it.
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = QUE_ER_BADQID;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );

	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	if(infoQue != NULL)
	{
		comm->command.cmd = QUEUEINFOQUEUE;
		comm->command.cmdStruct.QueInfoQue_S.Qid = qid;

		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		
		memcpy(infoQue, &(comm->response.respStruct.QueInfoQue_S.InfoQue), sizeof(QUEINFOQUE));
		temp_ret_val = comm->response.respStruct.QueInfoQue_S.RetCode;
		if(temp_ret_val == QUE_SUCCESS)
		{
			temp_ret_val = infoQue->CreateUid;
		}
		else
		{
			sprintf(buffer, "ERROR in QueInfoQue RetCode = %d Qid=%d\n",temp_ret_val,qid);
			//logger ( LOG_INFO, buffer);
		}
	}
	return temp_ret_val;
}

XINT	GLOBAL XAPI	QueInfoUser C_PROTOARGS((XINT uid, QUEINFOUSER FAR *queUsrInfo))
{
	//returns pid of the process created the que
	XINT temp_ret_val =0;
	XIPCINFOUSER usrInfo;
	memset(&usrInfo,0,sizeof(XIPCINFOUSER));
	temp_ret_val = XipcInfoUser(uid,&usrInfo);
	if(temp_ret_val == 0)
	{
		queUsrInfo->Pid = usrInfo.Pid;
		temp_ret_val = usrInfo.Pid;
	}
	else
	{
//		sprintf(buffer, "ERROR in QueInfoUser RetCode = %d\n",temp_ret_val);
//		logger ( LOG_ERR, buffer);
	}
	
	return temp_ret_val;
}

XINT	GLOBAL XAPI XipcInfoUser(XINT Uid, XIPCINFOUSER *InfoUser)
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val =0;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );

	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	if(Uid > -1)
	{
		comm->command.cmd = XCOMINFOUSER;

		comm->command.cmdStruct.XcomInfoUser.Uid = Uid;
		
		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));

		temp_ret_val = comm->response.respStruct.XcomInfoUser.RetCode;
		if(temp_ret_val == 0)
		{
			InfoUser->Uid = comm->response.respStruct.XcomInfoUser.Uid;
			InfoUser->Pid = comm->response.respStruct.XcomInfoUser.Pid;
			InfoUser->Tid = comm->response.respStruct.XcomInfoUser.Tid;
			
			strcpy(InfoUser->Name, comm->response.respStruct.XcomInfoUser.Name);
		}
		else
		{
			if(strcasecmp(comm->user.username, "xcomPrompt") != 0)
			{
				sprintf(buffer, "ERROR in XipcInfoUser RetCode = %d\n",temp_ret_val);
				logger ( LOG_ERR, buffer);
			}
		}
	}
	return temp_ret_val;
}

XINT	GLOBAL XAPI	QueInfoSys C_PROTOARGS((QUEINFOSYS FAR *arg1))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val =0;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );

	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	return temp_ret_val;
}

//XCOMABORT
XINT		GLOBAL XAPI XipcAbort C_PROTOARGS((XINT uid))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = 0;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	int	i = uid;

	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );

	// Even though we are deleting another user, we should still be logged in.
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;


	if ( shm_address->cmdSlot[i].user.userid >= 0 && shm_address->cmdSlot[i].user.userid < MAX_COMM_THREADS &&
						shm_address->cmdSlot[i].user.isBeingUsed == 1 )
	{
		// The user block is allocated, so deallocate it. All queues created by this user will not
		// be affected and continue to reside in XCOM memory.
		shm_address->cmdSlot[i].user.isBeingUsed = 0;

		// Reset user structure
		shm_address->cmdSlot[i].user.userid = -1;
		memset( shm_address->cmdSlot[i].user.username, 0x00, sizeof(shm_address->cmdSlot[i].user.username));
		shm_address->cmdSlot[i].user.isWaitingForMsg = 0;
		shm_address->cmdSlot[i].user.startReadTime = 0;
		shm_address->cmdSlot[i].user.timeout = 0;
		// decrement number of logged-in users. It must be validated that this user is not
		// calling logout more than once, else the count of users will be corrupted.
		shm_address->total_logged_in_users -= 1; //Less by one user
		//Unlock the write lock of the command slot. Basically freeing the command slot
		temp_ret_val = SEMA_TRYWAIT(&(shm_address->cmdSlot[i].w_lock));
		if (temp_ret_val == 0 || temp_ret_val == -1)
		{
			SEMA_POST(&(shm_address->cmdSlot[i].w_lock));
		}
		temp_ret_val = 0;
	}
	else
	{
		temp_ret_val = XIPC_ER_BADUID;
		sprintf(buffer, "ERROR in XipcAbort RetCode = %d\n",temp_ret_val);
		logger ( LOG_ERR, buffer);
	}
	return temp_ret_val;
}

XINT		GLOBAL XAPI XipcTrap C_PROTOARGS((INT arg3, XIPCSIGHANDLER *arg2))
{
	struct COMM_LAYOUT *comm = NULL;
/*
    If XipcTrap() returns 0, all dataservers will log an event through
    ATP function TrapFunction().
*/
	XINT temp_ret_val =1;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );

	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	return temp_ret_val;
}

XINT        GLOBAL XAPI XipcSystemErrorReport C_PROTOARGS((CHAR FAR *arg1, XINT arg2))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val =0;

	//	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
	//	return XIPC_ER_NOTLOGGEDIN;

	return temp_ret_val;
}

XINT		GLOBAL XAPI XipcStart C_PROTOARGS((CHAR FAR *arg1, CHAR FAR *arg2, XINT arg3))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val =0;
	return temp_ret_val;
}


/* MEMORY */

XINT GLOBAL XAPI MemAccess C_PROTOARGS((CHAR FAR *tableName))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = MEM_ER_BADSEGNAME;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );

	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	if(tableName != NULL)
	{
		comm->command.cmd = SHMMEMACCESS;
		//strcpy(comm->command.cmdStruct.MemAccess.TableName, tableName);
		memcpy(comm->command.cmdStruct.MemAccess.TableName, tableName,MAX_NAME_LENGTH);
		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		temp_ret_val = comm->response.respStruct.MemAccess.RetCode;
		
		if(temp_ret_val >= 0)
		{
			temp_ret_val = comm->response.respStruct.MemAccess.RetMid;
		}
		else
		{
			sprintf(buffer, "ERROR in MemAccess (%s) RetCode = %d\n",tableName, temp_ret_val);
			logger ( LOG_INFO, buffer);
		}
	}
	return temp_ret_val;
}

XINT	  GLOBAL XAPI MemSecDef C_PROTOARGS((SECTION section))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = MEM_ER_BADSECTION;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	comm->command.cmd = SHMMEMSECDEF;
	memcpy(&(comm->command.cmdStruct.MemSecDef.Section),&section, sizeof(SECTION));
	SEMA_POST_L(&(comm->command.c_lock));
	SEMA_WAIT_L(&(comm->response.r_lock));

	temp_ret_val = comm->response.respStruct.MemSecDef.RetCode;
	if(temp_ret_val < 0)
	{
		sprintf(buffer, "ERROR in MemSecDef RetCode = %d\n", temp_ret_val);
			logger( LOG_ERR, buffer);
	}
	return temp_ret_val;
}

SECTION   GLOBAL XAPI MemSection C_PROTOARGS((XINT mid, XINT offset, XINT size))
{
	SECTION section;

	section.Mid = mid;
	section.Offset = offset;
	section.Size = size;

	return section;
}

XINT GLOBAL XAPI MemCreate C_PROTOARGS((CHAR FAR *tableName, XINT size))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = MEM_ER_BADSEGNAME;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;
	if(tableName != NULL)
	{
		comm->command.cmd = SHMMEMCREATE;
		comm->command.cmdStruct.MemCreate.TableSize = size;
		strcpy(comm->command.cmdStruct.MemCreate.TableName, tableName);
		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		temp_ret_val = comm->response.respStruct.MemCreate.RetCode;
		if(temp_ret_val >= 0)
		{
			temp_ret_val = comm->response.respStruct.MemCreate.RetMid;
		}
	}
	if(temp_ret_val < 0)
	{
		sprintf(buffer, "ERROR in MemCreate %s - %d RetCode = %d\n",tableName,comm->response.respStruct.MemCreate.RetMid, comm->response.respStruct.MemCreate.RetCode);
			logger( LOG_ERR, buffer);
	}
	return temp_ret_val;
}

XINT	  GLOBAL XAPI MemDestroy C_PROTOARGS((XINT mid))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = MEM_ER_BADMID;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	comm->command.cmd = SHMMEMDESTROY;
	comm->command.cmdStruct.MemDestroy.Mid = mid;
	SEMA_POST_L(&(comm->command.c_lock));
	SEMA_WAIT_L(&(comm->response.r_lock));
	temp_ret_val = comm->response.respStruct.MemDestroy.RetCode;
	if(temp_ret_val < 0)
	{
		sprintf(buffer, "ERROR in MemDestroy (%d) RetCode = %d\n",mid, temp_ret_val);
		logger ( LOG_ERR, buffer);
	}
	return temp_ret_val;
}

XINT	  GLOBAL XAPI MemRead C_PROTOARGS(
(XINT mid, XINT offset, XINT size, XANY FAR *buf, XINT option1, XINT option2, XANY FAR *reserved, XIPC_CALLBACK_FUNC *callback))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = MEM_ER_BADBUFFER;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	if(buf != NULL)
	{
		if(mid > -1)
		{
			comm->command.cmd = SHMMEMREAD;
			comm->command.cmdStruct.MemReadWrite.Mid = mid;
			comm->command.cmdStruct.MemReadWrite.Offset = offset;
			comm->command.cmdStruct.MemReadWrite.Length = size;
			comm->command.cmdStruct.MemReadWrite.Buffer = NULL;
			comm->command.cmdStruct.MemReadWrite.Option= option1;

			SEMA_POST_L(&(comm->command.c_lock));
			SEMA_WAIT_L(&(comm->response.r_lock));
			temp_ret_val = comm->response.respStruct.MemReadWrite.RetCode;
			
			if(temp_ret_val == MEM_SUCCESS)
			{
				if(comm->command.cmdStruct.MemReadWrite.Buffer != NULL)
				{
					comm->command.cmdStruct.MemReadWrite.Buffer = (void *)( ((char *)sharedMemBaseAddr) +
																	(unsigned long)(comm->command.cmdStruct.MemReadWrite.Buffer) );

					memcpy(buf, comm->command.cmdStruct.MemReadWrite.Buffer,
								comm->command.cmdStruct.MemReadWrite.Length);
				}
			}
			
		}
		else
			temp_ret_val = MEM_ER_BADSID;
	}
	if(temp_ret_val < 0)
	{
		sprintf(buffer, "ERROR in MemRead (%d) RetCode = %d\n",mid, temp_ret_val );
		logger( LOG_ERR, buffer);

	}
	return temp_ret_val;
}

XINT	  GLOBAL XAPI MemWrite C_PROTOARGS(
(XINT mid, XINT offset, XINT size, XANY FAR *buf, XINT option1, XINT option2, XANY FAR *reserved, XIPC_CALLBACK_FUNC *callback))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = MEM_ER_BADBUFFER;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	if(buf != NULL)
	{
		if(mid > -1)
		{
			comm->command.cmd = SHMMEMWRITE;
			comm->command.cmdStruct.MemReadWrite.Mid = mid;
			comm->command.cmdStruct.MemReadWrite.Offset = offset;
			comm->command.cmdStruct.MemReadWrite.Length = size;

			comm->command.cmdStruct.MemReadWrite.Buffer = NULL;
			comm->command.cmdStruct.MemReadWrite.Option= option1;

			SEMA_POST_L(&(comm->command.c_lock));
			SEMA_WAIT_L(&(comm->response.r_lock));
			temp_ret_val = comm->response.respStruct.MemReadWrite.RetCode;
			if(temp_ret_val == MEM_SUCCESS)
			{
				if(comm->command.cmdStruct.MemReadWrite.Buffer != NULL)
				{
					comm->command.cmdStruct.MemReadWrite.Buffer = (void *)( ((char *)sharedMemBaseAddr) +
																	(unsigned long)(comm->command.cmdStruct.MemReadWrite.Buffer) );
					
					if((int)buf == -1) //buf -1 is used for MEMFILL
					{
						memset(comm->command.cmdStruct.MemReadWrite.Buffer,0,size);
					}
					else
					{
						memcpy(comm->command.cmdStruct.MemReadWrite.Buffer,buf,size);
					}
				}
			}
		}
		else
			temp_ret_val = MEM_ER_BADSID;
	}
	if(temp_ret_val < 0)
	{
		sprintf(buffer, "ERROR in MemWrite (%d) RetCode = %d\n",mid, temp_ret_val );
		logger( LOG_ERR, buffer);
	}
	return temp_ret_val;
}


PMIDLIST  FAR XCDECL  MemList C_PROTOARGS((SECTION sec,...))
{
	PMIDLIST ret_midlist = NULL;
	ret_midlist = (PMIDLIST)malloc(sizeof(SECTION));
	//This allocated memory will be freed in MemLock/MemUnlock functions
	memcpy(ret_midlist, &sec,sizeof(SECTION));
	return ret_midlist;
}

PSECTION  GLOBAL XAPI MemSectionBuild C_PROTOARGS((PSECTION pSection, XINT mid, XINT offset, XINT size))
{
	pSection->Mid = mid;
	pSection->Offset = offset;
	pSection->Size = size;
	
	return pSection;
}


XINT GLOBAL XAPI MemLock (
					XINT lockType,
					PMIDLIST midList,
					SECTION FAR * secPtrArg,
					XINT option1,
					XINT option2,
					XANY FAR * reserved,
					XIPC_CALLBACK_FUNC FAR * callback)
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = MEM_ER_BADSECTION;
	SECTION *section;
	int i = 0;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	if(midList != NULL)
	{
		section = (PSECTION)(midList);
		if(section->Mid < 0)
		{
			sprintf(buffer, "MemLock: Bad mid passed %d\n",section->Mid);
			logger( LOG_ERR, buffer);
			free(midList); // Memory leak fix - Nirbhay
			return -1;
		}
		else
		{
			comm->command.cmd = SHMMEMLOCK;
			comm->command.cmdStruct.MemLock.LockType = lockType;
			comm->command.cmdStruct.MemLock.Option = option1;
			comm->command.cmdStruct.MemLock.MidList.Mid = section->Mid;
			comm->command.cmdStruct.MemLock.MidList.Offset = section->Offset;
			comm->command.cmdStruct.MemLock.MidList.Size = section->Size;
			
			SEMA_POST_L(&(comm->command.c_lock));
			SEMA_WAIT_L(&(comm->response.r_lock));

			memcpy(secPtrArg,section,sizeof(SECTION));
			
			temp_ret_val = comm->response.respStruct.MemLock.RetCode;
		}
		free(midList);
	}
	if(temp_ret_val < 0)
	{
			sprintf(buffer, "ERROR in MemLock (%d) RetCode = %d by %s\n",midList[0].Mid, temp_ret_val,comm->user.username);
			logger( LOG_ERR, buffer);
	}

	return temp_ret_val;
}

XINT GLOBAL XAPI MemUnlock C_PROTOARGS((PMIDLIST midList, PSECTION secPtrArg))
{
	struct COMM_LAYOUT *comm = NULL;
	XINT temp_ret_val = MEM_ER_BADSECTION;
	SECTION *section;
	int i = 0;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );
	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	if(midList != NULL)
	{
		comm->command.cmd = SHMMEMUNLOCK;
		comm->command.cmdStruct.MemUnLock.MidList.Mid = midList->Mid;
		comm->command.cmdStruct.MemUnLock.MidList.Offset = midList->Offset;
		comm->command.cmdStruct.MemUnLock.MidList.Size = midList->Size;

		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));

		temp_ret_val = comm->response.respStruct.MemUnLock.RetCode;
		free(midList);
	}
	if(temp_ret_val < 0)
	{
		sprintf(buffer, "ERROR in MemUnLock (%d) RetCode = %d\n",midList[0].Mid, temp_ret_val );
		logger( LOG_ERR, buffer);
	}

	return temp_ret_val;
}


XINT	  GLOBAL XAPI MemInfoMem C_PROTOARGS((XINT mid, MEMINFOMEM FAR *infoMem))
{
	struct COMM_LAYOUT *comm = NULL;
	signed long ret_code = MEM_ER_BADMID;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );

	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;
	if(mid < 0)
	{
		ret_code = MEM_ER_BADMID;
	}
	else if(infoMem != NULL)
	{
		comm->command.cmd = SHMMEMINFOMEM;
		comm->command.cmdStruct.MemInfoMem.Mid = mid;
		SEMA_POST_L(&(comm->command.c_lock));
		SEMA_WAIT_L(&(comm->response.r_lock));
		memcpy(infoMem, &(comm->response.respStruct.MemInfoMem.InfoMem),sizeof(MEMINFOMEM));
		ret_code = comm->response.respStruct.MemInfoMem.RetCode;
	}
	else
		ret_code = MEM_ER_BADBUFFER;
	if(ret_code < 0)
	{
		sprintf(buffer, "ERROR in MemInfoMem (%d) RetCode = %d\n",mid,ret_code );
		logger( LOG_ERR, buffer);
	}
    return ret_code;
}

XINT	  GLOBAL XAPI MemInfoSys C_PROTOARGS((MEMINFOSYS FAR *infoSys))
{
	struct COMM_LAYOUT *comm = NULL;
	signed long ret_code = MEM_ER_BADBUFFER;
	int pos = getCommPos();		/*int pos = pthread_self();*/
	comm = ( ( pos < 0 || pos >= MAX_COMM_THREADS ) ? NULL : callee[pos] );

	if ( comm == NULL || comm->user.userid < 0 || comm->user.userid >= MAX_COMM_THREADS )
		return XIPC_ER_NOTLOGGEDIN;

	if(infoSys != NULL)
	{
		//comm->command.cmd = SHMMEMINFOSYS;
		infoSys->MemPoolSizeBytes = 10485760;	// available XCOM memory for creating tables.
		ret_code = MEM_SUCCESS;
	}
	else
	{
		ret_code = MEM_ER_BADBUFFER;
	}

    return ret_code;
}

void logger(int id, char *buffer)
{
	char buf[256] = {0};
	if( shm_address != NULL)
	{
		if(id == LOG_ERR && shm_address->isErrLogEnabled == 0)
		{
			return;
		}
		if(id == LOG_INFO && shm_address->isInfoLogEnabled == 0)
		{
			return;
		}
	}

	sprintf(buf, "XLIB: %s", buffer);
	#ifdef XCOMDEBUG
		if(id == LOG_ERR)
			printf(buffer);
		syslog ( id, buf);
	#else
		if(id == LOG_ERR)
			syslog ( id, buf);
	#endif
}

SECTION MemEol;
