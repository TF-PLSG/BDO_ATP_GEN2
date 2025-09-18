/*
 * File:   xcom.c
 * Author: epic
 *
 * Created on October 14, 2010, 11:39 AM
 */

#include <signal.h>
#include <syslog.h>
#include "queue.h"
#include "shm_mem.h"
#include "xcom.h"
#include "hash.h"
// To suppress all kind of logs
//#define printf(a,b,c,d,e,f)
//#define printf(a,b,c,d,e)
//#define printf(a,b,c,d)
//#define printf(a,b,c)
//#define printf(a,b)
//#define printf(x)
//#define printf()
//#define printf
//
//#define perror(x)
//#define perror()
//#define perror

struct SHR_MEM_LAYOUT *shm_address;
QUEUES *queueList;
struct SH_MEM_TABLES *tableList;
int err_code;

sem_t debug_sem;
pthread_t tid[MAX_USERS];
int myUserID = -1;
int debug = 0;
// External variables required by MyMalloc library
extern void *sharedMemAllocStartAddr;
extern void *sharedMemBaseAddr;

extern int EndProcessSignalled;
char       ExeName[256];    // required by init.c

#if PADDING_TEST
extern char allocPaddingSignature[ALLOC_AREA_PADDING_LEN];
#endif

CHAR   Version[] = "ATP_11.1.0";

void * thread_handler(void *param);
int processcommand( void *param);
void logger(int id, char *buffer);

// Nirbhay:
// main() function is in init.c that will call MainProcessor() after
// initializing this process as a daemon (detaching it from the session) and
// setting up signal handlers.
//
// SIGTERM ( kill [PID] ) will gracefully shutdown XCOM daemon by detaching
// and deleting the shared memory. This is done after the infinite while loop
// at the end of MainProcessor below. Please add any other shutdown code
// (destroying semaphores etc.), as required, after the while loop.
//

int XcomLogInit();
#define XCOM_ERROR_LOG "XCOM_ERROR_LOG"
#define XCOM_INFO_LOG "XCOM_INFO_LOG"

static XINT __isErrLogEnabled = 1;
static XINT __isInfoLogEnabled = 1;

static CHAR   global_err_str[100];
static INT    global_err_code;
#define FILE_OPEN_ERROR          1
#define FILE_READ_ERROR          2
#define SECTION_NOT_FOUND        3
#define FILE_FORMAT_ERROR        4
#define BUFFER_SIZE_INSUFFICIENT 5
#define KEY_NOT_FOUND            6

INT GetPrivateProfileString( pCHAR section_name,
                             pCHAR key_name,
                             pCHAR default_str,
                             pCHAR dest_buffer,
                             UINT  buffer_size,
                             pCHAR file_name );

INT GetPrivateProfileString( pCHAR section_name,
                             pCHAR key_name,
                             pCHAR default_str,
                             pCHAR dest_buffer,
                             UINT  buffer_size,
                             pCHAR file_name )
{
    FILE *src_file;
    CHAR buffer[256], full_section_name[256];
    INT  found_sect, found_eq;
    UINT temp;

    /* copy the default string to the destination buffer */
    strcpy( dest_buffer, default_str );
    sprintf( full_section_name, "[%s]", section_name);

    /* try to open the source file */
    src_file = fopen( file_name, "r+" );
    if( src_file == NULL )
    {
        sprintf( global_err_str, "Unable to open file - %s", file_name  );
        global_err_code = FILE_OPEN_ERROR;
        return 0;
    }

    /* file is open, go through each line of the file..... */
    found_sect = 0;
    while( !feof( src_file ) )
    {
        fgets( buffer, sizeof( buffer ), src_file );

        /* get rid of line-feed character */
        if( buffer[ strlen( buffer) - 1 ] == 0xA )
            buffer[ strlen( buffer) - 1 ] = 0;
        if( ferror( src_file ) )
        {
            sprintf( global_err_str, "File read error - %s", file_name  );
            global_err_code = FILE_READ_ERROR;
            fclose( src_file );
            return 0;
        }

        /* ....and look for the specified section */
        if( strstr( buffer, full_section_name ) != NULL )
        {
            /* found the section, break out */
            found_sect = 1;
            break;
        }
    }
    
    /* if end of file was reached and section was not found, return */
    if( !found_sect )
    {
        sprintf( global_err_str, "section not found" );
        global_err_code = SECTION_NOT_FOUND;
        fclose( src_file );
        return 0;
    }

    /* section has been located, now look for requested key_name */
    while( !feof( src_file ) )
    {
        /* get each line in the section.... */
        fgets( buffer, sizeof( buffer ), src_file );

        /* get rid of line-feed character */
        if( buffer[ strlen( buffer) - 1 ] == 0xA )
            buffer[ strlen( buffer) - 1 ] = 0;
        if( ferror( src_file ) )
        {
            sprintf( global_err_str, "File read error - %s", file_name  );
            global_err_code = FILE_READ_ERROR;
            fclose( src_file );
            return 0;
        }

        /* ....and check if it contains the required key_name */
        if( strncmp( key_name, buffer, strlen( key_name ) ) == 0 )
        {
            /* found the key_name, retrieve the value */
            found_eq = 0;
            for( temp = strlen( key_name ); temp < strlen( buffer ); temp++ )
            {
                if( buffer[temp] == '=' )
                {
                    found_eq = 1;
                    break;
                }
            }

            if( !found_eq )
            {
                sprintf( global_err_str, "File format error - %s", file_name  );
                global_err_code = FILE_FORMAT_ERROR;
                fclose( src_file );
                return 0;
            }

            while( ++temp < strlen( buffer ) && buffer[temp] == ' ' );
            if( strlen( &buffer[temp] ) > buffer_size )
            {
                sprintf( global_err_str, "buffer size insufficient - %s", file_name  );
                global_err_code = BUFFER_SIZE_INSUFFICIENT;
                fclose( src_file );
                return 0;
            }

            strcpy( dest_buffer, &buffer[temp] );
            fclose( src_file );
            return 1;
        }
        else if( buffer[0] == '[' )
        {
            /* reached end of section, key_name was not found */
            sprintf( global_err_str, "key not found" );
            global_err_code = KEY_NOT_FOUND;
            fclose( src_file );
            return 0;
        }
    }

    /* reached end of file, key_name was not found */
    sprintf( global_err_str, "key not found" );
    global_err_code = KEY_NOT_FOUND;
    fclose( src_file );
    return 0;
}




int XcomLogInit()
{
	int status = 0;
	FILE *fptr = NULL;
	char value[4] = {0};
	char  AscendentRoot[256];
	
	if(getRoot(AscendentRoot) == 1)
	{
		strcat(AscendentRoot, "config/tf.ini");
		memset(value, 0, 4);
		status = GetPrivateProfileString("XCOM", XCOM_ERROR_LOG, "1", value, 3, AscendentRoot);
		printf("%d, %s\n", status, value);
		if(strlen(value) != 0 && strcmp(value, "0") == 0)
		{
			__isErrLogEnabled = 0;
		}
		else
		{
			__isErrLogEnabled = 1;
		}
		memset(value, 0, 4);
		status = GetPrivateProfileString("XCOM", XCOM_INFO_LOG, "1", value, 3, AscendentRoot);
		printf("%d, %s\n", status, value);
		if(strlen(value) != 0 && strcmp(value, "0") == 0)
		{
			__isInfoLogEnabled = 0;
		}
		else
		{
			__isInfoLogEnabled = 1;
		}
	}
	printf("%s = %d\n", XCOM_ERROR_LOG, __isErrLogEnabled);
	printf("%s = %d\n", XCOM_INFO_LOG, __isInfoLogEnabled);
	return status;
}

