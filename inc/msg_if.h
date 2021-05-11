/*===================================================

                  MESSAGE INTERFACE HEADER


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

#ifndef __MSG_IF_H
#define __MSG_IF_H

#include "common.h"

#define SPI_DATA_BUFF	(0xFFu)
#define MCUMGR_MSG_MAX	(4u)

#define MCUMGR_START_FLAG           (0xA5u)
#define MCUMGR_END_FLAG             (0x5Au)
#define MCUMGR_HEADER_LEN           (2u)
#define MCUMGR_TAIL_LEN             (2u)

#define MCUMGR_START_FLAG_POS       (0u)
#define MCUMGR_DLC_POS              (1u)
#define MCUMGR_DATA_POS             (2u)
#define MCUMGR_DATA_SID_POS         (2u)

#define MCUMGR_ID_POS               (0u)

#define MCUMGR_GET_LEN(p)           (p)[MCUMGR_DLC_POS]
#define MCUMGR_GET_DATA_LEN(p)      ( ((p)[MCUMGR_DLC_POS] - MCUMGR_HEADER_LEN) - MCUMGR_TAIL_LEN )

typedef struct 
{
    double latitude;
    double longitude;	
    int	altitude;
    int 	heading;
    int	gpsSpeed;
    int 	gpsTime;
    int	satellites;
    int	Hdop;
    int	Vdop;
    int	gpsFix;
    int	gspSts;		
}gps_data_t;

//////////////////////////////////////////////////////////////////////
///////////// TCU < ------ > TCU APP //////////////////////////////////
//////////////////////////////////////////////////////////////////////

// TCU App --- > TCU Main message structure
typedef struct 
{
    int interval;
    int duration;	
}svt_start_req_t;

typedef struct 
{
    int result_code;	
}svt_result_ack_t;

typedef struct 
{
    int targetIdx;
    char targetTime[30];
    char name[30];
    double latitude;
    double longitude;	
}poi_info_t;

//TCU Main --- > TCU App message structure

typedef struct 
{
    gps_data_t gps_data;
}svt_result_info_t;

typedef struct 
{
    gps_data_t gps_data;
}sos_call_indi_info_t;

typedef struct 
{
    gps_data_t gps_data;
}airback_call_indi_info_t;

typedef struct 
{
    gps_data_t gps_data;
}concierge_call_indi_info_t;

//TCU App <-> TCU Main Que structure
typedef struct 
{
    long  data_type;
    int    length;	
    unsigned short   groupId;
    unsigned short  cmdId;	
    union {
        // TCU App --- > TCU Main		
        svt_start_req_t svt_req;
        svt_result_ack_t result;
        poi_info_t poi_info_data;		
        //nad_interface_info_s nad_if;
        //avn_interface_info_s avn_if;	

        //TCU Main --- > TCU App
        svt_result_info_t svt_result;
        sos_call_indi_info_t sos_call_indi_data;		
        airback_call_indi_info_t ariback_call_indi_data;		
        concierge_call_indi_info_t concierge_call_indi_data;	
		
    } data;
	
}tcuApp_cmdq_t;



//////////////////////////////////////////////////////////////////////
///////////// TCU < ------ > MCU IF mgr   // ////////////////////////////
//////////////////////////////////////////////////////////////////////





// MCU If Mgr -> TCU Data structure

typedef struct 
{
  byte call_type;	
}call_type_t;


// TCU -> MCU If mgr Data structure
typedef struct 
{
  byte call_status;	
}call_type_status_t;



//MCU IF mgr  <-> TCU Main Que structure
typedef struct 
{
    long  data_type;
    unsigned short  cmdId;
    int    length;	
    union {
          // MCU IF mgr --- > TCU Main
		
           // TCU Main --- > MCU IF mgr	
           call_type_status_t csts;

    } data;
	
}mcu_cmdq_t;

// MCU IF mgr <---> MCU
typedef struct {
	byte sof; //start of frame;
	byte length;  // data length 
	byte cmdId;   //command ID
       union {
        // MCU --- > MCU IF mgr	   	
           call_type_t ct;		
		
        // MCU IF mgr  --- > MCU	
           call_type_status_t csts;
		
       }data;
       byte cs;	 //check sum
	byte eof; //end of frame

}mcu_data_t;

typedef void (*fpMcuMgrNotifyType)(byte *ipU8Data, byte iU8Cnt);

typedef struct {
    byte u8Id;
    byte u8Status;
    fpMcuMgrNotifyType stNotifyTx;
    fpMcuMgrNotifyType stNotifyRx;
} stMcuMgrMsgType;

enum {
    MCUMGR_MSG_ID_01_ENG_START = 0,
	MCUMGR_MSG_ID_02_ENG_STOP,
	MCUMGR_MSG_ID_03_HAVC_STOP,
	MCUMGR_MSG_ID_04_DOOR_LOCK,
	MCUMGR_MSG_ID_05_DOOR_UNLOCK,
	MCUMGR_MSG_ID_06_TRUNK_UNLOCK, /* 5 */
	MCUMGR_MSG_ID_07_LIGHT_AND_HORN,
	MCUMGR_MSG_ID_08_VEH_STS_CHECK,
	MCUMGR_MSG_ID_31_CALL,
	MCUMGR_MSG_ID_32_BALARM,
	MCUMGR_MSG_ID_71_INIT,
	MCUMGR_MSG_ID_72_INIT_CHECK, /* 10 */
	MCUMGR_MSG_ID_81_DTC_RECOMMAND_NOTI,
	MCUMGR_MSG_ID_82_KEY_STATUS_NOTI,
	MCUMGR_MSG_ID_83_DTC_CODE_READ,
	MCUMGR_MSG_ID_C1_READ_DID,
	MCUMGR_MSG_ID_C2_WRITE_DID, /* 15 */
	MCUMGR_MSG_ID_C3_ROUTINE_CTRL,
	MCUMGR_MSG_ID_C4_IO_CTRL,
	MCUMGR_MSG_ID_MAX /* MAX */
};

