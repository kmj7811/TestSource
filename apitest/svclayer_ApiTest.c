#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "svclayer_ClientApi.h"



void print_help()
{
    printf("----HELP LIST----\n");
    printf(" 0: ORIG CALL\n");
    printf(" 1: ACCEPT CALL\n");
    printf(" 2: EDN CALL\n");
    printf(" 3: GET GPS\n");
}

int main(int argc, char** argv)
{
    int cmd;
    int res;
    
    printf("=============== Client Test start =============\r\n");
    

    
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
                break;
            } 
            case 3:
            {
                int32_t value = 0x60;
                const char* dummy = "dummy";
                svclayer_setGpsCommand(value,dummy);
                break;
            }                               
            default:
                print_help();
                break;
        }
    }
    return 0;
}

