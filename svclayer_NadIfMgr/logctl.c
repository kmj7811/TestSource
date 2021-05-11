/*===================================================

                  LOG CTRL C


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

#include <sys/ioctl.h>
#include <pthread.h>


#include <string.h> /* for memset */
#include <unistd.h> /* for close */
#include <sys/time.h> /* for gettimeofday */
#include <stdarg.h> /* for va_start */

#include <sys/ipc.h>
#include <sys/sem.h>


#include "logctl.h"
#include "common.h"


union semun{
	int                  val;
	struct   semid_ds   *buf;
	unsigned short int  *arrary;
};

#if 1
char* get_time(char* buf) {
    struct timeb start;
    char append[100];
    if (buf) {
        ftime(&start);
        strftime(buf, 100, "%H:%M:%S", localtime(&start.time));

        /* append milliseconds */
        sprintf(append, ":%03u", start.millitm);
        strcat(buf, append);
    }
    return buf;
}
#endif

int mlogThreadFn=0;

static char tcu_LogFilePath[200] = "/usrdata/di_log";

static char tcu_LogFilename0 [200] = "/usrdata/di_log/tcu_log" ;
static char tcu_LogFilenameBak0 [200] = "/usrdata/di_log/tcu_log.bak" ;
//static char tcu_LogFilename1 [200] = "/tmp/di_log/mcu_log" ;
//static char tcu_LogFilenameBak1 [200] = "/tmp/di_log/mcu_log.bak" ;
//static char tcu_LogFilename2 [200] = "/tmp/di_log/avn_log" ;
//static char tcu_LogFilenameBak2 [200] = "/tmp/di_log/avn_log.bak" ;
//static char tcu_LogFilename3 [200] = "/tmp/di_log/tcu_log3" ;
//static char tcu_LogFilenameBak3 [200] = "/tmp/di_log/tcu_log3.bak" ;
//static char tcu_LogFilename4 [200] = "/tmp/di_log/tcu_log4" ;
//static char tcu_LogFilenameBak4 [200] = "/tmp/di_log/tcu_log4.bak" ;
//static char tcu_LogFilename5 [200] = "/tmp/di_log/tcu_log5" ;
//static char tcu_LogFilenameBak5 [200] = "/tmp/di_log/tcu_log5.bak" ;


char tcu_Buffer[TCU_LOG_BUFFER_SIZE];
char tcu_Buffer2[TCU_LOG_BUFFER_SIZE];

static int log_select = 0;

static int tcu_LogCnt=0;
static int  tcu_Log_semid;

int tcu_Log_semaphore_init(void)
{

	union semun arg;

	if ( -1 == (tcu_Log_semid = semget( IPC_PRIVATE, 1, IPC_CREAT | 0666)))
	{
		printf( "tcu_Log_semaphore_init semget() Error\n");
		return -1;
	}	

	arg.val  =  1;                // set 1 to semaphore value
	if ( -1 == semctl(tcu_Log_semid, 0, SETVAL, arg))
	{
		printf( "tcu_Log_semaphore_init semctl()-SETVAL Error\n");
		return -1;
	}

	return 0;
}

int tcu_Log_semaphore_deinit(void)
{

	union semun arg;

	if ( -1 == semctl(tcu_Log_semid, 0, IPC_RMID, arg))
	{
		printf( "tcu_Log_semaphore_deinit semctl()-IPC_RMID Error\n");
		return -1;
	}

	return 0;
}

void tcu_Log_semaphore_lock(void)
{
	struct sembuf pbuf;

	pbuf.sem_num   = 0;
	pbuf.sem_op    = -1;
	pbuf.sem_flg   = SEM_UNDO;

	if ( -1 == semop(tcu_Log_semid, &pbuf, 1))
		printf( "tcu_Log_semaphore_lock() Error\n");
}

