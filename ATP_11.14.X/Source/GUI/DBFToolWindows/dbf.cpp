#include "stdafx.h"
#include "dbf.h"
#include <string.h>
static char	global_err_str[100];
static int	global_err_code;
extern "C"
{
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "ntutils.h"

#include "app_info.h"
#include "equitdb.h"
#include "txutils.h"
#include "memmnger.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "dbcommon.h"
#include "timings.h"
	#include "ptedes.h"

}

#include "Winsock2.h"


BYTE	des_key[200];
BYTE	cur_key[200];
BYTE	recs[4];

CHAR			version_msb = 11;
CHAR			version_lsb = 8;
BOOL		bCheckVersion = TRUE;
CHAR			Version[] = "ATP_11.8.4";	// should not be more than 32 chars

void strTrim(char *str, char trimChar);
void strTrimLeft(char *str, char trimChar);
void strTrimRight(char *str, char trimChar);
void strTrim(char *str, char trimChar)
{
	CString sz = str;
	sz.TrimLeft(trimChar);
	sz.TrimRight(trimChar);
	strcpy(str, sz);
}


void strTrimLeft(char *str, char trimChar)
{
	CString sz = str;
	sz.TrimLeft(trimChar);
	strcpy(str, sz);
}


void strTrimRight(char *str, char trimChar)
{
	CString sz = str;
	sz.TrimRight(trimChar);
	strcpy(str, sz);
}

long get_first_block_size()
{
	long size = 0;
	WORKING_DBF_TLF01 w_dbf_tlf01;
	memset(&w_dbf_tlf01, 0, sizeof(WORKING_DBF_TLF01));
	size = sizeof(w_dbf_tlf01.transaction_id) + 
		sizeof(w_dbf_tlf01.entry_type) +
		sizeof(w_dbf_tlf01.acquirer_id) +
		sizeof(w_dbf_tlf01.terminal_id) +
		sizeof(w_dbf_tlf01.response_text) +
		sizeof(w_dbf_tlf01.card_num_len);
	return size;
}

long txt_get_first_block_size()
{
	long size = 0;
	WORKING_DBF_TLF01 w_dbf_tlf01;
	memset(&w_dbf_tlf01, 0, sizeof(WORKING_DBF_TLF01));
	size = sizeof(w_dbf_tlf01.transaction_id) + 1 +
		sizeof(w_dbf_tlf01.entry_type) + 1 +
		sizeof(w_dbf_tlf01.acquirer_id) + 1 +
		sizeof(w_dbf_tlf01.terminal_id) + 1 +
		sizeof(w_dbf_tlf01.response_text) + 1 +
		sizeof(w_dbf_tlf01.card_num_len);
	return size;
}

