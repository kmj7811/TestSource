/*===================================================

                  PROCESS TCU AVN PACKET


GENERAL DESCRIPTION

  Process Msg from AVN  to TCU  thread 
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
#include "svclayer_ProcTcuAvnIfPkt.h"
#include "svclayer_ProcTcuAppPkt.h"
#include "svclayer_TcuMain.h"


typedef struct
{  
    avniftcu_cmd_type cmd;
    avniftcu_handler_type handler;
} avniftcu_cmd_processor_type;



static BOOL dummy(avn_cmdq_t *rcv_data);

static BOOL process_avniftcu_call_orig_cmd(avn_cmdq_t *rcv_data);
static BOOL process_avniftcu_call_end_cmd(avn_cmdq_t *rcv_data);
static BOOL process_avniftcu_nad_rf_info_req_cmd(avn_cmdq_t *rcv_data);



avniftcu_cmd_processor_type avniftcu_handler_tbl[] =
{
    {AVNIF_CALL_ORIG_CMD,        				process_avniftcu_call_orig_cmd},
    {AVNIF_CALL_END_CMD,        					process_avniftcu_call_end_cmd	},
    {AVNIF_NAD_CALL_STATUS,        				dummy},
    {AVNIF_NAD_SERVICE_STATUS,        			dummy},
    {AVNIF_NAD_STATIC_INFO_REQ,        			dummy},
    {AVNIF_NAD_RF_INFO_REQ,        				process_avniftcu_nad_rf_info_req_cmd},
    {AVNIF_MANUAL_DIAG_QUERY,        			dummy},
    {AVNIF_PROVISIONING_PIN,        				dummy},
    {AVNIF_SUBSCRIPTION_START,        			dummy},
    {AVNIF_SUBSCRIPTION_STOP,        			dummy},
    {AVNIF_TCU_ID_MAX,        					dummy}
};

BOOL process_avniftcu_call_orig_cmd(avn_cmdq_t *rcv_data)
{
    BOOL ret = TRUE;
    byte call_state;
    byte call_type = NCALL_TYPE;
    int callflag = 0;	
	
    LOGI("process_avniftcu_call_orig_cmd");
    call_state = nad_get_current_call_state();

    if(rcv_data == NULL)
    {
        LOGI("NULL data");
        return FALSE;		
    }
	
    if(call_state != CALL_IDLE)
    {
        LOGI("Now Call Is Progressing, Ignore Orign Cmd");
        //send_voice_call_failure_indi_to_avnif(LINE_BUSY_REASON);
        return FALSE;		
    }

    call_type = rcv_data->data.origin_indi.call_type;
    LOGI("Orign call type(%d)",call_type);	

#if 0	
    switch(call_type)
    {
        case CCALL_TYPE:
        {
            callflag = voice_origination(call_type);
            if(callflag < 0 )
            {
                LOGI("Orign call Fail");
                send_voice_call_failure_indi_to_avnif(MODME_ERR_REASON);				
            }	
            else
            {
                LOGI("Send CCAL Call Connecting Indi");
                send_voice_call_connecting_indi_to_avnif(CCALL_TYPE);
            }
            break;
        }
        case ECALL_TYPE:
        {
            callflag = voice_origination(call_type);
            if(callflag < 0 )
            {
                LOGI("Orign call Fail");
                send_voice_call_failure_indi_to_avnif(MODME_ERR_REASON);				
            }				
            else
            {
                LOGI("Send ECAL Call Connecting Indi");
                send_voice_call_connecting_indi_to_avnif(ECALL_TYPE);
            }
            break;
        }
        case BCALL_TYPE:
        {
            callflag = voice_origination(call_type);
            if(callflag < 0 )
            {
                LOGI("Orign call Fail");
                send_voice_call_failure_indi_to_avnif(MODME_ERR_REASON);				
            }				
            else
            {
                LOGI("Send BCAL Call Connecting Indi");
                send_voice_call_connecting_indi_to_avnif(BCALL_TYPE);
            }
            break;
        }
        case ACALL_TYPE:
        {
            callflag = voice_origination(call_type);
            if(callflag < 0 )
            {
                LOGI("Orign call Fail");
                send_voice_call_failure_indi_to_avnif(MODME_ERR_REASON);				
            }				
            else
            {
                LOGI("Send ACAL Call Connecting Indi");
                send_voice_call_connecting_indi_to_avnif(ACALL_TYPE);
            }
            break;
        }
        case NCALL_TYPE:
        {
            callflag = voice_origination(call_type);
            if(callflag < 0 )
            {
                LOGI("Orign call Fail");
                send_voice_call_failure_indi_to_avnif(MODME_ERR_REASON);				
            }				
            else
            {
                LOGI("Send NCAL Call Connecting Indi");
                send_voice_call_connecting_indi_to_avnif(NCALL_TYPE);
            }
            break;
        }
        default:
            break;			
    }
#endif	
    return ret;
}

BOOL process_avniftcu_call_end_cmd(avn_cmdq_t *rcv_data)
{
    BOOL ret = TRUE;
    LOGI("process_avnif_call_end_cmd");
    ret = voice_call_end();

    if(ret < 0 )
    {
    	 LOGI("END CALL fail");
        ret = FALSE;		
    }
    return ret;
}

BOOL process_avniftcu_nad_rf_info_req_cmd(avn_cmdq_t *rcv_data)
{
    avn_cmdq_t send_data;    
    BOOL ret = TRUE;
    int result = 0;	
    int sent = 0;	
    int length = 0;	
    byte nt_type = RAT_LTE;	
	
    LOGI("process_avniftcu_nad_rf_info_req_cmd");
    int i = 0;
    ST_CellInfo  info = {0};
    ret = QL_NW_GetServingCell(&info);
    for(i=0; i<info.validCnt; i++)
    {
        printf("ServingCell[%d]: rat=%s, mcc=%d, mnc=%d, lac = 0x%X, ci=0x%x\n", 
            i, 
            info.cellInfo[i].rat, 
            info.cellInfo[i].mcc, 
            info.cellInfo[i].mnc, 
            info.cellInfo[i].lac, 
            info.cellInfo[i].cid);
		
    if(strcmp("LTE", info.cellInfo[i].rat) == 0)
        nt_type = RAT_LTE;
    else if(strcmp("UMTS", info.cellInfo[i].rat) == 0)
        nt_type = RAT_WCDMA;
    else if(strcmp("GSM", info.cellInfo[i].rat) == 0)
        nt_type = RAT_GSM;		
    }

    memset(&send_data, 0, sizeof(send_data));   

    send_data.cmdId = NAD_RF_INFO_INDI;
    send_data.data.rf_info.network_type = nt_type;		
    send_data.length = sizeof(nad_rf_info_t);

    result = send_msgq_to_avnif(send_data);
    if(result !=0)
    {
        LOGI("Fail to send a send_msgq_to_avnif");
        ret = FALSE;		
    }


    return ret;	
}
static BOOL dummy(avn_cmdq_t *rcv_data)
{
  LOGI("dummy called\n");
  return TRUE;
}


int send_msgq_to_avnif(avn_cmdq_t send_data)
{
  int qid;
  int ret = 0;

  send_data.data_type = FRAME_TO_AVNIFMGR_TYPE;
  qid = msgget( (key_t)FRAME2AVNIF_MSGQ_KEY, IPC_CREAT | 0666);
  
  if(qid<0)
  {
    LOGI("send_msgq_to_avnif msgget error\n");
    ret = -1;
    return ret;
  }

  if(msgsnd( qid, &send_data, sizeof(send_data) - sizeof(long), 0)<0)
  {
    LOGI("send_msgq_to_avnif msgsnd error\n");
    ret = -1;
  }

  LOGI("send_msgq() key(%d) ret =%d",FRAME2AVNIF_MSGQ_KEY, ret);
  
  return ret;
}



avniftcu_handler_type avniftcu_handler_get(avniftcu_cmd_type cmd)
{
  EXCEPTION_INIT
  avniftcu_handler_type result = NULL;
  unsigned int i;
  
  if(cmd >= AVNIF_TCU_ID_MAX) THROW();

  // search command to process
  for(i = 0; i < ARR_SIZE(avniftcu_handler_tbl); i++)
  {
    if(cmd == avniftcu_handler_tbl[i].cmd) break;
  }
  if(i >= ARR_SIZE(avniftcu_handler_tbl))
  {
    LOGI("%s", "invalid cmd\n");
    THROW();
  }

  result = avniftcu_handler_tbl[i].handler;

  CATCH_ERR()
  {
    result = NULL;
  }

  return result;
}

void send_voice_call_connecting_indi_to_avnif(byte call_type)
{
    int result = 0;
    avn_cmdq_t send_data;

    memset(&send_data, 0, sizeof(send_data));   

    send_data.cmdId = CALL_CONNECTING_INDI;
    send_data.data.connecting_indi.call_type = call_type;
		
    send_data.length = sizeof(call_connecting_t);

    LOGI("send a send_voice_call_connecting_indi_to_avnif len(%d)",send_data.length);

    result = send_msgq_to_avnif(send_data);
    if(result !=0)
        LOGI("Fail to send a send_msgq_to_avnif");		

}

void send_voice_call_end_to_avnif()
{
    int result = 0;
    avn_cmdq_t send_data;

    memset(&send_data, 0, sizeof(send_data));   

    send_data.cmdId = CALL_END_INDI;		
    send_data.length = 0;

    LOGI("send a send_voice_call_end_to_avnif len(%d)",send_data.length);

    result = send_msgq_to_avnif(send_data);
    if(result !=0)
        LOGI("Fail to send a send_msgq_to_avnif");		

}

void send_voice_call_connected_indi_to_avnif(byte call_type)
{
    int result = 0;
    avn_cmdq_t send_data;

    memset(&send_data, 0, sizeof(send_data));   

    send_data.cmdId = CALL_CONNECTED_INDI;	
    send_data.data.connected_indi.call_type = call_type;	
    send_data.length = sizeof(call_connected_t);


    LOGI("send a send_voice_call_connected_indi_to_avnif len(%d)",send_data.length);

    result = send_msgq_to_avnif(send_data);
    if(result !=0)
        LOGI("Fail to send a send_msgq_to_avnif");		

}

void send_voice_call_failure_indi_to_avnif(byte reason)
{
    int result = 0;
    avn_cmdq_t send_data;

    memset(&send_data, 0, sizeof(send_data));   

    send_data.cmdId = CALL_FAILFURE_INDI;
    send_data.data.fail_reason.reason= reason;
		
    send_data.length = sizeof(call_failure_reason_t);

    LOGI("send a send_voice_call_failure_indi_to_avnif len(%d)",send_data.length);

    result = send_msgq_to_avnif(send_data);
    if(result !=0)
        LOGI("Fail to send a send_msgq_to_avnif");		

}

// Process pkt from AVNIF to TCU main(Service layer)
boolean process_avnIfTcu_pkt(avn_cmdq_t *rcv_data)
{
    EXCEPTION_INIT
    boolean ret = 0;

    avniftcu_handler_type handler = NULL;
	
    if(rcv_data == NULL)
    {
      LOGI("rcv_data packet is NULL");    	
      ret = -1;
      return ret;	  
    }
#if 1
    LOGI("AVN Pkt CMD(0x%x) Received",rcv_data->cmdId);   

	
    handler = avniftcu_handler_get(rcv_data->cmdId);		
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