enum {
	MCUMGR_MSG_STATE_NONE = 0,
	MCUMGR_MSG_STATE_TX,
	MCUMGR_MSG_STATE_PENDING,
	MCUMGR_MSG_STATE_MAX
};
//////////////////////////////////////////////////////////////////////
///////////// TCU < ------ > AVN mgr   // ////////////////////////////
//////////////////////////////////////////////////////////////////////

//AVN mgr  <-> TCU Main Que structure

typedef struct 
{
  byte call_type;	
}call_connecting_t;

typedef struct 
{
  byte call_type;	
}call_connected_t;

typedef struct 
{
  byte call_type;	
}call_origination_t;

typedef struct 
{
  byte reason;	
}call_failure_reason_t;

typedef struct 
{
  byte network_type;	
}nad_rf_info_t;

typedef struct 
{
    long  data_type;
    unsigned short  cmdId;
    int    length;	
    union {
            // AVN IF mgr --- > TCU Main
            call_origination_t origin_indi;
 			
             // TCU Main --- > AVN IF mgr
            call_connecting_t connecting_indi;
            call_connected_t  connected_indi;			
            call_failure_reason_t fail_reason;		
            nad_rf_info_t rf_info;	
			
            poi_info_t   poi;
    } data;
	
}avn_cmdq_t;




typedef struct {
	byte sof; //start of frame;
	byte cmdIdH;   //command ID
	byte cmdIdL;
	byte lenth_high;  // data length high
	byte lenth_low;   // data_length_low
       union {
        // AVN --- > AVN Mgr	   	
        call_origination_t origin_indi;
        // AVN Mgr --- > AVN	
        call_connecting_t calling_indi;
        call_failure_reason_t fail_reason;		
        nad_rf_info_t rf_info;		
        byte no_data_eof; //for no data payload
       }data;
	byte eof; //end of frame

}avn_data_t;


////////////////////////////////////////////////////////////////////////

////JUST FOR TEST /////////////
#define NAD2IF_MSGQ_KEY					0x5000
#define IF2NAD_MSGQ_KEY					0x5001

//Msg Q data type
#define NAD_TO_IF_TYPE						0x5000
#define IF_TO_NAD_TYPE						0x5001


