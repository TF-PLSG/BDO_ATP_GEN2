/* 
 * File:   queue.h
 * Author: epic
 *
 * Created on November 12, 2010, 3:34 PM
 */

#ifndef _QUEUE_H
#define	_QUEUE_H

#include "xcom.h"

#ifdef	__cplusplus
extern "C" {
#endif

	int isQueueNameExists(char *quename);
	QUEUE* createQue(char *quename, int uid);
	int addNode(QUEUE *currentQue,void *datum, int datalen, int priority, int userid);
	int putNode(QUEUE *currentQue, struct NODE *node);
	void *getNodeData(QUEUE *currentQue, int *datalen);
	struct NODE *getNode(QUEUE *currentQue);
	void changeQueueById(QUEUE *currentQue,int id);
	int getQueueIdByName(char *qname);
	QUEUE * getQueueById(int id);
	int queuePurge(int qid);
	int queueDestroy(int qid);
	int queuePurgeMessages(QUEUE *que);

#ifdef	__cplusplus
}
#endif

#endif	/* _QUEUE_H */