void MainProcessor()
{
	int semid, shmid = 0, i;
	int ret_code = 0;
	key_t semkey, shmkey = 0;
	int ret1 = 0;
	int ret = 0;
	char  AscendentRoot[256];

	char msg[256];
#if PADDING_TEST
	int paddingCtr;
#endif

	/* Check log enable status. Only if log enabled, logger will log into the syslog.*/
	XcomLogInit();

	//sleep(30);
	/* Generate an IPC key for the semaphore set and the shared      */
	/* memory segment.  Typically, an application specific path and  */
	/* id would be used to generate the IPC key.                     */
	if(getRoot(AscendentRoot) == 1)
	{
		strcat(AscendentRoot,SHMKEYPATH);
	}
	else
	{
		logger(LOG_ERR, "xcomdaemon ERROR: ASCENDENTROOT not set.\n");
		exit(-11);
	}
	shmkey = ftok(AscendentRoot, SHMKEYID);
	if (shmkey == -1)
	{
		logger(LOG_ERR, "xcomdaemon ERROR: ftok() failed\n");
		printf("%s\n",AscendentRoot);
		perror("ERR:\n");
		exit(-1);
	}

	/* Create Shared Memory segment using the shmkey
	 * SIZEOFSHMSEG = 50
	 */
	
	shmid = shmget(shmkey, SIZEOFSHMSEG, 0666 | IPC_CREAT | IPC_EXCL );

	if (shmid == -1)
	{
		logger(LOG_ERR, "xcomdaemon ERROR: shmget() failed\n");
		exit(-2);
	}

	/* Attach shared memory segment to the process
	 */

	shm_address = (struct SHR_MEM_LAYOUT *) shmat(shmid, NULL, 0);
	if (shm_address == NULL)
	{
		logger(LOG_ERR, "xcomdaemon ERROR: shmat() failed\n");
		exit(-3);
	}

	memset(shm_address, 0x00, SIZEOFSHMSEG);
	if(SEMA_INIT(&(shm_address->smphore),1,USYNC_PROCESS,NULL) != 0)
	{
		
		sprintf(msg,"xcomdaemon ERROR: semainit() failed - \terror code = %d\n",errno);
		logger(LOG_ERR, msg);
		exit(-4);
	}
	if(SEMA_INIT(&(shm_address->xcomQueues.qsmphore),1,USYNC_PROCESS,NULL) != 0)
	{
		sprintf(msg,"xcomdaemon ERROR: semainit() failed - \terror code = %d\n",errno);
		logger(LOG_ERR, msg);
		exit(-4);
	}

	if(SEMA_INIT(&(debug_sem),0,USYNC_PROCESS,NULL) != 0)
	{
		logger(LOG_ERR, "xcomdaemon ERROR: semainit() failed\n");
		exit(-4);
	}

	memset(shm_address->sema_pool,0,(sizeof(struct sema_pool_t)) * MAX_WAIT_SEMAPHORES);

//Initialization for MYMALLOC

    sharedMemAllocStartAddr = ((char*)shm_address) + sizeof(struct SHR_MEM_LAYOUT);
    if ((void *) sharedMemAllocStartAddr > (void *) (SHAREDMEMENDADDR) )
    {
        // Shared memory too small
        logger(LOG_ERR, "xcomdaemon ERROR: Shared Memory too small.\n");
        exit(-5);
    }
    else
    {
	
	sprintf (msg, "XCOM: Allocated = %ld, required = %ld.\n", SIZEOFSHMSEG,
	                        sizeof(struct SHR_MEM_LAYOUT));
	logger(LOG_INFO, msg);
    }

	sharedMemBaseAddr = sharedMemAllocStartAddr;    // Addresses will be stored relative to this base address.

#if PADDING_TEST
	// Initialize the Padding area, before the start of allocation area, with a signature to
	// guard against any overflows. Both the signature array and padding area are
	// initialized here. The signature array can be used for comparison later to detect an overflow.
	for ( paddingCtr = 1 ; paddingCtr <= ALLOC_AREA_PADDING_LEN ; paddingCtr++ )
	{
		shm_address->allocPadding[paddingCtr-1] = allocPaddingSignature[paddingCtr-1] = (char)paddingCtr;
	}
#endif

	if(myinit(SIZEOFSHMSEG - sizeof(struct SHR_MEM_LAYOUT)) != 0)
	{
	    logger (LOG_ERR, "xcomdaemon ERROR: MyMalloc init failed.\n");
	    exit(-6);
	}
	shm_address->tableArea.size = TABLE_SIZE;
	shm_address->tableArea.usedSize = 0;
	queueList = &(shm_address->xcomQueues);
	tableList = &(shm_address->xcomTables);
	for(i=0; i<MAX_SHM_TABLES; i++)
	{
		shm_address->xcomTables.userTables[i].tableId = -1;
	}
	for(i=0; i<MAX_QUEUES; i++)
	{
		shm_address->xcomQueues.userQueues[i].queueId = -1;
		shm_address->xcomQueues.userQueues[i].createdBy = -1;
	}
	hash_init();
	//Initialize the wait Semaphores
	if (SEMA_INIT(&(shm_address->psmphore), 1, USYNC_PROCESS, NULL) != 0)
	{
		sprintf(msg,"xcomdaemon ERROR: semainit() failed for tsmphore of wait thread  %d\n", i);
		logger(LOG_ERR, msg);
		exit(-9);
	}
	SEMA_WAIT(&(shm_address->psmphore));
	for (i = 0; i < MAX_WAIT_SEMAPHORES; i++)
	{
		if (SEMA_INIT(&(shm_address->sema_pool[i].tsmphore), 0, USYNC_PROCESS, NULL) != 0)
		{
			sprintf(msg,"xcomdaemon ERROR: semainit() failed for tsmphore of wait thread  %d\n", i);
			logger(LOG_ERR, msg);
			exit(-9);
		}
		
		shm_address->sema_pool[i].isBeingUsed = 0;
		shm_address->sema_pool[i].stop = 0;
	}
	SEMA_POST(&(shm_address->psmphore));


    for (i = 0; i < MAX_USERS; i++)
    {
        if (SEMA_INIT(&(shm_address->cmdSlot[i].command.c_lock), 0, USYNC_PROCESS, NULL) != 0)
        {
            sprintf(msg, "xcomdaemon ERROR: semainit() failed for C lock %d\n", i);
			logger(LOG_ERR, msg);
            exit(-7);
        }
        if (SEMA_INIT(&(shm_address->cmdSlot[i].response.r_lock), 0, USYNC_PROCESS, NULL) != 0)
        {
            sprintf(msg,"xcomdaemon ERROR: semainit() failed for R lock %d\n", i);
			logger(LOG_ERR, msg);
            exit(-8);
        }
        if (SEMA_INIT(&(shm_address->cmdSlot[i].w_lock), 1, USYNC_PROCESS, NULL) != 0)
        {
            sprintf(msg,"xcomdaemon ERROR: semainit() failed for R lock %d\n", i);
			logger(LOG_ERR, msg);
            exit(-9);
        }


	    ret1 = SEMA_TRYWAIT(&(shm_address->cmdSlot[i].w_lock));

	    if (ret1 == -1)
	    {
	        logger(LOG_ERR, "xcomdaemon ERROR: w_lock was busy\n");
	    }

	    SEMA_POST(&(shm_address->cmdSlot[i].w_lock));
	    shm_address->cmdSlot[i].place_holder_id = i;
	    ret = pthread_create(&tid[i], NULL, thread_handler, &(shm_address->cmdSlot[i]));

		// TODO: Error checking missing
	}

	shm_address->isErrLogEnabled = __isErrLogEnabled;
	shm_address->isInfoLogEnabled = __isInfoLogEnabled;

	sprintf(msg,"XcomDaemon started... Version %s\n", Version);
	logger(LOG_INFO, msg);

	printf("All Threads created - Thread count = %d\n",i);

	while ( !EndProcessSignalled )
	{
	    sleep(10);  // When End-Process is signalled, should exit within 10 sec.
	}

	// Prepare to exit.
	//
	// Detach shared memory from process memory and delete
	// it parmanently. Please note that system marks this shared memory as
	// "to be deleted". It will actually get deleted only after the last
	// process attached to it, is detached and no other process is using it.
	shmdt( (char*) shm_address);
	shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL);
}

