/*===================================================

                  PROCESS AVN PACKET


GENERAL DESCRIPTION

  Process Msg from MCU  to TCU  IF MGR thread 
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
#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>

#include <ql_oe.h>

#include "msg_if.h"
#include "tcu_interface.h"
#include "svclayer_ProcAvnPkt.h"
#include "svclayer_AvnIfMgr.h"


typedef struct
{  
    avn_cmd_type cmd;
    avn_handler_type handler;
} avn_cmd_processor_type;

typedef struct
{  
    tcuavnif_cmd_type cmd;
    tcuavnif_handler_type handler;
} tcuavnif_cmd_processor_type;

static BOOL dummy(avn_data_t *rcv_data);
static BOOL process_call_orig_cmd(avn_data_t *rcv_data);
static BOOL process_call_end_cmd(avn_data_t *rcv_data);
static BOOL process_nad_rf_info_req_cmd(avn_data_t *rcv_data);
static BOOL send_call_failure_indi(byte reason);

// AVN --- > AVN IF
avn_cmd_processor_type avn_handler_tbl[] =
{
    {AVN_CALL_ORIG_CMD,        				process_call_orig_cmd},
    {AVN_CALL_END_CMD,        				process_call_end_cmd},
    {AVN_NAD_CALL_STATUS,          			dummy},
    {AVN_NAD_SERVICE_STATUS,          		dummy},
    {AVN_NAD_STATIC_INFO_REQ,          		dummy},
    {AVN_NAD_RF_INFO_REQ,          			process_nad_rf_info_req_cmd},
    {AVN_ID_MAX,          					dummy}
};

static BOOL dummy2(avn_cmdq_t *rcv_data);
static BOOL process_tcuavnif_call_connecting_indi(avn_cmdq_t *rcv_data);
static BOOL process_tcuavnif_call_connected_indi(avn_cmdq_t *rcv_data);
static BOOL process_tcuavnif_call_failure_indi(avn_cmdq_t *rcv_data);
static BOOL process_tcuavnif_call_end_indi(avn_cmdq_t *rcv_data);
static BOOL process_tcuavnif_call_rf_info_indi(avn_cmdq_t *rcv_data);

//TCU --> AVN IF 
tcuavnif_cmd_processor_type tcuavnif_handler_tbl[] =
{
    {CALL_CONNECTING_INDI,        				process_tcuavnif_call_connecting_indi},
    {CALL_CONNECTED_INDI,        				process_tcuavnif_call_connected_indi},
    {CALL_FAILFURE_INDI,          					process_tcuavnif_call_failure_indi},
    {CALL_END_INDI,          						process_tcuavnif_call_end_indi},
    
    {NAD_CALL_STATE_INDI,          				dummy2},    
    {NAD_SERVICE_STATUS_INDI,          			dummy2},
    {NAD_STATIC_INFO_INDI,          				dummy2},
    {NAD_RF_INFO_INDI,          					process_tcuavnif_call_rf_info_indi},
	
    {MANUAL_DIAG_QUERY,dummy2},
    {PROVISIONING_PIN,dummy2},
    {SUBSCRIPTION_START,dummy2},
    {SUBSCRIPTION_STOP,dummy2},
    
    {DTC_INFORMATION,dummy2},
    {DIAG_ANALYSIS_INFO,dummy2},
    {SCHEDULED_DIAGNOSIS,dummy2},
    {ASCODE_INFORMATION,dummy2},
    {EXTEND_ASCODE_INFORMATION, dummy2},
    {LOCATION_SHARING_INFO, dummy2},
    {POI_INFORMATION, dummy2},
    {PROVISIONING_RESULT, dummy2},
    {SEURITY_AND_PROVISIONING, dummy2},
    {SUBSCRIPTION_CANCEL, dummy2},
    {SUBSCRIPTION_CLOSE, dummy2},
    {SUBSCRIPTION_HOLD, dummy2},
    {SUBSCRIPTION_HOLD_RELEASE, dummy2},
    {ERROR_INFORMATION, dummy2},
    {GENERAL_INFORMATION,  dummy2},
    {SUBSCRIPTION_RESULT, dummy2},
    {TCUAVNIF_ID_MAX, dummy2 }
};


int send_msgq_avnif_to_tcu(avn_cmdq_t send_data)
{
  int qid;
  int ret = 0;

  send_data.data_type = AVNIFMGR_TO_FRAME_TYPE;
  qid = msgget( (key_t)AVNIF2FRAME_MSGQ_KEY, IPC_CREAT | 0666);
  
  if(qid<0)
  {
    LOGI("send_msgq_avnif_to_tcu msgget error\n");
    ret = -1;
    return ret;
  }

  if(msgsnd( qid, &send_data, sizeof(send_data) - sizeof(long), 0)<0)
  {
    LOGI("send_msgq_avnif_to_tcu msgsnd error\n");
    ret = -1;
  }

  //usleep(ccu_nad_wait);
  LOGI("send_msgq() key(%d) ret =%d",AVNIF2FRAME_MSGQ_KEY, ret);
  
  return ret;
}

    unsigned short  cmdId;
    int    length;	
    union {
            // AVN IF mgr --- > TCU Main
            call_origination_t origin_indi;
 			
             // TCU Main --- > AVN IF mgr
            call_connecting_t connecting_indi;
            call_connected_t  connected_indi;			
            call_failure_reason_t fail_reason;		
            nad_rf_info_t rf_info;	
			
            poi_info_t   poi;
    } data;


BOOL process_call_orig_cmd(avn_data_t *rcv_data)
{
    int result = 0;
    avn_cmdq_t send_data;

    memset(&send_data, 0, sizeof(send_data));  

    send_data.cmdId = 	AVNIF_CALL_ORIG_CMD;
    send_data.data.origin_indi.call_type = rcv_data->data.origin_indi.call_type;
    send_data.length = sizeof(call_origination_t);
	
    LOGI("send AVNIF_CALL_ORIG_CMD to TCU main( len=%d)",send_data.length);

    result = send_msgq_avnif_to_tcu(send_data);
    if(result !=0)
        LOGI("Fail send_msgq_avnif_to_tcu");		
}

BOOL process_call_end_cmd(avn_data_t *rcv_data)
{
    int result = 0;
    avn_cmdq_t send_data;

    memset(&send_data, 0, sizeof(send_data));  

    send_data.cmdId = AVNIF_CALL_END_CMD;
    send_data.length = 0;
	
    LOGI("send AVNIF_CALL_END_CMD to TCU main( len=%d)",send_data.length);

    result = send_msgq_avnif_to_tcu(send_data);
    if(result !=0)
        LOGI("Fail send_msgq_avnif_to_tcu");	

    //Send End indi directry to AVN...Sometimes..Modem side could not orig Call..!!!
    //CallCb function also send call end indi to AVN IF both...for clarify
    send_voice_call_end_to_avnif(); 	
}


BOOL process_nad_rf_info_req_cmd(avn_data_t *rcv_data)
{
    int result = 0;
    avn_cmdq_t send_data;

    memset(&send_data, 0, sizeof(send_data));  

    send_data.cmdId = AVNIF_NAD_RF_INFO_REQ;
    send_data.length = 0;
	
    LOGI("send AVNIF_NAD_RF_INFO_REQ to TCU main( len=%d)",send_data.length);

    result = send_msgq_avnif_to_tcu(send_data);
    if(result !=0)
        LOGI("Fail send_msgq_avnif_to_tcu");		
}


BOOL process_tcuavnif_call_connecting_indi(avn_cmdq_t *rcv_data)
{

    avn_data_t avn_data;
    BOOL ret = TRUE;	
    int sent = 0;	
    int length = 0;	

    memset(&avn_data,0,sizeof(avn_data));

    LOGI("process_tcuavnif_call_connecting_indi");

    avn_data.sof = SOFRAME_AVN;
    avn_data.cmdIdH = 0x00;
    avn_data.cmdIdL = 0x00;  //future todo
    avn_data.lenth_high = 0x00;
    length = sizeof(call_connecting_t) + DEFAULT_PKT_LENGTH;	
    avn_data.lenth_low = sizeof(call_connecting_t);	//Skip SOF and EOF	
    avn_data.data.calling_indi.call_type = rcv_data->data.connecting_indi.call_type;	
    avn_data.eof = EOFRAME_AVN;


    sent = send_to_serial(&avn_data, length);

    if(sent > 0)
    {
        LOGI("Send Connecting Indi pkt to serial, size=%d",sent);
    }
    else
    {
        LOGI("Fail Send pkt to serial"); 
        ret = FALSE;				
    }


    return ret;	
	
}

BOOL process_tcuavnif_call_connected_indi(avn_cmdq_t *rcv_data)
{
    avn_data_t avn_data;
    BOOL ret = TRUE;	
    int sent = 0;	
    int length = 0;	

    //Already App know cal type. If they need it, You can insert the calltype info to payload
    memset(&avn_data,0,sizeof(avn_data));

    LOGI("process_tcuavnif_call_connected_indi");

    avn_data.sof = SOFRAME_AVN;
    avn_data.cmdIdH = 0x00;
    avn_data.cmdIdL = 0x01;  //future todo
    avn_data.lenth_high = 0x00;
    length = DEFAULT_PKT_LENGTH;	
    avn_data.lenth_low = 0;	
    avn_data.data.no_data_eof = EOFRAME_AVN;

    sent = send_to_serial(&avn_data, length);
	
	
    LOGI("Send Connected Indi pkt to serial, size=%d",sent);	
    if(sent < 0)
    {
        LOGI("Fail Send pkt to serial");   
        ret = FALSE;				
    }

    return ret;	
	
}

BOOL process_tcuavnif_call_failure_indi(avn_cmdq_t *rcv_data)
{
    avn_data_t avn_data;
    BOOL ret = TRUE;	
    int sent = 0;	
    int length = 0;	

    memset(&avn_data,0,sizeof(avn_data));

    LOGI("process_tcuavnif_call_failure_indi");

    avn_data.sof = SOFRAME_AVN;
    avn_data.cmdIdH = 0x00;
    avn_data.cmdIdL = 0x02;  //future todo
    avn_data.lenth_high = 0;
    avn_data.lenth_low = sizeof(call_failure_reason_t);	
    avn_data.data.fail_reason.reason = rcv_data->data.fail_reason.reason;
    avn_data.eof = EOFRAME_AVN;

    length = sizeof(call_failure_reason_t) + DEFAULT_PKT_LENGTH;

    sent = send_to_serial(&avn_data, length);

    LOGI("Send Call Failure Indi pkt to serial, size=%d",sent);	
    if(sent < 0)
    {
        LOGI("Fail Send pkt to serial"); 
        ret = FALSE;		
    }
    
    return ret;	
	
}

BOOL process_tcuavnif_call_end_indi(avn_cmdq_t *rcv_data)
{
    avn_data_t avn_data;
    BOOL ret = TRUE;	
    int sent = 0;	
    int length = 0;	

    memset(&avn_data,0,sizeof(avn_data));

    LOGI("process_tcuavnif_call_end_indi");

    avn_data.sof = SOFRAME_AVN;
    avn_data.cmdIdH = 0x00;
    avn_data.cmdIdL = 0x03;  //future todo
    avn_data.lenth_high = 0x00;
    length = DEFAULT_PKT_LENGTH;	
    avn_data.lenth_low = 0;	
    avn_data.data.no_data_eof = EOFRAME_AVN;

    sent = send_to_serial(&avn_data, length);

    LOGI("Send Call End pkt to serial, size=%d",sent);	
    if(sent < 0)
    {
        LOGI("Fail Send pkt to serial"); 
        ret = FALSE;				
    }

    return ret;	
	
}


BOOL process_tcuavnif_call_rf_info_indi(avn_cmdq_t *rcv_data)
{
    avn_data_t avn_data;    
    BOOL ret = TRUE;
    int sent = 0;	
    int length = 0;	
    byte nt_type = RAT_LTE;	
	
    LOGI("process_tcuavnif_call_rf_info_indi");

    memset(&avn_data,0,sizeof(avn_data));


    avn_data.sof = SOFRAME_AVN;
    avn_data.cmdIdH = 0x00;
    avn_data.cmdIdL = 0xC3;  
    avn_data.lenth_high = 0x00;
    length = sizeof(nad_rf_info_t) + DEFAULT_PKT_LENGTH;	
    avn_data.lenth_low = sizeof(nad_rf_info_t);	//Skip SOF and EOF
    avn_data.data.rf_info.network_type = rcv_data->data.rf_info.network_type;	
    avn_data.eof = EOFRAME_AVN;


    sent = send_to_serial(&avn_data, length);

    if(sent > 0)
    {
        LOGI("Send pkt to serial, size=%d",sent);
    }
    else
    {
        LOGI("Fail Send pkt to serial"); 
        ret = FALSE;				
    }

    return ret;	
}

static BOOL dummy(avn_data_t *rcv_data)
{
  LOGI("dummy called");
  return TRUE;
}

static BOOL dummy2(avn_cmdq_t *rcv_data)
{
  LOGI("dummy called");
  return TRUE;
}

avn_handler_type avn_handler_get(avn_cmd_type cmd)
{
  EXCEPTION_INIT
  avn_handler_type result = NULL;
  unsigned int i;
  
  if(cmd >= AVN_ID_MAX) THROW();

  // search command to process
  for(i = 0; i < ARR_SIZE(avn_handler_tbl); i++)
  {
    if(cmd == avn_handler_tbl[i].cmd) break;
  }
  if(i >= ARR_SIZE(avn_handler_tbl))
  {
    LOGI("%s", "invalid cmd\n");
    THROW();
  }

  result = avn_handler_tbl[i].handler;

  CATCH_ERR()
  {
    result = NULL;
  }

  return result;
}



//Process the packet from AVN side through USBtoSerial communication
int process_avn_pkt(avn_data_t* avn_pkt)
{
    EXCEPTION_INIT
    BOOL ret = 0;

    avn_handler_type handler = NULL;
	
    if(avn_pkt == NULL)
    {
      LOGI("rcv_data packet is NULL");    	
      ret = -1;
      return ret;	  
    }
#if 1
    LOGI("AVN Pkt CMDL(0x%x) Received",avn_pkt->cmdIdL);   

	
    handler = avn_handler_get(avn_pkt->cmdIdL);		
    if(!handler)
    {
      LOGI("%s", "can't process cmd since there is no handler to process it\n");
      THROW();
    }
    handler(avn_pkt);      

#endif

    CATCH_ERR()
    {
        ret = FALSE;
    }    
    
    
    return ret;	
}


tcuavnif_handler_type tcuavnif_handler_get(tcuavnif_cmd_type cmd)
{
  EXCEPTION_INIT
  tcuavnif_handler_type result = NULL;
  unsigned int i;
  
  if(cmd >= AVN_ID_MAX) THROW();

  // search command to process
  for(i = 0; i < ARR_SIZE(tcuavnif_handler_tbl); i++)
  {
    if(cmd == tcuavnif_handler_tbl[i].cmd) break;
  }
  if(i >= ARR_SIZE(tcuavnif_handler_tbl))
  {
    LOGI("%s", "invalid cmd\n");
    THROW();
  }

  result = tcuavnif_handler_tbl[i].handler;

  CATCH_ERR()
  {
    result = NULL;
  }

  return result;
}


BOOL process_tcuAvnIfMgr_pkt(avn_cmdq_t *rcv_data)
{
    EXCEPTION_INIT
    boolean ret = 0;

    tcuavnif_handler_type handler = NULL;
	
    if(rcv_data == NULL)
    {
      LOGI("rcv_data packet is NULL");    	
      ret = -1;
      return ret;	  
    }
#if 1
    LOGI("AVN Pkt CMD(0x%x) Received",rcv_data->cmdId);   

	
    handler = tcuavnif_handler_get(rcv_data->cmdId);		
    if(!handler)
    {
      LOGI("%s", "can't process cmd since there is no handler to process it\n");
      THROW();
    }
    handler(rcv_data);      

#endif

    CATCH_ERR()
    {
        ret = FALSE;
    }    
    
    
    return ret;	
}



