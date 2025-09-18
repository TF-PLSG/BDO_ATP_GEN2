
/* 
 * File:   xcom.h
 * Author: epic
 *
 * Created on November 12, 2010, 11:59 AM
 */

#ifndef _XCOM_H
#define	_XCOM_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <synch.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <xipc.h>
#include <basictyp.h>
#include <mymalloc.h>
#ifdef	__cplusplus
extern "C" {
#endif

#if defined(SOLARIS_BUILD)
	#define SEMA_T		sema_t  
	#define SEMA_CLOSE	sema_close 
	#define SEMA_DESTROY	sema_destroy 
	#define SEMA_GETVALUE	sema_getvalue 
	#define SEMA_INIT	sema_init 
	#define SEMA_POST	sema_post 
	#define SEMA_TRYWAIT	sema_trywait 
	#define SEMA_UNLINK	sema_unlink 
	#define SEMA_WAIT	sema_wait 
	#define SEMA_TIMEDWAIT	sema_timedwait 

	#define SEMA_WAIT_L(x) {XINT _wait_ret; char _wait_msg[256];  while((_wait_ret = sema_wait((x))) != 0){err_code = ERR_peek_last_error();sprintf(_wait_msg,"XCOM-H: SEMA ERROR in sema_wait. %d - ERR %d\n",_wait_ret,err_code); syslog(LOG_ERR, _wait_msg); if(_wait_ret != EINTR) break;}}

	#define SEMA_POST_L(x) {XINT _wait_ret; char _wait_msg[256]; _wait_ret = sema_post((x)); if(_wait_ret != 0) {err_code = ERR_peek_last_error();sprintf(_wait_msg,"XCOM-H: SEMA ERROR in sema_post. %d - ERR %d\n",_wait_ret,err_code); syslog(LOG_ERR, _wait_msg);}}

	#define TIMESPEC_T timespec_t
#elif defined(AIX_BUILD)
	#define SEMA_T		sem_t  
	#define SEMA_CLOSE	sem_close 
	#define SEMA_DESTROY	sem_destroy 
	#define SEMA_GETVALUE	sem_getvalue 
	#define SEMA_INIT(A,B,C,D)	sem_init(A,1,B)
	#define SEMA_POST(x)	SEMA_POST_L(x) 
	#define SEMA_TRYWAIT	sem_trywait 
	#define SEMA_UNLINK	sem_unlink 
	#define SEMA_WAIT(x)	SEMA_WAIT_L(x)
	#define SEMA_TIMEDWAIT	sem_timedwait 


	#define SEMA_WAIT_L(x) {XINT _wait_ret; char _wait_msg[256];  while((_wait_ret = sem_wait((x))) != 0){ret_code = _wait_ret; _wait_ret=errno;sprintf(_wait_msg,"XCOM-H: SEMA ERROR in sem_wait. %d ErrNo - %d File: %s Line: %d\n",_wait_ret, errno, __FILE__, __LINE__); syslog(LOG_ERR, _wait_msg); if(_wait_ret != EINTR) break;}}

	#define SEMA_POST_L(x) {XINT _wait_ret; char _wait_msg[256]; _wait_ret = sem_post((x)); if(_wait_ret != 0) {ret_code = _wait_ret; sprintf(_wait_msg,"XCOM-H: SEMA ERROR in sem_post. %d ErrNo - %d  File: %s Line: %d\n",_wait_ret,errno, __FILE__, __LINE__); syslog(LOG_ERR, _wait_msg);}}
	#define TIMESPEC_T struct timespec
#else
	// NOT IMPLEMENTED
	#error implement this platform specific code first
#endif

#define SHMKEYPATH	"abc"  /* Path used on ftok for shmget key  */
#define SHMKEYID	200              /* Id used on ftok for shmget key    */


#define SIZEOFSHMSEG 1073741824		/* Size of the shared mem segment    */

#define SHAREDMEMENDADDR (((char*)shm_address) + SIZEOFSHMSEG)

#define INIT_LOCK 0
#define INIT_UNLOCK 1

#define LOCKCODE 2905

// XCOM configuration parameters
#define MAX_USERS		500
#define MAX_SHM_TABLES	100
#define MAX_USER_NAME	25
#define MAX_QID_LIST	5
#define MAX_NAME_LENGTH 25
#define TABLE_SIZE		31457280
#define MAX_TRYWAIT		5
#define TRYWAIT_SLEEP	5 /*in milliseconds*/
#define MAX_QUEUES			(MAX_USERS*3)
#define MAX_WAIT_SEMAPHORES	(MAX_USERS*2)
#define TOTAL_CMD_SLOTS_AVAILABLE MAX_USERS

#define QUEUEACCESS			100
#define QUEUESPOOL			101
#define QUEUEDESTROY		102
#define QUEUEPURGE			103
#define QUEUEREAD			104
#define QUEUELISTBUILD		105
#define QUEUESEND			106
#define QUEUEFREEZE			107
#define QUEUECREATE			108
#define QUEUEDELETE			109
#define QUEUEINFOQUEUE		110
#define QUEUEINFOUSER		111
#define QUEUEINFOSYS		112
#define QUEUEGET			113
#define QUEUEWRITE			114

#define USERLOGIN			115
#define USERLOGOUT			116

#define SHMMEMACCESS		117
#define SHMMEMSECDEF		118
#define SHMMEMSECTION		119
#define SHMMEMCREATE		120
#define SHMMEMINFOSYS		121
#define SHMMEMDESTROY		122
#define SHMMEMREAD			123
#define SHMMEMWRITE			124
#define SHMMEMLOCK			125
#define SHMMEMLIST			126
#define SHMMEMSECTIONBUILD	127
#define SHMMEMUNLOCK		128
#define SHMMEMINFOMEM		129

#define XCOMABORT			130
#define XCOMINFOUSER		131



struct sema_pool_t {
	SEMA_T tsmphore;
	XINT isBeingUsed;
	XINT stop; //0 - thread will be on wait. 1 - thread will exit
};

union CMDSTRUCT
{
		struct 
		{
			MSGHDR	MsgHdr; 	/* The resultant MsgHdr 		*/
			void	*data;
			XINT	RetCode;	/* ... of completed async operation */
		}
		QueWrite;

		struct
		{
			XINT	Qid; 	/* The resultant MsgHdr 		*/
			XINT	RetCode;	/* ... of completed async operation */
		}
		QueDelete;

		struct
		{
			MSGHDR	MsgHdr; 		/* The resultant MsgHdr */
            char	Name[MAX_QUE_NAME_SIZE];
			XINT	NameLen;
			XINT	RetQid;
			XINT	RetCode;
		}
		QueAccess;

		struct
		{
			QIDLIST	QueList;
			void	*Data;
			XINT	DataLen;
			XINT	Priority;
			XINT	WaitOption;
			XINT	RetQid;
			XINT	RetCode;
		}
		QueSend;

		struct
		{
			QIDLIST	QueList;
			XINT	Priority;
			XINT	WaitOption;
			XINT	Timeout;
			MSGHDR	MsgHdr;
			XINT	RetQid;
			XINT	RetCode;
		}
		QueGet;

		struct
		{
			void	*data;
			MSGHDR  MsgHdr;
			XINT	MsgLen;
			XINT	RetCode;
		}
		QueRead;

		struct
		{
			char	Name[MAX_QUE_NAME_SIZE];
			XINT	NameLen;
			XINT	Pid;
			XINT	RetQid;
			XINT	RetCode;
		}
		QueCreate;
		struct
		{
			char	arg2[MAX_QUE_NAME_SIZE];
			XINT	arg1;
			XINT	RetQid;
			XINT	RetCode;
		}
		QueSpool;
		struct
		{
			QIDLIST	QueList;
			XINT	RetQid;
			XINT	RetCode;
		}
		QueListBuild;

		struct
		{
			char Username[MAX_USER_NAME];
			char Instance[MAX_INSTANCE_NAME_SIZE];
			XINT Pid;
			XINT Tid;
			XINT RetUid;
			XINT RetCode;
		}
		UserLogin;

		struct
		{
			XINT RetCode;
		}
		UserLogout;

		struct
		{
			char TableName[MAX_NAME_LENGTH];
			XINT RetCode;
			XINT RetMid;
		}
		MemAccess;
				
		struct
		{
			char TableName[MAX_NAME_LENGTH];
			XINT TableSize;
			XINT RetCode;
			XINT RetMid;
		}
		MemCreate;

		struct
		{
			XINT Mid;
			XINT RetCode;
		}
		MemDestroy;
		
					struct
					{
						XINT Mid;
						MEMINFOMEM InfoMem;
						XINT RetCode;
					}
					MemInfoMem;
					struct
					{
						SECTION Section;
						MEMINFOSEC InfoSec;
						XINT RetCode;
					}
					MemInfoSec;
					struct
					{
						MEMINFOSYS InfoSys;
						XINT RetCode;
					}
					MemInfoSys;
					struct
					{
						XINT Uid;
						MEMINFOUSER InfoUser;
						XINT RetCode;
					}
					MemInfoUser;
		struct
		{
			XINT LockType;
			SECTION MidList;
			XINT Option;
			SECTION RetSection;
			XINT RetCode;
		}
		MemLock;
		struct
		{
			SECTION MidList;
			SECTION RetSection;
			XINT RetCode;
		}
		MemUnLock;
		
		struct
		{
			SECTION Section;
			XINT RetCode;
		}
		MemSecDef;
		struct
		{
			XINT Mid;
			XINT Offset;
			XINT Size;
			SECTION RetSection;
			XINT RetCode;
		}
		MemSection;
		struct
		{
			XINT Mid;
			XINT Offset;
			XINT Size;
			SECTION RetSection;
			XINT RetCode;
		}
		MemSectionBuild;
		
		struct
		{
			XINT Mid;
			XINT Offset;
			XINT Length;
			void *Buffer;
			XINT Option;
			XINT RetCode;
		}
		MemReadWrite;

	struct
	{
		XINT Qid;
		QUEINFOQUE InfoQue;
		XINT RetCode;
	}
	QueInfoQue_S;
	
	struct
	{
		XINT Uid;				/* User Id */
		XINT Pid;				/* Process ID of user */
		XTID Tid;				/* Thread ID of user */
		XINT LoginTime; 		/* Time of login to Instance */
		CHAR Name[MAX_USER_NAME];	/* User login name	*/
		XINT RetCode;
	}
	XcomInfoUser;

};

struct NODE
{  
	MSGHDR MH;
	struct NODE *next;
    struct NODE *prev;
	void *data;
};

typedef struct _QUEUE {

// FIFO queue
	struct NODE *head;
	struct NODE *tail;
	
	struct sema_pool_t *poolPtr;
	SEMA_T wsmphore;
	XINT isBeingUsed;
    XINT queueId;
    XINT createdBy;	// User Id that created this queue - is not used/required or important as Id's are re-used.
	XINT pid;
    XINT totalMessages;
    XINT totalDataBytes;
    XINT totalAllowedMsgs;	// 0 means unlimited
    XINT totalAllowedData;	// 0 means unlimited
    char queueName[MAX_NAME_LENGTH];
	char isItFrozen;
    
}QUEUE;

typedef struct _QUEUES
{
   QUEUE userQueues[MAX_QUEUES];
   SEMA_T qsmphore;
   XINT totalQueues;
   XINT maxQueueId;
   XINT messageCount;
}QUEUES;

struct XCOMUSER
{
    QUEUE    removedMsgQueue;
	SEMA_T usmphore;
	long  startReadTime;
	XINT  userid;
	XINT  isWaitingForMsg;	// Set to 1 when user has issued a read. Polled by read scheduler.
    XINT  timeout;
	XINT isBeingUsed;
    char username[MAX_NAME_LENGTH];	// user name
	XINT Pid;
	XINT Tid;

	// queues related information
    //struct LIST *myQueues;	// list of pointers to global list of queues for those created by this user.
};

struct COMMAND_MSG {
	union CMDSTRUCT cmdStruct;
    SEMA_T c_lock;
	void *cmdstring;
	XINT cmd;
};

struct RESPONSE_MSG {
	union CMDSTRUCT respStruct;
    SEMA_T r_lock;
    void *respstring;
    XINT resp;
};

struct COMM_LAYOUT {
	struct XCOMUSER user;
    struct COMMAND_MSG command;
    struct RESPONSE_MSG response;
    SEMA_T w_lock;
    XINT place_holder_id;
};

struct USERS
{
    XINT total_logged_in_users;
    //struct LIST *users;		// linklist of users, data is pointer of type struct XCOMUSER.
    struct XCOMUSER allUsers[MAX_USERS];
};

/****************************************************/
// QUEUE Implementation

struct SHM_TABLE
{
	SEMA_T shsmphore;
	SEMA_T wsmphore;
    long maxTableSize;	// 32768 may mean "32 rows of 1024 bytes each" or "16 rows of 2048 bytes".
    					// Actual interpretation of data is upto the process/user who created the table.
    void *tablePtr;	// Pointer to the Table data segment (will point into the shared memory).
    XINT createdBy;	// User Id that created this table - is not used/required or important as Id's are re-used.
    XINT sizeofDataCurrentlyWritten;
    XINT tableId;
	char destroyed;
    XINT isBeingUsed;
    char tableName[MAX_NAME_LENGTH];
};

struct SH_MEM_TABLES
{
	struct SHM_TABLE userTables[MAX_SHM_TABLES];
	XINT totalTables;
	XINT maxTableId;
};

typedef struct TABLE_AREA_t
{
	XINT size;
	XINT usedSize;
	char memory[TABLE_SIZE];
}TABLE_AREA;

struct SHR_MEM_LAYOUT {
    struct COMM_LAYOUT cmdSlot[MAX_USERS];
    QUEUES	xcomQueues;	//NOTE: Currently not being used
    struct SH_MEM_TABLES xcomTables;
	SEMA_T psmphore;
	struct sema_pool_t sema_pool[MAX_WAIT_SEMAPHORES];
    //struct USERS	xcomUsers;
    QUEUE    removedMsgQueue;	// Realpay/Atp first removes the message headers from the queues to
								// get the message data length (Queget), allocates required memory and
								// then reads the messages data later (QueRead). For the period the
								// messages are removed from their queue, but not read by
								// ATP/Realpay yet, they will be kept in removedMsgQueue.
	SEMA_T smphore;		// Acts as a semaphore to synchronize shared memory between all the processes.
						// Use this semaphore for tasks that affect the entire shared memory - like
						// login, logout or instance-freezing etc., where all logged in users must wait.
						// For functions that only acts on a queue or memory segment, each queue/segment
						// is provided with it's own semaphore to lock only that component, instead of
						// the whole shared memory block.
	XINT total_logged_in_users;
	TABLE_AREA tableArea;

	XINT isErrLogEnabled;
	XINT isInfoLogEnabled;
//-----------------------------------------------------------
	// WARNING: DO NOT ADD ANY MEMBERS BELOW THIS.
	// Safeguarding the dynamic allocation area from an overflow.
#if PADDING_TEST
	char allocPadding[ALLOC_AREA_PADDING_LEN];
#endif
};

#ifdef	__cplusplus
}
#endif

#endif	/* _XCOM_H */

