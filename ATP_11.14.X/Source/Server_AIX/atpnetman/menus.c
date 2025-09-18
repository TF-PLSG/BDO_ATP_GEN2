/*****************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ATPNETMAN MENUS
 *  
 * TITLE:       ATP NETWORK MANAGER - INTERACTIVE MENUS
 *  
 * DESCRIPTION: This file contains functions that displays menus to interact
 *              with the user to gather input information.  This information
 *              includes Network ID and Type, Sign ON or OFF or Echo command,
 *              and logon bin number, which is optional.
 *
 * APPLICATION: ASCENDENT - REALPAY
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\atpnetman\menus.c  $

	  Rev 1.3   March 30 2009 17:04:10  Girija Y TF
   Added code to send Sign ON and OFF request to host.
   
      Rev 1.2   Nov 05 2004 17:45:52   dirby
   Corrected bug when looking to see if a network has any bins to
   be displayed.  The bin menu would display whether there were
   bins or not.
   
      Rev 1.1   Nov 05 2004 17:22:26   dirby
   Added global variable Dest_Queue to hold the XIPC queue name
   of the network Id selected from the menus.
   
      Rev 1.0   Nov 04 2004 15:22:02   dirby
   Initial revision.
 *
 *****************************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ntutils.h"

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "constants.h"
#include "prototypes.h"


/*+---------------------+
  | GLOBAL DECLARATIONS |
  +---------------------+*/

/* Program Flow Control */
extern INT   Exit;        /* Set to true or false to control program flow. */
extern INT   Menus;       /* Set to true of false to display menus or not. */
extern INT   XipcInstance;

/* Input Parameters */
extern INT   Network_ID_Count;
extern CHAR  Network_CMD[];
extern CHAR  Network_Id[];
extern CHAR  Network_Type[];
extern CHAR  Bin[];
extern CHAR Network_Queue[];

/* Host States */
extern CHAR  Current_State[];
extern CHAR  Intended_State[];

/* QUEUES */
extern CHAR  netds_que_name[];
//extern CHAR  Dest_Queue[];

/* Network Controller Records */
extern NCF01_GUI_LIST  Ncf01_List;
extern NCF01_REC       Network_Recs[MAX_NUM_NCF01_RECS];

/* Application Name */
extern CHAR  AppName[];

