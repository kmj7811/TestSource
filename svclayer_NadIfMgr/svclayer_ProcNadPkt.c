/*===================================================

                  PROCESS APP PACKET


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
#include "ql_oe.h"

#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>

//#include "common.h"
#include "tcu_interface.h"
#include "svclayer_ProcNadPkt.h"
//#include "dbus_c_if.h"
//#include "dbus/server.h"

#include "svclayer_ClientApi.h"



static BOOL dummy(msg_t *rcv_data);
static BOOL process_start_call_req(msg_t *rcv_data);
static BOOL process_call_status_indi(msg_t *rcv_data);
static BOOL process_call_end_indi(msg_t *rcv_data);
static BOOL process_call_failure_indi(msg_t *rcv_data);
static BOOL process_nw_status_indi(msg_t *rcv_data);
static BOOL process_gps_location_info_indi(msg_t *rcv_data);
static BOOL process_collide_info_indi(msg_t *rcv_data);

cmd_processor_type handler_tbl[] =
{
    {START_CALL_REQ,        				process_start_call_req},
    {CALL_STATUS_INDI,        				process_call_status_indi},
    {CALL_END_INDICATION,			          		process_call_end_indi},
    {CALL_FAILURE_INDI,          			process_call_failure_indi},
    {DATA_NW_STATUS_INDI,         	 		process_nw_status_indi},
    {GPS_INFO_INDI,          				process_gps_location_info_indi},
    {COLLIDE_INFO_INDI,          			process_collide_info_indi},
    {ID_MAX,          						dummy}
};

/////////////////////////////////////////////////////////////////////////

static BOOL dummy2(msg_t *rcv_data);
static BOOL process_nad_info_indi(msg_t *rcv_data);
static BOOL process_nad_nw_info_indi(msg_t *rcv_data);


cc_cmd_processor_type cc_handler_tbl[] =
{
    {NAD_INFO_IND,                    process_nad_info_indi},
    {NAD_NW_INFO_IND,               process_nad_nw_info_indi},
    {CC_ID_MAX,                   dummy2}
};


////////////////////////////////////////////////////////////////////////////


BOOL process_start_call_req(msg_t *rcv_data)
{
    BOOL res = TRUE;
    byte btn_type = 0;

    LOGI("Received CALL BTN Indi");

    if(rcv_data == NULL)
    {
    	LOGI("No Rcv Data");
    	return FALSE;
    }

/*
  ECALL_TRIGGER=2;
  BCALL_TRIGGER=3;
  CCALL_TRIGGER=4;
*/
    btn_type = rcv_data->data.start_call.call_type;
    LOGI("BTN call type is %d", btn_type);

#if 0
// temp_call
{
	if(btn_type == ECALL_TYPE)
	{
            int32_t value = 0x00; //call orig
            char* dir_num = "01024647811";
            svclayer_nad_set_command3(value, dir_num);
	}
}

// temp_call
{
	if(btn_type == BCALL_TYPE)
	{
            int32_t value = 0x01; //call accep
            
            svclayer_nad_set_command3(value, NULL);
	}
}

// temp_call
{
	if(btn_type == CCALL_TYPE)
	{
            int32_t value = 0x02; //call end
            
            svclayer_nad_set_command3(value, NULL);
	}
}

#endif

#if 1
    
    if(btn_type == ECALL_TYPE)
      sendNotification(2, NULL);
    else if(btn_type == BCALL_TYPE)
      sendNotification(3, NULL);
    else if(btn_type == CCALL_TYPE)
      sendNotification(4, NULL);
#endif
    return res;
}

/*
#define CALL_STATUS_INCOMMING     0x01
#define CALL_STATUS_ORIGIN        0x02
#define CALL_STATUS_RING        0x03
#define CALL_STATUS_CONNECTED     0x04
#define CALL_STATUS_TERMINATED      0x05
*/
BOOL process_call_status_indi(msg_t *rcv_data)
{
    BOOL res = TRUE;
    byte call_status;
    char dir[MAX_DIR];

    LOGI("Received Call Status Indi");

    if(rcv_data == NULL)
    {
    	LOGI("No Rcv Data");
    	return FALSE;
    }
    memset(dir,0,sizeof(dir));

    call_status = rcv_data->data.call_status.call_status;
    memcpy(dir,rcv_data->data.call_status.number,MAX_DIR);

    LOGI("Rcv call status is %d", call_status);
    LOGI("Rcv call dir is %s", dir);
/*
  INCOMING = 5;
  ORIGINATING = 6;
  ALERTING = 7;
  CONNECTED = 8;
  TERMINATED = 9;
  CALLFAIL = 10;
*/
    switch (call_status)
    {
      case CALL_STATUS_INCOMMING:
        sendNotification(5, &dir[0]);
        break;
      case CALL_STATUS_ORIGIN:
        sendNotification(6, NULL);
        break;
      case CALL_STATUS_RING:
        sendNotification(7, NULL);
        break;
      case CALL_STATUS_CONNECTED:
        sendNotification(8, NULL);
        break;
      case CALL_STATUS_TERMINATED:
        sendNotification(9, NULL);
        break;
      default:
        break;                        
    }
    return res;
}


