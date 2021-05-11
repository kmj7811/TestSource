/*===================================================

                  PROCESS TCUMCU PACKET H


GENERAL DESCRIPTION

  Process Msg from TCU MCU IF MGR  to TCU  thread 
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

#ifndef __PROCESS_TCUMCU_PKT_H
#define __PROCESS_TCUMCU_PKT_H
#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "msg_if.h"


typedef boolean (*mcuiftcu_handler_type)(mcu_cmdq_t *rcv_data);

typedef struct
{  
    mcuiftcu_cmd_type cmd;
    mcuiftcu_handler_type handler;
} mcuiftcu_cmd_processor_type;

int send_voice_call_end_to_mcuif();
int send_voice_call_result_indi_to_mcuif(byte call_status);

boolean process_mcuIfTcu_pkt(mcu_cmdq_t *rcv_data);


#endif //__PROCESS_TCUMCU_PKT_H


