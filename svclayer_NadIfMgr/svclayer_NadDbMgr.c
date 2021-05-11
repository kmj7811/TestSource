/*===================================================

                  TCU Local Data Base Mgr C


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
#include <ql_oe.h>
#include "svclayer_TcuDb.h"
#include "tcu_interface.h"
#include "common.h"

#define BUF_SIZE        128


timer_t tcuDbUpdateID;

#define TCU_DB_UPDATE_DURATION		2   // 2 sec

void print_current_db_status()
{
    tcu_db_items_u db_items_value;

    tcu_db_get(TCU_DB_VIN,&db_items_value);
    LOGI("vin = %s",db_items_value.vin);
    tcu_db_get(TCU_DB_VER,&db_items_value);
    LOGI("ver = %s",db_items_value.ver);
    tcu_db_get(TCU_DB_ICCID,&db_items_value);
    LOGI("iccid = %s",db_items_value.iccid);
    tcu_db_get(TCU_DB_DIR,&db_items_value);
    LOGI("dir = %s",db_items_value.dir);
    tcu_db_get(TCU_DB_IMEI,&db_items_value);
    LOGI("imei = %s",db_items_value.imei);
	
    tcu_db_get(TCU_DB_SIM_STATE,&db_items_value);
    LOGI("sim state = %d",db_items_value.sim_state);
    tcu_db_get(TCU_DB_RAT,&db_items_value);
    LOGI("rat = %d",db_items_value.rat);
    tcu_db_get(TCU_DB_PLMN,&db_items_value);
    LOGI("mcc = %d, mnc = %d",db_items_value.plmn.mcc, db_items_value.plmn.mnc);
    tcu_db_get(TCU_DB_LAC,&db_items_value);
    LOGI("lac = %d",db_items_value.lac);
    tcu_db_get(TCU_DB_REG_STATE,&db_items_value);
    LOGI("reg_state = %d",db_items_value.reg_state);
    tcu_db_get(TCU_DB_CS_STATE,&db_items_value);
    LOGI("cs_state = %d",db_items_value.cs_state);
    tcu_db_get(TCU_DB_PS_STATE,&db_items_value);
    LOGI("ps_state = %d",db_items_value.ps_state);
    tcu_db_get(TCU_DB_RSSI,&db_items_value);
    LOGI("rssi = %d",db_items_value.rssi);
        	
}
void updateNadStaticInfo()
{
    int    ret     = E_QL_OK;
    char   buf[BUF_SIZE] = {0};
    tcu_db_items_u db_items_value;	
    int length = 0;	
    char *pin_state[] = {"NOT_INSERTED", "READY", "PIN_REQ", "PUK_REQ", "BUSY", "BLOCKED", "UNKNOWN"};
	
    char* temp_vin = "0123456";
    char* temp_dir = "01024641111";	
//QL_DEV_GetImei
//QL_DEV_GetFirmwareVersion
//QL_SIM_GetICCID

    //tcu_get_VIN_number(); //future todo
    memcpy(db_items_value.vin,temp_vin,sizeof(temp_vin));
    tcu_db_put(TCU_DB_VIN, &db_items_value);
	
    ret = QL_DEV_GetFirmwareVersion(buf, BUF_SIZE);	
    //LOGI("ret = %d, FW Version: %s", ret, buf);
    length = get_string_num(buf);	
    memcpy(db_items_value.ver, buf, length);
    tcu_db_put(TCU_DB_VER, &db_items_value);

    ret = QL_SIM_GetICCID(E_QL_SIM_EXTERNAL_SLOT_1, buf, BUF_SIZE);
    //LOGI("ret = %d, ICCID: %s", ret, buf);
    length = get_string_num(buf);	
    memcpy(db_items_value.iccid, buf, length);
    tcu_db_put(TCU_DB_ICCID, &db_items_value);

    //tcu_get_DIR() //future todo
    memcpy(db_items_value.dir,temp_dir,sizeof(temp_dir));
    tcu_db_put(TCU_DB_DIR, &db_items_value);

    memset(buf,0,BUF_SIZE);
    ret = QL_DEV_GetImei(buf, BUF_SIZE);
    //LOGI("ret = %d, IMEI: %s", ret, buf);
    length = get_string_num(buf);	
    memcpy(db_items_value.imei, buf, length);	
    tcu_db_put(TCU_DB_IMEI, &db_items_value);

    ret = QL_SIM_GetState(E_QL_SIM_EXTERNAL_SLOT_1);
    //LOGI("QL_SIM_GetState = %s", pin_state[ret]);
    db_items_value.sim_state = ret;	
    tcu_db_put(TCU_DB_SIM_STATE, &db_items_value);	

	
}

void precessTcuDbUpdate()
{
    int    ret     = E_QL_OK;
    int i = 0;
    int start = 0;	
    char   buf[BUF_SIZE] = {0};
    tcu_db_items_u db_items_value;	
    int length = 0;	
    ST_CellInfo  info = {0};	
    byte nt_type = RAT_LTE;	

    QL_NW_REG_STATE_TYPE_T t_reg_state = {0};  
    char *reg_state[] = {"NOT_REGISTERED", 
                         "REGISTERED_HOME_NETWORK", 
                         "NOT_REGISTERED_SEARCHING_NOW", 
                         "DENIED", 
                         "UNKNOWN", 
                         "ROAMING"};
    
    char *cs_ps_state[] = {"UNKNOWN", 
                         "ATTACHED", 
                         "DETACHED"}; 



    ret = QL_NW_GetServingCell(&info);
    for(i=0; i<info.validCnt; i++)
    {
        //LOGI("ServingCell[%d]: rat=%s, mcc=%d, mnc=%d, lac = 0x%X, ci=0x%x\n",  i,  info.cellInfo[i].rat,  info.cellInfo[i].mcc, 
        //    info.cellInfo[i].mnc,   info.cellInfo[i].lac,  info.cellInfo[i].cid);
		
        if(strcmp("LTE", info.cellInfo[i].rat) == 0)
            nt_type = RAT_LTE;
        else if(strcmp("UMTS", info.cellInfo[i].rat) == 0)
            nt_type = RAT_WCDMA;
        else if(strcmp("GSM", info.cellInfo[i].rat) == 0)
            nt_type = RAT_GSM;		
    
        db_items_value.rat = nt_type;
        tcu_db_put(TCU_DB_RAT, &db_items_value);
    
        db_items_value.plmn.mcc = info.cellInfo[i].mcc;
        db_items_value.plmn.mnc = info.cellInfo[i].mnc;
        tcu_db_put(TCU_DB_PLMN, &db_items_value);	

        db_items_value.lac = info.cellInfo[i].lac;
        tcu_db_put(TCU_DB_LAC, &db_items_value);

		
    }

    ret = QL_NW_GetRegState_Ex(&t_reg_state);
    if(ret!=0) 
    {
        LOGI("Failed to QL_NW_GetRegState_Ex, ret = %d", ret);
    }
    else 
    {
        //LOGI("ret = %d, nw_state= %s, cs_state= %s, ps_state= %s,rssi= %d", ret,  reg_state[t_reg_state.e_state],  cs_ps_state[t_reg_state.e_cs_state], 
        //        cs_ps_state[t_reg_state.e_ps_state],  t_reg_state.rssi);
    }
    db_items_value.reg_state = t_reg_state.e_state;
    tcu_db_put(TCU_DB_REG_STATE, &db_items_value);	

    db_items_value.cs_state = t_reg_state.e_cs_state;
    tcu_db_put(TCU_DB_CS_STATE, &db_items_value);	
	
    db_items_value.ps_state = t_reg_state.e_ps_state;
    tcu_db_put(TCU_DB_PS_STATE, &db_items_value);	
	
    db_items_value.rssi = t_reg_state.rssi;
    tcu_db_put(TCU_DB_RSSI, &db_items_value);	

    //print_current_db_status(); If you want to see the DB value. Open it
	
}

void initTcuDbMgr()
{
    int ret = 0;
	
    LOGI("Init Tcu DB Mgr");
    updateNadStaticInfo();

    ret = makeTcuDbUpdateTimer();
    if(ret < 0)
        LOGI("Fail to Create TcuUpdateTimer");

    ret = start_tcuDbUpdateTimer(TCU_DB_UPDATE_DURATION,0);	
    if(ret < 0)
        LOGI("Fail to start TcuUpdateTimer");	
}


static void timerHandler( int sig, siginfo_t *si, void *uc )
{
    timer_t *tidp;
    tidp = (void**)(si->si_value.sival_ptr);
    if ( *tidp == tcuDbUpdateID )
        precessTcuDbUpdate();
    //else if ( *tidp == secondTimerID )
    //    printf("10ms\n");
    //else if ( *tidp == thirdTimerID )
    //    printf("100ms\n");
}

int get_string_num(char *string)
{
    int i = 0;
    int ret = 0;	
    char buf[100];		
    if(string == NULL)
      return ret;
    memcpy(buf,string,	sizeof(buf));
    for( i = 0; 	buf[i]!= '\0'; ++i);  
    return i;		
}

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



int makeTcuDbUpdateTimer()
{
    int result = 0;

    if(createTimer(&tcuDbUpdateID) !=0)  
    {
       LOGI("Fail to make makeTcuDbUpdateTimer");
       result = -1;	   
    }
    return result;
}

int start_tcuDbUpdateTimer(int sec,int msec)
{
    struct itimerspec its;
    int result = 0;	

    LOGI(" start_tcuDbUpdateTimer ");
    its.it_interval.tv_sec = sec;
    its.it_interval.tv_nsec = msec * 1000000;
    its.it_value.tv_sec = sec;
    its.it_value.tv_nsec = msec * 1000000;
	
    if(timer_settime(tcuDbUpdateID, 0, &its, NULL) == -1)
    {
        LOGI("error start_tcuDbUpdateTimer");
        exit(1);
    }
    return result;	
}
int stop_tcuDbUpdateTimer()
{
    struct itimerspec its;
    int result = 0;	
    memset((void*)&its, 0, sizeof(its));
	
    if (timer_settime(tcuDbUpdateID, 0, &its, NULL) == -1)
    {
        LOGI("error disarming timer\n");
        exit(1);
    }
    return result;	
}

void delete_tcuDbUpdateTimer()
{
    LOGI("delete_tcuDbUpdateTimer");
    timer_delete(tcuDbUpdateID);	
}

