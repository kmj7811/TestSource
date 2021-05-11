/*===================================================

                  PROCESS TCU APP PACKET


GENERAL DESCRIPTION

  Process Msg from TCU APP  to TCU  thread 
  Make Response command


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

#include "common.h"
#include "msg_if.h"
#include "tcu_interface.h"
#include "svclayer_ProcTcuAppPkt.h"
#include "svclayer_ProcTcuMcuIfPkt.h"
#include "svclayer_ProcTcuAvnIfPkt.h"
#include "svclayer_TcuMain.h"
#include "timer_if.h"


typedef struct
{  
    tcu_app_remote_cmd_type cmd;
    handler_type handler;
} tcu_app_remote_cmd_processor_type;

typedef struct
{  
    tcu_app_satety_cmd_type cmd;
    handler_type handler;
} tcu_app_safety_cmd_processor_type;

typedef struct
{  
    tcu_app_infortain_cmd_type cmd;
    handler_type handler;
} tcu_app_infortain_cmd_processor_type;


static boolean dummy(tcuApp_cmdq_t *rcv_data);
static boolean process_svt_start_req(tcuApp_cmdq_t *rcv_data);
static boolean  process_svt_stop_req(tcuApp_cmdq_t *rcv_data);


tcu_app_remote_cmd_processor_type remote_handler_tbl[] =
{
    {FIND_LOCATION_REQ,        			NULL},
    {DOOR_OPEN_REQ,       				NULL},
    {ENGINE_START_REQ,       			NULL},
    {ENGINE_STOP_REQ,       			NULL},
    {HAV_CONTROL_REQ,       			NULL},
    {LIGHT_HORN_REQ,       				NULL},
    {VEHICLE_STATUS_CHECK_REQ,      	NULL},
    {DOOR_OPEN_REQ,       				NULL},    
    {REMOTE_ID_MAX,          				dummy}
};


tcu_app_safety_cmd_processor_type safety_handler_tbl[] =
{
    {SOS_CALL_INDI_ACK,        			NULL},
    {AIRBACK_E_CALL_INDI_ACK,       	NULL},
    {CALL_EST_INDI_ACK,       			NULL},
    {CALL_END_INDI_ACK,       			NULL},
    {VEHICLE_TRACKING_START_REQ,    	process_svt_start_req},
    {VEHICLE_TRACKING_STOP_REQ,       	process_svt_stop_req},
    {VEHICLE_TRACKING_RESULT_ACK,     	NULL},
    {BRG_ALAM_INDI_ACK,       			NULL},    
    {DTC_RESULT_CODE_ACK,       		NULL},    
    {DIAGNOSIS_ANALYSIS_INFO,       	NULL},    
    {SCHEDULED_CHECK_REQ,       		NULL},    
    {SUPPLIES_ITEM_NOTI,       			NULL},    
    {CS_NOTI,       						NULL},    
    {CS_CALL_INDI_ACK,       			NULL},    
    {CALL_DIAL_REQ,       				NULL},    
    {POI_INFO,       						NULL},    
    {SFY_AND_CALL_ID_MAX,          		dummy}
};

tcu_app_infortain_cmd_processor_type infortain_handler_tbl[] =
{
    {SOS_CALL_INDI_ACK,        			NULL},
    {INFOTAIN_ID_MAX,          		dummy}
};




static boolean dummy(tcuApp_cmdq_t *rcv_data)
{
  LOGI("dummy called\n");
  return TRUE;
}


void process_tcu_call_wait_expired_timer()
{
    byte call_state = CALL_IDLE;
    int ret = 0;	

    LOGI("---------Call Wait  Timer Expired-------"); 
    //Call state maybe go to END status.		
    #if 0		
    call_state =  nad_get_current_call_state();	
    if(call_state != CALL_ACTIVE)	
    {
        ret =  send_voice_call_result_indi_to_mcuif(CALL_STATUS_LINE_BUSY);
        if(ret !=0)
        {
            LOGI("Msg Q send Fail"); 
        }	

        send_voice_call_failure_indi_to_avnif(LINE_BUSY_REASON);   //future todo
    }
    #endif	
    stop_tcu_call_active_wait_timer();	

	
}

int send_msgq_to_tcuapp(tcuApp_cmdq_t send_data)
{
  int qid;
  int ret = 0;

  send_data.data_type = FRAME_TO_APP_TYPE;
  qid = msgget( (key_t)FRAME2APP_MSGQ_KEY, IPC_CREAT | 0666);
  
  if(qid<0)
  {
    LOGI("send_msgq_to_tcuapp msgget error\n");
    ret = -1;
    return ret;
  }

  if(msgsnd( qid, &send_data, sizeof(send_data) - sizeof(long), 0)<0)
  {
    LOGI("send_msgq_to_tcuapp msgsnd error\n");
    ret = -1;
  }

  LOGI("send_msgq() key(%d) ret =%d",FRAME2APP_MSGQ_KEY, ret);
  
  return ret;
}


int send_sos_voice_call_indi_to_tcuapp()
{
    int result = 0;
    tcuApp_cmdq_t send_data;

    sos_call_indi_info_t sos_call_indi_data;		
    gps_data_t gps_data;	

    memset(&send_data, 0, sizeof(send_data));
    memset(&sos_call_indi_data, 0, sizeof(sos_call_indi_data));
    memset(&gps_data,0,sizeof(gps_data));
	
    
	
    //get_current_gps_position_data(&gps_data);  //future todo
    memcpy(&sos_call_indi_data.gps_data, &gps_data, sizeof(gps_data));

    send_data.groupId = SAFETY_AND_CALL_SRV_TO_APP;
    send_data.cmdId = SOS_CALL_INDI;
    send_data.length = sizeof(sos_call_indi_data) + 4; //groupID and cmdId
    memcpy(&send_data.data.sos_call_indi_data, &sos_call_indi_data, sizeof(sos_call_indi_data));

    LOGI("send a send_sos_voice_call_indi_to_tcuapp len(%d)",send_data.length);

#if 0	 // future todo
    result = send_msgq_to_tcuapp(send_data);
    if(result !=0)
        LOGI("Fail to send a send_sos_voice_call_indi_to_tcuapp");		
#endif

    return result;	

}
int send_acn_voice_call_indi_to_tcuapp()
{
    int result = 0;
    tcuApp_cmdq_t send_data;

    airback_call_indi_info_t ariback_call_indi_data;		
    gps_data_t gps_data;	

    memset(&send_data, 0, sizeof(send_data));
    memset(&ariback_call_indi_data, 0, sizeof(ariback_call_indi_data));
    memset(&gps_data,0,sizeof(gps_data));

    LOGI("send a send_acn_voice_call_indi_to_tcuapp");
	
    //get_current_gps_position_data(&gps_data);  //future todo
    memcpy(&ariback_call_indi_data.gps_data, &gps_data, sizeof(gps_data));

    send_data.groupId = SAFETY_AND_CALL_SRV_TO_APP;
    send_data.cmdId = AIRBACK_CALL_INDI;
	
    send_data.length = sizeof(ariback_call_indi_data) + 4; //groupID and cmdId
    memcpy(&send_data.data.sos_call_indi_data, &ariback_call_indi_data, sizeof(ariback_call_indi_data));

    result = send_msgq_to_tcuapp(send_data);
    if(result !=0)
        LOGI("Fail to send a send_acn_voice_call_indi_to_tcuapp");		

    return result;	
}
int send_concierge_voice_call_indi_to_tcuapp()
{
    int result = 0;
    tcuApp_cmdq_t send_data;

    concierge_call_indi_info_t concierge_call_indi_data;
    gps_data_t gps_data;	

    memset(&send_data, 0, sizeof(send_data));
    memset(&concierge_call_indi_data, 0, sizeof(concierge_call_indi_data));
    memset(&gps_data,0,sizeof(gps_data));

    LOGI("send a send_concierge_voice_call_indi_to_tcuapp");
	
    //get_current_gps_position_data(&gps_data);  //future todo
    memcpy(&concierge_call_indi_data.gps_data, &gps_data, sizeof(gps_data));

    send_data.groupId = SAFETY_AND_CALL_SRV_TO_APP;
    send_data.cmdId = CONCIERGE_CALL_INDI;
	
    send_data.length = sizeof(concierge_call_indi_data) + 4; //groupID and cmdId
    memcpy(&send_data.data.sos_call_indi_data, &concierge_call_indi_data, sizeof(concierge_call_indi_data));

    result = send_msgq_to_tcuapp(send_data);
    if(result !=0)
        LOGI("Fail to send a send_concierge_voice_call_indi_to_tcuapp");		

    return result;	
}


void send_voice_call_end_to_tcuapp()
{
    int result = 0;
    tcuApp_cmdq_t send_data;

    memset(&send_data, 0, sizeof(send_data));

    LOGI("send a send_voice_call_end_to_tcuapp");

    send_data.groupId = SAFETY_AND_CALL_SRV_TO_APP;
    send_data.cmdId = CALL_END_IND;
	
    send_data.length = 4; //groupID and cmdId

    result = send_msgq_to_tcuapp(send_data);
    if(result !=0)
        LOGI("Fail to send a send_voice_call_end_to_tcuapp");	

}

void send_voice_call_est_indi_to_tcuapp()
{
    int result = 0;
    tcuApp_cmdq_t send_data;

    memset(&send_data, 0, sizeof(send_data));

    LOGI("send a send_voice_call_est_indi_to_tcuapp");

    send_data.groupId = SAFETY_AND_CALL_SRV_TO_APP;
    send_data.cmdId = CALL_EST_IND;
	
    send_data.length = 4; //groupID and cmdId

    result = send_msgq_to_tcuapp(send_data);
    if(result !=0)
        LOGI("Fail to send a send_voice_call_est_indi_to_tcuapp");	

}

boolean process_svt_start_req(tcuApp_cmdq_t *rcv_data)
{
    int interval;
    int duration;

    interval = rcv_data->data.svt_req.interval;
    duration = rcv_data->data.svt_req.duration;	
    LOGI("SVT REQ Interval(%d) Duration(%d)",interval,duration);

    return 0;	  
}

boolean  process_svt_stop_req(tcuApp_cmdq_t *rcv_data)
{
    LOGI("SVT STOP");	
    return 0;	
}



handler_type tcuapp_remote_handler_get(tcu_app_remote_cmd_type cmd)
{
  EXCEPTION_INIT
  handler_type result = NULL;
  unsigned int i;
  
  if(cmd >= REMOTE_ID_MAX) THROW();

  // search command to process
  for(i = 0; i < ARR_SIZE(remote_handler_tbl); i++)
  {
    if(cmd == remote_handler_tbl[i].cmd) break;
  }
  if(i >= ARR_SIZE(remote_handler_tbl))
  {
    LOGI("%s", "invalid cmd\n");
    THROW();
  }

  result = remote_handler_tbl[i].handler;

  CATCH_ERR()
  {
    result = NULL;
  }

  return result;
}



handler_type tcuapp_safety_handler_get(tcu_app_satety_cmd_type cmd)
{
  EXCEPTION_INIT
  handler_type result = NULL;
  unsigned int i;
  
  if(cmd >= SFY_AND_CALL_ID_MAX) THROW();

  // search command to process
  for(i = 0; i < ARR_SIZE(safety_handler_tbl); i++)
  {
    if(cmd == safety_handler_tbl[i].cmd) break;
  }
  if(i >= ARR_SIZE(safety_handler_tbl))
  {
    LOGI("%s", "invalid cmd\n");
    THROW();
  }

  result = safety_handler_tbl[i].handler;

  CATCH_ERR()
  {
    result = NULL;
  }

  return result;
}


handler_type tcuapp_infortain_handler_get(tcu_app_infortain_cmd_type cmd)
{
  EXCEPTION_INIT
  handler_type result = NULL;
  unsigned int i;
  
  if(cmd >= INFOTAIN_ID_MAX) THROW();

  // search command to process
  for(i = 0; i < ARR_SIZE(infortain_handler_tbl); i++)
  {
    if(cmd == infortain_handler_tbl[i].cmd) break;
  }
  if(i >= ARR_SIZE(infortain_handler_tbl))
  {
    LOGI("%s", "invalid cmd\n");
    THROW();
  }

  result = infortain_handler_tbl[i].handler;

  CATCH_ERR()
  {
    result = NULL;
  }

  return result;
}


// Process pkt from TCUApp(OBIGO) to TCU main(Service layer)
boolean process_tcuAppTcu_pkt(tcuApp_cmdq_t *rcv_data)
{
    EXCEPTION_INIT
    boolean ret = 0;

    handler_type handler = NULL;
	
    if(rcv_data == NULL)
    {
      LOGI("rcv_data packet is NULL");    	
      ret = -1;
      return ret;	  
    }
#if 1
    LOGI("TCUApp Pkt CMD(0x%x) Received",rcv_data->cmdId);   

	
    if(rcv_data->groupId == REMOTE_CONTROL_TO_FRAME)
    {
        handler = tcuapp_remote_handler_get(rcv_data->cmdId);		
        if(!handler)
        {
          LOGI("%s", "can't process cmd since there is no handler to process it\n");
          THROW();
        }
        handler(rcv_data);      
    }    

    else if(rcv_data->groupId == SAFETY_AND_CALL_SRV_TO_FRAME)
    {	
        handler = tcuapp_safety_handler_get(rcv_data->cmdId);		
        if(!handler)
        {
          LOGI("%s", "can't process cmd since there is no handler to process it\n");
          THROW();
        }

        handler(rcv_data);
      
    }	
    else if(rcv_data->groupId == SUPPLIMENTARY_SRV_TO_FRAME)
    {	
        handler = tcuapp_infortain_handler_get(rcv_data->cmdId);		
        if(!handler)
        {
          LOGI("%s", "can't process cmd since there is no handler to process it\n");
          THROW();
        }

        handler(rcv_data);
      
    }	
	
#endif

    CATCH_ERR()
    {
        ret = FALSE;
    }    
    
    
    return ret;	
}






