/*===================================================

                  SVCLAYER CLIENT API C


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

/*=====================================================

                     INCLUDE FILES FOR MODULE

=====================================================*/
#include "ql_oe.h"
#include "common.h"
#include "tcu_interface.h"
#include "msg_if.h"
#include "svclayer_ClientApi.h"
#include "svclayer_NadMgrIpc.h"
#include "svclayer_TcuMain.h"


static call_callBack svclayer_callNotification = NULL;   //callback 으로 등록된 함수 ponter
static gps_callBack  svclayer_gpsNotification = NULL;    //FUTURE TODO
/////////////////////////////////////////////////////////////////////////////////////////////

int svclayer_NadStart()
{
   int res = 0;
   res = start_tcu_main();
   if(res != 0 )
   {
       LOGI("Fail to Start TCU Main");
       res = -1;
   }
   return res;
}
///////////////////////////////////////////////////////////////////////////////////////////////
void svclayer_regCall(call_callBack cb)
{
    LOGI("regi Call callback");
    if(cb == NULL)
    { 
        LOGI("Null Cb ptr");
        return;
    }
    svclayer_callNotification = cb;
}

void svclayer_regGps(gps_callBack cb)
{
    LOGI("regi GPS callback");
    if(cb == NULL)
    { 
        LOGI("Null Cb ptr");
        return;
    }
    svclayer_gpsNotification = cb;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sendNotification(int msgid, const char* msgargs)
{
    
    if(svclayer_callNotification ==  NULL)
    {
        LOGI("svclayer_callNotification is NULL");
        return;
    }
    LOGI("sendNotification");
    svclayer_callNotification(msgid, msgargs);  
      
}

void sendGpsNotification(int msgid, int status, gps_info_indi_t* info)
{
    
    if(svclayer_gpsNotification ==  NULL)
    {
        LOGI("svclayer_gpsNotification is NULL");
        return;
    }
    LOGI("sendGpsNotification");
    svclayer_gpsNotification(msgid, status,info);  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
int svclayer_setCallCommand(int32_t value,const char*  message) //--> int svclayer_nad_set_command3(int32_t value1,	const char*  message); //svclayer_NadMgrIpc.h
{
    int res = 0;
    LOGI("svclayer_setCallCommand command(0x%x) string: %s",value, message);
    svclayer_nad_set_command3(value, message);
    return res;
}
int svclayer_setGpsCommand(int32_t value, const char*  message)
{
    int res = 0;
    LOGI("svclayer_setGpsCommand command(0x%x) string: %s",value);
    svclayer_nad_set_Gpscommand(value, message);
    return res;
}







