/*===================================================

                  PROCESS TCU APP PACKET H


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

#ifndef __PROCESS_TCUAVN_PKT_H
#define __PROCESS_TCUAVN_PKT_H
#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "msg_if.h"


typedef BOOL (*avniftcu_handler_type)(avn_cmdq_t *rcv_data);



int send_msgq_to_avnif(avn_cmdq_t send_data);
void send_voice_call_connecting_indi_to_avnif(byte call_type);
void send_voice_call_connected_indi_to_avnif(byte call_type);
void send_voice_call_end_to_avnif();
void send_voice_call_failure_indi_to_avnif(byte reason);


boolean process_avnIfTcu_pkt(avn_cmdq_t *rcv_data);


#endif //__PROCESS_TCUAVN_PKT_H


