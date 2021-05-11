#include <sys/types.h>
#include <sys/stat.h>
#include "msg_if.h"

typedef unsigned char byte;

call_callBack sendNotification;   //callback 으로 등록된 함수 ponter
gps_callBack  sendGpsNotification;


typedef void (*call_callBack) (int msgid, const char* msgargs);   //sendNotification
typedef void (*gps_callBack) (int msgid, gps_info_indi_t* info);   //sendNotification

int svclayer_NadStart();
void svclayer_regCall(call_callBack cb);
void svclayer_regGps(gps_callBack cb);
int svclayer_setCallCommand(int32_t value,const char*  message); //--> int svclayer_nad_set_command3(int32_t value1,	const char*  message); //svclayer_NadMgrIpc.h
int svclayer_setGpsCommand(int32_t value);