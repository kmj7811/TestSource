#include <sys/types.h>
#include <sys/stat.h>
#include "msg_if.h"

typedef unsigned char byte;

typedef void (*call_callBack) (int msgid, const char* msgargs);   
typedef void (*gps_callBack) (int msgid, int status, gps_info_indi_t* info); 


/////////////////////////////////////////////////////////////////////
extern int svclayer_NadStart();
/////////////////////////////////////////////////////////////////////
extern void svclayer_regCall(call_callBack cb);
extern void svclayer_regGps(gps_callBack cb);
/////////////////////////////////////////////////////////////////////
extern void sendNotification(int msgid, const char* msgargs);
extern void sendGpsNotification(int msgid, int status, gps_info_indi_t* info);
/////////////////////////////////////////////////////////////////////
extern int svclayer_setCallCommand(int32_t value,const char*  message); //--> int svclayer_nad_set_command3(int32_t value1,	const char*  message); //svclayer_NadMgrIpc.h
extern int svclayer_setGpsCommand(int32_t value, const char*  message);