void tcu_Log_semaphore_unlock(void)
{
	struct sembuf vbuf;

	vbuf.sem_num   = 0;
	vbuf.sem_op    = 1;
	vbuf.sem_flg   = SEM_UNDO;

	if ( -1 == semop( tcu_Log_semid, &vbuf, 1))
		printf( "tcu_Log_semaphore_unlock() Error\n");
}


//static inline void logSysLog(int log_select,const char* fn,  int ln, const char *format, ...)
void logSysLog(int log_select,const char* fn,  int ln, const char *format, ...)
{
    //char fmt[TCULOGFILE_MAX_SIZE]= {0,};
    char cmd[64]= {0,};
    struct stat  st;
    struct timeval tv;
	
    FILE *fp=0;

	
    struct tm temp_time;
    char ctime_now[200];
    char  select_log[200] = {0,};
    char  select_log_back[200] = {0,};
    char buf[100];
    int nIdx = 0;
    va_list ap ;
    int fileSize=0;	

    struct timeb start;
    char append[100];
	
    if (ctime_now) {
        ftime(&start);
        strftime(ctime_now, 100, "%H:%M:%S", localtime(&start.time));

        /* append milliseconds */
        sprintf(append, ":%03u", start.millitm);
        strcat(ctime_now, append);
    }

    //gettimeofday(&tv, NULL);
    //temp_time = *gmtime(&tv.tv_sec);

    //strftime(ctime_now, sizeof(ctime_now), "%Y/%m/%d %H:%M:%S",&temp_time);

    if(log_select == TCU_LOG)
    {
      memcpy(select_log, tcu_LogFilename0,sizeof(tcu_LogFilename0));
      memcpy(select_log_back,tcu_LogFilenameBak0, sizeof(tcu_LogFilenameBak0));
    }
#if 0	
    else if(log_select == MCU_LOG)	
    {
      memcpy(select_log, tcu_LogFilename1,sizeof(tcu_LogFilename1));
      memcpy(select_log_back,tcu_LogFilenameBak1, sizeof(tcu_LogFilenameBak1));
    }
    else if(log_select == AVN_LOG)
    {
      memcpy(select_log, tcu_LogFilename2,sizeof(tcu_LogFilename2));
      memcpy(select_log_back,tcu_LogFilenameBak2, sizeof(tcu_LogFilenameBak2));
    }
	
    else if(log_select == NAD_MCU_IF_MGR)
    {
      memcpy(select_log, tcu_LogFilename3,sizeof(tcu_LogFilename3));
      memcpy(select_log_back,tcu_LogFilenameBak3, sizeof(tcu_LogFilenameBak3));
    }
    else if(log_select == NAD_AVN_IF_MGR)
    {
      memcpy(select_log, tcu_LogFilename4,sizeof(tcu_LogFilename4));
      memcpy(select_log_back,tcu_LogFilenameBak4, sizeof(tcu_LogFilenameBak4));
    }
    else if(log_select == NAD_NW_MGR)
    {
      memcpy(select_log, tcu_LogFilename5,sizeof(tcu_LogFilename5));
      memcpy(select_log_back,tcu_LogFilenameBak5, sizeof(tcu_LogFilenameBak5));
    }
#endif
    else
    {
        ;
    }
    //printf("selected log file %s", select_log);
	
    //tcu_Log_semaphore_lock();	
	
    if (stat(tcu_LogFilePath, &st) != 0) {
    	sprintf(cmd, "mkdir -p %s",tcu_LogFilePath);
    	system(cmd);
    }
    
    memset( tcu_Buffer, 0x00, sizeof(tcu_Buffer) ) ;
    
    
    snprintf( tcu_Buffer, TCU_LOG_BUFFER_SIZE, "\n[%s][%s:%d] ",ctime_now,fn,ln);
    nIdx = strlen(tcu_Buffer);

	
    va_start( ap, format );
	
    if ( nIdx < TCU_LOG_BUFFER_SIZE )
    {
      vsnprintf( tcu_Buffer + nIdx , TCU_LOG_BUFFER_SIZE - nIdx,  format, ap ) ;
      nIdx = strlen(tcu_Buffer);	  
    }
    va_end(ap);
    
    if ( nIdx < TCU_LOG_BUFFER_SIZE )
      tcu_Buffer[nIdx] = 0x00 ;
    else
      tcu_Buffer[ TCU_LOG_BUFFER_SIZE - 1 ] = 0x00 ;
        
    //printf(" tmp tcu_buffer:%s\n", tcu_Buffer);
    
    fp = fopen( (char*)select_log , "a+" );
    
    if(fp)
    {
      //printf(" tcu_LogCnt:%d\n", tcu_LogCnt);
	  
      if(tcu_LogCnt>=TCU_LOG_FILE_CNT) 
      {
        fseek(fp, 0, SEEK_END);
        fileSize = ftell(fp);
        rewind(fp); 
      }

      //printf(" fileSize:%d\n", fileSize);
	  
      fprintf(fp, "%s", tcu_Buffer ) ;
    
      fclose(fp);
    
      if(tcu_LogCnt>=TCU_LOG_FILE_CNT) 
      {
        if (fileSize > TCU_LOG_FILE_SIZE) 
	 {
	   rename(select_log,select_log_back);
          remove(select_log);          
        }
    
        tcu_LogCnt=0;
      } 
      else
      {
        tcu_LogCnt++;
      }
    }
    
    //tcu_Log_semaphore_unlock();
    	
}