#define CARD_NUM_OFFSET 122
int txt_decrypt_file(char *filename, char *cur_key1)
{
	
	WORKING_DBF_TLF01 w_dbf_tlf01;
	
	char filler[1024] = {0};
	BYTE header[10000] = {0};
	int bRet = 0;
	unsigned short dkey[9] = {0};
	char line[5120] = {0};
	CString szLine;
	char tmpFile[128] = {0};
	FILE *tmpPtr =NULL;
	FILE *fptr = NULL;
	long count = 0;

	memset(filler, ' ', 1023);
		
	if(filename == NULL)
		return -1;
	if(cur_key1 == NULL)
		return -2;
	if( strlen((char *)cur_key1) == 0)
		return -3;
	
	//cur_key = htons(cur_key1);
	strcpy((char *)dkey,cur_key1);
	
	for(int i = 0;i<8;i++)
	{
		dkey[i] = htons(dkey[i]);
	}
	sprintf(tmpFile,"%s.tmp",filename);
	
	fptr = fopen(filename,"rb");
	if(fptr == NULL)
		return -1;
	tmpPtr = fopen(tmpFile,"wb");
	if(tmpPtr == NULL)
		return -1;
	

	
	if(fptr != NULL)
	{
		//Last line will not definitely more than 100. This is will go to second last line. 
		//The total text in the last will is 75 chars. The total records number is the remaining 25 chars which in real time will not happen.
		fseek(fptr, -100, SEEK_END);
		memset(line, 0, sizeof(line));
		if(fgets(line, 5000, fptr) != NULL)
		{
			if(fgets(line, 5000, fptr) != NULL)
			{
				if(line[0] == '[')
				{
					if(line[2] == '#')
					{
						//already decrypted.
						return ALEADY_DECRYPTED;
					}
					szLine = line;
					int pos = szLine.Find('|', 0);
					int pos2 = szLine.Find('|', pos + 9);
					CString szVersion = szLine.Mid(pos +9, pos2 - (pos +9));
					szVersion.TrimRight();
					szVersion.TrimLeft();
					if ( (bCheckVersion) && szVersion.CompareNoCase(Version) != 0 )
					{
						bRet = VERSION_INCOMPATIBLE;
						return bRet;
					}
				}
			}
		}

		fseek(fptr, 0, SEEK_SET);
		rewind(fptr);
		printf("Decrypting the records......\n");
		
		//Get header line
		memset(line, 0, sizeof(line));
		if(fgets(line, 5000, fptr) != NULL)
		{
			fputs(line, tmpPtr);
			//File not empty.
			memset(line, 0, sizeof(line));
			while(fgets(line, 5000, fptr) != NULL)
			{
				if(line[0] == '[')
				{
					//its footer
					line[2] = '#';
					fputs(line, tmpPtr);
					memset(line, 0, sizeof(line));
				}
				else
				{
					//Get the encrypted card number
					bRet = 1;
					count++;
					memset(&w_dbf_tlf01, 0, sizeof(w_dbf_tlf01));
					memcpy(w_dbf_tlf01.card_num, line + CARD_NUM_OFFSET, sizeof(w_dbf_tlf01.card_num) - 1);
					
					strTrimRight(w_dbf_tlf01.card_num, ' ');
					
					des_decryption((pBYTE)w_dbf_tlf01.card_num, strlen(w_dbf_tlf01.card_num), dkey, 1);
					
					int len = strlen(w_dbf_tlf01.card_num);
					memcpy(w_dbf_tlf01.card_num + len, filler, sizeof(w_dbf_tlf01.card_num) - len );
					
					memcpy(line + CARD_NUM_OFFSET, w_dbf_tlf01.card_num, sizeof(w_dbf_tlf01.card_num));
					fputs(line, tmpPtr);
					memset(line, 0, sizeof(line));
				}
			}
		}
		
		fclose(tmpPtr);
		fclose(fptr);
	}
	
	
	if(bRet == 1)
	{
		
		tmpPtr =fopen(tmpFile,"rb");
		if(tmpPtr == NULL)
			return -1;
		fptr = fopen(filename,"wb");
		if(fptr == NULL)
			return -1;
		while(!feof(tmpPtr))
		{
			memset(header, 0, sizeof(header));
			int readBytes = fread(header, 1, 10000, tmpPtr);
			fwrite(header, 1, readBytes, fptr);
		}
		fclose(fptr);
		fclose(tmpPtr);
		remove(tmpFile);
		printf("%ld records decrypted.\n",count);
		
	}
	
	return bRet;
}


