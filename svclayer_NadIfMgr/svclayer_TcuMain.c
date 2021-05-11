/*===================================================

                  TCU MAIN C


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

/*=====================================================

                     INCLUDE FILES FOR MODULE

=====================================================*/
#include "ql_oe.h"

#include <execinfo.h>
#include <signal.h>

 


#include "svclayer_TcuMain.h"
#include "common.h"
#include "tcu_interface.h"
#include "svclayer_ProcTcuAppPkt.h"
#include "svclayer_ProcTcuMcuIfPkt.h"
#include "timer_if.h"
#include "svclayer_NadDbMgr.h"



int mTcuExitThread=0;
pthread_mutex_t mutexForCallStateTimer;

int bTCU_TEST = 0; // For future test



void LOCK_TCU_PLATFORM()
{
    LOGI("LOCK");
    TCU_PLATFORM_MUTEX_LOCK(&mutexForCallStateTimer);	
}
void UNLOCK_TCU_PLATFORM()
{
    LOGI("UNLOCK"); 
    TCU_PLATFORM_MUTEX_UNLOCK(&mutexForCallStateTimer);	
}

void *tcuAppTcuThreadFn(void *fn)
{
    int msqid;
    boolean bReturn;	
    tcuApp_cmdq_t  rcv_data;
    
    
    LOGI("--- tcuAppTcuThreadFn Start");
    
    msqid = msgget( (key_t)APP2FRAME_MSGQ_KEY, IPC_CREAT | 0666);
	
    if(msqid<0)
    {
      LOGI("tcuAppTcuThreadFn msgget error");
      return;
    }
    
    while(mTcuExitThread!=1) 
    {
      memset(&rcv_data,0,sizeof(tcuApp_cmdq_t));
      if (msgrcv( msqid, (void *)&rcv_data, sizeof(tcuApp_cmdq_t)- sizeof(long), APP_TO_FRAME_TYPE, 0) == -1)
      {
        LOGI("APP_TO_FRAME_TYPE msgrcv fail");
        usleep(10000);
        continue;
      }

      bReturn = process_tcuAppTcu_pkt(&rcv_data); 
      if(bReturn !=0)
      {
        LOGI("Process tcuApp packet error");      
      }
    }
    
    msgctl(msqid, IPC_RMID, 0);
    LOGI("---TCUApp to TCUThread Error -- Exit");	
    return;
	
}


void *mcuIfTcuThreadFn(void *fn)
{
    int msqid;
    boolean bReturn;	
    mcu_cmdq_t  rcv_data;

    	
    
    LOGI("--- mcuIfTcuThreadFn Start");
    
    msqid = msgget( (key_t)MCUIF2FRAME_MSGQ_KEY, IPC_CREAT | 0666);
	
    if(msqid<0)
    {
      LOGI("mcuIfTcuThreadFn msgget error");
      return;
    }
    
    while(mTcuExitThread!=1) 
    {
      memset(&rcv_data,0,sizeof(mcu_cmdq_t));
      if (msgrcv( msqid, (void *)&rcv_data, sizeof(mcu_cmdq_t)- sizeof(long), MCUIFMGR_TO_FRAME_TYPE, 0) == -1)
      {
        LOGI("MCUIFMGR_TO_FRAME_TYPE msgrcv fail");
        usleep(10000);
        continue;
      }

      bReturn = process_mcuIfTcu_pkt(&rcv_data); 
      if(bReturn !=0)
      {
        LOGI("Process mcu packet error");      
      }
    }
    
    msgctl(msqid, IPC_RMID, 0);
    LOGI("---MCU to TCUThread Error -- Exit");	
    return;
	
}