// Nirbhay:
// Added these functions as required by init.c
void EndProcess()
{
    logger( LOG_INFO, "Shutting down the XCOM daemon" );
}


/* ****************************************************************************** */
/* ****************************************************************************** */
void InitEventLogging()
{
#ifdef WIN32

   LONG Ret;
   HKEY hRegKey;
   DWORD Result;
   DWORD Data;
   BYTE   bData [100];

   DWORD  Type, Size;
   char   ValueName[20];

   char Buffer[256];
   char tempsrc[ 256 ];
   pCHAR  src;
   char MsgFile[256];


   #ifndef _WINDOWS_
      _tzset();
   #endif

   /*  Get the module name */
   src = tempsrc;

   for( Data = GetModuleFileName( NULL, src, 256 );
       Data > 0; Data-- )
   {
      if( tempsrc[ Data-1 ] == '\\' )
      {
         src = &tempsrc[ Data ];
         break;
      }
   }

   /*  strip the '.exe' at the end */
   for( Data = strlen( src );
       Data > 0; Data-- )
   {
      if( src[ Data-1 ] == '.' )
      {
         src[ Data-1 ] = 0;
         break;
      }
   }

   /* get pinnacle home directory and path to message file */
   wsprintf( Buffer, key_ascendent);
   Ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                  (LPCTSTR)Buffer,
                  0,
                  KEY_ALL_ACCESS,
                  &hRegKey );

   wsprintf( ValueName, "AscendentHome" );
   Size = sizeof( bData );
   Ret = RegQueryValueEx( hRegKey, ValueName, NULL, &Type, bData, &Size );
   lstrcpy( MsgFile, (pCHAR)bData );

   lstrcat( MsgFile, MSG_TEMPLATE_FILE );
   RegCloseKey( hRegKey );


   wsprintf( Buffer, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s", src );

   Ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                  (LPCTSTR)Buffer,
                  0,
                  KEY_ALL_ACCESS,
                  &hRegKey );

   if( Ret != ERROR_SUCCESS )
   {

      RegCreateKeyEx( HKEY_LOCAL_MACHINE,
                  (LPTSTR)Buffer,
                  0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
                  NULL, &hRegKey, &Result );

      wsprintf( Buffer, MsgFile );
      RegSetValueEx( hRegKey,
                  (LPCTSTR)"EventMessageFile",
                  0, REG_EXPAND_SZ,
                  (CONST BYTE *)Buffer,
                  (DWORD)lstrlen( Buffer ) + 1 );

      Data = 7;
      RegSetValueEx( hRegKey,
                  (LPCTSTR)"TypesSupported",
                  0, REG_DWORD,
                  (CONST BYTE * )&Data,
                  sizeof( int ) );
   }

   RegCloseKey( hRegKey );
   hEventSource = RegisterEventSource( NULL, src );

#else

   char  *logIda = NULL;
   int   logId;
   int   logFlag = LOG_LOCAL1;
   int   badVarValue = 0;
   char  logMsg[256];

   logIda = getenv("ASCENDENT_LOG_ID");
   if(logIda != NULL)
   {
      logId = atoi(logIda);
      switch(logId)
      {
         case 0: logFlag = LOG_LOCAL0;  break;
         case 1: logFlag = LOG_LOCAL1;  break;
         case 2: logFlag = LOG_LOCAL2;  break;
         case 3: logFlag = LOG_LOCAL3;  break;
         case 4: logFlag = LOG_LOCAL4;  break;
         case 5: logFlag = LOG_LOCAL5;  break;
         case 6: logFlag = LOG_LOCAL6;  break;
         case 7: logFlag = LOG_LOCAL7;  break;
         default:
         {
            badVarValue = 1;
         }
      }
   }
   openlog("ASCENDENT", LOG_CONS|LOG_NDELAY|LOG_PID, logFlag);

   if(badVarValue)
   {
      sprintf(logMsg,
        "Invalid value (%d) of ASCENDENT_LOG_ID; defaulting to LOG_LOCAL1",
        logId);
      logger( LOG_WARNING, logMsg);
   }

#endif
}

void ShutDownEventLogging()
{
#ifdef WIN32
   (VOID)DeregisterEventSource( hEventSource );
#else
   closelog();
#endif
}

void * thread_handler(void *param) {
	int ret_code = 0;
    struct COMM_LAYOUT * comm = (struct COMM_LAYOUT *) param;
	char buf[256];
	char buf1[512];
	if(param != NULL)
	{
		while (1) 
		{
			SEMA_WAIT(&(comm->command.c_lock));
			
			comm->response.resp = processcommand( param);
			SEMA_POST_L(&(comm->response.r_lock));
		}
	}
}

