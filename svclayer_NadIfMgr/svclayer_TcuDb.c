/*===================================================

                  TCU Local Data DB C


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
#include "svclayer_TcuDb.h"
#include "common.h"

// Global Var. for Local Database
tcu_db_items_value_type tcu_local_db_items_value;


void tcu_db_init()
{

  memset(tcu_local_db_items_value.vin, 0, MAX_STRING_LENGTH);
  memset(tcu_local_db_items_value.ver, 0, MAX_STRING_LENGTH);
  memset(tcu_local_db_items_value.iccid, 0, MAX_STRING_LENGTH);
  memset(tcu_local_db_items_value.dir, 0, 20);
  memset(tcu_local_db_items_value.imei, 0, 20);
  tcu_local_db_items_value.sim_state = 0;
  
  tcu_local_db_items_value.rat = 0xff;
  tcu_local_db_items_value.plmn.mcc = 0;
  tcu_local_db_items_value.plmn.mnc = 0;

  tcu_local_db_items_value.lac = 0;
  tcu_local_db_items_value.reg_state = 0;
  tcu_local_db_items_value.cs_state = 0;
  tcu_local_db_items_value.ps_state = 0;
  tcu_local_db_items_value.rssi = 0;

}


void tcu_db_put(tcu_db_items_type item, tcu_db_items_u *value_ptr)
{
    if(item > TCU_DB_ITEM_MAX)
    {
        LOGI("ERR, NO DB ITEM");
        return;			
    }

    switch(item)
    {
        case TCU_DB_VIN:
            memcpy(tcu_local_db_items_value.vin, value_ptr->vin, MAX_STRING_LENGTH);			
            break;			
        case TCU_DB_VER:
             memcpy(tcu_local_db_items_value.ver, value_ptr->ver, MAX_STRING_LENGTH);			
           break;			
        case TCU_DB_ICCID:
             memcpy(tcu_local_db_items_value.iccid, value_ptr->iccid, MAX_STRING_LENGTH);			
            break;			
        case TCU_DB_DIR:
             memcpy(tcu_local_db_items_value.dir, value_ptr->dir, 20);			
            break;			
       case TCU_DB_IMEI:
             memcpy(tcu_local_db_items_value.imei, value_ptr->imei, 20);			
            break;			
       case TCU_DB_SIM_STATE:
             tcu_local_db_items_value.sim_state =  value_ptr->sim_state;			
            break;			
        case TCU_DB_RAT:
            tcu_local_db_items_value.rat = value_ptr->rat;			
            break;			
        case TCU_DB_PLMN:
             tcu_local_db_items_value.plmn.mcc =  value_ptr->plmn.mcc;			
             tcu_local_db_items_value.plmn.mnc =  value_ptr->plmn.mnc;			
             break;	


        case TCU_DB_LAC:
            tcu_local_db_items_value.lac = value_ptr->lac ;
            break;			
        case TCU_DB_REG_STATE:
            tcu_local_db_items_value.reg_state = value_ptr->reg_state;	
            break;
        case TCU_DB_CS_STATE:
            tcu_local_db_items_value.cs_state = value_ptr->cs_state;	
            break;
        case TCU_DB_PS_STATE:
            tcu_local_db_items_value.ps_state = value_ptr->ps_state;	
            break;
        case TCU_DB_RSSI:
            tcu_local_db_items_value.rssi = value_ptr->rssi;	
            break;
			
        default:
             LOGI("Bad item parameter %d ",item);
            break;

    }

}


void tcu_db_get(tcu_db_items_type item, tcu_db_items_u *value_ptr)
{

    if(item > TCU_DB_ITEM_MAX)
    {
        LOGI("ERR, NO DB ITEM");
        return;			
    }

    switch(item)
    {
        case TCU_DB_VIN:
            memcpy(value_ptr->vin, tcu_local_db_items_value.vin, MAX_STRING_LENGTH);			
            break;			
        case TCU_DB_VER:
             memcpy(value_ptr->ver, tcu_local_db_items_value.ver, MAX_STRING_LENGTH);			
           break;			
        case TCU_DB_ICCID:
             memcpy(value_ptr->iccid, tcu_local_db_items_value.iccid, MAX_STRING_LENGTH);			
            break;			
        case TCU_DB_DIR:
             memcpy(value_ptr->dir, tcu_local_db_items_value.dir, 20);			
            break;			
       case TCU_DB_IMEI:
             memcpy(value_ptr->imei, tcu_local_db_items_value.imei, 20);			
            break;			
       case TCU_DB_SIM_STATE:
             value_ptr->sim_state = tcu_local_db_items_value.sim_state;			
            break;			


        case TCU_DB_RAT:
            value_ptr->rat = tcu_local_db_items_value.rat ;			
            break;			
        case TCU_DB_PLMN:
             value_ptr->plmn.mcc =  tcu_local_db_items_value.plmn.mcc;			
             value_ptr->plmn.mnc =  tcu_local_db_items_value.plmn.mnc;			
             break;	
			 
        case TCU_DB_LAC:
            value_ptr->lac = tcu_local_db_items_value.lac ;
            break;			
        case TCU_DB_REG_STATE:
            value_ptr->reg_state = tcu_local_db_items_value.reg_state ;	
            break;
        case TCU_DB_CS_STATE:
            value_ptr->cs_state = tcu_local_db_items_value.cs_state ;	
            break;
        case TCU_DB_PS_STATE:
            value_ptr->ps_state = tcu_local_db_items_value.ps_state ;	
            break;
        case TCU_DB_RSSI:
            value_ptr->rssi = tcu_local_db_items_value.rssi ;	
            break;
		
			
        default:
             LOGI("Bad item parameter %d ",item);
            break;

    }


}


