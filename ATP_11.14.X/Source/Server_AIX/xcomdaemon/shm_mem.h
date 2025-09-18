/* 
 * File:   queue.h
 * Author: epic
 *
 * Created on November 12, 2010, 3:34 PM
 */

#ifndef _SHM_MEM_H
#define	_SHM_MEM_H

#include "xcom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef	__cplusplus
extern "C" {
#endif

	int isTableNameExists(char *tblname);
	int createTable(char * tblname, int tblSize, struct SHM_TABLE ** tbl);
	int getMemIdByName(char *tblname);
	void changeTableById(struct SHM_TABLE **tbl,int id);
	//int createSection(int mid, int offset, int size, SECTION *section);
	int sectionDef(SECTION section);
	int MemoryWrite(struct SHM_TABLE *currentTable, void **dataum, int offset, int datalen);
	int MemoryRead(struct SHM_TABLE *currentTable, void **dataum, int offset, int datalen);
	int MemoryLock(struct SHM_TABLE *currentTable, int offset, int datalen);
	int MemoryUnLock(struct SHM_TABLE *currentTable, int offset, int datalen);
	struct SHM_TABLE * getTableByName(char *tblname);
	int deleteTable(int mid);
	void * tableAlloc(int nSize);

#ifdef	__cplusplus
}
#endif
#endif /* End of shm_mem.h*/
