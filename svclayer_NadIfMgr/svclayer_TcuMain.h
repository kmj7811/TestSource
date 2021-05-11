/*===================================================

                  CCU MAIN HEADER


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

#ifndef __CCU_MAIN_H
#define __CCU_MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>

 
#define FEATURE_CONCIERGE_MO_CALL

void LOCK_TCU_PLATFORM();
void UNLOCK_TCU_PLATFORM();
int start_tcu_main();

#endif //__CCU_MAIN_H

