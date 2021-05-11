/*===================================================

                  TCU Location Mgr C


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
#include <ql_oe.h>
#include "svclayer_NadLocMgr.h"
#include "tcu_interface.h"
#include "common.h"
#include "msg_if.h"
#include <sys/timeb.h>
#include <time.h>



int h_loc = 0;
di_gps_mode_t  di_gps_mode;
BOOL GpsActiveFlag = TRUE;

gps_info_indi_t gps_info;

#if 1
static char* get_timestamp(char* buf) {
    struct timeb start;
    char append[100];
    if (buf) {
        ftime(&start);
        strftime(buf, 100, "%H:%M:%S", localtime(&start.time));

        /* append milliseconds */
        sprintf(append, ":%03u", start.millitm);
        strcat(buf, append);
    }
    return buf;
}
#endif

static void ql_loc_rx_ind_msg_cb(loc_client_handle_type  h_loc,
                                 E_QL_LOC_NFY_MSG_ID_T   e_msg_id,
                                 void                    *pv_data,
                                 void                    *contextPtr)
{
    //LOGI("e_msg_id=%d", e_msg_id); 
#if 0
typedef enum 
{
    E_QL_LOC_LOCATION_LAT_LONG_VALID   = 0x0001, /**<  GPS location has valid latitude and longitude.  */
    E_QL_LOC_LOCATION_ALTITUDE_VALID   = 0x0002, /**<  GPS location has a valid altitude.  */
    E_QL_LOC_LOCATION_SPEED_VALID      = 0x0004, /**<  GPS location has a valid speed.  */
    E_QL_LOC_LOCATION_BEARING_VALID    = 0x0008, /**<  GPS location has a valid bearing.  */
    E_QL_LOC_LOCATION_ACCURACY_VALID   = 0x0010, /**<  GPS location has valid accuracy.  */
    E_QL_LOC_LOCATION_SOURCE_INFO_VALID= 0x0020, /**<  GPS location has valid source information.  */
    E_QL_LOC_LOCATION_IS_INDOOR_VALID  = 0x0040, /**<  GPS location has a valid "is indoor?" flag.  */
    E_QL_LOC_LOCATION_FLOOR_NUMBE_VALID= 0x0080, /**<  GPS location has a valid floor number.  */
    E_QL_LOC_LOCATION_MAP_URL_VALID    = 0x0100, /**<  GPS location has a valid map URL.  */
    E_QL_LOC_LOCATION_MAP_INDEX_VALID  = 0x0200, /**<  GPS location has a valid map index.  */
}E_QL_LOC_LOCATION_VALID_FLAG;	
#####Location timestamp info: timestamp=1538614189000(10:04:2018, 00:49)
 **** flag=0x3F, Latitude = 37.388825, Longitude=126.997925, accuracy = 3.216000  Altitude = 86.748840 Head = 100.599998 Speed = 14.620000 ****	
flag --> 3F (11 1111)  
#endif
    gps_info.gpsFix = 0; // Init gpsFix status every callback
    
    switch(e_msg_id)
    {
        case E_QL_LOC_NFY_MSG_ID_STATUS_INFO:
        {
            char *status[]  = {"NONE",  "SESSION_BEGIN",  "SESSION_END",  "ENGINE_ON",  "ENGINE_OFF"};
            E_QL_LOC_STATUS_VALUE_T e_status = *((E_QL_LOC_STATUS_VALUE_T*)pv_data);
            LOGI("GPS status=%s", status[e_status]);  
            break;			
        }

        case E_QL_LOC_NFY_MSG_ID_LOCATION_INFO:
        {
            struct tm *utc;
            int64_t timestamp_s;
           		
            QL_LOC_LOCATION_INFO_T *pt_location = (QL_LOC_LOCATION_INFO_T *)pv_data;
            #if 0				
            LOGI("**** flag=0x%X, Latitude = %f, Longitude=%f, accuracy = %f  Altitude = %f Head = %f Speed = %f ****", 
                        pt_location->flags, 
                        pt_location->latitude, 
                        pt_location->longitude, 
                        pt_location->accuracy,
                        pt_location->altitude,
                        pt_location->bearing,
                        pt_location->speed);
            #endif
			
			if(pt_location->flags >= 0x3)
				gps_info.gpsFix = 1;   //Location Information valid
                     else
				gps_info.gpsFix = 0;
					 
			gps_info.longitude = pt_location->longitude;
			gps_info.latitude = pt_location->latitude;
			gps_info.gpsType = 0;
			gps_info.altitude = pt_location->altitude;
			gps_info.heading = pt_location->bearing;
			gps_info.gpsSpeed = pt_location->speed;

            #if 0				
            timestamp_s = pt_location->timestamp/1000;  // ms --> s
            utc = gmtime((time_t*)&timestamp_s);
            printf("#####Location timestamp info: timestamp=%lld(%02d:%02d:%04d, %02d:%02d)\n", 
                    pt_location->timestamp, 
                    utc->tm_mon + 1, 
                    utc->tm_mday, 
                    utc->tm_year + 1900, 
                    utc->tm_hour,
                    utc->tm_min);   
            #endif			
            break;
        }
        case E_QL_LOC_NFY_MSG_ID_SV_INFO:
        {
            int num_sv = 0;
            int i = 0;			
            QL_LOC_SV_STATUS_T  *sv_data = (QL_LOC_SV_STATUS_T  *)pv_data;
            num_sv = sv_data->num_svs;
            for( i=0; i<num_sv; i++)
            {
                //LOGI("SNR[%d] = %f",i,sv_data->sv_list[i].snr);
            }
            break;
        }
        case E_QL_LOC_NFY_MSG_ID_NMEA_INFO:			
        {
            struct tm *utc;
            int64_t timestamp_s;
            QL_LOC_NMEA_INFO_T  *pt_nmea = (QL_LOC_NMEA_INFO_T  *)pv_data;
            timestamp_s = pt_nmea->timestamp/1000;  // ms --> s
            utc = gmtime((time_t*)&timestamp_s);
            //printf("NMEA info: timestamp=%lld(%02d:%02d:%04d, %02d:%02d), length=%d, nmea=%s\n", 
            //        pt_nmea->timestamp, utc->tm_mon + 1, utc->tm_mday, utc->tm_year + 1900, utc->tm_hour,
            //        utc->tm_min, pt_nmea->length, pt_nmea->nmea); 
            break;
        }
        case E_QL_LOC_NFY_MSG_ID_CAPABILITIES_INFO:
            break;
        case E_QL_LOC_NFY_MSG_ID_AGPS_STATUS:
            break;
        case E_QL_LOC_NFY_MSG_ID_NI_NOTIFICATION:
            break;
        case E_QL_LOC_NFY_MSG_ID_XTRA_REPORT_SERVER:
            break;
    }
}

