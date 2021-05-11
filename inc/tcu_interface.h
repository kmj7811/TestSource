/*===================================================

                  TCU INTERFACE H


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

#ifndef __TCU_INTERFACE_H
#define __TCU_INTERFACE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <syslog.h>



#define TCU_PLATFORM_MUTEX_INIT(mutext_ptr) \
      pthread_mutex_init(mutext_ptr,NULL)
#define TCU_PLATFORM_MUTEX_INIT_RECURSIVE(mutex_ptr) \
  do \
  { \
      pthread_mutexattr_t _attr; \
      pthread_mutexattr_init (&_attr); \
      pthread_mutexattr_settype (&_attr, PTHREAD_MUTEX_RECURSIVE_NP); \
      pthread_mutex_init(mutex_ptr, &_attr); \
      pthread_mutexattr_destroy (&_attr); \
  } while (0)


#define TCU_PLATFORM_MUTEX_DESTROY(mutex_ptr) \
  pthread_mutex_destroy (mutex_ptr)

#define TCU_PLATFORM_MUTEX_LOCK(mutex_ptr) \
  pthread_mutex_lock (mutex_ptr)

#define TCU_PLATFORM_MUTEX_TRY_LOCK(mutex_ptr) \
  pthread_mutex_trylock (mutex_ptr)

#define TCU_PLATFORM_MUTEX_UNLOCK(mutex_ptr) \
  pthread_mutex_unlock (mutex_ptr)




//Msg Q Key Vaule

#define FRAME2APP_MSGQ_KEY					0x4000
#define APP2FRAME_MSGQ_KEY					0x4001

#define FRAME2MCUIF_MSGQ_KEY					0x4002
#define MCUIF2FRAME_MSGQ_KEY					0x4003

#define FRAME2AVNIF_MSGQ_KEY					0x4004
#define AVNIF2FRAME_MSGQ_KEY					0x4005

//Msg Q data type
#define FRAME_TO_APP_TYPE						0x4000
#define APP_TO_FRAME_TYPE						0x4001

#define FRAME_TO_MCUIFMGR_TYPE				0x4002
#define MCUIFMGR_TO_FRAME_TYPE				0x4003

#define FRAME_TO_AVNIFMGR_TYPE						0x4004
#define AVNIFMGR_TO_FRAME_TYPE						0x4005



enum {
	APPTCUCOM_MSG_STATE_NONE = 0,
	APPTCUCOM_MSG_STATE_TX,
	APPTCUCOM_MSG_STATE_PENDING,
	APPTCUCOM_MSG_STATE_MAX
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////--TCU APP --/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TCU App <--> TCU main

//========================================
// REMOTE CONTROL RELATED MESSAGE
//========================================

#define REMOTE_CONTROL_TO_FRAME			0x00
#define REMOTE_CONTROL_TO_APP				0x01

typedef enum {
    FIND_LOCATION_REQ = 0,
    DOOR_OPEN_REQ,
    DOOR_LOCK_REQ,
    TRUNK_OPNE_REQ,
    ENGINE_START_REQ,
    ENGINE_STOP_REQ, /* 5 */
    HAV_CONTROL_REQ,
    LIGHT_HORN_REQ,
    VEHICLE_STATUS_CHECK_REQ,
    REMOTE_ID_MAX /* MAX */
}tcu_app_remote_cmd_type;

#define FIND_LOCATION_RSP					0
#define DOOR_OPEN_RSP						1
#define DOOR_LOCK_RSP						2
#define TRUNK_OPNE_RSP						3
#define ENGINE_START_RSP					4
#define ENGINE_STOP_RSP					5
#define HAV_ON_RSP							6
#define HAV_OFF_RSP							7
#define LIGHT_HORN_RSP						8
#define VEHICLE_STATUS_CHECK_RSP			9



//========================================
// SAFETY RELATED MESSAGE
//========================================
#define SAFETY_AND_CALL_SRV_TO_FRAME			0x02
#define SAFETY_AND_CALL_SRV_TO_APP			0x03


