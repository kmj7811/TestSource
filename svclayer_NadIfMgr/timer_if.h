/*===================================================

                  TIMER INTERFACE HEADER


GENERAL DESCRIPTION




Copyright (c) 2018 by Digen, Incorporated.  All Rights Reserved.
Digen Confidential and Proprietary.
=====================================================*/

/*====================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------


*/

#ifndef __TIMER_IF_H
#define __TIMER_IF_H


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>



// Timer Duration
#define TCU_CALL_ACTIVE_WAIT_DURATION			10 // 10sec



int create_tcu_call_active_wait_timer();
int start_tcu_call_active_wait_timer(int sec, int msec);
int stop_tcu_call_active_wait_timer();
void delete_tcu_call_active_wait_timer();
void makeTcuTimer();


#endif //__TIMER_IF_H


