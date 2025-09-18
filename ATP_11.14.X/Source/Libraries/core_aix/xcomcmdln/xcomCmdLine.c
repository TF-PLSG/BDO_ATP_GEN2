#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#ifdef __STDC__
#include <stdlib.h>
#endif
#include "xipc.h"
#include "xcom.h"

#define USERNAME "xcomPrompt"
#define MAX_WATCH_QUEUES 100
char QueName[301][MAX_NAME_LENGTH];
XINT QNos;
XINT Mode;
XINT Cycles;
XINT MsgSize;
XINT currentSessionId = -1;
char currentSessionName[MAX_NAME_LENGTH];
char Instance[MAX_NAME_LENGTH];
int EndProcessSignalled = 0;

void * SendQ(void *param);
void * ReadQ(void *param);
void * GetQ(void *param);
pthread_t SQid,RQid;
pthread_t GQid[150];
int pos;
extern struct SHM_MEM_LAYOUT *shm_address;
extern QUEUES *queueList;
FILE *outptr = stdout;

CHAR  	Version[] = "ATP_11.1.0";

void signal_handler( int signal )
{
	fflush(stdin);
	fflush(stdout);
	EndProcessSignalled = 1;
}

int getNextString(char *str, char *out)
{
	int i=pos,j=0,w=0;
	if(i > strlen(str))
		return -1;
	while(str[i] != '\0')
	{
		if(str[i] == ' ' || str[i] == '\0')
		{
				break;
		}
		else
		{
			out[j++] = str[i];
		}
		i++;
	}
	pos = i+1;
	out[j] = '\0';
	return strlen(out);
}