#if 1
void *logThreadFn(void *fn)
{
    int msqid;
    int ret;
    boolean bReturn;	    
    
    LOGI("--- logThreadFn Start\n");   
    
    while(mlogThreadFn!=1) 
    {
    
    }    
    LOGI("---External2CCUThread Error -- Exit\n");	
    return 0;
	
} 


void *logMainThreadFn(void *fn)
{
    EXCEPTION_INIT
    int result=0;
    //char flags = NULL;
    int count =0;
    int nRetry=0;
    pthread_t logThread;

    struct stat  st;
	
    
    pthread_attr_t attr;
    
    
    
    LOGI("---------logMainThreadFn Start------------\n");

    #if 1
    //NAD Rcv thread creation..
    memset(&attr,0,sizeof(pthread_attr_t));
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  // 자동 thread 해제 , join 필요 없음.
    if( pthread_create(&logThread,&attr, logThreadFn,NULL) < 0)
    {
      LOGI("logThreadFn Thread create error\n");
      THROW();
    }
    pthread_attr_destroy(&attr);
	
   #endif

#if 0	
    while(mTcuExitThread !=1)
    {
      LOGI("Test msg %d\n",count);
      count++;

#if 0	  
      if ( 0 > stat( TCULOGFILE, &st))
        LOGI( "There is no file\n");
      else
        LOGI( "File sie is %d\n", st.st_size);	  
#endif

      sleep(2);
    }
#endif

    CATCH_ERR()
    {
      result = 1;
    }
    //LOGI("---------logThreadFn End------------\n");

	
    return NULL;

}



int start_tcuLogCtl()
//int main(int argc,char *argv[])
{
    EXCEPTION_INIT
    pthread_t logMainThread;
    pthread_t tid;
    int result = 0;
    int ret;	

    tcu_Log_semaphore_init();


    LOGI("...............................................\n");
    LOGI("[!] LOG Control Started ver %s\n", LOG_CTL_VERSION);
    LOGI("...............................................\n");

	
    ret = pthread_create(&logMainThread,NULL,logMainThreadFn,NULL);
    if(ret !=0)
    {
      LOGI("Failed to create ccumainThread\n");
      THROW();		
    }
    pthread_join(logMainThread, NULL);
    sleep(5);
	  

    CATCH_ERR()
    {
      result = 1;
    }	

	
    LOGI("...................LOG_MAIN_END....................\n");

    tcu_Log_semaphore_deinit();
  
  return result;


}
#endif

