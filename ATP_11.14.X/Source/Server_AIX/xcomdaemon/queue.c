// new1.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>

#include "queue.h"
#include "mymalloc.h"	// Added to ensure that macro PADDING_TEST gets defined in this file. Do not remove.
int err_code;
extern QUEUES *queueList;
extern void *sharedMemBaseAddr;
int queueDestroy(int qid)
{
	QUEUE *que = NULL;
	int ret_code = QUE_ER_BADQID;
	int i = 0;

	ret_code = queuePurge(qid);
	if(ret_code == QUE_SUCCESS)
	{
		que = &(queueList->userQueues[qid]);
		hash_remove(que->queueName);
		que->queueId = -1;
		memset(que->queueName,0,MAX_NAME_LENGTH);
		que->createdBy = -1;
		SEMA_DESTROY(&(que->wsmphore));
		SEMA_WAIT(&(queueList->qsmphore));
		queueList->totalQueues--;
		SEMA_POST(&(queueList->qsmphore));
	}
	else
	{
		if(ret_code == QUE_ER_PURGED)
			ret_code = QUE_ER_DESTROYED;
	}
	return ret_code;
}
int queueLock(int qid)
{
	QUEUE *que = NULL;
	int ret_code = QUE_ER_BADQID;
	if(qid > -1)
	{

		que = &(queueList->userQueues[qid]);
		SEMA_WAIT(&(que->wsmphore));
		ret_code = 0;
	}
	return ret_code;
}
int queueUnLock(int qid)
{
	QUEUE *que = NULL;
	int ret_code = QUE_ER_BADQID;
	if(qid > -1)
	{
		que = &(queueList->userQueues[qid]);
		SEMA_POST(&(que->wsmphore));
		ret_code = 0;
	}
	return ret_code;
}

int queuePurge(int qid)
{
	QUEUE *que = NULL;
	struct NODE *node = NULL;
	struct NODE *tmp = NULL;
	int ret_code = -1;
	int i = 0;
	if(qid > queueList->maxQueueId)
		return QUE_ER_BADQID;

	que = &(queueList->userQueues[qid]);
	if(que->queueId < 0)
		return QUE_ER_NOTFOUND;
	ret_code = queuePurgeMessages(que);
	return ret_code;

}

int queuePurgeMessages(QUEUE *que)
{
	struct NODE *node = NULL;
	struct NODE *tmp = NULL;
	int ret_code = -1;
	int i = 0;

	//if(strlen(que->queueName) <= 0)

	SEMA_WAIT(&(que->wsmphore));
	ret_code = 0;
	if(ret_code == 0)
	{
		node = que->head;
		for(i = 0;(i<que->totalMessages) && (node != que->tail);i++)
		{
			node->data = (void *)( (char*)sharedMemBaseAddr + ((unsigned long)(node->data) ));
			myfree(node->data);
			memset(&(node->MH),0,sizeof(MSGHDR));
			node->data = NULL;
			node->prev = NULL;
			tmp = node;
			node = node->next;
			myfree(tmp);
		}
		if(node != NULL)
		{
			node->data = (void*) ( ( (unsigned long)(node->data)) + (char*)sharedMemBaseAddr );
			myfree(node->data);
			memset(&(node->MH),0,sizeof(MSGHDR));
			node->data = NULL;
			node->next = NULL;
			node->prev = NULL;
			myfree(node);
			node = NULL;
		}
		SEMA_WAIT(&(queueList->qsmphore));
		queueList->messageCount = queueList->messageCount - que->totalMessages;
		SEMA_POST(&(queueList->qsmphore));
		if(queueList->messageCount < 0)
		{
			printf("MEssage count below zero\n");
		}
		que->isBeingUsed = 0;
		que->totalMessages = 0;
		que->totalDataBytes = 0;
		que->totalAllowedMsgs = 0;
		que->totalAllowedData = 0;
		que->head = NULL;
		que->tail = NULL;
		SEMA_POST(&(que->wsmphore));
		
	}
	else
	{
		return QUE_ER_PURGED;
	}
	return QUE_SUCCESS;

}


int isQueueNameExists(char *quename)
{
	int i = 0;
	int id = queueList->maxQueueId;
	int foundEmptySlot = 0;
	
	for(i=0;i<=queueList->maxQueueId+1;i++)
	{
		if(queueList->userQueues[i].queueName[0] == '\0')
		{
			if(foundEmptySlot == 0)
			{
				id = i;
				foundEmptySlot = 1;
			}
		}
		else if(strcmp(queueList->userQueues[i].queueName, quename) == 0)
		{
			id = i;
			break;
		}
	}
	return id;
	//return FALSE;
}

// pass address of the head pointer, with data pointer of arbitrary size.
// datum should be allocated by the calling module.