void displayHelp()
{
	fprintf(outptr,"\nXCOM COMMAND LINE TOOL HELP:");
	fprintf(outptr,"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	fprintf(outptr,"\nNote: ");
	fprintf(outptr,"\nRule 1: All quename and tablename should start with alphabhet");
	fprintf(outptr,"\nRule 2: size and user id should be numeric.");
	fprintf(outptr,"\nany violation of these rules will result in unexpected behaviour of xcom command line tool\nand will affect the running ATP system.");
	fprintf(outptr,"\nThe short form of the commands can be used as specified in the commands list");
	fprintf(outptr,"\n");
	fprintf(outptr,"\nclear - clears the screen");
	fprintf(outptr,"\nhelp - displays this help content");
	fprintf(outptr,"\nQueAccess | qa	<quename> - Gets the ID of the specified Que");
	fprintf(outptr,"\nQueCreate | qc	<quename> - Creates the specified Que");
	fprintf(outptr,"\nQuePurge | qp		<quename> - Purges or clears the content of the specified Que");
	fprintf(outptr,"\nQueInfo | qi		<quename> - Displays the ID, Count and Owner of the specified Que ");
	fprintf(outptr,"\nQueDelete | QueDestroy | qd		<quename> - Destroys the specified Que from the XCOM");
	fprintf(outptr,"\nWatchQueue | wq	<quename1> [<quename2> <quename3> .... <quename100>] - Realtime watch window.");
	fprintf(outptr,"\n				Displays the message count of the specified Ques. It can display maximum of 100 queues");
	fprintf(outptr,"\nDisplayNonEmptyQues | neq - Displays the non empty Ques once ");
	fprintf(outptr,"\nDisplayNonEmptyQuesCont | neqc - A watch window of non empty ques");
	fprintf(outptr,"\nQueReadCont | qrc - Reads the specified Que continuously");
	fprintf(outptr,"\nMemAccess | ma	<tablename> - Displays the ID of the specified table");
	fprintf(outptr,"\nMemCreate | mc	<tablename> <size> - Create a table of the specified name and size");
	fprintf(outptr,"\nMemDestroy | md	<tablename> - Destroys the specified table");
	fprintf(outptr,"\nMemRead | mr		<tablename> <size> - Reads the specified table and size");
	fprintf(outptr,"\nMemWrite | mw		<tablename> <data> - writes the given data into the specified table. Here data is mostly characters");
	fprintf(outptr,"\nMemLock | ml		<tablename> <size> - Locks the specified table. User has to specify the size");
	fprintf(outptr,"\nMemUnlock | mul	<tablename> <size> - Locks the specified table. User has to specify the size");
	fprintf(outptr,"\nUserInfo | XcomInfoUser | XipcInfoUser | ui	<userid> - Displays the user information of the specified user ID");
	fprintf(outptr,"\nusers | ListAllUsers | aul - Lists all users currently logged in into the XCOM system");
	fprintf(outptr,"\nquit | q - exits the XCOM command line prompt");
	fprintf(outptr,"\n");

}

void processCommand(char *cmd)
{
	char s1[256] = {0}, s2[256] = {0};
	char quename[MAX_NAME_LENGTH] = {0};
	char watchQ[MAX_WATCH_QUEUES][MAX_NAME_LENGTH] = {0};
	int ret = 0, mid = -1, size = 0;
	char tableName[MAX_NAME_LENGTH] = {0},  tableSize[10] = {0};
	
	SECTION TempSec, RetSec;
	
	pos = 0;
	memset(s1,0,100);
	
		ret = getNextString(cmd,s1);
		if((strcasecmp(s1,"QueAccess") == 0) || (strcasecmp(s1,"qa") == 0))
		{
			memset(s1,0,100);
			ret = getNextString(cmd,s1);
			if(strlen(s1) == 0)
			{
				fprintf(outptr,"USAGE: QueAccess <quename>\n");
			}
			else
			{
				if(isalpha(s1[0]))
				{
					mid = QueAccess(s1);
					fprintf(outptr,"%s ~ %d\n",s1,mid);
				}
				else
				{
					mid = atoi(s1);
					if((mid == 0)&&(s1[0]!='0'))
						mid = -1;
					QUEINFOQUE queInfo;
					if(QueInfoQue(mid,&queInfo)>=0)
					{
						fprintf(outptr,"%s ~ %d\n",queInfo.Name,queInfo.Qid);
					}
				}
			}
		}
		else if((strcasecmp(s1,"QueCreate") == 0) || (strcasecmp(s1,"qc") == 0))
		{
			memset(s1,0,100);
			ret = getNextString(cmd,s1);
			if(strlen(s1) == 0)
			{
				fprintf(outptr,"USAGE: QueCreate <quename>\n");
			}
			else
			{
				if(isalpha(s1[0]))
				{
					mid = QueAccess(s1);
					if(mid < 0)
					{
						mid = QueCreate(s1, 0 ,0);
						fprintf(outptr,"%s ~ %d\n",s1,mid);
					}
					else
					{
						fprintf(outptr,"%s Que Already Exists ID = %d\n",s1,mid);
					}
				}
				else
				{
					fprintf(outptr,"QueName should start with alphabet\n");
				}
			}
		}
		else if((strcasecmp(s1,"QuePurge") == 0) || (strcasecmp(s1,"qp") == 0))
		{
			memset(s1,0,100);
			ret = getNextString(cmd,s1);
			if(strlen(s1) == 0)
			{
				fprintf(outptr,"USAGE: QuePurge <quename>\n");
			}
			else
			{
				if(isalpha(s1[0]))
				{
					mid = QueAccess(s1);
				}
				else
				{
					mid = atoi(s1);
					if((mid == 0)&&(s1[0]!='0'))
						mid = -1;
				}
				if(mid > -1)
				{
					ret = QuePurge(mid);
					fprintf(outptr,"QuePurge ret_code ~ %d\n",ret);
				}
				else
				{
					fprintf(outptr,"%s Que NOT FOUND\n",s1);
				}
			}
			
		}
		else if((strcasecmp(s1,"QueInfo") == 0) || (strcasecmp(s1,"qi") == 0))
		{
			memset(s1,0,100);
			ret = getNextString(cmd,s1);
			if(strlen(s1) == 0)
			{
				fprintf(outptr,"USAGE: QueInfo <quename>\n");
			}
			else
			{
				if(isalpha(s1[0]))
				{
					mid = QueAccess(s1);
				}
				else
				{
					mid = atoi(s1);
					if((mid == 0)&&(s1[0]!='0'))
						mid = -1;
				}
				QUEINFOQUE queInfo;
				if(QueInfoQue(mid,&queInfo)>=0)
				{
					fprintf(outptr,"Name        ~ %s\n",queInfo.Name);
					fprintf(outptr,"ID          ~ %d\n",queInfo.Qid);
					fprintf(outptr,"Count       ~ %d\n",queInfo.CountMessages);
					fprintf(outptr,"Created By  ~ %d",queInfo.CreateUid);
				}
				else
				{
					fprintf(outptr,"Error in QueInfo...\n");
				}
			}
		}
		else if((strcasecmp(s1,"QueList") == 0) || (strcasecmp(s1,"ques") == 0) || (strcasecmp(s1,"ql") == 0))
		{
			int i =0;
			fprintf(outptr,"Name\t\t");
			fprintf(outptr,"ID\t");
			fprintf(outptr,"Count\t");
			fprintf(outptr,"Created By\n");

			for(i=0;i<600;i++)
			{
				QUEINFOQUE queInfo;
				if(QueInfoQue(i,&queInfo)>=0)
				{
					fprintf(outptr,"%15s\t",queInfo.Name);
					fprintf(outptr,"%d\t",queInfo.Qid);
					fprintf(outptr,"%d\t",queInfo.CountMessages);
					fprintf(outptr,"%d\n",queInfo.CreateUid);
				}
			}
		}
		else if((strcasecmp(s1,"QueDelete") == 0) || (strcasecmp(s1,"QueDestroy") == 0) || (strcasecmp(s1,"qd") == 0))
		{
			memset(s1,0,100);
			ret = getNextString(cmd,s1);
			if(strlen(s1) == 0)
			{
				fprintf(outptr,"USAGE: QueDelete <quename>\n");
			}
			else
			{
				if(isalpha(s1[0]))
				{
					mid = QueAccess(s1);
				}
				else
				{
					mid = atoi(s1);
					if((mid == 0)&&(s1[0]!='0'))
						mid = -1;

				}
				if(mid <0)
				{
					fprintf(outptr,"%s Que NOT FOUND\n",s1);
				}
				else
				{
					if(QueDestroy(mid) <0)
					{
						fprintf(outptr,"QueDestroy failed %s ~ %d\n",s1,mid);
					}
					else
					{
						fprintf(outptr,"%s ~ %d Destroyed\n",s1,mid);
					}
				}
			}
		}
		else if((strcasecmp(s1,"WatchQueue") == 0) || (strcasecmp(s1,"wq") == 0))
		{
			int i=0;
			memset(s1,0,100);
			memset(watchQ,0,MAX_WATCH_QUEUES*MAX_NAME_LENGTH);
			
			do
			{
				ret = getNextString(cmd,watchQ[i]);
				i++;
			}while((ret > 0) && (i<MAX_WATCH_QUEUES));
			if(i == 1){
				fprintf(outptr,"You have enter atleast one Queue\n");
				fprintf(outptr,"USAGE: WatchQueue <quename1> [<quename2> ....]\n");
				fprintf(outptr,"Maximum of %d queues can be added to watch window\n",MAX_WATCH_QUEUES);
				return;
			}
			while(EndProcessSignalled == 0)
			{
				int  n = 0;
				fprintf(outptr,"\033[2KName\t\tID\t\tCount\n");
				++n;
					for(i=0;i<MAX_WATCH_QUEUES;i++)
					{
						mid = QueAccess(watchQ[i]);
						if(mid > -1)
						{
							QUEINFOQUE queInfo;
							if(QueInfoQue(mid,&queInfo)>=0)
							{
								fprintf(outptr,"\033[2K%10s\t%d\t\t%d\n",queInfo.Name,queInfo.Qid,queInfo.CountMessages);
							}
							else
							{
								fprintf(outptr,"\033[2K%10s\tNA\t\tNA\n",watchQ[i]);
							}
							++n;
						}
					}
				sleep(1);
				fprintf(outptr,"\033[%dF",n);
			}
			fprintf(outptr,"\n");
			EndProcessSignalled = 0;
		}
		else if((strcasecmp(s1,"DisplayNonEmptyQues") == 0) || (strcasecmp(s1,"neq") == 0))
		{
			int i=0;
			memset(s1,0,100);
			
			fprintf(outptr,"Name\t\tID\t\tCount\n");
			for(i=0;i<size;i++)
			{
				QUEINFOQUE queInfo;
				if(QueInfoQue(i,&queInfo)>=0)
				{
					if(queInfo.CountMessages > 0)
					{
						fprintf(outptr,"%s\t\t%d\t\t%d\n",queInfo.Name,queInfo.Qid,queInfo.CountMessages);
					
					}
				}
			}
		}
		else if((strcasecmp(s1,"DisplayNonEmptyQuesCont") == 0) || (strcasecmp(s1,"neqc") == 0))
		{
			int i=0;
			int n=0;
			long count = 0;
			memset(s1,0,100);
			//ret = getNextString(cmd,s1);
			ret = getNextString(cmd,tableSize);
			if(strlen(tableSize) == 0)
			{
				size = 300;
			}
			else
			{
				size = atoi(tableSize);
			}
			while(EndProcessSignalled == 0)
			{
				n=0;
				count = 0;
				//system("clear");
				fprintf(outptr,"\033[2KName\t\tID\t\tCount\n");
				++n;
					for(i=0;i<size;i++)
					{
						QUEINFOQUE queInfo;
						if(QueInfoQue(i,&queInfo)>=0)
						{
							if(queInfo.CountMessages > 0)
							{
								fprintf(outptr,"%10s\t%d\t\t%d\n",queInfo.Name,queInfo.Qid,queInfo.CountMessages);
								count = count + queInfo.CountMessages;
								++n;
							}
						}
					}
					fprintf(outptr,"Total Messages = %ld\n",count);
					fprintf(outptr,"\033[s");
					++n;
				sleep(1);
				fprintf(outptr,"\033[%dF",n);
			}
			EndProcessSignalled = 0;
			fprintf(outptr,"\033[u");
			fprintf(outptr,"\n");
		}
		else if((strcasecmp(s1,"QueReadCont") == 0) || (strcasecmp(s1,"qrc") == 0))
		{
			int Qid = -1,ret_code = -1;
			long totalMessages = 0;
				QIDLIST QidList;

			CHAR *p_msg = NULL;
			MSGHDR   message_hdr;
			memset(quename,0,MAX_NAME_LENGTH);
			ret = getNextString(cmd,quename);

			fprintf(outptr,"You are not authorized to run this command..\n");
			return;
			
			//mid = QueAccess(s1);
			fprintf(outptr,"\n");
			if(strlen(quename) != 0)
			{
				while(EndProcessSignalled == 0)
				{
					//Cid = QueAccess(cQue);
					Qid = QueAccess(quename);
					if(Qid < 0)
					{
						fprintf(outptr,"GetQ: Que %s doesnot exists... \n",quename);
						break;
					}
					else
					{
							QueListBuild(QidList,Qid, QUE_EOL);
							ret_code = QueGet ( &message_hdr, QUE_Q_HP, QidList, NULL_PTR, NULL_PTR, QUE_WAIT );
							if(ret_code < 0)
							{
								fprintf(outptr,"QueGet failed for Queue \"%s\" ret_code= %d\n",quename,ret_code);

								if(ret_code == -1101)
								{
									fprintf(outptr,"\n\nQueGet Failed because of Queues\n\n");
									while(ret_code < 0)
										sleep(10);
								}
								ret_code = 0;
							}
							else
							{
								if(message_hdr.Size > 0)
								{
									p_msg = malloc (message_hdr.Size);
									if (p_msg == NULL_PTR)
									{
										fprintf(outptr,"Malloc error \"%s\"\n",quename);
									}								
									else
									{
										//fprintf(outptr,"Reading to queue %s\n",quename);
										ret_code = QueRead (&message_hdr, p_msg, message_hdr.Size);
										
										if(ret_code < 0)
										{
											fprintf(outptr,"QueRead failed for Queue \"%s\" ret_code = %d\n",quename,ret_code);
										}
										else
										{
											totalMessages++;
										}
										free (p_msg);
										p_msg = NULL_PTR;
									}
								}
							}
					}
					fprintf(outptr,"\rTotal messages read = %d \t %s",totalMessages, quename);
				}
			}
			
			EndProcessSignalled = 0;
		}
		else if((strcasecmp(s1,"MemAccess") == 0) || (strcasecmp(s1,"ma") == 0))
		{
			memset(s1,0,100);
			ret = getNextString(cmd,s1);
			if(strlen(s1) == 0)
			{
				fprintf(outptr,"USAGE: MemAccess <tablename>\n");
				return;
			}
			mid = MemAccess(s1);
			fprintf(outptr,"%s ~ %d\n",s1,mid);
		}
		else if(strcasecmp(s1,"MemSecDef") == 0)
		{
			fprintf(outptr,"This command is obselete\n");
		}
		else if(strcasecmp(s1,"MemSection") == 0)
		{
			fprintf(outptr,"This command is obselete\n");
		}
		else if((strcasecmp(s1,"MemCreate") == 0) || (strcasecmp(s1,"mc") == 0))
		{
			memset(s1,0,100);
			ret = getNextString(cmd,tableName);
			if(strlen(tableName) == 0)
			{
				fprintf(outptr,"USAGE: MemCreate <tablename> <tablesize>\n");
				return;
			}
			ret = getNextString(cmd,tableSize);
			if(strlen(tableSize) == 0)
			{
				fprintf(outptr,"Enter table size\n");
				return;
			}
			size = atoi(tableSize);
			ret = MemCreate(tableName,size);
			fprintf(outptr,"Return Code = %d\n",ret);
		}
		else if(strcasecmp(s1,"MemInfoSys") == 0)
		{
			fprintf(outptr,"This command is obselete\n");
		}
		else if((strcasecmp(s1,"MemDestroy") == 0) || (strcasecmp(s1,"md") == 0))
		{
			ret = getNextString(cmd,tableName);
			if(strlen(tableName) == 0)
			{
				fprintf(outptr,"USAGE: MemDestroy <tablename>\n");
				return;
			}
			ret = MemAccess(tableName);
			if(ret >= 0)
			{
				ret = MemDestroy(ret);
			}
			fprintf(outptr,"Return Code = %d\n",ret);
		}
		else if((strcasecmp(s1,"MemRead") == 0) || (strcasecmp(s1,"mr") == 0))
		{
			memset(s1,0,100);
			ret = getNextString(cmd,tableName);
			if(strlen(tableName) == 0)
			{
				fprintf(outptr,"USAGE: MemRead <tablename> <size>\n");
				return;
			}
			ret = getNextString(cmd,s2);
			size = atoi(s2);
			if(size == 0)
			{
				fprintf(outptr,"USAGE: MemRead <tablename> <size>\n");
				return;
			}
			mid = MemAccess(tableName);
			if(mid < 0)
			{
				fprintf(outptr,"Table does not exist\n");
			}
			else
			{
				ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, mid, 0, size + 1),MEM_EOL), &RetSec, MEM_WAIT);
				if(ret >= 0)
				{
					ret = MemRead(mid,0, size,	s2,MEM_WAIT);
					fprintf(outptr,"Return Code		= %d\n",ret);
					fprintf(outptr,"Table Contents	= %s\n",s2+1);
					ret = MemUnlock(MemList(*MemSectionBuild(&TempSec, mid, 0, size + 1), MEM_EOL), &RetSec);
				}
				else
				{
					fprintf(outptr,"Cannot lock the table Return Code = %d\n",ret);
				}
			}
		}
		else if((strcasecmp(s1,"MemWrite") == 0) || (strcasecmp(s1,"mw") == 0))
		{
			memset(s1,0,100);
			ret = getNextString(cmd,tableName);
			if(strlen(tableName) == 0)
			{
				fprintf(outptr,"USAGE: MemWrite <tablename> <data>\n");
				return;
			}
			ret = getNextString(cmd,s2);
			size = strlen(s2);
			if(size == 0)
			{
				fprintf(outptr,"USAGE: MemWrite <tablename> <data>\n");
				return;
			}
			mid = MemAccess(tableName);
			if(mid < 0)
			{
				fprintf(outptr,"Table does not exist\n");
			}
			else
			{
				ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, mid, 0, size + 1),MEM_EOL), &RetSec, MEM_WAIT);
				if(ret >= 0)
				{
					ret = MemWrite(mid,0, size,	s2,MEM_WAIT);
					fprintf(outptr,"Return Code = %d\n",ret);
					ret = MemUnlock(MemList(*MemSectionBuild(&TempSec, mid, 0, size + 1), MEM_EOL), &RetSec);
				}
				else
				{
					fprintf(outptr,"Cannot lock the table Return Code = %d\n",ret);
				}
			}
		}
		else if((strcasecmp(s1,"MemLock") == 0) || (strcasecmp(s1,"ml") == 0))
		{
			memset(s1,0,100);
			ret = getNextString(cmd,tableName);
			if(strlen(tableName) == 0)
			{
				fprintf(outptr,"USAGE: MemLock <tablename> <size>\n");
				return;
			}
			ret = getNextString(cmd,s2);
			size = strlen(s2);
			if(size == 0)
			{
				fprintf(outptr,"USAGE: MemLock <tablename> <size>\n");
				return;
			}
			mid = MemAccess(tableName);
			if(mid < 0)
			{
				fprintf(outptr,"Table does not exist\n");
			}
			else
			{
				ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, mid, 0, size + 1),MEM_EOL), &RetSec, MEM_WAIT);
				fprintf(outptr,"Return Code = %d\n",ret);
			}
		}
		else if(strcasecmp(s1,"Tables") == 0 || strcasecmp(s1,"ListTables") == 0 || strcasecmp(s1,"memtables") == 0)
		{
			//fprintf(outptr,"This command is obselete\n");
			//return;
			int i=0;
			MEMINFOMEM MemInfo;
			fprintf(outptr,"ID \t%-25s\tSIZE\n","NAME");
			for(i=0;i<MAX_SHM_TABLES;i++)
			{
				memset( &MemInfo, 0x00, sizeof(MEMINFOMEM));
				ret = MemInfoMem(i,&MemInfo);
				if(ret == 0 && MemInfo.Mid > -1)
				{
					fprintf(outptr,"%03d\t%-25s\t%d\n",MemInfo.Mid, MemInfo.Name, MemInfo.Size);
				}
			}
		}
		else if(strcasecmp(s1,"MemSectionBuild") == 0)
		{
			fprintf(outptr,"This command is obselete\n");
		}
		else if((strcasecmp(s1,"MemUnlock") == 0) || (strcasecmp(s1,"mul") == 0))
		{
			memset(s1,0,100);
			ret = getNextString(cmd,tableName);
			if(strlen(tableName) == 0)
			{
				fprintf(outptr,"USAGE: MemUnLock <tablename> <size>\n");
				return;
			}
			ret = getNextString(cmd,s2);
			size = strlen(s2);
			if(size == 0)
			{
				fprintf(outptr,"USAGE: MemLock <tablename> <size>\n");
				return;
			}
			mid = MemAccess(tableName);
			if(mid < 0)
			{
				fprintf(outptr,"Table does not exist\n");
			}
			else
			{
				ret = MemUnlock(MemList(*MemSectionBuild(&TempSec, mid, 0, size + 1), MEM_EOL), &RetSec);
				fprintf(outptr,"Return Code = %d\n",ret);
			}
		}
		else if((strcasecmp(s1,"MemInfoMem") == 0) || (strcasecmp(s1,"mim") == 0) || (strcasecmp(s1,"TableInfo") == 0) || (strcasecmp(s1,"ti") == 0))
		{
			MEMINFOMEM MemInfo;
			memset( &MemInfo, 0x00, sizeof(MEMINFOMEM) );
			memset(s1,0,100);
			ret = getNextString(cmd,tableName);
			if(strlen(tableName) == 0)
			{
				fprintf(outptr,"USAGE: MemInfoMem <tablename> <size>\n");
				return;
			}
			if(isalpha(tableName[0]))
			{
				mid = MemAccess(tableName);
			}
			else
			{
				mid = atoi(tableName);
				if((mid == 0)&&(tableName[0]!='0'))
					mid = -1;
			}
			if(mid < 0)
			{
				fprintf(outptr,"Table does not exist\n");
			}
			else
			{
				ret = MemInfoMem(mid, &MemInfo);
				if(ret == 0)
				{
					fprintf(outptr,"Table ID:   %d\n", MemInfo.Mid);
					fprintf(outptr,"Table Name: %s\n", MemInfo.Name);
					fprintf(outptr,"Created By: %d\n", MemInfo.CreateUid);
					fprintf(outptr,"Max Size  : %d\n", MemInfo.Size);
				}
			}
		}
		else if((strcasecmp(s1,"MIMS") == 0))
		{
			MEMINFOMEM MemInfo;
			int i = 0;
			memset( &MemInfo, 0x00, sizeof(MEMINFOMEM) );
			memset(s1,0,100);
			ret = getNextString(cmd,tableName);
			if(strlen(tableName) == 0)
			{
				fprintf(outptr,"USAGE: MemInfoMem <tablename> <size>\n");
				return;
			}
			if(isalpha(tableName[0]))
			{
				for(i=0;i<MAX_SHM_TABLES;i++)
				{
					if(strcmp(((struct SHR_MEM_LAYOUT*)shm_address)->xcomTables.userTables[i].tableName, tableName) == 0)
					{
						if(((struct SHR_MEM_LAYOUT*)shm_address)->xcomTables.userTables[i].destroyed == 0)
						{
							mid = i;
							break;
						}
						else
						{
							printf("The Table was Destroyed\n");
							mid = -1;
						}
					}
				}
				mid = MemAccess(tableName);
			}
			else
			{
				mid = atoi(tableName);
				if((mid == 0)&&(tableName[0]!='0'))
					mid = -1;
			}
			if(mid < 0)
			{
				fprintf(outptr,"Table does not exist\n");
			}
			else
			{
				ret = MemInfoMem(mid, &MemInfo);
				if(ret == 0)
				{
					fprintf(outptr,"Table ID:   %d\n", MemInfo.Mid);
					fprintf(outptr,"Table Name: %s\n", MemInfo.Name);
					fprintf(outptr,"Created By: %d\n", MemInfo.CreateUid);
					fprintf(outptr,"Max Size  : %d\n", MemInfo.Size);
				}
				printf("Additional info\n");
				printf("Max Size : %d\n",((struct SHR_MEM_LAYOUT*)shm_address)->xcomTables.userTables[mid].maxTableSize);
				if(((struct SHR_MEM_LAYOUT*)shm_address)->xcomTables.userTables[mid].destroyed == 1)
				{
					printf("The table is destroyed\n");
				}
				else
				{
					printf("Rare case: The destroyed flag = %d\n",((struct SHR_MEM_LAYOUT*)shm_address)->xcomTables.userTables[mid].destroyed);
				}

			}
		}
		else if((strcasecmp(s1,"UserInfo") == 0) || (strcasecmp(s1,"XcomInfoUser") == 0) || (strcasecmp(s1,"XipcInfoUser") == 0) || (strcasecmp(s1,"ui") == 0))
		{
			XIPCINFOUSER infoUser;
			int done = 0;
			memset(&infoUser, 0, sizeof(XIPCINFOUSER));
			memset(s1,0,100);
			ret = getNextString(cmd,s1);
			int i = 0;
			if(strlen(s1) == 0)
			{
				fprintf(outptr,"Enter User id:");
				scanf("%d",&size);
			}
			else
			{
				if(isalpha(s1[0]))
				{
					printf("Retrieving user information. Please wait... \n\n");
					size = -1;
					done = 1;

					for(i=0;i<MAX_USERS;i++)
					{
						ret = XipcInfoUser(i,&infoUser);
						if(ret == 0 && infoUser.Uid > -1)
						{
							if(strcasecmp(infoUser.Name, s1) == 0)
							{
								fprintf(outptr,"InfoUser.Uid          = %d\n",infoUser.Uid);
								fprintf(outptr,"InfoUser.Pid          = %d\n",infoUser.Pid);
								fprintf(outptr,"InfoUser.Tid          = %d\n",infoUser.Tid);
								fprintf(outptr,"InfoUser.Name         = %s\n",infoUser.Name);
								fprintf(outptr,"RemovedMsgQueue Count = %d\n",((struct SHR_MEM_LAYOUT*)shm_address)->cmdSlot[infoUser.Uid].user.removedMsgQueue.totalMessages);


								i = MAX_USERS;
								size = infoUser.Uid;
							}
						}
					}
					if(size == -1)
					{
						fprintf(outptr,"User '%s' not logged in. \n", s1);
					}
				}
				else
				{
					size = atoi(s1);
				}
			}
			if(done == 0)
			{
				ret = XipcInfoUser(size,&infoUser);
				if(ret == 0)
				{
					fprintf(outptr,"InfoUser.Uid  = %d\n",infoUser.Uid);
					fprintf(outptr,"InfoUser.Pid  = %d\n",infoUser.Pid);
					fprintf(outptr,"InfoUser.Tid  = %d\n",infoUser.Tid);
					fprintf(outptr,"InfoUser.Name = %s\n",infoUser.Name);
				}
				if(ret == XIPC_ER_NOTFOUND)
				{
					fprintf(outptr,"User name not found\n");
				}

				fprintf(outptr,"Return Code = %d\n",ret);
			}
			
		}
		else if((strcasecmp(s1,"users") == 0) || (strcasecmp(s1,"ListAllUsers") == 0) || (strcasecmp(s1,"aul") == 0))
		{
			int i=0;
			int count=0;
			CHAR        sysCommand[200] = "";
			CHAR	grep[100] = {0};
			int retval = 0;
			XIPCINFOUSER infoUser;
			memset(&infoUser, 0, sizeof(XIPCINFOUSER));
			memset(s1,0,100);
			ret = getNextString(cmd,s1);
			fprintf(outptr,"User Id\t\t\tUser Name\tProcessID\tStatus\n");
			for(i=0;i<MAX_USERS;i++)
			{
				size = i;
				ret = XipcInfoUser(size,&infoUser);
				if(ret == 0 && infoUser.Uid > -1)
				{
					sprintf( sysCommand,
					"ps -ef | grep %d | grep -v grep > /dev/null",  /* Do not display errors. */
					infoUser.Pid, grep);					
					retval = system( sysCommand );

					if ( (retval == 1) || (retval == 256)  ) /* 256=no Pid to kill */
					{//Dead process
						if(strcasecmp(s1,"CleanUp") == 0)
						{
							ret = XipcAbort(infoUser.Uid);
							fprintf(outptr,"%d\t",infoUser.Uid);
							fprintf(outptr,"%25s\t",infoUser.Name);
							fprintf(outptr,"% 8d\t",infoUser.Pid);
							if(ret == 0)
							{
								fprintf(outptr,"LOGOUT\n");
							}
							else
							{
								fprintf(outptr,"UNABLE TO LOGOUT\n");
							}
						}
						else if(strcasecmp(s1,"LIVE") != 0)
						{
							fprintf(outptr,"%d\t",infoUser.Uid);
							fprintf(outptr,"%25s\t",infoUser.Name);
							fprintf(outptr,"% 8d\t",infoUser.Pid);
							fprintf(outptr,"DEAD\n");
						}
					}
					else
					{
						if((strcasecmp(s1,"DEAD") != 0) && (strcasecmp(s1,"CleanUp") != 0))
						{
							fprintf(outptr,"%d\t",infoUser.Uid);
							fprintf(outptr,"%25s\t",infoUser.Name);
							fprintf(outptr,"% 8d\t",infoUser.Pid);
							fprintf(outptr,"LIVE\n");
						}
					}
					count++;
				}
			}
			fprintf(outptr,"Total logged in users = %d\n",count);
		}
		else if((strcasecmp(s1,"checkuserspid") == 0) || (strcasecmp(s1,"cup") == 0))
		{
			int i=0;
			int count=0;
			CHAR        sysCommand[200] = "";
			int retval = 0;
			XIPCINFOUSER infoUser;
			memset(&infoUser, 0, sizeof(XIPCINFOUSER));
			memset(s1,0,100);
			fprintf(outptr,"User Id\t\t\tUser Name\tProcessID\tSTATUS\n");
			for(i=0;i<MAX_USERS;i++)
			{
				size = i;
				ret = XipcInfoUser(size,&infoUser);
				if(ret == 0 && infoUser.Uid > -1)
				{
					fprintf(outptr,"%d\t",infoUser.Uid);
					fprintf(outptr,"%25s\t\t",infoUser.Name);
					fprintf(outptr,"% 8d\t",infoUser.Pid);

					sprintf( sysCommand,
					"ps -ef | grep %d | grep -v grep > /dev/null",  /* Do not display errors. */
					infoUser.Pid);
					
					retval = system( sysCommand );

					if ( (retval == 1) || (retval == 256)  ) /* 256=no Pid to kill */
					{
						fprintf(outptr,"DEAD\n");
					}
					else
					{
						fprintf(outptr,"LIVE\n");
					}
					count++;
				}
			}
			fprintf(outptr,"Total logged in users = %d\n",count);
		}
		else if((strcasecmp(s1,"XcomAbort") == 0) || (strcasecmp(s1,"XipcAbort") == 0) || (strcasecmp(s1,"xa") == 0))
		{
			XIPCINFOUSER infoUser;
			memset(&infoUser, 0, sizeof(XIPCINFOUSER));
			memset(s1,0,100);
			ret = getNextString(cmd,s1);
			int uid=-1;
			if(strlen(s1) == 0)
			{
				fprintf(outptr,"Enter User id:");
				scanf("%d",&size);
			}
			else
			{
				uid = atoi(s1);
			}
			if(uid == currentSessionId)
			{
				fprintf(outptr,"Cannot abort self. Use 'selfabort' command\n");
				return;
			}
			ret = XipcInfoUser(uid,&infoUser);
			if(ret == 0)
			{
				ret = XipcAbort(uid);
				if(ret == 0)
				{
					fprintf(outptr,"User %s (%d) has aborted\n",infoUser.Name, infoUser.Uid);
				}
				else
				{
					fprintf(outptr,"Cannot abort the user %s (%d)\n",infoUser.Name, infoUser.Uid);
				}
			}
			if(ret == XIPC_ER_NOTFOUND)
			{
				fprintf(outptr,"User name not found\n");
			}
			fprintf(outptr,"Return Code = %d\n",ret);
		}
		else if((strcasecmp(s1,"SelfAbort") == 0) || (strcasecmp(s1,"sa") == 0))
		{
			int i=0;
			int sessionCount = 0;
			XIPCINFOUSER infoUser;
			memset(&infoUser, 0, sizeof(XIPCINFOUSER));
			memset(s1,0,100);
			for(i=0;i<MAX_USERS;i++)
			{
				size = i;
				ret = XipcInfoUser(size,&infoUser);
				if(ret == 0 && infoUser.Uid > -1)
				{
					if(strcmp(infoUser.Name,currentSessionName) == 0)
					{
						if(infoUser.Uid != currentSessionId)
						{
							ret = XipcAbort(infoUser.Uid);
							if(ret == 0)
							{
								sessionCount++;
							}
						}
					}
				}
			}
			if(sessionCount >0)
			{
				fprintf(outptr,"%d session(s) of xcomPrompt has been aborted\n",sessionCount);
			}
			else
			{
				fprintf(outptr,"No other sessions of xcomPrompt are logged in\n");
			}
			fprintf(outptr,"Current session user id is %d\n",currentSessionId);
		}
		else if((strcasecmp(cmd,"help") == 0))
		{
			displayHelp();
		}else
		{
			fprintf(outptr,"%s unrecognized command\n",cmd);
			fprintf(outptr,"type 'help' for list of commands available\n");
		}
}

