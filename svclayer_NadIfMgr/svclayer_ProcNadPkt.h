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

#ifndef __SVC_PROC_NAD_PKT_H
#define __SVC_PROC_NAD_PKT_H

#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "msg_if.h"
#include "common.h"

#define NADEXPORT __attribute__ ((visibility("default")))
#define NADLOCAL __attribute__ ((visibility("hidden")))

#define DEFAULT_PKT_LENGTH	6   //sof + cmdidh + cmdidl + lengthH + lengthL + eof

typedef BOOL (*handler_type)(msg_t *rcv_data);

typedef struct
{  
	cmd_type cmd;
	handler_type handler;
} cmd_processor_type;

typedef struct
{
	cc_cmd_type cmd;
	handler_type handler;
} cc_cmd_processor_type;

#if defined(__cplusplus)
extern "C" {
#endif
  BOOL process_nadMsg_pkt(msg_t* pkt);
#if defined(__cplusplus)
}
#endif

handler_type handler_get(cmd_type cmd);
int send_msgq_if_to_nad(msg_t send_data);
int send_msgq_nad_to_if(msg_t send_data);

#endif //__SVC_PROC_NAD_PKT_H
