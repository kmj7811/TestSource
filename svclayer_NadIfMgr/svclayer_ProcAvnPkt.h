/*===================================================

                  PROCESS AVN PACKET H


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

#ifndef __PROCESS_AVN_PKT_H
#define __PROCESS_AVN_PKT_H
#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>


#include "common.h"
#include "msg_if.h"

#define DEFAULT_PKT_LENGTH	6   //sof + cmdidh + cmdidl + lengthH + lengthL + eof

typedef BOOL (*tcuavnif_handler_type)(avn_cmdq_t *rcv_data);
typedef BOOL (*avn_handler_type)(avn_data_t *rcv_data);


BOOL process_tcuAvnIfMgr_pkt(avn_cmdq_t *rcv_data);
int process_avn_pkt(avn_data_t* avn_pkt);



#endif //__PROCESS_AVN_PKT_H