void processRedirectCommand(char *cmd)
{
	char *pch;
	pch = strchr(cmd,'>');
	int pos = pch-cmd;
	if(cmd[pos-1] == ' ')
		cmd[pos-1] = '\0';
	else
		cmd[pos] = '\0';
	pch = cmd+pos+1;
	if(pch[0] == ' ')
		pch = pch+1;
	//fprintf(stdout,"%s\n",cmd);
	//fprintf(stdout,"%s\n",pch);
	FILE *fptr = NULL;
	fptr = fopen(pch,"w");
	if(fptr != NULL)
	{
		outptr = fptr;
		processCommand(cmd);
	}
	else
	{
		fprintf(stdout,"%s: A file or directory in the path name does not exist.");
	}
	
	outptr = stdout;
	fclose(fptr);
}

int main(int argc, char *argv[])
{

	XINT Qid;
	XINT ret_code = 0;
	QIDLIST QidList;
	time_t ltime;
	//struct tm *start_tm,*end_tm, *qs_start, *qs_end, *qg_start, *qg_end;
	CHAR *p_msg = NULL;
	MSGHDR   message_hdr;

	XINT StartTime;
	XINT FinTime;
	
	CHAR *Buf;
	
	char input[512] = {0};
	char commandline[512] = {0};
	int exit = 0;
	strcpy(currentSessionName,USERNAME);
	strcpy(Instance,"xcomPrompt:pte_ipc");
	if( signal( SIGTERM, SIG_IGN ) != SIG_IGN )
			signal( SIGTERM, signal_handler );
	if( signal( SIGINT, SIG_IGN ) != SIG_IGN )
			signal( SIGINT, signal_handler );

	//ltime=time(NULL);
	//start_tm=localtime(&ltime);
	Cycles = 1;
	MsgSize = 20221;
	QNos = 0; //used as no of queues
	int i =0;
	char cQue[MAX_NAME_LENGTH] = {0};
	ret_code = XipcLogin(Instance, currentSessionName);
	usleep(20 * 1000);
	fprintf(outptr,"xcom prompt\n");
	if(ret_code < 0)
	{
		fprintf(outptr,"Login user %s failed; ret_code = %d\n", currentSessionName, ret_code);
		return -1;
	}
	currentSessionId = ret_code;
	
	if(argc > 1)
	{
		i=1;
		for(i=1;i<argc;i++)
		{
			strcat(input, argv[i]);
		}
		processCommand(input);
		XipcLogout();

		return 0;
	}
	system("clear");
	do
	{	
		EndProcessSignalled = 0;
		outptr = stdout;
		if( signal( SIGTERM, SIG_IGN ) != SIG_IGN )
			signal( SIGTERM, signal_handler );
	if( signal( SIGINT, SIG_IGN ) != SIG_IGN )
			signal( SIGINT, signal_handler );

		fprintf(outptr,"\nxcom>\033[K");
		fflush(stdin);
		memset(input,0,512);
		scanf("%[^\n]",input);
		if((strcasecmp(input,"quit") == 0) || (strcasecmp(input,"q") == 0))
		{
			exit = 1;
		}
		else if((strcasecmp(input,"clear") == 0))
		{
			fflush(stdin);
			fflush(stdout);
			system("clear");
		}
		else if((strcasecmp(input,"help") == 0))
		{
			displayHelp();
		}
		else if(strchr(input,'>') != NULL)
		{
			processRedirectCommand(input);
		}
		else if(input[0] == '!')
		{
			system(&input[1]);
		}
		else if(strlen(input) != 0)
		{
			processCommand(input);
		}
		else
		{
			//fprintf(outptr,"\n");
		}
	}while(exit != 1);
		XipcLogout();

	return 0;
}
