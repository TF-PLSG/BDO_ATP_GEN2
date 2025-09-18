/*******************************************************************************
* Copyright (c) 2000, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:       echo_timer.h
*  
* TITLE:       
*  
* DESCRIPTION: This module contains all the function prototypes for echo_timer.c
*
* APPLICATION: 
*
* AUTHOR:       D. Durga
*
* REVISION HISTORY:
*
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptetime.h"
#include "ptetimer.h"
#include "txutils.h"
#include "ntutils.h"
#include "echo_timer.h"
#include "memmnger.h"



/*
 *BYTE set_echo_timer(char* echo_key,WORD timer_length, pPTE_MSG p_msg_set_timer,char* reply_q, BYTE flag )
 *     echo_key        : sent by the network handler
 *     timer_length    : sent by the network handler
 *     p_msg_set_timer : gets populated with the set timer request message to timerds
 *     reply_q         : application_que
 *     flag            : 1 - sends the request to timerds to set echo timer
 *                       0 - returns to the calling function with the message.
 *Returs: true/flase on success/failure
 */
BYTE set_echo_timer(char* echo_key,WORD timer_length, pPTE_MSG *p_msg_set_timer,char* reply_q, BYTE flag )
{
LONG ret_code;
char Buffer[256],ErrorMsg[256];
pPTE_MSG temp_msg=NULL_PTR;

         if( timer_length == 0) return true;
         //*p_msg_set_timer = ptetimer_bld_start_timer_req(echo_key,NULL,NULL,"timerdsA",reply_q,timer_length,0,NULL_PTR,0); 
         temp_msg = ptetimer_bld_start_timer_req(echo_key,NULL,NULL,"timerdsA",reply_q,timer_length,0,NULL_PTR,0); 
         if ( NULL_PTR == temp_msg )
		 {
            TxUtils_Send_Msg_To_Operator( 1,
                           "Insufficient Memory to build start timer message",
                            1, WARN_MSG, "set_echo_timer", 3,
                            WARNING_ERROR," ",
                            NULL_PTR, NULL_PTR);
            return(false);
	     }
         
         if( flag == 0)
         {
             *p_msg_set_timer=temp_msg;
             return true;
         }
         else if(flag ==1)
         {
             ret_code = pteipc_send(temp_msg,"timerdsA");
             free( temp_msg );
             if( ret_code < 0 )
		     {
                pteipc_get_errormsg( ret_code, ErrorMsg );
                sprintf( Buffer, "Error on pteipc_send to que timerdsA: %s",
                          ErrorMsg );
                TxUtils_Send_Msg_To_Operator( 1, Buffer, 1, ERROR_MSG, 
                                             "set_echo_timer", 4, FATAL_ERROR,
                                              " ",
                                              NULL_PTR, NULL_PTR);
                
                return(false);
		     }
             return true;
         }
         else return false;
}


/*
 *BYTE clear_echo_timer(char* echo_key,pPTE_MSG pte_clr_timer,BYTE flag)
 *     echo_key        : sent by the network handler
 *     p_msg_clr_timer : gets populated with the clear timer request message to timerds
 *     reply_q         : application_que
 *     flag            : 1 - sends the request to timerds to clear echo timer
 *                       0 - returns to the calling function with the message.
 *Returs: true/flase on success/failure
 */

BYTE clear_echo_timer(char* echo_key,pPTE_MSG *pte_clr_timer,char* reply_q,BYTE flag)
{
LONG ret_code;
char Buffer[256],ErrorMsg[256];
pPTE_MSG temp_msg=NULL_PTR;

       /* build PTEMSG to clear Timer in the Data Server */ 
   temp_msg = ptetimer_bld_clear_timer_req( echo_key,"timerdsA",reply_q);
   if ( NULL_PTR == temp_msg )
   { 
      TxUtils_Send_Msg_To_Operator(1,
                      "Insufficient Memory to build clear timer message",
                       1, WARN_MSG, "incoming_host_message_timer_reply",
                       3, WARNING_ERROR, " ",
                       NULL_PTR, NULL_PTR);
      return(false);
   }
     if( flag == 0)
     {
         *pte_clr_timer=temp_msg;
         return true;
     }
   else if(flag ==1)
   {
       /* get Timer message from Host Data Server now  */
       ret_code = pteipc_send( temp_msg, "timerdsA");
       free( temp_msg);
       if(ret_code < 0)
       {   /* Failed clearing timer data */
           pteipc_get_errormsg( ret_code, ErrorMsg );
           sprintf( Buffer, "pteipc error sending to que timerdsA: %s",
                     ErrorMsg );
           TxUtils_Send_Msg_To_Operator(1, Buffer, 1, ERROR_MSG, 
                                 "incoming_host_message_timer_reply", 4,
                                  FATAL_ERROR, " ",
                                  NULL_PTR, NULL_PTR);
           return(false);
       }
       return true;
   }
   else return false;

}

