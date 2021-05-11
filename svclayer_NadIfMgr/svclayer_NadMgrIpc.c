/*===================================================

                  SERVICE LAYER NAD MGR IPC File


GENERAL DESCRIPTION




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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "svclayer_NadMgrIpc.h"
#include "svclayer_NadIfPkt.h"



////////////////////////////////
//Temp function.... need to be moved to MCU mgr file or other files
//TBD
////////////////////////////////
void get_collideInfo()
{
    //char tmp[10];
    byte collide = 0; //0: BTN PRESSED, 1: CAR COLLIED
    send_collide_info_indi(collide);
}
//////////////////////////////

BOOL process_call_start_req(const char* message)
{
	BOOL res = TRUE;
	char dial_string[MAX_DIAL_STRING];

	if( message == NULL)
	{
	    res = FALSE;
           LOGI("No Dial Digit");
	    return res;
	}

	memcpy(dial_string,message,MAX_DIAL_STRING);
	//res = st_call_MOCall(dial_string);
       res = voice_origination(dial_string);
	if(res != FALSE)
	{
	    LOGI("Fail to Orig Call");
	}
	return res;
}
BOOL process_call_accep_req()
{
	BOOL res = TRUE;
	LOGI("process_call_accep_req");
	res = voice_call_accept();
	if(res != FALSE)
	{
	    LOGI("Fail to Accept Call");
	}	
	return res;

}
BOOL process_call_end_req()
{
	BOOL res = TRUE;
	LOGI("process_call_end_req");
       res =  voice_call_end();
	if(res != FALSE)
	{
	    LOGI("Fail to End Call");
	}		   
	return res;

}
BOOL process_data_nw_status_req()
{
	BOOL res = TRUE;
	BOOL flag = TRUE;
	LOGI("process_data_nw_status_req");
	//flag = get_data_nw_status();
	res = send_data_nw_status_indi(flag);
	return res;
}

BOOL process_gps_info_req()
{
	BOOL res = TRUE;
	LOGI("process_gps_info_req");
	get_locationInfo();
	return res;
}
BOOL process_collide_info_req()
{
	BOOL res = TRUE;
	LOGI("process_collide_info_req");
	//Get Collide Info from MCU...TBD
	get_collideInfo();
	return res;
}

void process_svclayerIfMsgSafety(int value, const char* message)
{
	BOOL res = TRUE;
    switch(value)
	{
        case CALL_START_REQ:
        	res = process_call_start_req(message);
        	if(res == FALSE)
        	{
        		//Send Call failure indicator to APP
        		LOGI("Send Fail Indicator to APP");
        	}
        	break;
        case CALL_ACCEPT_REQ:
        	process_call_accep_req();
        	break;
        case CALL_END_REQ:
        	process_call_end_req();
        	break;
        case DATA_NW_STATUS_REQ:
        	process_data_nw_status_req();
        	break;

        case COLLIDE_INFO_REQ:
        	process_collide_info_req();
        	break;

        default:
        	break;
	}
}

void process_svclayerIfMsgGps(int value, const char* message)
{
	BOOL res = TRUE;
    switch(value)
	{       

        case GPS_INFO_REQ:
        	process_gps_info_req();
        	break;
        default:
        	break;
	}
}
//////////////////////////////////////////////////////////////////


int svclayer_nad_set_command1
(
    int32_t value1
)
{
    int res = 0;
	LOGI("test_API_SendCommand1 command : %d",value1);

	if(value1 == 1)
	{
		LOGI(" value 1 command");
		//send_test_data_one();
		//start_locationTrigger();
	}
	if(value1 == 2)
	{
		LOGI(" value 2 command");
		//send_test_data_two();
		//stop_locationTrigger();
	}
	return res;
}
int svclayer_nad_set_command2
(
	const char*  message
)
{
    int res = 0;
    LOGI("test_API_SendCommand2 command : %s",message);

    return res;
}

/// For Call related IPC
int svclayer_nad_set_command3
(
	int32_t value1,
	const char* message
)
{
    int res = 0;
    LOGI("svclayer_nad_set_command3 command(0x%x) string: %s",value1, message);
    process_svclayerIfMsgSafety(value1, message);

    return res;
}

/// For GPS related IPC
int svclayer_nad_set_Gpscommand
(
	int32_t value1,
	const char* message
)
{
    int res = 0;
    LOGI("svclayer_nad_set_Gpscommand command(0x%x) string: %s",value1, message);
    process_svclayerIfMsgGps(value1, message);

    return res;
}
/////////////////////////////////////////////////////////////////////////////

BOOL process_nad_info_req()
{
    BOOL res = TRUE;
    LOGI("process_nad_info_req");
    //get_nad_StaticDb_info();
    return res;

}
BOOL process_nad_nw_info_req()
{
    BOOL res = TRUE;
    LOGI("process_nad_nw_info_req");
    //get_nad_NwDb_info();
    return res;

}

void process_svclayerIfMsgCommon(int value, const char* message)
{
	//BOOL res = TRUE;
    switch(value)
	{
		case NAD_INFO_REQ:
			process_nad_info_req();
			break;
		case NAD_NW_INFO_REQ:
			process_nad_nw_info_req();
			break;

        default:
        	break;
	}
}
int svclayer_nad_set_command4
(
	int32_t value1,
	const char* message
)
{
    int res = 0;
    LOGI("svclayer_nad_set_command4 command(0x%x) string: %s",value1, message);
    process_svclayerIfMsgCommon(value1, message);
    
    return res;
}

int svclayer_nad_get_command1
(
    char* message, size_t messageSize
)
{

    #if 1
    int res = 0;
    char temp[100];
    int i = 2;
    memset(temp,0,100);
    sprintf(temp,"test num %d",i);
    LOGI("Tx String is %s",temp);
    
    
    memcpy(message, temp, 100);
    #endif
    LOGI("unit_Test_GetCommand1");
    return res;
}