typedef enum {
    SOS_CALL_INDI_ACK = 0,
    AIRBACK_E_CALL_INDI_ACK,
    CALL_EST_INDI_ACK,
    CALL_END_INDI_ACK,
    VEHICLE_TRACKING_START_REQ,
    VEHICLE_TRACKING_STOP_REQ, /* 5 */
    VEHICLE_TRACKING_RESULT_ACK,
    BRG_ALAM_INDI_ACK,
    DTC_RESULT_CODE_ACK,
    DIAGNOSIS_ANALYSIS_INFO,
    SCHEDULED_CHECK_REQ, /*10 */
    SUPPLIES_ITEM_NOTI,    
    CS_NOTI,
    CS_CALL_INDI_ACK,
    CALL_DIAL_REQ,    
    POI_INFO, /* 15 */
    SFY_AND_CALL_ID_MAX /* MAX */
}tcu_app_satety_cmd_type;


#define SOS_CALL_INDI						0
#define AIRBACK_CALL_INDI					1
#define CALL_EST_IND						2
#define CALL_END_IND						3
#define VEHICLE_TRACKING_START_ACK		4
#define VEHICLE_TRACKING_STOP_ACK			5
#define VEHICLE_TRACKING_RESULT_INFO		6
#define BRG_ALAM_INDI_						7
#define DTC_RESULT_CODE					8

#define DIAGNOSIS_ANALYSIS_INFO_ACK		9
#define SCHEDULED_CHECK_REQ_ACK			10
#define SUPPLIES_ITEM_NOTI_ACK				11
#define CS_NOTI_ACK							12
#define CONCIERGE_CALL_INDI				13
#define CALL_DIAL_REQ_ACK					14
#define POI_INFO_ACK						15


//========================================
// INFORTAINMENT AND CONVENIENCE RELATED MESSAGE
//========================================
#define SUPPLIMENTARY_SRV_TO_FRAME		0x04
#define SUPPLIMENTARY_SRV_TO_APP			0x05

typedef enum {
    FRIEND_LOCATION_SHARING_INFO = 0,
    INFOTAIN_ID_MAX /* MAX */
}tcu_app_infortain_cmd_type;


#define FRIEND_LOCATION_SHARING_INFO_ACK0	0

//========================================
// Common Info
//========================================
#define COMMON_INFO_TO_FRAME        			0x06
#define COMMON_INFO_TO_APP          			0x07
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////--MCU--////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MCU < -> MCU Mgr
#define SOFRAME								0xA5
#define EOFRAME								0x5A

//AVN Mgr < -> AVN
#define SOFRAME_AVN						0x7F
#define EOFRAME_AVN						0x7F


// Command ID List
#define ENGINE_START						0x01
#define ENGINE_STOP							0x02
#define HAVC_STOP							0x03
#define DOOR_LOCK							0x04
#define DOOR_UNLOCK						0x05
#define TRUNK_UNLOCK						0x06
#define LIGHT_AND_HORN						0x07
#define VEHICLE_STATUS_CHECK				0x08

#define EXT_CALL								0x31
#define BURGLAR_ALAM						0x32

#define NAD_INIT							0x71
#define NAD_INIT_CHECK						0x72

#define DTC_RECOMMAND_NOTI				0x81
#define KEY_STATUS							0x82
#define DTC_CODE_READ						0x83

#define ENGINE_STATUS_REPORT				0x82
#define AUTO_DIAG_ACTIVATE				0x83

#define TRANSFER_DATA						0xB4
#define END_OF_DATA						0xB5
#define REQUEST_CRC							0xB6
#define REQUEST_FLASH_ERASE				0xB7
#define REQUEST_FLASH_PROGRAM			0xB8


#define DIAG_REQ							0xC1



//Call Type
//#define SOS_CALL_BTN_INDI					0x00
//#define AIRBACK_COLLIDE_INDI				0x01
//#define CONCIERGE_CALL_BTN_INDI			0x02

#define E_CALL_BTN_INDI						0x00
#define B_CALL_BTN_INDI						0x01
#define C_CALL_BTN_INDI						0x02