BOOL process_call_end_indi(msg_t *rcv_data)
{
    BOOL res = TRUE;
    return res;
}
BOOL process_call_failure_indi(msg_t *rcv_data)
{
    BOOL res = TRUE;
    byte reason;
    
    LOGI("Received Call failure Indi");

    if(rcv_data == NULL)
    {
      LOGI("No Rcv Data");
      return FALSE;
    }

    reason = rcv_data->data.call_failure.fail_reason;

    LOGI("Fail reason is %d", reason);

#if 0
    if(reason == 0) /* Normal */
      sendNotification(10, "normal");
    else if(reason == 1) /* Line Busy */
      sendNotification(10, "line busy");
#endif

#if 1
    sendNotification(10, NULL);
#endif

    return res;
}

/*
  NW_VALID = 11;
  NW_INVALID = 12;
  */

BOOL process_nw_status_indi(msg_t *rcv_data)
{
    BOOL res = TRUE;
    byte nw_status = 0;
    if(rcv_data == NULL)
    {
      LOGI("No Rcv Data");
      return FALSE;
    }
    LOGI("Received Data NW Status INDI");
    LOGI("Data NW status is %d", rcv_data->data.nw_status.status);
    nw_status = rcv_data->data.nw_status.status;
    //SEND TO APP WITH DATA NW STATUS

#if 1
    if(nw_status == 1)
      sendNotification(11, NULL);  //NW_VALID
    else if(nw_status == 0)
      sendNotification(12, NULL);  //NW_INVALID
#endif
    return res;
}

