#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 

#define CARD_NUMBER_POSITION 20
#define AUTH_NUMBER_OFFSET 120
#define CARD_NUMBER_LENGTH 16
#define AMEX_CARD_NUMBER_LENGTH 15
#define AUTH_NUMBER_POSITION 120
#define AUTH_NUMBER_LENGTH 6
#define AMOUNT_POSITION 59
#define AMOUNT_LENGTH 12
#define CARD_MONEY_OFFSET  23
#define MONEY_AUTH_OFFSET 49
#define LAST_6_DIGITS_RRN 114

static unsigned long long int totalprocessedamount = 0;
static unsigned long long int totalnonprocessedamount =0;
static unsigned long totalprocessedcount = 0;
static unsigned long totalnonprocessedcount = 0;
static unsigned int Flag =1;

char recordline1[300]={0};
char recordline2[300]={0};
char recordline3[30] ={0};
char header[300]={0};
char trailer[300]={0};

unsigned char Amount_EPOS[13]={0}; // 12 byte amount
unsigned char Version[]="ATP_11.1.0";
char Cardnumber_EPOS[17]={0}; // 16 byte card number
char Authnumber_EPOS[7]={0}; // 6 byte Auth Number

char Cardnumber_lookup[17]={0}; //16 byte card number
char Authnumber_Lookup[7]={0}; // 6 byte auth number

FILE *fpE;
FILE *fpL;
FILE *fpP;
FILE *fpNP;

/*
This fucnton comapre Card number and Auth number from Lookup file and epos file.
if comparision is successful then return 0 else 1
*/
int compare()
{
	if (strncmp(Cardnumber_lookup,Cardnumber_EPOS,strlen(Cardnumber_lookup))== 0)
	{
		if((strncmp(Authnumber_Lookup,Authnumber_EPOS,strlen(Authnumber_Lookup))== 0))
		{
			Flag =0;
			return 0;
		}
		else
		{
			Flag =1;
			return 1 ;
		}
	}
	else
	{
		Flag =1;
		return 1;
	}
}

/*
This function will crate the trailer record for  Processed  transaction
*/
void CreateTrailersProcessed()
{
   int Seg10count=0;
   int Segcount=0;
   Segcount=totalprocessedcount -1; //Total number except headerand trailer
   Seg10count=Segcount/2;
   memset(trailer, 0, sizeof(trailer));
   sprintf(trailer, "%1s%09d%016llu%1s%016s%016d%1s%015llu%170s",
		"9",         
	     Seg10count,
		 totalprocessedamount,
		 " ",
		 "0",
		 Seg10count,
		 "+",
		 totalprocessedamount,         
		 " ");
   fwrite(trailer,strlen(header)-1,sizeof(char),fpP); 
}
/*
This function will crate the trailer record for Non_Duplicate ( Non_Processed ) transaction
*/
void CreateTrailersNonProcessed()
{
   int Seg10count=0;
   int Segcount=0;
   Segcount=totalnonprocessedcount -1; //Total number except headerand trailer
   Seg10count=Segcount/2;
   memset(trailer, 0, sizeof(trailer));
   sprintf(trailer, "%1s%09d%016llu%1s%016s%016d%1s%015llu%170s",
	     "9",         
		 Seg10count,
		 totalnonprocessedamount,
		 " ",
		 "0",
		 Seg10count,
		 "+",
		 totalnonprocessedamount,         
         " ");
     fwrite(trailer,strlen(header) -1,sizeof(char),fpNP);
}

/*
This function will write the header in Processed.txt and Non_Processed.txt
*/
void WriteHeader()
{
	fwrite(header,strlen(header),sizeof(char),fpP);
	fwrite(header,strlen(header),sizeof(char),fpNP);
}