int processcommand( void *param)
{
	struct COMM_LAYOUT * comm = (struct COMM_LAYOUT *) param;
	int cmdID = comm->command.cmd;
	int ret_code = 0;
	void *read_data = NULL;
	struct NODE *node = NULL;
	int i = 0;
	int qid = -1;
	int mid = -1;
	char msg[512] = "";
	int myUserID = -1;
	TIMESPEC_T timeout;

	int id;

	QUEUE *currentQue = NULL, *additionalQue = NULL;
	struct SHM_TABLE *currentTable = NULL;
	switch(cmdID)
	{
		case USERLOGOUT:
			sprintf(msg,"\n USER Logout... user id = %d \t user name = %s \t tid = %d pid = %d\n", comm->user.userid, comm->user.username, comm->place_holder_id,comm->user.Pid);
			logger(LOG_INFO,msg);	
		    
			myUserID = comm->user.userid;
			//###################################################################
				// C R I T I C A L    S E C T I O N    S T A R T S

			// TODO: This second lock may not be required, but first analyze the above pending question !!.
				// While critical section is locked.
			//###################################################################
				// C R I T I C A L    S E C T I O N    S T A R T S

				// Check if this user is actually logged in.... How?
				// Temporary logic, until we are statically linked to all the processes - Global variable
				// myUserID stores the XCOM User Id assigned to this process in XipcLoginEx(). This
				// ID can be searched and if present, logout will be allowed. This will also take care when
				// logout is called twice without calling Login.

				if ( myUserID >= 0 && comm->user.isBeingUsed == 1 )
				{
					// The user block is allocated, so deallocate it. All queues created by this user will not
					// be affected and continue to reside in XCOM memory.
					comm->user.isBeingUsed = 0;

					// Reset user structure
					comm->user.userid = -1;
					memset( comm->user.username, 0x00, sizeof(comm->user.username));
					comm->user.isWaitingForMsg = 0;
					comm->user.startReadTime = 0;
					comm->user.timeout = 0;
					comm->user.Pid = 0;
					comm->user.Tid = 0;
							
					// Queues will continue to be there even if the user,
					// who created the queue, logs out. This is a default
					// behavior copied from XIPC.

					myUserID = -1;	// reset global user id so that second call to logout doesn't do anything.

					// decrement number of logged-in users. It must be validated that this user is not
					// calling logout more than once, else the count of users will be corrupted.
					shm_address->total_logged_in_users -= 1; //Less by one user
					ret_code = 0;
					sprintf (msg,"XCOM: Process Logged out. ret_code = %d\n",ret_code);
					logger(LOG_INFO,msg);
					
				}
				else
				{
					// Error: Logout is called out of sync with login or it is called twice/more than once.
					sprintf (msg, "XCOM: ERROR - XipcLogout() called out of order or called without logging in first.\n");
					logger (LOG_ERR, msg);
					ret_code = -1;
				}
				comm->response.respStruct.UserLogout.RetCode = ret_code;

			//###################################################################
				// Unlock while exiting critical section
			
				

		break;
		case USERLOGIN:
			
			if(comm->command.cmdStruct.UserLogin.Username != NULL)
			{
				comm->command.cmdStruct.UserLogin.RetUid = -1;
			
					if ( ( shm_address->total_logged_in_users >= 0 ) &&
							( shm_address->total_logged_in_users < MAX_USERS ) )
					{
						i = comm->place_holder_id;

						if ( i >= MAX_USERS )
						{
							sprintf (msg, "XCOM: Login failed for %s with instance name %s. No space in user structure.\n", comm->command.cmdStruct.UserLogin.Username, comm->command.cmdStruct.UserLogin.Instance);
							logger(LOG_ERR, msg);
							ret_code = -5;
						}
						else
						{
		
							comm->user.isBeingUsed = 1;
							/*Clear the removedMsgQueue of this user*/
							currentQue = &(comm->user.removedMsgQueue);
							queuePurgeMessages(currentQue);
							comm->user.userid = myUserID = i;	// Index itself will be used as userid, it will ensure reusability of userid.
							strcpy ( comm->user.username, comm->command.cmdStruct.UserLogin.Username);
							comm->user.Pid = comm->command.cmdStruct.UserLogin.Pid;
							comm->user.Tid = comm->command.cmdStruct.UserLogin.Tid;
							comm->user.isWaitingForMsg = 0;	//
							comm->user.startReadTime = 0;	//
							comm->user.timeout = 0;	//
							shm_address->total_logged_in_users += 1;	// increment num of logged-in users.
							comm->response.respStruct.UserLogin.RetUid = myUserID;

							sprintf (msg, "XCOM: %s logged in with instance name %s. Total Users = %d.\n", comm->user.username, comm->command.cmdStruct.UserLogin.Instance, shm_address->total_logged_in_users);
							logger (LOG_INFO, msg);
						}
					}
					else if ( shm_address->total_logged_in_users >= MAX_USERS )
					{
						sprintf (msg, "XCOM: Too many users already logged in. %s logging in with instance name %s.\n", comm->command.cmdStruct.UserLogin.Username, comm->command.cmdStruct.UserLogin.Instance,
																			shm_address->total_logged_in_users);
						logger(LOG_ERR, msg);
						ret_code = -6;	// Too many logged in users
					}
					else
					{
						sprintf (msg, "XCOM: Illegal total users count %d for %s logging in with instance name %s. Count reset to 0.\n",
							shm_address->total_logged_in_users,comm->command.cmdStruct.UserLogin.Username, comm->command.cmdStruct.UserLogin.Instance, shm_address->total_logged_in_users);
						logger(LOG_ERR, msg);
						shm_address->total_logged_in_users = 0;
						ret_code = -7;
					}
					sprintf(msg," USER Login\tRet code = %d\tUser= %s\t%d\n\n",ret_code,comm->user.username, comm->user.userid );
					logger(LOG_INFO,msg);
			}
			else
				ret_code = QUE_ER_BADUSERNAME;    // Invalid user name
			break;

		case QUEUEACCESS:
			if(comm->command.cmdStruct.QueAccess.Name != NULL && comm->command.cmdStruct.QueAccess.NameLen > 0)
			{
				ret_code = getQueueIdByName(comm->command.cmdStruct.QueAccess.Name);
				if(ret_code < 0)
				{
					
					comm->response.respStruct.QueAccess.RetQid = -1;
					comm->response.respStruct.QueAccess.RetCode = QUE_ER_NOACCESS;
					ret_code = QUE_ER_NOACCESS;
				}
				else
				{
					comm->response.respStruct.QueAccess.RetQid = ret_code;
					comm->response.respStruct.QueAccess.RetCode = QUE_SUCCESS;
					ret_code = QUE_SUCCESS;

				}
			}
		break;
		case QUEUESPOOL:
			//To be
			logger(LOG_INFO,"QUEUESPOOL\n");
			ret_code = 0;
		break;
		
		case QUEUEDESTROY:

			ret_code = queueDestroy(comm->command.cmdStruct.QueDelete.Qid);
			comm->response.respStruct.QueDelete.RetCode = ret_code;
			if(ret_code < 0)
			{
				sprintf(msg,"ERROR in QUEUEDESTROY - User:\"%s\" - %d in tid = %d\tSlot = %d\n",comm->user.username,comm->command.cmdStruct.QueDelete.Qid,pthread_self(), comm->place_holder_id );
				logger(LOG_ERR,msg);
			}

		break;

		case QUEUEPURGE:
			ret_code = queuePurge(comm->command.cmdStruct.QueDelete.Qid);
			comm->response.respStruct.QueDelete.RetCode = ret_code;
			ret_code = 0;
		break;

		case QUEUEREAD:
			{
				currentQue = NULL;
				currentQue = &(comm->user.removedMsgQueue);
				if(currentQue != NULL)
				{
					ret_code = 0;
					read_data = getNodeData(currentQue, &ret_code);
					comm->response.respStruct.QueRead.MsgLen = ret_code;
					if(read_data != NULL)
					{
						comm->response.respStruct.QueRead.data = read_data;
						comm->response.respStruct.QueRead.RetCode = QUE_SUCCESS;
						comm->response.respStruct.QueRead.MsgLen = ret_code;
						ret_code = QUE_SUCCESS;
					}
					else
					{
						comm->response.respStruct.QueRead.data = NULL;
						comm->response.respStruct.QueRead.RetCode = QUE_ER_NOTFOUND;
						ret_code = QUE_ER_NOTFOUND;
						sprintf(msg,"ERROR in QUEREAD read_data is NULL - User:\"%s\" RetCode =%d, len = %d\n", comm->user.username,ret_code, comm->response.respStruct.QueRead.MsgLen);
					}
				}
				else
				{
					comm->response.respStruct.QueRead.data = NULL;
					comm->response.respStruct.QueRead.RetCode = QUE_ER_NOTFOUND;
					ret_code = QUE_ER_NOTFOUND;
					sprintf(msg,"ERROR in QUEREAD CurrentQue is NULL - User:\"%s\" RetCode =%d, len = %d\n", comm->user.username,ret_code, comm->response.respStruct.QueRead.MsgLen);
				}
				if(ret_code < 0)
				{
					logger(LOG_ERR,msg);
				}
			}
			
		break;

		case QUEUELISTBUILD:
			//QueListBuild is implemented in Library function itself
			logger(LOG_INFO,"QUEUELISTBUILD\n");
			ret_code = 0;
		break;
		case QUEUESEND:
#ifdef XCOMDEBUG
			if(shm_address->xcomQueues.messageCount < 0)
			{
				sprintf(msg,"ERROR in QueSend Message count less than zero\n");
				logger(LOG_ERR,msg);
			}
#endif
			//Write the data to the list of queues
			if(comm->command.cmdStruct.QueSend.QueList != NULL)
			{
				PQIDLIST quelist = (PQIDLIST)comm->command.cmdStruct.QueSend.QueList;
				
					qid = quelist[0];
					currentQue = NULL;
					currentQue = getQueueById(qid);
					
					if(currentQue != NULL)
					{
						if(comm->command.cmdStruct.QueSend.WaitOption != XIPC_BLOCKOPT_NOWAIT)
						{
							SEMA_WAIT(&(currentQue->wsmphore));
							ret_code = 0;
						}
						else
						{
							int loop = 0;
							ret_code = -1;
							while((loop < MAX_TRYWAIT) && (ret_code != 0))
							{
								ret_code = SEMA_TRYWAIT(&(currentQue->wsmphore));
								loop++;
							}
						}
						if(ret_code == 0)
						{
							ret_code = addNode(currentQue,comm->command.cmdStruct.QueSend.Data, 
														  comm->command.cmdStruct.QueSend.DataLen,
														  comm->command.cmdStruct.QueSend.Priority, 
														  comm->user.userid);
							SEMA_POST_L(&(currentQue->wsmphore));
						}
						else
						{
							ret_code = QUE_ER_TIMEOUT;
							sprintf(msg,"ERROR in QUEUESEND TIMEOUT - User:\"%s\" addNode to Q - (%s) ret_code = %d\n",comm->user.username,currentQue->queueName, comm->response.respStruct.QueSend.RetCode);
						}
					}
				
				comm->response.respStruct.QueSend.RetCode = ret_code;
				comm->response.respStruct.QueSend.DataLen = comm->command.cmdStruct.QueSend.DataLen;
			}
			else
			{
				comm->response.respStruct.QueSend.RetCode = QUE_ER_NOTFOUND;
				sprintf(msg,"ERROR in QUEUESEND - User:\"%s\" addNode to Q - (%s) ret_code = %d\n",comm->user.username,currentQue->queueName, comm->response.respStruct.QueSend.RetCode);
			}
			if(comm->response.respStruct.QueSend.RetCode < 0)
			{
				logger(LOG_ERR,msg);
			}

		break;

		case QUEUEFREEZE:
			//Not required by ATP
			ret_code = 0;
		break;

		case QUEUECREATE:
			//Create que with the given queue name
			if(comm->command.cmdStruct.QueCreate.Name != NULL && comm->command.cmdStruct.QueCreate.NameLen > 0)
			{
				currentQue = NULL;
				currentQue = createQue(comm->command.cmdStruct.QueCreate.Name, comm->user.userid);

				if(currentQue != NULL)
				{
					currentQue->pid = comm->command.cmdStruct.QueCreate.Pid;
					comm->response.respStruct.QueCreate.RetQid = currentQue->queueId;
					comm->response.respStruct.QueCreate.RetCode = QUE_SUCCESS;
					ret_code = QUE_SUCCESS;
				}
				else
				{
					comm->response.respStruct.QueCreate.RetCode = QUE_ER_BADQUENAME;
					ret_code = QUE_ER_BADQUENAME;
				}
			}
			if(ret_code < 0)
			{
				sprintf(msg,"ERROR in QUEUECREATE - User:\"%s\" - ret_code = %d - QueName = %d\t%s\n",comm->user.username,ret_code, comm->response.respStruct.QueCreate.RetQid,comm->command.cmdStruct.QueCreate.Name);
				logger(LOG_INFO,msg);
			}
		break;

		case QUEUEDELETE:
			//QueDelete not used in ATP
			ret_code = 0;
		break;
		case QUEUEINFOQUEUE:
			//Returns the uid
			ret_code = 0;
			if(comm->command.cmdStruct.QueInfoQue_S.Qid > -1)
			{
				currentQue = NULL;
				currentQue = getQueueById(comm->command.cmdStruct.QueInfoQue_S.Qid);
				if(currentQue != NULL)
				{

			strncpy(comm->response.respStruct.QueInfoQue_S.InfoQue.Name,currentQue->queueName,MAX_NAME_LENGTH);
					comm->response.respStruct.QueInfoQue_S.InfoQue.Qid			 = currentQue->queueId;
					comm->response.respStruct.QueInfoQue_S.InfoQue.CreateUid	 = currentQue->createdBy;
					comm->response.respStruct.QueInfoQue_S.InfoQue.CountMessages = currentQue->totalMessages;
					comm->response.respStruct.QueInfoQue_S.RetCode				 = QUE_SUCCESS;
				}
				else
				{
					comm->response.respStruct.QueInfoQue_S.RetCode = QUE_ER_NOTFOUND;
				}
			}
			else
			{
				comm->response.respStruct.QueInfoQue_S.RetCode = QUE_ER_BADQID;
			}
		break;
		case QUEUEINFOUSER:
		
			ret_code = 0;
		break;
		case QUEUEINFOSYS:
			//Not required
			ret_code = 0;
		break;

		case QUEUEGET:
                    // We can get one or two queue ID's
			if(comm->command.cmdStruct.QueGet.QueList != NULL && comm->command.cmdStruct.QueGet.QueList[0] > -1)
			{
				comm->user.isWaitingForMsg = 1;
				PQIDLIST quelist = (PQIDLIST)comm->command.cmdStruct.QueGet.QueList;
				qid = quelist[0];
				currentQue = &(shm_address->xcomQueues.userQueues[qid]);
				
				if(quelist[1] > -1)
				{
					additionalQue = &(shm_address->xcomQueues.userQueues[ quelist[1] ]);
				}
				else
				{
					additionalQue = NULL;
				}
				//Both The QUEUES
				if(currentQue != NULL && additionalQue != NULL)
				{
					SEMA_WAIT(&(additionalQue->wsmphore));
					SEMA_WAIT(&(currentQue->wsmphore));
					node = NULL;
					if(additionalQue->totalMessages > 0)
					{
						node = getNode(additionalQue);
					}
					else if(currentQue->totalMessages > 0)
					{
						node = getNode(currentQue);
					}
					if(node != NULL)
					{
						SEMA_POST(&(currentQue->wsmphore));
						SEMA_POST(&(additionalQue->wsmphore));
						memset(&(comm->response.respStruct.QueGet.MsgHdr), 0, sizeof(MSGHDR));
						memcpy(&(comm->response.respStruct.QueGet.MsgHdr), &(node->MH), sizeof(MSGHDR));
						putNode(&(comm->user.removedMsgQueue),node);
						comm->response.respStruct.QueGet.RetCode = QUE_SUCCESS;
					}
					else
					{
						int wait_ret_code = 0;
						
						if(comm->command.cmdStruct.QueGet.WaitOption != XIPC_BLOCKOPT_NOWAIT)
						{
							if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) {
								sprintf(msg,"Error in clock_gettime\n");
								logger(LOG_ERR,msg);
								return -1;
							}
							if(comm->command.cmdStruct.QueGet.Timeout > 0)
							{
								timeout.tv_sec += comm->command.cmdStruct.QueGet.Timeout;
							}
							else if(comm->command.cmdStruct.QueGet.WaitOption == XIPC_BLOCKOPT_WAIT)
							{
								timeout.tv_sec = 0;
							}
							SEMA_WAIT(&(shm_address->psmphore));
							for(i=0;i<MAX_WAIT_SEMAPHORES;i++)
							{
								if(shm_address->sema_pool[i].isBeingUsed == 0)
								{
									shm_address->sema_pool[i].isBeingUsed = 1;
									id = i;
									break;
								}
							}
							
							if(currentQue->poolPtr == NULL)
							{
								currentQue->poolPtr = &(shm_address->sema_pool[id]);
								currentQue->poolPtr->stop++;
								additionalQue->poolPtr = &(shm_address->sema_pool[id]);
								additionalQue->poolPtr->stop++;
							}
							else
							{
								shm_address->sema_pool[id].isBeingUsed = 0; //Releasing recently acquired sem.
								additionalQue->poolPtr = currentQue->poolPtr;
								additionalQue->poolPtr->stop++;
								currentQue->poolPtr->stop++;
							}
							SEMA_POST(&(shm_address->psmphore));
							
							SEMA_POST(&(currentQue->wsmphore));
							SEMA_POST(&(additionalQue->wsmphore));
							
							if(timeout.tv_sec >0)
							{
								wait_ret_code = SEMA_TIMEDWAIT(&(currentQue->poolPtr->tsmphore), &timeout);
							}
							else
							{
								SEMA_WAIT(&(currentQue->poolPtr->tsmphore));
								//ret_code = 0;
							}
							SEMA_WAIT(&(additionalQue->wsmphore));
							SEMA_WAIT(&(currentQue->wsmphore));
							if((additionalQue->poolPtr == NULL) || (currentQue->poolPtr == NULL))
							{
								sprintf(msg,"poolPtr is NULL. 1 AdditionalQue =%s currentQue =%s user=%s\n", additionalQue->queueName,currentQue->queueName,comm->user.username);
									logger(LOG_INFO,msg);
							}
							else
							{
								additionalQue->poolPtr->stop--;
								currentQue->poolPtr->stop--;
								
								SEMA_WAIT(&(shm_address->psmphore));
								if((additionalQue->poolPtr->stop == 0) || (currentQue->poolPtr->stop == 0))
								{
									currentQue->poolPtr->isBeingUsed = 0;
									additionalQue->poolPtr = NULL;
									currentQue->poolPtr = NULL;
								}
								SEMA_POST(&(shm_address->psmphore));
							}
							if(wait_ret_code == 0)
							{
								node = NULL;
								if(additionalQue->totalMessages > 0)
								{
									node = getNode(additionalQue);
								}
								else if(currentQue->totalMessages > 0)
								{
									node = getNode(currentQue);
								}
								else
								{
									comm->response.respStruct.QueGet.RetCode = QUE_ER_TIMEOUT;
								}
								
								if(node != NULL)
								{
									//To log message for diagonizing the applnk crash.
									#ifdef _DEBUG
									if(strcmp(currentQue->queueName,"applnkA") == 0)
									{
										sprintf(msg,"Message in applnkA size = %d",node->MH.Size);
										logger(LOG_INFO,msg);
									}
									#endif
									memset(&(comm->response.respStruct.QueGet.MsgHdr), 0, sizeof(MSGHDR));
									memcpy(&(comm->response.respStruct.QueGet.MsgHdr), &(node->MH), sizeof(MSGHDR));
									putNode(&(comm->user.removedMsgQueue),node);
									comm->response.respStruct.QueGet.RetCode = QUE_SUCCESS;
								}
							}
							else
							{
								comm->response.respStruct.QueGet.RetCode = QUE_ER_TIMEOUT;
							}
							SEMA_POST(&(currentQue->wsmphore));
							SEMA_POST(&(additionalQue->wsmphore));
						}
						else
						{
							SEMA_POST(&(currentQue->wsmphore));
							SEMA_POST(&(additionalQue->wsmphore));
							comm->response.respStruct.QueGet.RetCode = QUE_ER_NOWAIT;
						}
					}
				}
				else if(currentQue != NULL && additionalQue == NULL)
				{
					//############################################//
					//For only one queue
					SEMA_WAIT(&(currentQue->wsmphore));

					node = NULL;
					ret_code = 0;
					if(currentQue->totalMessages > 0)
						ret_code = 100;
					node = getNode(currentQue);
					if(node == NULL)
					{
						if(ret_code == 100)
						{
							sprintf(msg,"CRITICAL ERROR in reading Queues. 2\n");
							logger(LOG_ERR,msg);
							comm->response.respStruct.QueGet.RetCode = QUE_ER_SYSERR;
							//sem_wait(&debug_sem);
							
						}
					}
					if(node != NULL)
					{
						SEMA_POST(&(currentQue->wsmphore));
						memset(&(comm->response.respStruct.QueGet.MsgHdr), 0, sizeof(MSGHDR));
						memcpy(&(comm->response.respStruct.QueGet.MsgHdr), &(node->MH), sizeof(MSGHDR));
						putNode(&(comm->user.removedMsgQueue),node);
						comm->response.respStruct.QueGet.RetCode = QUE_SUCCESS;
					}
					else
					{
						if(comm->command.cmdStruct.QueGet.WaitOption != XIPC_BLOCKOPT_NOWAIT)
						{

							if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) {
								sprintf(msg,"Error in clock_gettime\n");
								logger(LOG_ERR,msg);
								return -1;
							}

							if(comm->command.cmdStruct.QueGet.Timeout > 0)
							{
								timeout.tv_sec += comm->command.cmdStruct.QueGet.Timeout;
								//printf("Timout secs %d\n",comm->command.cmdStruct.QueGet.Timeout);
							
							}
							else if(comm->command.cmdStruct.QueGet.WaitOption == XIPC_BLOCKOPT_WAIT)
							{
								timeout.tv_sec = 0;
							}
							SEMA_WAIT(&(shm_address->psmphore));
							for(i=0;i<MAX_WAIT_SEMAPHORES;i++)
							{
								if(shm_address->sema_pool[i].isBeingUsed == 0)
								{
									shm_address->sema_pool[i].isBeingUsed = 1;
									id = i;
									break;
								}
							}
							
							currentQue->poolPtr = &(shm_address->sema_pool[id]);
							currentQue->poolPtr->stop++;
							SEMA_POST(&(shm_address->psmphore));
							SEMA_POST(&(currentQue->wsmphore));
							
							if(timeout.tv_sec >0)
							{
								ret_code = SEMA_TIMEDWAIT(&(currentQue->poolPtr->tsmphore), &timeout);
							}
							else
							{
								SEMA_WAIT(&(currentQue->poolPtr->tsmphore));
								ret_code = 0;
							}
							SEMA_WAIT(&(shm_address->psmphore));
							if(currentQue->poolPtr != NULL)
							{
								currentQue->poolPtr->stop--;
								if(currentQue->poolPtr->stop == 0)
								{
									currentQue->poolPtr->isBeingUsed = 0;
									currentQue->poolPtr = NULL;
								}
							}
							SEMA_POST(&(shm_address->psmphore));

							SEMA_WAIT(&(currentQue->wsmphore));
							if(ret_code == 0)
							{
								
								node = NULL;
								node = getNode(currentQue);
								
								if(node != NULL)
								{
									memset(&(comm->response.respStruct.QueGet.MsgHdr), 0, sizeof(MSGHDR));
									memcpy(&(comm->response.respStruct.QueGet.MsgHdr), &(node->MH), sizeof(MSGHDR));
									putNode(&(comm->user.removedMsgQueue),node);
									comm->response.respStruct.QueGet.RetCode = QUE_SUCCESS;
								}
							}
							else
							{
								comm->response.respStruct.QueGet.RetCode = QUE_ER_TIMEOUT;
							}
							SEMA_POST(&(currentQue->wsmphore));
						}
						else
						{
							SEMA_POST(&(currentQue->wsmphore));
							comm->response.respStruct.QueGet.RetCode = QUE_ER_NOWAIT;
						}
					}
				}
			}
			else
			{
				comm->response.respStruct.QueGet.RetCode = QUE_ER_NOTFOUND;
			}
			comm->user.isWaitingForMsg = 0;
		break;

		case QUEUEWRITE:
			//Not need for Que Write implementation as it is not used in ATP
			ret_code = 0;
		break;
		case SHMMEMACCESS:
			//logger(LOG_INFO,"SHMACCESS\n");
			ret_code = 0;
			if(comm->command.cmdStruct.MemAccess.TableName != NULL)
			{
				mid = getMemIdByName(comm->command.cmdStruct.MemAccess.TableName);
				if(mid > -1)
				{
					ret_code = MEM_SUCCESS;
					comm->response.respStruct.MemAccess.RetMid = mid;
				}
				else
				{
					ret_code = MEM_ER_NOTFOUND;
					comm->response.respStruct.MemAccess.RetMid = -1;
				}
				comm->response.respStruct.MemAccess.RetCode = ret_code;
			}
			break;
		case SHMMEMSECDEF:
			
			ret_code = MEM_ER_BADSIZE;
			if((comm->command.cmdStruct.MemSecDef.Section.Mid > -1) && (comm->command.cmdStruct.MemSecDef.Section.Size > 0))
			{
				ret_code = sectionDef(comm->command.cmdStruct.MemSecDef.Section);
				
			}
			comm->response.respStruct.MemSecDef.RetCode = ret_code;
			if(ret_code < 0)
			{
				sprintf(msg,"ERROR in SHMMEMSECDEF RetCode = %d\n",ret_code);
				logger(LOG_ERR,msg);
			}
			break;
		case SHMMEMSECTION:
			//The MemSection function is implemented in Library function itself..
			break;
		case SHMMEMCREATE:
			
			mid = -1;
			ret_code = -1;
			if((comm->command.cmdStruct.MemCreate.TableName != NULL) && (comm->command.cmdStruct.MemCreate.TableSize > 0))
			{
				currentTable = NULL;
				ret_code = createTable(	comm->command.cmdStruct.MemCreate.TableName,
											comm->command.cmdStruct.MemCreate.TableSize, &currentTable);
				if(currentTable != NULL)
				{
					comm->response.respStruct.MemCreate.RetMid = currentTable->tableId;
				}
				else
				{
					comm->response.respStruct.MemCreate.RetMid = -1;
				}
				comm->response.respStruct.MemCreate.RetCode = ret_code;
			}
			if(ret_code < 0)
			{
				sprintf(msg,"ERROR in SHMMEMCREATE - User:\"%s\" - RetCode = %d\t Name = %s\tid=  %d\n",comm->user.username,comm->response.respStruct.MemCreate.RetCode, comm->command.cmdStruct.MemCreate.TableName,comm->response.respStruct.MemCreate.RetMid);
				logger(LOG_ERR,msg);
			}
			break;
		case SHMMEMINFOSYS:
			break;
		case SHMMEMDESTROY:
			ret_code = 0;
			if(comm->command.cmdStruct.MemDestroy.Mid > -1)
			{
				comm->response.respStruct.MemDestroy.RetCode = deleteTable(comm->command.cmdStruct.MemDestroy.Mid);
			}
			sprintf(msg,"SHMMEMDESTROY - User:\"%s\" - Mid = %d \t Ret_code = %d\n",comm->user.username,comm->command.cmdStruct.MemDestroy.Mid,comm->response.respStruct.MemDestroy.RetCode);
			logger(LOG_INFO,msg);
			break;
		case SHMMEMREAD:
			
			if(comm->command.cmdStruct.MemReadWrite.Buffer == NULL)
			{
				currentTable = NULL;
				changeTableById(&currentTable, comm->command.cmdStruct.MemReadWrite.Mid);
				if(currentTable != NULL)
				{
					comm->response.respStruct.MemReadWrite.RetCode = MemoryRead(currentTable, 
																&(comm->command.cmdStruct.MemReadWrite.Buffer),	
																comm->command.cmdStruct.MemReadWrite.Offset,
																comm->command.cmdStruct.MemReadWrite.Length);
					if(comm->response.respStruct.MemReadWrite.RetCode == 0)
					{
						comm->command.cmdStruct.MemReadWrite.Buffer = 
											(void *) (((char *)(comm->command.cmdStruct.MemReadWrite.Buffer)) - ((char *)sharedMemBaseAddr));
					}
					else
					{
						sprintf(msg,"ERROR in SHMMEMREAD - User:\"%s\" RetCode = %d \tOffset = %d\tSize=%d\tName=\"%s\"\n",comm->user.username,comm->response.respStruct.MemReadWrite.RetCode,comm->command.cmdStruct.MemReadWrite.Offset,comm->command.cmdStruct.MemReadWrite.Length,currentTable->tableName);
					}
				}
				else
				{
					comm->response.respStruct.MemReadWrite.RetCode = MEM_ER_BADMID;
				}
			}
			else
			{
				comm->response.respStruct.MemReadWrite.RetCode = MEM_ER_BADSECTION;
			}
			if(comm->response.respStruct.MemReadWrite.RetCode < 0)
			{
				logger(LOG_ERR,msg);
			}
			break;
			
		case SHMMEMWRITE:
			
			if(comm->command.cmdStruct.MemReadWrite.Buffer == NULL)
			{
				currentTable = NULL;
				changeTableById(&currentTable, comm->command.cmdStruct.MemReadWrite.Mid);
				if(currentTable != NULL)
				{
					comm->response.respStruct.MemReadWrite.RetCode = MemoryWrite(currentTable, 
																&comm->command.cmdStruct.MemReadWrite.Buffer,
																comm->command.cmdStruct.MemReadWrite.Offset,
																comm->command.cmdStruct.MemReadWrite.Length);
					if(comm->response.respStruct.MemReadWrite.RetCode == 0)
					{
						comm->command.cmdStruct.MemReadWrite.Buffer = 
											(void *) (((char *)(comm->command.cmdStruct.MemReadWrite.Buffer)) - ((char *)sharedMemBaseAddr));
					}
					
				}
				else
				{
					comm->response.respStruct.MemReadWrite.RetCode = MEM_ER_BADMID;
				}
			}
			else
			{
				comm->response.respStruct.MemReadWrite.RetCode = MEM_ER_BADSECTION;
			}
			if(comm->response.respStruct.MemReadWrite.RetCode < 0)
			{
				sprintf(msg,"ERROR in SHMMEMWRITE - User:\"%s\" RetCode = %d\tSize = %d\n",comm->user.username,comm->response.respStruct.MemReadWrite.RetCode,comm->command.cmdStruct.MemReadWrite.Length);
				logger(LOG_ERR,msg);
			}
			break;
			
		case SHMMEMLOCK:
			
			comm->response.respStruct.MemLock.RetCode = MEM_ER_BADMID;
			if(comm->command.cmdStruct.MemLock.MidList.Mid > -1)
			{
				currentTable = NULL;
				changeTableById(&currentTable, comm->command.cmdStruct.MemLock.MidList.Mid);
				if(currentTable != NULL)
				{
					ret_code = MemoryLock(currentTable, 
											comm->command.cmdStruct.MemLock.MidList.Offset, 
											comm->command.cmdStruct.MemLock.MidList.Size);
					//memcpy(&(comm->response.respStruct.MemLock.RetSection),&(comm->command.cmdStruct.MemLock.MidList), sizeof(SECTION));
					if(comm->response.respStruct.MemLock.RetCode == MEM_ER_BADSIZE)
					{
						sprintf(msg,"BADSIZEERROR SHMMEMLOCK - User:\"%s\" RetCode = %d \tName=\"%s\" Offset=%d datalen =%d Tblsize =%d\n",comm->user.username,ret_code,currentTable->tableName,comm->command.cmdStruct.MemLock.MidList.Offset, comm->command.cmdStruct.MemLock.MidList.Size, currentTable->maxTableSize);
					}

					comm->response.respStruct.MemLock.RetCode = ret_code;
					sprintf(msg,"SHMMEMLOCK - User:\"%s\" RetCode = %d \tName=\"%s\"\n",comm->user.username,ret_code,currentTable->tableName);
				}
				else
				{
					sprintf(msg,"SHMMEMLOCK - User:\"%s\" Current table NULL= %d\n",comm->user.username,comm->command.cmdStruct.MemLock.MidList.Mid);
				}
			}
			else
			{
				sprintf(msg,"SHMMEMLOCK  - User:\"%s\" MID= %d\n",comm->user.username,comm->command.cmdStruct.MemLock.MidList.Mid);
			}
			if(comm->response.respStruct.MemLock.RetCode < 0)
			{
				logger(LOG_ERR,msg);
			}
			break;
		case SHMMEMLIST:
			//MemList is implemented in Library function itself
			break;
		case SHMMEMSECTIONBUILD:
			//MemSectionBuild is implemented in Library function itself
			break;
		case SHMMEMUNLOCK:

			comm->response.respStruct.MemUnLock.RetCode = MEM_ER_BADMID;
			if(comm->command.cmdStruct.MemUnLock.MidList.Mid > -1)
			{
				currentTable = NULL;
				changeTableById(&currentTable, comm->command.cmdStruct.MemUnLock.MidList.Mid);
				if(currentTable != NULL)
				{
					ret_code = MemoryUnLock(currentTable, 
											comm->command.cmdStruct.MemUnLock.MidList.Offset, 
											comm->command.cmdStruct.MemUnLock.MidList.Size);
					//memcpy(&(comm->response.respStruct.MemUnLock.RetSection),&(comm->command.cmdStruct.MemUnLock.MidList[i]), sizeof(SECTION));
					comm->response.respStruct.MemUnLock.RetCode = ret_code;
					break;
				}
			}
			if(comm->response.respStruct.MemUnLock.RetCode < 0)
			{
				sprintf(msg,"ERROR in SHMMEMUNLOCK - User:\"%s\" RetCode = %d \tName=\"%s\"\n",comm->user.username,ret_code,currentTable->tableName);
				logger(LOG_INFO,msg);
			}
			break;
		case SHMMEMINFOMEM:
			
			comm->response.respStruct.MemInfoMem.RetCode = MEM_ER_BADMID;
			if(comm->command.cmdStruct.MemInfoMem.Mid > -1)
			{
				currentTable = NULL;
				changeTableById(&currentTable, comm->command.cmdStruct.MemInfoMem.Mid);
				if(currentTable != NULL)
				{
					comm->response.respStruct.MemInfoMem.InfoMem.Mid = currentTable->tableId;
					comm->response.respStruct.MemInfoMem.InfoMem.CreateUid = currentTable->createdBy;
					comm->response.respStruct.MemInfoMem.InfoMem.Size = currentTable->maxTableSize;
					strcpy(comm->response.respStruct.MemInfoMem.InfoMem.Name, currentTable->tableName);
				}
				comm->response.respStruct.MemInfoMem.RetCode = MEM_SUCCESS;
			}
			if(comm->response.respStruct.MemInfoMem.RetCode < 0)
			{
				sprintf(msg,"ERROR in SHMMEMINFOMEM - User:\"%s\" RetCode = %d \t Size = %d\n",comm->user.username,comm->response.respStruct.MemInfoMem.RetCode,comm->response.respStruct.MemInfoMem.InfoMem.Size );
				logger(LOG_INFO,msg);
			}
			break;

		case XCOMABORT:
			//Abort is implemented in the library function itself...
			break;
		case XCOMINFOUSER:
			
			if(comm->command.cmdStruct.XcomInfoUser.Uid < 0)
			{
				ret_code = XIPC_ER_BADUID;    // Invalid user name
			}
			else
			{
				ret_code = XIPC_ER_NOTFOUND;    // user name not found
				myUserID = comm->command.cmdStruct.XcomInfoUser.Uid;
				for(i=0;i<MAX_USERS;i++)
				{
					if(shm_address->cmdSlot[i].user.userid == myUserID)
					{
						comm->response.respStruct.XcomInfoUser.Uid = shm_address->cmdSlot[i].user.userid;
						comm->response.respStruct.XcomInfoUser.Pid = shm_address->cmdSlot[i].user.Pid;
						comm->response.respStruct.XcomInfoUser.Tid = shm_address->cmdSlot[i].user.Tid;
						
						strcpy(comm->response.respStruct.XcomInfoUser.Name, shm_address->cmdSlot[i].user.username);
						ret_code = XIPC_SUCCESS;
						break;
					}
				}
			}
			comm->response.respStruct.XcomInfoUser.RetCode= ret_code;
			break;

		default:
			ret_code = -1;
		break;
	}
	return ret_code;
}

