/******************************************************************************
  
   Module: dialog.c
   
   Title: dialog manager
  
   Description: 

   $Log:   N:\PVCS\PTE\Tools\Flow_File_Generator\HostGen\dbs_sett.c  $
   
      Rev 1.0   Dec 30 1999 13:14:34   dirby
   Initial release for Host Generator.
   
      Rev 1.0   Nov 30 1998 11:39:40   MSALEH
   Dbs_settlement trafgen
   works with dbs_sett.flw flow file
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
//#include <sys/contract/process.h>

#include "pte.h"
#include "ptetime.h"
#include "convert.h"
#include "ptecom.h"
#include "ntutils.h"
#include "trafsim.h"

/* Extern definition section */
extern int  volatile EndProcessSignalled;
extern int  volatile MainProcessDone;
extern CHAR          ServiceName[12];

INT          connection_type;

PRIVATE CHAR         reply_que[9] = "";
PRIVATE CHAR         term_id[9];
PRIVATE LONG         numeric_term_id;
PRIVATE CHAR         max_terminals[6];
PRIVATE LONG         numeric_max_terminals;
PRIVATE CHAR         sleep_time[6];
PRIVATE LONG         numeric_sleep_time;
PRIVATE CHAR         Flow_File_Name[32];

PRIVATE WORD         primary_tcp_port_number, secondary_tcp_port_number;
PRIVATE CHAR         primary_tcp_host_name[21], secondary_tcp_host_name[21];
BOOL                 volatile use_primary = true;

PRIVATE LOCAL_DM_CONFIG     dm_config;

CHAR                MSGFLW_FILE_PATH[256];
MSGFLOW                     msgflow;

PRIVATE BYTE outgoing_buf[MAX_OUTGOING_BUFFER_SIZE];
PRIVATE BYTE incoming_buf[MAX_OUTGOING_BUFFER_SIZE];
CHAR    Version[]               = "ATP_11.1.0";

/*************************************************************
 *************************************************************/
void MainProcessor()
{};

/*************************************************************
 *************************************************************/






/*****************************************************************/
/*****************************************************************/
void read_flow_file()
{
   INT  iMsg, len;
   CHAR filename[256];

   // Populate msgflow structure with the message flow from the .flw file
   memset (&msgflow, 0, sizeof(msgflow));

   sprintf(filename, "%s%s", MSGFLW_FILE_PATH, Flow_File_Name);
   msgflow.fp = fopen(filename,"r");

   if (!msgflow.fp)
   {
      printf("Failed to open message file.\n");
	  getchar();
      exit(0);
   }

   for (iMsg = 0; iMsg < NTFLOW_MAXMSG; iMsg++)
   {
      if (feof(msgflow.fp))
         break;

      fgets(msgflow.vMsg[iMsg], NTFLOW_MAXMSGLEN, msgflow.fp);
      len = strlen(msgflow.vMsg[iMsg]);
      
      if (len == 0)
         break;

      /* remove the newline from the line */
      len--;
      msgflow.vMsg[iMsg][len] = 0;
   }

   msgflow.nMsg = iMsg;
}

/*************************************************************
 *************************************************************/
INT connection_is_up()
{
  return(connection_up);
}


/*************************************************************
 *************************************************************/
void SetStartupEnvironment()
{
   DWORD rc;
   CHAR sectionname  [] = "HOSTGEN";
   CHAR tmpfilename  [61];
   CHAR filename     [256];
   CHAR tmpstr       [256];
   
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%shostsim.ini", tmpstr);
  
   rc = GetPrivateProfileString(
       sectionname, 	           // points to section name 
      "SERVICE_NAME",        // points to key name 
      "ncvisaA",	              // points to default string 
      tmpfilename,              // points to destination buffer 
      sizeof(tmpfilename) - 1,  // size of destination buffer 
      filename                  // points to initialization filename 
   );

   strcpy(reply_que , tmpfilename);
   

}   /* SetStartupEnvironment() */


/*************************************************************
 *************************************************************/
void EndProcess()
{
/*
  Sleep(3000);
*/
  sleep(3);
  LogEvent("Ascendent: Host Traffic Generator is shutting down", INFO_MSG);
}

extern CHAR           ExeName     [MAX_APP_NAME_SIZE];

/*************************************************************
 *************************************************************/
void main(int argc, char *argv[])
{
   INT        MsgLen;
   BOOL       failure_occured = false;
   BYTE       repeat;
   CHAR       AppName [8];
   CHAR       tmpstr  [256];
   CHAR     XipcInstanceName[12];
   pPTE_MSG       p_msg_out= NULL_PTR;
   LONG ret_code;

  LogEvent("Ascendent: Host simulator starts", INFO_MSG);
  strcpy( ExeName, argv[0] );

  GetAppName( AppName );
  GetXipcInstanceName (XipcInstanceName);

   SetStartupEnvironment();

   if (pteipc_init_single_instance_app (AppName, XipcInstanceName) == false)
   {     
      printf("Failed to create XIPC queues");
      pteipc_shutdown_single_instance_app();
      exit(0);
   }


  repeat = 'Y';

  while ((repeat == 'Y') || (repeat == 'y'))
  {
  printf("Enter Flow File Name [ex dbs_sett.flw]:");
  gets(Flow_File_Name);printf("\n");
   if(strlen(Flow_File_Name) == 0)
      break;
   read_flow_file();
	MsgLen    = strlen(msgflow.vMsg[0]) / 2;
	ascii_to_bin ((pBYTE)&msgflow.vMsg[0], outgoing_buf, MsgLen * 2);


	
   p_msg_out = ptemsg_build_msg( MT_INCOMING,
                                 0,
                                 0,
                                 reply_que,
                                 AppName,
                                 (pBYTE)outgoing_buf,
                                 MsgLen,
                                 0);
	
    if(p_msg_out == NULL_PTR)
    {
         printf("Insufficient Memory to build message");
         break;
    }
	 ret_code = pteipc_send(p_msg_out, reply_que);
	 if(ret_code < 0)
 	 {
	    pteipc_get_errormsg(ret_code, tmpstr);
	    printf("failed transmit to : %s. %s", reply_que, tmpstr);
       break;
  	 }

	 free(p_msg_out);
  }

  printf("\n\nPress Any Key to End Simulation");
  getchar();
  EndProcess();
  MainProcessDone = 1;
  pteipc_shutdown_single_instance_app();
  exit(0);
}