/*
This function write seg 10 and seg 11 for a record which is already processed in Processed.txt file
*/
void UpdateProcessedFile ()
{
	totalprocessedamount=totalprocessedamount + atoi(Amount_EPOS);
	totalprocessedcount = totalprocessedcount +1;
	fwrite(recordline1,strlen(recordline1),sizeof(char),fpP);
	fwrite(recordline2,strlen(recordline2),sizeof(char),fpP);
	memset(recordline1, 0,sizeof(recordline1));
	memset(recordline2, 0,sizeof(recordline2));
}

/*
This function write seg 10 and seg 11 for a record which is not processed in Non_Processed.txt file
*/
void UpdateNonProcessedFile ()
{
	totalnonprocessedamount=totalnonprocessedamount +atoi(Amount_EPOS);
	totalnonprocessedcount = totalnonprocessedcount +1;
	fwrite(recordline1,strlen(recordline1),sizeof(char),fpNP);
	fwrite(recordline2,strlen(recordline2),sizeof(char),fpNP);
	memset(recordline1, 0,sizeof(recordline1));
	memset(recordline2, 0,sizeof(recordline2));
}

/*
This function finds the Card Number and Auth number from Lookup file
*/
void FindCardNumnerAuthnumberLKP()
{
	memset(Cardnumber_lookup,0,sizeof(Cardnumber_lookup));
	memset(Authnumber_Lookup,0,sizeof(Authnumber_Lookup));
	if ((strncmp(recordline3,"34",2)==0)||(strncmp(recordline3,"37",2)==0))
	{
		strncpy(Cardnumber_lookup,recordline3,AMEX_CARD_NUMBER_LENGTH);
	}
	else
	{
		strncpy(Cardnumber_lookup,recordline3,CARD_NUMBER_LENGTH);
	}
	strncpy(Authnumber_Lookup,recordline3+strlen(Cardnumber_lookup),AUTH_NUMBER_LENGTH); 
}

/*
This function finds the Card Number and Auth number from EPOS file
*/
void FindCardNumnerAuthnumberAmount_EPOS()
{
	memset(Cardnumber_EPOS,0,sizeof(Cardnumber_EPOS));
	memset(Amount_EPOS,0,sizeof(Amount_EPOS));
	memset(Authnumber_EPOS,0,sizeof(Authnumber_EPOS));
	if ((strncmp(recordline1+CARD_NUMBER_POSITION,"34",2)==0)||(strncmp(recordline1+CARD_NUMBER_POSITION,"37",2)==0))
	{
		strncpy(Cardnumber_EPOS,recordline1+CARD_NUMBER_POSITION,AMEX_CARD_NUMBER_LENGTH);
	}
	else
	{
		strncpy(Cardnumber_EPOS,recordline1+CARD_NUMBER_POSITION,CARD_NUMBER_LENGTH);
	}
	strncpy( Amount_EPOS,recordline1+AMOUNT_POSITION, AMOUNT_LENGTH);
	//strncpy(Authnumber_EPOS,recordline1+AUTH_NUMBER_OFFSET,AUTH_NUMBER_LENGTH); 
	strncpy(Authnumber_EPOS,recordline1+LAST_6_DIGITS_RRN,AUTH_NUMBER_LENGTH); 
} 

/*
Read Seg10 from EPOS file
*/
int ReadSeg10EPOS()
{
	int len = 0;
	if(fgets(recordline1,sizeof(recordline1)- 1,fpE)!=NULL)
	{
		len = strlen(recordline1);
		recordline1[len-1] = '\n';
		FindCardNumnerAuthnumberAmount_EPOS();
	}
}

/*
Read Seg11 from EPOS file
*/
int ReadSeg11EPOS()
{
	int len = 0;
	fgets(recordline2,sizeof(recordline2)-1,fpE);
	len = strlen(recordline2);
	recordline2[len-1] = '\n';
}


int ReadLineLookup()
{
	int len = 0;
	if(	fgets(recordline3,sizeof(recordline3)-1,fpL)!=NULL)
	{
		len = strlen(recordline3);
		recordline3[len-1] = '\n';
		FindCardNumnerAuthnumberLKP();
	}
}

