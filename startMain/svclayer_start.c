#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "svclayer_ClientApi.h"
#include "msg_if.h"

#if 1
void Client_IfCallback(int msgid, const char* msgargs)
{
    printf("Client_IfCallback value is %d, msg: %s\n", msgid,msgargs);
    return;
}
void Client_IfGpsCallback(int msgid, int status, gps_info_indi_t* info)
{
    printf("Client_IfGpsCallback value is %d, status: %s\n", msgid,status);
    printf("GFix: %d\n", info->gpsFix);
    printf("gTyp: %d\n", info->gpsType);
    printf("long: %lf\n", info->longitude);
    printf("lati: %lf\n",info->latitude);
    printf("alti: %d\n", info->altitude);
    printf("head: %d\n", info->heading);
    printf("Sped: %d\n", info->gpsSpeed);
    return;
}
void print_help()
{
    printf("----HELP LIST----\n");
    printf(" 1: ORIG CALL\n");
    printf(" 2: ACCEPT CALL\n");
    printf(" 3: EDN CALL\n");
}

int main(int argc, char** argv)
{
    int cmd;
    int res;
        
    printf("=============== svc layer start =============\r\n");
    
    
    printf("Register Call CallBack\n");
    svclayer_regCall(Client_IfCallback);       
 
    printf("Register GPS CallBack\n");
    svclayer_regGps(Client_IfGpsCallback);
    
    printf("Do Start Tcu Main\n");
    res = svclayer_NadStart();
    if(res !=0)
    {
        printf("Fail to Start TCU Main\n");
        res = -1;
        return res;
    }
      
     
    
    #if 0
    print_help();
    
    while(1)
    {
       printf("please input cmd: ");
        scanf("%d", &cmd);

        switch(cmd)
        {
            case 0:
            {
                int32_t value = 0x00;
                const char* dummy = "dummy";
                svclayer_setCallCommand(value,"01024647811");
            }
                break;
            case 1:
            {
                int32_t value = 0x01;
                const char* dummy = "dummy";
                svclayer_setCallCommand(value,dummy);
            }            
                break;
            case 2:
            {
                int32_t value = 0x02;
                const char* dummy = "dummy";
                svclayer_setCallCommand(value,dummy);
            }                  
            default:
                print_help();
                break;
        }
    }
    #endif
    
    return 0;
}
#endif 

#if 0
int main(int argc, char** argv)
{

    int res;
    
    printf("=============== svc layer start =============\r\n");
    
 
    
    printf("Do Start Tcu Main\n");
    res = svclayer_NadStart();
    if(res !=0)
    {
        printf("Fail to Start TCU Main\n");
        res = -1;
        return res;
        
    }
    while(1)
    {
    }
    
    return res;
}

#endif