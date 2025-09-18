#include "shm_mem.h"
int err_code;
extern struct SH_MEM_TABLES *tableList;
extern struct SHR_MEM_LAYOUT *shm_address;
int isTableNameExists(char *tblname)
{
	int i = 0;
	int id = tableList->maxTableId;
	int foundEmpty = 0;
	for(i=0;i<MAX_SHM_TABLES;i++)
	{
		if(tableList->userTables[i].tableName[0] == '\0')
		{
			if(foundEmpty == 0)
			{
				id = i;	//Found an empty table. Still check whether are any tables with the same name.
				foundEmpty = 1;
			}
		}
		else if(strcmp(tableList->userTables[i].tableName, tblname) == 0)
		{
			id = i;
			break;
		}
	}
	if(i == MAX_SHM_TABLES && foundEmpty == 0)
	{
		id = i;
	}
	return id;
}

int createTable(char * tblname, int tblSize, struct SHM_TABLE ** tbl)
{
	//struct SHM_TABLE *tbl = NULL;
	*tbl = NULL;
	int ret_code = MEM_SUCCESS;
	int id = -1;
	id = isTableNameExists(tblname);
	if(id >= MAX_SHM_TABLES)
		return MEM_ER_NOMORE;
	if(id > tableList->maxTableId)
		tableList->maxTableId = id;
	if(tableList->userTables[id].tableName[0] == '\0')
	{
		*tbl = &(tableList->userTables[id]);
		(*tbl)->tablePtr = tableAlloc(tblSize);
		if((*tbl)->tablePtr == NULL)
		{
			printf("createTable: table alloc failed\n");
			ret_code = MEM_ER_BADSIZE;
		}
		else
		{
			if((SEMA_INIT(&((*tbl)->shsmphore),0,USYNC_PROCESS,NULL) != 0))
			{
				printf("createTable: semainit() failed shsmphore\n");
				ret_code = MEM_ER_MEMORY;
			}
			else
			{			
				if((SEMA_INIT(&((*tbl)->wsmphore),0,USYNC_PROCESS,NULL) != 0))
				{
					printf("createTable: semainit() failed wsmphore\n");
					ret_code = MEM_ER_MEMORY;
				}
				else
				{
					(*tbl)->tableId = id;
					tableList->totalTables++;
					memcpy((*tbl)->tableName,tblname,MAX_NAME_LENGTH);
					(*tbl)->isBeingUsed = 0;
					(*tbl)->maxTableSize = tblSize;
					(*tbl)->destroyed = 0;
					SEMA_POST(&((*tbl)->wsmphore));
					SEMA_POST(&((*tbl)->shsmphore));
					ret_code = MEM_SUCCESS;
				}
			}
		}
	}
	else
	{
		if(tableList->userTables[id].maxTableSize == tblSize)
		{
			tableList->totalTables++;
			*tbl = &(tableList->userTables[id]);
			(*tbl)->destroyed = 0;
			ret_code = MEM_SUCCESS;
		}
		else
		{
			ret_code = MEM_ER_DUPLICATE;
		}
	}
	return ret_code;
}

int getMemIdByName(char *tblname)
{
	int memid = -1;
	int i = 0;
	for(i=0;i<MAX_SHM_TABLES;i++)
	{
		if(strcmp(tableList->userTables[i].tableName, tblname) == 0)
		{
			if(tableList->userTables[i].destroyed == 0)
			{
				memid = i;
				break;
			}
		}
	}
	return memid;
}

void changeTableById(struct SHM_TABLE **tbl,int id)
{
	if(id > -1 && id < MAX_SHM_TABLES)
	{
		*tbl = &tableList->userTables[id];
	}
	else
	{
		//printf("There is no Table with the given QId\n");
		*tbl = NULL;
	}
}

int sectionDef(SECTION section)
{
	int ret = 0;
	int totalSize = MEM_ER_BADSECTION;
	struct SHM_TABLE *currentTable = NULL;
	
	changeTableById(&currentTable,section.Mid);

	if(currentTable == NULL)
	{
		ret = MEM_ER_NOTFOUND;
	}
	else
	{
		totalSize = section.Offset + section.Size;
		if(totalSize > currentTable->maxTableSize)
		{
			ret = MEM_ER_BADSIZE;
		}
		else
		{
			ret = MEM_SUCCESS;
		}
	}
	return ret;
}

