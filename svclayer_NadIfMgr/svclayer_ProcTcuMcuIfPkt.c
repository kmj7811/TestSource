/*===================================================

                  PROCESS TCUMCU PACKET


GENERAL DESCRIPTION

  Process Msg from TCU  to MCUIF   
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

#include "msg_if.h"
#include "tcu_interface.h"
#include "svclayer_ProcTcuMcuIfPkt.h"
#include "svclayer_ProcTcuAppPkt.h"
#include "svclayer_ProcTcuAvnIfPkt.h"
#include "svclayer_TcuMain.h"
#include "timer_if.h"

#include "svclayer_NadIfPkt.h"


static boolean mcu_dummy(mcu_cmdq_t *rcv_data);
static boolean process_ecall_btn_indi_to_tcu(mcu_cmdq_t *rcv_data);
static boolean process_bcall_btn_indi_to_tcu(mcu_cmdq_t *rcv_data);
static boolean process_ccall_btn_indi_to_tcu(mcu_cmdq_t *rcv_data);

//MCU IF mgr -- > TCU
mcuiftcu_cmd_processor_type mcuiftcu_handler_tbl[] =
{
    {E_CALL_BTN_INDI_TO_TCU,        		process_ecall_btn_indi_to_tcu},
    {B_CALL_BTN_INDI_TO_TCU,       		process_bcall_btn_indi_to_tcu},
    {C_CALL_BTN_INDI_TO_TCU,     		process_ccall_btn_indi_to_tcu},
    {A_CALL_BTN_INDI_TO_TCU,     		mcu_dummy},
    {MCU_ID_MAX,          				mcu_dummy}
};


static boolean mcu_dummy(mcu_cmdq_t *rcv_data)
{
  LOGI("dummy called\n");
  return TRUE;
}

int send_msgq_to_mcuif(mcu_cmdq_t send_data)
{
  int qid;
  int ret = 0;

  send_data.data_type = FRAME_TO_MCUIFMGR_TYPE;
  qid = msgget( (key_t)FRAME2MCUIF_MSGQ_KEY, IPC_CREAT | 0666);
  
  if(qid<0)
  {
    LOGI("send_msgq_to_mcuif msgget error\n");
    ret = -1;
    return ret;
  }

  if(msgsnd( qid, &send_data, sizeof(send_data) - sizeof(long), 0)<0)
  {
    LOGI("send_msgq_to_mcuif msgsnd error\n");
    ret = -1;
  }

  //usleep(ccu_nad_wait);
  LOGI("send_msgq() key(%d) ret =%d",FRAME2MCUIF_MSGQ_KEY, ret);
  
  return ret;
}

int send_voice_call_result_indi_to_mcuif(byte call_status)
{
    mcu_cmdq_t send_data;
    int send_result = 0;
	
    send_data.cmdId = 	CALL_RESULT_INDI_TO_MCU;	
    send_data.length = sizeof(call_type_status_t);
    send_data.data.csts.call_status = call_status;
	
    send_result =  send_msgq_to_mcuif(send_data);
	
    if(send_result !=0)
    {
        LOGI("Msg Q send Fail"); 
    }	
    return send_result;	
}
 
int send_voice_call_end_to_mcuif()
{
    mcu_cmdq_t send_data;
    int send_result = 0;
	
    send_data.cmdId = 	CALL_END_INDI_TO_MCU;
	
    send_result =  send_msgq_to_mcuif(send_data);
	
    if(send_result !=0)
    {
        LOGI("Msg Q send Fail"); 
    }	
    return send_result;	
}


boolean process_ecall_btn_indi_to_tcu(mcu_cmdq_t *rcv_data)
{
    boolean ret = 0;
    int handler = 0;
    int call_state;
	
    ///////////////////////////////
    //set_BtnType(ECALL_BTN);
    ret = send_start_call_indi(ECALL_TYPE);
    //////////////////////////////
#if 0
    handler = nad_get_voice_handler_allocated();
    if(handler !=TRUE)
    {
        LOGI("Voice Call Handler allocation Fail"); 
        send_voice_call_result_indi_to_mcuif(CALL_STATUS_VOICE_HANDLER_FAIL);
        //Need check..Call fail is right 
        //Or..Need some other indicator
        //send_voice_call_failure_indi_to_avnif(CALL_NO_RESOURCE);				   
    }
    else
    {  
        call_state = nad_get_current_call_state();
        if(call_state != CALL_END)
        {
            LOGI("NOW Call Progressing Ignore second input");
        }
        else
        {
            set_btn_call_type(ECALL_TYPE);
            ret = voice_origination(ECALL_TYPE); 
		
            if(ret != 0)
            {
                send_voice_call_result_indi_to_mcuif(CALL_ORIG_FAIL);
                LOGI("Fail to origination E-Call");	
            }
            else
            {
                LOGI("ECALL ORIG");
                //send_sos_voice_call_indi_to_tcuapp();
                send_voice_call_connecting_indi_to_avnif(ECALL_TYPE); 
                //start timer					
                start_tcu_call_active_wait_timer(CALL_CONECT_WAIT_TIME,0);					 				
            }			    
        }
    
    }
#endif	
    return ret;	  
}

boolean process_bcall_btn_indi_to_tcu(mcu_cmdq_t *rcv_data)
{
    boolean ret = 0;
    int handler = 0;
    int call_state;

    ///////////////////////////////
    //set_BtnType(BCALL_BTN);
    ret = send_start_call_indi(BCALL_TYPE);
    //////////////////////////////
		
#if 0	
    handler = nad_get_voice_handler_allocated();
    if(handler !=TRUE)
    {
        LOGI("Voice Call Handler allocation Fail"); 
        send_voice_call_result_indi_to_mcuif(CALL_STATUS_VOICE_HANDLER_FAIL);
        //Need check..Call fail is right 
        //Or..Need some other indicator
        //send_voice_call_failure_indi_to_avnif(CALL_NO_RESOURCE);				   
    }
    else
    {  
        call_state = nad_get_current_call_state();
        if(call_state != CALL_END)
        {
            LOGI("NOW Call Progressing Ignore second input");
        }
        else
        {
            set_btn_call_type(BCALL_TYPE);
            ret = voice_origination(BCALL_TYPE); 
		
            if(ret != 0)
            {
                send_voice_call_result_indi_to_mcuif(CALL_ORIG_FAIL);
                LOGI("Fail to origination B-Call");	
            }
            else
            {
                LOGI("BCALL ORIG");
                //send_sos_voice_call_indi_to_tcuapp();
                send_voice_call_connecting_indi_to_avnif(BCALL_TYPE); 
                //start timer					
                start_tcu_call_active_wait_timer(CALL_CONECT_WAIT_TIME,0);					 				
            }			    
        }
    
    }
#endif	
    return ret;	  
}

boolean process_ccall_btn_indi_to_tcu(mcu_cmdq_t *rcv_data)
{
    boolean ret = 0;
    int handler = 0;
    int call_state;

    ///////////////////////////////
    //set_BtnType(CCALL_BTN);
    ret = send_start_call_indi(CCALL_TYPE);
    //////////////////////////////
#if 0	
    #ifdef FEATURE_CONCIERGE_MO_CALL
    {
        handler = nad_get_voice_handler_allocated();

        if(handler !=TRUE)
        {
            LOGI("Voice Call Handler allocation Fail"); 
            send_voice_call_result_indi_to_mcuif(CALL_STATUS_VOICE_HANDLER_FAIL);
            //Need check..Call fail is right 
            //Or..Need some other indicator
            //send_voice_call_failure_indi_to_avnif(CALL_NO_RESOURCE);				   
        }
        else
        {  
            call_state = nad_get_current_call_state();
            if(call_state != CALL_END)
            {
                LOGI("NOW Call Progressing Ignore second input");
            }
            else
            {
                set_btn_call_type(CCALL_TYPE);
                ret = voice_origination(CCALL_TYPE); 
    		
                if(ret != 0)
                {
                    send_voice_call_result_indi_to_mcuif(CALL_ORIG_FAIL);
                    LOGI("Fail to origination C-Call");	
                }
                else
                {
                    LOGI("CCALL ORIG");
                    //send_sos_voice_call_indi_to_tcuapp();
                    send_voice_call_connecting_indi_to_avnif(CCALL_TYPE); 
                    //start timer					
                    start_tcu_call_active_wait_timer(CALL_CONECT_WAIT_TIME,0);					 				
                }			    
            }
    
        }
    }		 
    #else
    {
        handler = nad_get_voice_handler_allocated();
        if(handler !=TRUE)
        {
            LOGI("Voice Call Handler allocation Fail"); 
            send_voice_call_result_indi_to_mcuif(CALL_STATUS_VOICE_HANDLER_FAIL);
            //Need check..Call fail is right 
            //Or..Need some other indicator
            send_voice_call_failure_indi_to_avnif(CALL_NO_RESOURCE);				   
            
        }
        else
        {  
            send_concierge_voice_call_indi_to_tcuapp();  
            send_voice_call_connecting_indi_to_avnif(CALL_TYPE_CONCIERGE);
            //start timer					
            start_tcu_call_active_wait_timer(CALL_CONECT_WAIT_TIME,0);					 
			
        }
    }		 
    #endif		 
#endif

    return ret;	  
}

mcuiftcu_handler_type mcuiftcu_handler_get(mcuiftcu_cmd_type cmd)
{
  EXCEPTION_INIT
  mcuiftcu_handler_type result = NULL;
  unsigned int i;
  
  if(cmd >= MCU_ID_MAX) THROW();

  // search command to process
  for(i = 0; i < ARR_SIZE(mcuiftcu_handler_tbl); i++)
  {
    if(cmd == mcuiftcu_handler_tbl[i].cmd) break;
  }
  if(i >= ARR_SIZE(mcuiftcu_handler_tbl))
  {
    LOGI("%s", "invalid cmd\n");
    THROW();
  }

  result = mcuiftcu_handler_tbl[i].handler;

  CATCH_ERR()
  {
    result = NULL;
  }

  return result;
}

boolean process_mcuIfTcu_pkt(mcu_cmdq_t *rcv_data)
{
    EXCEPTION_INIT
    boolean ret = 0;

    mcuiftcu_handler_type mcu_handler = NULL;
	
    if(rcv_data == NULL)
    {
      LOGI("rcv_data packet is NULL");    	
      ret = -1;
      return ret;	  
    }
#if 1
    LOGI("MCU Pkt CMD(0x%x) Received",rcv_data->cmdId);   

    mcu_handler = mcuiftcu_handler_get(rcv_data->cmdId);		
    if(!mcu_handler)
    {
        LOGI("%s", "can't process cmd since there is no handler to process it\n");
        THROW();
    }
    mcu_handler(rcv_data);      

    CATCH_ERR()
    {
        ret = FALSE;
    }                    
	
#endif
    
    return ret;	
}






