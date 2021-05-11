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
#include "svclayer_NadMgrIpc.h "
#include "svclayer_TcuMain.h"

int svclayer_NadStart()
{
   int res = 0;
   res = start_tcu_main();
   if(res != 0 )
   {
       printf("Fail to Start TCU Main\n");
       res = -1;
   }
   return res;
}
void svclayer_regCall(call_callBack cb)
{
    printf("regi Call call back\n");
    if(cb == NULL)
    { 
        printf("Null Cb ptr\n");
        return;
    }
    sendNotification = cb;
}
void svclayer_regGps(gps_callBack cb)
{
    printf("regi Gps call back\n");
    if(cb == NULL)
    { 
        printf("Null Cb ptr\n");
        return;
    }
    //sendNotification = cb;
}
int svclayer_setCallCommand(int32_t value,const char*  message); //--> int svclayer_nad_set_command3(int32_t value1,	const char*  message); //svclayer_NadMgrIpc.h
{
    int res = 0;
    LOGI("svclayer_setCallCommand command(0x%x) string: %s",value1, message);
    svclayer_nad_set_command3(value1, message);
    return res;
}
int svclayer_setGpsCommand(int32_t value);
{
    int res = 0;
    LOGI("svclayer_setGpsCommand command(0x%x) string: %s",value1);
    return res;
}