QUEUE* createQue(char *quename, int uid)
{
	QUEUE *que = NULL;
	int ret_code =0;
	int id = -1;
	id = isQueueNameExists(quename);
	if(id > queueList->maxQueueId)
	{
		SEMA_WAIT(&(queueList->qsmphore));
		queueList->maxQueueId = id;
		SEMA_POST(&(queueList->qsmphore));
	}
	
	if(queueList->userQueues[id].queueName[0] == '\0')
	{
		hash_insert(quename,strhash(quename), id);
		que = &queueList->userQueues[id];
		que->poolPtr = NULL;
		if(SEMA_INIT(&(que->wsmphore),0,USYNC_PROCESS,NULL) != 0)
		{
			printf("createQue: semainit() failed\n");
		}
		else
		{
			que->queueId = id;
			que->createdBy = uid;
			SEMA_WAIT(&(queueList->qsmphore));
			queueList->totalQueues++;
			SEMA_POST(&(queueList->qsmphore));
			que->head = NULL;
			que->tail = NULL;
			que->totalMessages = 0;
			SEMA_POST(&(que->wsmphore));
			strcpy(que->queueName,quename);
		}
	}
	return que;
}

int addNode(QUEUE *currentQue,void *datum,int datalen, int priority, int userid) {
    struct NODE *ptr = NULL, *q = NULL;
	int ret_code = 0;
    if (currentQue->head == NULL) {

        // First node
        (currentQue->head) = (struct NODE*) mymalloc(sizeof (struct NODE));
        if ((currentQue->head) == NULL) { // Allocation failed
            return -1;
        }
        (currentQue->head)->data = datum; // Validating datum is caller's responsibility.

		currentQue->head->MH.Priority = priority;
		currentQue->head->MH.GetQid = currentQue->queueId;
		currentQue->head->MH.Size = datalen;
		currentQue->head->MH.Uid = userid;
		
		currentQue->tail = currentQue->head;
        (currentQue->head)->next = (currentQue->tail); // Circular
        (currentQue->head)->prev = (currentQue->tail);
        currentQue->tail->next = currentQue->head;
		currentQue->tail->prev = currentQue->head;
		currentQue->totalMessages++;
		SEMA_WAIT(&(queueList->qsmphore));
		queueList->messageCount++;
		SEMA_POST(&(queueList->qsmphore));
			if(currentQue->poolPtr != NULL)
				SEMA_POST(&(currentQue->poolPtr->tsmphore));
        return 0;
    }

	ptr = (struct NODE*)  mymalloc(sizeof (struct NODE));
	if (ptr == NULL) { // Allocation failed
	    return -1;
	}
	ptr->data = datum;
	ptr->MH.Priority = priority;
	ptr->MH.GetQid = currentQue->queueId;
	ptr->MH.Size = datalen;
	ptr->MH.Uid = userid;
    if ((currentQue->tail)->MH.Priority >= priority)
	{
		//Current priority is lesser so Make Ptr as tail
		ptr->next = currentQue->tail->next;
		ptr->prev = (currentQue->tail);
		currentQue->tail->next->prev = ptr;
		currentQue->tail->next = ptr;
		currentQue->tail = ptr;
		currentQue->totalMessages++;
		SEMA_WAIT(&(queueList->qsmphore));
		queueList->messageCount++;
		SEMA_POST(&(queueList->qsmphore));

		if(currentQue->totalMessages == 1)
			if(currentQue->poolPtr != NULL)
				SEMA_POST(&(currentQue->poolPtr->tsmphore));
    }
	else
	{
        q = currentQue->tail;
		//From tail, Find the position where the priority will fit.
        while (q != NULL && q->MH.Priority < priority && q != currentQue->head)
		{
            /*find the position of element */
            q = q->prev;
        }

		if(q == currentQue->head && q->MH.Priority < priority)
		{
			ptr->prev = currentQue->head->prev;
			ptr->next = currentQue->head;
			currentQue->head->prev->next = ptr;
			currentQue->head->prev = ptr;
			currentQue->head = ptr;
			currentQue->totalMessages++;
			SEMA_WAIT(&(queueList->qsmphore));
			queueList->messageCount++;
			SEMA_POST(&(queueList->qsmphore));

			if(currentQue->totalMessages == 1)
				if(currentQue->poolPtr != NULL)
				SEMA_POST(&(currentQue->poolPtr->tsmphore));
		}
		else
		{
	        /*insert new element */
			ptr->next = q->next;
			ptr->prev = q;
			q->next->prev = ptr;
			q->next = ptr;
			currentQue->totalMessages++;
			SEMA_WAIT(&(queueList->qsmphore));
			queueList->messageCount++;
			SEMA_POST(&(queueList->qsmphore));

			if(currentQue->totalMessages == 1)
				if(currentQue->poolPtr != NULL)
				SEMA_POST(&(currentQue->poolPtr->tsmphore));
		}
    }
    return 0;
}