//MCU IF Mgr --> TCU main
typedef enum {
    E_CALL_BTN_INDI_TO_TCU = 0,
    B_CALL_BTN_INDI_TO_TCU,
    C_CALL_BTN_INDI_TO_TCU,
    A_CALL_BTN_INDI_TO_TCU,
    MCU_ID_MAX /* MAX */
}mcuiftcu_cmd_type;


//TCU main ..> MCU IF Mgr
typedef enum {
    CALL_RESULT_INDI_TO_MCU = 0,
    CALL_END_INDI_TO_MCU,
    MCUIF_ID_MAX /* MAX */
}tcumcuif_cmd_type;


#define CALL_STATUS_SUCESS						0
#define CALL_STATUS_LINE_BUSY					1
#define CALL_STATUS_VOICE_HANDLER_FAIL		2
#define CALL_ORIG_FAIL							3


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////--AVN--/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//CALL FAILURE REASON
#define NORAML_END_REASON 		0x00
#define LINE_BUSY_REASON			0x01
#define MODME_ERR_REASON			0x02
#define UNDIFINED_REASON			0xFF


//RAT INFO
#define RAT_LTE		0x00
#define RAT_WCDMA	0x01
#define RAT_GSM		0x02


//AVN <--> AVN Mgr

//AVN Mgr <--> TCU main

#if 0
0x01: PROVISIONING PIN
0xA0: NAD CALL STATUS
0xA1: NAD SERVICE STATUS
0xA2: NAD STATIC INFO REQ
0xA3: NAD RF INFO REQ
0xD0: SUBSCRIPTON START
0xD1: SUBSCRIPTION STOP

#endif

//AVNIF to TCU Cmd

typedef enum {

    AVNIF_CALL_ORIG_CMD =0x00,
    AVNIF_CALL_END_CMD,

    AVNIF_NAD_CALL_STATUS = 0xC0,
    AVNIF_NAD_SERVICE_STATUS,
    AVNIF_NAD_STATIC_INFO_REQ,
    AVNIF_NAD_RF_INFO_REQ,
    
    AVNIF_MANUAL_DIAG_QUERY,
    AVNIF_PROVISIONING_PIN,
    AVNIF_SUBSCRIPTION_START,
    AVNIF_SUBSCRIPTION_STOP,
    AVNIF_TCU_ID_MAX /* MAX */
}avniftcu_cmd_type;

//TCU to AVNIF Cmd
typedef enum {
    CALL_CONNECTING_INDI,
    CALL_CONNECTED_INDI,
    CALL_FAILFURE_INDI,
    CALL_END_INDI,
    
    NAD_CALL_STATE_INDI, 
    NAD_SERVICE_STATUS_INDI, 
    NAD_STATIC_INFO_INDI, 
    NAD_RF_INFO_INDI,
    
    MANUAL_DIAG_QUERY,
    PROVISIONING_PIN,
    SUBSCRIPTION_START,
    SUBSCRIPTION_STOP,
    
    DTC_INFORMATION,
    DIAG_ANALYSIS_INFO,
    SCHEDULED_DIAGNOSIS,
    ASCODE_INFORMATION,
    EXTEND_ASCODE_INFORMATION, 
    LOCATION_SHARING_INFO, 
    POI_INFORMATION, 
    PROVISIONING_RESULT, 
    SEURITY_AND_PROVISIONING, 
    SUBSCRIPTION_CANCEL, 
    SUBSCRIPTION_CLOSE, 
    SUBSCRIPTION_HOLD, 
    SUBSCRIPTION_HOLD_RELEASE, 
    ERROR_INFORMATION, 
    GENERAL_INFORMATION,  
    SUBSCRIPTION_RESULT, 
    TCUAVNIF_ID_MAX /* MAX */
}tcuavnif_cmd_type;


//AVN to AVNIF Cmd
typedef enum {
    AVN_CALL_ORIG_CMD =0x00,
    AVN_CALL_END_CMD,

    AVN_NAD_CALL_STATUS = 0xC0,
    AVN_NAD_SERVICE_STATUS,
    AVN_NAD_STATIC_INFO_REQ,
    AVN_NAD_RF_INFO_REQ,
    
    AVN_ID_MAX /* MAX */
}avn_cmd_type;


#endif //__TCU_INTERFACE_H