int MemoryWrite(struct SHM_TABLE *currentTable, void **dataum, int offset, int datalen)
{
	int ret_code = MEM_ER_BADSECTION;
	void *writePtr = NULL;

	if((offset + datalen) > currentTable->maxTableSize)
	{
		ret_code = MEM_ER_BADSIZE;
	}
	else
	{
		if(sem_wait(&(currentTable->wsmphore)) == 0)
		{
			writePtr = ((char *)(currentTable->tablePtr)) + offset;
			//Calculate the pointer to write the data and send the location to the SHL. The SHL will copy the data
			*dataum = writePtr;
			currentTable->sizeofDataCurrentlyWritten = datalen;
			SEMA_POST(&(currentTable->wsmphore));
			ret_code = MEM_SUCCESS;
		}
		else
		{
			ret_code = -6666;
			//perror("\n");
		}
		
	}
	return ret_code;
}

int MemoryRead(struct SHM_TABLE *currentTable, void **dataum, int offset, int datalen)
{
	int ret_code = MEM_ER_BADSECTION;
	void *readPtr = NULL;
	if((offset + datalen) > currentTable->maxTableSize)
	{
		ret_code = MEM_ER_BADSIZE;
	}
	else
	{
		if(sem_wait(&(currentTable->wsmphore)) == 0)
		{
			readPtr = ((char *)(currentTable->tablePtr)) + offset;
			//Calculate the pointer to Read the data and send the location to the SHL. The SHL will copy the data
			*dataum = readPtr;
			SEMA_POST(&(currentTable->wsmphore));
			ret_code = MEM_SUCCESS;
		}
		else
		{
			ret_code = -7777;
		}
	}
	return ret_code;
}

int MemoryLock(struct SHM_TABLE *currentTable, int offset, int datalen)
{
	int ret_code = MEM_ER_BADSECTION;
	if((offset + datalen) > currentTable->maxTableSize)
	{
		ret_code = MEM_ER_BADSIZE;
	}
	else
	{
		if(sem_wait(&(currentTable->shsmphore)) == 0)
		{
			ret_code = MEM_SUCCESS;
		}
		else
		{
			//perror("MemLock ");
			ret_code = -8888;
		}
	}
	return ret_code;
}

int MemoryUnLock(struct SHM_TABLE *currentTable, int offset, int datalen)
{
	int ret_code = MEM_ER_BADSIZE;
	if((offset + datalen) > currentTable->maxTableSize)
	{
		ret_code = MEM_ER_BADSIZE;
	}
	else
	{
		if(sem_trywait(&(currentTable->shsmphore)) != 0)
		{
			if(sem_post(&(currentTable->shsmphore)) == 0)
			{
				ret_code = MEM_SUCCESS;
			}
			else
			{
				//perror("MemLock ");
				ret_code = -8888;
			}
		}
		else
		{
			ret_code = MEM_ER_NOTLOCKED;
			SEMA_POST(&(currentTable->shsmphore));
		}
	}
	return ret_code;
}


struct SHM_TABLE *getTableByName(char *tblname)
{
	struct SHM_TABLE *currentTable = NULL;
	int i = 0;
	for(i=0;i<MAX_SHM_TABLES;i++)
	{
		if(strcmp(tableList->userTables[i].tableName, tblname) == 0)
		{
			currentTable = &(tableList->userTables[i]);
			break;
		}
	}
	return currentTable;
}

int deleteTable(int mid)
{
	int ret_code = MEM_ER_BADMID;
	struct SHM_TABLE *currentTable = NULL;
	if ( mid > -1 && mid < MAX_SHM_TABLES )
	{
		currentTable = &(tableList->userTables[mid]);
		if(currentTable != NULL)
		{
			SEMA_WAIT(&(currentTable->wsmphore));

			currentTable->destroyed = 1;
			tableList->totalTables--;
			SEMA_POST(&(currentTable->wsmphore));

			ret_code = MEM_SUCCESS;

		}
	}
	return ret_code;
}

void * tableAlloc(int nSize)
{
	void *allocStart = NULL;
	if(shm_address != NULL)
	{
		int availableSize = shm_address->tableArea.size - shm_address->tableArea.usedSize;
		if(availableSize > nSize)
		{
			allocStart = ((shm_address->tableArea.memory) + shm_address->tableArea.usedSize);
			shm_address->tableArea.usedSize += nSize;		// Added by Nirbhay: increment the usedSize
		}
	}
	return allocStart;
}