int setTcuLocationPositionMode(int bitmask, int mode, int recurrence, int interval, int accuracy, int preferred_time)
{
    di_gps_mode.t_mode.mode =mode ;
    di_gps_mode.bitmask = bitmask;
    di_gps_mode.t_mode.recurrence =  recurrence;
    di_gps_mode.min_interval = interval;  //report nmea frequency 1Hz
    di_gps_mode.preferred_accuracy =  accuracy;    // <50m
    di_gps_mode.preferred_timet_mode = preferred_time;    // 90s
}



void initDefaultTcuPositionMode()
{
    di_gps_mode.bitmask = 511; //All bit mask  (NiNfy|AGPS|XTRA|UTC|CAP|NMEA|SV|Status|Location)
    di_gps_mode.t_mode.mode = E_QL_LOC_POS_MODE_STANDALONE;
    di_gps_mode.t_mode.recurrence =  E_QL_LOC_POS_RECURRENCE_PERIODIC;
    di_gps_mode.min_interval = 1000;  //report nmea frequency 0.5Hz
    di_gps_mode.preferred_accuracy =  50;    // <50m
    di_gps_mode.preferred_timet_mode = 90;    // 90s
}

int initTcuLocationMgr()
{
    int  ret = E_QL_OK;
    int  bitmask     = 0;
    QL_LOC_POS_MODE_INFO_T  t_mode      = {0};
    QL_LOC_LOCATION_INFO_T  t_loc_info  = {0};
    int   timeout_sec = 60;


    initDefaultTcuPositionMode();	
	
    ret = QL_LOC_Client_Init(&h_loc);
    LOGI("QL_LOC_Client_Init ret %d with h_loc=%d", ret, h_loc);
    if(ret != E_QL_OK)
        return ret;		

    ret = QL_LOC_AddRxIndMsgHandler(ql_loc_rx_ind_msg_cb, (void*)h_loc);
    LOGI("QL_LOC_AddRxIndMsgHandler ret %d", ret); 
    if(ret != E_QL_OK)
        return ret;	
	
}