////////////////////////////////////////////
#define MAX_DIR		20
////////////////////////////////////////////
#define ECALL_TYPE    0
#define BCALL_TYPE    1
#define CCALL_TYPE    2
#define NCALL_TYPE    0xff

#define ECALL_BTN   0
#define BCALL_BTN   1
#define CCALL_BTN   2
#define NULL_BTN    0xFF
////////////////////////////////////////////

typedef struct
{
  byte call_type;
}start_call_type_t;
typedef struct
{
  byte call_status;
  byte number[MAX_DIR];
}call_status_t;
typedef struct
{
  byte fail_reason;
}call_failure_t;
typedef struct
{
  byte status;
}nw_status_t;
typedef struct
{
  byte collide;
}collide_info_t;

/*
LE_POS_STATE_NO_FIX = 0,        ///< The position is not fixed.
LE_POS_STATE_FIX_2D = 1,        ///< 2-Dimensional position fix.
LE_POS_STATE_FIX_3D = 2,        ///< 3-Dimensional position fix.
LE_POS_STATE_FIX_ESTIMATED = 3,   ///< Estimated (i.e. forward predicted) position fix.
LE_POS_STATE_UNKNOWN = 4        ///< Unknown state.
*/


typedef struct
{
  double latitude;
  double longitude;
  int altitude;
  int heading;
  int gpsSpeed;
  int gpsFix;
  byte gpsType;  //0: WGS84, 1: BESSEL
}gps_info_indi_t;

///////////////////////////////////////////
#define IMEI_MAX  30
#define ICCID_MAX 30
#define DIR_MAX   20
#define SWVER_MAX 40
typedef struct
{
  byte imei[IMEI_MAX];
  byte iccid[ICCID_MAX];
  byte dir[DIR_MAX];
  byte swVer[SWVER_MAX];
}nad_info_indi_t;

#define MCC_MNC_MAX 8
typedef struct
{
  byte rat;
  byte reg_state;
  byte mcc_mnc[MCC_MNC_MAX];
  int lac;
  int tac;
  int cellid;
  int signal_value;
}nad_nw_info_indi_t;

///////////////////////////////////////////
typedef struct
{
	long  data_type;
  byte  group;
	byte  cmd;
	union {
		start_call_type_t start_call;
		call_status_t call_status;
		call_failure_t call_failure;
		nw_status_t nw_status;
		collide_info_t collide_info;
		gps_info_indi_t gps_info;

    /////////////////////////////
    nad_info_indi_t nad_info;
    nad_nw_info_indi_t nw_info;

	}data;

}msg_t;
///////////////////////////////


//NAD NGR --- > IF MGR/////
typedef enum {
  START_CALL_REQ =0x00,
  CALL_STATUS_INDI,
  CALL_END_INDICATION,
  CALL_FAILURE_INDI,

  DATA_NW_STATUS_INDI = 0x30,

  GPS_INFO_INDI = 0x60,

  APP_NAD_CALL_STATUS = 0xC0,

  COLLIDE_INFO_INDI = 0xF0,

  ID_MAX /* MAX */
}cmd_type;


///////////////////////////////////////////////////////////


//IF MGR ---> NAD MGR
#define CALL_START_REQ        0x00
#define CALL_ACCEPT_REQ       0x01
#define CALL_END_REQ          0x02

#define DATA_NW_STATUS_REQ    0x30

#define GPS_INFO_REQ          0x60
#define COLLIDE_INFO_REQ      0x70
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
#define CALL_STATUS_INCOMMING			0x01
#define CALL_STATUS_ORIGIN				0x02
#define CALL_STATUS_RING				  0x03
#define CALL_STATUS_CONNECTED			0x04
#define CALL_STATUS_TERMINATED		0x05
/////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//Common data and Others
#define NAD_INFO_REQ          0x00
#define NAD_NW_INFO_REQ       0x01

//NAD NGR --- > IF MGR/////
typedef enum {
  NAD_INFO_IND =0x00,
  NAD_NW_INFO_IND,
    CC_ID_MAX /* MAX */
}cc_cmd_type;

///////////////////////////////////////////////////////////




#endif //__MSG_IF_H


