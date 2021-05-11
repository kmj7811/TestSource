/*===================================================

                  TIMER INTERFACE


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

#include "ql_oe.h"

#include <stdio.h>
#include <time.h>

#include "common.h"
#include "timer_if.h"

#include "svclayer_ProcTcuAppPkt.h"
#include "svclayer_TcuMain.h"


timer_t tcuCallIndiTimeId;

static void timerHandler( int sig, siginfo_t *si, void *uc )
{
    timer_t *tidp;
    tidp = (void**)(si->si_value.sival_ptr);
    if ( *tidp == tcuCallIndiTimeId )
        process_tcu_call_wait_expired_timer();
    //else if ( *tidp == secondTimerID )
    //    printf("10ms\n");
    //else if ( *tidp == thirdTimerID )
    //    printf("100ms\n");
}

//example
// 1 sec timer : makeTimer( timerID, 1, 0 )
// 100 msec timer : makeTimer( timerID, 0, 100 )
#if 0
static int makeTimer( timer_t *timerID, int sec, int msec )
{
    struct sigevent te;
    struct itimerspec its;
    struct sigaction sa;
    int sigNo = SIGRTMIN;
	
    LOGI("makeTimer duration(%d)",sec);
	
    /* Set up signal handler. */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timerHandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(sigNo, &sa, NULL) == -1) {
        perror("sigaction");
        return -1;		
    }
    
    /* Set and enable alarm */
    te.sigev_notify = SIGEV_SIGNAL;
    te.sigev_signo = sigNo;
    te.sigev_value.sival_ptr = timerID;
    timer_create(CLOCK_REALTIME, &te, timerID);
    
    its.it_interval.tv_sec = sec;
    its.it_interval.tv_nsec = msec * 1000000;
    its.it_value.tv_sec = sec;
    its.it_value.tv_nsec = msec * 1000000;
    timer_settime(*timerID, 0, &its, NULL);
    
    return 0;
}
#endif


static int createTimer( timer_t *timerID)
{
    struct sigevent te;
    struct sigaction sa;
    int sigNo = SIGRTMIN;
		
    /* Set up signal handler. */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timerHandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(sigNo, &sa, NULL) == -1) {
        perror("sigaction");
        return -1;		
    }
    
    /* Set and enable alarm */
    te.sigev_notify = SIGEV_SIGNAL;
    te.sigev_signo = sigNo;
    te.sigev_value.sival_ptr = timerID;
    timer_create(CLOCK_REALTIME, &te, timerID);    
    
    return 0;
}

int create_tcu_call_active_wait_timer()
{
    int result = 0;
	
    LOGI("Make call active wait timer ");
    if(createTimer(&tcuCallIndiTimeId) !=0) 
    {
       LOGI("Fail to make call active wait Timer\n");
       result = -1;	   
    }
    return result;
}

int start_tcu_call_active_wait_timer(int sec, int msec)
{
    struct itimerspec its;
    int result = 0;	

    LOGI("Start call active wait timer ");
    its.it_interval.tv_sec = sec;
    its.it_interval.tv_nsec = msec * 1000000;
    its.it_value.tv_sec = sec;
    its.it_value.tv_nsec = msec * 1000000;
	
    LOCK_TCU_PLATFORM();
    if(timer_settime(tcuCallIndiTimeId, 0, &its, NULL) == -1)
    {
        LOGI("error start_tcu_call_active_wait_timer\n");
        exit(1);
    }
		
    UNLOCK_TCU_PLATFORM();

    return result;		 
	
}
int stop_tcu_call_active_wait_timer()
{
    struct itimerspec its;
    int result = 0;	
    memset((void*)&its, 0, sizeof(its));
	
    LOCK_TCU_PLATFORM();	
    if (timer_settime(tcuCallIndiTimeId, 0, &its, NULL) == -1)
    {
        LOGI("error disarming timer\n");
        exit(1);
    }
    UNLOCK_TCU_PLATFORM();
    return result;	
}

void delete_tcu_call_active_wait_timer()
{
    LOGI("Delete Call Wait Timer");
    LOCK_TCU_PLATFORM();	
    timer_delete(tcuCallIndiTimeId);
    UNLOCK_TCU_PLATFORM();
	
}


void makeTcuTimer()
{
    int result = 0;

	
    result = create_tcu_call_active_wait_timer();
    if(result !=0)
	LOGI("fail to create tcuCallIndiTimeId ");	

	
}





