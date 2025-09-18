#include "hash.h"

int err_code;

int size = MAX_BUCKETS;
int strhash(const char *str) 
{
	unsigned long hash = 0;
	int i=0;
	while(i <= strlen(str)) 
	{
		hash = hash + (hash<<5) + (unsigned long)(str[i]); //DJB2 hash algorithm: hash = hash*33+str[i] and note: hash<<5 is same as hash*(2 to the power of 5) which means hash *32, add hash to this value to have hash*33
		i++;
	}
	//hash += strlen(str);
	return (hash%size); //return the hashed number which must be between 0 an size-1, this is why I used hash%size
}

int hash_insert(const char *qname, int hash,int id)
{
	if(hash > MAX_BUCKETS)
	{
		return -1;
	}

	if(QueueBucket[hash].count >= MAX_NODES)
	{
		return -2;
	}
	int i = 0;
	for( i=0; i<MAX_NODES; i++)
	{
		if(QueueBucket[hash].nodes[i].id == -1)
		{
			QueueBucket[hash].nodes[i].id = id;
			strcpy(QueueBucket[hash].nodes[i].name,qname);
			QueueBucket[hash].count++;
			break;
		}
	}
	return 0;
	
}

int hash_find (const char * qname, int *id)
{
	if(qname == NULL)
	{
		return -1;
	}
	int hash = strhash(qname);
	int i = 0;
	*id = -1;

	if(hash > MAX_BUCKETS)
	{
		return -2;
	}
	if(QueueBucket[hash].count <= 0)
	{
		return -3;
	}
	i = 0;
	while(i < MAX_NODES)
	{
		if(strcmp(QueueBucket[hash].nodes[i].name, qname) == 0)
		{
			*id = QueueBucket[hash].nodes[i].id;
			break;
		}
		i++;
	}
	
	return i+1;
}
int hash_find_bucket (const char * qname, int *id, int *nodeno, int *bucketno)
{
	if(qname == NULL)
	{
		return -1;
	}
	int hash = strhash(qname);
	int i = 0;
	*id = -1;

	if(hash > MAX_BUCKETS)
	{
		return -2;
	}
	if(QueueBucket[hash].count <= 0)
	{
		return -3;
	}
	i = 0;
	while(i < MAX_NODES)
	{
		if(strcmp(QueueBucket[hash].nodes[i].name, qname) == 0)
		{
			*id = QueueBucket[hash].nodes[i].id;
			*nodeno = i+1;
			*bucketno = hash;
			break;
		}
		i++;
	}
	
	return i+1;
}

int hash_init()
{
	int i=0,j=0;
	for(i=0;i<MAX_BUCKETS;i++)
	{
		for(j=0;j<MAX_NODES;j++)
		{
			memset(QueueBucket[i].nodes[j].name,0,MAX_NAME_LENGTH);
			QueueBucket[i].nodes[j].id = -1;
		}
	}
}
int hash_remove(const char * qname)
{
	if(qname == NULL || strlen(qname) ==0)
	{
		return -1;
	}
	int hash = strhash(qname);
	int i = 0;
	
	if(hash > MAX_BUCKETS)
	{
		return -2;
	}
	if(QueueBucket[hash].count <= 0)
	{
		return -3;
	}
	while(i < MAX_NODES)
	{
		if(strcmp(QueueBucket[hash].nodes[i].name, qname) == 0)
		{
			memset(QueueBucket[hash].nodes[i].name,0,MAX_NAME_LENGTH);
			QueueBucket[hash].nodes[i].id = -1;
			QueueBucket[hash].count--;
			break;
		}
		i++;
	}
	
	return i;
}


void hashdisplay()
{
	int total = 0;
	int i=0;
	int j=0;
	for(i=0;i<MAX_BUCKETS+1;i++)
	{
		printf("QueueBucket[%d].count = %d\n",i,QueueBucket[i].count);
		for(j=0;j<QueueBucket[i].count;j++)
		{
			printf("%25s \t\t %03d\n",QueueBucket[i].nodes[j].name,QueueBucket[i].nodes[j].id);
		}
		total += QueueBucket[i].count;
	}
	printf("Total = %d\n",total);
}

int dummymain(int argc, char* argv[])
{
	char names[300][25];
	char input[25];
	char AQ[25] = {0};
	char IQ[25] = {0};
	char CQ[25] = {0};
	FILE *fp = NULL;
	int i =0;
	int qid = 0;
	for (i=0;i<MAX_BUCKETS;i++)
	{
		memset(QueueBucket,0,sizeof(struct BUCKET));
	}

	fp = fopen("ques.txt","r");
	i=0;
	while(fp != NULL)
	{
		memset(names[i],0,25);
		fgets(names[i],25,fp);
		int n = strlen(names[i]);
		names[i][n-1] = '\0';
		if(strlen(names[i]) == 0 || names[i][0] == 10)
		{
			break;
		}
		sprintf(AQ,"%sA",names[i]);
		sprintf(CQ,"%sC",names[i]);
		sprintf(IQ,"%sI",names[i]);
		//puts(names[i]);

		hash_insert(AQ,strhash(AQ),qid++);
		hash_insert(IQ,strhash(IQ),qid++);
		hash_insert(CQ,strhash(CQ),qid++);

	//	printf("%s\t\t\t%d\n",AQ,strhash(AQ));
	//	printf("%s\t\t\t%d\n",IQ,strhash(IQ));
	//	printf("%s\t\t\t%d\n",CQ,strhash(CQ));
		

		
		i++;
	}
	
	//display();
	int ex = 0;
	while(ex == 0)
	{
		printf("\n\nEnter a name to find: ");
		scanf("%s",input);
		int id = -1;
		int it = hash_find(input,&id);
		if(it > 0)
		{
			printf("%s\t\t%d\n",input,id);
		}
		else
		{
			printf("Not found\n");
		}
		printf("No of iterations = %d\n",it);
		printf("Enter zero to continue: ");
		scanf("%d",&ex);
		
	}

	printf("Total size = %d\n",sizeof(QueueBucket));
	return 0;
}