extern  LOCAL_NCF01_GUI_LIST local_ncf01;    //Girija GB
/******************************************************************************
 *
 *  NAME:         SHOW_MENUS
 *
 *  DESCRIPTION:  This function is the main function dealing with interactive
 *                menus.  It calls the necessary functions to display them
 *                and collect the inputs.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      Exit - (Global) Set to 'true' when user wants to exit
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void show_menus()
{
   display_netid_menu();
   if ( Exit == false )
   {
      display_netcmd_menu();
      if ( Exit == false )
      {
         display_nettype_menu();
         if ( Network_Type[0] == 'S' )
         {
            /* This means user wants to send command now without type or bin. */
            Network_Type[0] = 'I'; /* Set default value */
         }
         else if ( Exit == false )
         {
            display_netbin_menu();
         }
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         DISPLAY_NETID_MENU
 *
 *  DESCRIPTION:  This function will display the Network ID menu.
 *
 *  INPUTS:       Ncf01_List - (Global) List of Network Ids
 *
 *  OUTPUTS:      Exit       - (Global) Set to 'true' when user wants to exit
 *                Network_Id - (Global) Network Id selected from menu list
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void display_netid_menu()
{
   INT   i,j;
   INT   user_input;
   INT   entry = INVALID;
   INT   row_cnt;
   INT   factor = EVEN;
   CHAR  instr[80] = "";
    CHAR queue[2]="A";
   if ( Network_ID_Count == 0 )
   {
      printf("\nNo records obtained from NCF01.\n\n" );
      Exit = true;
   }
   else
   {
      display_main_header();

      while( entry == INVALID )
      {
         /* Display Network ID Header. */
         printf("   NETWORK IDs\n" );
         printf("   -----------\n" );

         if ( Network_ID_Count < 6 )
         {
            /* Display one column. */
            /* ------------------- */
            for( i=0; i<Network_ID_Count; i++ )
            {
               printf("   %d. %s\n", i+1, Network_Recs[i].network_id );
            }
         }
         else if ( Network_ID_Count < 13 )
         {
            /* Display two columns. */
            /* -------------------- */
            row_cnt = Network_ID_Count / 2;
            if ( (Network_ID_Count%2) == 1 )
               factor = ODD;

            for( i=0; i<row_cnt; i++ )
            {
               printf("   %d. %s\t\t%2d. %s\n",
                      i+1,
                      Network_Recs[i].network_id,
                      i+1+row_cnt+factor,
                      Network_Recs[i+row_cnt+factor].network_id );
            }

            if ( factor == ODD )
               printf("   %d. %s\n",
                      row_cnt+1,
                      Network_Recs[row_cnt].network_id );
         }
         else
         {
            /* Display three columns. */
            /* ---------------------- */
            row_cnt = Network_ID_Count / 3;
            if ( (Network_ID_Count%3) != 0 )
               factor = ODD;

            for( i=0; i<row_cnt; i++ )
            {
               /* Make sure 3rd column is not empty.
                * 'j' is the item number of third column.
                */
               j = i+1+(row_cnt+factor)*2;
               if ( j <= Network_ID_Count )
               {
                  printf("   %2d. %s\t\t%2d. %s\t\t%2d. %s\n",
                         i+1,
                         Network_Recs[i].network_id,
                         i+1+row_cnt+factor,
                         Network_Recs[i+row_cnt+factor].network_id,
                         i+1+(row_cnt+factor)*2,
                         Network_Recs[i+(row_cnt+factor)*2].network_id );
               }
               else
               {
                  printf("   %2d. %s\t\t%2d. %s\n",
                         i+1,
                         Network_Recs[i].network_id,
                         i+1+row_cnt+factor,
                         Network_Recs[i+row_cnt+factor].network_id);
               }
            }

            if ( factor == ODD )
            {
               printf("   %2d. %s\t\t%2d. %s\n",
                       row_cnt+1,
                       Network_Recs[row_cnt].network_id,
                      (row_cnt+1)*2,
                       Network_Recs[(row_cnt+1)*2-1].network_id );
            }
         }

         /* Prompt for user's selection. */
         printf("\nEnter a number to choose a Network ID (or 'e' to exit): ");
         scanf( "%s", instr );

         if ( (instr[0] == 'e') || (instr[0] == 'E') )
         {
            Exit  = true;
            entry = VALID;
         }
         else
         {
            user_input = atoi(instr);
            if ( (user_input <= 0)               ||
                 (user_input > Network_ID_Count) ||
                 (strlen(instr) > 2)             ||
                 (false == isnum(instr) )         )
            {
               printf("\n\nInvalid entry '%s'. Please try again.\n\n", instr );
            }
            else
            {
               /* Put selected Network Id into Global variable. */
               entry = VALID;
               strcpy( Network_Id,
                       Network_Recs[user_input-1].network_id );
			  // strcpy(Network_Queue,Network_Id);
			  // strcpy( Dest_Queue_atpnetman, Network_Queue );
			   //strcat(Dest_Queue_atpnetman,queue);
            }
         }
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         DISPLAY_NETCMD_MENU
 *
 *  DESCRIPTION:  This function will display the Network Management Commands.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      Exit        - (Global) Set to 'true' when user wants to exit
 *                Network_CMD - (Global) Network Id selected from menu list
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void display_netcmd_menu()
{
   INT   user_input;
   INT   entry = INVALID;
   CHAR  instr[80] = "";

   display_main_header();

   while( entry == INVALID )
   {
      /* Display Network Command Header. */
      printf("   NETWORK MANAGEMENT COMMANDS\n" );
      printf("   ---------------------------\n" );

     // printf("   1. Sign On\n   2. Sign Off\n   3. Echo Test\n   4. Sign On Advice Retrieval\n   5. Sign Off Advice Retrieval\n" );
		 printf("   1. Sign On\n   2. Sign Off\n   3. Echo Test\n " );
      /* Prompt for user's selection. */
      printf("\nEnter a number to choose a Network CMD (or 'e' to exit): ");
      scanf( "%s", instr );

      if ( (instr[0] == 'e') || (instr[0] == 'E') )
      {
         Exit  = true;
         entry = VALID;
      }
      else
      {
         user_input = atoi(instr);
         if ( (user_input <= 0)      ||
              (user_input > 5)       ||
              (strlen(instr) > 1)    ||
              (false == isnum(instr) ) )
         {
            printf("\n\nInvalid entry '%s'. Please try again.\n\n", instr );
         }
         else
         {
            /* Put selected Network CMD into Global variable. */
            entry = VALID;
            if ( user_input == 1 )
               strcpy( Network_CMD, LOGON );
            else if ( user_input == 2 )
               strcpy( Network_CMD, LOGOFF );
            else if ( user_input == 3 )
               strcpy( Network_CMD, ECHO );
			else if ( user_input == 4 )
				strcpy( Network_CMD, SIGNON_ADVICE_RETRIEVAL);
			else
				strcpy( Network_CMD, SIGNOFF_ADVICE_RETRIEVAL);
         }
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         DISPLAY_NETTYPE_MENU
 *
 *  DESCRIPTION:  This function will display the Network Type menu. This
 *                gives the option to choose Acquiring or Issuing network.
 *
 *  INPUTS:       Network_Recs - (Global) Network records for our application
 *                Network_Id   - (Global) Network ID selected from the menu
 *
 *  OUTPUTS:      Exit         - (Global) Set to 'true' when user wants to exit
 *                Network_Recs - (Global) Network records for our application
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void display_nettype_menu()
{
   INT   i;
   INT   net_type_cnt = 1;
   INT   entry = INVALID;
   CHAR  user_input;
   CHAR  instr[80] = "";

   /* Determine if Network ID has 2 types. */
   for( i=0; i<Network_ID_Count; i++ )
   {
      if ( 0 == strcmp(Network_Recs[i].network_id, Network_Id) )
      {
         if ( Network_Recs[i].network_type[0] == 'B' )
         {
            /* Network handles Issuing and Acquiring. */
            net_type_cnt = 2;
         }
         break;
      }
   }

   /* No need for this menu if only one type. */
   if ( net_type_cnt == 1 )
   {
      Network_Type[0] = Network_Recs[i].network_type[0];
   }
   else
   {
      display_main_header();

      while( entry == INVALID )
      {
         /* Display Network ID Header. */
         printf("   NETWORK TYPEs\n" );
         printf("   -------------\n" );

         printf("   A = Acquirer\n" );
         printf("   I = Issuer\n" );
         printf("   S = Send Sign On/Off/Echo to host\n\n" );

         printf("   Selecting S will ignore the optional\n" );
         printf("   entries of Network Type and Logon Bin.\n\n" );

         /* Prompt for user's selection. */
         printf("\nEnter type (or 'S' to send or 'e' to exit): ");
         scanf( "%s", instr );

         if ( (instr[0] == 'e') || (instr[0] == 'E') )
         {
            Exit  = true;
            entry = VALID;
         }
         else
         {
            if ( strlen(instr) > 1 )
            {
               printf("\n\nInvalid entry '%s'. Please try again.\n\n", instr );
            }
            else
            {
               user_input = toupper(instr[0]);
               if ( (user_input != 'A') &&
                    (user_input != 'I') &&
                    (user_input != 'S')  )
               {
                  printf("\n\nInvalid entry '%s'. Please try again.\n\n", instr );
               }
               else
               {
                  /* Put selected Network Type into Global variable. */
                  entry = VALID;
                  Network_Type[0] = user_input;
               }
            }
         }
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         DISPLAY_NETBIN_MENU
 *
 *  DESCRIPTION:  This function will display the Network Bin menu.  These
 *                are the various accounts that can be logged on to at the
 *                host.
 *
 *  INPUTS:       Network_Recs - (Global) Array of network records
 *
 *  OUTPUTS:      Exit - (Global) Set to 'true' when user wants to exit
 *                Bin  - (Global) Logon bin selected from menu list
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void display_netbin_menu()
{
   INT   i,j;
   INT   user_input;
   INT   entry = INVALID;
   INT   bin_cnt;
   INT   row_cnt;
   INT   bins_exist = false;
   INT   factor = EVEN;
   CHAR  instr[80] = "";

   /* Determine if Network ID has any logon accounts. */
   for( i=0; i<Network_ID_Count; i++ )
   {
      if ( 0 == strcmp(Network_Recs[i].network_id, Network_Id) )
      {
         if ( Network_Recs[i].bin[0][0] != 0x00 )
         {
            /* This network does have logon accounts. */
            bins_exist = true;
         }
         break;
      }
   }

   if ( bins_exist == true )
   {
      display_main_header();

      while( entry == INVALID )
      {
         /* Get number of logon bins. */
         for( j=0,bin_cnt=0; j<10; j++ )
         {
            if ( Network_Recs[i].bin[j][0] != 0x00 )
               bin_cnt++;
         }

         /* Display Network ID Header. */
         printf("   LOGON BINs\n" );
         printf("   ----------\n" );

         if ( bin_cnt < 6 )
         {
            /* Display one column. */
            /* ------------------- */
            for( j=0; j<bin_cnt; j++ )
            {
               printf("   %d. %s\n", j+1, Network_Recs[i].bin[j]);
            }
         }
         else
         {
            /* Display two columns. */
            /* -------------------- */
            row_cnt = bin_cnt / 2;
            if ( (bin_cnt%2) == 1 )
               factor = ODD;

            for( j=0; j<row_cnt; j++ )
            {
               printf("   %d. %s\t\t%2d. %s\n",
                      j+1,
                      Network_Recs[i].bin[j],
                      j+1+row_cnt+factor,
                      Network_Recs[i].bin[j+row_cnt+factor] );
            }

            if ( factor == ODD )
               printf("   %d. %s\n",
                      row_cnt+1,
                      Network_Recs[i].bin[row_cnt] );
         }

         /* Add the 'S' option to the menu. */
         printf("\n   S = Send Sign On/Off/Echo to host\n\n");

         printf("   Select 'S' to send command without a bin.\n");
         printf("   Select menu number to send command with a bin.\n");

         /* Prompt for user's selection. */
         printf("\nEnter Bin number (or 'S' to send or 'e' to exit): ");
         scanf( "%s", instr );

         if ( (instr[0] == 'e') || (instr[0] == 'E') )
         {
            Exit  = true;
            entry = VALID;
         }
         else
         {
            if ( (instr[0] == 's' || instr[0] == 'S') && (strlen(instr) == 1))
            {
               entry = VALID;
            }
            else
            {
               user_input = atoi(instr);
               if ( (user_input <= 0)      ||
                    (user_input > bin_cnt) ||
                    (strlen(instr) > 2)    ||
                    (false == isnum(instr) ) )
               {
                  printf("\n\nInvalid entry '%s'. Please try again.\n\n", instr );
               }
               else
               {
                  /* Put selected Network Id into Global variable. */
                  entry = VALID;
                  strcpy( Bin, Network_Recs[i].bin[user_input-1] );
               }
            }
         }
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         DISPLAY_MAIN_HEADER
 *
 *  DESCRIPTION:  This function will clear the window and display a static
 *                screen containing the application name.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
void display_main_header()
{
   INT   i;
   INT   len;
   CHAR  buffer[100] = "";

   /* Clear window for Windows or Unix (non-windows) OS. */
   #ifdef WIN32
      system( "cls" );
   #else
      system( "clear" );
   #endif

   sprintf(buffer, "%s", AppName );
   len = strlen( buffer );
   if (len < sizeof(buffer)-5 )
   {
      /* Top Line */
      printf("   ");
      len += 4;
      for( i=0; i<len; i++ )
      {
         printf("*");
      }

      /* Application Name */
      printf("\n   * %s *\n", AppName );

      /* Bottom Line */
      printf("   ");
      for( i=0; i<len; i++ )
      {
         printf("*");
      }
      printf("\n\n");
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         isnum
 *
 *  DESCRIPTION:  This function checks for a string to be all numbers.
 *
 *  INPUTS:       in_str - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if string is all numbers, else false 
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT isnum(pCHAR in_str)
{
   INT i;
   INT length;

   length = (INT)strlen(in_str);

   for (i = 0; i < length; i++)
   {
      if (!isdigit(in_str[i]))
        return(false);
   }
   return(true);
}

