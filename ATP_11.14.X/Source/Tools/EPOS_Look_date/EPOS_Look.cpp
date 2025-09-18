// EPOS_Look.cpp : Defines the entry point for the console application.
/*
TF- Phani - 06th JULY 2015

This tool created to create NON processed EPOS file due to invalid date from orginal EPOS file.
- Original EPOS file has invalid date as 1406xx, So this tool shall pick all records with that date
AN write into "nonprocessed.txt".  

- Input is : Orignal EPOS file [EPOS created by ATP with invalid dates]

- Outpu is : Nonprocesses.txt [This file contains all non processed records or tx in rejected reports]


*/

#include "stdafx.h"

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

static unsigned long int totalprocessedamount = 0;
static unsigned  long int totalnonprocessedamount =0;
static unsigned long totalprocessedcount = 0;
static unsigned long totalnonprocessedcount = 0;
static unsigned int Flag =1;

char recordline1[300]={0};
char recordline2[300]={0};
char recordline3[30] ={0};
char header[2300]={0};


unsigned char Amount_EPOS[13]={0}; // 12 byte amount
unsigned char Version[]="ATP_11.1.0";
char Cardnumber_EPOS[17]={0}; // 16 byte card number
char Authnumber_EPOS[7]={0}; // 6 byte Auth Number

char Cardnumber_lookup[17]={0}; //16 byte card number
char Authnumber_Lookup[7]={0}; // 6 byte auth number
unsigned char trailer[1300]={0};
FILE *fpE;
//FILE *fpL;
//FILE *fpP;
FILE *fpNP;

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

void ReadHeaderEPOS()
{
	fgets(header,sizeof(header) -1,fpE);

}
void WriteHeader()
{
	char temp[3]="";
	strcpy(temp,"44");
	//fwrite(header,strlen(header),sizeof(char),fpP);
	memcpy(&header[76],temp,2 );
	fwrite(header,strlen(header),sizeof(char),fpNP);
}
int main(int argc, char* argv[])
{
	//	printf("Hello World!\n");

	int LookupLinesCount = 0;
	int LookupEPOSLines  = 0;
	char EposFile[50]={0};
	char lookupFile[50]={0};
	char segment[3]={0};
	char Txdate[8]={0};
	char line[1024]={0};
	int line_len;
	unsigned int segment10_count=0;
	unsigned int NpWritenLines=0;
	//totalnonprocessedamount =0;
	strcpy(EposFile,argv[1]);

		printf(" This tool created to create NON processed EPOS file due to invalid date from orginal EPOS file.\n" );
		printf("	- Original EPOS file has invalid date as 1406xx, So this tool shall pick all records with that date \n"  );
		printf("AND write into 'nonprocessed.txt' \n" );
		printf("dates to chek is hard coded - 1406** , 1405** ,  \n" );
		printf("- Input is : Orignal EPOS file [EPOS created by ATP with invalid dates. Ex: 'EPOS_credit_20150701_6.upl' \n" );
		printf("- Outpu is : Nonprocesses.txt [This file contains all non processed records or tx in rejected reports] \n" );
	
	/* Open the efpos record file*/
	fpE = fopen(argv[1],"r+"); 
	if (fpE == NULL)
	{
		printf("EPOS File open failed\n");
		exit(1);
	}

	/* Open Non_Duplicate.txt Output file */
	fpNP = fopen("Non_Processed.txt","w"); 
	if (fpNP == NULL)
	{
		printf("EPOS Lookup File open failed\n");
		exit(1);
	}


	LookupEPOSLines  = countline(fpE);

	//fseek(fpL, 0, SEEK_SET);
	fseek(fpE, 0, SEEK_SET);
	printf("EPOSLines = %d \n",LookupEPOSLines);

	ReadHeaderEPOS();
	WriteHeader();


	while(fgets(line, 1023, fpE) != NULL)
	{
		memset(Amount_EPOS,0x00,sizeof(Amount_EPOS));
		line_len = strlen(line);

		if(line_len > 52)
		{
			segment[0] = line[51];
			segment[1] = line[52];
			segment[2] = '\0';

		}


		// get the date 
		strncpy(Txdate, &line[39],6);

		if(strncmp(Txdate,"1406",4) == 0  || strncmp(Txdate,"1405",4) == 0 )
		{

			if(strcmp(segment, "10") == 0)
			{
				segment10_count= segment10_count+1 ;
				strncpy( (char *)Amount_EPOS,line+AMOUNT_POSITION, AMOUNT_LENGTH);
				totalnonprocessedamount=totalnonprocessedamount +atoi((char *)Amount_EPOS);
			}



			if(line[40] == '4')
				line[40] = '5';

			fwrite(line,strlen(line),sizeof(char),fpNP);
			NpWritenLines= NpWritenLines+1;



		}


	}



	// Create trailer

	memset(trailer, 0, sizeof(trailer));
	sprintf((char *)trailer, "%170s"," ");
	memset(trailer, 0, sizeof(trailer));
	//sprintf(trailer, "%1s%09d%016llu%1s%016s%016d%1s%015llu%170s",

	/*sprintf(trailer, "%1s%09d%016I64u%1s%016s%016d%1s%015I64u%170s",
	"9",         
	NpWritenLines,
	" " ,
	" ",
	"0",
	" ",
	"+",
	" ",         
	" ");*/
	sprintf((char *)trailer, "9%09d%016u %016s%016d+%015u%170s",
		//"9",         
		NpWritenLines,
		// " ",
		totalnonprocessedamount,       
		"0",
		// " ",
		segment10_count,
		//	 " ",
		totalnonprocessedamount,         
		" ");

	fwrite(trailer,strlen(header) -1,sizeof(char),fpNP);
	printf("\n"); printf("\n"); 
	printf("Dedicted Invalid date records: %d !!! \n",segment10_count);
	printf("\n"); 
	// fclose(fpL);
	fclose(fpE);
	//fclose(fpP);
	fclose(fpNP);
	 printf("EPOS date check tool completed !!! \n");
	 printf("Please check 'nonprocessed.txt' file for all non processed transactions..All the best.. \n");

	return 0;
}

