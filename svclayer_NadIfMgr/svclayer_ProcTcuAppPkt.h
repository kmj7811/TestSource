/*===================================================

                  PROCESS TCU APP PACKET H


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

#ifndef __PROCESS_TCUAPP_PKT_H
#define __PROCESS_TCUAPP_PKT_H
#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "msg_if.h"


typedef boolean (*handler_type)(tcuApp_cmdq_t *rcv_data);



int send_sos_voice_call_indi_to_tcuapp();
int send_acn_voice_call_indi_to_tcuapp();
int send_concierge_voice_call_indi_to_tcuapp();
void send_voice_call_end_to_tcuapp();
void send_voice_call_est_indi_to_tcuapp();
void process_tcu_call_wait_expired_timer();


boolean process_tcuAppTcu_pkt(tcuApp_cmdq_t *rcv_data);


#endif //__PROCESS_TCUAPP_PKT_H


