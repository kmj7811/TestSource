/*===================================================

                  SVCLAYER NAD TO IF PACKET


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

#include "common.h"
#include "msg_if.h"
#include "tcu_interface.h"
#include "svclayer_NadIfPkt.h"
#include "svclayer_ProcNadPkt.h"


/*
 typedef enum {
	START_CALL_REQ =0x00,
	CALL_STATUS_INDI,
	CALL_FAILURE_INDI,

	DATA_NW_STATUS_INDI = 0x30,

	GPS_INFO_INDI = 0x60,

    APP_NAD_CALL_STATUS = 0xC0,

	COLLIDE_INFO_INDI = 0xF0,

*/
BOOL send_start_call_indi(byte btn_type)
{
    int result = TRUE;
    msg_t send_data;

    memset(&send_data, 0, sizeof(send_data));

    send_data.group = SAFETY_AND_CALL_SRV_TO_APP;
    send_data.cmd = START_CALL_REQ;
    send_data.data.start_call.call_type = btn_type;
    LOGI("send a send_start_call_indi btn(%d)",btn_type);

    result = send_msgq_nad_to_if(send_data);
    if(result !=0)
    {
        LOGI("Fail to send a send_msgq_to_avnif");
        result = FALSE;
    }
    return result;
}


BOOL send_start_status_indi(byte call_status, char* dir)
{
    int result = TRUE;
    msg_t send_data;
    //char num[MAX_DIR];

    memset(&send_data, 0, sizeof(send_data));
    LOGI("Call status is %d",call_status);
    send_data.group = SAFETY_AND_CALL_SRV_TO_APP;    
    send_data.cmd = CALL_STATUS_INDI;
    if(call_status == CALL_STATUS_INCOMMING)
    {
    if(dir == NULL)
    {
      LOGI("No dial Number");
      result = FALSE;
      return result;
    }
      LOGI("Incoming Number is %s",dir);
      memcpy(send_data.data.call_status.number, dir,MAX_DIR);
      send_data.data.call_status.call_status = call_status;
    }
    else
    {
      send_data.data.call_status.call_status = call_status;
    }

    result = send_msgq_nad_to_if(send_data);

    if(result !=0)
    {
        LOGI("Fail to send a send_msgq_to_avnif");
        result = FALSE;
    }
    return result;
}

BOOL send_gps_info_indi(gps_info_indi_t info)
{
    int result = TRUE;
    msg_t send_data;
    //char num[MAX_DIR];

    memset(&send_data, 0, sizeof(send_data));
    LOGI("send_gps_info_indi");
    send_data.group = SAFETY_AND_CALL_SRV_TO_APP;
    send_data.cmd = GPS_INFO_INDI;
    memcpy(&send_data.data.gps_info, &info, sizeof(info));

    result = send_msgq_nad_to_if(send_data);

    if(result !=0)
    {
        LOGI("Fail to send a send_msgq_to_avnif");
        result = FALSE;
    }
    return result;
}
BOOL send_data_nw_status_indi(BOOL flag)
{
    int result = TRUE;
    msg_t send_data;
    //char num[MAX_DIR];

    memset(&send_data, 0, sizeof(send_data));
    LOGI("send_data_nw_status_indi");
    send_data.group = SAFETY_AND_CALL_SRV_TO_APP;
    send_data.cmd = DATA_NW_STATUS_INDI;
    send_data.data.nw_status.status = flag;
    result = send_msgq_nad_to_if(send_data);

    if(result !=0)
    {
        LOGI("Fail to send a send_msgq_to_avnif");
        result = FALSE;
    }
    return result;
}
BOOL send_collide_info_indi(byte collide)
{
    int result = TRUE;
    msg_t send_data;
    //char num[MAX_DIR];

    memset(&send_data, 0, sizeof(send_data));
    LOGI("send_collide_info_indi %d", collide);
    send_data.group = SAFETY_AND_CALL_SRV_TO_APP;
    send_data.cmd = COLLIDE_INFO_INDI;
    send_data.data.collide_info.collide = collide;

#if 1
    result = send_msgq_nad_to_if(send_data);

    if(result !=0)
    {
        LOGI("Fail to send a send_msgq_to_avnif");
        result = FALSE;
    }
#endif
    return result;
}

BOOL send_nad_info_indi(nad_info_indi_t *info)
{
  int result = TRUE;
  msg_t send_data;

  if(info == NULL)
  {
    LOGI("Info Null");
    return FALSE;
  }
  memset(&send_data, 0, sizeof(send_data));

    LOGI("send_nad_info_indi");
    send_data.group = COMMON_INFO_TO_APP;
    send_data.cmd = NAD_INFO_IND;
    memcpy(&send_data.data.nad_info, info, sizeof(nad_info_indi_t));

    result = send_msgq_nad_to_if(send_data);

    if(result !=0)
    {
        LOGI("Fail to send a send_msgq_to_avnif");
        result = FALSE;
    }
  return result;

}
BOOL send_nad_nw_info_indi(nad_nw_info_indi_t *info)
{
  int result = TRUE;
  msg_t send_data;

  if(info == NULL)
  {
    LOGI("Info Null");
    return FALSE;
  }
  memset(&send_data, 0, sizeof(send_data));

    LOGI("send_nad_nw_info_indi");
    send_data.group = COMMON_INFO_TO_APP;
    send_data.cmd = NAD_NW_INFO_IND;
    memcpy(&send_data.data.nw_info, info, sizeof(nad_nw_info_indi_t));

    result = send_msgq_nad_to_if(send_data);

    if(result !=0)
    {
        LOGI("Fail to send a send_msgq_to_avnif");
        result = FALSE;
    }
  return result;


  return result;

}
