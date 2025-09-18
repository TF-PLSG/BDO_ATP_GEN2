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

#ifndef echo_timer_H
#define echo_timer_H

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"




BYTE set_echo_timer(char* echo_key, WORD timer_length,pPTE_MSG* pte_set_timer,char* reply_q,BYTE flag); //flag=1 sends to timerds, flag=0 returns 

BYTE clear_echo_timer(char* echo_key,pPTE_MSG* pte_clr_timer,char* reply_q,BYTE flag);//flag=1 sends to timerds, flag=0 returns  


BYTE chain_clear_set_echo_timer(char* echo_key, WORD timer_length,char* reply_q);

BYTE chain_echo_host(char* echo_key, WORD timer_length,char* reply_q,pPTE_MSG pte_msg);


#endif
