/*===================================================

                  SVCLAYER NAD MGR IPC H


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

#ifndef __SVC_NAD_MGR_IPC_H
#define __SVC_NAD_MGR_IPC_H

#include "common.h"



#define MAX_DIAL_STRING				20

#define CALL_START_REQ 				0x00
#define CALL_ACCEPT_REQ				0x01
#define CALL_END_REQ				0x02

#define DATA_NW_STATUS_REQ			0x30

#define GPS_INFO_REQ				0x60


void get_collideInfo();
BOOL process_call_start_req(const char* message);
BOOL process_call_accep_req();
BOOL process_call_end_req();
BOOL process_data_nw_status_req();
BOOL process_gps_info_req();
BOOL process_collide_info_req();
void process_svclayerIfMsgSafety(int value, const char* message);

//////////////////////////////////////////////////////////////////
int svclayer_nad_set_command1( int32_t value1);
int svclayer_nad_set_command2(const char*  message);
int svclayer_nad_set_command3(int32_t value1,	const char*  message);
int svclayer_nad_set_command4(int32_t value1,const char*  message);
int svclayer_nad_get_command1(char* message, size_t messageSize);
int svclayer_nad_set_Gpscommand(int32_t value1,	const char* message);
/////////////////////////////////////////////////////////////////////////////
BOOL process_nad_info_req();
BOOL process_nad_nw_info_req();
void process_svclayerIfMsgCommon(int value, const char* message);
void process_svclayerIfMsgGps(int value, const char* message);

#endif //__SVC_NAD_MGR_IPC_H