void *avnTcuThreadFn(void *fn)
{
    int msqid;
    boolean bReturn;	
    avn_cmdq_t  rcv_data;

    	
    
    LOGI("--- avnTcuThreadFn Start");
    
    msqid = msgget( (key_t)AVNIF2FRAME_MSGQ_KEY, IPC_CREAT | 0666);
	
    if(msqid<0)
    {
      LOGI("avnTcuThreadFn msgget error");
      return;
    }
    
    while(mTcuExitThread!=1) 
    {
      memset(&rcv_data,0,sizeof(avn_cmdq_t));
      if (msgrcv( msqid, (void *)&rcv_data, sizeof(avn_cmdq_t)- sizeof(long), AVNIFMGR_TO_FRAME_TYPE, 0) == -1)
      {
        LOGI("AVNIFMGR_TO_FRAME_TYPE msgrcv fail");
        usleep(10000);
        continue;
      }

      bReturn = process_avnIfTcu_pkt(&rcv_data); 
      if(bReturn !=0)
      {
        LOGI("Process mcu packet error");      
      }
    }
    
    msgctl(msqid, IPC_RMID, 0);
    LOGI("---MCU to TCUThread Error -- Exit");	
    return;
	
}



static void* mThreadFn(void* context)
{
  int msqid;
  BOOL bReturn = TRUE;
  msg_t rcv_data;

  LOGI("--- mThreadFn Start");

  msqid = msgget( (key_t)NAD2IF_MSGQ_KEY, IPC_CREAT | 0666);

  if(msqid<0)
  {
    LOGI("mThreadFn msgget error\n");
    return(void*)-1;
  }

  while(mTcuExitThread!=1)
  {
    memset(&rcv_data,0,sizeof(msg_t));
    if (msgrcv( msqid, (void *)&rcv_data, sizeof(msg_t)- sizeof(long), NAD2IF_MSGQ_KEY, 0) == -1)
    {
      LOGI("SENDDAT_KEY msgrcv fail\n");
      usleep(10000);
      continue;
    }

    bReturn = process_nadMsg_pkt(&rcv_data);
    if(!bReturn)
    {
      LOGI("fail to process app msg");
    }
  }
  
  LOGI("---mThreadFn Error -- Exit\n");
  return (void*)-1;
  return NULL;
}
void *tcuMainThreadFn(void *fn)
{
    EXCEPTION_INIT
    int result=0;
   
    pthread_t tcuAppTcuThread;   
    pthread_t avnTcuThread;
    pthread_t mcuTcuThread;

    pthread_t msvcReceiverThread;  
	
    pthread_attr_t attr;
    
    
    
    LOGI("---------tcuMainThreadFn Start------------");

    TCU_PLATFORM_MUTEX_INIT(&mutexForCallStateTimer);	

    //Thread creation for process packet from MCUIF mgr to TCU main...
    memset(&attr,0,sizeof(pthread_attr_t));
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  // 자동 thread 해제 , join 필요 없음.
    if( pthread_create(&mcuTcuThread,&attr, mcuIfTcuThreadFn,NULL) < 0)
    {
      LOGI("\n\n\n\n ***** Opps !!! mcuIfTcuThreadFn Thread create error\n");
      THROW();
    }
    pthread_attr_destroy(&attr);
	
    //Thread creation for process packet from TCUApp to TCU main.
    memset(&attr,0,sizeof(pthread_attr_t));
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  // 자동 thread 해제 , join 필요 없음.
    if( pthread_create(&tcuAppTcuThread,&attr, tcuAppTcuThreadFn,NULL) < 0)
    {
      LOGI("\n\n\n\n ***** Opps !!! tcuAppTcuThread Thread create error");
      THROW();
    }
    pthread_attr_destroy(&attr);

    //Thread creation for process packet from AVN IF mgr to TCU main.
    memset(&attr,0,sizeof(pthread_attr_t));
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  // 자동 thread 해제 , join 필요 없음.
    if( pthread_create(&avnTcuThread,&attr, avnTcuThreadFn,NULL) < 0)
    {
      LOGI("\n\n\n\n ***** Opps !!! avnTcuThread Thread create error");
      THROW();
    }
    pthread_attr_destroy(&attr);

	
    //Thread creation for process packet from TCUApp to TCU main.
    memset(&attr,0,sizeof(pthread_attr_t));
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  // 자동 thread 해제 , join 필요 없음.
    if( pthread_create(&msvcReceiverThread,&attr, mThreadFn,NULL) < 0)
    {
      LOGI("\n\n\n\n ***** Opps !!! msvcReceiverThread Thread create error");
    }
    pthread_attr_destroy(&attr);


#if 0	
    result = startMcuIfMgr();
    if(result !=0)
    {
        LOGI("\n\n\n\n ***** Opps !!! Fail startMcuIfMgr");
        THROW();		
    }
#endif	
    result = startAvnIfMgr();
    if(result !=0)
    {
        LOGI("\n\n\n\n ***** Opps !!! Fail startAvnIfMgr");
        THROW();		
    }

// If TcuMain is integrated other process. It should be blocked
// If TcuMain is main function. It should be open
#if 0
    while(mTcuExitThread !=1)
    {
        if(bTCU_TEST == 1)
        {
        }

    }
#endif

    LOGI("\n\n\n\n TCU MAIN STARTED ----");
    
    CATCH_ERR()
    {
      result = 1;
    }
    LOGI("---------tcuMainThreadFn End------------\n");

	
    return  (void*)-1;

}