void get_poisition_once(void)
{
    int  ret = E_QL_OK;
    int  bitmask     = 0;	
    QL_LOC_POS_MODE_INFO_T  t_mode      = {0};
    QL_LOC_LOCATION_INFO_T  t_loc_info  = {0};
    int  timeout_sec = 60;
	
    LOGI("============get_poisition_once");     

    bitmask = 1; //force set to 1 to get location only.
    ret = QL_LOC_Set_Indications(h_loc, bitmask);
    if(ret != E_QL_OK)
    {
        LOGI("Fail QL_LOC_Set_Indications ret %d", ret);     
        return ;
    }

    t_mode.mode                 = E_QL_LOC_POS_MODE_STANDALONE;
    t_mode.recurrence           = E_QL_LOC_POS_RECURRENCE_SINGLE;
    t_mode.min_interval         = 1000;  //report nmea frequency 1Hz
    t_mode.preferred_accuracy   = 50;    // <50m
    t_mode.preferred_time       = 90;    // 90s
    ret = QL_LOC_Set_Position_Mode(h_loc, &t_mode);
    LOGI("QL_LOC_Set_Position_Mode ret %d", ret);
    
    ret = QL_LOC_Get_Current_Location(h_loc, &t_loc_info, timeout_sec);
    LOGI(" QL_LOC_Get_Current_Location ret %d", ret);
    if(ret < 0)
    {
        if(ret == -2)
        {// -2: timeout, may need try again
            LOGI("QL_LOC_Get_Current_Location timeout, try again!");
        }
        else
        {
            LOGI("QL_LOC_Get_Current_Location Fail, ret %d", ret);
        }
    }
    else
    {
        LOGI("**** Latitude = %lf, Longitude=%lf, altitude=%lf, accuracy = %f ****", 
                t_loc_info.latitude, t_loc_info.longitude, t_loc_info.altitude, t_loc_info.accuracy);
    }
    return;	
	
}

BOOL getGpsActiveFlag()
{
    return GpsActiveFlag;
}
void setGpsActiveFlag(BOOL flag)
{
    GpsActiveFlag = flag;
}

int startNavigation()
{
    int  ret = E_QL_OK;
    QL_LOC_POS_MODE_INFO_T  t_mode      = {0};	
    LOGI("===============startNavigation===============");

    /* Set what we want callbacks for */
    ret = QL_LOC_Set_Indications(h_loc, di_gps_mode.bitmask);
    if(ret != E_QL_OK)
    {
        LOGI("Fail QL_LOC_Set_Indications ret %d", ret);     
        return ret;
    }

    t_mode.mode                 = di_gps_mode.t_mode.mode;
    t_mode.recurrence           = di_gps_mode.t_mode.recurrence;
    t_mode.min_interval         = di_gps_mode.min_interval;  
    t_mode.preferred_accuracy   = di_gps_mode.preferred_accuracy;    
    t_mode.preferred_time       = di_gps_mode.preferred_timet_mode;    
	
    ret = QL_LOC_Set_Position_Mode(h_loc, &t_mode);	
    if(ret != E_QL_OK)
    {
        LOGI("Fail QL_LOC_Set_Position_Mode ret %d", ret);     
        return ret;
    }
	
    ret = QL_LOC_Start_Navigation(h_loc); 
    if(ret != E_QL_OK)
    {
        LOGI("Fail QL_LOC_Start_Navigation ret %d", ret);     
        return ret;
    }

    setGpsActiveFlag(TRUE);	
	
}

int stopNavigation()
{
    int  ret = E_QL_OK;
    ret = QL_LOC_Stop_Navigation(h_loc);
    if(ret != E_QL_OK)
    {
        LOGI("Fail stopNavigation ret %d", ret);     
        return ret;
    }
    setGpsActiveFlag(FALSE);	
}


void show_location_info()
{

	printf("---LOCATION INFORMATION---\n");
	printf("GFix: %d\n", gps_info.gpsFix);
	printf("gTyp: %d\n", gps_info.gpsType);
	printf("long: %lf\n", gps_info.longitude);
	printf("lati: %lf\n", gps_info.latitude);
	printf("alti: %d\n", gps_info.altitude);
	printf("head: %d\n", gps_info.heading);
	printf("Sped: %d\n", gps_info.gpsSpeed);
}
void get_locationInfo()
{
    char tmp[10];
    LOGI("Time %s",get_timestamp(tmp));
    show_location_info();
    send_gps_info_indi(gps_info);
}

int startTcuLocationMgr()
{
    int  ret  = E_QL_OK;
    
    LOGI("===============startTcuLocationMgr===============");

    ret = initTcuLocationMgr();

    if(getGpsActiveFlag())
    {
        ret = startNavigation();
        if(ret != E_QL_OK)
        {
            LOGI("Fail stopNavigation ret %d", ret);     
            return ret;
        }		
    }
    else
    {
    	;
    }
    return ret;	
}


