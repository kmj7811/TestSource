/*===================================================

                  TCU Local Data Base Header


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
#include <ql_oe.h>

#ifndef __TCU_DB_H
#define __TCU_DB_H

#define MAX_STRING_LENGTH	30

typedef enum {
  TCU_DB_VIN = 0,
  TCU_DB_VER,
  TCU_DB_ICCID,
  TCU_DB_DIR,
  TCU_DB_IMEI,
  TCU_DB_SIM_STATE,

  TCU_DB_RAT,
  TCU_DB_PLMN,
  TCU_DB_LAC,
  TCU_DB_REG_STATE,
  TCU_DB_CS_STATE,
  TCU_DB_PS_STATE,
  
  TCU_DB_RSSI,
  TCU_DB_ITEM_MAX              = 999 // LAST Item.

//////////////////////////////////////////
} tcu_db_items_type;

typedef struct
{
  int mcc;
  int mnc;
}lplmn_identity_type;

typedef struct {
  char vin[MAX_STRING_LENGTH];
  char ver[MAX_STRING_LENGTH];
  char iccid[MAX_STRING_LENGTH];
  char dir[20];
  char imei[20];
  byte sim_state;

  byte rat;
  lplmn_identity_type plmn; 
  int lac;
  byte reg_state;
  byte cs_state;
  byte ps_state;

  int rssi;


} tcu_db_items_value_type;

typedef struct {
  char vin[MAX_STRING_LENGTH];
  char ver[MAX_STRING_LENGTH];
  char iccid[MAX_STRING_LENGTH];
  char dir[20];
  char imei[20];
  byte sim_state;

  byte rat;
  lplmn_identity_type plmn;

  int lac;
  byte reg_state;
  byte cs_state;
  byte ps_state;

  int rssi;

} tcu_db_items_u;



extern void tcu_db_init(void);
extern void tcu_db_put(tcu_db_items_type item, tcu_db_items_u *value_ptr);
extern void tcu_db_get(tcu_db_items_type item, tcu_db_items_u *value_ptr);

#endif //__TCU_DB_H