//segfault가 발생했을 때 호출될 함수
//From http://blog.naver.com/PostView.nhn?blogId=hermet&logNo=198926090&beginTime=0&jumpingVid=&from=section&redirect=Log&widgetTypeCall=true
//확인하기 이해서 다음의 명령어 사용한다.
//$addr2line -fe ./tcu_main 0x7000f43  <-0: 번째 address
void sighandler(int sig)
{
    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size); 

    for(i = 2; i < size; i++)
        printf("%d: %s\n", (int)i - 2, strings[i]); 

    free(strings);
    exit(1);
}

void sigquithd(int sig)
{
    LOGI("CTRL+C init and exit program");
    mTcuExitThread = -1;	
}


//int main(int argc,char *argv[])
int start_tcu_main()
{
    EXCEPTION_INIT
    pthread_t tcumainThread;
    int result = 0;
    int ret;	

    LOGI("......................................");
    LOGI("TCU MAIN Started ver %s", TCU_VERSION);
    LOGI("......................................");
    
    system("insmod /usr/lib/modules/3.18.48/kernel/drivers/spi/spidev.ko busnum=6 chipselect=0");
    
    //segfault 발생시 호출될 sighandler 함수 등록..
    signal(SIGSEGV, sighandler); 
    signal(SIGINT, sigquithd);

    result = start_tcuLogCtl();
    if(result !=0)
    {
        LOGI("\n\n\n\n ***** Opps !!! Fail to init start_tcuLogCtl \n\n\n\n ");
        //Do init VoiceCallMgr again after time elapse ..
        //It must be implemented later..
        //THROW();		
    }

	
    //Do any varialbe Initialization process
    //Initialization TCU Local Varialbles

    makeTcuTimer();
    
    //Do any function Initialization process
    initTcuDbMgr(); 

    //Location Manager Start
    result = startTcuLocationMgr();
    if(result !=0)
    {
        LOGI("\n\n\n\n ***** Opps !!! Fail startTcuLocationMgr \n\n\n\n ");
        //Do init LocationMgr again after time elapse ..
        //It must be implemented later..
        //THROW();	
        sleep(1); 
        LOGI("......startLocMgr Again......");
        result = startTcuLocationMgr();	
        if(result !=0)
        {
          LOGI("\n\n\n\n ***** Opps !!! Fail startTcuLocationMgr again \n\n\n\n ");
          sleep(1);
          result = startTcuLocationMgr(); 
          if(result !=0)
          {
            LOGI("\n\n\n\n ***** Opps !!! Fail startTcuLocationMgr again \n\n\n\n ");          
          }
        }
    }
    
    //////////////////////////////////////////////
    result = voiceCallInit();
    if(result !=0)
    {
        LOGI("\n\n\n\n ***** Opps !!! Fail to init VoiceCallMgr \n\n\n\n ");
        //Do init VoiceCallMgr again after time elapse ..
        //It must be implemented later..
        //THROW();		
    }

	
    //while(1)
    {
    	
      ret = pthread_create(&tcumainThread,NULL,tcuMainThreadFn,NULL);
      if(ret !=0)
      {
        LOGI("\n\n\n\n ***** Opps !!! Failed to create tcumainThread\n");
        THROW();		
      }
      pthread_join(tcumainThread, NULL);
      sleep(5);
    }


    //allocate voice handler for call
   //Allocate data handler for data service future todo
   //Allocate SMS handler for sms service future todo

    CATCH_ERR()
    {
      result = 1;
    }	

	
    LOGI("...................TCU_MAIN_TERMINATED....................\n");
  
  return result;


}

