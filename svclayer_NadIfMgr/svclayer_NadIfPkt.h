/*===================================================

                  SVCLAYER NAD TO IF PACKET H


GENERAL DESCRIPTION

  Process Msg from MCU  to TCU  IF MGR  thread 
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

#ifndef __SVC_NAD_TO_IF_H
#define __SVC_NAD_TO_IF_H
#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "msg_if.h"
#include "common.h"


BOOL send_start_call_indi(byte btn_type);
BOOL send_start_status_indi(byte call_status, char* dir);
BOOL send_gps_info_indi(gps_info_indi_t info);
BOOL send_data_nw_status_indi(BOOL flag);
BOOL send_collide_info_indi(byte collide);
BOOL send_nad_info_indi(nad_info_indi_t *info);
BOOL send_nad_nw_info_indi(nad_nw_info_indi_t *info);

#endif //__SVC_NAD_TO_IF_H