BOOL process_gps_location_info_indi(msg_t *rcv_data)
{
    BOOL res = TRUE;
    gps_info_indi_t info;
    LOGI("Received GPS INFO INDI");

    if(rcv_data == NULL)
    {
        LOGI(" No GPS Data");
        return FALSE;
    }
    
    memset(&info, 0, sizeof(info));   

    printf("GFix: %d\n", rcv_data->data.gps_info.gpsFix);
    printf("gTyp: %d\n", rcv_data->data.gps_info.gpsType);
    printf("long: %lf\n", rcv_data->data.gps_info.longitude);
    printf("lati: %lf\n", rcv_data->data.gps_info.latitude);
    printf("alti: %d\n", rcv_data->data.gps_info.altitude);
    printf("head: %d\n", rcv_data->data.gps_info.heading);
    printf("Sped: %d\n", rcv_data->data.gps_info.gpsSpeed);

    //SEND GPS INFO TO APP
    memcpy(&info, &rcv_data->data.gps_info, sizeof(info));    
    sendGpsNotification(0x60, 0, &info);
    return res;
}
BOOL process_collide_info_indi(msg_t *rcv_data)
{
    BOOL res = TRUE;
    LOGI("Received Collide INFO INDI");
    printf("Collide: %d\n", rcv_data->data.collide_info.collide);
    //SEND TO APP WITH COLLIDE INFO

    return res;
}
static BOOL dummy(msg_t *rcv_data)
{
  LOGI("dummy called");
  return TRUE;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
static BOOL dummy2(msg_t *rcv_data)
{
  LOGI("dummy called");
  return TRUE;
}
BOOL process_nad_info_indi(msg_t *rcv_data)
{
  BOOL res = TRUE;
  LOGI("process_nad_info_indi");

  printf("imei: %s\n", rcv_data->data.nad_info.imei);
  printf("iccid: %s\n", rcv_data->data.nad_info.iccid);
  printf("dir: %s\n", rcv_data->data.nad_info.dir);
  printf("swVer: %s\n", rcv_data->data.nad_info.swVer);

  //SEND TO APP WITH COLLIDE INFO
  return res;
}
BOOL process_nad_nw_info_indi(msg_t *rcv_data)
{
  BOOL res = TRUE;
  LOGI("process_nad_nw_info_indi");

  printf("rat: %d\n", rcv_data->data.nw_info.rat);
  printf("reg_state: %d\n", rcv_data->data.nw_info.reg_state);
  printf("mcc_mnc: %s\n", rcv_data->data.nw_info.mcc_mnc);
  printf("lac: %d\n", rcv_data->data.nw_info.lac);
  printf("tac: %d\n", rcv_data->data.nw_info.tac);
  printf("cellid: %d\n", rcv_data->data.nw_info.cellid);
  printf("signal_value: %d\n", rcv_data->data.nw_info.signal_value);

  //SEND TO APP WITH COLLIDE INFO
  return res;
}


//////////////////////////////////////////////////////////////


handler_type handler_get(cmd_type cmd)
{
  EXCEPTION_INIT
  handler_type result = NULL;
  unsigned int i;
  
  if(cmd >= ID_MAX) THROW();

  // search command to process
  for(i = 0; i < ARR_SIZE(handler_tbl); i++)
  {
    if(cmd == handler_tbl[i].cmd) break;
  }
  if(i >= ARR_SIZE(handler_tbl))
  {
    LOGI("%s", "invalid cmd\n");
    THROW();
  }

  result = handler_tbl[i].handler;

  CATCH_ERR()
  {
    result = NULL;
  }

  return result;
}

handler_type cc_handler_get(cc_cmd_type cmd)
{
  EXCEPTION_INIT
  handler_type result = NULL;
  unsigned int i;

  if(cmd >= CC_ID_MAX) THROW();

  // search command to process
  for(i = 0; i < ARR_SIZE(cc_handler_tbl); i++)
  {
    if(cmd == cc_handler_tbl[i].cmd) break;
  }
  if(i >= ARR_SIZE(cc_handler_tbl))
  {
    LOGI("%s", "invalid cmd\n");
    THROW();
  }

  result = cc_handler_tbl[i].handler;

  CATCH_ERR()
  {
    result = NULL;
  }

  return result;
}

//Process the packet from APP side
BOOL process_nadMsg_pkt(msg_t* pkt)
{
    EXCEPTION_INIT
    BOOL ret = TRUE;
    
    handler_type handler = NULL;
    
    if(pkt == NULL)
    {
        LOGI("rcv_data packet is NULL");    	
        ret = FALSE;
        return ret;	  
    }
    #if 1
    LOGI("Nad Pkt Group(%d) CMDL(0x%x) Received",pkt->group,pkt->cmd);
    
    if(pkt->group == SAFETY_AND_CALL_SRV_TO_APP)
    {
        handler = handler_get(pkt->cmd);
    }
    else if(pkt->group == COMMON_INFO_TO_APP)
    {
        handler = cc_handler_get(pkt->cmd);
    }
    
    if(!handler)
    {
        LOGI("%s", "can't process cmd since there is no handler to process it\n");
        THROW();
    }
	
    handler(pkt);
    
    #endif
    
    CATCH_ERR()
    {
        ret = FALSE;
    }    


  return ret;	
}

int send_msgq_if_to_nad(msg_t send_data)
{
  int qid;
  int ret = 0;

  send_data.data_type = IF_TO_NAD_TYPE;
  qid = msgget( (key_t)IF2NAD_MSGQ_KEY, IPC_CREAT | 0666);

  if(qid<0)
  {
    LOGI("send_msgq_to_appif msgget error\n");
    ret = -1;
    return ret;
  }

  if(msgsnd( qid, &send_data, sizeof(send_data) - sizeof(long), 0)<0)
  {
    LOGI("send_msgq_to_appif msgsnd error\n");
    ret = -1;
  }

  LOGI("send_msgq() key(%d) ret =%d",IF2NAD_MSGQ_KEY, ret);

  return ret;
}
int send_msgq_nad_to_if(msg_t send_data)
{
  int qid;
  int ret = 0;

  send_data.data_type = NAD_TO_IF_TYPE;
  qid = msgget( (key_t)NAD2IF_MSGQ_KEY, IPC_CREAT | 0666);

  if(qid<0)
  {
    LOGI("send_msgq_to_appif msgget error\n");
    ret = -1;
    return ret;
  }

  if(msgsnd( qid, &send_data, sizeof(send_data) - sizeof(long), 0)<0)
  {
    LOGI("send_msgq_to_appif msgsnd error\n");
    ret = -1;
  }

  LOGI("send_msgq() key(%d) ret =%d",NAD2IF_MSGQ_KEY, ret);

  return ret;
}