int putNode(QUEUE *currentQue, struct NODE *node)
{
	int ret_code = 0;
    if(currentQue == NULL)
    {
        //printf("putNode() Queue is NULL\n");
        return -1;
    }
    if(node == NULL)
    {
        //printf("putNode() node is NULL\n");
        return -2;
    }

    if (currentQue->head == NULL)
    {
        // First node
        (currentQue->head) = node;

        currentQue->tail = currentQue->head;
        (currentQue->head)->next = (currentQue->tail); // Circular
        (currentQue->head)->prev = (currentQue->tail);
        currentQue->tail->next = currentQue->head;
        currentQue->tail->prev = currentQue->head;
        currentQue->totalMessages++;

        if(currentQue->totalMessages == 1)
            if(currentQue->poolPtr != NULL)
				SEMA_POST(&(currentQue->poolPtr->tsmphore));
        return 0;
    }

    node->next = currentQue->tail->next;
    node->prev = (currentQue->tail);
    currentQue->tail->next->prev = node;
    currentQue->tail->next = node;
    currentQue->tail = node;
    currentQue->totalMessages++;
    if(currentQue->totalMessages == 1)
        if(currentQue->poolPtr != NULL)
				SEMA_POST(&(currentQue->poolPtr->tsmphore));

    return 0;
}

void initialize()
{
	int ret_code = 0;
	SEMA_WAIT(&(queueList->qsmphore));
	queueList->totalQueues = 0;
	SEMA_POST(&(queueList->qsmphore));
}


void *getNodeData(QUEUE *currentQue, int *datalen)
{
	int ret_code = 0;
    struct NODE *tmp;
    void *dataum = NULL;
    if (currentQue->head == NULL)
    {
        //printf("Queue Underflow\n");
    }
    else
	{
        tmp = currentQue->head;
		currentQue->totalMessages--;
		SEMA_WAIT(&(queueList->qsmphore));
		queueList->messageCount--;
		SEMA_POST(&(queueList->qsmphore));
		if(queueList->messageCount < 0 || currentQue->totalMessages < 0) 
		{
			printf("MEssage count below zero\n");
		}
		dataum = tmp->data;
		*datalen = tmp->MH.Size;
		if(currentQue->totalMessages <= 0 && currentQue->head != NULL)
		{
			currentQue->totalMessages = 0;
			myfree(currentQue->head);
			currentQue->head = NULL;
		}
		else
		{
			currentQue->head->prev->next = currentQue->head->next;
			currentQue->head->next->prev = currentQue->head->prev;
			currentQue->head = currentQue->head->next;
			myfree(tmp);
			tmp = NULL;
		}
		
    }
	
    return dataum;
}/*End of getNodeData()*/

struct NODE *getNode(QUEUE *currentQue)
{
	struct NODE *node = NULL;

	if (currentQue->head != NULL)   // Queue is not empty
	{
		node = currentQue->head;
		currentQue->totalMessages--;

		if(currentQue->totalMessages <= 0 && currentQue->head != NULL)
		{
			currentQue->totalMessages = 0;
			currentQue->head = NULL;
		}
		else
		{
			currentQue->head->prev->next = currentQue->head->next;
			currentQue->head->next->prev = currentQue->head->prev;
			currentQue->head = currentQue->head->next;
		}
	}

//	if(node == NULL && (currentQue->head != NULL || currentQue->totalMessages > 0))
//	{
//		printf("\n\nMismatch\n\n");
//	}

	return node;
}

void changeQueueById(QUEUE *currentQue,int id)
{
	if(id > -1 && id <= queueList->maxQueueId)
	{
		currentQue = &queueList->userQueues[id];
	}
	else
	{
		printf("changeQueueById(): There is no queue as per the given QId = %d\n", id);
	}
}

QUEUE * getQueueById(int id)
{
	QUEUE *currentQue = NULL;
	if(id < 0 || id > 1000) 
	{
		printf("getQueueById(): %d",id);
		return NULL;
	}
	
		currentQue = &queueList->userQueues[id];
	if(currentQue == NULL || currentQue->queueName[0] == '\0')
	{
		currentQue = NULL;
		printf("getQueueById(): There is no queue as per the given QId = %d\n", id);
	}
	return currentQue;
}


int getQueueIdByName(char *qname)
{
	int i = 0;
	int id = -1;
	if(qname != NULL)
	{
		if(hash_find(qname,&id) >= 0)
		{
			return id;
		}
	}
	return (id);
}