/*
This function read the header from EPOS file
*/
void ReadHeaderEPOS()
{
	fgets(header,sizeof(header) -1,fpE);
	
}

int countline(FILE *fp)
{
	int c;
	int count =0;
	do
	{
	  c = fgetc (fp);
      if (c == '\n')
	  {
		  count++;
	  }
	}
	while (c != EOF);
	return count ;
}

int main(int argc, char *argv[])
{

  int LookupLinesCount = 0;
  int LookupEPOSLines  = 0;

  if(argc<3)
  {
       printf("MerchBatch(EFTPOS) requires Two command line parameters to work correctly:			\n");
       printf("(1) EPOS_credit File name.															\n");
       printf("(2) LookupFile.																		\n");
   	   printf("This two input files should be present in the director where this tool is installed. \n");   
   	   printf("                           						                                    \n");
	   printf("This tool create two file											                \n");
       printf("1)Processed.txt														                \n");
       printf("2)Non_Processed.txt												                    \n");
	   printf("Processed.txt file contains transactions which are already processed by Cadencie.	\n");
	   printf("Non_Processed.txt file contains transactions which are not processed by Cadencie.	\n");
	   printf("                           						                                    \n");
	   printf("       eftpos EPOS_credit_20141016_3.upl LookupFile.tx								\n");
	   printf("Output is placed in the current subdirectory.										\n");
	   printf("																						\n");
	   printf("Credit/Debit EFTPOS Version: %s\n\n", Version);
       exit(1);
  }

 /* Open the efpos record file*/
  fpE = fopen("EPOS_credit_20141016_3.upl","r+"); 
  if (fpE == NULL)
  {
	  printf("EPOS File open failed\n");
	  exit(1);
  }

  /* Open Lookup File*/
  fpL = fopen("LookupFile.txt","r"); 
  if (fpL == NULL)
  {
	  printf("EPOS Lookup File open failed\n");
	  exit(1);
  }

  /* Open Duplicate.txt file */
  fpP = fopen("Processed.txt","w"); 
  if (fpP == NULL)
  {
	  printf("EPOS Lookup File open failed\n");
	  exit(1);
  }
 
 /* Open Non_Duplicate.txt Output file */
  fpNP = fopen("Non_Processed.txt","w"); 
  if (fpNP == NULL)
  {
	  printf("EPOS Lookup File open failed\n");
	  exit(1);
  }

  LookupLinesCount = countline(fpL);
  LookupEPOSLines  = countline(fpE);
 
  fseek(fpL, 0, SEEK_SET);
  fseek(fpE, 0, SEEK_SET);
  printf("LookupLinesCount %d LookupEPOSLines = %d \n",LookupLinesCount,LookupEPOSLines);
  
  int temp_len= 0;
  ReadHeaderEPOS();
  WriteHeader();

  /* Total count  conatains header and trailer*/
  /*Loop should run till last seg 10 */
  while (LookupEPOSLines >= 3)
  {
	    temp_len = LookupLinesCount;
		ReadSeg10EPOS();
		ReadSeg11EPOS();

		while(temp_len != 0)
	    {
			ReadLineLookup();
			if(!compare())
			{
				UpdateProcessedFile();
				fseek(fpL, 0, SEEK_SET);
				temp_len = 0;
			}
			else if (temp_len !=0)
			{ 
				temp_len = temp_len -1;
			}
	    }
		if (Flag == 1)
		{
			UpdateNonProcessedFile();
			fseek(fpL, 0, SEEK_SET);
		}
	LookupEPOSLines = LookupEPOSLines -2;
	Flag= 1;
   }

   CreateTrailersProcessed();
   CreateTrailersNonProcessed();
   fclose(fpL);
   fclose(fpE);
   fclose(fpP);
   fclose(fpNP);
   return 0;
}
