#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUCKETS 5000
#define MAX_NODES 100
#define MAX_NAME_LENGTH 25
struct HASHNODE 
{
	char name[25];
	int id;
};

struct BUCKET
{
	int count;
	struct HASHNODE nodes[MAX_NODES];
};

struct BUCKET QueueBucket[MAX_BUCKETS+1];

void hashdisplay();
int hash_find (const char * qname, int *id);
int hash_find_bucket (const char * qname, int *id, int *nodeno, int *bucketno);
int hash_remove (const char * qname);
int hash_insert(const char *qname, int hash,int id);
int strhash(const char *str);