/*
 *BYTE chain_clear_set_echo_timer(char* echo_key, WORD timer_length,char* reply_q)
 *     echo_key        : sent by the network handler
 *     timer_length    : sent by the network handler
 *     reply_q         : application_que
 *Returs: true/flase on success/failure
 */

BYTE chain_clear_set_echo_timer(char* echo_key, WORD timer_length,char* reply_q)
{
LONG ret_code;
char Buffer[256],ErrorMsg[256];
pPTE_MSG pte_echo_timer=NULL_PTR;
pPTE_MSG pte_chain_msg=NULL_PTR;

    if( timer_length <= 0) return true;
    //bld clear et msg
    if((clear_echo_timer(echo_key,&pte_echo_timer,reply_q,0)) == false)
    {
        return false;
    }
    else
    {
        //pte_chain_msg= ptemsg_chain_message (pte_chain_msg,pte_echo_timer);
        pte_chain_msg= pte_echo_timer;
        if( pte_chain_msg == NULL_PTR) return false;
        else
         {
             //free( pte_echo_timer );
             //bld set et msg
             if((set_echo_timer(echo_key,timer_length,&pte_echo_timer,reply_q,0)) == false)
             {
                 return false;
             }
             else
             {
                pte_chain_msg= ptemsg_chain_message (pte_chain_msg,pte_echo_timer);
                
                if( pte_chain_msg == NULL_PTR) return false;
                
                ret_code = pteipc_send(pte_chain_msg,"timerdsA");
                //free(pte_echo_timer );
                free(pte_chain_msg );
                if( ret_code < 0 )
                {
                   pteipc_get_errormsg( ret_code, ErrorMsg );
                   sprintf( Buffer, "Error on pteipc_send to que timerdsA: %s",ErrorMsg );
                   TxUtils_Send_Msg_To_Operator( 1, Buffer, 1, ERROR_MSG, 
                                                "chain_clear_set_echo_timer", 4, FATAL_ERROR," ",NULL_PTR, NULL_PTR);
                   return(false);
                }
                else return true;
             }
         }
    }
}

/*
 *BYTE chain_echo_host(char* echo_key, WORD timer_length,char* reply_q,pPTE_MSG pte_echo_timer)
 *     echo_key        : sent by the network handler
 *     timer_length    : sent by the network handler
 *     reply_q         : application_que
 *Returs: true/flase on success/failure
 */


BYTE chain_echo_host(char* echo_key, WORD timer_length,char* reply_q,pPTE_MSG pte_msg)
{
LONG ret_code;
char Buffer[256],ErrorMsg[256];
pPTE_MSG pte_echo_timer=NULL_PTR;
pPTE_MSG pte_msg_chain=NULL_PTR;

    if(pte_msg == NULL_PTR) return false; 
    if( timer_length == 0)
    {
       //send the pet_msg to timerds and return
       ret_code=pteipc_send( pte_msg, "timerdsA");
       if(ret_code < 0)
       {  /* failed getting timer data */
          pteipc_get_errormsg( ret_code, ErrorMsg );
          sprintf( Buffer, "pteipc error sending to que timerdsA: %s",
                    ErrorMsg );
          TxUtils_Send_Msg_To_Operator(1, Buffer, 1, ERROR_MSG, 
                                    "chain_echo_host", 4,
                                     FATAL_ERROR, " ",
                                     NULL_PTR, NULL_PTR);
          return(false);
       }  
       else return true;
    }
    //bld clear et msg
    if((clear_echo_timer(echo_key,&pte_echo_timer,reply_q,0)) == false)
    {
        return false;
    }
    else
    {
        //chain the message and send it to timerds
        //pPTE_MSG ptemsg_chain_message ( pPTE_MSG p_pte_msg1,pPTE_MSG p_pte_msg2 );
       pte_msg_chain = ptemsg_chain_message (pte_msg,pte_echo_timer);
       if( pte_msg_chain == NULL_PTR)   return false;
       else
       {
           //free(pte_echo_timer);
           if((set_echo_timer(echo_key,timer_length,&pte_echo_timer,reply_q,0)) == false)
           {
               return false;
           }
           else
           {
               //chain the set_timer_request to chain_msg
               pte_msg_chain = ptemsg_chain_message (pte_msg_chain,pte_echo_timer);
               if( pte_msg_chain == NULL_PTR)   return false;
               else
               {
                   //send the chain message to timerds
                   ret_code=pteipc_send( pte_msg_chain, "timerdsA");
                   free( pte_msg_chain);
                   if(ret_code < 0)
                   {  /* failed getting timer data */
                      pteipc_get_errormsg( ret_code, ErrorMsg );
                      sprintf( Buffer, "pteipc error sending to que timerdsA: %s",
                                ErrorMsg );
                      TxUtils_Send_Msg_To_Operator(1, Buffer, 1, ERROR_MSG, 
                                                "chain_echo_host", 4,
                                                 FATAL_ERROR, " ",
                                                 NULL_PTR, NULL_PTR);
                      return(false);
                   }
                   else  return true;
               }
           }

        }
    }
}
