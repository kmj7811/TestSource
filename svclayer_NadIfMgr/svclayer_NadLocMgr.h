/*===================================================

                  TCU Location Mgr Header


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

#ifndef __TCU_LOCATION_H
#define __TCU_LOCATION_H

typedef struct 
{
    QL_LOC_POS_MODE_INFO_T  t_mode;
    int bitmask;	
    int min_interval;  //report nmea frequency 0.5Hz
    int preferred_accuracy;    // <50m
    int preferred_timet_mode;    // 90s

	
}di_gps_mode_t;

int startTcuLocationMgr();

#endif //__TCU_LOCATION_H