int dbf_decrypt_file(char *filename, char *cur_key1)
{
	
	int i;
	WORKING_DBF_TLF01 w_dbf_tlf01;
	WORKING_DBF_TLF01_ARCHIVE w_dbf_tlf01_archive;
	BYTE header[10000] = {0};
	int bRet = 0;
	unsigned short dkey[9] = {0};

	

	if(filename == NULL)
		return -1;
	if(cur_key1 == NULL)
		return -2;
	if( strlen((char *)cur_key1) == 0)
		return -3;
	
	//cur_key = htons(cur_key1);
	strcpy((char *)dkey,cur_key1);
	
	for(int i = 0;i<8;i++)
	{
		dkey[i] = htons(dkey[i]);
	}
	char tmpFile[128] = {0};
	sprintf(tmpFile,"%s.tmp",filename);
	FILE *tmpPtr =NULL;
	
	FILE *fptr = fopen(filename,"rb");
	if(fptr == NULL)
		return -1;
	int card_size = sizeof(w_dbf_tlf01.card_num);
	long block_1 = get_first_block_size();
	long block_2 = sizeof(WORKING_DBF_TLF01) - block_1 - card_size;
	long count = 0;
	if(fptr != NULL)
	{
		rewind(fptr);
		printf("Decrypting the records......\n");
		memset(&w_dbf_tlf01, 0, sizeof(WORKING_DBF_TLF01));
		memset(&w_dbf_tlf01_archive, 0, sizeof(WORKING_DBF_TLF01_ARCHIVE));
		fread(header,1,MAX_HEADER_SIZE + MAX_FIELDS_SIZE +1, fptr);
		if ( (bCheckVersion == TRUE) && ((header[13] != version_msb) || (header[14] != version_lsb)) )
		{
			bRet = VERSION_INCOMPATIBLE;
		}
		else
		{
			if(header[12] == 0x00)
			{
				if(header[MAX_HEADER_SIZE + 848] == 10)
				{
					tmpPtr = fopen(tmpFile,"wb");
					if(tmpPtr != NULL)
					{
						header[12] = 0x11;
						fwrite(header,1,MAX_HEADER_SIZE + MAX_FIELDS_SIZE +1, tmpPtr);
						while(!feof(fptr))
						{
							bRet = 1;
							fread(&w_dbf_tlf01_archive, 1, sizeof(WORKING_DBF_TLF01_ARCHIVE), fptr);
							for(int i=0;i<sizeof(w_dbf_tlf01_archive.card_num);i++)
							{
								if(w_dbf_tlf01_archive.card_num[i] == 0x20)
									w_dbf_tlf01_archive.card_num[i] = 0x00;
							}
							printf("%s\n",w_dbf_tlf01_archive.card_num);
							des_decryption((pBYTE)w_dbf_tlf01_archive.card_num,strlen(w_dbf_tlf01_archive.card_num),dkey,1);
							for(i=0;i<sizeof(w_dbf_tlf01_archive.card_num);i++)
							{
								if(w_dbf_tlf01_archive.card_num[i] == 0x00)
									w_dbf_tlf01_archive.card_num[i] = 0x20;
							}
							fwrite(&w_dbf_tlf01_archive, 1, sizeof(WORKING_DBF_TLF01_ARCHIVE), tmpPtr);
							memset(&w_dbf_tlf01_archive, 0, sizeof(WORKING_DBF_TLF01_ARCHIVE));
							count++;
						}
						if(bRet != 1)
						{
							remove(tmpFile);
						}
						fclose(tmpPtr);
					}
				}
				else
				{
					tmpPtr = fopen(tmpFile,"wb");
					if(tmpPtr != NULL)
					{
						header[12] = 0x11;
						fwrite(header,1,MAX_HEADER_SIZE + MAX_FIELDS_SIZE +1, tmpPtr);
						while(!feof(fptr))
						{
							bRet = 1;
							fread(&w_dbf_tlf01, 1, sizeof(WORKING_DBF_TLF01), fptr);
							for(int i=0;i<sizeof(w_dbf_tlf01.card_num);i++)
							{
								if(w_dbf_tlf01.card_num[i] == 0x20)
									w_dbf_tlf01.card_num[i] = 0x00;
							}
							printf("%s\n",w_dbf_tlf01.card_num);
							des_decryption((pBYTE)w_dbf_tlf01.card_num,strlen(w_dbf_tlf01.card_num),dkey,1);
							for(i=0;i<sizeof(w_dbf_tlf01.card_num);i++)
							{
								if(w_dbf_tlf01.card_num[i] == 0x00)
									w_dbf_tlf01.card_num[i] = 0x20;
							}
							fwrite(&w_dbf_tlf01, 1, sizeof(WORKING_DBF_TLF01), tmpPtr);
							memset(&w_dbf_tlf01, 0, sizeof(WORKING_DBF_TLF01));
							count++;
						}
						if(bRet != 1)
						{
							remove(tmpFile);
						}
						fclose(tmpPtr);
					}
				}
			}
			else
			{
				printf("The file '%s' is already decrypted.\n",filename);
				bRet = 0;
			}
		}
		fclose(fptr);
	}


	if(bRet == 1)
	{

		tmpPtr =fopen(tmpFile,"rb");
		if(tmpPtr == NULL)
				return -1;
		fptr = fopen(filename,"wb");
		if(fptr == NULL)
				return -1;
		while(!feof(tmpPtr))
		{
			fread(header,10000,1,tmpPtr);
			fwrite(header,10000,1,fptr);
		}
		fclose(fptr);
		fclose(tmpPtr);
		remove(tmpFile);
		printf("%ld records decrypted.\n",count);

	}

		return bRet;
}
