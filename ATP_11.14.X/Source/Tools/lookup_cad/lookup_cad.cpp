// lookup_cad.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <ctype.h>
bool isnum(char* in_str)
{
   int i;
   int length;
   length = (int)strlen(in_str);
   for (i = 0; i < length; i++)
   {
      if (!isdigit(in_str[i]))
        return(false);
   }
   return(true);
}
int main(int argc, char* argv[])
{
	//printf("Hello World!\n");
	char readMsg[251]={0};
	char card_num[23]="",auth_nbr[7]="",mID[12]="";
	FILE *fPtrCad=NULL,*CardPtr=NULL;


	fPtrCad=fopen("R03MERCHBATCH_20150701_12184321.txt","r");
   // fwrite(Name,sizeof(char)*20,1,file);
    CardPtr=fopen("Lookup-R03MERCHBATCH_20150701_12184321.txt","w");

	if(fPtrCad != NULL)
	{
		while(fgets(readMsg, 150, fPtrCad) != NULL)
		{

		int len = strlen(readMsg);
		if(len > 0 )
		{
			strncpy(mID,&readMsg[20],10);
			if(strlen(mID) == 10 && isnum(mID)== true)
			{strncpy(card_num,&readMsg[33],20);
			strncpy(auth_nbr,&readMsg[74],6);
			fprintf(CardPtr,"%s %s\n",card_num,auth_nbr);
			}
		}

		memset(readMsg,0 ,sizeof(readMsg));

		}
	
	}


	fclose(fPtrCad);
	fclose(CardPtr);


//	FILE *file=fopen("NameofFile","rb");
//	fread(Name,sizeof(char)*20,1,file);
// all reading

 //  fclose(file);

// 



	return 0;
}

