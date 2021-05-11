/*===================================================

                  NAD VOICE CALL Telephony Mgr C


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

voice_client_handle_type    h_voice     = 0;
int     voice_call_id = 0;


int voice_handler_allocated = FALSE;
byte call_state_value = CALL_IDLE;	
int btn_calltype = NCALL_TYPE;

int get_btn_call_type()
{
	return btn_calltype;
}
void set_btn_call_type(int callType)
{
	btn_calltype = callType;
}

byte nad_get_current_call_state()
{
    LOGI("Current NAD Call State is %d",call_state_value);
    return call_state_value;	
}

byte nad_set_current_call_state(byte state)
{
	call_state_value = state;
}
	
int nad_get_voice_handler_allocated()
{
    LOGI("Voice handler allocated %d",voice_handler_allocated);
    return  voice_handler_allocated;
}


static void nad_voice_call_cb_func(int                     call_id,
                                  char*                   phone_num,
                                  E_QL_VOICE_CALL_STATE_T state,
                                  void                    *contextPtr)
{
    voice_client_handle_type *ph_voice = (voice_client_handle_type *)contextPtr;
    int ret = 0;
    int call_type = NCALL_TYPE;
    
    char *call_state[] = {"INCOMING", "DIALING", "ALERTING", "ACTIVE", "HOLDING", "END", "WAITING"};
    byte callstate = 0;
    
    if(*ph_voice == 0)//Call_id got here, you can save it for the later operation
    {
        *ph_voice = call_id;
    }
    
    LOGI("######### Call id=%d, PhoneNum:%s, event=%s!  ######\n", call_id, phone_num, call_state[state]);

    LOGI("Call state is %d", state);
    if(call_state[state] == "INCOMING")
    {
        voice_call_id = call_id;
        #if 0
        //Incomming call, Answer the call
        //sleep(2);
        ret = QL_Voice_Call_Answer(h_voice, call_id);  
        LOGI("Answer Incomming Call ret = %d",ret);  
        #endif			
        /////////////////////////////////////
        #if 0
        #define CALL_STATUS_INCOMMING           0x01
        #define CALL_STATUS_ORIGIN              0x02
        #define CALL_STATUS_RING                0x03
        #define CALL_STATUS_CONNECTED           0x04
        #define CALL_STATUS_TERMINATED          0x05
        #endif
        send_start_status_indi(CALL_STATUS_INCOMMING, phone_num);		
        	
    }
    else if(call_state[state] == "DIALING")
    {
        callstate = CALL_DIALING;	
    }	
    else if(call_state[state] == "END")
    {
        callstate = CALL_END;	
        send_voice_call_end_to_avnif();  
        send_voice_call_end_to_mcuif();
		
        set_btn_call_type(NCALL_TYPE);

       /////////////////////////////////////
        #if 0
        #define CALL_STATUS_INCOMMING           0x01
        #define CALL_STATUS_ORIGIN              0x02
        #define CALL_STATUS_RING                0x03
        #define CALL_STATUS_CONNECTED           0x04
        #define CALL_STATUS_TERMINATED          0x05
        #endif
        send_start_status_indi(CALL_STATUS_TERMINATED, NULL);
        //////////////////////////////////// 		
     
    }
    else if(call_state[state] == "ACTIVE")
    {
        callstate = CALL_ACTIVE;
        //Send Connected indicat to AVN
        call_type = get_btn_call_type();
        send_voice_call_connected_indi_to_avnif(call_type);	
        send_voice_call_result_indi_to_mcuif(CALL_STATUS_SUCESS);	

        /////////////////////////////////////
        #if 0
        #define CALL_STATUS_INCOMMING           0x01
        #define CALL_STATUS_ORIGIN              0x02
        #define CALL_STATUS_RING                0x03
        #define CALL_STATUS_CONNECTED           0x04
        #define CALL_STATUS_TERMINATED          0x05
        #endif
        send_start_status_indi(CALL_STATUS_CONNECTED, NULL);		
    }
    else if(call_state[state] == "ALERTING")
    {
        /////////////////////////////////////
        #if 0
        #define CALL_STATUS_INCOMMING           0x01
        #define CALL_STATUS_ORIGIN              0x02
        #define CALL_STATUS_RING                0x03
        #define CALL_STATUS_CONNECTED           0x04
        #define CALL_STATUS_TERMINATED          0x05
        #endif
        send_start_status_indi(CALL_STATUS_RING, NULL);    
    }

	
	nad_set_current_call_state(callstate);
    //tcu_process_nad_call_state(call_state_value,call_id);
	
}

void init_call_number_cfg()
{
    int fd1,fd2,fd3,fd4,fd5;
    int readn;
    int writen;
	
    char *default_ccall_str = "07048503349";
    char *default_bcall_str = "07048503349";
    char *default_ecall_str = "07048503349";
    char *default_acall_str = "07048503349";
    char *default_ncall_str = "114";

#if 0
    //Open File and Get File Descripter//
    fd1 = open ("../usrdata/ccall_num" , O_RDWR);

    if (fd1 == -1)
    {
        printf ("File open failed.....\n");    
        // Write buf2 into fd
        fd1 = creat ("../usrdata/ccall_num", 0644);
    
        if (fd1 == -1)
            printf ("Creat function failed\n");	 
    
        writen = write(fd1, default_ccall_str, strlen(default_ccall_str));
        close( fd1);		
	
    }
    //Open File and Get File Descripter//
    fd2 = open ("../usrdata/bcall_num" , O_RDWR);

    if (fd2 == -1)
    {
        printf ("File open failed.....\n");    
        // Write buf2 into fd
        fd2 = creat ("../usrdata/bcall_num", 0644);
    
        if (fd2 == -1)
            printf ("Creat function failed\n");	 
    
        writen = write(fd2, default_bcall_str, strlen(default_bcall_str));	
        close( fd2);		
		
    }
	
    //Open File and Get File Descripter//
    fd3 = open ("../usrdata/ecall_num" , O_RDWR);

    if (fd3 == -1)
    {
        printf ("File open failed.....\n");    
        // Write buf2 into fd
        fd3 = creat ("../usrdata/ecall_num", 0644);
    
        if (fd3 == -1)
            printf ("Creat function failed\n");	 
    
        writen = write(fd3, default_ecall_str, strlen(default_ecall_str));	
        close( fd3);		
		
    }
	
    //Open File and Get File Descripter//
    fd4 = open ("../usrdata/acall_num" , O_RDWR);

    if (fd4 == -1)
    {
        printf ("File open failed.....\n");    
        // Write buf2 into fd
        fd4 = creat ("../usrdata/acall_num", 0644);
    
        if (fd4 == -1)
            printf ("Creat function failed\n");	 
    
        writen = write(fd4, default_acall_str, strlen(default_acall_str));	
        close( fd4);		
		
    }
	
    //Open File and Get File Descripter//
    fd5 = open ("../usrdata/ncall_num" , O_RDWR);

    if (fd5 == -1)
    {
        printf ("File open failed.....\n");    
        // Write buf2 into fd
        fd5 = creat ("../usrdata/ncall_num", 0644);
    
        if (fd5 == -1)
            printf ("Creat function failed\n");	 
    
        writen = write(fd5, default_ncall_str, strlen(default_ncall_str));	
        close( fd5);		
		
    }		
#else
    //Open File and Get File Descripter//
    fd1 = open ("../usrdata/ccall_num" , O_RDWR);

    if (fd1 == -1)
    {
        printf ("File open failed.....\n");    
        // Write buf2 into fd
        fd1 = creat ("../usrdata/ccall_num", 0644);
    
        if (fd1 == -1)
            printf ("Creat function failed\n");	 
    
        writen = write(fd1, default_ccall_str, strlen(default_ccall_str));
        close( fd1);		
	
    }
    //Open File and Get File Descripter//
    fd2 = open ("../usrdata/bcall_num" , O_RDWR);

    if (fd2 == -1)
    {
        printf ("File open failed.....\n");    
        // Write buf2 into fd
        fd2 = creat ("../usrdata/bcall_num", 0644);
    
        if (fd2 == -1)
            printf ("Creat function failed\n");	 
    
        writen = write(fd2, default_bcall_str, strlen(default_bcall_str));	
        close( fd2);		
		
    }
	
    //Open File and Get File Descripter//
    fd3 = open ("../usrdata/ecall_num" , O_RDWR);

    if (fd3 == -1)
    {
        printf ("File open failed.....\n");    
        // Write buf2 into fd
        fd3 = creat ("../usrdata/ecall_num", 0644);
    
        if (fd3 == -1)
            printf ("Creat function failed\n");	 
    
        writen = write(fd3, default_ecall_str, strlen(default_ecall_str));	
        close( fd3);		
		
    }
	
    //Open File and Get File Descripter//
    fd4 = open ("../usrdata/acall_num" , O_RDWR);

    if (fd4 == -1)
    {
        printf ("File open failed.....\n");    
        // Write buf2 into fd
        fd4 = creat ("../usrdata/acall_num", 0644);
    
        if (fd4 == -1)
            printf ("Creat function failed\n");	 
    
        writen = write(fd4, default_acall_str, strlen(default_acall_str));	
        close( fd4);		
		
    }
	
    //Open File and Get File Descripter//
    fd5 = open ("../usrdata/ncall_num" , O_RDWR);

    if (fd5 == -1)
    {
        printf ("File open failed.....\n");    
        // Write buf2 into fd
        fd5 = creat ("../usrdata/ncall_num", 0644);
    
        if (fd5 == -1)
            printf ("Creat function failed\n");	 
    
        writen = write(fd5, default_ncall_str, strlen(default_ncall_str));	
        close( fd5);		
		
    }	
#endif

	
}

int voiceCallInit()
{

    int    ret     = E_QL_OK;

    if(voice_handler_allocated == TRUE)
    {
        LOGI("Voice all handler init aleady");
        return -1;		
    }
	
    ret = QL_Voice_Call_Client_Init(&h_voice);

    if(ret != 0 && ret != 1)
    {
        LOGI("QL_Voice_Call_Client_Init FAIL.%d\n");
        return -1;
    }
    LOGI("QL_Voice_Call_Client_Init ret = %d, with h_voice=%d\n", ret, h_voice);
	
    ret = QL_Voice_Call_AddStateHandler(h_voice, nad_voice_call_cb_func, &voice_call_id);
    if(ret != 0 && ret != 1)
    {
        LOGI("QL_Voice_Call_AddStateHandler FAIL.%d\n");
        return -1;
    }
    LOGI("QL_Voice_Call_AddStateHandler ret = %d\n", ret);	

    init_call_number_cfg();
	
    voice_handler_allocated = TRUE;
	
    return ret;

}

int get_string_count(char *string)
{
    int i = 0;
    int ret = 0;	
    char buf[100];		
    if(string == NULL)
      return ret;
    memcpy(buf,string,	sizeof(buf));
    for( i = 0; 	buf[i]!= '\0'; ++i);  
    return i;		
}
int get_call_number(char* number, int calltype)
{
    int ret = -1;
    int fd;
    int readfn;	
    int current;	
    char buf[30] = {0};	

    if(number == NULL)
    {
        LOGI("Null info");
        ret = -1;
        return ret;		
    }
    switch(calltype)
    {
        case CCALL_TYPE:
            fd = open("../usrdata/ccall_num", O_RDWR);
            if(fd == -1)	
            {
                 LOGI ("ccall_num File open failed.....");
                 ret = -1;	                 				 
            }
            else
            {
                readfn = read(fd,buf,sizeof(buf));
                LOGI ("CCALL contents(%d) is ... %s",readfn, buf);
                strncpy(number,buf,readfn);
                ret = 0;				
            }
            break;	
			
        case BCALL_TYPE:
            fd = open("../usrdata/bcall_num", O_RDWR);
            if(fd == -1)	
            {
                 LOGI ("bcall_num File open failed.....");
                 ret = -1;	                 				 
            }
            else
            {
                readfn = read(fd,buf,sizeof(buf));
                LOGI ("BCALL contents(%d) is ... %s",readfn, buf);
                strncpy(number,buf,readfn);
                ret = 0;				
            }
            break;			
			
        case ECALL_TYPE:
            fd = open("../usrdata/ecall_num", O_RDWR);
            if(fd == -1)	
            {
                 LOGI ("ecall_num File open failed.....");
                 ret = -1;	                 				 
            }
            else
            {
                readfn = read(fd,buf,sizeof(buf));
                LOGI ("ECALL contents(%d) is ... %s",readfn, buf);
                strncpy(number,buf,readfn);
                ret = 0;				
            }
            break;			

        case ACALL_TYPE:
            fd = open("../usrdata/acall_num", O_RDWR);
            if(fd == -1)	
            {
                 LOGI ("acall_num File open failed.....");
                 ret = -1;	                 				 
            }
            else
            {
                readfn = read(fd,buf,sizeof(buf));
                LOGI ("AALL contents(%d) is ... %s",readfn, buf);
                strncpy(number,buf,readfn);
                ret = 0;				
            }
            break;			

        case NCALL_TYPE:
            fd = open("../usrdata/ncall_num", O_RDWR);
            if(fd == -1)	
            {
                 LOGI ("ncall_num File open failed.....");
                 ret = -1;	                 				 
            }
            else
            {
                readfn = read(fd,buf,sizeof(buf));
                LOGI ("NCALL contents(%d) is ... %s",readfn, buf);
                strncpy(number,buf,readfn);
                ret = 0;				
            }
            break;
       default:
	     break;
    }

    close(fd);
	
    return ret;	
}

//int voice_origination(int calltype)
int voice_origination(char* orig_number)
{

    int    ret     = E_QL_OK;
    //char save_num[20] = {0};	
    //char orig_number[20] = {0};

#if 0	 
    if(get_call_number(save_num, calltype) < 0 )
    {        
        LOGI("No Dial Number Info File");	
        ret = -1;
        return ret;
    }
    else
    {
        strncpy(orig_number,save_num,get_string_count(save_num));    
        LOGI("Dial Number is %s",orig_number);		
    }
#endif		
   if(orig_number == NULL)
   {
      LOGI("No Dial Number Info File");	
       ret = -1;
       return ret;
   }
	
    if(voice_handler_allocated == FALSE)
    {
        ret = QL_Voice_Call_Client_Init(&h_voice);
    
        if(ret != 0 && ret != 1)
        {
            LOGI("QL_Voice_Call_Client_Init FAIL.%d\n");
            return -1;
        }
        LOGI("QL_Voice_Call_Client_Init ret = %d, with h_voice=%d\n", ret, h_voice);
    	
        ret = QL_Voice_Call_AddStateHandler(h_voice, nad_voice_call_cb_func, &voice_call_id);
        if(ret != 0 && ret != 1)
        {
            LOGI("QL_Voice_Call_AddStateHandler FAIL.%d\n");
            return -1;
        }
        LOGI("QL_Voice_Call_AddStateHandler ret = %d\n", ret);	
    
        voice_handler_allocated = TRUE;
    	
    }
		
    //btn_calltype = 	calltype;	
    //ret = QL_Voice_Call_Start(h_voice, E_QL_VCALL_EXTERNAL_SLOT_1, dial_number, &voice_call_id);
    ret = QL_Voice_Call_Start(h_voice, E_QL_VCALL_EXTERNAL_SLOT_1, orig_number, &voice_call_id);
    LOGI("QL_Voice_Call_Start ret = %d, with voice_call_id=%d\n", ret, voice_call_id);
    
 
    return ret;

}
int voice_call_accept()
{
    int    ret     = E_QL_OK;
    byte call_state = 0;

    if(voice_handler_allocated == FALSE)
    {
        LOGI("No voice call handler");
        return -1;		
    }	

    ret = QL_Voice_Call_Answer(h_voice, voice_call_id);  
    LOGI("Answer Incomming Call ret = %d",ret);  	
    return ret;

}
int voice_call_end()
{

    int    ret     = E_QL_OK;
    byte call_state = 0;

    if(voice_handler_allocated == FALSE)
    {
        LOGI("Voice handler already released and de-init");
        return -1;		
    }	
	
    ret = QL_Voice_Call_End(h_voice, voice_call_id);
    LOGI(" ret = %d\n", ret);
    return ret;
}