#ifdef XCOMDEBUG

void DbxFindUserByName(char *buffer)
{
	int i=0;
	for(i=0;i<MAX_USERS;i++)
	{
		if(strcmp(shm_address->cmdSlot[i].user.username,buffer) == 0)
		{
			printf("DbxFindUserByName: User name = %s\t Slot = %d\t User ID\n",shm_address->cmdSlot[i].user.username,i,shm_address->cmdSlot[i].user.userid);
			break;
		}
	}
}
void DbxFindUserById(int uid)
{
	int i=0;
	for(i=0;i<MAX_USERS;i++)
	{
		if(shm_address->cmdSlot[i].user.userid == uid)
		{
			printf("DbxFindUserById: User name = %s\t Slot = %d\t User ID\n",shm_address->cmdSlot[i].user.username,i,shm_address->cmdSlot[i].user.userid);
			break;
		}
	}
}
void DbxFindQueueByName(char *buffer)
{
	int i=0;
	for(i=0;i<MAX_QUEUES;i++)
	{
		if(strcmp(shm_address->xcomQueues.userQueues[i].queueName,buffer) == 0)
		{
			printf("DbxFindQueueByName: Que name = %s\t Que id = %d\n",shm_address->xcomQueues.userQueues[i].queueName,shm_address->xcomQueues.userQueues[i].queueId);
			break;
		}
	}
}

#endif

void logger(int id, char *buffer)
{
	char buf[256] = {0};

	if( shm_address != NULL)
	{
		if(id == LOG_ERR && shm_address->isErrLogEnabled == 0 && __isErrLogEnabled == 0)
		{
			return;
		}
		if(id == LOG_INFO && shm_address->isInfoLogEnabled == 0 && __isInfoLogEnabled == 0)
		{
			return;
		}
	}

	sprintf(buf, "XCOM: %s", buffer);

#ifdef XCOMDEBUG
	if ( id == LOG_ERR )
		printf (buffer);
	syslog ( id, buf);
#else
	syslog ( id, buf);
#endif
}

